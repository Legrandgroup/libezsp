/**
 * @file green-power-sink.h
 *
 * @brief Access to green power capabilities
 */
 
#pragma once

#include "../zbmessage/green-power-frame.h"
#include "../zbmessage/green-power-device.h"
#include "../green-power-observer.h"
#include "../ezsp-dongle.h"
#include "zigbee-messaging.h"
#include "green-power-sink-table.h"
#include "../ezsp-protocol/struct/ember-gp-sink-table-entry-struct.h"
#include "../ezsp-protocol/struct/ember-process-gp-pairing-parameter.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

typedef enum
{
    SINK_NOT_INIT, // starting state
    SINK_READY,  // default state if no action in progress
    SINK_ERROR,  // something wrong
    SINK_COM_OPEN,  // GP Proxy Commissioning mode open
    SINK_COM_IN_PROGRESS,  // GP sink receive gpf comm frame
    SINK_COM_OFFLINE_IN_PROGRESS, // Doing offline commissioning for GPD list
}ESinkState;

class CGpSink : public CEzspDongleObserver
{
public:
    CGpSink( CEzspDongle &i_dongle, CZigbeeMessaging &i_zb_messaging );

    CGpSink() = delete; /* Construction without arguments is not allowed */
    CGpSink(const CGpSink&) = delete; /* No copy construction allowed */

    CGpSink& operator=(CGpSink) = delete; /* No assignment allowed */

    /**
     * @brief Initialize sink, shall be done after a network init.
     */
    void init();

    /**
     * Clear all GP tables
     */
    void gpClearAllTables();

    /**
     * @brief Open a commissioning session for limited time, close as soon as a binding is done.
     */
    void openCommissioningSession();

    /**
     * @brief Force to close commissioning session
     */
    void closeCommissioningSession();

    /**
     * @brief add a green power sink table entry
     *
     * @return index of entry in sink table, or GP_SINK_INVALID_ENTRY if table is full
     */
    uint8_t registerGpd( uint32_t i_source_id );

    /**
     * @brief Add a green power device to this sink
     *
     * @param gpd The description oof the device to add
     */
    void registerGpd( const CGpDevice &gpd );

    /**
     * Observer
     */
    void handleDongleState( EDongleState i_state );
    void handleEzspRxMessage( EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive );

    /**
     * Managing Observer of this class
     */
    bool registerObserver(CGpObserver* observer);
    bool unregisterObserver(CGpObserver* observer);


private:
    CEzspDongle &dongle;
    CZigbeeMessaging &zb_messaging;
    CGpSinkTable sink_table;
    ESinkState sink_state;
    // parameters to save for pairing
    CGpFrame gpf_comm_frame;
    uint8_t sink_table_index;
    CEmberGpSinkTableEntryStruct sink_table_entry;

    std::set<CGpObserver*> observers;   /*!< List of observers of this class */

    /**
     * @brief Notify observers of this class
     *
     * @param i_gpf The received GP frame
     */
    void notifyObserversOfRxGpFrame( CGpFrame i_gpf );

    /**
     * @brief Private utility function to manage error state
     */
    void setSinkState( ESinkState i_state );

    /**
     * @brief send zigbee unicast message GP Proxy Commissioning Mode.
     *        done from sink to local dongle.
     * @warning All parameters are hardcoded for testing
     */
    void sendLocalGPProxyCommissioningMode(uint8_t i_option);

    /**
     * @brief Retrieves the sink table entry stored at the specified index
     *
     * @param i_index The index to lookup
     */
    void gpSinkGetEntry( uint8_t i_index );

    /**
     * @brief Finds or allocates a sink entry
     * 
     * @param i_src_id GPD source ID address to search
     */
    void gpSinkTableFindOrAllocateEntry( uint32_t i_src_id );

    /**
     * @brief Updates the sink table entry at the specified index.
     * 
     * @param i_index The index of the requested sink table entry.
     * @param i_entry An EmberGpSinkTableEntry struct containing a copy of the sink entry to be updated.
     */
    void gpSinkSetEntry( uint8_t i_index, CEmberGpSinkTableEntryStruct& i_entry );

    /**
     * @brief Update the GP Proxy table based on a GP pairing.
     */
    void gpProxyTableProcessGpPairing( CProcessGpPairingParam& i_param );     
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
