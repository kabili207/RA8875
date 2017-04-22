#include "SPI.h"
#include <stdio.h>
#include <ftdi.h>

#include <chrono>
#include <thread>
#include <iso646.h>
#include <cassert>

namespace hw
{
	SPI::SPI(IDevice& device, Pin cs, int max_speed_hz, int mode, bool lsbFirst)
		: m_device(&device)
		, m_cs(cs)
		, m_flags(0)
	{
		// D0=clock(output), D1=MOSI(output), D1=MISO(input)
		m_device->setPinDirection(Pin::D0, Direction::Out);
		m_device->setPinDirection(Pin::D1, Direction::Out);
		m_device->setPinDirection(Pin::D2, Direction::In);

		// Initialize chip select pin if provided to output high.
		m_device->setPinDirection(cs, Direction::Out);
		m_device->setHigh(m_cs);

		// Initialize clock, mode, and bit order.
		setClock(max_speed_hz);
		setMode(mode);
		setBitOrder(lsbFirst);
	}

	///
	/// Set the speed of the SPI clock in hertz.  Note that not all speeds
	/// are supported and a lower speed might be chosen by the hardware.
	///
	void SPI::setClock(int hz) const
	{
		m_device->setClock(hz);
	}


	///
	/// Set SPI mode which controls clock polarity and phase.  Should be a
	/// numeric value 0, 1, 2, or 3.  See wikipedia page for details on meaning:
	/// http://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus
	///
	void SPI::setMode(int mode)
	{
		switch (mode)
		{
		case 0: 
			// Mode 0 captures on rising clock, propagates on falling clock
			m_flags = (m_flags & ~0x5) | MPSSE_WRITE_NEG;
			m_device->setLow(Pin::D0);
			break;
		case 1:
			// Mode 1 capture of falling edge, propagate on rising clock
			m_flags = (m_flags & ~0x5) | MPSSE_READ_NEG;
			m_device->setLow(Pin::D0);
			break;
		case 2:
			// Mode 2 capture on rising clock, propagate on falling clock
			m_flags = (m_flags & ~0x5) | MPSSE_WRITE_NEG;
			m_device->setHigh(Pin::D0);
			break;
		case 3:
			// Mode 3 capture on falling edge, propagage on rising clock
			m_flags = (m_flags & ~0x5) | MPSSE_READ_NEG;
			m_device->setHigh(Pin::D0);
			break;
		default:
			assert(false);
		}
	}


	/// 
	/// Set order of bits to be read/written over serial lines.  Should be
	/// either MSBFIRST for most - significant first, or LSBFIRST for
	/// least - signifcant first.
	/// 
	void SPI::setBitOrder(bool lsbFirst)
	{
		if (lsbFirst)
		{
			m_flags |= MPSSE_LSB;
		}
		else
		{
			m_flags &= ~MPSSE_LSB;
		}
	}

	/// 
	/// Half-duplex SPI write.  The specified array of bytes will be clocked
	/// out the MOSI line.
	/// 
	void SPI::write(const uint8_t* data, uint16_t length) const
	{
		m_device->setLow(m_cs);
		m_device->writeByte(MPSSE_DO_WRITE | m_flags);
		m_device->writeUInt16(length-1);
		m_device->write(data, length);
		m_device->writeByte(0x87);
		m_device->setHigh(m_cs);
	}

	/// 
	/// Half-duplex SPI read.  The specified length of bytes will be clocked
	/// in the MISO line and returned as a bytearray object.
	/// 
	int SPI::read(uint8_t* data, uint16_t length) const
	{
		m_device->setLow(m_cs);
		m_device->writeByte(MPSSE_DO_READ | m_flags);
		m_device->writeUInt16(length - 1);
		m_device->writeByte(0x87);
		m_device->setHigh(m_cs);

		return m_device->read(data, length);
	}

	/// 
	/// Full-duplex SPI read and write.  The specified array of bytes will be
	/// clocked out the MOSI line, while simultaneously bytes will be read from
	/// the MISO line.Read bytes will be returned as a bytearray object.
	///
	int SPI::transfer(const uint8_t* output, uint8_t* response, uint16_t length) const
	{
		m_device->setLow(m_cs);
		m_device->writeByte(MPSSE_DO_WRITE | MPSSE_DO_READ | m_flags);
		m_device->writeUInt16(length - 1);
		m_device->write(output, length);
		m_device->writeByte(0x87);
		m_device->setHigh(m_cs);

		return m_device->read(response, length);
	}
}
