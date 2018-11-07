#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>

#include "IAsyncDataInputObservable.h"
#include "IAsyncDataInputObserver.h"

using namespace std;

class TempProbe : public IAsyncDataInputObservable {
public:
	TempProbe() {};

	void measureTemp() {
		int temp = (rand() % (25-15)) + 15;
		this->notifyObservers(temp);
	}
};

class TempDisplayer : public IAsyncDataInputObserver {
public:
	TempDisplayer(const string& name): name(name) {};
	~TempDisplayer() {};

	void notify(const int& content) {
		cout << name << ": Received temperature " << content << endl;
	};
private :
	string name;
};

int main(int argc, char** argv) {
	srand(time(NULL));
	TempDisplayer disp1("Displayer 1");
	TempDisplayer disp2("Displayer 2");

	TempProbe probe;
	probe.registerObserver(&disp1);
	probe.registerObserver(&disp2);

	for(int i = 0; i < 20; ++i) {
		probe.measureTemp();
	}

	probe.unregisterObserver(&disp1);
	probe.unregisterObserver(&disp2);

	return 0;
}
