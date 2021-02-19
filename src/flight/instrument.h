/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#ifndef SRC_FLIGHT_INSTRUMENTS_H_
#define SRC_FLIGHT_INSTRUMENTS_H_

typedef enum APP_instument_types {
    APP_INSTRUMENT_BLANK_SCREEN = 0,
    APP_INSTRUMENT_HORIZON,
    APP_INSTRUMENT_ALTITUDE_SPEED,
    APP_INSTRUMENT_RADIO,
    APP_INSTRUMENT_ELEMENTS_COUNT
} APP_instument_types;

typedef enum APP_input_events {
    APP_INPUT_BUTTON0 = 0,
    APP_INPUT_BUTTON1,
    APP_INPUT_BUTTON2,
    APP_INPUT_BUTTON3,
    APP_INPUT_BUTTON4,
    APP_INPUT_BUTTON5,
    APP_INPUT_ENCODER1_CW,
    APP_INPUT_ENCODER1_CCW,
    APP_INPUT_ENCODER2_CW,
    APP_INPUT_ENCODER2_CCW
} APP_input_events;

void APP_instruments_init();
void APP_instruments_refresh();
void APP_instruments_change(unsigned int instrument);

void APP_radio_panel_init();
void APP_radio_panel_refresh();
void APP_radio_panel_input(APP_input_events event);

void APP_radio_panel_com1_activate();
void APP_radio_panel_com1_set_spacing(uint8_t spacing);
void APP_radio_panel_com1_set_primary(uint8_t freq_natural, uint16_t freq_fraction);
void APP_radio_panel_com1_set_standby(uint8_t freq_natural, uint16_t freq_fraction);

void APP_radio_panel_com1_toggle();

void APP_radio_panel_com1_inc();
void APP_radio_panel_com1_dec();

void APP_horizon_init();
void APP_horizon_refresh();
void APP_horizon_input(APP_input_events event);

typedef enum APP_instrument_horizon_param {
    /* Angle of attack */
    APP_INSTRUMENT_HORIZON_PITCH = 0,
    /* Roll angle */
    APP_INSTRUMENT_HORIZON_BANK,

    APP_INSTRUMENT_HORIZON_PARAM_COUNT
} APP_instrument_horizon_param;

void APP_horizon_update(APP_instrument_horizon_param param, float value);


void APP_altispeed_init();
void APP_altispeed_refresh();
void APP_altispeed_input(APP_input_events event);

typedef enum APP_instrument_altispeed_param {
    /* Barometric pressure */
    APP_INSTRUMENT_ALTISPEED_BARO = 0,
    /* Never Exceed Speed in smooth air. This speed must not be exceeded in any operation. */
    APP_INSTRUMENT_ALTISPEED_VNE,
    /*  Maximum Structural Cruising Speed. This speed may be exceeded only in smooth air, and then only with caution. */
    APP_INSTRUMENT_ALTISPEED_VNO,
    /* Maximum Flaps Extended Speed. This speed must not be exceeded with the given flap setting.*/
    APP_INSTRUMENT_ALTISPEED_VFE,
    /* Stalling Speed or the minimum steady flight speed is the minimum speed at which the airplane is controllable.*/
    APP_INSTRUMENT_ALTISPEED_VS,
    /* Stalling Speed or the minimum steady flight speed is the minimum speed at which the airplane is controllable in the landing configuration at the most forward center of gravity. */
    APP_INSTRUMENT_ALTISPEED_VMCA,
    /* Pressure Altitude is the altitude read from an altimeter. */
    APP_INSTRUMENT_ALTISPEED_ALTITUDE,
    /* Knots Indicated Airspeed is the speed shown on the airspeed indicator and expressed in knots. */
    APP_INSTRUMENT_ALTISPEED_KIAS,

    APP_INSTRUMENT_ALTISPEED_PARAM_COUNT
} APP_instrument_altispeed_param;

void APP_altispeed_update(APP_instrument_altispeed_param param, int16_t value);

#endif /* SRC_FLIGHT_INSTRUMENTS_H_ */
