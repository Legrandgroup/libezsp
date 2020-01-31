#pragma once

#include <cstdint>
#include <vector>
#include <memory>	// For std::unique_ptr
#include <functional>   // For std::function

#include "spi/TimerBuilder.h"

namespace NSEZSP {

/**
 * @brief Internal states for CLibEzspMain
 * 
 * @note Not all these states are exposed to the outside, only CLibEzspState states (and the related changes) are notified
 *       A mapping between CLibEzspInternalState and CLibEzspMain can be found in method setState()
 */
enum class EBootloaderStage {
    RX_FLUSH,               /*<! Initial state, first flush all incoming bytes from serial link */
    PROBE,                  /*<! We don't know yet if we are in bootloader mode, we are just probing */
    TOPLEVEL_MENU_HEADER,   /*<! Toplevel menu header, displaying Gecko Bootloader v1.6.0 */
    TOPLEVEL_MENU_CONTENT,  /*<! Toplevel menu content, displaying a menu with the various numeric options */
    TOPLEVEL_MENU_PROMPT,   /*<! String "BL >" following the toplevel menu header */
    XMODEM_READY_CHAR_WAIT, /*<! Waiting for successive 'C' characters transmitted by the bootloader (this means an incoming firmware image transfer using X-modem is expected by the bootloader) */
    XMODEM_XFR,             /*<! A firmware image transfer using X-modem is ongoing */
};


class CBootloaderPromptCallback
{
public:
    virtual ~CBootloaderPromptCallback() { }
    virtual void blpCbInfo( ) = 0;
};

class CBootloaderPrompt : protected NSSPI::ITimerVisitor
{
public:
    static const std::string GECKO_BOOTLOADER_HEADER;
    static const std::string GECKO_BOOTLOADER_PROMPT;
    static const uint16_t GECKO_QUIET_RX_TIMEOUT;

    typedef std::function<int (size_t& writtenCnt, const uint8_t* buf, size_t cnt)> FBootloaderWriteFunc;    /*!< Callback type for method registerSerialWriteFunc() */
    typedef std::function<void (void)> FFirmwareTransferStartFunc;  /*!< Callback type for method selectModeUpgradeFw() */

    CBootloaderPrompt() = delete; /* Construction without arguments is not allowed */
    /**
     * ipCb : call to inform state of ash
     * ipTimer : timer object pass to ash module to manage internal timer
     */
    CBootloaderPrompt(CBootloaderPromptCallback *ipCb, NSSPI::TimerBuilder &i_timer_factory);

    CBootloaderPrompt(const CBootloaderPrompt&) = delete; /* No copy construction allowed */

    CBootloaderPrompt& operator=(CBootloaderPrompt) = delete; /* No assignment allowed */

    /**
     * @brief Register a serial write functor
     *
     * @param newWriteFunc A callback function of type int func(size_t& writtenCnt, const void* buf, size_t cnt), that we will invoke to write bytes to the serial port we are decoding (or nullptr to disable callbacks)
     */
    void registerSerialWriteFunc(FBootloaderWriteFunc newWriteFunc);

    void registerPromptDetectCallback(std::function<void (void)> newObsPromptDetectCallback);

    void reset();

    void probe();

    /**
     * @brief Select the application run bootloader menu
     * 
     * @note As this method interacts with the booloader menu prompt, we should already be waiting on the bootloader prompt
     */
    bool selectModeRun();

    /**
     * @brief Select the firmware upgrade bootloader menu
     * 
     * @param callback Is a method of type void func(void) that will be invoked when the bootloader is waiting for an image transfer
     * 
     * @note As this method interacts with the booloader menu prompt, we should already be waiting on the bootloader prompt
     */
    bool selectModeUpgradeFw(FFirmwareTransferStartFunc callback);

    EBootloaderStage decode(std::vector<uint8_t> &i_data);

protected:
    void trigger(NSSPI::ITimer* triggeringTimer);

private:
    std::unique_ptr<NSSPI::ITimer> timer;  /*!< A pointer to a timer instance */
    CBootloaderPromptCallback *pCb; /*!< A callback to invoke when bootloader prompt has been reached */
    std::vector<uint8_t> accumulatedBytes;  /*!< The current accumulated incoming bytes (not yet parsed) */
    bool bootloaderCLIChecked;  /*!< Did we validate that we are currently in bootloader prompt mode? */
    EBootloaderStage state; /*!< The current state in which we guess the bootloader is currently on the NCP */
    FBootloaderWriteFunc bootloaderWriteFunc;   /*!< A function to write bytes to the serial port */
    std::function<void (void)> promptDetectCallback;    /*!< A callback function invoked when the bootloader prompt is reached */
    FFirmwareTransferStartFunc firmwareTransferStartFunc;  /*!< A callback function invoked when the bootloader is is waiting for an image transfer */
};

}
