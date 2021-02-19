/*
 * Copyright(c) 2021 Ianislav Trendafilov. All rights reserved.
 */

#include "app.h"

int main(void) {

    APP_systemBoot();
    APP_systemInit();

    for (;;) {
        APP_loop();
    }

    return (0);
}
