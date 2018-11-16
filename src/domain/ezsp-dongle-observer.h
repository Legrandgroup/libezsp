#pragma once

#include <vector>
#include "ezsp-protocol/ezsp-enum.h"

class CEzspDongleObserver {
public:
	CEzspDongleObserver() {};
	virtual ~CEzspDongleObserver() {};


    virtual void handleDongleState( EDongleState i_state ) = 0;
    virtual void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive ) = 0;
};
