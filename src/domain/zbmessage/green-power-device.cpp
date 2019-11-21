/**
 * @file green-power-device.cpp
 *
 * @brief Represents data for a green power device
 */

#include "green-power-device.h"

CGpDevice::CGpDevice(uint32_t i_source_id, const EmberKeyData& i_key) :
    source_id(i_source_id),
	key(i_key)
{
}

uint32_t CGpDevice::getSourceId() const
{
	return this->source_id;
}

EmberKeyData CGpDevice::getKey() const
{
	return this->key;
}

const EmberKeyData CGpDevice::UNKNOWN_KEY({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
