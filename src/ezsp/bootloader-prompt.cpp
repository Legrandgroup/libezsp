/**
 * @file bootloader-prompt.cpp
 **/

#include <iostream>

#include "bootloader-prompt.h"

#include "spi/ILogger.h"
#include <sstream>
#include <iomanip>

using namespace std;

CBootloaderPrompt::CBootloaderPrompt(CBootloaderPromptCallback *ipCb, TimerBuilder &i_timer_factory) :
	timer(i_timer_factory.create()),
  pCb(nullptr),
  accumulatedBytes(),
  bootloaderCLIChecked(false),
  state(EBootloaderStage::RX_FLUSH),
  bootloaderWriteFunc(nullptr)
{
}

void CBootloaderPrompt::registerSerialWriteFunc(FBootloaderWriteFunc newWriteFunc)
{
  this->bootloaderWriteFunc = newWriteFunc;
}

void CBootloaderPrompt::reset()
{
  state = EBootloaderStage::RX_FLUSH;
  accumulatedBytes.clear();
  bootloaderCLIChecked = false;
  /* If we don't receive any byte after GECKO_QUIET_RX_TIMEOUT ms, assume we have flushed the RX */
  timer->start(CBootloaderPrompt::GECKO_QUIET_RX_TIMEOUT, [this](ITimer *ipTimer) { clogD << "Initial flush is over\n"; this->probe(); } );
}

void CBootloaderPrompt::probe()
{
  static const uint8_t probeSeq[] = "\n";
  this->state = EBootloaderStage::PROBE;
  if (this->bootloaderWriteFunc)
  {
    size_t writtenBytes;
    clogD << "Starting probe\n";
    this->bootloaderWriteFunc(writtenBytes, probeSeq, sizeof(probeSeq) -1 );  /* We don't send the terminating '\0' of probeSeq */
  }
  else
  {
    clogE << "Cannot start probing because no write functor is available\n";
  }
}

bool CBootloaderPrompt::selectModeRun()
{
  static const uint8_t cmdSeq[] = "2";

  if (this->state != EBootloaderStage::TOPLEVEL_MENU_PROMPT)
  {
    clogE << "Cannot type command without a valid prompt\n";
    return false;
  }
  if (this->bootloaderWriteFunc)
  {
    size_t writtenBytes;
    clogD << "Entering run command\n";
    this->bootloaderWriteFunc(writtenBytes, cmdSeq, sizeof(cmdSeq) -1 );  /* We don't send the terminating '\0' of cmdSeq */
  }
  else
  {
    clogE << "Cannot type commands on CLI because no write functor is available\n";
  }
  return true;
}

EBootloaderStage CBootloaderPrompt::decode(std::vector<uint8_t> &i_data)
{
  uint8_t val;

  if (!i_data.empty() && this->state == EBootloaderStage::RX_FLUSH)
  {
    /* We are flushing the leading bytes, discard input and restart the initial RX timer */
    //clogD << "Received " << i_data.size() << " bytes while in flush mode (discarding)\n";
    i_data.clear();
    timer->stop();
    timer->start(CBootloaderPrompt::GECKO_QUIET_RX_TIMEOUT, [this](ITimer *ipTimer) { clogD << "Initial flush is over\n"; this->probe(); } );
    return state;
  }
  while( !i_data.empty() )
  {
    val = i_data.front();
    i_data.erase(i_data.begin());
    if (val != 0x00)
    {
      accumulatedBytes.push_back(val);
    }
    else
    {
      accumulatedBytes.clear(); /* 0x00 will reset the parser, note that most EZSP adapters tend to send '\0' right after the bootloader prompt */
      return state;
    }
  }
  /* Note: from now on, there can be no 0x00 byte inside the buffer, thus we can convert it to a string */
  std::string str(accumulatedBytes.begin(), accumulatedBytes.end());
  std::stringstream msg;
  msg << "Accumulated buffer:";
  for (size_t loop=0; loop<accumulatedBytes.size(); loop++)
      msg << " " << std::hex << std::setw(2) << std::setfill('0') <<
          +(static_cast<const unsigned int>(accumulatedBytes[loop]));
  msg << "\n";
  clogE << msg.str();
  clogE << "Equivalent string: \"" << str << "\"\n";
  size_t bootloaderPrompt = str.find(CBootloaderPrompt::GECKO_BOOTLOADER_HEADER);
  if (str.size()!=0 && str.back() == 'r')
  {
    size_t bootloaderPrompt = str.find(CBootloaderPrompt::GECKO_BOOTLOADER_HEADER);
    if (bootloaderPrompt != std::string::npos)
    {
      /* We found the Gecko bootloader header, this is good news */
      if (EBootloaderStage::PROBE != state)
      {
        clogW << "Got a bootloader header while not in probe mode, we will reset to TOPLEVEL_MENU_HEADER state anyway\n";
      }
      state=EBootloaderStage::TOPLEVEL_MENU_HEADER;
      accumulatedBytes.clear(); /* Remove the menu header from accumulated bytes (has been parsed) */
      return state;
    }
  }
  if (EBootloaderStage::TOPLEVEL_MENU_HEADER == state)
  {
    size_t eolChar = str.find('\n');
    if (eolChar != std::string::npos)
    {
      state = EBootloaderStage::TOPLEVEL_MENU_CONTENT;
      std::string version = str;
      clogE << "Got version \"" << version << "\"\n";
      /* FIXME: should tidy up this string (trailing \r\n, leading and trailing spaces) */
      accumulatedBytes.erase(accumulatedBytes.begin(), accumulatedBytes.begin() + eolChar); /* Remove the version string from accumulated bytes (has been parsed) */
      return state;
    }
  }
  if (EBootloaderStage::TOPLEVEL_MENU_HEADER == state ||
      EBootloaderStage::TOPLEVEL_MENU_CONTENT == state)
  {
    size_t blPrompt = str.find(CBootloaderPrompt::GECKO_BOOTLOADER_PROMPT);
    if (blPrompt != std::string::npos)
    {
      state = EBootloaderStage::TOPLEVEL_MENU_PROMPT;
      clogD << "Got bootloader prompt at position \"" << static_cast<unsigned int>(blPrompt) << "\"\n";
      accumulatedBytes.erase(accumulatedBytes.begin(), accumulatedBytes.begin() + blPrompt); /* Remove all text up to (and including) the bootloader prompt from accumulated bytes (has been parsed) */
      this->selectModeRun();
      return state;
    }
  }
}

const std::string CBootloaderPrompt::GECKO_BOOTLOADER_HEADER = "Gecko Bootloader";
const std::string CBootloaderPrompt::GECKO_BOOTLOADER_PROMPT = "BL >";
const uint16_t CBootloaderPrompt::GECKO_QUIET_RX_TIMEOUT = 500; /* If there are more than 500ms between two RX bytes while flushing, assume RX flush is done */