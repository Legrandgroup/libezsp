#include <pp/Selector.h>
#include <pp/File.h>
#include <pp/Tty.h>
#include <pp/diag.h>
#include <IUartDriver.h>

class RaritanEventLoop {
public:
	RaritanEventLoop() : m_selector(*pp::SelectorSingleton::getInstance()) {}

	/**
	 * @brief Run the main loop
	 */
	void run() {
		this->m_selector.run();
	}
	pp::Selector& getSelector() {
		return this->m_selector;
	}

private:
	pp::Selector& m_selector;
};


class UartDriverRaritan : public IUartDriver {
public:
	UartDriverRaritan(RaritanEventLoop& eventLoop) : m_eventLoop(eventLoop), m_sel_handle(), m_serial_tty() {}
	virtual ~UartDriverRaritan() {
		this->close();
	}

	void open(const std::string& serialPortName, unsigned int baudRate = 57600) {
		pp::Tty::UPtr tmpSerialPortUPTR;
		pp::Tty::open(tmpSerialPortUPTR, serialPortName,
			pp::FileDescriptor::AF_READ_WRITE,
			pp::FileDescriptor::CF_OPEN_EXISTING);

		m_serial_tty = std::move(tmpSerialPortUPTR);
		m_serial_tty->setParams(static_cast<int>(baudRate), pp::Tty::Parity::None, 8, false, false, true);

		auto cbin = [this](pp::Selector::SelectableHandle& sh, short events, short&) {
			if (events & pp::Selector::EVENT_POLLIN) {
				char readData[20];
				size_t rdcnt;
				this->m_serial_tty->read(rdcnt, readData, 20);
				PPD_DEBUG_HEX("read from dongle: ", readData, rdcnt);
				sh.getSelector()->stopAsync();
			}
		};
		unsigned char buf[5] = { 0x1a, 0xc0, 0x38, 0xbc, 0x7e};
		size_t written;
		m_serial_tty->write(written, buf, 5);

		this->m_eventLoop.getSelector().addSelectable(m_sel_handle, m_serial_tty, POLLIN, cbin);
	}

	void close() {
		//this->s->close();
	}

private:
	RaritanEventLoop& m_eventLoop;
	pp::Selector::SelectableHandle m_sel_handle;
	pp::Tty::SPtr m_serial_tty;
};

int main() {
	RaritanEventLoop eventLoop;
	UartDriverRaritan uartDriver(eventLoop);

	uartDriver.open("/dev/ttyUSB0", 57600);
	eventLoop.run();

	return 0;
}
