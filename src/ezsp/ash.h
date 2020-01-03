#pragma once

#include <cstdint>
#include <vector>
#include <memory>	// For std::unique_ptr

#include "spi/TimerBuilder.h"

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
    virtual ~CAshCallback() { }
    virtual void ashCbInfo( EAshInfo info ) = 0;
};

class CAsh : protected ITimerVisitor
{
public:
    CAsh() = delete; /* Construction without arguments is not allowed */
    /**
     * ipCb : call to inform state of ash
     * ipTimer : timer object pass to ash module to manage internal timer
     */
    CAsh(CAshCallback *ipCb, TimerBuilder &i_timer_factory);

    CAsh(const CAsh&) = delete; /* No copy construction allowed */

    CAsh& operator=(CAsh) = delete; /* No assignment allowed */

    std::vector<uint8_t> resetNCPFrame(void);

    std::vector<uint8_t> AckFrame(void);

    std::vector<uint8_t> DataFrame(std::vector<uint8_t> i_data);

    std::vector<uint8_t> decode(std::vector<uint8_t> &i_data);

    bool isConnected(void){ return stateConnected; }

    static std::string EAshInfoToString( EAshInfo in );
protected:
    void trigger(ITimer* triggeringTimer);

private:
    uint8_t ackNum;
    uint8_t frmNum;
    uint8_t seq_num;
    bool stateConnected;
    std::unique_ptr<ITimer> timer;
    CAshCallback *pCb;

    std::vector<uint8_t> in_msg;

    uint16_t computeCRC( std::vector<uint8_t> i_msg );
    void decode_flag(std::vector<uint8_t> &lo_msg);
    void clean_flag(std::vector<uint8_t> &lo_msg);
    std::vector<uint8_t> stuffedOutputData(std::vector<uint8_t> i_msg);
    std::vector<uint8_t> dataRandomise(std::vector<uint8_t> i_data, uint8_t start);
};
}
