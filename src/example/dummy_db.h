/**
 * @file dummy_db.h
 *
 * @brief Dummy database, just to have link between eui64 and short address
 */
#pragma once

#include <vector>

#include "../domain/ezsp-protocol/ezsp-enum.h"


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
    bool addProduct(EmberEUI64 i_eui64, EmberNodeId i_addr);

    EmberNodeId getProductAddress(EmberEUI64 i_eui64);
    EmberEUI64 getProductEui(EmberNodeId i_addr);

    // awfull, just to be quick
    EmberEUI64 dongleEui64;

private:
    std::vector<CProduct> products;
};
