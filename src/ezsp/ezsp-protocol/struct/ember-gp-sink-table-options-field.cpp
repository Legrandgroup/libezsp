/**
 * @file ember-gp-sink-table-options-field.cpp
 *
 * @brief Represents Ember's sink table entry options bit field
 *
 * option : 0x02A8 (cf. A3.3.2.2.1 Options parameter of the Sink Table from doc doc-14-0563-16-batt-green-power-spec_ProxyBasic.pdf)
 *     - bits 0..2 : Application Id (0b000 : use source Id)
 *     - bits 3..4 : Communication mode (0b01 : groupcast forwarding of the GP Notification command to DGroupID)
 *     - bit 5 : Sequence number capabilities (0b1 : use incremental sequence number)
 *     - bit 6 : RxOnCapability (0b0 : not capable)
 *     - bit 7 : FixedLocation (0b1 : not a mobile device)
 *     - bit 8 : AssignedAlias (0b0 : the derived alias is used)
 *     - bit 9 : Security use (0b1 : indicates that security-related parameters of the Sink Table entry are present)
 *     - bit 10..15 : Reserved
 */

#include <sstream>
#include <iomanip>

#include <ezsp/ezsp-protocol/struct/ember-gp-sink-table-options-field.h>

CEmberGpSinkTableOption::CEmberGpSinkTableOption() :
        application_id(),
        communication_mode(),
        sequence_number_capabilities(),
        rx_on_capability(),
        fixed_location(),
        assigned_alias(),
        security_use()
{

}


CEmberGpSinkTableOption::CEmberGpSinkTableOption(const CEmberGpSinkTableOption& other) :
        application_id(other.application_id),
        communication_mode(other.communication_mode),
        sequence_number_capabilities(other.sequence_number_capabilities),
        rx_on_capability(other.rx_on_capability),
        fixed_location(other.fixed_location),
        assigned_alias(other.assigned_alias),
        security_use(other.security_use)
{

}


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
        communication_mode((static_cast<uint8_t>(i_options>>3)&0x3)),
        sequence_number_capabilities((i_options&0x20)!=0),
        rx_on_capability((i_options&0x40)!=0),
        fixed_location((i_options&0x80)!=0),
        assigned_alias((i_options&0x100)!=0),
        security_use((i_options&0x200)!=0)
{
}

/**
 * @brief constructor from commissioning payload option and more
 *
 * @param i_application_id : application id meeans way to address gpd : by sourceid or ieee
 * @param i_gpdf_commissioning_payload : permit to know capability of gpd
 */
CEmberGpSinkTableOption::CEmberGpSinkTableOption(const uint8_t i_application_id, CGpdCommissioningPayload i_gpdf_commissioning_payload) :
        application_id(i_application_id),
        communication_mode(1),
        sequence_number_capabilities(i_gpdf_commissioning_payload.isMACsequenceNumberCapability()),
        rx_on_capability(i_gpdf_commissioning_payload.isRxOnCapability()),
        fixed_location(i_gpdf_commissioning_payload.isFixedLocation()),
        assigned_alias(0),
        security_use(i_gpdf_commissioning_payload.isExtendedOptionsFieldPresent())
{

}

/**
 * This method is a friend of CEmberGpSinkTableOption class
 * swap() is needed within operator=() to implement to copy and swap paradigm
**/
void swap(CEmberGpSinkTableOption& first, CEmberGpSinkTableOption& second) /* nothrow */
{
  using std::swap;	// Enable ADL

  swap(first.application_id, second.application_id);
  swap(first.communication_mode, second.communication_mode);
  swap(first.sequence_number_capabilities, second.sequence_number_capabilities);
  swap(first.rx_on_capability, second.rx_on_capability);
  swap(first.fixed_location, second.fixed_location);
  swap(first.assigned_alias, second.assigned_alias);
  swap(first.security_use, second.security_use);
  /* Once we have swapped the members of the two instances... the two instances have actually been swapped */
}

CEmberGpSinkTableOption& CEmberGpSinkTableOption::operator=( CEmberGpSinkTableOption other)
{
  swap(*this, other);
  return *this;
}

/**
 * raw getter
 */
uint16_t CEmberGpSinkTableOption::get() const
{
    uint16_t o_options;
    o_options = static_cast<uint16_t>(static_cast<uint16_t>(application_id) << 0);
    o_options |= static_cast<uint16_t>(static_cast<uint16_t>(communication_mode) << 3);
    o_options |= static_cast<uint16_t>(static_cast<uint16_t>(sequence_number_capabilities) << 5);
    o_options |= static_cast<uint16_t>(static_cast<uint16_t>(rx_on_capability) << 6);
    o_options |= static_cast<uint16_t>(static_cast<uint16_t>(fixed_location) << 7);
    o_options |= static_cast<uint16_t>(static_cast<uint16_t>(assigned_alias) << 8);
    o_options |= static_cast<uint16_t>(static_cast<uint16_t>(security_use) << 9);

    return o_options;
}

std::string CEmberGpSinkTableOption::String() const
{
    std::stringstream buf;

    buf << "CEmberGpSinkTableOption : { ";
    buf << "[application_id : "<< std::hex << std::setw(2) << std::setfill('0') << unsigned(application_id) << "]";
    buf << "[communication_mode : "<< std::hex << std::setw(2) << std::setfill('0') << unsigned(communication_mode) << "]";
    buf << "[sequence_number_capabilities : "<< std::hex << std::setw(2) << std::setfill('0') << unsigned(sequence_number_capabilities) << "]";
    buf << "[rx_on_capability : "<< std::hex << std::setw(2) << std::setfill('0') << unsigned(rx_on_capability) << "]";
    buf << "[fixed_location : "<< std::hex << std::setw(2) << std::setfill('0') << unsigned(fixed_location) << "]";
    buf << "[assigned_alias : "<< std::hex << std::setw(2) << std::setfill('0') << unsigned(assigned_alias) << "]";
    buf << "[security_use : "<< std::hex << std::setw(2) << std::setfill('0') << unsigned(security_use) << "]";
    buf << " }";

    return buf.str();
}

std::ostream& operator<< (std::ostream& out, const CEmberGpSinkTableOption& data){
    out << data.String();
    return out;
}
