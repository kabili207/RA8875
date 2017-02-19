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

#define RA8875_INTC1_KEY        0x10
#define RA8875_INTC1_DMA        0x08
#define RA8875_INTC1_TP         0x04
#define RA8875_INTC1_BTE        0x02

#define RA8875_INTC2_KEY        0x10
#define RA8875_INTC2_DMA        0x08
#define RA8875_INTC2_TP         0x04
#define RA8875_INTC2_BTE        0x02

namespace hw
{
	// All Registers.
	enum class RA8875::Register : uint8_t {
		STSR = 0x00, // Status Register
		PWRR = 0x01, // Power and Display Control Register (PWRR)
		MRWC = 0x02, // Memory Read/Write Command (MRWC)
		PCSR = 0x04, // Pixel Clock Setting Register (PCSR)
		SROC = 0x05, // Serial Flash/ROM Configuration Register (SROC)
		SFCLR = 0x06, // Serial Flash/ROM CLK Setting Register(SFCLR)
		SYSR = 0x10, // System Configuration Register (SYSR)
		GPI = 0x12, // GPI
		GPO = 0x13, // GPO
		HDWR = 0x14, // LCD Horizontal Display Width Register (HDWR)
		HNDFTR = 0x15, // Horizontal Non-Display Period Fine Tuning Option Register (HNDFTR)
		HNDR = 0x16, // LCD Horizontal Non-Display Period Register (HNDR)
		HSTR = 0x17, // HSYNC Start Position Register (HSTR)
		HPWR = 0x18, // HSYNC Pulse Width Register (HPWR)
		VDHR0 = 0x19, // LCD Vertical Display Height Register (VDHR0)
		VDHR1 = 0x1A, // LCD Vertical Display Height Register0 (VDHR1)
		VNDR0 = 0x1B, // LCD Vertical Non-Display Period Register (VNDR0)
		VNDR1 = 0x1C, // LCD Vertical Non-Display Period Register (VNDR1)
		VSTR0 = 0x1D, // VSYNC Start Position Register (VSTR0)
		VSTR1 = 0x1E, // VSYNC Start Position Register (VSTR1)
		VPWR = 0x1F, // VSYNC Pulse Width Register (VPWR)
		DPCR = 0x20, // Display Configuration Register (DPCR)
		FNCR0 = 0x21, // Font Control Register 0 (FNCR0)
		FNCR1 = 0x22, // Font Control Register 1 (FNCR1)
		CGSR = 0x23, // CGRAM Select Register (CGSR)
		HOFS0 = 0x24, // Horizontal Scroll Offset Register 0 (HOFS0)
		HOFS1 = 0x25, // Horizontal Scroll Offset Register 1 (HOFS1)
		VOFS0 = 0x26, // Vertical Scroll Offset Register 0 (VOFS0)
		VOFS1 = 0x27, // Vertical Scroll Offset Register 1 (VOFS1)
		FLDR = 0x29, // Font Line Distance Setting Register (FLDR)
		F_CURXL = 0x2A, // Font Write Cursor Horizontal Position Register 0 (F_CURXL)
		F_CURXH = 0x2B, // Font Write Cursor Horizontal Position Register 1 (F_CURXH)
		F_CURYL = 0x2C, // Font Write Cursor Vertical Position Register 0 (F_CURYL)
		F_CURYH = 0x2D, // Font Write Cursor Vertical Position Register 1 (F_CURYH)
		F_TSET = 0x2E, // Font Write Type Setting Register
		F_RSET = 0x2F, // Serial Font ROM Setting
		HSAW0 = 0x30, // Horizontal Start Point 0 of Active Window (HSAW0)
		HSAW1 = 0x31, // Horizontal Start Point 1 of Active Window (HSAW1)
		VSAW0 = 0x32, // Vertical Start Point 0 of Active Window (VSAW0)
		VSAW1 = 0x33, // Vertical Start Point 1 of Active Window (VSAW1)
		HEAW0 = 0x34, // Horizontal End Point 0 of Active Window (HEAW0)
		HEAW1 = 0x35, // Horizontal End Point 1 of Active Window (HEAW1)
		VEAW0 = 0x36, // Vertical End Point of Active Window 0 (VEAW0)
		VEAW1 = 0x37, // Vertical End Point of Active Window 1 (VEAW1)
		HSSW0 = 0x38, // Horizontal Start Point 0 of Scroll Window (HSSW0)
		HSSW1 = 0x39, // Horizontal Start Point 1 of Scroll Window (HSSW1)
		VSSW0 = 0x3A, // Vertical Start Point 0 of Scroll Window (VSSW0)
		VSSW1 = 0x3B, // Vertical Start Point 1 of Scroll Window (VSSW1)
		HESW0 = 0x3C, // Horizontal End Point 0 of Scroll Window (HESW0)
		HESW1 = 0x3D, // Horizontal End Point 1 of Scroll Window (HESW1)
		VESW0 = 0x3E, // Vertical End Point 0 of Scroll Window (VESW0)
		VESW1 = 0x3F, // Vertical End Point 1 of Scroll Window (VESW1)
		MWCR0 = 0x40, // Memory Write Control Register 0 (MWCR0)
		MWCR1 = 0x41, // Memory Write Control Register1 (MWCR1)
		BTCR = 0x44, // Blink Time Control Register (BTCR)
		MRCD = 0x45, // Memory Read Cursor Direction (MRCD)
		CURH0 = 0x46, // Memory Write Cursor Horizontal Position Register 0 (CURH0)
		CURH1 = 0x47, // Memory Write Cursor Horizontal Position Register 1 (CURH1)
		CURV0 = 0x48, // Memory Write Cursor Vertical Position Register 0 (CURV0)
		CURV1 = 0x49, // Memory Write Cursor Vertical Position Register 1 (CURV1)
		RCURH0 = 0x4A, // Memory Read Cursor Horizontal Position Register 0 (RCURH0)
		RCURH1 = 0x4B, // Memory Read Cursor Horizontal Position Register 1 (RCURH1)
		RCURV0 = 0x4C, // Memory Read Cursor Vertical Position Register 0 (RCURV0)
		RCURV1 = 0x4D, // Memory Read Cursor Vertical Position Register 1 (RCURV1)
		CURHS = 0x4E, // Font Write Cursor and Memory Write Cursor Horizontal Size Register (CURHS)
		CURVS = 0x4F, // Font Write Cursor Vertical Size Register (CURVS)
		BECR0 = 0x50, // BTE Function Control Register 0 (BECR0)
		BECR1 = 0x51, // BTE Function Control Register1 (BECR1)
		LTPR0 = 0x52, // Layer Transparency Register0 (LTPR0)
		LTPR1 = 0x53, // Layer Transparency Register1 (LTPR1)
		HSBE0 = 0x54, // Horizontal Source Point 0 of BTE (HSBE0)
		HSBE1 = 0x55, // Horizontal Source Point 1 of BTE (HSBE1)
		VSBE0 = 0x56, // Vertical Source Point 0 of BTE (VSBE0)
		VSBE1 = 0x57, // Vertical Source Point 1 of BTE (VSBE1)
		HDBE0 = 0x58, // Horizontal Destination Point 0 of BTE (HDBE0)
		HDBE1 = 0x59, // Horizontal Destination Point 1 of BTE (HDBE1)
		VDBE0 = 0x5A, // Vertical Destination Point 0 of BTE (VDBE0)
		VDBE1 = 0x5B, // Vertical Destination Point 1 of BTE (VDBE1)
		BEWR0 = 0x5C, // BTE Width Register 0 (BEWR0)
		BEWR1 = 0x5D, // BTE Width Register 1 (BEWR1)
		BEHR0 = 0x5E, // BTE Height Register 0 (BEHR0)
		BEHR1 = 0x5F, // BTE Height Register 1 (BEHR1)
		BGCR0 = 0x60, // Background Color Register 0 (BGCR0)
		BGCR1 = 0x61, // Background Color Register 1 (BGCR1)
		BGCR2 = 0x62, // Background Color Register 2 (BGCR2)
		FGCR0 = 0x63, // Foreground Color Register 0 (FGCR0)
		FGCR1 = 0x64, // Foreground Color Register 1 (FGCR1)
		FGCR2 = 0x65, // Foreground Color Register 2 (FGCR2)
		PTNO = 0x66, // Pattern Set No for BTE (PTNO)
		BGTR0 = 0x67, // Background Color Register for Transparent 0 (BGTR0)
		BGTR1 = 0x68, // Background Color Register for Transparent 1 (BGTR1)
		BGTR2 = 0x69, // Background Color Register for Transparent 2 (BGTR2)
		TPCR0 = 0x70, // Touch Panel Control Register 0 (TPCR0)
		TPCR1 = 0x71, // Touch Panel Control Register 1 (TPCR1)
		TPXH = 0x72, // Touch Panel X High Byte Data Register (TPXH)
		TPYH = 0x73, // Touch Panel Y High Byte Data Register (TPYH)
		TPXYL = 0x74, // Touch Panel X/Y Low Byte Data Register (TPXYL)
		GCHP0 = 0x80, // Graphic Cursor Horizontal Position Register 0 (GCHP0)
		GCHP1 = 0x81, // Graphic Cursor Horizontal Position Register 1 (GCHP1)
		GCVP0 = 0x82, // Graphic Cursor Vertical Position Register 0 (GCVP0)
		GCVP1 = 0x83, // Graphic Cursor Vertical Position Register 1 (GCVP1)
		GCC0 = 0x84, // Graphic Cursor Color 0 (GCC0)
		GCC1 = 0x85, // Graphic Cursor Color 1 (GCC1)
		PLLC1 = 0x88, // PLL Control Register 1 (PLLC1)
		PLLC2 = 0x89, // PLL Control Register 2 (PLLC2)
		P1CR = 0x8A, // PWM1 Control Register (P1CR)
		P1DCR = 0x8B, // PWM1 Duty Cycle Register (P1DCR)
		P2CR = 0x8C, // PWM2 Control Register (P2CR)
		P2DCR = 0x8D, // PWM2 Control Register (P2DCR)
		MCLR = 0x8E, // Memory Clear Control Register (MCLR)
		DCR = 0x90, // Draw Line/Circle/Square Control Register (DCR)
		DLHSR0 = 0x91, // Draw Line/Square Horizontal Start Address Register0 (DLHSR0)
		DLHSR1 = 0x92, // Draw Line/Square Horizontal Start Address Register1 (DLHSR1)
		DLVSR0 = 0x93, // Draw Line/Square Vertical Start Address Register0 (DLVSR0)
		DLVSR1 = 0x94, // Draw Line/Square Vertical Start Address Register1 (DLVSR1)
		DLHER0 = 0x95, // Draw Line/Square Horizontal End Address Register0 (DLHER0)
		DLHER1 = 0x96, // Draw Line/Square Horizontal End Address Register1 (DLHER1)
		DLVER0 = 0x97, // Draw Line/Square Vertical End Address Register0 (DLVER0)
		DLVER1 = 0x98, // Draw Line/Square Vertical End Address Register1 (DLVER1)
		DCHR0 = 0x99, // Draw Circle Center Horizontal Address Register0 (DCHR0)
		DCHR1 = 0x9A, // Draw Circle Center Horizontal Address Register1 (DCHR1)
		DCVR0 = 0x9B, // Draw Circle Center Vertical Address Register0 (DCVR0)
		DCVR1 = 0x9C, // Draw Circle Center Vertical Address Register1 (DCVR1)
		DCRR = 0x9D, // Draw Circle Radius Register (DCRR)
		ELLIPSE = 0xA0, // Draw Ellipse/Ellipse Curve/Circle Square Control Register
		ELL_A0 = 0xA1, // Draw Ellipse/Circle Square Long axis Setting Register (ELL_A0)
		ELL_A1 = 0xA2, // Draw Ellipse/Circle Square Long axis Setting Register (ELL_A1)
		ELL_B0 = 0xA3, // Draw Ellipse/Circle Square Short axis Setting Register (ELL_B0)
		ELL_B1 = 0xA4, // Draw Ellipse/Circle Square Short axis Setting Register (ELL_B1)
		DEHR0 = 0xA5, // Draw Ellipse/Circle Square Center Horizontal Address Register0 (DEHR0)
		DEHR1 = 0xA6, // Draw Ellipse/Circle Square Center Horizontal Address Register1 (DEHR1)
		DEVR0 = 0xA7, // Draw Ellipse/Circle Square Center Vertical Address Register0 (DEVR0)
		DEVR1 = 0xA8, // Draw Ellipse/Circle Square Center Vertical Address Register1 (DEVR1)
		DTPH0 = 0xA9, // Draw Triangle Point 2 Horizontal Address Register0 (DTPH0)
		DTPH1 = 0xAA, // Draw Triangle Point 2 Horizontal Address Register1 (DTPH1)
		DTPV0 = 0xAB, // Draw Triangle Point 2 Vertical Address Register0 (DTPV0)
		DTPV1 = 0xAC, // Draw Triangle Point 2 Vertical Address Register1 (DTPV1)
		SSAR0 = 0xB0, // Source Starting Address REG0 (SSAR0)
		SSAR1 = 0xB1, // Source Starting Address REG 1 (SSAR1)
		SSAR2 = 0xB2, // Source Starting Address REG 2 (SSAR2)
		BWR0 = 0xB4, // Block Width REG 0(BWR0) / DMA Transfer Number REG 0 (DTNR0)
		BWR1 = 0xB5, // Block Width REG 1 (BWR1)
		BHR0 = 0xB6, // Block Height REG 0(BHR0) /DMA Transfer Number REG 1 (DTNR1)
		BHR1 = 0xB7, // Block Height REG 1 (BHR1)
		SPWR0 = 0xB8, // Source Picture Width REG 0(SPWR0) / DMA Transfer Number REG 2(DTNR2)
		SPWR1 = 0xB9, // Source Picture Width REG 1 (SPWR1)
		DMACR = 0xBF, // DMA Configuration REG (DMACR)
		KSCR1 = 0xC0, // Key-Scan Control Register 1 (KSCR1)
		KSCR2 = 0xC1, // Key-Scan Controller Register 2 (KSCR2)
		KSDR0 = 0xC2, // Key-Scan Data Register (KSDR0)
		KSDR1 = 0xC3, // Key-Scan Data Register (KSDR1)
		KSDR2 = 0xC4, // Key-Scan Data Register (KSDR2)
		GPIOX = 0xC7, // Extra General Purpose IO Register (GPIOX)
		FWSAXA0 = 0xD0, // Floating Windows Start Address XA 0 (FWSAXA0)
		FWSAXA1 = 0xD1, // Floating Windows Start Address XA 1 (FWSAXA1)
		FWSAYA0 = 0xD2, // Floating Windows Start Address YA 0 (FWSAYA0)
		FWSAYA1 = 0xD3, // Floating Windows Start Address YA 1 (FWSAYA1)
		FWW0 = 0xD4, // Floating Windows Width 0 (FWW0)
		FWW1 = 0xD5, // Floating Windows Width 1 (FWW1)
		FWH0 = 0xD6, // Floating Windows Height 0 (FWH0)
		FWH1 = 0xD7, // Floating Windows Height 1 (FWH1)
		FWDXA0_ = 0xD8, // Floating Windows Display X Address 0 (FWDXA0)
		FWDXA1_ = 0xD9, // Floating Windows Display X Address 1 (FWDXA1)
		FWDYA0_ = 0xDA, // Floating Windows Display Y Address 0 (FWDYA0)
		FWDYA1_ = 0xDB, // Floating Windows Display Y Address 1 (FWDYA1)
		SACS_MODE = 0xE0, // Serial Flash/ROM Direct Access Mode
		SACS_ADDR = 0xE1, // Serial Flash/ROM Direct Access Mode Address
		SACS_DATA = 0xE2, // Serial Flash/ROM Direct Access Data Read
		INTC1 = 0xF0, // Interrupt Control Register1 (INTC1)
		INTC2 = 0xF1, // Interrupt Control Register2 (INTC2)
	};


	RA8875::RA8875(IDevice& device, Pin cs, Pin rst, Pin wait, Pin interrupt)
		: m_device(&device)
		, m_spi(device, cs, 2000000, 0, false)
		, m_rst(rst)
		, m_wait(wait)
		, m_interrupt(interrupt)
		, m_width(0)
		, m_height(0)
		, m_textScale(0)
		, m_size(_800x480)
	{
		m_device->setPinDirection(m_rst, Direction::Out);
		m_device->setPinDirection(m_wait, Direction::In);
		m_device->setPinDirection(m_interrupt, Direction::In);
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

		uint8_t x = readRegister8(Register::STSR);
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
			setRegister8(Register::PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPON);
		else
			setRegister8(Register::PWRR, RA8875_PWRR_NORMAL | RA8875_PWRR_DISPOFF);
	}

	void RA8875::sleep(bool sleep) const
	{
		if (sleep)
			setRegister8(Register::PWRR, RA8875_PWRR_DISPOFF | RA8875_PWRR_SLEEP);
		else
			setRegister8(Register::PWRR, RA8875_PWRR_DISPOFF);
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
		setRegister16(Register::F_CURXL, x);
		setRegister16(Register::F_CURYL, y);
	}

	void RA8875::textColor(uint16_t foreColor, uint16_t bgColor) const
	{
		setColorRegister(Register::FGCR0, foreColor);
		setColorRegister(Register::BGCR0, bgColor);

		/* Clear transparency flag */
		uint8_t temp = readRegister8(Register::FNCR1);
		temp &= ~(1 << 6); // Clear bit 6
		writeData(temp);
	}

	void RA8875::textTransparent(uint16_t foreColor) const
	{
		/* Set Fore Color */
		setColorRegister(Register::FGCR0, foreColor);

		/* Set transparency flag */
		uint8_t temp = readRegister8(Register::FNCR1);
		temp |= (1 << 6); // Set bit 6
		writeData(temp);
	}

	void RA8875::textEnlarge(uint8_t scale)
	{
		if (scale > 3) scale = 3;

		/* Set font size flags */
		uint8_t temp = readRegister8(Register::FNCR1);
		temp &= ~(0xF); // Clears bits 0..3
		temp |= scale << 2;
		temp |= scale;
		writeData(temp);

		m_textScale = scale;
	}

	void RA8875::textWrite(const char* buffer) const
	{
		if (buffer != nullptr)
		{
			writeCommand(RA8875_MRWC);
			while (*buffer != 0)
			{
				writeData(*buffer++);
			}
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
		setRegister16(Register::CURH0, x);
		setRegister16(Register::CURV0, y);
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

	void RA8875::drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color) const
	{
		setRegister16(Register::DLHSR0, x0);
		setRegister16(Register::DLVSR0, y0);
		setRegister16(Register::DLHER0, x1);
		setRegister16(Register::DLVER0, y1);
		setColorRegister(Register::FGCR0, color);

		setRegister8(Register::DCR, 0x80);
		waitPoll(Register::DCR, RA8875_DCR_LINESQUTRI_STATUS);
	}

	void RA8875::drawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) const
	{
		rectHelper(x, y, x + w, y + h, color, false);
	}

	void RA8875::fillRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color) const
	{
		rectHelper(x, y, x + w, y + h, color, true);
	}

	void RA8875::drawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color) const
	{
		circleHelper(x0, y0, r, color, false);
	}

	void RA8875::fillCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color) const
	{
		circleHelper(x0, y0, r, color, true);
	}

	void RA8875::drawTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) const
	{
		triangleHelper(x0, y0, x1, y1, x2, y2, color, false);
	}

	void RA8875::fillTriangle(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color) const
	{
		triangleHelper(x0, y0, x1, y1, x2, y2, color, true);
	}

	void RA8875::drawEllipse(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint16_t color) const
	{
		ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, false);
	}

	void RA8875::fillEllipse(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint16_t color) const
	{
		ellipseHelper(xCenter, yCenter, longAxis, shortAxis, color, true);
	}

	void RA8875::drawCurve(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint8_t curvePart, uint16_t color) const
	{
		curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, false);
	}

	void RA8875::fillCurve(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint8_t curvePart, uint16_t color) const
	{
		curveHelper(xCenter, yCenter, longAxis, shortAxis, curvePart, color, true);
	}

	void RA8875::GPIOX(bool on) const
	{
		if (on)
			setRegister8(Register::GPIOX, 1);
		else
			setRegister8(Register::GPIOX, 0);
	}

	void RA8875::PWM1config(bool on, uint8_t clock) const
	{
		if (on)
			setRegister8(Register::P1CR, RA8875_P1CR_ENABLE | (clock & 0xF));
		else
			setRegister8(Register::P1CR, RA8875_P1CR_DISABLE | (clock & 0xF));
	}

	void RA8875::PWM2config(bool on, uint8_t clock) const
	{
		if (on)
			setRegister8(Register::P2CR, RA8875_P2CR_ENABLE | (clock & 0xF));
		else
			setRegister8(Register::P2CR, RA8875_P2CR_DISABLE | (clock & 0xF));
	}

	void RA8875::PWM1out(uint8_t p) const
	{
		setRegister8(Register::P1DCR, p);
	}

	void RA8875::PWM2out(uint8_t p) const
	{
		setRegister8(Register::P2DCR, p);
	}

	void RA8875::touchEnable(bool on) const
	{
		uint8_t   adcClk = uint8_t(RA8875_TPCR0_ADCCLK_DIV4);

		if (m_size == _800x480) // match up touch size with LCD size
			adcClk = uint8_t(RA8875_TPCR0_ADCCLK_DIV16);

		if (on)
		{
			/* Enable Touch Panel (Reg 0x70) */
			setRegister8(Register::TPCR0, RA8875_TPCR0_ENABLE | RA8875_TPCR0_WAIT_4096CLK | RA8875_TPCR0_WAKEENABLE | adcClk); // 10mhz max!
			/* Set Auto Mode      (Reg 0x71) */
			setRegister8(Register::TPCR1, RA8875_TPCR1_AUTO | RA8875_TPCR1_DEBOUNCE);
			/* Enable TP INT */
			setRegister8(Register::INTC1, readRegister8(Register::INTC1) | RA8875_INTC1_TP);
		}
		else
		{
			/* Disable TP INT */
			setRegister8(Register::INTC1, readRegister8(Register::INTC1) & ~RA8875_INTC1_TP);
			/* Disable Touch Panel (Reg 0x70) */
			setRegister8(Register::TPCR0, RA8875_TPCR0_DISABLE);
		}
	}

	bool RA8875::touched() const
	{
		return (readRegister8(Register::INTC2) & RA8875_INTC2_TP) != 0;
	}

	bool RA8875::touchRead(uint16_t *x, uint16_t *y) const
	{
		uint16_t tx, ty;
		uint8_t temp;

		tx = readRegister8(Register::TPXH);
		ty = readRegister8(Register::TPYH);
		temp = readRegister8(Register::TPXYL);
		tx <<= 2;
		ty <<= 2;
		tx |= temp & 0x03;        // get the bottom x bits
		ty |= (temp >> 2) & 0x03; // get the bottom y bits

		*x = tx;
		*y = ty;

		/* Clear TP INT Status */
		setRegister8(Register::INTC2, RA8875_INTC2_TP);
		return true;
	}

	void RA8875::setRegister8(Register reg, uint8_t val) const
	{
		writeCommand(uint8_t(reg));
		writeData(val);
	}

	void RA8875::setRegister16(Register reg, uint16_t val) const
	{
		writeCommand(uint8_t(reg) + 0);
		writeData(uint8_t(val & 0xFF));
		writeCommand(uint8_t(reg) + 1);
		writeData(uint8_t(val >> 8));
	}

	void RA8875::setColorRegister(Register reg, uint16_t color) const
	{
		writeCommand(uint8_t(reg) + 0);
		writeData(uint8_t((color & 0xf800) >> 11));
		writeCommand(uint8_t(reg) + 1);
		writeData(uint8_t((color & 0x07e0) >> 5));
		writeCommand(uint8_t(reg) + 2);
		writeData(uint8_t(color & 0x001f));
	}

	uint8_t RA8875::readRegister8(Register reg) const
	{
		writeCommand(uint8_t(reg));
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

	bool RA8875::waitPoll(Register reg, uint8_t f) const
	{
		while (1)
		{
			uint8_t temp = readRegister8(reg);
			if ((temp & f) == 0)
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
			setRegister8(Register::PLLC1, RA8875_PLLC1_PLLDIV1 + 10);
			delay(1);
			setRegister8(Register::PLLC2, RA8875_PLLC2_DIV4);
			delay(1);
		}
		else /* if (m_size == _800x480) */
		{
			setRegister8(Register::PLLC1, RA8875_PLLC1_PLLDIV1 + 10);
			delay(1);
			setRegister8(Register::PLLC2, RA8875_PLLC2_DIV4);
			delay(1);
		}
	}

	void RA8875::initialize() const
	{
		PLLinit();
		setRegister8(Register::SYSR, RA8875_SYSR_16BPP | RA8875_SYSR_MCU8);

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

		setRegister8(Register::PCSR, pixclk);
		delay(1);

		/* Horizontal settings registers */
		setRegister8(Register::HDWR, uint8_t((m_width / 8) - 1));                 // H width: (HDWR + 1) * 8 = 480
		setRegister8(Register::HNDFTR, RA8875_HNDFTR_DE_HIGH + hsync_finetune);
		setRegister8(Register::HNDR, (hsync_nondisp - hsync_finetune - 2) / 8);   // H non-display: HNDR * 8 + HNDFTR + 2 = 10
		setRegister8(Register::HSTR, hsync_start / 8 - 1);                        // Hsync start: (HSTR + 1)*8 
		setRegister8(Register::HPWR, RA8875_HPWR_LOW + (hsync_pw / 8 - 1));       // HSync pulse width = (HPWR+1) * 8

		/* Vertical settings registers */
		setRegister16(Register::VDHR0, m_height - 1);
		setRegister16(Register::VNDR0, vsync_nondisp - 1);                        // V non-display period = VNDR + 1
		setRegister16(Register::VSTR0, vsync_start - 1);                          // Vsync start position = VSTR + 1
		setRegister8(Register::VPWR, RA8875_VPWR_LOW + vsync_pw - 1);             // Vsync pulse width = VPWR + 1

		/* Set active window X */
		setRegister16(Register::HSAW0, 0);                                        // horizontal start point
		setRegister16(Register::HEAW0, m_width - 1);                              // horizontal end point

		/* Set active window Y */
		setRegister16(Register::VSAW0, 0);                                        // vertical start point
		setRegister16(Register::VEAW0, m_height - 1);                             // vertical end point

		/* ToDo: Setup touch panel? */

		/* Clear the entire window */
		setRegister8(Register::MCLR, RA8875_MCLR_START | RA8875_MCLR_FULL);
		delay(500);
	}

	void RA8875::circleHelper(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color, bool filled) const
	{
		setRegister16(Register::DCHR0, x0);
		setRegister16(Register::DCVR0, y0);
		setRegister8(Register::DCRR, r);
		setColorRegister(Register::FGCR0, color);

		/* Draw! */
		setRegister8(Register::DCR, RA8875_DCR_CIRCLE_START | (filled ? RA8875_DCR_FILL : RA8875_DCR_NOFILL));

		/* Wait for the command to finish */
		waitPoll(Register::DCR, RA8875_DCR_CIRCLE_STATUS);
	}

	void RA8875::rectHelper(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color, bool filled) const
	{
		setRegister16(Register::DLHSR0, x);
		setRegister16(Register::DLVSR0, y);
		setRegister16(Register::DLHER0, w);
		setRegister16(Register::DLVER0, h);
		setColorRegister(Register::FGCR0, color);

		/* Draw! */
		setRegister8(Register::DCR, filled ? 0xB0 : 0x90);

		/* Wait for the command to finish */
		waitPoll(Register::DCR, RA8875_DCR_LINESQUTRI_STATUS);
	}

	void RA8875::triangleHelper(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color, bool filled) const
	{
		setRegister16(Register::DLHSR0, x0);
		setRegister16(Register::DLVSR0, y0);
		setRegister16(Register::DLHER0, x1);
		setRegister16(Register::DLVER0, y1);
		setRegister16(Register::DTPH0, x2);
		setRegister16(Register::DTPV0, y2);
		setColorRegister(Register::FGCR0, color);

		/* Draw! */
		setRegister8(Register::DCR, filled ? 0xA1 : 0x81);

		/* Wait for the command to finish */
		waitPoll(Register::DCR, RA8875_DCR_LINESQUTRI_STATUS);
	}

	void RA8875::ellipseHelper(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint16_t color, bool filled) const
	{
		setRegister16(Register::DEHR0, xCenter);
		setRegister16(Register::DEVR0, yCenter);
		setRegister16(Register::ELL_A0, longAxis);
		setRegister16(Register::ELL_B0, shortAxis);
		setColorRegister(Register::FGCR0, color);

		/* Draw! */
		setRegister8(Register::ELLIPSE, filled ? 0xC0 : 0x80);

		/* Wait for the command to finish */
		waitPoll(Register::ELLIPSE, RA8875_ELLIPSE_STATUS);
	}

	void RA8875::curveHelper(uint16_t xCenter, uint16_t yCenter, uint16_t longAxis, uint16_t shortAxis, uint8_t curvePart, uint16_t color, bool filled) const
	{
		setRegister16(Register::DEHR0, xCenter);
		setRegister16(Register::DEVR0, yCenter);
		setRegister16(Register::ELL_A0, longAxis);
		setRegister16(Register::ELL_B0, shortAxis);
		setColorRegister(Register::FGCR0, color);

		/* Draw! */
		setRegister8(Register::ELLIPSE, (filled ? 0xD0 : 0x90) | (curvePart & 0x03));

		/* Wait for the command to finish */
		waitPoll(Register::ELLIPSE, RA8875_ELLIPSE_STATUS);
	}


	void RA8875::delay(int ms)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(ms));
	}
}
