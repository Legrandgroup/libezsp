/*
 * Observable.hpp
 *
 *  Created on: Jun 19, 2015
 *      Author: alex
 */

#ifndef OBSERVABLE_HPP_
#define OBSERVABLE_HPP_

#include <set>
#include "Observer.hpp"

class Observable {
public:
	Observable();
	virtual ~Observable();

	bool registerObserver(Observer* observer);
	bool unregisterObserver(Observer* observer);

	void notifyObservers(const int& dataToPush);
private:
	std::set<Observer*> observers;
};

#endif /* OBSERVABLE_HPP_ */
