/**
 * @file green-power-observer.h
 *
 * @brief Describes the interface for an observer that wants to process incoming green power frames
 */
#pragma once

#include <stdbool.h>

#include "zbmessage/green-power-frame.h"

/**
 * @brief enumetion for possible status of a gpd key from an incomming gpd
 */
enum class CGpdKeyStatus {
    Valid,      /*<! key is known for this gpd and it's the good one */
    Invalid,    /*<! key is known for this gpd but decryption failed, wrong key ? */
    Undefined   /*<! it's not possible to know the status of key for this gpd. */
};

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
     * @param i_gpd_known The green power device is known by the sink
     * @param i_gpd_key_status Status of key for this GPD
     */
    virtual void handleRxGpdId( uint32_t &i_gpd_id, bool i_gpd_known, CGpdKeyStatus i_gpd_key_status ) = 0;

};
