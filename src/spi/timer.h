#ifndef TIMER_H
#define TIMER_H

#include <cstdint>


class CTimer {
public:

    virtual void start( uint16_t i_timeout ) = 0;

    virtual void stop() = 0;

};

#endif // TIMER_H