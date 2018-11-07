#ifndef TIMER_H
#define TIMER_H

#include <cstdint>

#include "observer.h"

class CTimer : public CObservable
{
public:

    void start( uint16_t i_timeout );

    void stop();

    uint32_t Statut(void) const {return 0;}
};

#endif // TIMER_H