# RA8875 Display test.

This is my attempt at talking to the RA8875 chipset, through the FT232H chipset over USB.

I use these two boards:

RA8875 breakout board:
https://www.adafruit.com/products/1590

FT232H usb board:
https://www.adafruit.com/products/2264

And this display:
https://www.adafruit.com/products/1596

The code assumes the following connections:
* connect 5v and gnd accordingly
* Connect (FT232H)D0  -> (RA8875)SCK
* Connect (FT232H)D1  -> (RA8875)MOSI
* Connect (FT232H)D2  -> (RA8875)MISO
* Connect (FT232H)D3  -> (RA8875)CS
* Connect (FT232H)D4  -> (RA8875)RST


## How to build the code?
* get premake5 from here: http://premake.github.io/
* run "premake5.exe vs2015"
* open the generated Visual Studio project in "build/displayTest.sln"

## How to setup the FT232H drivers?
* follow these steps: https://learn.adafruit.com/adafruit-ft232h-breakout/windows-setup#libusb-driver-install




