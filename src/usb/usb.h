/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */
#ifndef SRC_USB_H_
#define SRC_USB_H_

#define MAX_STR_LENGTH  64
#define RX_STR_LENGTH   (2 * MAX_STR_LENGTH)

#include <stdbool.h>

#include <bsp/board.h>
#include "tusb.h"

void APP_usb_setup();
void APP_usb_loop();

void APP_usb_send_message(char* message);

unsigned int USBHID0_receiveData(unsigned char *pStr,
                                 unsigned int length,
                                 unsigned int timeout);
unsigned int USBHID0_sendData(const unsigned char *pStr,
                              unsigned int length,
                              unsigned int timeout);
bool USBHID0_waitForConnect(unsigned int timeout);

#endif /* SRC_USB_H_ */
