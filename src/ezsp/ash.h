/**
 * @file ash.h
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
	XX(ASH_STATE_CHANGE,) \

class CAsh : protected NSSPI::ITimerVisitor
{
public:
	/**
	 * @brief Current state for the ASH decoder
	 *
	 * @note The lines above describes all states known in order to build both an enum and enum-to-string/string-to-enum methods
	 *       In this macro, XX is a placeholder for the macro to use for building.
	 *       We start numbering from 1, so that 0 can be understood as value not found for enum-generator.h
	 * @see enum-generator.h
	 */
	DECLARE_ENUM(EAshInfo, ASH_INFO)

    CAsh() = delete; /* Construction without arguments is not allowed */
    /**
     * @param ipCb Callback invoked on ASH state change
     * @param i_timer_builder Timer builder object used to generate timers
     */
    CAsh(CAshCallback *ipCb, const NSSPI::TimerBuilder& i_timer_builder);

    CAsh(const CAsh&) = delete; /* No copy construction allowed */

    CAsh& operator=(CAsh) = delete; /* No assignment allowed */

    NSSPI::ByteBuffer resetNCPFrame(void);

    NSSPI::ByteBuffer AckFrame(void);

    NSSPI::ByteBuffer DataFrame(NSSPI::ByteBuffer i_data);

    NSSPI::ByteBuffer decode(NSSPI::ByteBuffer &i_data);

    bool isConnected(void){ return stateConnected; }

    static std::string EAshInfoToString( EAshInfo in );
protected:
    void trigger(NSSPI::ITimer* triggeringTimer);

private:
    uint8_t ackNum;
    uint8_t frmNum;
    uint8_t seq_num;
    bool stateConnected;
    std::unique_ptr<NSSPI::ITimer> timer;
    CAshCallback *pCb;

    NSSPI::ByteBuffer in_msg;

    uint16_t computeCRC( NSSPI::ByteBuffer i_msg );
    void decode_flag(NSSPI::ByteBuffer &lo_msg);
    void clean_flag(NSSPI::ByteBuffer &lo_msg);
    NSSPI::ByteBuffer stuffedOutputData(NSSPI::ByteBuffer i_msg);
    NSSPI::ByteBuffer dataRandomise(NSSPI::ByteBuffer i_data, uint8_t start);
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
	virtual void ashCbInfo(CAsh::EAshInfo info) = 0;
};

} // namespace NSEZSP
