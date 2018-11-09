/**
 * 
 */

#include "zclheader.h"


CZCLHeader::CZCLHeader()
{
  manufacturer_code = LG_MAN_CODE;
  transaction_number = 0;
  cmd_id = 0;
}

/**
 * @brief SetMSPSpecific : build default msp cluster specific ZCL header
 * @param i_cmd_id       : command identifier
 * @param i_direction    : model direction
 */
void CZCLHeader::SetMSPSpecific( uint16_t i_profile_id, uint8_t i_cmd_id, EZCLFrameCtrlDirection i_direction, uint8_t i_transaction_number )
{
  cmd_id = i_cmd_id;
  frm_ctrl.SetDirection( i_direction );
  transaction_number = i_transaction_number;
}

/**
 * @brief SetMSPGeneral  : build default msp cluster general ZCL header
 * @param i_profile_id
 * @param i_cmd_id       : command identifier
 * @param i_direction    : model direction
 * @param i_transaction_number
 */
void CZCLHeader::SetMSPGeneral( uint16_t i_profile_id, uint8_t i_cmd_id,
                                EZCLFrameCtrlDirection i_direction, uint8_t i_transaction_number )
{
  cmd_id = i_cmd_id;
  frm_ctrl.SetDirection( i_direction );
  frm_ctrl.SetFrameType( E_FRM_TYPE_GENERAL );
  transaction_number = i_transaction_number;
}

/**
 * @brief SetPublicSpecific : build default public cluster specific ZCL header
 * @param i_cmd_id       : command identifier
 * @param i_direction    : model direction
 */
void CZCLHeader::SetPublicSpecific( uint16_t i_manufacturer_id, uint8_t i_cmd_id, EZCLFrameCtrlDirection i_direction, uint8_t i_transaction_number )
{
  cmd_id = i_cmd_id;
  frm_ctrl.SetDirection( i_direction );
  transaction_number = i_transaction_number;
  if( 0xFFFFU == i_manufacturer_id )
  {
    frm_ctrl.SetManufacturerCodePresent( false );
  }
  manufacturer_code = i_manufacturer_id;
}

/**
 * @brief SetPublicGeneral  : build default public cluster general ZCL header
 * @param i_manufacturer_id
 * @param i_cmd_id       : command identifier
 * @param i_direction    : model direction
 * @param i_transaction_number
 */
void CZCLHeader::SetPublicGeneral( uint16_t i_manufacturer_id, uint8_t i_cmd_id,
                                EZCLFrameCtrlDirection i_direction, uint8_t i_transaction_number )
{
  cmd_id = i_cmd_id;
  frm_ctrl.SetDirection( i_direction );
  frm_ctrl.SetFrameType( E_FRM_TYPE_GENERAL );
  transaction_number = i_transaction_number;
  if( 0xFFFFU == i_manufacturer_id )
  {
    frm_ctrl.SetManufacturerCodePresent( false );
  }
  manufacturer_code = i_manufacturer_id;
}

std::vector<uint8_t> CZCLHeader::GetZCLHeader(void)
{
  std::vector<uint8_t> lo_data;

  lo_data.push_back(frm_ctrl.GetFrmCtrlByte());
  if( frm_ctrl.IsManufacturerCodePresent() )
  {
    lo_data.push_back( (uint8_t)(manufacturer_code&0xFF) );
    lo_data.push_back( (uint8_t)((manufacturer_code>>8)&0xFF) );
  }
  lo_data.push_back(transaction_number);
  lo_data.push_back(cmd_id);

  return lo_data;
}

uint8_t CZCLHeader::SetZCLHeader(std::vector<uint8_t> i_data )
{
  uint8_t l_idx = 0;
  frm_ctrl.SetFrmCtrlByte(i_data.at(l_idx)&0xFF);
  l_idx += 1;
  if( frm_ctrl.IsManufacturerCodePresent() )
  {
    manufacturer_code = i_data.at(l_idx)&0xFF;
    l_idx += 1;
    manufacturer_code |= ((i_data.at(l_idx)&0xFF)<<8);
    l_idx += 1;
  }
  transaction_number = i_data.at(l_idx)&0xFF;
  l_idx += 1;
  cmd_id = i_data.at(l_idx)&0xFF;
  l_idx += 1;

  return l_idx;
}

