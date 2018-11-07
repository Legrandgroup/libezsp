#include <pp/Selector.h>
#include <pp/File.h>
#include <pp/Tty.h>
#include <pp/diag.h>
#include <Iuart-driver.h>

class UartDriverRaritan : public IUartDriver {
public:
	UartDriverRaritan() : selector(*pp::SelectorSingleton::getInstance()), s() {}
	virtual ~UartDriverRaritan() {
		this->close();
	}

	void run() {
		this->selector.run();
	}
	void open(const std::string& serialPort, unsigned int baudRate = 57600) {
		pp::Tty::UPtr f;
		pp::Tty::open(f, serialPort,
			pp::FileDescriptor::AF_READ_WRITE,
			pp::FileDescriptor::CF_OPEN_EXISTING);

		this->s = std::move(f);
		this->s->setParams(static_cast<int>(baudRate), pp::Tty::Parity::None, 8, false, false, true);

		auto cbin = [this](pp::Selector::SelectableHandle& sh, short events, short&) {
			if (events & pp::Selector::EVENT_POLLIN) {
				char readData[20];
				size_t rdcnt;
				this->s->read(rdcnt, readData, 20);
				PPD_DEBUG_HEX("read from dongle: ", readData, rdcnt);
				sh.getSelector()->stopAsync();
			}
		};
		unsigned char buf[5] = { 0x1a, 0xc0, 0x38, 0xbc, 0x7e};
		size_t written;
		this->s->write(written, buf, 5);

		this->selector.addSelectable(mySelHandle, this->s, POLLIN, cbin);
	}

	void close() {
		//000this->s->close();
	}

public:
	pp::Selector& selector;
private:
	pp::Selector::SelectableHandle mySelHandle;
	pp::Tty::SPtr s;
};

int main() {
	
	UartDriverRaritan uartDriver;

	uartDriver.open("/dev/ttyUSB0", 57600);
	uartDriver.run();

	return 0;
}
