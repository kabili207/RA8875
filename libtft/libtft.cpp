
#include "libtft.h"
#include "RA8875.h"


FT232HHandle TFT_createDevice() {
	return reinterpret_cast<void*>(new hw::FT232H);
}

int TFT_openDevice(FT232HHandle device) {
	return reinterpret_cast<hw::FT232H*>(device)->open();
}

RA8875Handle TFT_createTft(FT232HHandle device) {
	return reinterpret_cast<void*>(new hw::RA8875(*reinterpret_cast<hw::FT232H*>(device)));
}

void TFT_destroyTft(RA8875Handle tft) {
	delete reinterpret_cast<hw::RA8875*>(tft);
}

void TFT_destroyDevice(FT232HHandle device) {
	reinterpret_cast<hw::FT232H*>(device)->close();
	delete reinterpret_cast<hw::FT232H*>(device);
}


bool TFT_begin(RA8875Handle tft, TFT_DisplaySize s) {
	return reinterpret_cast<hw::RA8875*>(tft)->begin(s);
}

void TFT_softReset(RA8875Handle tft) {
	reinterpret_cast<hw::RA8875*>(tft)->softReset();
}

void TFT_displayOn(RA8875Handle tft, bool on) {
	reinterpret_cast<hw::RA8875*>(tft)->displayOn(on);
}

void TFT_sleep(RA8875Handle tft, bool sleep) {
	reinterpret_cast<hw::RA8875*>(tft)->sleep(sleep);
}


/** Text functions **/
void TFT_textMode(RA8875Handle tft) {
	reinterpret_cast<hw::RA8875*>(tft)->textMode();
}

void TFT_textSetCursor(RA8875Handle tft, uint16_t x, uint16_t y) {
	reinterpret_cast<hw::RA8875*>(tft)->textSetCursor(x, y);
}

void TFT_textColor(RA8875Handle tft, uint16_t foreColor, uint16_t bgColor) {
	reinterpret_cast<hw::RA8875*>(tft)->textColor(foreColor, bgColor);
}

void TFT_textTransparent(RA8875Handle tft, uint16_t foreColor) {
	reinterpret_cast<hw::RA8875*>(tft)->textTransparent(foreColor);
}

void TFT_textEnlarge(RA8875Handle tft, uint8_t scale) {
	reinterpret_cast<hw::RA8875*>(tft)->textEnlarge(scale);
}

void TFT_textWrite(RA8875Handle tft, const char* buffer) {
	reinterpret_cast<hw::RA8875*>(tft)->textWrite(buffer);
}

/* Graphics functions */
void TFT_graphicsMode(RA8875Handle tft) {
	reinterpret_cast<hw::RA8875*>(tft)->graphicsMode();
}

void TFT_setXY(RA8875Handle tft, uint16_t x, uint16_t y) {
	reinterpret_cast<hw::RA8875*>(tft)->setXY(x, y);
}

void TFT_drawPixel(RA8875Handle tft, int16_t x, int16_t y, uint16_t color) {
	reinterpret_cast<hw::RA8875*>(tft)->drawPixel(x, y, color);
}

void TFT_drawPixels(RA8875Handle tft, uint16_t p[], uint16_t count, int16_t x, int16_t y) {
	reinterpret_cast<hw::RA8875*>(tft)->drawPixels(p, count, x, y);
}

//void TFT_fillRect() {
//	reinterpret_cast<hw::RA8875*>(tft)->fillRect();
//}

/* HW accelerated wrapper functions (override Adafruit_GFX prototypes) */

void TFT_fillScreen(RA8875Handle tft, uint16_t color) {
	reinterpret_cast<hw::RA8875*>(tft)->fillScreen(color);
}

void TFT_drawLine(RA8875Handle tft, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) {
	reinterpret_cast<hw::RA8875*>(tft)->drawLine(x0, y0, x1, y1, color);
}

void TFT_drawRect(RA8875Handle tft, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	reinterpret_cast<hw::RA8875*>(tft)->drawRect(x, y, w, h, color);
}

void TFT_fillRect(RA8875Handle tft, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) {
	reinterpret_cast<hw::RA8875*>(tft)->fillRect(x, y, w, h, color);
}

void TFT_drawCircle(RA8875Handle tft, uint16_t x0, uint16_t y0, uint8_t r, uint16_t color) {
	reinterpret_cast<hw::RA8875*>(tft)->drawCircle(x0, y0, r, color);
}

void TFT_fillCircle(RA8875Handle tft, uint16_t x0, uint16_t y0, uint8_t r, uint16_t color) {
	reinterpret_cast<hw::RA8875*>(tft)->fillCircle(x0, y0, r, color);
}

void TFT_drawTriangle(RA8875Handle tft, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	reinterpret_cast<hw::RA8875*>(tft)->drawTriangle(x0, y0, x1, y1, x2, y2, color);
}

void TFT_fillTriangle(RA8875Handle tft, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) {
	reinterpret_cast<hw::RA8875*>(tft)->fillTriangle(x0, y0, x1, y1, x2, y2, color);
}

void TFT_drawEllipse(RA8875Handle tft, uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint16_t color) {
	reinterpret_cast<hw::RA8875*>(tft)->drawEllipse(xCenter, yCenter, longAxis, shortAxis, color);
}

void TFT_fillEllipse(RA8875Handle tft, uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint16_t color) {
	reinterpret_cast<hw::RA8875*>(tft)->fillEllipse(xCenter, yCenter, longAxis, shortAxis, color);
}

void TFT_drawCurve(RA8875Handle tft, uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint8_t curvePart, uint16_t color) {
	reinterpret_cast<hw::RA8875*>(tft)->drawCurve(xCenter, yCenter, longAxis, shortAxis, curvePart, color);
}

void TFT_fillCurve(RA8875Handle tft, uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint8_t curvePart, uint16_t color) {
	reinterpret_cast<hw::RA8875*>(tft)->fillCurve(xCenter, yCenter, longAxis, shortAxis, curvePart, color);
}

/* Backlight */
void TFT_brightness(RA8875Handle tft, uint8_t val) {
	reinterpret_cast<hw::RA8875*>(tft)->brightness(val);
}

void TFT_backlight(RA8875Handle tft, bool on) {
	reinterpret_cast<hw::RA8875*>(tft)->backlight(on);
}

void TFT_GPIOX(RA8875Handle tft, bool on) {
	reinterpret_cast<hw::RA8875*>(tft)->GPIOX(on);
}

void TFT_PWM1config(RA8875Handle tft, bool on, uint8_t clock) {
	reinterpret_cast<hw::RA8875*>(tft)->PWM1config(on, clock);
}

void TFT_PWM2config(RA8875Handle tft, bool on, uint8_t clock) {
	reinterpret_cast<hw::RA8875*>(tft)->PWM2config(on, clock);
}

void TFT_PWM1out(RA8875Handle tft, uint8_t p) {
	reinterpret_cast<hw::RA8875*>(tft)->PWM1out(p);
}

void TFT_PWM2out(RA8875Handle tft, uint8_t p) {
	reinterpret_cast<hw::RA8875*>(tft)->PWM2out(p);
}

/* Touch screen */
void TFT_touchEnable(RA8875Handle tft, bool on) {
	reinterpret_cast<hw::RA8875*>(tft)->touchEnable(on);
}

bool TFT_touched(RA8875Handle tft) {
	return reinterpret_cast<hw::RA8875*>(tft)->touched();
}

bool TFT_touchRead(RA8875Handle tft, uint16_t *x, uint16_t *y) {
	return reinterpret_cast<hw::RA8875*>(tft)->touchRead(x, y);
}

/* Low level access */
void TFT_setRegister8(RA8875Handle tft, TFT_Register reg, uint8_t val) {
	reinterpret_cast<hw::RA8875*>(tft)->setRegister8(reg, val);
}

void TFT_setRegister16(RA8875Handle tft, TFT_Register reg, uint16_t val) {
	reinterpret_cast<hw::RA8875*>(tft)->setRegister16(reg, val);
}

void TFT_setColorRegister(RA8875Handle tft, TFT_Register reg, uint16_t color) {
	reinterpret_cast<hw::RA8875*>(tft)->setColorRegister(reg, color);
}

uint8_t TFT_readRegister8(RA8875Handle tft, TFT_Register reg) {
	return reinterpret_cast<hw::RA8875*>(tft)->readRegister8(reg);
}


void TFT_writeData(RA8875Handle tft, uint8_t d) {
	reinterpret_cast<hw::RA8875*>(tft)->writeData(d);
}

uint8_t TFT_readData(RA8875Handle tft) {
	return reinterpret_cast<hw::RA8875*>(tft)->readData();
}

void TFT_writeCommand(RA8875Handle tft, uint8_t d) {
	reinterpret_cast<hw::RA8875*>(tft)->writeCommand(d);
}

uint8_t TFT_readStatus(RA8875Handle tft) {
	return reinterpret_cast<hw::RA8875*>(tft)->readStatus();
}

bool TFT_waitPoll(RA8875Handle tft, TFT_Register reg, uint8_t f) {
	return reinterpret_cast<hw::RA8875*>(tft)->waitPoll(reg, f);
}

uint16_t TFT_width(RA8875Handle tft) {
	return reinterpret_cast<hw::RA8875*>(tft)->width();
}

uint16_t TFT_height(RA8875Handle tft) {
	return reinterpret_cast<hw::RA8875*>(tft)->height();
}