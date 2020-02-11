/**
 * @file green-power-frame.cpp
 *
 * @brief Handles decoding of a green power frame @todo encoding
 */

#include <sstream>
#include <iomanip>

#include "ezsp/byte-manip.h"
#include "ezsp/zbmessage/green-power-frame.h"

#include "ezsp/ezsp-protocol/struct/ember-gp-address-struct.h"

#include "spi/ILogger.h"
#include "spi/aes/AesBuilder.h"

using NSEZSP::CGpFrame;

CGpFrame::CGpFrame():
    link_value(0),
    sequence_number(0),
    source_id(0),
    security(GPD_NO_SECURITY),
    key_type(GPD_KEY_TYPE_NO_KEY),
    auto_commissioning(false),
    rx_after_tx(false),
    security_frame_counter(0),
    command_id(0xFF),
    mic(0),
    proxy_table_entry(0xFF),
    payload()
{
}

CGpFrame::CGpFrame(const NSSPI::ByteBuffer& raw_message):
    application_id(0),
    link_value(0),
    sequence_number(0),
    source_id(0),
    security(GPD_NO_SECURITY),
    key_type(GPD_KEY_TYPE_NO_KEY),
    auto_commissioning(false),
    rx_after_tx(false),
    security_frame_counter(0),
    command_id(0xFF),
    mic(0),
    proxy_table_entry(0xFF),
    payload()
{

    CEmberGpAddressStruct gp_address = CEmberGpAddressStruct(NSSPI::ByteBuffer(raw_message.begin()+3,raw_message.end()));

    this->application_id = gp_address.getApplicationId();
    /* only sourceId addressing mode is supported */
    if (this->application_id == 0) {
        link_value = raw_message.at(1);
        sequence_number = raw_message.at(2);
        source_id = gp_address.getSourceId();
        security = static_cast<EGpSecurityLevel>(raw_message.at(13));
        key_type = static_cast<EGpSecurityKeyType>(raw_message.at(14));
        auto_commissioning = raw_message.at(15);
        rx_after_tx = raw_message.at(16);
        security_frame_counter = quad_u8_to_u32(raw_message.at(20), raw_message.at(19), raw_message.at(18), raw_message.at(17));
        command_id = raw_message.at(21);
        mic = quad_u8_to_u32(raw_message.at(25), raw_message.at(24), raw_message.at(23), raw_message.at(22));
        proxy_table_entry = raw_message.at(26);
        for( unsigned int loop=0; loop<raw_message.at(27); loop++ )
        {
            payload.push_back(raw_message.at(28+loop));
        }
    }
    else {
        clogW << "Unsupported application ID: " << std::dec << this->application_id << ". Ignoring\n";
    }
}

uint8_t CGpFrame::toNwkFCByteField() const
{
    uint8_t nwkFC = 0;
    nwkFC |= 0x00;  /* Assume bit 1 to 0 = 0b00 (Frame type = Data) */
    nwkFC |= 0x0c;  /* Assume bit 5 to 2 = 0b0011 (Protocol version = 0x03) */
    if (this->auto_commissioning) {
        nwkFC |= 0x40;  /* Bit 6 is set when autocommissionning is true */
    }
    nwkFC |= 0x80;  /* Assume bit 7 is set (NWK frame extension is enabled) */
    return nwkFC;
}

uint8_t CGpFrame::toExtNwkFCByteField() const
{
    uint8_t extNwkFC = 0;
    extNwkFC |= this->application_id & 0x07;  /* Bits 2 to 0 contain the application ID */
    extNwkFC |= (static_cast<uint8_t>(this->security) & 0x03) << 3;    /* Bits 4 to 3 contain the security level */
    if (this->key_type != EGpSecurityKeyType::GPD_KEY_TYPE_NO_KEY) {
        extNwkFC |= 0x20;   /* Bit 5 is set if security is enabled */
    }
    if (this->rx_after_tx) {
        extNwkFC |= 0x40;   /* Bit 6 is set if RX after TX is true */
    }
    if (false) { /* Bit 7 is set if direction is to GPD (not in our case for received frames) */
        extNwkFC |= 0x80;   /* Bit 7 is set if transmission is done to GPD, cleared if transmission is done from GPD */
    }
    return extNwkFC;
}

NSEZSP::GPNonce CGpFrame::computeNonce(uint32_t sourceId, uint32_t frameCounter) const
{
    NSEZSP::GPNonce nonce;

    unsigned int idx = 0;
    /* For details about the GP bonce calculation, see 09-5499-25, section A 1.5.4.1 */

    /* Append twice the source ID, represented as little endian (see 09-5499-25, section A 1.5.4.1) */
    nonce.at(idx++) = u32_get_byte0(sourceId);
    nonce.at(idx++) = u32_get_byte1(sourceId);
    nonce.at(idx++) = u32_get_byte2(sourceId);
    nonce.at(idx++) = u32_get_byte3(sourceId);
    nonce.at(idx++) = u32_get_byte0(sourceId);
    nonce.at(idx++) = u32_get_byte1(sourceId);
    nonce.at(idx++) = u32_get_byte2(sourceId);
    nonce.at(idx++) = u32_get_byte3(sourceId);
    /* Append frame Counter bytes represented as little endian */
    nonce.at(idx++) = u32_get_byte0(frameCounter);
    nonce.at(idx++) = u32_get_byte1(frameCounter);
    nonce.at(idx++) = u32_get_byte2(frameCounter);
    nonce.at(idx++) = u32_get_byte3(frameCounter);
    /* Append security control byte, static (see figure 11 in 09-5499-25, section A 1.5.4.1) */
    /* Warning: bits are described from LSB to MSB in that figure 11 */
    nonce.at(idx++) = 0x05U;

    return nonce;
}

bool CGpFrame::validateMIC(const EmberKeyData& i_gpd_key) const
{
    if (this->security != EGpSecurityLevel::GPD_FRM_COUNTER_MIC_SECURITY) {
        clogW << "Unsupported security level: " << std::dec << this->security << "\n";
        return false;
    }
    NSEZSP::GPNonce nonce = this->computeNonce(this->source_id, this->security_frame_counter);
    clogD << "Nonce: " << NSSPI::Logger::byteSequenceToString(nonce) << "\n";

    /* Source ID is appended to header only if application ID is 0b000, we always append source ID below, so make sure we don't do it mistakenly */
    if (this->application_id != 0) {
        clogW << "Unsupported application ID: " << std::dec << this->application_id << ". Ignoring\n";
        return false;
    }
    std::array<uint8_t, 10> header;
    header.at(0) = this->toNwkFCByteField();
    header.at(1) = this->toExtNwkFCByteField();
    header.at(2) = u32_get_byte0(this->source_id);
    header.at(3) = u32_get_byte1(this->source_id);
    header.at(4) = u32_get_byte2(this->source_id);
    header.at(5) = u32_get_byte3(this->source_id);
    header.at(6) = u32_get_byte0(this->security_frame_counter);
    header.at(7) = u32_get_byte1(this->security_frame_counter);
    header.at(8) = u32_get_byte2(this->security_frame_counter);
    header.at(9) = u32_get_byte3(this->security_frame_counter);

    clogD << "Header: " << NSSPI::Logger::byteSequenceToString(header) << "\n";

    NSSPI::ByteBuffer a(header.begin(), header.end());

    /* Append payload to header (only when security_level is 2 or 1 (see 09-5499-25, section A 1.5.4.3.1)) */
    a.append(this->payload);

    clogD << "a: " << NSSPI::Logger::byteSequenceToString(a) << "\n";

    uint16_t La = a.size();

    NSSPI::ByteBuffer add_auth_data;

    add_auth_data.push_back(u16_get_lo_u8(La));
    add_auth_data.push_back(u16_get_hi_u8(La));
    add_auth_data.append(a);

    NSSPI::IAes *aes = NSSPI::AesBuilder::create();
    {
        unsigned int padToAesBlockSize = add_auth_data.size() % NSSPI::IAes::AES_BLOCK_SIZE;
        if (padToAesBlockSize>0) {  /* Only pad if there are remaining bytes outside of an AES block boundary */
            for (size_t i = 0; i<NSSPI::IAes::AES_BLOCK_SIZE - padToAesBlockSize; i++) {
                add_auth_data.push_back(0x00);  /* Pad with byte 0x00 */
            }
        }
    }
    return true;
}

std::string CGpFrame::String() const
{
    std::stringstream buf;

    buf << "CGpFrame: { ";
    buf << "[link_value: 0x" << std::hex << std::setw(2) << std::setfill('0') << +(link_value) << "]";
    buf << "[sequence_number: 0x"<< std::hex << std::setw(2) << std::setfill('0') << +(sequence_number) << "]";
    buf << "[source_id: " << std::hex << std::setw(8) << std::setfill('0') << static_cast<unsigned int>(source_id) << "]";
    buf << "[security: " << std::dec << static_cast<unsigned int>(this->security) << "]";
    buf << "[key_type: " << std::dec << static_cast<unsigned int>(this->key_type) << "]";
    buf << "[auto_commissioning: " << std::string(this->auto_commissioning?"Y":"N") << "]";
    buf << "[rx_after_tx: " << std::string(this->rx_after_tx?"Y":"N") << "]";
    buf << "[security_frame_counter: " << std::hex << std::setw(8) << std::setfill('0') << static_cast<unsigned int>(security_frame_counter) << "]";
    buf << "[command_id: " << std::hex << std::setw(2) << std::setfill('0') << +(command_id) << "]";
    buf << "[mic: "<< std::hex << std::setw(8) << std::setfill('0') << static_cast<unsigned int>(mic) << "]";
    buf << "[proxy_table_entry: 0x"<< std::hex << std::setw(2) << std::setfill('0') << +(proxy_table_entry) << "]";
    buf << "[payload:" << NSSPI::Logger::byteSequenceToString(payload) << "]";
    buf << " }";

    return buf.str();
}
