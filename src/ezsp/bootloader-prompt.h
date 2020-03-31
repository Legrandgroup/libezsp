/***
 * @file bootloader-prompt.h
 * 
 * @brief Ember bootloader CLI decoder
 */

#pragma once

#include <cstdint>
#include <memory>	// For std::unique_ptr
#include <functional>   // For std::function

#include "spi/TimerBuilder.h"
#include "spi/ByteBuffer.h"
#include "ezsp/enum-generator.h"

namespace NSEZSP {

#define BOOTLOADER_STAGE_LIST(XX) \
	XX(RX_FLUSH,=1)             /*<! Initial state, first flush all incoming bytes from serial link */ \
	XX(PROBE,)                  /*<! We don't know yet if we are in bootloader mode, we are just probing */ \
	XX(TOPLEVEL_MENU_HEADER,)   /*<! Toplevel menu header, displaying Gecko Bootloader v1.6.0 */ \
	XX(TOPLEVEL_MENU_CONTENT,)  /*<! Toplevel menu content, displaying a menu with the various numeric options */ \
	XX(TOPLEVEL_MENU_PROMPT,)   /*<! String "BL >" following the toplevel menu header */ \
	XX(XMODEM_READY_CHAR_WAIT,) /*<! Waiting for successive 'C' characters transmitted by the bootloader (this means an incoming firmware image transfer using X-modem is expected by the bootloader) */ \
	XX(XMODEM_XFR,)             /*<! A firmware image transfer using X-modem is ongoing */ \

/**
 * @brief Internal stages for bootloader prompt detection and interaction
 *
 * @note The lines above describes all states known in order to build both an enum and enum-to-string/string-to-enum methods
 *       In this macro, XX is a placeholder for the macro to use for building.
 *       We start numbering from 1, so that 0 can be understood as value not found for enum-generator.h
 * @see enum-generator.h
 */
class EBootloader {
public:
	DECLARE_ENUM(Stage, BOOTLOADER_STAGE_LIST);
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
     * @param i_timer_builder Timer builder object used to generate timers
     */
    explicit CBootloaderPrompt(const NSSPI::TimerBuilder& i_timer_builder);

    CBootloaderPrompt(const CBootloaderPrompt&) = delete; /* No copy construction allowed */

    CBootloaderPrompt& operator=(CBootloaderPrompt) = delete; /* No assignment allowed */

    /**
     * @brief Register a serial write functor
     *
     * @param newWriteFunc A callback function of type int func(size_t& writtenCnt, const void* buf, size_t cnt), that we will invoke to write bytes to the serial port we are decoding (or nullptr to disable callbacks)
     */
    void registerSerialWriteFunc(FBootloaderWriteFunc newWriteFunc);

	/**
	 * @brief Check if a serial write functor is registered
	 *
	 * @return true if a serial write functor is active or false otherwise
	 */
	bool hasARegisteredSerialWriteFunc() const;

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
    EBootloader::Stage decode(NSSPI::ByteBuffer& i_data);

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
    EBootloader::Stage state; /*!< The current state in which we guess the bootloader is currently on the NCP */
	FBootloaderWriteFunc serialWriteFunc;   /*!< A function to write bytes to the serial port */
    std::function<void (void)> promptDetectCallback;    /*!< A callback function invoked when the bootloader prompt is reached */
    FFirmwareTransferStartFunc firmwareTransferStartFunc;  /*!< A callback function invoked when the bootloader is is waiting for an image transfer */
};

}
