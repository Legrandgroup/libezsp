/**
* @brief Details of options bit filed for sink table entry
*          option : 0x02A8 (cf. A3.3.2.2.1 Options parameter of the Sink Table from doc doc-14-0563-16-batt-green-power-spec_ProxyBasic.pdf)
*              - bits 0..2 : Application Id (0b000 : use source Id)
*              - bits 3..4 : Communication mode (0b01 : groupcast forwarding of the GP Notification command to DGroupID)
*              - bit 5 : Sequence number capabilities (0b1 : use incremental sequence number)
*              - bit 6 : RxOnCapability (0b0 : not capable)
*              - bit 7 : FixedLocation (0b1 : not a mobile device)
*              - bit 8 : AssignedAlias (0b0 : the derived alias is used)
*              - bit 9 : Security use (0b1 : indicates that security-related parameters of the Sink Table entry are present)
*              - bit 10..15 : Reserved
*/

#pragma once

#include <cstdint>
#include "../ezsp-enum.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

class CEmberGpSinkTableOption
{
    public:
        /**
         * @brief Default constructor
         *
         * Construction without arguments is not allowed
         */
        CEmberGpSinkTableOption() = delete;

        /**
         * @brief Copy constructor
         *
         * Copy construction is forbidden on this class
         */
        CEmberGpSinkTableOption(const CEmberGpSinkTableOption& other) = delete;

        /**
         * @brief Assignment operator
         *
         * Assignment is forbidden on this class
         */
        CEmberGpSinkTableOption& operator=(const CEmberGpSinkTableOption& other) = delete;   


        /**
         * raw constructor
         */
        CEmberGpSinkTableOption(const uint16_t i_options);

        /**
         * raw getter
         */
        uint16_t get(void);
        

    private:
        uint8_t application_id;
        uint8_t communication_mode;
        bool sequence_number_capabilities;
        bool rx_on_capability;
        bool fixed_location;
        bool assigned_alias;
        bool security_use;
};