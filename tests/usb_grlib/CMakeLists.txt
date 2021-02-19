add_executable(usb_grlib
        usb_grlib.c
        ili9486_driver.c
        usb_descriptors.c
        )

target_include_directories(usb_grlib PRIVATE ${CMAKE_CURRENT_LIST_DIR} ${CMAKE_SOURCE_DIR}/lib/grlib)

target_link_libraries(usb_grlib PRIVATE pico_stdlib pico_multicore tinyusb_device tinyusb_board grlib)

# create map/bin/hex file etc.
pico_add_extra_outputs(usb_grlib)

# add url via pico_set_program_url
example_auto_set_url(usb_grlib)
