#include <iostream>
#include <ctime>
#include <string>

#include "RaritanUARTInputObservable.h"
#include "IAsyncDataInputObserver.h"

#include <sstream>
#include <iomanip>

using namespace std;

class TempDisplayer : public IAsyncDataInputObserver {
public:
	TempDisplayer(const string& name): name(name) {};
	~TempDisplayer() {};

	virtual void handleInputData(const std::vector<unsigned char>& dataIn) {
		std::stringstream bufDump;
		for (auto i =dataIn.begin(); i != dataIn.end(); ++i) {
			bufDump << std::hex << std::setw(2) << std::setfill('0') << static_cast<unsigned int>(*i) << " ";
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

	RaritanUARTInputObservable probe;
	probe.registerObserver(&disp1);
	probe.registerObserver(&disp2);

	for(int i = 0; i < 5; ++i) {
		probe.genRandomData();
	}

	probe.unregisterObserver(&disp1);
	probe.unregisterObserver(&disp2);

	return 0;
}
