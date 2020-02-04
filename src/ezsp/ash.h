#pragma once

#include <cstdint>
#include <vector>
#include <memory>	// For std::unique_ptr

#include "spi/TimerBuilder.h"
#include "spi/ByteBuffer.h"

namespace NSEZSP {

typedef enum {
  ASH_RESET_FAILED,
  ASH_ACK,
  ASH_NACK,
  ASH_STATE_CHANGE
}EAshInfo;

class CAshCallback
{
public:
    virtual ~CAshCallback() = default;
    virtual void ashCbInfo( EAshInfo info ) = 0;
};

class CAsh : protected NSSPI::ITimerVisitor
{
public:
    CAsh() = delete; /* Construction without arguments is not allowed */
    /**
     * ipCb : call to inform state of ash
     * ipTimer : timer object pass to ash module to manage internal timer
     */
    CAsh(CAshCallback *ipCb, NSSPI::TimerBuilder &i_timer_factory);

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
}
