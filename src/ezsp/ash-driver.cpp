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
constexpr uint32_t T_RX_ACK_MIN       = 400;
constexpr uint32_t T_RX_ACK_INIT      = 1600;
constexpr uint32_t T_RX_ACK_MAX       = 3200;
constexpr uint32_t T_ACK_ASH_RESET    = 5000;

AshDriver::AshDriver(CAshCallback* ipCb, const NSSPI::TimerBuilder& i_timer_builder, NSSPI::GenericAsyncDataInputObservable* serialReadObservable) :
	enabled(true),
	ackTimer(i_timer_builder.create()),
	ashCodec(ipCb),
	serialReadObservable(serialReadObservable),
	serialWriteFunc(nullptr),
	serialRWMutex() {
	/* Tell the codec that it should invoke cancelTimer() below to cancel ACk timeoutes when a proper ASH ACK is received */

	this->ashCodec.setAckTimeoutCancelFunc([this]() {
		this->ackTimer->stop();
	});
	this->registerSerialReadObservable(this->serialReadObservable);	/* Register ourselves as an async observer if a valid serialReadObservable was provided */
}

AshDriver::~AshDriver() {
	this->ashCodec.setAckTimeoutCancelFunc(nullptr);	/* Disable any timeout callback */
	this->registerSerialReadObservable(nullptr);	/* Remove ourselves from the observers */
}

void AshDriver::disable() {
	this->enabled = false;
}

void AshDriver::enable() {
	this->enabled = true;
}

void AshDriver::trigger(NSSPI::ITimer* triggeringTimer) {
	if (!this->ashCodec.isInConnectedState()) {
		if (this->ashCodec.pCb) {
			this->ashCodec.pCb->ashCbInfo(NSEZSP::AshCodec::ASH_RESET_FAILED);
		}
	}
	else {
		clogE << "ASH ACK timeout while connected\n";
	}
}

void AshDriver::registerSerialWriter(FAshDriverWriteFunc newWriteFunc) {
	this->serialWriteFunc = newWriteFunc;
}

void AshDriver::registerSerialWriter(NSSPI::IUartDriverHandle uartHandle) {
	this->registerSerialWriter([uartHandle](size_t& writtenCnt, const uint8_t* buf, size_t cnt) -> int {
		return uartHandle->write(writtenCnt, buf, cnt);
	});
}

bool AshDriver::hasARegisteredSerialWriter() const {
	return (this->serialWriteFunc != nullptr);
}

void AshDriver::registerSerialReadObservable(NSSPI::GenericAsyncDataInputObservable* serialReadObservable) {
	if (this->serialReadObservable) {	/* First, unregister ourselves from any previous async observable */
		this->serialReadObservable->unregisterObserver(this);
	}
	this->serialReadObservable = serialReadObservable;
	if (serialReadObservable) {
		serialReadObservable->registerObserver(this);	/* Register ourselves as an async observer to receive incoming bytes received from the serial port */
	}
}

void AshDriver::handleInputData(const unsigned char* dataIn, const size_t dataLen) {
	if (this->enabled) { /* We only process incoming traffic on serial port in enabled mode */
		NSSPI::ByteBuffer inputData(dataIn, dataLen);
		const std::lock_guard<std::recursive_mutex> serialRWLock(this->serialRWMutex);	/* Make sure there is no write before we handle the read data (and ack if needed) */
		this->appendIncoming(inputData); /* Note: resulting decoded EZSP message will be notified to the caller (observer) using our observable property */
	}
	else {
		//clogD << "AshDriver ignoring incoming data in disabled mode\n";
	}
}

bool AshDriver::sendAshFrame(const NSSPI::ByteBuffer& frame) {
	size_t writtenBytes = 0;

	if (!this->serialWriteFunc) {
		clogE << "Cannot send NCP reset frame because no write functor is available\n";
		return false;
	}
	if (!this->enabled) {
		clogW << "Requested to write to serial port while in disabled mode\n";
		return false;
	}
	
	{
		const std::lock_guard<std::recursive_mutex> serialRWLock(this->serialRWMutex);
		if (this->serialWriteFunc(writtenBytes, frame.data(), frame.size()) < 0 ) {
			clogE << "Failed sending reset frame to serial port\n";
			return false;
		}
	}
	
	if (frame.size() != writtenBytes) {
		clogE << "Reset frame not fully written to serial port\n";
		return false;
	}
	return true;
}

bool AshDriver::sendResetNCPFrame() {

	this->ackTimer->stop();	/* Stop any possibly running timer */

	if (!this->sendAshFrame(this->ashCodec.forgeResetNCPFrame())) {
		return false;
	}
	/* Start RESET confirmation timer */
	this->ackTimer->start(T_ACK_ASH_RESET, this);

	return true;
}

bool AshDriver::sendAckFrame() {
	return this->sendAshFrame(this->ashCodec.forgeAckFrame());
}

bool AshDriver::sendDataFrame(const NSSPI::ByteBuffer& i_data) {
	/* FIXME: sendDataFrame() should not be allowed until the previous ack is confirmed, or the peer may have missed a frame! */
	this->ackTimer->stop();	/* Stop any possibly running timer */

	if (!this->sendAshFrame(this->ashCodec.forgeDataFrame(i_data))) {
		return false;
	}
	/* Start ACK timer */
	this->ackTimer->start(T_RX_ACK_INIT, this);

	return true;
}

void AshDriver::appendIncoming(NSSPI::ByteBuffer& i_data) {
	std::vector<NSSPI::ByteBuffer> ezspPayloads = this->ashCodec.appendIncoming(i_data);
	if (ezspPayloads.size()>1) {
		clogW << "Multiple EZSP payloads extraction from one ASH stream\n";	/* This should rarely occur except of very very slow hosts */
	}
	for (auto ezspPayload : ezspPayloads) {
		std::size_t ezspPayloadSize = ezspPayload.size();
		if (ezspPayloadSize>128) {	/* ASH should not carry payloads larger than 128 bytes */
			clogE << "EZSP payload too large. Ignored\n";
		}
		else {
			uint8_t ezspAsMemoryBuffer[ezspPayloadSize];
			ezspPayload.toMemory(ezspAsMemoryBuffer);
			this->notifyObservers(static_cast<unsigned char*>(ezspAsMemoryBuffer), ezspPayloadSize);
		}
	}
}

bool AshDriver::isConnected() const {
	return this->ashCodec.isInConnectedState();
}
