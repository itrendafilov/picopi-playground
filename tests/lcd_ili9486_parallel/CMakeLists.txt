add_executable(lcd_ili9486_parallel
        lcd_ili9486_parallel.c
        )

# Pull in our (to be renamed) simple get you started dependencies
target_link_libraries(lcd_ili9486_parallel pico_stdlib hardware_spi)

# create map/bin/hex file etc.
pico_add_extra_outputs(lcd_ili9486_parallel)

# add url via pico_set_program_url
example_auto_set_url(lcd_ili9486_parallel)
