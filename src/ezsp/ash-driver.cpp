/**
 * @file ash-driver.cpp
 *
 * @brief ASH serial driver
 **/

#include <iostream>
#include <list>
#include <map>
#include <iomanip>

#include "ash-driver.h"
#include "ezsp/ezsp-protocol/ezsp-enum.h"
#include "ezsp/byte-manip.h"

#include "spi/ILogger.h"

// For debug logging only
//#include <sstream>

using NSEZSP::AshDriver;

/**
 * The receive timeout settings - min/initial/max - defined in milliseconds
 */
constexpr size_t T_RX_ACK_MIN = 400;
constexpr size_t T_RX_ACK_INIT = 1600;
constexpr size_t T_RX_ACK_MAX = 3200;
constexpr size_t T_ACK_ASH_RESET = 5000;

constexpr uint8_t ASH_CANCEL_BYTE     = 0x1A;
constexpr uint8_t ASH_FLAG_BYTE       = 0x7E;
constexpr uint8_t ASH_SUBSTITUTE_BYTE = 0x18;
constexpr uint8_t ASH_XON_BYTE        = 0x11;
constexpr uint8_t ASH_OFF_BYTE        = 0x13;
constexpr uint8_t ASH_TIMEOUT         = -1;

constexpr uint32_t ASH_MAX_LENGTH     = 131;

AshDriver::AshDriver(CAshCallback* ipCb, const NSSPI::TimerBuilder& i_timer_builder) :
	ashCodec(ipCb, i_timer_builder /*FIXME: timer should only be used inside driver*/),
	stateConnected(false),
	ackTimer(i_timer_builder.create())
{
}

void AshDriver::trigger(NSSPI::ITimer* triggeringTimer) {
    if( !stateConnected )
    {
		if (this->ashCodec.pCb) {
			this->ashCodec.pCb->ashCbInfo(NSEZSP::AshCodec::ASH_RESET_FAILED);
		}
    }
    else
    {
        clogE << "ASH timeout while connected\n";
    }
}

NSSPI::ByteBuffer AshDriver::sendResetNCPFrame(void) {
	return this->ashCodec.resetNCPFrame();
}

NSSPI::ByteBuffer AshDriver::sendAckFrame(void) {
	return this->ashCodec.AckFrame();
}

NSSPI::ByteBuffer AshDriver::sendDataFrame(NSSPI::ByteBuffer i_data) {
	return this->ashCodec.DataFrame(i_data);
}

NSSPI::ByteBuffer AshDriver::decode(NSSPI::ByteBuffer& i_data) {
	return this->ashCodec.decode(i_data);
}