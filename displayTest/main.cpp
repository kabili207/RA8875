/* simple.c

Simple libftdi usage example

This program is distributed under the GPL, version 2
*/

#include <stdio.h>
#include <stdlib.h>
#include <FT232H.h>
#include <windows.h>

int main(void)
{
	FT232H ft232h;
	if (!ft232h.open())
	{
		return -1;
	}

	SPI spi(&ft232h, Pin::C0, 1000000, 0, false);
	ft232h.setup(Pin::D7, Mode::Out);
	
	for (int i = 0; i < 10; i++)
	{
		ft232h.set_high(Pin::D7);
		Sleep(500);
		ft232h.set_low(Pin::D7);
		Sleep(500);
	}

	ft232h.close();
}
