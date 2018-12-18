/**
 * @file dummy_db.cpp
 *
 * @brief Dummy database, just to have link between eui64 and short address
 */

#include "dummy_db.h"

Cdb::Cdb() :
	dongleEui64(),
	products()
{
}

bool Cdb::addProduct(const EmberEUI64 i_eui64, const EmberNodeId i_addr)
{
    bool lo_adding = false;
    // check if already present
    if( INVALID_NODE_ID == getProductAddress(i_eui64) )
    {
        CProduct p(i_eui64, i_addr);
        products.push_back(p);
        lo_adding = true;        
    }

    return lo_adding;
}

EmberNodeId Cdb::getProductAddress(const EmberEUI64 i_eui64) const
{
    EmberNodeId l_addr = INVALID_NODE_ID;

    for(auto product : products)
    {
        if( product.eui64 == i_eui64 )
        {
            l_addr = product.addr;
        }
    }

    return l_addr;
}

EmberEUI64 Cdb::getProductEui(const EmberNodeId i_addr) const
{
    EmberEUI64 l_eui;

    for(auto product : products)
    {
        if( product.addr == i_addr )
        {
            l_eui = product.eui64;
        }
    }

    return l_eui;
}
