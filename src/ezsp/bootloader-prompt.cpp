/**
 * @file bootloader-prompt.cpp
 **/

#include <iostream>
#include <string>
#include <algorithm>
#include <sstream>
#include <iomanip>

#include "bootloader-prompt.h"

#include "spi/ILogger.h"

DEFINE_ENUM(Stage, BOOTLOADER_STAGE_LIST, NSEZSP::EBootloader)

using NSEZSP::CBootloaderPrompt;

CBootloaderPrompt::CBootloaderPrompt(const NSSPI::TimerBuilder& i_timer_builder) :
	timer(i_timer_builder.create()),
	accumulatedBytes(),
	bootloaderCLIChecked(false),
	state(EBootloader::Stage::RX_FLUSH),
	bootloaderWriteFunc(nullptr),
	promptDetectCallback(nullptr),
	firmwareTransferStartFunc(nullptr)
{
}

std::string CBootloaderPrompt::trim(const std::string &s)
{
	auto start = s.begin();
	while (start != s.end() && (std::isspace(*start)!=0)) {
		start++;
	}

	auto end = s.end();
	do {
		end--;
	} while (std::distance(start, end) > 0 && (std::isspace(*end)!=0));

	return std::string(start, end + 1);
}

void CBootloaderPrompt::trigger(NSSPI::ITimer* triggeringTimer)
{
  clogD << "Initial flush is over\n";
  this->probe();
}

void CBootloaderPrompt::registerSerialWriteFunc(FBootloaderWriteFunc newWriteFunc)
{
  this->bootloaderWriteFunc = newWriteFunc;
}

void CBootloaderPrompt::registerPromptDetectCallback(std::function<void (void)> newObsPromptDetectCallback)
{
  this->promptDetectCallback = newObsPromptDetectCallback;
}


void CBootloaderPrompt::reset() {
	this->state = EBootloader::Stage::RX_FLUSH;
  accumulatedBytes.clear();
  bootloaderCLIChecked = false;
  this->firmwareTransferStartFunc = nullptr;  /* Remove any callback for image transfer */
  /* If we don't receive any byte after GECKO_QUIET_RX_TIMEOUT ms, assume we have flushed the RX */
  timer->start(CBootloaderPrompt::GECKO_QUIET_RX_TIMEOUT, this);
}

void CBootloaderPrompt::probe()
{
  static const uint8_t probeSeq[] = "\n";
	this->state = EBootloader::Stage::PROBE;
	if (this->bootloaderWriteFunc) {
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

	if (this->state != EBootloader::Stage::TOPLEVEL_MENU_PROMPT) {
    clogE << "Cannot type command without a valid prompt\n";
    return false;
  }
  if (this->bootloaderWriteFunc)
  {
    size_t writtenBytes;
    clogD << "Entering run command\n";
	this->state = EBootloader::Stage::RX_FLUSH; /* Reset our internal state (not in menu anymore) */
    this->bootloaderWriteFunc(writtenBytes, cmdSeq, sizeof(cmdSeq) -1 );  /* We don't send the terminating '\0' of cmdSeq */
  }
  else
  {
    clogE << "Cannot type commands on CLI because no write functor is available\n";
  }
  return true;
}

bool CBootloaderPrompt::selectModeUpgradeFw(FFirmwareTransferStartFunc callback) {
  static const uint8_t cmdSeq[] = "1";

	if (this->state != EBootloader::Stage::TOPLEVEL_MENU_PROMPT) {
    clogE << "Cannot type command without a valid prompt\n";
    return false;
  }
  if (this->bootloaderWriteFunc)
  {
    size_t writtenBytes;
    clogD << "Entering upload ebl command\n";
	this->state = EBootloader::Stage::XMODEM_READY_CHAR_WAIT;  /* We are now waiting for the X-modem transfer ready character */
    this->firmwareTransferStartFunc = callback;
    this->bootloaderWriteFunc(writtenBytes, cmdSeq, sizeof(cmdSeq) -1 );  /* We don't send the terminating '\0' of cmdSeq */
  }
  else
  {
    clogE << "Cannot type commands on CLI because no write functor is available\n";
  }
  return true;
}

NSEZSP::EBootloader::Stage CBootloaderPrompt::decode(NSSPI::ByteBuffer& i_data) {
  uint8_t val;

  if (i_data.empty())
  {
    return this->state;
  }
	if (this->state == EBootloader:: Stage::RX_FLUSH) {
    /* We are flushing the leading bytes, discard input and restart the initial RX timer */
    //clogD << "Received " << i_data.size() << " bytes while in flush mode (discarding)\n";
    i_data.clear();
    timer->stop();
    timer->start(CBootloaderPrompt::GECKO_QUIET_RX_TIMEOUT, this);
    return state;
  }
	else if (this->state == EBootloader::Stage::XMODEM_READY_CHAR_WAIT) {
		/* Note: it is safe to invoke method bak() because we know that i_data is not empty from the test above */
		if (i_data.back() == 'C') { /* 'C' is the X-modem ready character */
      clogD << "Got the X-modem ready character from adapter\n";
		if (this->firmwareTransferStartFunc) {
		this->state = EBootloader::Stage::XMODEM_XFR;
        this->firmwareTransferStartFunc();  /* Invoke the firmware transfer ready function that has been set in selectModeUpgradeFw() */
        this->firmwareTransferStartFunc = nullptr;  /* Remove the callback */
        this->probe();  /* Restart probing to find out if we are back in the bootloader prompt */
      }
    }
  }
  bool lastRXByteIsNUL(false);
  while( !i_data.empty() )
  {
    if (lastRXByteIsNUL)
    {
      accumulatedBytes.clear(); /* If we receive additional bytes after '\0', discard all bytes prior (and including) '\0', and start accumulating again */
    }
    lastRXByteIsNUL = false;  /* For now, there is a new byte after the last '\0' */
    val = i_data.front();
    i_data.erase(i_data.begin());
    if (val != 0x00)
    {
      accumulatedBytes.push_back(val);
    }
    else
    {
      /* 0x00 will reset the parser, note that most EZSP adapters tend to send '\0' right after the bootloader prompt, so we want to parse that prompt,
         and clear the accumulatedBytes buffer only when we receive bytes next time */
      lastRXByteIsNUL=true;
    }
  }
  /* Note: from now on, there can be no 0x00 byte inside the buffer, thus we can convert it to a string */
  std::string str(accumulatedBytes.begin(), accumulatedBytes.end());
  std::stringstream msg;
  msg << "Accumulated buffer:";
  for (size_t loop=0; loop<accumulatedBytes.size(); loop++) {
      msg << " " << std::hex << std::setw(2) << std::setfill('0') <<
          +(static_cast<const unsigned int>(accumulatedBytes[loop]));
  }
  msg << "\n";
  clogD << msg.str();
  clogD << "Equivalent string: \"" << str << "\"\n";
  size_t blh = str.find(CBootloaderPrompt::GECKO_BOOTLOADER_HEADER);
  if (blh != std::string::npos)
  {
    /* We found the Gecko bootloader header, this is good news */
    if (EBootloader::Stage::PROBE != state) {
      clogW << "Got a bootloader header while not in probe mode, we will reset to TOPLEVEL_MENU_HEADER state anyway\n";
    }
		state = EBootloader::Stage::TOPLEVEL_MENU_HEADER;
    clogD << "Got bootloader header at position \"" << static_cast<unsigned int>(blh) << "\"\n";
    accumulatedBytes.erase(accumulatedBytes.begin(), accumulatedBytes.begin() + blh + CBootloaderPrompt::GECKO_BOOTLOADER_HEADER.length()); /* Remove all text up to (and including) the bootloader header from accumulated bytes (has been parsed) */
    str = std::string(accumulatedBytes.begin(), accumulatedBytes.end());  /* Accumulated buffer has been updated, reconstruct str */
  }
	if (EBootloader::Stage::TOPLEVEL_MENU_HEADER == state) {
    size_t eolChar = str.find('\n');
    if (eolChar != std::string::npos)
    {
		state = EBootloader::Stage::TOPLEVEL_MENU_CONTENT;
      std::string version(trim(std::string(str.begin(), str.begin() + eolChar - 1)));
      accumulatedBytes.erase(accumulatedBytes.begin(), accumulatedBytes.begin() + eolChar); /* Remove the version string from accumulated bytes (has been parsed) */
      clogD << "Got version \"" << version << "\"\n";
      str = std::string(accumulatedBytes.begin(), accumulatedBytes.end());  /* Accumulated buffer has been updated, reconstruct str */
    }
  }
	if (EBootloader::Stage::TOPLEVEL_MENU_HEADER == state ||
	    EBootloader::Stage::TOPLEVEL_MENU_CONTENT == state) {
    size_t blPrompt = str.find(CBootloaderPrompt::GECKO_BOOTLOADER_PROMPT);
    if (blPrompt != std::string::npos) {
			state = EBootloader::Stage::TOPLEVEL_MENU_PROMPT;
      //clogD << "Got bootloader prompt at position \"" << static_cast<unsigned int>(blPrompt) << "\"\n";
      accumulatedBytes.erase(accumulatedBytes.begin(), accumulatedBytes.begin() + blPrompt); /* Remove all text up to (and including) the bootloader prompt from accumulated bytes (has been parsed) */
      /* Note: the line below is commented-out because there is no code that continues parsing str, so even if it is now different
         from what is inside accumulatedBytes, we'd rather not add useless processing (creation of a string that will then be destroyed).
         If more processing on str is added below, that line should however be uncommented */
      //str = std::string(accumulatedBytes.begin(), accumulatedBytes.end());  /* Accumulated buffer has been updated, reconstruct str */
      if (this->promptDetectCallback)
      {
        clogD << "Invoking promptDetectCallback\n";
        this->promptDetectCallback();
      }
    }
  }
  if (lastRXByteIsNUL)
  {
    accumulatedBytes.clear();
  }
  return state;
}

const std::string CBootloaderPrompt::GECKO_BOOTLOADER_HEADER = "Gecko Bootloader";
const std::string CBootloaderPrompt::GECKO_BOOTLOADER_PROMPT = "BL >";
const uint16_t CBootloaderPrompt::GECKO_QUIET_RX_TIMEOUT = 100; /* If there are more than 100ms between two RX bytes while flushing, assume RX flush is done */
