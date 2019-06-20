/**
 * @file green-power-observer.h
 *
 * @brief Describes the interface for an observer that wants to process incoming green power frames
 */
#pragma once

#include "zbmessage/green-power-frame.h"

class CGpObserver {
public:
    CGpObserver() {};
    virtual ~CGpObserver() {};

    /**
     * @brief Method that will be invoked on incoming green power frames
     *
     * @param i_gpf The green power frame received
     */
    virtual void handleRxGpFrame( CGpFrame &i_gpf ) = 0;
};