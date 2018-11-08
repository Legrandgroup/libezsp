#ifndef ZCLFRAMECONTROL_H
#define ZCLFRAMECONTROL_H

#include <cstdint>
#include <vector>

typedef enum
{
  E_DIR_CLIENT_TO_SERVER = 0,
  E_DIR_SERVER_TO_CLIENT = 1
}EZCLFrameCtrlDirection;

typedef enum
{
  E_FRM_TYPE_GENERAL = 0,
  E_FRM_TYPE_SPECIFIC = 1
}EZCLFrameCtrlFrameType;

typedef enum
{
  E_SW_CODE_SHORT,
  E_SW_CODE_EVO2
}EZCLFrameCtrlSoftwareCode;

class CZCLFrameControl
{
public:
  CZCLFrameControl();
  CZCLFrameControl( uint8_t i_byte );

  // direction
  EZCLFrameCtrlDirection GetDirection(void) { return direction; }
  void SetDirection( EZCLFrameCtrlDirection i_direction ) { direction = i_direction; }

  // default response
  bool GetDisableDefaultRspState(void) { return disable_default_rsp; }
  void SetDisableDefaultRspState( bool i_disable ) {disable_default_rsp = i_disable; }

  // frame type
  EZCLFrameCtrlFrameType GetFrmType(void) { return frame_type; }
  void SetFrameType( EZCLFrameCtrlFrameType i_type ) { frame_type = i_type; }

  // manufacturer code present
  bool IsManufacturerCodePresent(void) { return manufacturer_code_present; }
  void SetManufacturerCodePresent( bool i_present ) { manufacturer_code_present = i_present; }

  // software version
  EZCLFrameCtrlSoftwareCode GetSwVersion( void ) { return software_code; }
  void SetSwVersion( EZCLFrameCtrlSoftwareCode i_version ) { software_code = i_version; }

  // concatenate
  uint8_t GetFrmCtrlByte();
  void SetFrmCtrlByte( uint8_t i_byte );
  void SetFrmCtrl( CZCLFrameControl i_frm_ctrl ) { SetFrmCtrlByte( i_frm_ctrl.GetFrmCtrlByte() ); }

private:
  /** direction of message */
  EZCLFrameCtrlDirection direction;
  /** default response to message, 0:default rsp return, 1:not return (default state for MSP) */
  bool disable_default_rsp;
  /** type of frame, cluster specific or general */
  EZCLFrameCtrlFrameType frame_type;
  /** manufacturer code present in the frame (1) or not (0) */
  bool manufacturer_code_present;
  /** software version indication, MSP : 0 for short frame (no deviceid and battery fields), 1 for long frame (generation 2.1) */
  EZCLFrameCtrlSoftwareCode software_code;
};

#endif // ZCLFRAMECONTROL_H
