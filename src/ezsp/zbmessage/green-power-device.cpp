/**
 * @file green-power-device.cpp
 *
 * @brief Represents data for a green power device
 */

#include "green-power-device.h"

CGpDevice::CGpDevice(uint32_t i_source_id, const EmberKeyData& i_key) :
    source_id(i_source_id),
	key(i_key),
	option(0x02A8),
	security_option(0x12)
{
}

/**
 * This method is a friend of CEmberGpSinkTableOption class
 * swap() is needed within operator=() to implement to copy and swap paradigm
**/
void swap(CGpDevice& first, CGpDevice& second) /* nothrow */
{
  using std::swap;	// Enable ADL

  swap(first.source_id, second.source_id);
  swap(first.key, second.key);
  swap(first.option, second.option);
  swap(first.security_option, second.security_option);
  /* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}

CGpDevice& CGpDevice::operator=( CGpDevice other)
{
  swap(*this, other);
  return *this;
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
