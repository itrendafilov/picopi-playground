/*
 * Copyright 2020-2021 Ianislav Trendafilov. All rights reserved.
 */

#include "displays/displays.h"
#include "flight/instrument.h"
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "bsp/board.h"

// #include "pico/sem.h"
// static semaphore_t semInstUpdate;

volatile static unsigned int activeInstrument = APP_INSTRUMENT_ALTITUDE_SPEED; // APP_INSTRUMENT_HORIZON; //

static void instrumentClearScreen() {
    Graphics_clearDisplay(&g_sContext);
}

static void ignoreCallback() {
}
static void ignoreEventCallback(APP_input_events event) {
}


typedef void (*InstrumentCallback)();
typedef void (*InstrumentEventCallback)(APP_input_events event);

static const InstrumentCallback APP_instruments_init_callbacks[APP_INSTRUMENT_ELEMENTS_COUNT] = {
    &instrumentClearScreen,             /*     APP_INSTRUMENT_BLANK_SCREEN      */
    &APP_horizon_init,                  /*     APP_INSTRUMENT_HORIZON           */
    &APP_altispeed_init,                /*     APP_INSTRUMENT_ALTITUDE_SPEED    */
    &APP_radio_panel_init               /*     APP_INSTRUMENT_RADIO             */
};

static const InstrumentCallback APP_instruments_refresh_callbacks[APP_INSTRUMENT_ELEMENTS_COUNT] = {
    &ignoreCallback,                    /*     APP_INSTRUMENT_BLANK_SCREEN      */
    &APP_horizon_refresh,               /*     APP_INSTRUMENT_HORIZON           */
    &APP_altispeed_refresh,             /*     APP_INSTRUMENT_ALTITUDE_SPEED    */
    &APP_radio_panel_refresh            /*     APP_INSTRUMENT_RADIO             */
};

static const InstrumentCallback APP_instruments_input_callbacks[APP_INSTRUMENT_ELEMENTS_COUNT] = {
    &ignoreEventCallback,               /*     APP_INSTRUMENT_BLANK_SCREEN      */
    &APP_horizon_input,                 /*     APP_INSTRUMENT_HORIZON           */
    &APP_altispeed_input,               /*     APP_INSTRUMENT_ALTITUDE_SPEED    */
    &APP_radio_panel_input              /*     APP_INSTRUMENT_RADIO             */
};

typedef enum INSTRUMENT_INPUTS {
    INSTRUMENT_INPUT_BUTTON0 = 0,
    INSTRUMENT_INPUT_BUTTON1,
    INSTRUMENT_INPUT_BUTTON2,
    INSTRUMENT_INPUT_BUTTON3,
    INSTRUMENT_INPUT_BUTTON4,
    INSTRUMENT_INPUT_BUTTON5,
    INSTRUMENT_INPUT_ENCODER1A,
    INSTRUMENT_INPUT_ENCODER2A,
    INSTRUMENT_INPUT_ENCODER1B,
    INSTRUMENT_INPUT_ENCODER2B,
    INSTRUMENT_INPUT_COUNT
} INSTRUMENT_INPUTS;
// Filtered values for inputs
static uint8_t instrumentinput[INSTRUMENT_INPUT_COUNT] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

static unsigned int instrumentReadInput(INSTRUMENT_INPUTS input, bool value) {
    register uint8_t val = instrumentinput[input];
    val <<= 1;
    val |= value;
    instrumentinput[input] = val;
    return (val & 0x0F) == 0x08 ? 1 << input: 0; // (val & 0b0000_1111) == 0b0000_1000
}

static void instrumentsInit() {
    APP_instruments_init_callbacks[activeInstrument]();
}

static void instrumentsHandleInput(APP_input_events event) {
    APP_instruments_input_callbacks[activeInstrument](event);
}

void instrumentsResetGraphics() {
    Graphics_setForegroundColor(&g_sContext, ClrWhite);
    Graphics_setBackgroundColor(&g_sContext, ClrBlue);
    Graphics_setFont(&g_sContext, &g_sFontFixed6x8);
    Graphics_clearDisplay(&g_sContext);
}

/*
 *  We would like to perform a redraw of an instrument
 */
void instrumentRedraw()
{
    for (;;) {
        multicore_fifo_pop_blocking();
        APP_instruments_refresh_callbacks[activeInstrument]();
    }
}

/*
 *  New external event input received (button / switch / encoder)
 */
// Void swiInstrumentInputFxn(UArg arg0, UArg arg1)
// {
//     GPIO_toggle(Board_LED1);

//     switch (arg0) {
//         case 0x40:
//             // check value of B channel when falling edge had occurred
//             if(instrumentinput[INSTRUMENT_INPUT_ENCODER1B] & 0x3E) { // 0b0011_1000
//                 Log_print0(Diags_INFO, "Encoder1 rotated in CCW direction");
//                 instrumentsHandleInput(APP_INPUT_ENCODER1_CCW);
//             } else {
//                 Log_print0(Diags_INFO, "Encoder1 rotated in CW direction");
//                 instrumentsHandleInput(APP_INPUT_ENCODER1_CW);
//             }
//             break;
//         case 0x80:
//             // check value of B channel when falling edge had occurred
//             if(instrumentinput[INSTRUMENT_INPUT_ENCODER2B] & 0x3E) {
//                 Log_print0(Diags_INFO, "Encoder2 rotated in CCW direction");
//                 instrumentsHandleInput(APP_INPUT_ENCODER2_CCW);
//             } else {
//                 Log_print0(Diags_INFO, "Encoder2 rotated in CW direction");
//                 instrumentsHandleInput(APP_INPUT_ENCODER2_CW);
//             }
//             break;
//         default:
//             Log_print1(Diags_INFO, "Button input 0x%x", arg0);
//             if (arg0 & 0x01) {
//                 instrumentsHandleInput(APP_INPUT_BUTTON0);
//             } else if (arg0 & 0x02) {
//                 instrumentsHandleInput(APP_INPUT_BUTTON1);
//             } else if (arg0 & 0x04) {
//                 instrumentsHandleInput(APP_INPUT_BUTTON2);
//             } else if (arg0 & 0x08) {
//                 instrumentsHandleInput(APP_INPUT_BUTTON3);
//             } else if (arg0 & 0x10) {
//                 instrumentsHandleInput(APP_INPUT_BUTTON4);
//             } else if (arg0 & 0x20) {
//                 instrumentsHandleInput(APP_INPUT_BUTTON5);
//             }
//     }
// }

// Void instrumentReadInputsFxn(UArg arg0)
// {
//     register uint16_t changes = 0;
// #ifdef INSTRUMENT_INPUT_READ_RAW // read using low-level registers
//     /*
//      *  MSP_EXP430F5529LP_S1 = GPIOMSP430_P2_6   == INSTRUMENT_INPUT_BUTTON4
//      *  MSP_EXP430F5529LP_S2 = GPIOMSP430_P3_1   == INSTRUMENT_INPUT_BUTTON5
//      *  MSP_EXP430F5529LP_B1 = GPIOMSP430_P2_0   == INSTRUMENT_INPUT_BUTTON0
//      *  MSP_EXP430F5529LP_B2 = GPIOMSP430_P2_1   == INSTRUMENT_INPUT_BUTTON1
//      *  MSP_EXP430F5529LP_ENC1A = GPIOMSP430_P2_4
//      *  MSP_EXP430F5529LP_ENC1B = GPIOMSP430_P2_5
//      *  MSP_EXP430F5529LP_B3 = GPIOMSP430_P2_2   == INSTRUMENT_INPUT_BUTTON2
//      *  MSP_EXP430F5529LP_B4 = GPIOMSP430_P2_3   == INSTRUMENT_INPUT_BUTTON3
//      *  MSP_EXP430F5529LP_ENC2A = GPIOMSP430_P2_7
//      *  MSP_EXP430F5529LP_ENC2B = GPIOMSP430_P3_0
//      */

//     register uint16_t portVal;

//     portVal = P3IN;
//     instrumentReadInput(INSTRUMENT_INPUT_ENCODER2B, portVal & 0x1);//update only
//     portVal >>= 1;
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_BUTTON5, portVal & 0x1);

//     portVal = P2IN;
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_BUTTON0, portVal & 0x1);
//     portVal >>= 1;
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_BUTTON1, portVal & 0x1);
//     portVal >>= 1;
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_BUTTON2, portVal & 0x1);
//     portVal >>= 1;
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_BUTTON3, portVal & 0x1);
//     portVal >>= 1;
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_ENCODER1A, portVal & 0x1);
//     portVal >>= 1;
//     instrumentReadInput(INSTRUMENT_INPUT_ENCODER1B, portVal & 0x1);//update only
//     portVal >>= 1;
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_BUTTON4, portVal & 0x1);
//     portVal >>= 1;
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_ENCODER2A, portVal & 0x1);
// #else  // read using TI-RTOS
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_BUTTON0, GPIO_read(Board_BUTTON0));
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_BUTTON1, GPIO_read(Board_BUTTON1));
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_BUTTON2, GPIO_read(Board_BUTTON2));
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_BUTTON3, GPIO_read(Board_BUTTON3));
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_BUTTON4, GPIO_read(Board_BUTTON4));
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_BUTTON5, GPIO_read(Board_BUTTON5));
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_ENCODER1A, GPIO_read(Board_ENCODER1_A));
//     changes |= instrumentReadInput(INSTRUMENT_INPUT_ENCODER2A, GPIO_read(Board_ENCODER2_A));

//     // Also filter inputs for B channel on encoders
//     instrumentReadInput(INSTRUMENT_INPUT_ENCODER1B, GPIO_read(Board_ENCODER1_B));
//     instrumentReadInput(INSTRUMENT_INPUT_ENCODER2B, GPIO_read(Board_ENCODER2_B));
// #endif /* INSTRUMENT_INPUT_READ_RAW */

//     if (changes) {
//         Swi_Params swiParams;
//         Swi_getAttrs(swiInputHandle, NULL, &swiParams);
//         swiParams.arg0 = changes;
//         Swi_setAttrs(swiInputHandle, NULL, &swiParams);

//         Swi_post(swiInputHandle);
//     }
// }

// Void instrumentPanelRefreshFxn(UArg arg0, UArg arg1)
// {
//     Log_print0(Diags_INFO, "Running task to periodically toggle LED2.");

// #ifdef Board_LCD_ILI9486
//     // Initialize driver
//     ili9486_DriverInit();
//     // Set Graphics Library context
//     Graphics_initContext(&g_sContext, &g_ili9486_Driver);
// #endif /* Board_LCD_ILI9341 */


// #ifdef Board_LCD_ILI9341
//     // Initialize driver
//     ili9341_DriverInit();
//     // Set Graphics Library context
//     Graphics_initContext(&g_sContext, &g_ili9341_Driver);
// #endif /* Board_LCD_ILI9341 */

// #ifdef Board_LCD_SSD1306
//     // Initialize driver
//     ssd1306_DriverInit();
//     // Set Graphics Library context
//     Graphics_initContext(&g_sContext, &g_ssd1306_Driver);
// #endif /* Board_LCD_SSD1306 */

//     instrumentsResetGraphics();

//     instrumentsInit();

//     for(;;) {
//         /* Wait for a signal that there is something to refresh */
//         Semaphore_pend(semInstUpdate, BIOS_WAIT_FOREVER);
//         /* Send a signal to Redraw SWI */
//         Swi_post(swiRedrawHandle);
//         GPIO_toggle(Board_LED1);
//     }
// }

void APP_instruments_init() {
// sem_init(&semInstUpdate, 0, 1);
#ifdef Board_LCD_ILI9486
    // Initialize driver
    ili9486_DriverInit();
    // Set Graphics Library context
    Graphics_initContext(&g_sContext, &g_ili9486_Driver);
#endif /* Board_LCD_ILI9341 */

    // Setup the redraw method into Core1
    // signaling will happen over the
    // multicore push/pop API
    multicore_launch_core1(instrumentRedraw);

    // Draw default instument
    instrumentsResetGraphics();
    instrumentsInit();
}

void APP_instruments_refresh() {
    // send any message to Core1 so it unblocks
    multicore_fifo_push_blocking(1);
}

void APP_instruments_change(unsigned int instrument) {
    activeInstrument = instrument;

    instrumentsResetGraphics();      // Reset graphics config

    instrumentsInit();               // Initialize (fresh draw) the new instrument
}
