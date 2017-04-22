#pragma once


#ifndef __libra8875_h__
#define __libra8875_h__

#if defined(_MSC_VER)
//  Microsoft 
#define EXPORT __declspec(dllexport)
#define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
//  GCC
#define EXPORT __attribute__((visibility("default")))
#define IMPORT
#else
//  do nothing and hope for the best?
#define EXPORT
#define IMPORT
#pragma warning Unknown dynamic link import/export semantics.
#endif

#include <stdint.h>

#include "FT232H.h"
#include "RA8875.h"

typedef void* FT232HHandle;
typedef void* RA8875Handle;

extern "C"
{
	/* Wrappers */
	EXPORT FT232HHandle   TFT_createDevice();
	EXPORT RA8875Handle   TFT_createTft(FT232HHandle device);
	EXPORT int     TFT_openDevice(FT232HHandle device);
	EXPORT void    TFT_destroyTft(RA8875Handle tft);
	EXPORT void    TFT_destroyDevice(FT232HHandle device);

	EXPORT bool    TFT_begin(RA8875Handle tft, TFT_DisplaySize s);
	EXPORT void    TFT_softReset(RA8875Handle tft);
	EXPORT void    TFT_displayOn(RA8875Handle tft, bool on);
	EXPORT void    TFT_sleep(RA8875Handle tft, bool sleep);

	/* Text functions */
	EXPORT void    TFT_textMode(RA8875Handle tft);
	EXPORT void    TFT_textSetCursor(RA8875Handle tft, uint16_t x, uint16_t y);
	EXPORT void    TFT_textColor(RA8875Handle tft, uint16_t foreColor, uint16_t bgColor);
	EXPORT void    TFT_textTransparent(RA8875Handle tft, uint16_t foreColor);
	EXPORT void    TFT_textEnlarge(RA8875Handle tft, uint8_t scale);
	EXPORT void    TFT_textWrite(RA8875Handle tft, const char* buffer);

	/* Graphics functions */
	EXPORT void    TFT_graphicsMode(RA8875Handle tft);
	EXPORT void    TFT_setXY(RA8875Handle tft, uint16_t x, uint16_t y);
	//EXPORT void    TFT_fillRect();

	/* HW accelerated wrapper functions (override Adafruit_GFX prototypes) */
	EXPORT void    TFT_fillScreen(RA8875Handle tft, uint16_t color);
	EXPORT void    TFT_drawLine(RA8875Handle tft, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
	EXPORT void    TFT_drawRect(RA8875Handle tft, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
	EXPORT void    TFT_fillRect(RA8875Handle tft, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);
	EXPORT void    TFT_drawCircle(RA8875Handle tft, uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
	EXPORT void    TFT_fillCircle(RA8875Handle tft, uint16_t x0, uint16_t y0, uint8_t r, uint16_t color);
	EXPORT void    TFT_drawTriangle(RA8875Handle tft, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
	EXPORT void    TFT_fillTriangle(RA8875Handle tft, uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
	EXPORT void    TFT_drawEllipse(RA8875Handle tft, uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint16_t color);
	EXPORT void    TFT_fillEllipse(RA8875Handle tft, uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint16_t color);
	EXPORT void    TFT_drawCurve(RA8875Handle tft, uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint8_t curvePart, uint16_t color);
	EXPORT void    TFT_fillCurve(RA8875Handle tft, uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint8_t curvePart, uint16_t color);

	/* Backlight */
	EXPORT void    TFT_brightness(RA8875Handle tft, uint8_t val);
	EXPORT void    TFT_backlight(RA8875Handle tft, bool on);
	EXPORT void    TFT_GPIOX(RA8875Handle tft, bool on);
	EXPORT void    TFT_PWM1config(RA8875Handle tft, bool on, uint8_t clock);
	EXPORT void    TFT_PWM2config(RA8875Handle tft, bool on, uint8_t clock);
	EXPORT void    TFT_PWM1out(RA8875Handle tft, uint8_t p);
	EXPORT void    TFT_PWM2out(RA8875Handle tft, uint8_t p);

	/* Touch screen */
	EXPORT void    TFT_touchEnable(RA8875Handle tft, bool on);
	EXPORT bool    TFT_touched(RA8875Handle tft);
	EXPORT bool    TFT_touchRead(RA8875Handle tft, uint16_t *x, uint16_t *y);

	/* Low level access */
	EXPORT void     TFT_setRegister8(RA8875Handle tft, TFT_Register reg, uint8_t val);
	EXPORT void     TFT_setRegister16(RA8875Handle tft, TFT_Register reg, uint16_t val);
	EXPORT void     TFT_setColorRegister(RA8875Handle tft, TFT_Register reg, uint16_t color);
	EXPORT uint8_t  TFT_readRegister8(RA8875Handle tft, TFT_Register reg);

	EXPORT void     TFT_writeData(RA8875Handle tft, uint8_t d);
	EXPORT uint8_t  TFT_readData(RA8875Handle tft);
	EXPORT void     TFT_writeCommand(RA8875Handle tft, uint8_t d);
	EXPORT uint8_t  TFT_readStatus(RA8875Handle tft);
	EXPORT bool     TFT_waitPoll(RA8875Handle tft, TFT_Register reg, uint8_t f);
	EXPORT uint16_t TFT_width(RA8875Handle tft);
	EXPORT uint16_t TFT_height(RA8875Handle tft);

}


#endif /* __libra8875_h__ */
