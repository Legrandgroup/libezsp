#pragma once

#include <cstdint>
#include <vector>

#include "zclframecontrol.h"

#define LG_MAN_CODE 0x1021

class CZCLHeader
{
public:
  CZCLHeader();
  CZCLHeader( std::vector<uint8_t> i_data ) { SetZCLHeader(i_data); }

  // high level

  /**
   * @brief SetMSPSpecific : build default msp cluster specific ZCL header
   * @param i_cmd_id       : command identifier
   * @param i_direction    : model direction
   */
  void SetMSPSpecific( uint16_t i_profile_id, uint8_t i_cmd_id,
                         EZCLFrameCtrlDirection i_direction, uint8_t i_transaction_number = 0 );

  /**
   * @brief SetMSPGeneral  : build default msp cluster general ZCL header
   * @param i_profile_id
   * @param i_cmd_id       : command identifier
   * @param i_direction    : model direction
   * @param i_transaction_number
   */
  void SetMSPGeneral( uint16_t i_profile_id, uint8_t i_cmd_id,
                        EZCLFrameCtrlDirection i_direction, uint8_t i_transaction_number = 0 );

  /**
   * @brief SetPublicSpecific : build default public cluster specific ZCL header
   * @param i_cmd_id       : command identifier
   * @param i_direction    : model direction
   */
  void SetPublicSpecific( uint16_t i_manufacturer_id, uint8_t i_cmd_id,
                                      EZCLFrameCtrlDirection i_direction, uint8_t i_transaction_number );

  /**
   * @brief SetPublicGeneral  : build default public cluster general ZCL header
   * @param i_manufacturer_id
   * @param i_cmd_id       : command identifier
   * @param i_direction    : model direction
   * @param i_transaction_number
   */
  void SetPublicGeneral(uint16_t i_manufacturer_id, uint8_t i_cmd_id,
                                  EZCLFrameCtrlDirection i_direction, uint8_t i_transaction_number );

  // frame control
  CZCLFrameControl *GetFrmCtrl(void) { return &frm_ctrl; }
  void SetFrameControl( CZCLFrameControl i_frm_ctrl ) { frm_ctrl.SetFrmCtrl( i_frm_ctrl ); }

  // manufacturer code
  uint16_t GetManCode(void) { return manufacturer_code; }
  void SetManCode( uint16_t i_code ) { manufacturer_code = i_code; }

  // transaction number
  uint8_t GetTransactionNb(void) { return transaction_number; }
  void SetTransactionNb( uint8_t i_tr_nb ) { transaction_number = i_tr_nb; }

  // command id
  uint8_t GetCmdId( void ) { return cmd_id; }
  void SetCmdId( uint8_t i_cmd_id ) { cmd_id = i_cmd_id; }

  // concatenate
  std::vector<uint8_t> GetZCLHeader(void);
  /**
   * @brief SetZCLHeader : parse zcl header from data buffer
   * @param i_data : data buffer
   * @return size of header in byte
   */
  uint8_t SetZCLHeader( std::vector<uint8_t> i_data );


private:
  /** */
  uint8_t cmd_id;
  /** */
  CZCLFrameControl frm_ctrl;
  /** legrand : 0x1021 */
  uint16_t manufacturer_code;
  /** */
  uint8_t transaction_number;
};
