/**
 * @file green-power-sink.h
 * @brief access to green power capabilities
 */
#pragma once

#include "../zbmessage/green-power-frame.h"
#include "../green-power-observer.h"
#include "../ezsp-dongle.h"
#include "zigbee-messaging.h"
#include "green-power-sink-table.h"
#include "../ezsp-protocol/struct/ember-gp-sink-table-entry-struct.h"

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
    void init(void);

    /**
     * Clear all GP tables
     */
    void gpClearAllTables( void );

    /**
     * @brief Open a commissioning session for limited time, close as soon as a binding is done.
     */
    void openCommissioningSession(void);

    /**
     * @brief Force to close commissioning session
     */
    void closeCommissioningSession(void);

    /**
     * @brief add a green power sink table entry
     *
     * @return index of entry in sink table, or GP_SINK_INVALID_ENTRY if table is full
     */
    uint8_t registerGpd( uint32_t i_source_id );

    /**
     * @brief add a green power device
     */
    void registerGpd( uint32_t i_source_id, std::vector<uint8_t> i_key );

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

    /**
     * Notify Observer of this class
     */
    std::set<CGpObserver*> observers;
    void notifyObserversOfRxGpFrame( CGpFrame i_gpf );

    void setSinkState( ESinkState i_state );

    /**
     * @brief send zigbee unicast message GP Proxy Commissioning Mode.
     *          done from sink to local dongle.
     *      WARNING all parameters are hardcoded for testing
     */
    void sendLocalGPProxyCommissioningMode(uint8_t i_option);

    /**
     * Retrieves the sink table entry stored at the passed index.
     */
    void gpSinkGetEntry( uint8_t i_index );

    /**
     * @brief Finds or allocates a sink entry
     * 
     * @param i_src_id : gpd address to be found
     */
    void gpSinkTableFindOrAllocateEntry( uint32_t i_src_id );

    /**
     * @brief Retrieves the sink table entry stored at the passed index.
     * 
     * @param i_index The index of the requested sink table entry.
     * @param i_entry An EmberGpSinkTableEntry struct containing a copy of the sink entry to be updated.
     */
    void gpSinkSetEntry( uint8_t i_index, CEmberGpSinkTableEntryStruct& i_entry );

    /**
     * Update the GP Proxy table based on a GP pairing.
     */
    void gpProxyTableProcessGpPairing( std::vector<uint8_t> i_param );     
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
