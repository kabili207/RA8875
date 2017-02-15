#include "SPI.h"
#include <stdio.h>
#include <ftdi.h>

#include <chrono>
#include <thread>
#include <iso646.h>

namespace hw
{
	SPI::SPI(IDevice& device, Pin cs, int max_speed_hz, int mode, bool lsbFirst)
		: m_device(&device)
		, m_cs(cs)
		, m_write_clock_ve()
		, m_read_clock_ve()
		, m_clock_base()
		, m_lsbfirst(0)
	{
		// Initialize chip select pin if provided to output high.
		m_device->setPinDirection(cs, Direction::Out);
		m_device->setHigh(m_cs);

		// Initialize clock, mode, and bit order.
		set_clock_hz(max_speed_hz);
		set_mode(mode);
		set_bit_order(lsbFirst);
	}

	SPI::~SPI()
	{
	}

	///
	/// Set the speed of the SPI clock in hertz.  Note that not all speeds
	/// are supported and a lower speed might be chosen by the hardware.
	///
	void SPI::set_clock_hz(int hz)
	{
		m_device->setClock(hz);
	}


	///
	/// Set SPI mode which controls clock polarity and phase.  Should be a
	/// numeric value 0, 1, 2, or 3.  See wikipedia page for details on meaning:
	/// http://en.wikipedia.org/wiki/Serial_Peripheral_Interface_Bus
	///
	void SPI::set_mode(int mode)
	{
		// Set clock and DO as output, DI as input.
		m_device->setPinDirection(Pin::D0, Direction::Out);
		m_device->setPinDirection(Pin::D1, Direction::Out);
		m_device->setPinDirection(Pin::D2, Direction::In);

		if (mode == 0)
		{
			// Mode 0 captures on rising clock, propagates on falling clock
			m_write_clock_ve = 1;
			m_read_clock_ve = 0;
			// Clock base is low.
			m_device->setLow(Pin::D0);
		}
		else if (mode == 1)
		{
			// Mode 1 capture of falling edge, propagate on rising clock
			m_write_clock_ve = 0;
			m_read_clock_ve = 1;
			// Clock base is low.
			m_device->setLow(Pin::D0);
		}
		else if (mode == 2)
		{
			// Mode 2 capture on rising clock, propagate on falling clock
			m_write_clock_ve = 1;
			m_read_clock_ve = 0;
			// Clock base is high.
			m_device->setHigh(Pin::D0);
		}
		else if (mode == 3)
		{
			// Mode 3 capture on falling edge, propagage on rising clock
			m_write_clock_ve = 0;
			m_read_clock_ve = 1;
			// Clock base is high.
			m_device->setHigh(Pin::D0);
		}
	}


	/// 
	/// Set order of bits to be read/written over serial lines.  Should be
	/// either MSBFIRST for most - significant first, or LSBFIRST for
	/// least - signifcant first.
	/// 
	void SPI::set_bit_order(bool lsbFirst)
	{
		m_lsbfirst = lsbFirst ? 1 : 0;
	}

	/// 
	/// Half-duplex SPI write.  The specified array of bytes will be clocked
	/// out the MOSI line.
	/// 
	void SPI::write(const uint8_t* data, uint16_t length)
	{
		// Build command to read and write SPI data.
		uint8_t command = uint8_t(0x10 | (m_lsbfirst << 3) | m_write_clock_ve);

		// execute.
		m_device->setLow(m_cs);
		m_device->writeByte(command);
		m_device->writeUInt16(length-1);
		m_device->write(data, length);
		m_device->writeByte(0x87);
		m_device->setHigh(m_cs);
	}

	/// 
	/// Half-duplex SPI read.  The specified length of bytes will be clocked
	/// in the MISO line and returned as a bytearray object.
	/// 
	int SPI::read(uint8_t* data, uint16_t length)
	{
		// Build command to read and write SPI data.
		uint8_t command = uint8_t(0x20 | (m_lsbfirst << 3) | (m_read_clock_ve << 2));

		// execute.
		m_device->setLow(m_cs);
		m_device->writeByte(command);
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
	int SPI::transfer(const uint8_t* output, uint8_t* response, uint16_t length)
	{
		// Build command to read and write SPI data.
		uint8_t command = uint8_t(0x30 | (m_lsbfirst << 3) | (m_read_clock_ve << 2) | m_write_clock_ve);

		// execute.
		m_device->setLow(m_cs);
		m_device->writeByte(command);
		m_device->writeUInt16(length - 1);
		m_device->write(output, length);
		m_device->writeByte(0x87);
		m_device->setHigh(m_cs);

		return m_device->read(response, length);
	}
}