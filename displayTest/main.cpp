#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include "FT232H.h"
#include "RA8875.h"

#include "libtft.h"

#include <thread>

static void delay(int ms)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int main(void)
{
	FT232HHandle device = TFT_createDevice();
	if (!TFT_openDevice(device))
	{
		return -1;
	}

	
	//hw::RA8875 tft(ft232h);
	RA8875Handle tft = TFT_createTft(device);

	if (!TFT_begin(tft, TFT_DisplaySize::_800x480)) {
		fprintf(stderr, "RA8875 Not Found!\n");
		return -1;
	}
	
	//TFT_displayOn(tft, true);
	TFT_backlight(tft, true);      // Enable TFT - display enable tied to GPIOX
	TFT_brightness(tft, 255);
	TFT_fillScreen(tft, 0x1111);

	TFT_displayOn(tft, true);

	/* Switch to text mode */
	TFT_textMode(tft);
	TFT_setFont(tft, TFT_Font::Calibri20);

	/* Set a solid for + bg color ... */
	/* ... or a fore color plus a transparent background */

	/* Set the cursor location (in pixels) */
	TFT_textSetCursor(tft, 10, 10);

	/* Render some text! */
	char string[15] = "Hello, World! ";
	TFT_textTransparent(tft, RA8875_WHITE);
	TFT_textWrite(tft, string);
	TFT_textColor(tft, RA8875_WHITE, RA8875_RED);
	TFT_textWrite(tft, string);
	TFT_textTransparent(tft, RA8875_CYAN);
	TFT_textWrite(tft, string);
	TFT_textTransparent(tft, RA8875_GREEN);
	TFT_textWrite(tft, string);
	TFT_textColor(tft, RA8875_YELLOW, RA8875_CYAN);
	TFT_textWrite(tft, string);
	TFT_textColor(tft, RA8875_BLACK, RA8875_MAGENTA);
	TFT_textWrite(tft, string);

	/* Change the cursor location and color ... */
	TFT_textSetCursor(tft, 100, 100);
	TFT_textTransparent(tft, RA8875_RED);
	/* If necessary, enlarge the font */
	TFT_setFont(tft, TFT_Font::Consolas24);
	TFT_setFontScale(tft, 1, 1);
	/* ... and render some more text! */
	TFT_textWrite(tft, string);
	TFT_textSetCursor(tft, 100, 150);
	TFT_setFontScale(tft, 2, 2);
	TFT_textWrite(tft, string);
	delay(500);

	// the other testing stuff.
	TFT_graphicsMode(tft);

	TFT_fillScreen(tft, RA8875_RED);
	delay(500);
	TFT_fillScreen(tft, RA8875_YELLOW);
	delay(500);
	TFT_fillScreen(tft, RA8875_GREEN);
	delay(500);
	TFT_fillScreen(tft, RA8875_CYAN);
	delay(500);
	TFT_fillScreen(tft, RA8875_MAGENTA);
	delay(500);
	TFT_fillScreen(tft, RA8875_BLACK);

	// Try some GFX acceleration!
	for (int i = 0; i < 50; ++i)
	{
		int x = rand() % TFT_width(tft);
		int y = rand() % TFT_height(tft);
		TFT_fillCircle(tft, x, y, 50, rand() & 0xffff);
		TFT_drawCircle(tft, x, y, 50, RA8875_BLACK);
	}

	TFT_fillRect(tft, 11, 11, 398, 198, RA8875_BLUE);
	TFT_drawRect(tft, 10, 10, 400, 200, RA8875_GREEN);
	//TFT_fillRoundRect(tft, 200, 10, 200, 100, 10, RA8875_RED);
	//TFT_drawPixel(tft, 10, 10, RA8875_BLACK);
	//TFT_drawPixel(tft, 11, 11, RA8875_BLACK);
	TFT_drawLine(tft, 10, 10, 200, 100, RA8875_RED);
	TFT_drawTriangle(tft, 200, 15, 250, 100, 150, 125, RA8875_BLACK);
	TFT_fillTriangle(tft, 200, 16, 249, 99, 151, 124, RA8875_YELLOW);
	TFT_drawEllipse(tft, 300, 100, 100, 40, RA8875_BLACK);
	TFT_fillEllipse(tft, 300, 100, 98, 38, RA8875_GREEN);
	// Argument 5 (curvePart) is a 2-bit value to control each corner (select 0, 1, 2, or 3)
	TFT_drawCurve(tft, 50, 100, 80, 40, 2, RA8875_BLACK);
	TFT_fillCurve(tft, 50, 100, 78, 38, 2, RA8875_WHITE);

	delay(1000);

	TFT_displayOn(tft, false);
	TFT_backlight(tft, false);

	TFT_backlight(tft, true);
	TFT_displayOn(tft, true);

	//TFT_sleep(tft, true);
	//TFT_sleep(tft, false);
	//TFT_softReset(tft);
	
	TFT_destroyDevice(device);

	return 0;
}
