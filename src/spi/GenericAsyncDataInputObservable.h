#pragma once

#include <set>
#include <vector>
#include "IAsyncDataInputObserver.h"

class GenericAsyncDataInputObservable {
public:
	GenericAsyncDataInputObservable();
	virtual ~GenericAsyncDataInputObservable();

	bool registerObserver(IAsyncDataInputObserver* observer);
	bool unregisterObserver(IAsyncDataInputObserver* observer);

	void notifyObservers(const unsigned char* inputData, const size_t inputDataLen);
private:
	std::set<IAsyncDataInputObserver*> observers;
};
