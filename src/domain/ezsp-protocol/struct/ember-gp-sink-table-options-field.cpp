
/**
* @brief Details of options bit filed for sink table entry
*/

#include "ember-gp-sink-table-options-field.h"

/**
 * raw constructor
 *              - bits 0..2 : Application Id (0b000 : use source Id)
 *              - bits 3..4 : Communication mode (0b01 : groupcast forwarding of the GP Notification command to DGroupID)
 *              - bit 5 : Sequence number capabilities (0b1 : use incremental sequence number)
 *              - bit 6 : RxOnCapability (0b0 : not capable)
 *              - bit 7 : FixedLocation (0b1 : not a mobile device)
 *              - bit 8 : AssignedAlias (0b0 : the derived alias is used)
 *              - bit 9 : Security use (0b1 : indicates that security-related parameters of the Sink Table entry are present)
 *              - bit 10..15 : Reserved
 */
CEmberGpSinkTableOption::CEmberGpSinkTableOption(const uint16_t i_options) :
        application_id(static_cast<uint8_t>(i_options&0x7)),
        communication_mode(static_cast<uint8_t>((i_options>>3)&0x3)),
        sequence_number_capabilities((i_options&0x20)!=0),
        rx_on_capability((i_options&0x40)!=0),
        fixed_location((i_options&0x80)!=0),
        assigned_alias((i_options&0x100)!=0),
        security_use((i_options&0x200)!=0)
{
}

/**
 * raw getter
 */
uint16_t CEmberGpSinkTableOption::get(void)
{
    uint16_t o_options = 0 + \
                        application_id + \
                        (communication_mode<<3) + \
                        (sequence_number_capabilities<<5) + \
                        (rx_on_capability<<6) + \
                        (fixed_location<<7) + \
                        (assigned_alias<<8) + \
                        (security_use<<9);

    return o_options;
}
