/**
 * @file endpoint.h
 *
 * @brief endpoint object
 */
#pragma once

#include <spi/ByteBuffer.h>

namespace NSMAIN {

class CEndpoint {
    public:
        /**
         * @brief Default constructor
         *
         * Construction without arguments is not allowed
         */
        CEndpoint();

        /**
         * @brief Assignment operator
         *
         * Copy construction is forbidden on this class
         */
        CEndpoint& operator=(const CEndpoint& other) = default;

        /**
         * @brief Constructor with endpoint and cluster
         *
         * @param endpoint Endpoint ID
         * @param in_cluster vector of in cluster
         * @param out_cluster vector of out cluster
         */
        explicit CEndpoint(uint8_t endpoint, std::vector<uint16_t> in_cluster, std::vector<uint16_t> out_cluster);

        //Getter
        uint16_t getEndpoint() const {
            return endpoint;
        }

        std::vector<uint16_t> getClusterIn() const{
            return cluster_in;
        }

        std::vector<uint16_t> getClusterOut() const{
            return cluster_out;
        }

    private:
        uint8_t endpoint;
        std::vector<uint16_t> cluster_in;
        std::vector<uint16_t> cluster_out;
};

}