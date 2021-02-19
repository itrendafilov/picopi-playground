/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <string.h>
#include "pico/stdlib.h"
#include "pico/sync.h"

#include "usb.h"
#include "app.h"

/* Data buffers */
volatile char* messageRxPointer = NULL;

semaphore_t semRxReady;

void APP_usb_setup() {
    tusb_init();
    sem_init(&semRxReady, 0, 1);
}

void APP_usb_send_message(char* message)
{
//    tud_hid_report(0, message, strlen(message));   // ToDo: Fix this!!!
}

bool USBHID0_waitForFlightCommand(char* ptr) {
    // wait for tud_hid_set_report_cb to fill in the buffer and detect command end char
}

void APP_usb_loop() {
    uint8_t i;

    char command[RX_STR_LENGTH] = "";
    char* endTransmission = "\r\n!\r\n";
    USBHID0_waitForFlightCommand(command);

    messageRxPointer = command;

    for (;;) {
        tud_task(); // tinyusb device task
        if (sem_acquire_timeout_ms(&semRxReady, 0)) {
            break;
        }
    }

    // Echo back the characters received
    APP_usb_send_message(command); // ToDo: remove me?

    for (i = 0; i < APP_USB_COMMAND_COUNT; i++) {
        size_t cmdStrLen = strlen(APP_usb_commands[i].command);
        if (0 == strncmp(APP_usb_commands[i].command, command, cmdStrLen)
                && command[cmdStrLen] == ' ') {

            APP_usb_commands[i].callback(&command[cmdStrLen + 1]);
            APP_usb_send_message(endTransmission);
            return;
        }
    }

    // If no command matches then send error (jumped over on match)
    // Prepare the outgoing string
    strcpy(command,"\r\nNo such command\r\n!\r\n");

    // Send the response over USB
    APP_usb_send_message(command);
}


//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {
}

// Invoked when device is unmounted
void tud_umount_cb(void) {
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) {
    (void) remote_wakeup_en;
}

// Invoked when usb bus is resumed
void tud_resume_cb(void) {
}

//--------------------------------------------------------------------+
// USB HID
//--------------------------------------------------------------------+

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    // TODO not Implemented
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
    // This example doesn't use multiple report and report ID
    (void) report_id;
    (void) report_type;
    uint8_t len = strlen(buffer);
    strncat((char*)messageRxPointer, buffer+1, len);
    for (int i = len; i > 0; i--, messageRxPointer++) {
        if (*messageRxPointer == '!') {
            *messageRxPointer = '\0';
            sem_release(&semRxReady);
        }
    }
}
