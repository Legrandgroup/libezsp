/**
 * @file ember-gp-sink-table-options-field.h
 *
 * @brief Represents Ember's sink table entry options bit field
 *
 * option : 0x02A8 (cf. A3.3.2.2.1 Options parameter of the Sink Table from doc doc-14-0563-16-batt-green-power-spec_ProxyBasic.pdf)
 *     - bits 0..2 : Application Id (0b000 : use source Id)
 *     - bits 3..4 : Communication mode (0b01 : groupcast forwarding of the GP Notification command to DGroupID)
 *     - bit 5 : Sequence number capabilities (0b1 : use incremental sequence number)
 *     - bit 6 : RxOnCapability (0b0 : not capable)
 *     - bit 7 : FixedLocation (0b1 : not a mobile device)
 *     - bit 8 : AssignedAlias (0b0 : the derived alias is used)
 *     - bit 9 : Security use (0b1 : indicates that security-related parameters of the Sink Table entry are present)
 *     - bit 10..15 : Reserved
 */

#pragma once

#include <cstdint>
#include "ezsp/ezsp-protocol/ezsp-enum.h"
#include "ezsp/zbmessage/gpd-commissioning-command-payload.h" // BAD DEPENDANCY NEED TO BE INPROVE

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

class CEmberGpSinkTableOption
{
    public:
        /**
         * @brief Default constructor
         */
        CEmberGpSinkTableOption();

        /**
         * @brief Copy constructor
         *
         * @param other The object to copy from
         */
        CEmberGpSinkTableOption(const CEmberGpSinkTableOption& other);

        /**
         * @brief Assignment operator
         *
         * @param other The object to assign from
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
         * @brief Raw getter
         *
         * @return This option field object represented as a 16-bit word
         */
        uint16_t get() const;

        /**
         * @brief Getter for the enclosed application ID
         *
         * @return The enclosed application ID
         */
        uint8_t getApplicationId() const { return application_id; }

        /**
         * @brief Getter for the enclosed communication mode
         *
         * @return The enclosed communication mode
         */
        uint8_t getCommunicationMode() const { return communication_mode; }

        /**
         * @brief Getter for the enclosed sequence number capabilities
         *
         * @return The enclosed sequence number capabilities
         */
        bool isSequenceNumberCapabilities() const { return sequence_number_capabilities; }

        /**
         * @brief Getter for the enclosed RX On capability
         *
         * @return true if this object contains an RX On capability
         */
        bool isRxOnCapability() const { return rx_on_capability; }

        /**
         * @brief Getter for the enclosed fixed location
         *
         * @return true if this object contains a fixed location capability
         */
        bool isFixedLocation() const { return fixed_location; }

        /**
         * @brief Getter for the enclosed assigned alias
         *
         * @return true if this object contains an assigned alias
         */
        bool isAssignedAlias() const { return assigned_alias; }

        /**
         * @brief Getter for the enclosed security use
         *
         * @return true if this object uses security
         */
        bool isSecurityUse() const { return security_use; }

        /**
         * @brief Setter for RxOnCapability
         *
         * @param i_rx_on_capable
         */
        void setRxOnCapability(bool i_rx_on_capable){ rx_on_capability = i_rx_on_capable; }

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
        uint8_t application_id; /*!< The application ID contained in this sink table entry options bit field */
        uint8_t communication_mode; /*!< The communication mode contained in this sink table entry options bit field */
        bool sequence_number_capabilities; /*!< The sequence number capabilities contained in this sink table entry options bit field */
        bool rx_on_capability; /*!< The RX On capavility toggle contained in this sink table entry options bit field */
        bool fixed_location; /*!< The fixed location toggle contained in this sink table entry options bit field */
        bool assigned_alias; /*!< The assigned alias toggle contained in this sink table entry options bit field */
        bool security_use; /*!< The security use toggle contained in this sink table entry options bit field */
};

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
