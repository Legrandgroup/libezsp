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
#include "../../zbmessage/gpd-commissioning-command-payload.h" // BAD DEPENDANCY NEED TO BE INPROVE

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
         */
        CEmberGpSinkTableOption(const CEmberGpSinkTableOption& other);

        /**
         * @brief Assignment operator
         */
        CEmberGpSinkTableOption& operator=( CEmberGpSinkTableOption other);

        /**
         * @brief swap function to allow implementing of copy-and-swap idiom on members of type CEmberGpSinkTableOption
         *
         * This function will swap all attributes of \p first and \p second
         * See http://stackoverflow.com/questions/3279543/what-is-the-copy-and-swap-idiom
         *
         * @param first The first object
         * @param second The second object
         */
        friend void (::swap)(CEmberGpSinkTableOption& first, CEmberGpSinkTableOption& second);        

        /**
         * @brief raw constructor
         */
        CEmberGpSinkTableOption(const uint16_t i_options);

        /**
         * @brief constructor from commissioning payload option and more
         * 
         * @param i_application_id : application id meeans way to address gpd : by sourceid or ieee
         * @param i_gpdf_commissioning_option : permit to know capability of gpd
         */
        CEmberGpSinkTableOption(const uint8_t i_application_id, CGpdCommissioningPayload i_gpdf_commissioning_payload);

        /**
         * @brief raw getter
         */
        uint16_t get(void);

        /**
         * @brief application_id getter
         */
        uint8_t getApplicationId(void){ return application_id; }
        /**
         * @brief communication_mode getter
         */
        uint8_t getCommunicationMode(void){ return communication_mode; }
        /**
         * @brief sequence_number_capabilities getter
         */
        bool isSequenceNumberCapabilities(void){ return sequence_number_capabilities; }
        /**
         * @brief rx_on_capability getter
         */
        bool isRxOnCapability(void){ return rx_on_capability; }
        /**
         * @brief fixed_location getter
         */
        bool isFixedLocation(void){ return fixed_location; }
        /**
         * @brief assigned_alias getter
         */
        bool isAssignedAlias(void){ return assigned_alias; }
        /**
         * @brief security_use getter
         */
        bool isSecurityUse(void){ return security_use; }


        /**
         * @brief Dump this instance as a string
         *
         * @return The resulting string
         */
        std::string String() const;

        /**
         * @brief Serialize to an iostream
         *
         * @param out The original output stream
         * @param data The object to serialize
         *
         * @return The new output stream with serialized data appended
         */
        friend std::ostream& operator<< (std::ostream& out, const CEmberGpSinkTableOption& data);        
        
    private:
        uint8_t application_id;
        uint8_t communication_mode;
        bool sequence_number_capabilities;
        bool rx_on_capability;
        bool fixed_location;
        bool assigned_alias;
        bool security_use;
};