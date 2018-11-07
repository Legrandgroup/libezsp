#include <pp/Selector.h>
#include <pp/File.h>
#include <pp/Tty.h>
#include <pp/diag.h>
#include <iostream>

int main() {
	pp::Tty::UPtr f;
	pp::Tty::SPtr s;
	pp::Selector::SelectableHandle mySelHandle;
	pp::Selector selector;
	pp::Selector::TimedCbHandle touthandle;

	pp::Tty::open(f, "/dev/ttyUSB0",
			pp::FileDescriptor::AF_READ_WRITE,
                        pp::FileDescriptor::CF_OPEN_EXISTING);

	s = std::move(f);
	s->setParams(57600, pp::Tty::Parity::None, 8, false, false, true);

	auto cbin = [s, &touthandle](pp::Selector::SelectableHandle& sh, short events, short&) {
		if (events & pp::Selector::EVENT_POLLIN) {
			char readData[20];
			size_t rdcnt;
			s->read(rdcnt, readData, 20);
			PPD_DEBUG_HEX("read from dongle: ", readData, rdcnt);
			std::cout << "read from dongle done" << std::endl;
			touthandle.removeFromSelector();
			sh.getSelector()->stopAsync();
		}
	};

	auto tcb = [s, &mySelHandle](pp::Selector::TimedCbHandle& th) {
		std::cout << "timeout" << std::endl;
		mySelHandle.removeFromSelector();
		th.getSelector()->stopAsync();
		//mySelHandle.removeFromSelector();
	};

	unsigned char buf[5] = { 0x1a, 0xc0, 0x38, 0xbc, 0x7e};
	size_t written;
	s->write(written, buf, 5);

	selector.addSelectable(mySelHandle, s, POLLIN, cbin);

	selector.addCallback(touthandle, 30/*00*/, pp::Selector::ONCE, tcb);
		
	PPD_DEBUG("Going to run main loop");
	selector.run();
	PPD_DEBUG("main loop stopped");

	return 0;
}
