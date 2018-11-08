#include "RaritanUartInputObservable.h"
#include <vector>
#include <cstdlib> // For rand()
#include <iostream>	// FIXME: Temp for debug

RaritanUartInputObservable::RaritanUartInputObservable() {
}

RaritanUartInputObservable::~RaritanUartInputObservable() {
}

void RaritanUartInputObservable::genRandomData() {
	unsigned char asyncData[20];
	size_t dataLen = static_cast<size_t>(1+static_cast<unsigned int>(rand())%sizeof(asyncData));
	std::cout << "Observable is generating random buffer of " << dataLen << " bytes" << std::endl;
	for (unsigned int bytePos=0; bytePos<dataLen; bytePos++) {
		asyncData[bytePos]=static_cast<unsigned char>(rand() % 256);
	}
	this->notifyObservers(asyncData, dataLen);
}
