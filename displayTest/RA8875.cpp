//
// This file is heavily based on the adafruit project for the RA8875
// https://github.com/adafruit/Adafruit_RA8875
//
#include "RA8875.h"
#include <thread>

// Command/Data pins for SPI
#define RA8875_DATAWRITE        0x00
#define RA8875_DATAREAD         0x40
#define RA8875_CMDWRITE         0x80
#define RA8875_CMDREAD          0xC0

// Registers & bits
#define RA8875_PWRR             0x01
#define RA8875_PWRR_DISPON      0x80
#define RA8875_PWRR_DISPOFF     0x00
#define RA8875_PWRR_SLEEP       0x02
#define RA8875_PWRR_NORMAL      0x00
#define RA8875_PWRR_SOFTRESET   0x01

#define RA8875_MRWC             0x02

#define RA8875_GPIOX            0xC7

#define RA8875_PLLC1            0x88
#define RA8875_PLLC1_PLLDIV2    0x80
#define RA8875_PLLC1_PLLDIV1    0x00

#define RA8875_PLLC2            0x89
#define RA8875_PLLC2_DIV1       0x00
#define RA8875_PLLC2_DIV2       0x01
#define RA8875_PLLC2_DIV4       0x02
#define RA8875_PLLC2_DIV8       0x03
#define RA8875_PLLC2_DIV16      0x04
#define RA8875_PLLC2_DIV32      0x05
#define RA8875_PLLC2_DIV64      0x06
#define RA8875_PLLC2_DIV128     0x07

#define RA8875_SYSR             0x10
#define RA8875_SYSR_8BPP        0x00
#define RA8875_SYSR_16BPP       0x0C
#define RA8875_SYSR_MCU8        0x00
#define RA8875_SYSR_MCU16       0x03

#define RA8875_PCSR             0x04
#define RA8875_PCSR_PDATR       0x00
#define RA8875_PCSR_PDATL       0x80
#define RA8875_PCSR_CLK         0x00
#define RA8875_PCSR_2CLK        0x01
#define RA8875_PCSR_4CLK        0x02
#define RA8875_PCSR_8CLK        0x03

#define RA8875_HDWR             0x14

#define RA8875_HNDFTR           0x15
#define RA8875_HNDFTR_DE_HIGH   0x00
#define RA8875_HNDFTR_DE_LOW    0x80

#define RA8875_HNDR             0x16
#define RA8875_HSTR             0x17
#define RA8875_HPWR             0x18
#define RA8875_HPWR_LOW         0x00
#define RA8875_HPWR_HIGH        0x80

#define RA8875_VDHR0            0x19
#define RA8875_VDHR1            0x1A
#define RA8875_VNDR0            0x1B
#define RA8875_VNDR1            0x1C
#define RA8875_VSTR0            0x1D
#define RA8875_VSTR1            0x1E
#define RA8875_VPWR             0x1F
#define RA8875_VPWR_LOW         0x00
#define RA8875_VPWR_HIGH        0x80

#define RA8875_HSAW0            0x30
#define RA8875_HSAW1            0x31
#define RA8875_VSAW0            0x32
#define RA8875_VSAW1            0x33

#define RA8875_HEAW0            0x34
#define RA8875_HEAW1            0x35
#define RA8875_VEAW0            0x36
#define RA8875_VEAW1            0x37

#define RA8875_MCLR             0x8E
#define RA8875_MCLR_START       0x80
#define RA8875_MCLR_STOP        0x00
#define RA8875_MCLR_READSTATUS  0x80
#define RA8875_MCLR_FULL        0x00
#define RA8875_MCLR_ACTIVE      0x40

#define RA8875_DCR                    0x90
#define RA8875_DCR_LINESQUTRI_START   0x80
#define RA8875_DCR_LINESQUTRI_STOP    0x00
#define RA8875_DCR_LINESQUTRI_STATUS  0x80
#define RA8875_DCR_CIRCLE_START       0x40
#define RA8875_DCR_CIRCLE_STATUS      0x40
#define RA8875_DCR_CIRCLE_STOP        0x00
#define RA8875_DCR_FILL               0x20
#define RA8875_DCR_NOFILL             0x00
#define RA8875_DCR_DRAWLINE           0x00
#define RA8875_DCR_DRAWTRIANGLE       0x01
#define RA8875_DCR_DRAWSQUARE         0x10


#define RA8875_ELLIPSE                0xA0
#define RA8875_ELLIPSE_STATUS         0x80

#define RA8875_MWCR0            0x40
#define RA8875_MWCR0_GFXMODE    0x00
#define RA8875_MWCR0_TXTMODE    0x80

#define RA8875_CURH0            0x46
#define RA8875_CURH1            0x47
#define RA8875_CURV0            0x48
#define RA8875_CURV1            0x49

#define RA8875_P1CR             0x8A
#define RA8875_P1CR_ENABLE      0x80
#define RA8875_P1CR_DISABLE     0x00
#define RA8875_P1CR_CLKOUT      0x10
#define RA8875_P1CR_PWMOUT      0x00

#define RA8875_P1DCR            0x8B

#define RA8875_P2CR             0x8C
#define RA8875_P2CR_ENABLE      0x80
#define RA8875_P2CR_DISABLE     0x00
#define RA8875_P2CR_CLKOUT      0x10
#define RA8875_P2CR_PWMOUT      0x00

#define RA8875_P2DCR            0x8D

#define RA8875_TPCR0                  0x70
#define RA8875_TPCR0_ENABLE           0x80
#define RA8875_TPCR0_DISABLE          0x00
#define RA8875_TPCR0_WAIT_512CLK      0x00
#define RA8875_TPCR0_WAIT_1024CLK     0x10
#define RA8875_TPCR0_WAIT_2048CLK     0x20
#define RA8875_TPCR0_WAIT_4096CLK     0x30
#define RA8875_TPCR0_WAIT_8192CLK     0x40
#define RA8875_TPCR0_WAIT_16384CLK    0x50
#define RA8875_TPCR0_WAIT_32768CLK    0x60
#define RA8875_TPCR0_WAIT_65536CLK    0x70
#define RA8875_TPCR0_WAKEENABLE       0x08
#define RA8875_TPCR0_WAKEDISABLE      0x00
#define RA8875_TPCR0_ADCCLK_DIV1      0x00
#define RA8875_TPCR0_ADCCLK_DIV2      0x01
#define RA8875_TPCR0_ADCCLK_DIV4      0x02
#define RA8875_TPCR0_ADCCLK_DIV8      0x03
#define RA8875_TPCR0_ADCCLK_DIV16     0x04
#define RA8875_TPCR0_ADCCLK_DIV32     0x05
#define RA8875_TPCR0_ADCCLK_DIV64     0x06
#define RA8875_TPCR0_ADCCLK_DIV128    0x07

#define RA8875_TPCR1            0x71
#define RA8875_TPCR1_AUTO       0x00
#define RA8875_TPCR1_MANUAL     0x40
#define RA8875_TPCR1_VREFINT    0x00
#define RA8875_TPCR1_VREFEXT    0x20
#define RA8875_TPCR1_DEBOUNCE   0x04
#define RA8875_TPCR1_NODEBOUNCE 0x00
#define RA8875_TPCR1_IDLE       0x00
#define RA8875_TPCR1_WAIT       0x01
#define RA8875_TPCR1_LATCHX     0x02
#define RA8875_TPCR1_LATCHY     0x03

#define RA8875_TPXH             0x72
#define RA8875_TPYH             0x73
#define RA8875_TPXYL            0x74

#define RA8875_INTC1            0xF0
#define RA8875_INTC1_KEY        0x10
#define RA8875_INTC1_DMA        0x08
#define RA8875_INTC1_TP         0x04
#define RA8875_INTC1_BTE        0x02

#define RA8875_INTC2            0xF1
#define RA8875_INTC2_KEY        0x10
#define RA8875_INTC2_DMA        0x08
#define RA8875_INTC2_TP         0x04
#define RA8875_INTC2_BTE        0x02

namespace hw
{
	RA8875::RA8875(IDevice& device, Pin cs, Pin rst)
		: m_device(&device)
		, m_spi(device, cs, 3000000, 0, false)
		, m_rst(rst)
		, m_width(0)
		, m_height(0)
		, m_textScale(0)
		, m_size(_800x480)
	{
		m_device->setPinDirection(m_rst, Direction::Out);
	}

	RA8875::~RA8875()
	{
	}

	bool RA8875::begin(DisplaySize s)
	{
		m_size = s;
		if (m_size == _480x272)
		{
			m_width = 480;
			m_height = 272;
		}
		else if (m_size == _800x480)
		{
			m_width = 800;
			m_height = 480;
		}
		else
		{
			return false;
		}

		m_device->setLow(m_rst);
		delay(100);
		m_device->setHigh(m_rst);
		delay(100);

		uint8_t x = readRegister8(0);
		printf("RA8875: 0x%02x\n", x);
		if (x != 0x75) {
			return false;
		}

		initialize();
		return true;
	}

	void RA8875::softReset() const
	{
		writeCommand(RA8875_PWRR);
		writeData(RA8875_PWRR_SOFTRESET);
		writeData(RA8875_PWRR_NORMAL);
		delay(1);
	}

	void RA8875::displayOn(bool on) const
	{
		if (on)
			setRegister8(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);
		else
			setRegister8(RA8875_PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
	}

	void RA8875::sleep(bool sleep) const
	{
		if (sleep)
			setRegister8(RA8875_PWRR, RA8875_PWRR_DISPOFF | RA8875_PWRR_SLEEP);
		else
			setRegister8(RA8875_PWRR, RA8875_PWRR_DISPOFF);
	}

	void RA8875::textMode() const
	{
		/* Set text mode */
		writeCommand(RA8875_MWCR0);
		uint8_t temp = readData();
		temp |= RA8875_MWCR0_TXTMODE; // Set bit 7
		writeData(temp);

		/* Select the internal (ROM) font */
		writeCommand(0x21);
		temp = readData();
		temp &= ~((1 << 7) | (1 << 5)); // Clear bits 7 and 5
		writeData(temp);
	}

	void RA8875::textSetCursor(uint16_t x, uint16_t y) const
	{
		setRegister16(0x2A, x);
		setRegister16(0x2C, y);
	}

	void RA8875::textColor(uint16_t foreColor, uint16_t bgColor) const
	{
		/* Set Fore Color */
		setColorRegister(0x63, foreColor);

		/* Set Background Color */
		setColorRegister(0x60, bgColor);

		/* Clear transparency flag */
		writeCommand(0x22);
		uint8_t temp = readData();
		temp &= ~(1 << 6); // Clear bit 6
		writeData(temp);
	}

	void RA8875::textTransparent(uint16_t foreColor) const
	{
		/* Set Fore Color */
		setColorRegister(0x63, foreColor);

		/* Set transparency flag */
		writeCommand(0x22);
		uint8_t temp = readData();
		temp |= (1 << 6); // Set bit 6
		writeData(temp);
	}

	void RA8875::textEnlarge(uint8_t scale)
	{
		if (scale > 3) scale = 3;

		/* Set font size flags */
		writeCommand(0x22);
		uint8_t temp = readData();
		temp &= ~(0xF); // Clears bits 0..3
		temp |= scale << 2;
		temp |= scale;
		writeData(temp);

		m_textScale = scale;
	}

	void RA8875::textWrite(const char* buffer, uint16_t len) const
	{
		if (len == 0) len = strlen(buffer);
		writeCommand(RA8875_MRWC);
		for (uint16_t i = 0; i < len; i++)
		{
			writeData(buffer[i]);
		}
	}

	void RA8875::graphicsMode() const
	{
		writeCommand(RA8875_MWCR0);
		uint8_t temp = readData();
		temp &= ~RA8875_MWCR0_TXTMODE; // bit #7
		writeData(temp);
	}

	void RA8875::setXY(uint16_t x, uint16_t y) const
	{
		setRegister16(RA8875_CURH0, x);
		setRegister16(RA8875_CURV0, y);
	}

	void RA8875::fillRect() const
	{
		writeCommand(RA8875_DCR);
		writeData(RA8875_DCR_LINESQUTRI_STOP | RA8875_DCR_DRAWSQUARE);
		writeData(RA8875_DCR_LINESQUTRI_START | RA8875_DCR_FILL | RA8875_DCR_DRAWSQUARE);
	}

	void RA8875::fillScreen(uint16_t color) const
	{
		rectHelper(0, 0, m_width - 1, m_height - 1, color, true);
	}

	void RA8875::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) const
	{
		setRegister16(0x91, x0);
		setRegister16(0x93, y0);
		setRegister16(0x95, x1);
		setRegister16(0x97, y1);
		setColorRegister(0x63, color);

		/* Draw! */
		writeCommand(RA8875_DCR);
		writeData(0x80);

		/* Wait for the command to finish */
		waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
	}

	void RA8875::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) const
	{
		rectHelper(x, y, x + w, y + h, color, false);
	}

	void RA8875::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) const
	{
		rectHelper(x, y, x + w, y + h, color, true);
	}

	void RA8875::drawCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) const
	{
		circleHelper(x0, y0, r, color, false);
	}

	void RA8875::fillCircle(int16_t x0, int16_t y0, int16_t r, uint16_t color) const
	{
		circleHelper(x0, y0, r, color, true);
	}

	void RA8875::drawTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) const
	{
		triangleHelper(x0, y0, x1, y1, x2, y2, color, false);
	}

	void RA8875::fillTriangle(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) const
	{
		triangleHelper(x0, y0, x1, y1, x2, y2, color, true);
	}

	void RA8875::drawEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color) const
	{
		ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, false);
	}

	void RA8875::fillEllipse(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color) const
	{
		ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, true);
	}

	void RA8875::drawCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color) const
	{
		curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, false);
	}

	void RA8875::fillCurve(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color) const
	{
		curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, true);
	}

	void RA8875::GPIOX(bool on) const
	{
		if (on)
			setRegister8(RA8875_GPIOX, 1);
		else
			setRegister8(RA8875_GPIOX, 0);
	}

	void RA8875::PWM1config(bool on, uint8_t clock) const
	{
		if (on) 
			setRegister8(RA8875_P1CR, RA8875_P1CR_ENABLE | (clock & 0xF));
		else 
			setRegister8(RA8875_P1CR, RA8875_P1CR_DISABLE | (clock & 0xF));
	}

	void RA8875::PWM2config(bool on, uint8_t clock) const
	{
		if (on) 
			setRegister8(RA8875_P2CR, RA8875_P2CR_ENABLE | (clock & 0xF));
		else 
			setRegister8(RA8875_P2CR, RA8875_P2CR_DISABLE | (clock & 0xF));
	}

	void RA8875::PWM1out(uint8_t p) const
	{
		setRegister8(RA8875_P1DCR, p);
	}

	void RA8875::PWM2out(uint8_t p) const
	{
		setRegister8(RA8875_P2DCR, p);
	}

	void RA8875::touchEnable(bool on) const
	{
		uint8_t   adcClk = uint8_t(RA8875_TPCR0_ADCCLK_DIV4);

		if (m_size == _800x480) //match up touch size with LCD size
			adcClk = uint8_t(RA8875_TPCR0_ADCCLK_DIV16);

		if (on)
		{
			/* Enable Touch Panel (Reg 0x70) */
			setRegister8(RA8875_TPCR0, RA8875_TPCR0_ENABLE |
				RA8875_TPCR0_WAIT_4096CLK |
				RA8875_TPCR0_WAKEENABLE |
				adcClk); // 10mhz max!
						 /* Set Auto Mode      (Reg 0x71) */
			setRegister8(RA8875_TPCR1, RA8875_TPCR1_AUTO |
				// RA8875_TPCR1_VREFEXT |
				RA8875_TPCR1_DEBOUNCE);
			/* Enable TP INT */
			setRegister8(RA8875_INTC1, readRegister8(RA8875_INTC1) | RA8875_INTC1_TP);
		}
		else
		{
			/* Disable TP INT */
			setRegister8(RA8875_INTC1, readRegister8(RA8875_INTC1) & ~RA8875_INTC1_TP);
			/* Disable Touch Panel (Reg 0x70) */
			setRegister8(RA8875_TPCR0, RA8875_TPCR0_DISABLE);
		}
	}

	bool RA8875::touched() const
	{
		return (readRegister8(RA8875_INTC2) & RA8875_INTC2_TP) != 0;
	}

	bool RA8875::touchRead(uint16_t *x, uint16_t *y) const
	{
		uint16_t tx, ty;
		uint8_t temp;

		tx = readRegister8(RA8875_TPXH);
		ty = readRegister8(RA8875_TPYH);
		temp = readRegister8(RA8875_TPXYL);
		tx <<= 2;
		ty <<= 2;
		tx |= temp & 0x03;        // get the bottom x bits
		ty |= (temp >> 2) & 0x03; // get the bottom y bits

		*x = tx;
		*y = ty;

		/* Clear TP INT Status */
		setRegister8(RA8875_INTC2, RA8875_INTC2_TP);
		return true;
	}

	void RA8875::setRegister8(uint8_t reg, uint8_t val) const
	{
		writeCommand(reg);
		writeData(val);
	}

	void RA8875::setRegister16(uint8_t reg, uint16_t val) const
	{
		setRegister8(reg+0, uint8_t(val & 0xFF));
		setRegister8(reg+1, uint8_t(val >> 8));
	}

	void RA8875::setColorRegister(uint8_t reg, uint16_t color) const
	{
		setRegister8(reg+0, uint8_t((color & 0xf800) >> 11));
		setRegister8(reg+1, uint8_t((color & 0x07e0) >> 5));
		setRegister8(reg+2, uint8_t(color & 0x001f));
	}

	uint8_t RA8875::readRegister8(uint8_t reg) const
	{
		writeCommand(reg);
		return readData();
	}

	void RA8875::writeData(uint8_t d) const
	{
		uint8_t data[] = { RA8875_DATAWRITE, d };
		m_spi.write(data, 2);
	}

	uint8_t RA8875::readData() const
	{
		uint8_t data[] = { RA8875_DATAREAD, 0 };
		uint8_t response[2];
		m_spi.transfer(data, response, 2);
		return response[1];
	}

	void RA8875::writeCommand(uint8_t d) const
	{
		uint8_t data[] = { RA8875_CMDWRITE, d };
		m_spi.write(data, 2);
	}

	uint8_t RA8875::readStatus() const
	{
		uint8_t data[] = { RA8875_CMDREAD, 0 };
		uint8_t response[2];
		m_spi.transfer(data, response, 2);
		return response[1];
	}

	bool RA8875::waitPoll(uint8_t r, uint8_t f) const
	{
		while (1)
		{
			uint8_t temp = readRegister8(r);
			if (!(temp & f))
				return true;
		}
		// MEMEFIX: yeah i know, unreached! - add timeout?
	}

	uint16_t RA8875::width() const
	{
		return m_width;
	}

	uint16_t RA8875::height() const
	{
		return m_height;
	}

	// -- Private methods below -------------------------

	void RA8875::PLLinit() const
	{
		if (m_size == _480x272)
		{
			setRegister8(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 10);
			delay(1);
			setRegister8(RA8875_PLLC2, RA8875_PLLC2_DIV4);
			delay(1);
		}
		else /* if (m_size == _800x480) */
		{
			setRegister8(RA8875_PLLC1, RA8875_PLLC1_PLLDIV1 + 10);
			delay(1);
			setRegister8(RA8875_PLLC2, RA8875_PLLC2_DIV4);
			delay(1);
		}
	}

	void RA8875::initialize() const
	{
		PLLinit();
		setRegister8(RA8875_SYSR, RA8875_SYSR_16BPP | RA8875_SYSR_MCU8);

		/* Timing values */
		uint8_t pixclk;
		uint8_t hsync_start;
		uint8_t hsync_pw;
		uint8_t hsync_finetune;
		uint8_t hsync_nondisp;
		uint8_t vsync_pw;
		uint16_t vsync_nondisp;
		uint16_t vsync_start;

		/* Set the correct values for the display being used */
		if (m_size == _480x272)
		{
			pixclk = RA8875_PCSR_PDATL | RA8875_PCSR_4CLK;
			hsync_nondisp = 10;
			hsync_start = 8;
			hsync_pw = 48;
			hsync_finetune = 0;
			vsync_nondisp = 3;
			vsync_start = 8;
			vsync_pw = 10;
		}
		else // (_size == RA8875_800x480)
		{
			pixclk = RA8875_PCSR_PDATL | RA8875_PCSR_2CLK;
			hsync_nondisp = 26;
			hsync_start = 32;
			hsync_pw = 96;
			hsync_finetune = 0;
			vsync_nondisp = 32;
			vsync_start = 23;
			vsync_pw = 2;
		}

		setRegister8(RA8875_PCSR, pixclk);
		delay(1);

		/* Horizontal settings registers */
		setRegister8(RA8875_HDWR, (m_width / 8) - 1);                          // H width: (HDWR + 1) * 8 = 480
		setRegister8(RA8875_HNDFTR, RA8875_HNDFTR_DE_HIGH + hsync_finetune);
		setRegister8(RA8875_HNDR, (hsync_nondisp - hsync_finetune - 2) / 8);    // H non-display: HNDR * 8 + HNDFTR + 2 = 10
		setRegister8(RA8875_HSTR, hsync_start / 8 - 1);                         // Hsync start: (HSTR + 1)*8 
		setRegister8(RA8875_HPWR, RA8875_HPWR_LOW + (hsync_pw / 8 - 1));        // HSync pulse width = (HPWR+1) * 8

		/* Vertical settings registers */
		setRegister8(RA8875_VDHR0, uint16_t((m_height - 1) & 0xFF));
		setRegister8(RA8875_VDHR1, uint16_t((m_height - 1) >> 8));
		setRegister8(RA8875_VNDR0, vsync_nondisp - 1);                          // V non-display period = VNDR + 1
		setRegister8(RA8875_VNDR1, vsync_nondisp >> 8);
		setRegister8(RA8875_VSTR0, vsync_start - 1);                            // Vsync start position = VSTR + 1
		setRegister8(RA8875_VSTR1, vsync_start >> 8);
		setRegister8(RA8875_VPWR, RA8875_VPWR_LOW + vsync_pw - 1);            // Vsync pulse width = VPWR + 1

		/* Set active window X */
		setRegister8(RA8875_HSAW0, 0);                                        // horizontal start point
		setRegister8(RA8875_HSAW1, 0);
		setRegister8(RA8875_HEAW0, uint16_t((m_width - 1) & 0xFF));            // horizontal end point
		setRegister8(RA8875_HEAW1, uint16_t((m_width - 1) >> 8));

		/* Set active window Y */
		setRegister8(RA8875_VSAW0, 0);                                        // vertical start point
		setRegister8(RA8875_VSAW1, 0);
		setRegister8(RA8875_VEAW0, uint16_t((m_height - 1) & 0xFF));           // horizontal end point
		setRegister8(RA8875_VEAW1, uint16_t((m_height - 1) >> 8));

		/* ToDo: Setup touch panel? */

		/* Clear the entire window */
		setRegister8(RA8875_MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);
		delay(500);
	}

	void RA8875::circleHelper(int16_t x0, int16_t y0, int16_t r, uint16_t color, bool filled) const
	{
		setRegister16(0x99, x0);
		setRegister16(0x9b, y0);
		setRegister8(0x9d, r);    // TODO: lookup spec, see if radius is 16-bit over into 0xde register.
		setColorRegister(0x64, color);

		/* Draw! */
		writeCommand(RA8875_DCR);
		writeData(RA8875_DCR_CIRCLE_START | (filled ? RA8875_DCR_FILL : RA8875_DCR_NOFILL));

		/* Wait for the command to finish */
		waitPoll(RA8875_DCR, RA8875_DCR_CIRCLE_STATUS);
	}

	void RA8875::rectHelper(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color, bool filled) const
	{
		setRegister16(0x91, x);
		setRegister16(0x93, y);
		setRegister16(0x95, w);
		setRegister16(0x97, h);
		setColorRegister(0x63, color);

		/* Draw! */
		writeCommand(RA8875_DCR);
		writeData(filled ? 0xB0 : 0x90);

		/* Wait for the command to finish */
		waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
	}

	void RA8875::triangleHelper(int16_t x0, int16_t y0, int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color, bool filled) const
	{
		setRegister16(0x91, x0);
		setRegister16(0x93, y0);
		setRegister16(0x95, x1);
		setRegister16(0x97, y1);
		setRegister16(0xA9, x2);
		setRegister16(0xAB, y2);
		setColorRegister(0x63, color);

		/* Draw! */
		writeCommand(RA8875_DCR);
		writeData(filled ? 0xA1 : 0x81);

		/* Wait for the command to finish */
		waitPoll(RA8875_DCR, RA8875_DCR_LINESQUTRI_STATUS);
	}

	void RA8875::ellipseHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint16_t color, bool filled) const
	{
		setRegister16(0xA5, xCenter);
		setRegister16(0xA7, yCenter);
		setRegister16(0xA1, longAxis);
		setRegister16(0xA3, shortAxis);
		setColorRegister(0x63, color);

		/* Draw! */
		writeCommand(0xA0);
		writeData(filled ? 0xC0 : 0x80);

		/* Wait for the command to finish */
		waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
	}

	void RA8875::curveHelper(int16_t xCenter, int16_t yCenter, int16_t longAxis, int16_t shortAxis, uint8_t curvePart, uint16_t color, bool filled) const
	{
		setRegister16(0xA5, xCenter);
		setRegister16(0xA7, yCenter);
		setRegister16(0xA1, longAxis);
		setRegister16(0xA3, shortAxis);
		setColorRegister(0x63, color);

		/* Draw! */
		writeCommand(0xA0);
		writeData((filled ? 0xD0 : 0x90) | (curvePart & 0x03));

		/* Wait for the command to finish */
		waitPoll(RA8875_ELLIPSE, RA8875_ELLIPSE_STATUS);
	}


	void RA8875::delay(int ms)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}
}
