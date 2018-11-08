#pragma once

#include <vector>
#include <cstdlib>	// For size_t

class IAsyncDataInputObserver {
public:
	IAsyncDataInputObserver() {};
	virtual ~IAsyncDataInputObserver() {};

	// Here we are sending a whole buffer during notification
	virtual void handleInputData(const unsigned char* dataIn, const size_t dataLen) = 0;
};
