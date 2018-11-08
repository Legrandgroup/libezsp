#include <iostream>
#include <ctime>
#include <string>

#include "IAsyncDataInputObserver.h"
#include "raritan/RaritanUartInputObservable.h"

#include <sstream>
#include <iomanip>

using namespace std;

class TempDisplayer : public IAsyncDataInputObserver {
public:
	TempDisplayer(const string& name): name(name) {};
	~TempDisplayer() {};

	virtual void handleInputData(const unsigned char* dataIn, const size_t dataLen) {
		std::stringstream bufDump;

		for (size_t i =0; i<dataLen; i++) {
			bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(dataIn[i]) << " ";
		}
		cout << name << ": Received buffer " << bufDump.str() << endl;
	};
private :
	string name;
};

int main(int argc, char** argv) {
	srand(time(NULL));
	TempDisplayer disp1("Input processor #1");
	TempDisplayer disp2("Input processor #2");

	RaritanUartInputObservable probe;
	probe.registerObserver(&disp1);
	probe.registerObserver(&disp2);

	for(int i = 0; i < 5; ++i) {
		probe.genRandomData();
	}

	probe.unregisterObserver(&disp1);
	probe.unregisterObserver(&disp2);

	return 0;
}
