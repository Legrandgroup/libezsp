/**
 * @file synchroneThread.cpp
 *
 * @brief This thread is used in the onReceivedZdpActiveEp observer to synchronize 
 *        the sending and the response of the ZDP_SIMPLE_DESC command.
 *        This avoids recording several endpoints or missing some
 */
#include <synchroneThread.h>
#include <iostream>

using NSMAIN::SynchroneThread;

SynchroneThread::SynchroneThread(NSEZSP::CEzsp& libEzsp, NSEZSP::EmberNodeId address, std::vector<uint8_t> ep_list) :
        stop(false),
        libEzsp(libEzsp),
        address(address),
        ep_list(ep_list){}

void SynchroneThread::operator()() {
    while (!stop) {
        std::unique_lock<std::mutex> lock(mutex);

        if (stop) {
            break;
        }

        if(loop == ep_list.size() - 1){
            stopThread();
        }else{
            std::cout << "Thread running..." << std::endl;
            payload.clear();
            payload.push_back(NSEZSP::u16_get_lo_u8(address));
            payload.push_back(NSEZSP::u16_get_hi_u8(address));
            payload.push_back(ep_list[loop]);
            libEzsp.SendZDOCommand(address,NSEZSP::ZDP_SIMPLE_DESC,payload);
            loop++;
        }

        cv.wait(lock);
    }
    std::cout << "Thread stopped." << std::endl;
}

void SynchroneThread::stopThread() {
    stop = true;
    cv.notify_one();
}

void SynchroneThread::event() {
    cv.notify_one();
}