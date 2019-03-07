/**
 * @file zclframecontrol.h
 *
 * @brief Handles encoding/decoding of the ZCL frame control byte
 */

#pragma once

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
  E_SW_CODE_SHORT = 0,
  E_SW_CODE_EVO2 = 1
}EZCLFrameCtrlSoftwareCode;

class CZCLFrameControl; /* Forward declaration */
void swap(CZCLFrameControl& first, CZCLFrameControl& second); /* Declaration before qualifying ::swap() as friend for class CZCLFrameControl */

class CZCLFrameControl
{
public:
  /**
   * @brief Default constructor
   */
  CZCLFrameControl();

  /**
   * @brief Construction from a byte
   *
   * @param i_byte The byte to construct from
   */
  CZCLFrameControl( uint8_t i_byte );

  /**
   * @brief Copy constructor
   *
   * @param other The object to copy from
   */
  CZCLFrameControl(const CZCLFrameControl& other);

  /**
   * @brief Assignment operator
   * @param other The object to assign to the lhs
   *
   * @return The object that has been assigned the value of \p other
   */
  CZCLFrameControl& operator=(CZCLFrameControl other);

  /**
   * \brief swap function to allow implementing of copy-and-swap idiom on members of type CZCLFrameControl
   *
   * This function will swap all attributes of \p first and \p second
   * See http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
   *
   * @param first The first object
   * @param second The second object
   */
  friend void (::swap)(CZCLFrameControl& first, CZCLFrameControl& second);

  // direction
  EZCLFrameCtrlDirection GetDirection(void) const { return direction; }
  void SetDirection( const EZCLFrameCtrlDirection i_direction ) { direction = i_direction; }

  // default response
  bool GetDisableDefaultRspState(void) const { return disable_default_rsp; }
  void SetDisableDefaultRspState( const bool i_disable ) {disable_default_rsp = i_disable; }

  // frame type
  EZCLFrameCtrlFrameType GetFrmType(void) const { return frame_type; }
  void SetFrameType( const EZCLFrameCtrlFrameType i_type ) { frame_type = i_type; }

  // manufacturer code present
  bool IsManufacturerCodePresent(void) const { return manufacturer_code_present; }
  void SetManufacturerCodePresent( const bool i_present ) { manufacturer_code_present = i_present; }

  // software version
  EZCLFrameCtrlSoftwareCode GetSwVersion( void ) const { return software_code; }
  void SetSwVersion( const EZCLFrameCtrlSoftwareCode i_version ) { software_code = i_version; }

  // concatenate
  uint8_t GetFrmCtrlByte() const;

private:
  /** type of frame, cluster specific or general */
  EZCLFrameCtrlFrameType frame_type;
  /** manufacturer code present in the frame (1) or not (0) */
  bool manufacturer_code_present;
  /** direction of message */
  EZCLFrameCtrlDirection direction;
  /** default response to message, 0:default rsp return, 1:not return (default state for MSP) */
  bool disable_default_rsp;
  /** software version indication, MSP : 0 for short frame (no deviceid and battery fields), 1 for long frame (generation 2.1) */
  EZCLFrameCtrlSoftwareCode software_code;
};
