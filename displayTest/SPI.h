#pragma once

#include "IDevice.h"

namespace hw
{
	class SPI
	{
	public:
		SPI(IDevice& device, Pin cs, int max_speed_hz = 1000000, int mode = 0, bool lsbFirst = false);

		void setClock(int hz) const;
		void setMode(int mode);
		void setBitOrder(bool lsbFirst);

		void write(const uint8_t* data, uint16_t length) const;
		int  read(uint8_t* data, uint16_t length) const;
		int  transfer(const uint8_t* output, uint8_t* response, uint16_t length) const;

	private:
		IDevice*    m_device;
		Pin         m_cs;
		int         m_write_clock_ve;
		int         m_read_clock_ve;
		bool        m_clock_base;
		int         m_lsbfirst;
	};
}
