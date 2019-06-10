/**
 * @file green-power-sink.h
 * @brief access to green power capabilities
 */
#pragma once

#include "../zbmessage/green-power-frame.h"
#include "../green-power-observer.h"
#include "../ezsp-dongle.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN


class CGpSink : public CEzspDongleObserver
{
public:
    CGpSink( CEzspDongle &i_dongle );

    CGpSink() = delete; /* Construction without arguments is not allowed */
    CGpSink(const CGpSink&) = delete; /* No copy construction allowed */

    CGpSink& operator=(CGpSink) = delete; /* No assignment allowed */

    /**
     * Observer
     */
    void handleDongleState( EDongleState i_state ){;}
    void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive );    

    /**
     * Managing Observer of this class
     */
	bool registerObserver(CGpObserver* observer);
	bool unregisterObserver(CGpObserver* observer);


private:
    CEzspDongle &dongle;

    /**
     * Notify Observer of this class
     */
    std::set<CGpObserver*> observers;
    void notifyObserversOfRxGpFrame( CGpFrame i_gpf );    
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
