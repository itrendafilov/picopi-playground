/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#ifndef SRC_APP_H_
#define SRC_APP_H_

#include <stdint.h>

void APP_systemBoot();

void APP_systemInit();

void APP_loop();


// Callback methods on user input message
// example format
// void my_user_command(char* params);

typedef void (*USBHID_Callback_Handler)(char*);

typedef struct {
    char* command;
    USBHID_Callback_Handler callback;
} USBHID_User_Command;

#define APP_USB_COMMAND_COUNT 5
extern const USBHID_User_Command APP_usb_commands[APP_USB_COMMAND_COUNT];

#endif /* SRC_APP_H_ */
