/**
 * 
 */

#include "dummy_db.h"

bool Cdb::addProduct(EmberEUI64 i_eui64, EmberNodeId i_addr)
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

EmberNodeId Cdb::getProductAddress(EmberEUI64 i_eui64)
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

EmberEUI64 Cdb::getProductEui(EmberNodeId i_addr)
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