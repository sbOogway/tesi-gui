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
    fprintf(stdout, "%d.%d.%d-%s\n", LVGL_VERSION_MAJOR, LVGL_VERSION_MINOR, LVGL_VERSION_PATCH, LVGL_VERSION_INFO);
}

/**
 * @brief Print usage information
 */
static void print_usage(void)
{
    fprintf(stdout, "\nlvglsim [-V] [-B] [-b backend_name] [-W window_width] [-H window_height]\n\n");
    fprintf(stdout, "-V print LVGL version\n");
    fprintf(stdout, "-B list supported backends\n");
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

static float target_temperature = 15.0;
lv_obj_t * screen;
lv_obj_t * target_temperature_label;

const int padding_button = 50;
const int height_button  = 50;
const int width_button   = 50;

static void increment_temperature(lv_event_t * e)
{
    if(lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    target_temperature++;
    fprintf(stdout, "debug callback -> %.1f\n", target_temperature);
    lv_label_set_text_fmt(target_temperature_label, "%.1f°C", target_temperature);
    
}

static void decrement_temperature(lv_event_t * e)
{
    if(lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }
    target_temperature--;
    fprintf(stdout, "debug callback -> %.1f\n", target_temperature);
    lv_label_set_text_fmt(target_temperature_label, "%.1f°C", target_temperature);
    
}

/**
 * @brief entry point
 * @description start a demo
 * @param argc the count of arguments in argv
 * @param argv The arguments
 */
int main(int argc, char ** argv)
{


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


    lv_display_t* disp = lv_linux_fbdev_create();
    lv_linux_fbdev_set_file(disp, "/dev/fb0");
    // lv_linux_fbdev_set_force_refresh(true);

    screen = lv_scr_act();

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

    target_temperature_label = lv_label_create(screen);
    lv_label_set_text_fmt(target_temperature_label, "%.1f°C", target_temperature);
    lv_obj_set_style_text_font(target_temperature_label, &lv_font_montserrat_48, 0);
    lv_obj_align(target_temperature_label, LV_ALIGN_CENTER, 0, 0);

    /* Enter the run loop of the selected backend */
    driver_backends_run_loop();

    return 0;
}
