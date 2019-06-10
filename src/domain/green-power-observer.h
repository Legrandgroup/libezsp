/*
 */
#pragma once

#include "zbmessage/green-power-frame.h"


class CGpObserver {
public:
	CGpObserver() {};
	virtual ~CGpObserver() {};
	
    virtual void handleRxGpFrame( CGpFrame &i_gpf ) = 0;
};