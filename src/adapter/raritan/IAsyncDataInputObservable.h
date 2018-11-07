#pragma once

#include <set>
#include "IAsyncDataInputObserver.h"

class IAsyncDataInputObservable {
public:
	IAsyncDataInputObservable();
	virtual ~IAsyncDataInputObservable();

	bool registerObserver(IAsyncDataInputObserver* observer);
	bool unregisterObserver(IAsyncDataInputObserver* observer);

	void notifyObservers(const int& dataToPush);
private:
	std::set<IAsyncDataInputObserver*> observers;
};
