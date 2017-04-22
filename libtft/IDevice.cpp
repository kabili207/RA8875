#include "IDevice.h"

namespace hw
{
	int IDevice::writeByte(uint8_t data)
	{
		return write(&data, sizeof(data));
	}

	int IDevice::writeUInt16(uint16_t data)
	{
		return write(reinterpret_cast<const uint8_t*>(&data), sizeof(data));
	}

	int IDevice::writeList(const std::initializer_list<uint8_t>& list)
	{
		return write(list.begin(), list.size());
	}
}
