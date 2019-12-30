/**
 * @file zclframecontrol.cpp
 *
 * @brief Handles encoding/decoding of the ZCL frame control byte
 */

#include "ezsp/zbmessage/zclframecontrol.h"


CZCLFrameControl::CZCLFrameControl() :
	frame_type(E_FRM_TYPE_SPECIFIC),
	manufacturer_code_present(true),
	direction(E_DIR_CLIENT_TO_SERVER),
	disable_default_rsp(true),
	software_code(E_SW_CODE_SHORT)
{
}

CZCLFrameControl::CZCLFrameControl(uint8_t i_byte) :
	frame_type(static_cast<EZCLFrameCtrlFrameType>(i_byte & 0x03)),
	manufacturer_code_present(static_cast<uint8_t>(i_byte>>2) & 0x01),
	direction(static_cast<EZCLFrameCtrlDirection>(static_cast<uint8_t>(i_byte>>3) & 0x01)),
	disable_default_rsp(static_cast<uint8_t>(i_byte>>4) & 0x01),
	software_code(static_cast<EZCLFrameCtrlSoftwareCode>(static_cast<uint8_t>(i_byte>>5) & 0x07))
{
}

CZCLFrameControl::CZCLFrameControl(const CZCLFrameControl& other) :
	frame_type(other.frame_type),
	manufacturer_code_present(other.manufacturer_code_present),
	direction(other.direction),
	disable_default_rsp(other.disable_default_rsp),
	software_code(other.software_code)
{
}

uint8_t CZCLFrameControl::GetFrmCtrlByte() const
{
  uint8_t lo_byte = 0;

  lo_byte = frame_type & 0x03;
  if (manufacturer_code_present)
      lo_byte |= 0x04;
  if (direction == E_DIR_SERVER_TO_CLIENT)
      lo_byte |= 0x08;
  if (disable_default_rsp)
      lo_byte |= 0x10;
  if (software_code == E_SW_CODE_EVO2)
      lo_byte |= 0x20;

  return lo_byte;
}

/**
 * This method is a friend of CZCLFrameControl class
 * swap() is needed within operator=() to implement to copy and swap paradigm
**/
void swap(CZCLFrameControl& first, CZCLFrameControl& second) /* nothrow */
{
  using std::swap;	// Enable ADL

  swap(first.frame_type, second.frame_type);
  swap(first.manufacturer_code_present, second.manufacturer_code_present);
  swap(first.direction, second.direction);
  swap(first.disable_default_rsp, second.disable_default_rsp);
  swap(first.software_code, second.software_code);
  /* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}

CZCLFrameControl& CZCLFrameControl::operator=(CZCLFrameControl other) {
  swap(*this, other);
  return *this;
}
