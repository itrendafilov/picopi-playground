add_library(grlib
        grlib/button.c
        grlib/checkbox.c
        grlib/circle.c
        grlib/context.c
        grlib/display.c
        grlib/image.c
        grlib/imageButton.c
        grlib/line.c
        grlib/radioButton.c
        grlib/rectangle.c
        grlib/string.c
        )

target_include_directories(grlib PRIVATE grlib)
pico_add_extra_outputs(grlib)

add_library(speedmath
        speedmath/speedmath.c
        )

target_include_directories(speedmath PRIVATE speedmath)
pico_add_extra_outputs(speedmath)
