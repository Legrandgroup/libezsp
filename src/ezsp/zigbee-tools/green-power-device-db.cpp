/**
 * @file green-power-device-db.cpp
 *
 * @brief Database storing known encryption/authentication keys for green power devices
 */

#include <iomanip>

#include "spi/ILogger.h"
#include "green-power-device-db.h"

using NSEZSP::CGPDeviceDb;

CGPDeviceDb::CGPDeviceDb() :
    gp_dev_list()
{
}

void CGPDeviceDb::clear()
{
    this->gp_dev_list.clear();
}

void CGPDeviceDb::insertDevice(uint32_t i_source_id, const NSEZSP::EmberKeyData& i_key)
{
    if (this->gp_dev_list.insert(std::make_pair(i_source_id, i_key)).second == false) {
        clogW << "Overwriting source ID 0x" << std::hex << std::setw(4) << std::setfill('0') << i_source_id << "\n";
    }
}

void CGPDeviceDb::insertDevice(const NSEZSP::CGpDevice& i_gp_device)
{
    this->insertDevice(i_gp_device.getSourceId(), i_gp_device.getKey());
}

void CGPDeviceDb::setDb(const std::vector<NSEZSP::CGpDevice>& i_gp_devices_list)
{
    this->gp_dev_list.clear();
    for(auto it = i_gp_devices_list.begin(); it != i_gp_devices_list.end(); ++it) {
        this->insertDevice(*it);
    }
}

bool CGPDeviceDb::getKeyForSourceId(uint32_t i_source_id, NSEZSP::EmberKeyData& o_key) const
{
    auto search = this->gp_dev_list.find(i_source_id);
    if (search != this->gp_dev_list.end()) {
        o_key = search->second;
        return true;
    }
    else {
        return false;
    }
}