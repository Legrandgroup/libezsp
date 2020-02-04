#pragma once

#include <cstdint>
#include <memory>	// For std::unique_ptr
#include <functional>   // For std::function

#include "spi/TimerBuilder.h"
#include "spi/ByteBuffer.h"

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
     * @brief Constructor
     *
     * @param i_timer_factory Timer builder object used to generate timers
     */
    CBootloaderPrompt(NSSPI::TimerBuilder &i_timer_factory);

    CBootloaderPrompt(const CBootloaderPrompt&) = delete; /* No copy construction allowed */

    CBootloaderPrompt& operator=(CBootloaderPrompt) = delete; /* No assignment allowed */

    /**
     * @brief Register a serial write functor
     *
     * @param newWriteFunc A callback function of type int func(size_t& writtenCnt, const void* buf, size_t cnt), that we will invoke to write bytes to the serial port we are decoding (or nullptr to disable callbacks)
     */
    void registerSerialWriteFunc(FBootloaderWriteFunc newWriteFunc);

    /**
     * @brief Register a prompt detection callback
     *
     * @param newObsPromptDetectCallback A callback function of that we will invoke each time we reach a bootloader prompt (or nullptr to disable callbacks)
     */
    void registerPromptDetectCallback(std::function<void (void)> newObsPromptDetectCallback);

    /**
     * @brief Reset the bootloader parser state
     */
    void reset();

    /**
     * @brief Start probing for a bootloader prompt
     *
     * We will try to stroke the prompt by entering a LF character in order to get an Ember serial bootloader header+prompt
     */
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

    /**
     * @brief Decode new incoming bytes output by the bootloader
     * 
     * @param i_data New bytes to add to the previously accumulated ones
     */
    EBootloaderStage decode(NSSPI::ByteBuffer& i_data);

protected:
    /**
     * @brief Internal method invoked on timeouts
     */
    void trigger(NSSPI::ITimer* triggeringTimer);
    
    /**
     * @brief Trim a string (removing leading and trailing whitespaces)
     * @param s The string to trim
     * @return A copy of @p s, without leading and trailing whitespaces
     */
    static std::string trim(const std::string &s);

private:
    std::unique_ptr<NSSPI::ITimer> timer;  /*!< A pointer to a timer instance */
    NSSPI::ByteBuffer accumulatedBytes;  /*!< The current accumulated incoming bytes (not yet parsed) */
    bool bootloaderCLIChecked;  /*!< Did we validate that we are currently in bootloader prompt mode? */
    EBootloaderStage state; /*!< The current state in which we guess the bootloader is currently on the NCP */
    FBootloaderWriteFunc bootloaderWriteFunc;   /*!< A function to write bytes to the serial port */
    std::function<void (void)> promptDetectCallback;    /*!< A callback function invoked when the bootloader prompt is reached */
    FFirmwareTransferStartFunc firmwareTransferStartFunc;  /*!< A callback function invoked when the bootloader is is waiting for an image transfer */
};

}
