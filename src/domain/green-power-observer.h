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
     * @brief Method that will be invoked on incoming valid green power frames
     *
     * @param i_gpf The green power frame received
     */
    virtual void handleRxGpFrame( CGpFrame &i_gpf ) = 0;

    /**
     * @brief Method that will be invoked on every green power frame receive on our radio channel
     *
     * @param i_gpd_source_id The green power device id observe
     */
    virtual void handleRxGpdId( uint32_t &i_gpd_id ) = 0;

};