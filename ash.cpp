/**
 * 
 * */

#include "ash.h"

using namespace std;

/**
 * The receive timeout settings - min/initial/max - defined in milliseconds
 */
#define T_RX_ACK_MIN  400
#define T_RX_ACK_INIT 1600
#define T_RX_ACK_MAX 3200

#define ASH_CANCEL_BYTE     0x1A
#define ASH_FLAG_BYTE       0x7E
#define ASH_SUBSTITUTE_BYTE 0x18
#define ASH_XON_BYTE        0x11
#define ASH_OFF_BYTE        0x13
#define ASH_TIMEOUT         -1

#define ASH_MAX_LENGTH 131

CAsh::CAsh()
{
    in_msg.clear();
    ackNum = 0;
    frmNum = 0;
    seq_num = 0;
    stateConnected = false;

    timer.AddObs(this);

}

void CAsh::Update(const CObservable* observable) const
{
  //on affiche l'état de la variable
  //cout<<observable->Statut()<<endl;
}

uint16_t CAsh::computeCRC( vector<uint8_t> i_msg )
{
  uint16_t lo_crc = 0xFFFF; // initial value
  uint16_t polynomial = 0x1021; // 0001 0000 0010 0001 (0, 5, 12)

  for (int cnt = 0; cnt < i_msg.size(); cnt++) {
      for (int i = 0; i < 8; i++) {
          bool bit = ((i_msg.at(cnt) >> (7 - i) & 1) == 1);
          bool c15 = ((lo_crc >> 15 & 1) == 1);
          lo_crc <<= 1;
          if (c15 ^ bit) {
              lo_crc ^= polynomial;
          }
      }
  }

  lo_crc &= 0xffff;

  return lo_crc;
}

vector<uint8_t> CAsh::resetNCPFrame(void)
{
    ackNum = 0;
    frmNum = 0;
    seq_num = 0;
    stateConnected = false;
    vector<uint8_t> lo_msg;

    timer.stop();

    lo_msg.push_back(0xC0);

    uint16_t crc = computeCRC(lo_msg);
    lo_msg.push_back((uint8_t)(crc>>8));
    lo_msg.push_back((uint8_t)(crc&0xFF));

    lo_msg = stuffedOutputData(lo_msg);

    lo_msg.insert( lo_msg.begin(), ASH_CANCEL_BYTE );

    // start timer
    timer.start( T_RX_ACK_INIT );

    return lo_msg; // { 0x1A, 0xC0, 0x38, 0xBC, 0x7E }
}



/**
 * PRIVATE FUNCTION
 */


vector<uint8_t> CAsh::stuffedOutputData(vector<uint8_t> i_msg)
{
  vector<uint8_t> lo_msg;

  for (int cnt = 0; cnt < i_msg.size(); cnt++) {
      switch (i_msg.at(cnt)) {
          case 0x7E:
              lo_msg.push_back( 0x7D );
              lo_msg.push_back( 0x5E );
              break;
          case 0x7D:
              lo_msg.push_back( 0x7D );
              lo_msg.push_back( 0x5D );
              break;
          case 0x11:
              lo_msg.push_back( 0x7D );
              lo_msg.push_back( 0x31 );
              break;
          case 0x13:
              lo_msg.push_back( 0x7D );
              lo_msg.push_back( 0x33 );
              break;
          case 0x18:
              lo_msg.push_back( 0x7D );
              lo_msg.push_back( 0x38 );
              break;
          case 0x1A:
              lo_msg.push_back( 0x7D );
              lo_msg.push_back( 0x3A );
              break;
          default:
              lo_msg.push_back( i_msg.at(cnt) );
              break;
      }
  }
  lo_msg.push_back( 0x7E );


  return lo_msg;
}