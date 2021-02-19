add_executable(usb_lcd_ili9486
        usb_lcd_ili9486.c
        ili9486.c
        usb_descriptors.c
        )

target_include_directories(usb_lcd_ili9486 PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(usb_lcd_ili9486 PRIVATE pico_stdlib pico_multicore tinyusb_device tinyusb_board)

# create map/bin/hex file etc.
pico_add_extra_outputs(usb_lcd_ili9486)

# add url via pico_set_program_url
example_auto_set_url(usb_lcd_ili9486)
