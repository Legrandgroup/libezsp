/**
 * @file ash-codec.h
 *
 * @brief ASH protocol decoder/encoder
 **/

#pragma once

#include <cstdint>
#include <memory>	// For std::unique_ptr

#include "spi/TimerBuilder.h"
#include "spi/ByteBuffer.h"
#include "ezsp/enum-generator.h"

namespace NSEZSP {

class CAshCallback;     /* Forward declaration */

#define ASH_INFO(XX) \
	XX(ASH_RESET_FAILED,=1) \
	XX(ASH_ACK,) \
	XX(ASH_NACK,) \
	XX(ASH_STATE_DISCONNECTED,) \
	XX(ASH_STATE_CONNECTED,) \

class AshCodec {
public:
	/**
	 * @brief Current state for the ASH decoder
	 *
	 * @note The lines above describes all states known in order to build both an enum and enum-to-string/string-to-enum methods
	 *       In this macro, XX is a placeholder for the macro to use for building.
	 *       We start numbering from 1, so that 0 can be understood as value not found for enum-generator.h
	 * @see enum-generator.h
	 */
	DECLARE_ENUM(EAshInfo, ASH_INFO);

	AshCodec() = delete; /* Construction without arguments is not allowed */

	/**
	 * Constructor
	 *
	 * @param ipCb Callback invoked on ASH state change
	 * @param ackTimeoutCancelFunc A function callback invoked to cancel a running ack timeout (run when a proper ack is received)
	 */
	AshCodec(CAshCallback *ipCb, std::function<void (void)> ackTimeoutCancelFunc = nullptr);

	/**
	 * Copy constructor
	 *
	 * @warning No copy construction allowed
	 */
	AshCodec(const AshCodec&) = delete;

	/**
	 * Assignment operator
	 *
	 * @warning No assignment is allowed
	 */
	AshCodec& operator=(AshCodec) = delete;

	/**
	 * @brief Check whether we are in ASH connected state or not
	 *
	 * @return true if we are in ASH connected state
	 */
	bool isInConnectedState() const;

	/**
	 * @brief Select the callback to invoke to cancel an ack timer
	 *
	 * @param ackTimeoutCancelFunc The callback function to invoke (or nullptr to disable this callback)
	 */
	void setAckTimeoutCancelFunc(std::function<void (void)> ackTimeoutCancelFunc) {
		this->ackTimerCancelFunc = ackTimeoutCancelFunc;
	}

	/**
	 * @brief Create an ASH Reset NCP frame
	 *
	 * @return The ASH frame as a buffer
	 */
	NSSPI::ByteBuffer forgeResetNCPFrame(void);

	/**
	 * @brief Create an ASH ack frame
	 *
	 * @return The ASH frame as a buffer
	 */
	NSSPI::ByteBuffer forgeAckFrame(void);

	/**
	 * @brief Create an ASH data frame containing i_data as payload and return it
	 *
	 * @param[in] i_data The EZSP payload to be carried by the ASH frame
	 * 
	 * @note This buffer needs to be passed by copy because it is modified internally inside this method.
	 *
	 * @return The ASH frame as a buffer
	 */
	NSSPI::ByteBuffer forgeDataFrame(NSSPI::ByteBuffer i_data);

	/**
	 * @brief Try to append a chunk of ASH bytes to the current accumulated incoming bytes
	 *
	 * @param[in] i_data A stream of bytes to try to decode. If one or more messages could be extracted, then the successfully decoded bytes will be stripped from this buffer
	 *
	 * @note This buffer needs to be passed by copy because it is modified internally inside this method.
	 * 
	 * @return The parsed data contained in the ASH frame (mainly EZSP message contained in ASH data frames)
	 */
	NSSPI::ByteBuffer appendIncoming(NSSPI::ByteBuffer& i_data);

	/**
	 * @brief Compute an ASH CRC16 on a speficied buffer
	 *
	 * @param buf The raw data on which we will compute the CRC16
	 *
	 * @return The 16-bit computed CRC
	 */
	static uint16_t computeCRC(const NSSPI::ByteBuffer& buf);

	/**
	 * @brief Remove ASH byte stuffing on an ASH raw stream
	 *
	 * @param i_data The raw stream from which byte stuffing will be removed
	 *
	 * @return An ASH payload with byte stuffing removed
	 */
	static NSSPI::ByteBuffer removeByteStuffing(const NSSPI::ByteBuffer& i_data);

	/**
	 * @brief Apply ASH byte stuffing to an ASH payload
	 *
	 * @param i_data The payload to which byte stuffing will be added
	 *
	 * @return A byte stuffed raw stream for transmission over a serial link
	 */
	static NSSPI::ByteBuffer addByteStuffing(const NSSPI::ByteBuffer& i_data);

	/**
	 * @brief Apply ASH randomisation (XORed LFSR) to an ASH payload
	 *
	 * @param i_data The payload to randomize
	 *
	 * @return A randomized buffer for transmission over a serial link
	 */
	static NSSPI::ByteBuffer dataRandomize(const NSSPI::ByteBuffer& i_data, uint8_t start = 0);

protected:
	void trigger(NSSPI::ITimer* triggeringTimer);

private:
	/**
	 * @brief Process an ASH byte stream located between two FLAG bytes (ASH-encoded content)
	 * 
	 * @return The decoded ASH payload if any
	 */
	NSSPI::ByteBuffer processInterFlagStream();

public:
	CAshCallback *pCb;
private:
	std::function<void (void)> ackTimerCancelFunc;	/*!< The function we will invoke to cancel an ack timeout */
	uint8_t nextExpectedFEAckNum; /*!< The sequence number value of the next far-end (=from remote) ACK we are expecting to receive, meaning that the remote will then acknowlegde reception of the the last frame received with sequence number nextExpectedFEackNum-1 */
	uint8_t frmNum; /*!< The sequence number of the next data frame we will send */
	uint8_t lastReceivedByNEAckNum;	/*!< The ACk value that the near-end (us) will send to acknowledge the last far-end (=from remote) frame, meaning we acknowlegde reception of all frames up to sequence number lastReceivedByNEAckNum-1 */
	uint8_t ezspSeqNum;	/*!< FIXME: should be moved out of ASH, this is EZSP specific. The EZSP sequence number (wrapping 0-255 counter) */
	bool stateConnected;	/*!< Are we currently in connected state? (meaning we have an active working ASH handshake between host and NCP) */
	NSSPI::ByteBuffer in_msg; /*!< Currently accumulated buffer */
};

/**
 * @brief Interface of a ASH state change callback
 *
 * Objects that want to receive ASH state change should derive from this class and override method ashCbInfo
 */
class CAshCallback {
public:
	/**
	 * @brief Destructor
	 */
	virtual ~CAshCallback() = default;

	/**
	 * @brief Callback method invoked on ASH state change
	 *
	 * @param info The new ASH state
	 */
	virtual void ashCbInfo(AshCodec::EAshInfo info) = 0;
};

} // namespace NSEZSP
