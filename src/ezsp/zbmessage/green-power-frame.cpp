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

    //clogD << "Lionel: constructing CGPFrame from buffer " << NSSPI::Logger::byteSequenceToString(raw_message) << "\n";
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

NSEZSP::GPNonce CGpFrame::computeNonce(uint32_t sourceId, uint32_t frameCounter)
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

NSSPI::ByteBuffer CGpFrame::getLastXiAESCBC(const EmberKeyData& i_gpd_key, const NSSPI::ByteBuffer& X0, const NSSPI::ByteBuffer& B0, const NSSPI::ByteBuffer& B, unsigned int& lastIndex)
{
    NSSPI::ByteBuffer result;
    lastIndex = 0;

    if (B.size() % NSSPI::IAes::AES_BLOCK_SIZE != 0) {
        clogE << "B should have a size equal to  a multiple of " << std::dec << static_cast<unsigned int>(NSSPI::IAes::AES_BLOCK_SIZE)
              << " but is " << B.size() << " bytes long\n";
        return NSSPI::ByteBuffer();
    }
    if (B0.size() != NSSPI::IAes::AES_BLOCK_SIZE) {
        clogE << "Wrong size for B0\n";
        return NSSPI::ByteBuffer();
    }
    if (X0.size() != NSSPI::IAes::AES_BLOCK_SIZE) {    /* X0 is our iv and should have the size of an AES block */
        clogE << "Wrong size for X0\n";
        return NSSPI::ByteBuffer();
    }
    uint8_t X0buf[X0.size()];
    X0.toMemory(X0buf);
    /* FIXME: memory leak below! */
    NSSPI::IAes *aes = NSSPI::AesBuilder::create();

    aes->set_key(i_gpd_key);

    uint8_t Bi_1buf[NSSPI::IAes::AES_BLOCK_SIZE];
    B0.toMemory(Bi_1buf);

    uint8_t Xibuf[NSSPI::IAes::AES_BLOCK_SIZE];

    clogD << "index runs from 1 to " << std::dec << static_cast<unsigned int>(B.size()/NSSPI::IAes::AES_BLOCK_SIZE+2) << "\n";
    for (unsigned int index = 1; index < B.size()/NSSPI::IAes::AES_BLOCK_SIZE+2; index++) {
        clogD << "B" << std::dec << (index-1) << ": " << NSSPI::Logger::byteSequenceToString(Bi_1buf, NSSPI::IAes::AES_BLOCK_SIZE) << "\n";
        clogD << "Running AES-CBC cypher on B"  << (index-1) << "\n";
        aes->cbc_encrypt(Bi_1buf, Xibuf, NSSPI::IAes::AES_BLOCK_SIZE, X0buf);  // Initially, this is X1=cipher.encrypt(X0)
        clogD << "X" << std::dec << index << ": " << NSSPI::Logger::byteSequenceToString(Xibuf, NSSPI::IAes::AES_BLOCK_SIZE) << "\n";
        for (unsigned int i=0; i<NSSPI::IAes::AES_BLOCK_SIZE; i++) {
            Bi_1buf[i] = B[i + (index-1) * NSSPI::IAes::AES_BLOCK_SIZE];
        }
        lastIndex = index;
    }

    return NSSPI::ByteBuffer(Xibuf, NSSPI::IAes::AES_BLOCK_SIZE);
}

bool CGpFrame::validateMIC(const EmberKeyData& i_gpd_key) const
{
    if (this->security != EGpSecurityLevel::GPD_FRM_COUNTER_MIC_SECURITY) {
        clogW << "Unsupported security level: " << std::dec << this->security << "\n";
        return false;
    }
    clogD << "Command ID from payload: 0x" << std::hex << std::setw(2) << std::setfill('0') << +(this->command_id) << "\n";
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

    /* First push_back the command ID byte (it is not part of the payload attribute) */
    a.push_back(this->command_id);
    /* Append payload to header (only when security_level is 2 or 1 (see 09-5499-25, section A 1.5.4.3.1)) */
    a.append(this->payload);

    clogD << "a: " << NSSPI::Logger::byteSequenceToString(a) << "\n";

    uint16_t La = a.size();

    NSSPI::ByteBuffer add_auth_data;

    add_auth_data.push_back(u16_get_hi_u8(La));
    add_auth_data.push_back(u16_get_lo_u8(La));
    add_auth_data.append(a);

    NSSPI::IAes *aes = NSSPI::AesBuilder::create();
    NSSPI::ByteBuffer padded_add_auth_data(add_auth_data);    /* Prepare a copy of add_auth_data that is going to be padded to align it to an exact multiple of AES block below */
    {
        unsigned int padToAesBlockSize = padded_add_auth_data.size() % NSSPI::IAes::AES_BLOCK_SIZE;
        if (padToAesBlockSize>0) {  /* Only pad if there are remaining bytes outside of an AES block boundary */
            for (size_t i = 0; i<NSSPI::IAes::AES_BLOCK_SIZE - padToAesBlockSize; i++) {
                padded_add_auth_data.push_back(0x00);  /* Pad with byte 0x00 */
            }
        }
    }
    clogD << "padded_add_auth_data: " << NSSPI::Logger::byteSequenceToString(padded_add_auth_data) << " (" << std::dec << padded_add_auth_data.size() << " bytes)\n";

    /* Define the plain text data (this is message m in AES CCM's terminology) */
    NSSPI::ByteBuffer plain_text_data; /* AES's m is an empty string when security_level is 2 or 1 (see 09-5499-25, section A 1.5.4.3.1) */

    NSSPI::ByteBuffer& padded_plain_text_data = plain_text_data; /* We should pad plain_text_data to be the smallest multiple of NSSPI::IAes::AES_BLOCK_SIZE here, but we unly support security levels leading to an empty value, so we don't do any additional padding here */
    clogD << "padded_plain_text_data: " << NSSPI::Logger::byteSequenceToString(padded_plain_text_data) << "\n";
    NSSPI::ByteBuffer auth_data(std::move(padded_add_auth_data));
    auth_data.append(padded_plain_text_data);

    /* Compute the flags, harcoded to 0x49 because:
       nonce is 13-bytes long, thus AES CCM's L value is 2 (len(nonce)=15-L)
       AES CCM's M value is 4 (4-bytes MIC)
       flags = CGpFrame::computeFlags(a.size(), 4, 2)
    */
    uint8_t flags = 0x49;

    NSSPI::ByteBuffer B0({flags});
    B0.append(nonce);

    uint16_t Lm = static_cast<uint16_t>(plain_text_data.size());  /* This will always be 0x0000 because m (and thus plain_text_data) is 0 */

    /* The two lines below actually thus append twice 0x00 */
    B0.push_back(u16_get_lo_u8(Lm));
    B0.push_back(u16_get_hi_u8(Lm));

    clogD << "auth_data: " << NSSPI::Logger::byteSequenceToString(auth_data) << "\n";

    NSSPI::ByteBuffer B(std::move(auth_data));  /* Prepare a copy of auth_data that is going to be padded to align it to an exact multiple of AES block below */
    {
        unsigned int padToAesBlockSize = B.size() % NSSPI::IAes::AES_BLOCK_SIZE;
        if (padToAesBlockSize>0) {  /* Only pad if there are remaining bytes outside of an AES block boundary */
            for (unsigned int i = 0; i<NSSPI::IAes::AES_BLOCK_SIZE - padToAesBlockSize; i++) {
                B.push_back(0x00);  /* Pad with byte 0x00 */
            }
        }
    }

    clogD << "B: " << NSSPI::Logger::byteSequenceToString(B) << " (" << static_cast<unsigned int>(B.size()) << " bytes)\n";

    /* X0 contains 16 times 0x00 */
    NSSPI::ByteBuffer X0 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    unsigned int lastIndex;

    clogD << "Running AES-CBC part of AES-CCM\n";
    uint32_t T;
    {
        NSSPI::ByteBuffer Xi = CGpFrame::getLastXiAESCBC(i_gpd_key, X0, B0, B, lastIndex);
        clogD << "Got last Xi result:\n";
        clogD << "X" << lastIndex << ": " << NSSPI::Logger::byteSequenceToString(Xi) << "\n";

        T = quad_u8_to_u32(Xi.at(3), Xi.at(2), Xi.at(1), Xi.at(0));    /* Grab the first 4 bytes of Xi, and make it a 32 bit word (little-endian) */
    } /* Xi now goes out of scope */

    clogD << "T: 0x" << std::hex << std::setw(8) << std::setfill('0') << T << "\n";

    clogD << "Running AES-CTR part of AES-CCM\n";

    NSSPI::ByteBuffer A0;
    A0.push_back(flags & 0x03);
    A0.append(nonce);
    A0.push_back(0x00);
    A0.push_back(0x00);

    NSSPI::ByteBuffer& iv = A0;

    /* Calculating the MIC (without any encryption), means running AES-CTR only once, with a counter that is equal to the IV
     * In that case, we run AES only once, and AES-CTR is just AES-EBC using the IV, XORed with the input.
     * In order not to create a dependency on AES-CTR, we will just implement it below using AES-EBC.
     * If encryption support is required, some refactor will be needed, where the IV is firs XORed with the 128-bit incrementing counter at each pass.
     */
    if (iv.size() != NSSPI::IAes::AES_BLOCK_SIZE) {
        clogE << "Internal error iv does not match AES block size\n";
        return false;
    }
    uint8_t ivBuf[NSSPI::IAes::AES_BLOCK_SIZE];
    iv.toMemory(ivBuf);
    uint8_t EBuf[NSSPI::IAes::AES_BLOCK_SIZE];
    aes->encrypt(ivBuf, EBuf);
    //clogD << "E: " << NSSPI::Logger::byteSequenceToString(EBuf, NSSPI::IAes::AES_BLOCK_SIZE) << "\n";
    /* The code above is commented out, because in our specific case, X0 is full of 0x00 */
    //for (unsigned int i=0; i<NSSPI::IAes::AES_BLOCK_SIZE; i++) {
    //    EBuf[i] ^= X0[i];    /* XOR the result of AES on counter with the plaintext */
    //}
    NSSPI::ByteBuffer r(EBuf, NSSPI::IAes::AES_BLOCK_SIZE);

    clogD << "A0: " << NSSPI::Logger::byteSequenceToString(A0) << "\n";
    clogD << "r(E(Key,A0)): " << NSSPI::Logger::byteSequenceToString(r) << "\n";
    clogD << "T: 0x" << std::hex << std::setw(8) << std::setfill('0') << T << "\n";

    if (r.size()<sizeof(T)) {
        clogE << "Internal error: mismatch bewteen r and T sizes\n";
        return false;
    }

    uint32_t U = quad_u8_to_u32(r[3], r[2], r[1], r[0]);    /* Keep only the 4 first bytes of r, store them in U */
    U ^= T; /* Xor with T */

    clogD << "Re-calculated MIC: 0x" << std::hex << std::setw(8) << std::setfill('0') << U << "\n";
    clogD << "MIC enclosed in GP frame: 0x" << std::hex << std::setw(8) << std::setfill('0') << this->mic << "\n";
    return (this->mic == U);
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
