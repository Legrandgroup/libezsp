/**
 * 
 */

#include "ezsp-dongle.h"


CEzspDongle::CEzspDongle( CDongleHandler *ipCb )
{
    wait_rsp = false;
    pUart = nullptr;
    pHandler = ipCb;
    ash = new CAsh(static_cast<CAshCallback*>(this), nullptr);
}

CEzspDongle::~CEzspDongle()
{
    pUart = nullptr;
    delete ash;
}

bool CEzspDongle::open(IUartDriver *ipUart)
{
    bool lo_success = true;
    std::vector<uint8_t> l_buffer;
    size_t l_size;

    if( nullptr == ipUart )
    {
        lo_success = false;
    }
    else
    {
        pUart = ipUart;

        // reset ash ncp
        l_buffer = ash->resetNCPFrame();

        if( pUart->write(l_size, l_buffer.data(), l_buffer.size()) < 0 )
        {
            // error
            lo_success = false;
            pUart = nullptr;
        }
        else
        {
            if( l_size != l_buffer.size() )
            {
                // error size mismatch
                lo_success = false;
                pUart = nullptr;
            }
            else
            {
                std::cout << "CEzspDongle::open register uart !" << std::endl;
                uartIncomingDataHandler.registerObserver(this);
                pUart->setIncomingDataHandler(&uartIncomingDataHandler);
            }
        }
    }

    return lo_success;
}

void CEzspDongle::ashCbInfo( EAshInfo info ) 
{ 
    std::cout <<  "ashCbInfo : " << CAsh::EAshInfoToString(info) << std::endl; 

    if( ASH_STATE_CHANGE == info )
    {
        // inform upper layer that dongle is ready !
        if( ash->isConnected() )
        {
            pHandler->dongleState( DONGLE_READY );
        }
        else
        {
            pHandler->dongleState( DONGLE_REMOVE );
        }
    }
}

void CEzspDongle::handleInputData(const unsigned char* dataIn, const size_t dataLen)
 {
    std::vector<uint8_t> li_data;
    std::vector<uint8_t> lo_msg;

    li_data.clear();
    for( size_t loop=0; loop< dataLen; loop++ )
    {
        li_data.push_back(dataIn[loop]);
    }

    lo_msg = ash->decode(&li_data);

    // send incomming mesage to application
    if( !lo_msg.empty() )
    {
        size_t l_size;

        //std::cout << "CEzspDongle::handleInputData ash message decoded" << std::endl;

        // send ack
        std::vector<uint8_t> l_msg = ash->AckFrame();
        pUart->write(l_size, l_msg.data(), l_msg.size());

        // call handler

        // low level
        if( nullptr != pHandler ) { pHandler->ashRxMessage(lo_msg); }

        // ezsp
        // extract ezsp command
        EEzspCmd l_cmd = static_cast<EEzspCmd>(lo_msg.at(2));
        // keep only payload
        lo_msg.erase(lo_msg.begin(),lo_msg.begin()+3);    

        // response to a sending command
        sMsg l_msgQ = sendingMsgQueue.front();
        if( l_msgQ.i_cmd == l_cmd )
        {
            if( nullptr != l_msgQ.cb )
            {
                l_msgQ.cb(l_cmd,lo_msg);
            }

            // remove waiting message and send next
            sendingMsgQueue.pop();
            wait_rsp = false;
            sendNextMsg();
        }
        else
        {
            // ezsp callback
            if( nullptr != pHandler ) 
            { 
                pHandler->ezspHandler( l_cmd, lo_msg ); 
            }
        }
    }    
}

void CEzspDongle::sendCommand(EEzspCmd i_cmd, std::vector<uint8_t> i_cmd_payload, 
                                std::function<void (EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive)> callBackFunction )
{
    sMsg l_msg;

    l_msg.i_cmd = i_cmd;
    l_msg.payload = i_cmd_payload;
    l_msg.cb = callBackFunction;
    
    sendingMsgQueue.push(l_msg);

    sendNextMsg();
}


/**
 * 
 * PRIVATE
 * 
 */

void CEzspDongle::sendNextMsg( void )
{
    if( (!wait_rsp) && (!sendingMsgQueue.empty()) )
    {
        sMsg l_msg = sendingMsgQueue.front();

        // encode command using ash and write to uart
        std::vector<uint8_t> li_data;
        std::vector<uint8_t> l_enc_data;
        size_t l_size;

        li_data.clear();
        li_data.push_back(static_cast<uint8_t>(l_msg.i_cmd));
        for( size_t loop=0; loop< l_msg.payload.size(); loop++ )
        {
            li_data.push_back(l_msg.payload.at(loop));
        }

        //-- std::cout << "CEzspDongle::sendCommand ash->DataFrame" << std::endl;
        l_enc_data = ash->DataFrame(li_data);
        if( nullptr != pUart )
        {
            //-- std::cout << "CEzspDongle::sendCommand pUart->write" << std::endl;
            pUart->write(l_size, l_enc_data.data(), l_enc_data.size());

            wait_rsp = true;
        }
    }
}

