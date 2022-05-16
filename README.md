# Microsoft Flight Simulator 2020 control panel using Raspberry Pi Pico 

Provided as is. I haven't played with the module for some time.

## Getting started

See [Getting Started with the Raspberry Pi Pico](https://rptl.io/pico-get-started) and the README in the [pico-sdk](https://github.com/raspberrypi/pico-sdk) for information
on getting up and running.

## ILI9486 LCD driver

ili9486 is commonly used in TFT display modules with 320x480 pixels.
It has IM0-IM2 pins to set between different interfaces (SPI, 8bit parallel, 16bit parallel).
In this code we utilize 16bit parallel interface. Schematic can be found under picodisplay.pdf

## Changing to your ILI9486 module

You need to edit src/displays/drivers/ili9486_driver.c in order to make it work with your own setup.

## Flight control app

There is a very simple C# based app under Control-Tool/

