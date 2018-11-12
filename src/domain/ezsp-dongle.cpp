/**
 * 
 */

#include "ezsp-dongle.h"


CEzspDongle::CEzspDongle( CEzspHandler *ipCb )
{
    pUart = nullptr;
    pCb = ipCb;
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
        }
    }

    return lo_success;
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

        // send ack
        std::vector<uint8_t> l_msg = ash->AckFrame();
        pUart->write(l_size, l_msg.data(), l_msg.size());

        if( nullptr != pCb ) {
            pCb->ashRxMessage(lo_msg);
            EzspProcess( lo_msg );
        }
      }    
}

void CEzspDongle::sendCommand(EEzspCmd i_cmd, std::vector<uint8_t> i_cmd_payload, 
                                std::function<void (EEzspCmd i_cmd, std::vector<uint8_t> i_msg_receive)> callBackFunction )
{
    // encode command using ash and write to uart
    std::vector<uint8_t> li_data;
    std::vector<uint8_t> l_enc_data;
    size_t l_size;

    li_data.clear();
    li_data.push_back(static_cast<uint8_t>(i_cmd));
    for( size_t loop=0; loop< i_cmd_payload.size(); loop++ )
    {
        li_data.push_back(i_cmd_payload.at(loop));
    }

    l_enc_data = ash->DataFrame(li_data);
    if( nullptr != pUart )
    {
        pUart->write(l_size, l_enc_data.data(), l_enc_data.size());
    }

    // store callback for answer
    if( nullptr != callBackFunction )
    {
        rspCbTable.push_back({i_cmd,callBackFunction});
    }
}

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
            pCb->ezspHandler( l_cmd, i_rx_msg );
        }
        break;

        default :
        {
            // verify that callback are associate to this command
            for(size_t loop=0; loop<rspCbTable.size(); loop++)
            {
                if( rspCbTable.at(loop).i_cmd == l_cmd )
                {
                    rspCbTable.at(loop).cb(l_cmd,i_rx_msg);
                    rspCbTable.erase(rspCbTable.begin()+loop);
                    loop = 0;
                }
            }

        }
        break;
    }
}
