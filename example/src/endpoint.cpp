/**
 * @file endpoint.h
 *
 * @brief endpoint object
 */

#include <sstream>
#include <iomanip>
#include <iostream>

#include "endpoint.h"

using NSMAIN::CEndpoint;

CEndpoint::CEndpoint():
    endpoint(0),
    cluster_in(0),
    cluster_out(0){
}

CEndpoint::CEndpoint(uint8_t endpoint, std::vector<uint16_t> in_cluster, std::vector<uint16_t> out_cluster):
    endpoint(0),
    cluster_in(0),
    cluster_out(0){
        this->endpoint = endpoint;
        this->cluster_in = in_cluster;
        this->cluster_out = out_cluster;
}
