#include <iostream>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <cstdlib> // For rand()
#include <iostream>	// FIXME: Temp for debug

#include "IAsyncDataInputObserver.h"
#include "GenericAsyncDataInputObservable.h"

class FakeUartInputObservable : public GenericAsyncDataInputObservable {
public:
	FakeUartInputObservable() { }
	virtual ~FakeUartInputObservable() { }

	void genRandomData() {
		unsigned char asyncData[20];
		size_t dataLen = static_cast<size_t>(1+static_cast<unsigned int>(rand())%sizeof(asyncData));
		std::cout << "Observable is generating random buffer of " << dataLen << " bytes" << std::endl;
		for (unsigned int bytePos=0; bytePos<dataLen; bytePos++) {
			asyncData[bytePos]=static_cast<unsigned char>(rand() % 256);
		}
		this->notifyObservers(asyncData, dataLen);
	}
};

class TempDisplayer : public IAsyncDataInputObserver {
public:
	TempDisplayer(const std::string& name): name(name) {};
	~TempDisplayer() {};

	virtual void handleInputData(const unsigned char* dataIn, const size_t dataLen) {
		std::stringstream bufDump;

		for (size_t i =0; i<dataLen; i++) {
			bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(dataIn[i]) << " ";
		}
		std::cout << name << ": Received buffer " << bufDump.str() << std::endl;
	};
private :
	std::string name;
};

int main(int argc, char** argv) {
	srand(time(NULL));
	TempDisplayer disp1("Input processor #1");
	TempDisplayer disp2("Input processor #2");

	FakeUartInputObservable inputProcessor;
	inputProcessor.registerObserver(&disp1);
	inputProcessor.registerObserver(&disp2);

	for(int i = 0; i < 5; ++i) {
		inputProcessor.genRandomData();
	}

	inputProcessor.unregisterObserver(&disp1);
	inputProcessor.unregisterObserver(&disp2);

	return 0;
}
