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
    std::cout <<  "ashCbInfo : " << info << std::endl; 

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

        if( nullptr != pHandler ) {
            pHandler->ashRxMessage(lo_msg);
            EzspProcess( lo_msg );
        }

        // remove waiting message and send next
        if( wait_rsp )
        {
            sendingMsgQueue.pop();
            wait_rsp = false;
            sendNextMsg();
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

void CEzspDongle::EzspProcess( std::vector<uint8_t> i_rx_msg )
{
    EEzspCmd l_cmd = static_cast<EEzspCmd>(i_rx_msg.at(2));

    // keep only payload
    i_rx_msg.erase(i_rx_msg.begin(),i_rx_msg.begin()+3);

    // first manage handler and then search if callback is associate to a particular request
    switch( l_cmd )
    {
        case EZSP_TIMER_HANDLER :
        case EZSP_DEBUG_HANDLER :
        case EZSP_STACK_STATUS_HANDLER :
        case EZSP_ENERGY_SCAN_RESULT_HANDLER :
        case EZSP_NETWORK_FOUND_HANDLER :
        case EZSP_SCAN_COMPLETE_HANDLER :
        case EZSP_CHILD_JOIN_HANDLER :
        case EZSP_REMOTE_SET_BINDING_HANDLER :
        case EZSP_REMOTE_DELETE_BINDING_HANDLER :
        case EZSP_MESSAGE_SENT_HANDLER :
        case EZSP_POLL_COMPLETE_HANDLER :
        case EZSP_POLL_HANDLER :
        case EZSP_INCOMING_SENDER_EUI64_HANDLER :
        case EZSP_INCOMING_MESSAGE_HANDLER :
        case EZSP_INCOMING_ROUTE_RECORD_HANDLER :
        case EZSP_INCOMING_MANY_TO_ONE_ROUTE_REQUEST_HANDLER :
        case EZSP_INCOMING_ROUTE_ERROR_HANDLER :
        case EZSP_ID_CONFLICT_HANDLER :
        case EZSP_MAC_PASSTHROUGH_MESSAGE_HANDLER :
        case EZSP_MAC_FILTER_MATCH_MESSAGE_HANDLER :
        case EZSP_RAW_TRANSMIT_COMPLETE_HANDLER :
        case EZSP_SWITCH_NETWORK_KEY_HANDLER :
        case EZSP_ZIGBEE_KEY_ESTABLISHMENT_HANDLER :
        case EZSP_TRUST_CENTER_JOIN_HANDLER :
        case EZSP_GENERATE_CBKE_KEYS_HANDLER :
        case EZSP_CALCULATE_SMACS_HANDLER :
        case EZSP_DSA_SIGN_HANDLER :
        case EZSP_DSA_VERIFY_HANDLER :
        case EZSP_MFGLIB_RX_HANDLER :
        case EZSP_INCOMING_BOOTLOAD_MESSAGE_HANDLER :
        case EZSP_BOOTLOAD_TRANSMIT_COMPLETE_HANDLER :
        {
            pHandler->ezspHandler( l_cmd, i_rx_msg );
        }
        break;

        default :
        {
            // verify that callback are associate to this command
            sMsg l_msg = sendingMsgQueue.front();
            if( l_msg.i_cmd == l_cmd )
            {
                if( nullptr != l_msg.cb )
                {
                    l_msg.cb(l_cmd,i_rx_msg);
                }
            }
        }
        break;
    }
}


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

