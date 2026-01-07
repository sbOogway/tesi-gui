/*******************************************************************
 *
 * main.c - LVGL simulator for GNU/Linux
 *
 * Based on the original file from the repository
 *
 * @note eventually this file won't contain a main function and will
 * become a library supporting all major operating systems
 *
 * To see how each driver is initialized check the
 * 'src/lib/display_backends' directory
 *
 * - Clean up
 * - Support for multiple backends at once
 *   2025 EDGEMTech Ltd.
 *
 * Author: EDGEMTech Ltd, Erik Tagirov (erik.tagirov@edgemtech.ch)
 *
 ******************************************************************/
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <execinfo.h>

#include <common-control/common-control.h>

#include "lvgl/lvgl.h"
#include "lvgl/demos/lv_demos.h"

#include "src/lib/driver_backends.h"
#include "src/lib/simulator_util.h"
#include "src/lib/simulator_settings.h"

/* Internal functions */
static void configure_simulator(int argc, char ** argv);
static void print_lvgl_version(void);
static void print_usage(void);

/* contains the name of the selected backend if user
 * has specified one on the command line */
static char * selected_backend;

/* Global simulator settings, defined in lv_linux_backend.c */
extern simulator_settings_t settings;

/**
 * @brief Print LVGL version
 */
static void print_lvgl_version(void)
{
    LOG_INFO("%d.%d.%d-%s\n", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH, LVGL_VERSION_INFO);
}

/**
 * @brief Print usage information
 */
static void print_usage(void)
{
    LOG_INFO("\nlvglsim [-V] [-B] [-b backend_name] [-W window_width] [-H window_height]\n\n");
    LOG_INFO("-V print LVGL version\n");
    LOG_INFO("-B list supported backends\n");
}

/**
 * @brief Configure simulator
 * @description process arguments recieved by the program to select
 * appropriate options
 * @param argc the count of arguments in argv
 * @param argv The arguments
 */
static void configure_simulator(int argc, char ** argv)
{
    int opt = 0;

    selected_backend = NULL;
    driver_backends_register();

    const char * env_w = getenv("LV_SIM_WINDOW_WIDTH");
    const char * env_h = getenv("LV_SIM_WINDOW_HEIGHT");
    /* Default values */
    settings.window_width  = atoi(env_w ? env_w : "800");
    settings.window_height = atoi(env_h ? env_h : "480");

    /* Parse the command-line options. */
    while((opt = getopt(argc, argv, "b:fmW:H:BVh")) != -1) {
        switch(opt) {
            case 'h':
                print_usage();
                exit(EXIT_SUCCESS);
                break;
            case 'V':
                print_lvgl_version();
                exit(EXIT_SUCCESS);
                break;
            case 'B':
                driver_backends_print_supported();
                exit(EXIT_SUCCESS);
                break;
            case 'b':
                if(driver_backends_is_supported(optarg) == 0) {
                    die("error no such backend: %s\n", optarg);
                }
                selected_backend = strdup(optarg);
                break;
            case 'W': settings.window_width = atoi(optarg); break;
            case 'H': settings.window_height = atoi(optarg); break;
            case ':':
                print_usage();
                die("Option -%c requires an argument.\n", optopt);
                break;
            case '?': print_usage(); die("Unknown option -%c.\n", optopt);
        }
    }
}

void handler_sigill_debug(int sig)
{
    // // Use async-signal-safe functions only
    // const char * msg = "SIGILL (Illegal Instruction) received - attempting backtrace\n";
    // write(STDERR_FILENO, msg, strlen(msg));

    // // Try to get backtrace if available (may not be fully async-signal-safe)
    // // but useful for debugging
    // void * array[10];
    // size_t size = backtrace(array, 10);
    // if(size > 0) {
    //     const char * bt_msg = "Backtrace:\n";
    //     write(STDERR_FILENO, bt_msg, strlen(bt_msg));
    //     backtrace_symbols_fd(array, size, STDERR_FILENO);
    // } else {
    //     const char * no_bt_msg = "No backtrace available\n";
    //     write(STDERR_FILENO, no_bt_msg, strlen(no_bt_msg));
    // }

    // // Use _exit instead of exit for signal handlers (no cleanup, safer)
    // _exit(1);
}

pid_t pid_control_pid;

int sensors_count;

char * sensors_format_buffer;
int max_buffer_size;

const char * target_temperature_format  = "Target T:  %.1f°C";
const char * current_temperature_format = "Current T: %.1f°C";
const char * temperature_format         = "%1.f°C";

static float target_temperature;

lv_obj_t * screen;
lv_obj_t * target_temperature_label;
lv_obj_t * current_temperature_label;

const int padding_button = 50;
const int height_button  = 50;
const int width_button   = 50;

void set_target_temperature(float t)
{
    LOG_DEBUG("debug callback -> %.1f\n", target_temperature);
    target_temperature += t;
    lv_label_set_text_fmt(target_temperature_label, target_temperature_format, target_temperature);
    write_float_to_file(TARGET_TEMPERATURE_FILE, target_temperature);
    kill(pid_control_pid, SIGUSR1);
}

static void increment_temperature(lv_event_t * e)
{
    if(lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    set_target_temperature(1);
}

static void decrement_temperature(lv_event_t * e)
{
    if(lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    set_target_temperature(-1);
}

void update_current_temperature()
{
    target_temperature = get_float_from_file(TARGET_TEMPERATURE_FILE);
    lv_label_set_text_fmt(target_temperature_label, target_temperature_format, target_temperature);
    
    sensors_format_buffer[0] = '\0';

    for(int i = 0; i < sensors_count; i++) {
        char buf[64];
        char temp_buf[32];
        snprintf(buf, sizeof(buf), "%s/s%d", CURRENT_TEMPERATURE_FILE, i);
        float sensor_temp = get_float_from_file(buf);

        LOG_DEBUG("%.1f", sensor_temp);

        snprintf(temp_buf, sizeof(temp_buf), "s%d %.1f°C", i, sensor_temp);

        strncat(sensors_format_buffer, temp_buf, max_buffer_size - strlen(sensors_format_buffer) - 1);
        LOG_DEBUG(sensors_format_buffer);

        if(i < sensors_count - 1) {
            strncat(sensors_format_buffer, " ", max_buffer_size - strlen(sensors_format_buffer) - 1);
        }
    }

    lv_label_set_text(current_temperature_label, sensors_format_buffer);

    LOG_DEBUG("updated current temperature label succesfully");
}

/**
 * @brief entry point
 * @description start a demo
 * @param argc the count of arguments in argv
 * @param argv The arguments
 */
int main(int argc, char ** argv)
{
    log_init();
    log_set_level(LOG_DEBUG);
    log_set_output(LOG_OUTPUT_CONSOLE);

    /**
     * now we sleep until the pid-control process finishes initialization
     * of the 1-wire bus.
     */
    sigset_t set, old_set;
    int sig;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, &old_set);
    LOG_INFO("Waiting synchronously for SIGUSR1...");
    sigwait(&set, &sig); // Blocks until SIGUSR1 is received
    LOG_INFO("Received signal %d", sig);

    system("pgrep " PID_CONTROL_PROCESS_NAME " > " PID_CONTROL_PID_FILE);
    pid_control_pid = get_pid_from_file(PID_CONTROL_PID_FILE);
    LOG_INFO("PID control file found -> %d ...\n", pid_control_pid);

    // write_pid_to_file(TEMP_CONTROL_PID_FILE);

    sensors_count = get_int_from_file(NUMBER_OF_SENSORS_FILE);
    LOG_INFO("found file with sensor number. sensors count -> %d", sensors_count);

    max_buffer_size       = sensors_count * 32 + 1;
    sensors_format_buffer = malloc(max_buffer_size);

    struct sigaction sa;
    sa.sa_handler = update_current_temperature;
    sigaction(SIGUSR1, &sa, NULL);

    configure_simulator(argc, argv);

    /* Initialize LVGL. */
    lv_init();

    /* Initialize the configured backend */
    if(driver_backends_init_backend(selected_backend) == -1) {
        die("Failed to initialize display backend");
    }

    /* Enable for EVDEV support */
#if LV_USE_EVDEV
    if(driver_backends_init_backend("EVDEV") == -1) {
        die("Failed to initialize evdev");
    }
#endif

#if LV_USE_LINUX_FBDEV
    lv_display_t * disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, "/dev/fb0");
#endif

    target_temperature = get_float_from_file(TARGET_TEMPERATURE_FILE);
    screen             = lv_scr_act();

    lv_obj_t * temps_container = lv_obj_create(screen);
    lv_obj_set_width(temps_container, lv_pct(100));
    lv_obj_set_height(temps_container, lv_pct(100));
    lv_obj_set_flex_flow(temps_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(temps_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t * increment_temperature_button = lv_btn_create(screen);
    lv_obj_align(increment_temperature_button, LV_ALIGN_BOTTOM_RIGHT, -padding_button, -padding_button);
    lv_obj_set_height(increment_temperature_button, height_button);
    lv_obj_set_width(increment_temperature_button, width_button);
    lv_obj_add_event_cb(increment_temperature_button, increment_temperature, LV_EVENT_ALL, NULL);

    lv_obj_t * increment_temperature_label = lv_label_create(increment_temperature_button);
    lv_label_set_text(increment_temperature_label, "+");
    lv_obj_set_style_text_font(increment_temperature_label, &lv_font_montserrat_48, 0);
    lv_obj_center(increment_temperature_label);

    lv_obj_t * decrement_temperature_button = lv_btn_create(screen);
    lv_obj_align(decrement_temperature_button, LV_ALIGN_BOTTOM_LEFT, padding_button, -padding_button);
    lv_obj_set_height(decrement_temperature_button, height_button);
    lv_obj_set_width(decrement_temperature_button, width_button);
    lv_obj_add_event_cb(decrement_temperature_button, decrement_temperature, LV_EVENT_ALL, NULL);

    lv_obj_t * decrement_temperature_label = lv_label_create(decrement_temperature_button);
    lv_label_set_text(decrement_temperature_label, "-");
    lv_obj_set_style_text_font(decrement_temperature_label, &lv_font_montserrat_48, 0);
    lv_obj_center(decrement_temperature_label);

    target_temperature_label = lv_label_create(temps_container);
    lv_label_set_text_fmt(target_temperature_label, target_temperature_format, target_temperature);
    lv_obj_set_style_text_font(target_temperature_label, &lv_font_montserrat_48, 0);
    lv_obj_align(target_temperature_label, LV_ALIGN_CENTER, 0, 0);

    current_temperature_label = lv_label_create(temps_container);
    // lv_label_set_text_fmt(current_temperature_label, current_temperature_format, current_temperature);
    update_current_temperature();
    lv_obj_set_style_text_font(current_temperature_label, &lv_font_montserrat_48, 0);
    lv_obj_align(current_temperature_label, LV_ALIGN_CENTER, 0, 0);

    /*
    we reset the signal mask to original one after ui elements initialization
    to avoid
    */
    sigprocmask(SIG_SETMASK, &old_set, NULL);

    // Register SIGILL handler with sigaction for better reliability
    struct sigaction sa_sigill;
    sigemptyset(&sa_sigill.sa_mask);
    sa_sigill.sa_handler = handler_sigill_debug;
    sa_sigill.sa_flags   = SA_RESETHAND; // Reset handler to default after first call
    sigaction(SIGILL, &sa_sigill, NULL);

    /* Enter the run loop of the selected backend */
    driver_backends_run_loop();

    log_cleanup();

    free(sensors_format_buffer);
    return 0;
}
