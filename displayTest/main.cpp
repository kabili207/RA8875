#include <stdio.h>
#include <stdlib.h>
#include <windows.h>

#include "FT232H.h"
#include "RA8875.h"

int main(void)
{
	hw::FT232H ft232h;
	if (!ft232h.open())
	{
		return -1;
	}

	hw::RA8875 tft(ft232h);

	if (!tft.begin(hw::RA8875::_800x480)) {
		fprintf(stderr, "RA8875 Not Found!\n");
		return -1;
	}
	
	tft.displayOn(true);
	tft.GPIOX(true);      // Enable TFT - display enable tied to GPIOX
	tft.PWM1config(true, RA8875_PWM_CLK_DIV1024); // PWM output for backlight
	tft.PWM1out(255);
	tft.fillScreen(0x1111);

	/* Switch to text mode */
	tft.textMode();

	/* Set a solid for + bg color ... */
	/* ... or a fore color plus a transparent background */

	/* Set the cursor location (in pixels) */
	tft.textSetCursor(10, 10);

	/* Render some text! */
	char string[15] = "Hello, World! ";
	tft.textTransparent(RA8875_WHITE);
	tft.textWrite(string);
	tft.textColor(RA8875_WHITE, RA8875_RED);
	tft.textWrite(string);
	tft.textTransparent(RA8875_CYAN);
	tft.textWrite(string);
	tft.textTransparent(RA8875_GREEN);
	tft.textWrite(string);
	tft.textColor(RA8875_YELLOW, RA8875_CYAN);
	tft.textWrite(string);
	tft.textColor(RA8875_BLACK, RA8875_MAGENTA);
	tft.textWrite(string);

	/* Change the cursor location and color ... */
	tft.textSetCursor(100, 100);
	tft.textTransparent(RA8875_RED);
	/* If necessary, enlarge the font */
	tft.textEnlarge(1);
	/* ... and render some more text! */
	tft.textWrite(string);
	tft.textSetCursor(100, 150);
	tft.textEnlarge(2);
	tft.textWrite(string);

	ft232h.close();
	return 0;
}
