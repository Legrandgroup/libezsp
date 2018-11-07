#include <pp/Selector.h>
#include <pp/File.h>
#include <pp/Tty.h>
#include <pp/diag.h>

int main() {
	pp::Tty::UPtr f;
	pp::Tty::SPtr s;
	pp::Selector::SelectableHandle mySelHandle;
	pp::Selector selector;
	pp::Tty::open(f, "/dev/ttyUSB0",
			pp::FileDescriptor::AF_READ_WRITE,
                        pp::FileDescriptor::CF_OPEN_EXISTING);

	s = std::move(f);
	f->setParams(57600, pp::Tty::Parity::None, 8, false, false, true);

	auto cbin = [s](pp::Selector::SelectableHandle& sh, short events, short&) {
		if (events & pp::Selector::EVENT_POLLIN) {
			char readData[20];
			size_t rdcnt;
			s->read(rdcnt, readData, 20);
			PPD_DEBUG_HEX("read from dongle: ", readData, rdcnt);
			sh.getSelector()->stopAsync();
		}
	};
	unsigned char buf[5] = { 0x1a, 0xc0, 0x38, 0xbc, 0x7e};
	size_t written;
	s->write(written, buf, 5);

	selector.addSelectable(mySelHandle, s, POLLIN, cbin);
	
	selector.run();

	return 0;
}
