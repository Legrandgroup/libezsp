/**
 * @file dummy_db.h
 *
 * @brief Dummy database, just to have link between eui64 and short address
 */

#pragma once

#include <vector>

#include "domain/ezsp-protocol/ezsp-enum.h"


class CProduct
{
public:
    CProduct(EmberEUI64 i_eui64, EmberNodeId i_addr) : eui64(i_eui64), addr(i_addr){;}
    EmberEUI64 eui64;
    EmberNodeId addr;
};

class Cdb
{
public:
	Cdb();

    bool addProduct(const EmberEUI64 i_eui64, const EmberNodeId i_addr);

    EmberNodeId getProductAddress(const EmberEUI64 i_eui64) const;
    EmberEUI64 getProductEui(const EmberNodeId i_addr) const;

    // awfull, just to be quick
    EmberEUI64 dongleEui64;

private:
    std::vector<CProduct> products;
};
