#ifndef ASH_H
#define ASH_H

#include <cstdint>
#include <vector>

#include "observer.h"
#include "timer.h"


typedef enum {
  ASH_RESET_FAILED,
  ASH_ACK,
  ASH_NACK
}EAshInfo;

class CAsh : public CObservateur 
{
public:
    CAsh();

    std::vector<uint8_t> resetNCPFrame(void);

    void Update(const CObservable* observable) const;

private:
    uint8_t ackNum;
    uint8_t frmNum;
    uint8_t seq_num;
    bool stateConnected;
    CTimer timer;

    std::vector<uint8_t> in_msg;

    uint16_t computeCRC( std::vector<uint8_t> i_msg );
    std::vector<uint8_t> stuffedOutputData(std::vector<uint8_t> i_msg);

};


#endif // ASH_H