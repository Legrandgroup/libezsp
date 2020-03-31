/**
 * @file ash-driver.h
 *
 * @brief ASH serial driver
 **/

#pragma once

#include <cstdint>
#include <memory>	// For std::unique_ptr

#include "spi/GenericAsyncDataInputObservable.h"
#include "spi/TimerBuilder.h"
#include "spi/ByteBuffer.h"
#include "ezsp/enum-generator.h"
#include "ezsp/ash-codec.h"

namespace NSEZSP {

class AshDriver : public NSSPI::GenericAsyncDataInputObservable, protected NSSPI::ITimerVisitor {
public:
	AshDriver() = delete; /* Construction without arguments is not allowed */

    /**
     * @param ipCb Callback invoked on ASH state change
     * @param i_timer_builder Timer builder object used to generate timers
     */
	AshDriver(CAshCallback *ipCb, const NSSPI::TimerBuilder& i_timer_builder);

	AshDriver(const AshDriver&) = delete; /* No copy construction allowed */

	AshDriver& operator=(AshDriver) = delete; /* No assignment allowed */

    NSSPI::ByteBuffer sendResetNCPFrame(void);

    NSSPI::ByteBuffer sendAckFrame(void);

    NSSPI::ByteBuffer sendDataFrame(NSSPI::ByteBuffer i_data);

    NSSPI::ByteBuffer decode(NSSPI::ByteBuffer &i_data);

    bool isConnected(void){ return stateConnected; }

protected:
    void trigger(NSSPI::ITimer* triggeringTimer);

private:
	NSEZSP::AshCodec ashCodec;	/*!< ASH codec utility methods */
	uint8_t ackNum; /*!< The sequence number of the next frame we are expecting, meaning we acknowlegde reception of the the last frame received with sequence number ackNum-1 */
	uint8_t frmNum; /*!< The sequence number of the next data frame originated by us */
	uint8_t ezspSeqNum;	/*!< FIXME: should be moved out of ASH, this is EZSP specific. The EZSP sequence number (wrapping 0-255 counter) */
	bool stateConnected;	/*!< Are we currently in connected state? (meaning we have an active working ASH handshake between host and NCP) */
	std::unique_ptr<NSSPI::ITimer> ackTimer;	/*!< A timer checking acknowledgement of the initial RESET (if !stateConnected) of the last ASH DATA frame (if stateConnected) */

    NSSPI::ByteBuffer in_msg;

    void decode_flag(NSSPI::ByteBuffer &lo_msg);
};

} // namespace NSEZSP
