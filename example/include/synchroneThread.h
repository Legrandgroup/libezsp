/**
 * @file synchroneThread.h
 *
 * @brief This thread is used in the onReceivedZdpActiveEp observer to synchronize 
 *        the sending and the response of the ZDP_SIMPLE_DESC command.
 *        This avoids recording several endpoints or missing some
 */
#pragma once

#include <string>
#include <mutex>
#include <condition_variable>
#include <ezsp/ezsp.h>
#include <ezsp/zbmessage/zdp-enum.h>

namespace NSMAIN {
    
class SynchroneThread {
public:
    /**
     * @brief Default constructor
     *
     * @param libEzsp the CEzsp instance to use to communicate with the EZSP adapter
     * @param address node id
     * @param ep_list list of endpoint
     */
    explicit SynchroneThread(NSEZSP::CEzsp& libEzsp, NSEZSP::EmberNodeId address, std::vector<uint8_t> ep_list);

    /**
     * @brief Assignment operator
     *
     * Copy construction is forbidden on this class
     */
    SynchroneThread& operator=(const SynchroneThread& other) = default;

    /**
     * @brief Start function
     *
     * Function executed during the lifetime of the thread
     */
    void operator()();

    /**
     * @brief Stop the tread
     *
     * Stop the tread
     */
    void stopThread();

    /**
     * @brief Continue event 
     *
     * Notify the thread to continue its execution
     */
    void event();

private:
    bool stop;
    NSEZSP::CEzsp& libEzsp;
    NSEZSP::EmberNodeId address;
    std::vector<uint8_t> ep_list;
    NSSPI::ByteBuffer payload;
    uint8_t loop = 0;
    std::mutex mutex;
    std::condition_variable cv;
};

}