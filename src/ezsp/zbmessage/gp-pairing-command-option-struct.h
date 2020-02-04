/**
 * @file gp-pairing-command-option-struct.h
 *
 * @brief Option field of gp pairing command according to A.3.3.5.2 GP Pairing command from docs-14-0563-16-batt-green-power-spec_ProxyBasic.pdf
 */
 
#pragma once

#include <cstdint>

#include "ezsp/ezsp-protocol/struct/ember-gp-sink-table-options-field.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN

namespace NSEZSP {

class CGpPairingCommandOption
{
    public:
        /**
         * @brief Default constructor
         *
         * Construction without arguments is not allowed
         */
        CGpPairingCommandOption() = delete;

        /**
         * @brief Copy constructor
         *
         * Copy construction is forbidden on this class
         */
        CGpPairingCommandOption(const CGpPairingCommandOption& other) = delete;

        /**
         * @brief Constructor based on sink table entry option field
         *
         * @param i_sink_table_option
         * @param i_add_sink : true to add, false to remove
         * @param i_remove_gpd : true to remove
         * @param i_security_level
         * @param i_security_key_type
         * @param i_frm_counter_present
         * @param i_key_present
         * @param i_radius_present
         */
        CGpPairingCommandOption(CEmberGpSinkTableOption i_sink_table_option, bool i_add_sink, bool i_remove_gpd,
                                uint8_t i_security_level, uint8_t i_security_key_type, bool i_frm_counter_present,
                                bool i_key_present, bool i_radius_present );

        /**
         * @brief Option getter
         *
         * @return This GP pairing command option represented as a 32-bit word
         */
        uint32_t get() const;


    private:
        /**
         *  Bit 0..2: The ApplicationID sub-field contains the information about the application used by the GPD.
         *      ApplicationID = 0b000 indicates the GPD ID field has the length of 4B and contains the GPD SrcID; the Endpoint field is absent.
         *      ApplicationID = 0b010 indicates the GPD ID field has the length of 8B and contains the GPD IEEE address; the Endpoint field is present.
         *      All values of ApplicationID other than 0b000 and 0b010 are reserved in the current version of the Green Power cluster specification.
         */
        uint8_t application_id; /*!< The ApplicationID value (contains the information about the application used by the GPD) */
        /**
         *  Bit 3: The AddSink sub-field of the Options field indicates, whether the GP sink wishes to add or remove a
         *          pairing for the GPD identified by the GPD ID. If set to 0b1 the pairing is being added. If set to 0b0 the
         *          pairing is being removed; then, the following fields are not present: DeviceID, GPD security Frame
         *          Counter, GPD key, AssignedAlias, and ForwardingRadius.
         */
        bool add_sink;  /*!< The AddSink value of the Options field. Indicates, whether the GP sink wishes to add or remove a pairing for the GPD identified by the GPD ID */
        /**
         * Bit 4: The RemoveGPD sub-field of the Options field, if set to 0b1, indicates that the GPD identified by the
         *          GPD ID is being removed from the network. Then, none of the optional fields is present.
         */
        bool remove_gpd;        /*!< The RemoveGPD sub-field of the Options field, true indicates that the GPD identified by the */
        /**
         * Bit 5..6: The Communication mode sub-field defines the communication mode requested by the sink, and can take values as defined in Table 27.
         */
        uint8_t communication_mode;     /*!< The Communication mode sub-field defines the communication mode requested by the sink */
        /**
         * Bit 7: The GPDfixed sub-field and GPD MAC sequence number capabilities sub-field is copied from the corresponding
         *          FixedLocation and Sequence number capabilities sub-fields of the Options parameter of the Sink Table for this GPD.
         */
        bool gpd_fixed; /*!< The GPDfixed sub-field value */
        /**
         * Bit 8: see above
         */
        bool gpd_mac_seq_number_capability; /*!< The GPD MAC sequence number capabilities sub-field */
        /**
         * Bit 9..10: The SecurityLevel and SecurityKeyType SHALL carry the values of the corresponding parameters in Sink Table entry for this GPD.
         */
        uint8_t security_level; /*!< The SecurityLevel for this GDP command */
        /**
         * Bit 11..13: The sub-fields GPDsecurityFrameCounterPresent and GPDsecurityKeyPresent, if set to 0b1, indicate the presence of the fields
         *              GPDsecurityFrameCounter and GPDsecurityKey, respectively, which then carry the corresponding values from the Sink Table for this GPD.
         *              When the sub-fields GPDsecurityFrameCounterPresent and GPDsecurityKeyPresent are set to 0b0,
         *              the fields GPDsecurityFrame-Counter and GPDsecurityKey, respectively, are not present.
         */
        uint8_t security_key_type; /*!< The SecurityKeyType for this GDP command */
        /**
         * Bit 14: The GPDsecurityFrameCounter field SHALL be present whenever the AddSink sub-field of the Options field is set to 0b1;
         *          independent of the security level. If the SecurityLevel sub-field is set to 0b10-0b11 or if the SecurityLevel is 0b00 and
         *          the GPD MAC sequence number capabilities sub-field is set to 0b1, the GPDsecurityFrameCounter field carries the current value of
         *          the GPD security frame counter field from the Sink Table entry corresponding to the GPD ID. If the SecurityLevel is 0b00 and
         *          the GPD MAC sequence number capabilities sub-field is set to 0b0, the GPDsecurityFrameCounter SHALL be set to 0b00000000.
         */
        bool gpd_security_frame_counter_present; /*!< Represents whether the GPD security frame counter is present in this GDP command */
        /**
         * Bit 15: see above
         */
        bool gpd_security_key_present; /*!< Represents whether the GPD security key is present in this GDP command */
        /**
         * Bit 16: The AssignedAlias present sub-field, if set to 0b1, indicates that the AssignedAlias field is present and
         *          carries the Alias value to be used for this GPD instead of the derived alias.
         */
        bool assigned_alias_present; /*!< Represents whether the AssignedAlias field is present and carries the Alias value to be used for this GPD instead of the derived alias */
        /**
         * Bit 17: The Forwarding Radius present sub-field, if set to 0b1, indicates that the Forwarding Radius field is
         *          present and carries the Forwarding Radius value to be used as value of the radius in the groupcast forwarding
         *          of the GPDF packet. If the Forwarding Radius field is not present, and a new Proxy Table entry is to be created,
         *          the default value of 0x00 SHALL be used. The value 0x00 indicates unspecified,
         *          i.e. twice the value of the nwkMaxDepth attribute of the NIB, as specified by [1].
         */
        bool forwarding_radius_present; /*!< Represents whether the Forwarding Radius sub-field is present in this GDP command */
        /**
         * Bit 18..23: Reserved
         */
};

} // namespace NSEZSP
#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
