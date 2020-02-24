/**
 * @file green-power-device-db.h
 *
 * @brief Database storing known encryption/authentication keys for green power devices
 */

#pragma once

#include <map>

#include "ezsp/zbmessage/green-power-device.h"

namespace NSEZSP {

class CGPDeviceDb
{
public:
    CGPDeviceDb();

    /**
     * @brief Clear all entries in the database
     */
    void clear();

    /**
     * @brief Add/overwrite a device entry in the database
     * 
     * @note If an element with the same source ID already exist, it will be overwritten
     * 
     * @param[in] i_source_id The source ID of the new device to add
     * @param[in] i_key The key of the new device to add
     */
    void insertDevice(uint32_t i_source_id, const NSEZSP::EmberKeyData& i_key);

    /**
     * @brief Add/overwrite a device entry in the database
     * 
     * @note If an element with the same source ID already exist, it will be overwritten
     * 
     * @param[in] i_gp_device A CGpDevice object containing the source ID and the key of the new device to add
     */
    void insertDevice(const NSEZSP::CGpDevice& i_gp_device);

    /**
     * @brief Set the database with the exact content provided as specific device list
     * 
     * @note The database will contain exactly this device list, any previous content will be destroyed
     * 
     * @param[in] i_gp_devices_list A vector of CGpDevice objects containing the source ID and the key of all devices to store in the database
     */
    void setDb(const std::vector<NSEZSP::CGpDevice>& i_gp_devices_list);

    /**
     * @brief Retrieve the key for a specific source ID
     * 
     * @param[in] i_source_id The source ID we are searching
     * @param[out] o_key a variable where to store the key corresponding to @p o_source_id
     * 
     * @return true if the source ID could be found in the database, false otherwise (in that case, o_source_id will remain unchanged)
     */
    bool getKeyForSourceId(uint32_t i_source_id, NSEZSP::EmberKeyData& o_key) const;

private:
    std::map<uint32_t, NSEZSP::EmberKeyData> gp_dev_list;   /*!< The green power device list stored in this class */
};

} // namespace NSEZSP
