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

	bool isConnected(void) {
		return stateConnected;
	}

protected:
	void trigger(NSSPI::ITimer* triggeringTimer);

private:
	NSEZSP::AshCodec ashCodec;	/*!< ASH codec utility methods */
	bool stateConnected;	/*!< Are we currently in connected state? (meaning we have an active working ASH handshake between host and NCP) */
	std::unique_ptr<NSSPI::ITimer> ackTimer;	/*!< A timer checking acknowledgement of the initial RESET (if !stateConnected) of the last ASH DATA frame (if stateConnected) */
};

} // namespace NSEZSP
