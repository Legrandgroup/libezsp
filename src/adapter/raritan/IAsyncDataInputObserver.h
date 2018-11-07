#pragma once

#include <vector>

class IAsyncDataInputObserver {
public:
	IAsyncDataInputObserver() {};
	virtual ~IAsyncDataInputObserver() {};

	// Here we are sending a whole buffer during notification
	virtual void notify(const std::vector<unsigned char>& dataIn) = 0;
};
