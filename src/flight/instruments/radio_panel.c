/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include <string.h>

#include <grlib.h>
#include "displays/displays.h"
#include "usb/usb.h"

#include <speedmath.h>
#include "flight/instrument.h"
// See https://en.wikipedia.org/wiki/Airband
//#define BAND_NAV_START 108
//#define BAND_NAV_END   117.95
//#define BAND_COM_START 118.0
//#define BAND_COM_END   136.975

typedef struct {
    uint8_t natural;
    uint16_t fraction;
} RadioFrequency;

typedef struct {
    uint8_t spacing;
    RadioFrequency bandFreqStart;
    RadioFrequency bandFreqEnd;
} RadioBand;

typedef struct {
    RadioBand band;
    RadioFrequency primary;
    RadioFrequency standby;
} RadioChannel;

RadioChannel radio_com1 = {
    .band = {
             .spacing = 25,  // Older radios are 50 kHz, newer are 25 kHz
             .bandFreqStart = { 118, 000 },
             .bandFreqEnd = { 136, 975 }
    },
    .primary = { 122, 100 },
    .standby = { 122, 100 }
};
bool radio_com1_rotate_high = false;

void decade_to_text(char* out, int16_t decade) {
    out[0] = '0' + modBy10(divBy100(decade) % 10);
    out[1] = '0' + modBy10(divBy10(decade) % 10);
    out[2] = '0' + modBy10(decade);
}
void freq_to_text(char* buffer, RadioFrequency value) {
    decade_to_text(&buffer[0], value.natural);
    decade_to_text(&buffer[4], value.fraction);
    buffer[3] = '.';
    buffer[7] = '\0';
}

void APP_radio_panel_draw_freq(char* text, int x, int y) {

    Graphics_setFont(&g_sContext, &g_sFontCmss18b);
    Graphics_drawString(&g_sContext, (int8_t*)text, 7, x, y, true);

    Graphics_flushBuffer(&g_sContext);
}

void APP_radio_panel_refresh() { // ToDo: optimize refresh of single value only
    char text[8];

    APP_usb_send_message("\r\nRADIO_Update");

    // Draw COM1
    freq_to_text(text, radio_com1.primary);
    APP_radio_panel_draw_freq(text, 0, 0);
    APP_usb_send_message("\r\nCOM1 primary: ");
    APP_usb_send_message(text);

    freq_to_text(text, radio_com1.standby);
    APP_radio_panel_draw_freq(text, 64, 0);
    APP_usb_send_message("\r\nCOM1 standby: ");
    APP_usb_send_message(text);
}

void APP_radio_panel_com1_activate() {
    uint8_t tempN = radio_com1.primary.natural;
    uint16_t tempF = radio_com1.primary.fraction;
    radio_com1.primary.natural = radio_com1.standby.natural;
    radio_com1.primary.fraction = radio_com1.standby.fraction;
    radio_com1.standby.natural = tempN;
    radio_com1.standby.fraction = tempF;

    APP_instruments_refresh();
}

void APP_radio_panel_com1_set_spacing(uint8_t spacing) {
    APP_usb_send_message("\r\nRADIO_Update");
    if (spacing == 50) {
        radio_com1.band.spacing = 50;
        APP_usb_send_message("\r\nSpacing: 50");
    } else {
        radio_com1.band.spacing = 25;
        APP_usb_send_message("\r\nSpacing: 25");
    }
}

void APP_radio_panel_com1_set_primary(uint8_t freq_natural, uint16_t freq_fraction) {
    radio_com1.primary.natural = freq_natural;
    // round to channel spacing
    radio_com1.primary.fraction = (freq_fraction/radio_com1.band.spacing)*radio_com1.band.spacing;

    APP_instruments_refresh();
}

void APP_radio_panel_com1_set_standby(uint8_t freq_natural, uint16_t freq_fraction) {
    radio_com1.standby.natural = freq_natural;
    // round to channel spacing
    radio_com1.standby.fraction = (freq_fraction/radio_com1.band.spacing)*radio_com1.band.spacing;

    APP_instruments_refresh();
}

void APP_radio_panel_com1_toggle() {
    radio_com1_rotate_high = !radio_com1_rotate_high;
}

void APP_radio_panel_com1_inc() {
    if (radio_com1_rotate_high) {
        radio_com1.standby.natural++;
    } else {
        radio_com1.standby.fraction += radio_com1.band.spacing;
        if (radio_com1.standby.fraction >= 1000) {
            radio_com1.standby.fraction = 0;
            radio_com1.standby.natural++;
        }
    }
    if (radio_com1.standby.natural > radio_com1.band.bandFreqEnd.natural) {
        radio_com1.standby.natural = radio_com1.band.bandFreqStart.natural;
    }

    APP_instruments_refresh();
}

void APP_radio_panel_com1_dec() {
    if (radio_com1_rotate_high) {
        radio_com1.standby.natural--;
    } else {
        if (radio_com1.standby.fraction <= radio_com1.band.spacing) {
            radio_com1.standby.fraction = 1000;
            radio_com1.standby.natural--;
        } else {
            radio_com1.standby.fraction-=radio_com1.band.spacing;
        }
    }
    if (radio_com1.standby.natural < radio_com1.band.bandFreqStart.natural) {
        radio_com1.standby.natural = radio_com1.band.bandFreqEnd.natural;
    }

    APP_instruments_refresh();
}

void APP_radio_panel_init() {
    Graphics_clearDisplay(&g_sContext);

    APP_radio_panel_refresh();
}

void APP_radio_panel_input(APP_input_events event) {
    switch (event) {
        case APP_INPUT_ENCODER1_CCW:
            break;
        case APP_INPUT_ENCODER1_CW:
            break;
        case APP_INPUT_ENCODER2_CCW:
            APP_radio_panel_com1_dec();
            break;
        case APP_INPUT_ENCODER2_CW:
            APP_radio_panel_com1_inc();
            break;

        case APP_INPUT_BUTTON0:
            APP_radio_panel_com1_activate();
            break;
        case APP_INPUT_BUTTON1:
            APP_radio_panel_com1_toggle();
            break;
        case APP_INPUT_BUTTON2:
            break;
        case APP_INPUT_BUTTON3:
            break;


        case APP_INPUT_BUTTON4:
            APP_instruments_change(APP_INSTRUMENT_ALTITUDE_SPEED);
            break;
        case APP_INPUT_BUTTON5:
             APP_instruments_change(APP_INSTRUMENT_HORIZON);
             break;
    }
}
