#pragma once

#include <set>
#include <vector>
#include "IAsyncDataInputObserver.h"

class IAsyncDataInputObservable {
public:
	IAsyncDataInputObservable();
	virtual ~IAsyncDataInputObservable();

	bool registerObserver(IAsyncDataInputObserver* observer);
	bool unregisterObserver(IAsyncDataInputObserver* observer);

	void notifyObservers(const std::vector<unsigned char>& inputData);
private:
	std::set<IAsyncDataInputObserver*> observers;
};
