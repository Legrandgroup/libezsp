#pragma once

#include <cstdint>
#include <vector>
#include <memory>	// For std::unique_ptr
#include <functional>   // For std::function

#include "spi/TimerBuilder.h"

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
};


class CBootloaderPromptCallback
{
public:
    virtual ~CBootloaderPromptCallback() { }
    virtual void blpCbInfo( ) = 0;
};

class CBootloaderPrompt
{
public:
    static const std::string GECKO_BOOTLOADER_HEADER;
    static const std::string GECKO_BOOTLOADER_PROMPT;
    static const uint16_t GECKO_QUIET_RX_TIMEOUT;

    typedef std::function<int (size_t& writtenCnt, const void* buf, size_t cnt)> FBootloaderWriteFunc;    /*!< Callback type for method registerSerialWriteFunc() */

    CBootloaderPrompt() = delete; /* Construction without arguments is not allowed */
    /**
     * ipCb : call to inform state of ash
     * ipTimer : timer object pass to ash module to manage internal timer
     */
    CBootloaderPrompt(CBootloaderPromptCallback *ipCb, TimerBuilder &i_timer_factory);

    CBootloaderPrompt(const CBootloaderPrompt&) = delete; /* No copy construction allowed */

    CBootloaderPrompt& operator=(CBootloaderPrompt) = delete; /* No assignment allowed */

    /**
     * @brief Register a serial write functor
     *
     * @param newWriteFunc A callback function of type int func(size_t& writtenCnt, const void* buf, size_t cnt), that we will invoke to write bytes to the serial port we are decoding (or nullptr to disable callbacks)
     */
    void registerSerialWriteFunc(FBootloaderWriteFunc newWriteFunc);

    void reset();

    void probe();

    bool selectModeRun();

    EBootloaderStage decode(std::vector<uint8_t> &i_data);

private:
    std::unique_ptr<ITimer> timer;  /*!< A pointer to a timer instance */
    CBootloaderPromptCallback *pCb; /*!< A callback to invoke when bootloader prompt has been reached */
    std::vector<uint8_t> accumulatedBytes;  /*!< The current accumulated incoming bytes (not yet parsed) */
    bool bootloaderCLIChecked;  /*!< Did we validate that we are currently in bootloader prompt mode? */
    EBootloaderStage state; /*!< The current state in which we guess the bootloader is currently on the NCP */
    FBootloaderWriteFunc bootloaderWriteFunc;   /*!< A function to write bytes to the serial port */
};
