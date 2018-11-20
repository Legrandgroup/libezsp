/**
 * 
 */

#include "zclframecontrol.h"


CZCLFrameControl::CZCLFrameControl()
{
  /** direction of message */
  direction = E_DIR_CLIENT_TO_SERVER;
  /** default response to message, 0:default rsp return, 1:not return (default state for MSP) */
  disable_default_rsp = true;
  /** type of frame, cluster specific or general */
  frame_type = E_FRM_TYPE_SPECIFIC;
  /** manufacturer code present in the frame (1) or not (0) */
  manufacturer_code_present = true;
  /** software version indication, MSP : 0 for short frame (no deviceid and battery fields), 1 for long frame (generation 2.1) */
  software_code = E_SW_CODE_SHORT; //E_SW_CODE_EVO2;
}

CZCLFrameControl::CZCLFrameControl(uint8_t i_byte)
{
  SetFrmCtrlByte(i_byte);
}

uint8_t CZCLFrameControl::GetFrmCtrlByte()
{
  uint8_t lo_byte = 0;

  if (frame_type == E_FRM_TYPE_SPECIFIC)
      lo_byte |= 0x03;
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

void CZCLFrameControl::SetFrmCtrlByte( uint8_t i_byte )
{
  frame_type = static_cast<EZCLFrameCtrlFrameType>(i_byte & 0x03);
  manufacturer_code_present = (i_byte>>2) & 0x01;
  direction = static_cast<EZCLFrameCtrlDirection>((i_byte>>3) & 0x01);
  disable_default_rsp = (i_byte>>4) & 0x01;
  software_code = static_cast<EZCLFrameCtrlSoftwareCode>((i_byte>>5) & 0x07);
}

