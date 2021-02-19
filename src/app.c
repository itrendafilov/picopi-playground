/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <string.h>

/* App includes */
#include "app.h"
#include "config/board.h"
#include "flight/instrument.h"
#include "displays/displays.h"
#include "leds/leds.h"
#include "usb/usb.h"

/*
 * System bootstrap
 */
void APP_systemBoot() {
    board_init();

#ifdef Board_LCD_ILI9486
    // Initialize driver
    ili9486_DriverInit();
    // Set Graphics Library context
    Graphics_initContext(&g_sContext, &g_ili9486_Driver);
#endif /* Board_LCD_ILI9341 */

}

/*
 * System bootstrapped. Now initialize long-running processes
 */
void APP_systemInit() {
    APP_led1_enable();

    APP_instruments_init();

    APP_usb_setup();

    APP_led1_disable();

//    printf("Finished initialization.");

    APP_led1_start(1000);
}

/*
 * Main application loop
 */
void APP_loop() {
    APP_usb_loop();
}

void APP_usb_cmd_led(char* params) {
    // Compare to string #1, and respond
    if (!(strcmp(params, "ON"))){

        // Turn off timer; no longer toggling LED
        APP_led1_stop();

        // Turn on LED P1.0
        APP_led1_enable();

        // Send the response over USB
        APP_usb_send_message("\r\nLED is ON");

    // Compare to string #2, and respond
    } else if (!(strcmp(params, "OFF"))){

        // Turn off timer; no longer toggling LED
        APP_led1_stop();

        // Turn off LED P1.0
        APP_led1_disable();

        // Send the response over USB
        APP_usb_send_message("\r\nLED is OFF");

    // Compare to string #3, and respond
    } else if (!(strcmp(params, "TOGGLE - SLOW"))){

        // Turn off timer while changing toggle period
        APP_led1_stop();

        // Set timer period for slow LED toggle (1000 ms)
        APP_led1_start(1000);

        // Send the response over USB
        APP_usb_send_message("\r\nLED is toggling slowly");

    // Compare to string #4, and respond
    } else if (!(strcmp(params,"TOGGLE - FAST"))){

        // Turn off timer while changing toggle period
        APP_led1_stop();

        // Set timer period for fast LED toggle (250 ms)
        APP_led1_start(250);

        // Send the response over USB
        APP_usb_send_message("\r\nLED is toggling fast");

    // Handle other
    } else {
        // Send the response over USB
        APP_usb_send_message("\r\nNo such command");
    }
}

int16_t read_intn(char* str, uint8_t num) {
    char* strEnd = str + num;
    int16_t ret = 0;
    bool negative = *str == '-';
    if (negative) str++;

    while (str < strEnd) {
        // return 0 if not digits
        if (*str < '0' || *str > '9') {
            return 0;
        }

        ret*=10;
        ret+=*str - '0';
        str++;
    }
    if (negative) ret *= -1;
    return ret;
}

int16_t read_int(char* str) {
    int16_t ret = 0;
    bool negative = *str == '-';
    if (negative) str++;

    while (*str != '\0') {
        // return 0 if not digits
        if (*str < '0' || *str > '9') {
            return 0;
        }

        ret*=10;
        ret+=*str - '0';
        str++;
    }
    if (negative) ret *= -1;
    return ret;
}

void APP_usb_cmd_param(char* params) {
    // Compare to string #1, and respond
    if (!(strncmp(params, "ALTITUDE ", 9))){
        uint16_t value = read_int(params + 9);
        APP_altispeed_update(APP_INSTRUMENT_ALTISPEED_ALTITUDE, value);
    } else if (!(strncmp(params, "BARO ", 5))){
        uint16_t value = read_int(params + 5);
        APP_altispeed_update(APP_INSTRUMENT_ALTISPEED_BARO, value);
    } else if (!(strncmp(params, "KIAS ", 5))){
        uint16_t value = read_int(params + 5);
        APP_altispeed_update(APP_INSTRUMENT_ALTISPEED_KIAS, value);
    } else if (!(strncmp(params, "PITCH ", 6))){
        float value = (float)read_int(params + 6) / 10.0;
        APP_horizon_update(APP_INSTRUMENT_HORIZON_PITCH, value);
    } else if (!(strncmp(params, "BANK ", 5))){
        float value = (float)read_int(params + 5) / 10.0;
        APP_horizon_update(APP_INSTRUMENT_HORIZON_BANK, value);
    } else {
        // Send the response over USB
        APP_usb_send_message("\r\nNo such command");
    }
}

void APP_usb_cmd_radio_com1(char* params) {
    // Compare to string #1, and respond
    if (!(strcmp(params, "ACTIVATE"))){
        APP_radio_panel_com1_activate();
    } else if (!(strcmp(params,"TOGGLE"))){
        APP_radio_panel_com1_toggle();
    } else if (!(strcmp(params, "FREQ - INC"))) {
        APP_radio_panel_com1_inc();
    } else if (!(strcmp(params, "FREQ - DEC"))) {
        APP_radio_panel_com1_dec();
    } else if (!(strncmp(params, "SPACE ",6))) {
        APP_radio_panel_com1_set_spacing((read_intn(params + 6, 2)));
    } else if (!(strncmp(params, "SET PRIMARY ",12))){
        // ToDo: better input handling here
        uint8_t natural = read_intn(params + 12, 3);
        uint16_t fraction = read_intn(params + 16, 3);
        APP_radio_panel_com1_set_primary(natural, fraction);
    } else if (!(strncmp(params, "SET STANDBY ",12))){
        // ToDo: better input handling here
        uint8_t natural = read_intn(params + 12, 3);
        uint16_t fraction = read_intn(params + 16, 3);
        APP_radio_panel_com1_set_standby(natural, fraction);
    } else {
        // Send the response over USB
        APP_usb_send_message("\r\nNo such command");
    }
}

void APP_usb_cmd_horizon(char* params) {
    // ToDo: better input handling here
    char* space = strchr(params, ' ');
    int16_t attackAngle = read_intn(params, space - params);
    space++;
    int16_t rollAngle = read_intn(space, strlen(space));
    APP_horizon_update((float)attackAngle/10.0, (float)rollAngle/10.0);
}

void APP_usb_cmd_instrument(char* params) {
    // Compare to string #1, then #2, and so on
    if (!(strcmp(params, "HORIZON"))){
       APP_instruments_change(APP_INSTRUMENT_HORIZON);

    } else if (!(strcmp(params, "RADIO"))){
        APP_instruments_change(APP_INSTRUMENT_RADIO);

    } else if (!(strcmp(params, "ALTISPEED"))){
        APP_instruments_change(APP_INSTRUMENT_ALTITUDE_SPEED);

    } else if (!(strcmp(params, "BLANK"))){
        APP_instruments_change(APP_INSTRUMENT_BLANK_SCREEN);

    } else {
       // Send the response over USB
       APP_usb_send_message("\r\nNo such command");
    }
}

const USBHID_User_Command APP_usb_commands[APP_USB_COMMAND_COUNT] = {
    {
         .command = "LED",
         .callback = &APP_usb_cmd_led
    },
    {
         .command = "PARAM",
         .callback = &APP_usb_cmd_param
    },
    {
         .command = "RADIO COM1",
         .callback = &APP_usb_cmd_radio_com1
    },
    {
         .command = "HORIZON",
         .callback = &APP_usb_cmd_horizon
    },
    {
         .command = "INSTRUMENT",
         .callback = &APP_usb_cmd_instrument
    }
};
