#include "RaritanUARTInputObservable.h"
#include <vector>
#include <cstdlib> // For rand()

RaritanUARTInputObservable::RaritanUARTInputObservable() {
}

RaritanUARTInputObservable::~RaritanUARTInputObservable() {
}

void RaritanUARTInputObservable::genRandomData() {
	std::vector<unsigned char> asyncData;
	for (unsigned int bytePos=0; bytePos<static_cast<unsigned int>(1+rand()%10); bytePos++) {
		asyncData.push_back(static_cast<unsigned char>(rand() % 256));
	}
	this->notifyObservers(asyncData);
}
