#pragma once

#include <set>
#include "IObserver.h"

class IObservable {
public:
	IObservable();
	virtual ~IObservable();

	bool registerObserver(IObserver* observer);
	bool unregisterObserver(IObserver* observer);

	void notifyObservers(const int& dataToPush);
private:
	std::set<IObserver*> observers;
};
