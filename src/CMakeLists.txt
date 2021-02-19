add_executable(flightpanel
        main.c
        app.c
        
        usb/usb.c
        usb/usb_descriptors.c

        leds/led1.c

        displays/drivers/ili9486_driver.c
        displays/drivers/vd_driver.c

        displays/raster/cached.c
        displays/raster/lines.c
        displays/raster/rectangles.c
        displays/raster/triangles.c

        displays/raster/resources/fontebrima10b.c
        displays/raster/resources/fontebrima12b.c
        displays/raster/resources/fontebrima16b.c
        displays/raster/resources/fontebrima20b.c
        displays/raster/resources/fontebrima24b.c
        displays/raster/resources/numbers_ebrima.c
        displays/raster/resources/numbers_ebrima.h
        displays/raster/resources/numbers_ebrima_big.c
        displays/raster/resources/numbers_ebrima_huge.c
        displays/raster/resources/numbers_ebrima_medium.c
        displays/raster/resources/numbers_ebrima_medium0.c
        displays/raster/resources/numbers_ebrima_small.c
        displays/raster/resources/numbers_medium_memmap.c
        displays/raster/resources/numbers_memmap.c

        flight/instrument.c
        flight/instrument.h
        flight/instruments/altispeed.c
        flight/instruments/altispeed/altispeed_draw.h
        flight/instruments/altispeed/altispeed_scheduler.c
        flight/instruments/altispeed/altispeed_zone0.c
        flight/instruments/altispeed/altispeed_zone1.c
        flight/instruments/altispeed/altispeed_zone2.c
        flight/instruments/altispeed/altispeed_zone3.c
        flight/instruments/altispeed/altispeed_zone4.c
        flight/instruments/altispeed/altispeed_zone5.c
        flight/instruments/altispeed/altispeed_zone6.c
        flight/instruments/horizon.c
        flight/instruments/radio_panel.c

        ../lib/fonts/fontcm12b.c
        ../lib/fonts/fontcm16b.c
        ../lib/fonts/fontcmss18b.c
        ../lib/fonts/fontfixed6x8.c
        )

target_include_directories(flightpanel PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
        ${CMAKE_SOURCE_DIR}/lib
        ${CMAKE_SOURCE_DIR}/lib/grlib
        ${CMAKE_SOURCE_DIR}/lib/speedmath
        )
target_link_libraries(flightpanel PRIVATE pico_stdlib pico_multicore tinyusb_device tinyusb_board grlib speedmath)

pico_add_extra_outputs(flightpanel)