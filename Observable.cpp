/*
 * Observable.cpp
 *
 *  Created on: Jun 19, 2015
 *      Author: alex
 */

#include "Observable.hpp"

using namespace std;

Observable::Observable() {

}

Observable::~Observable() {
}


bool Observable::registerObserver(Observer* observer) {
	return this->observers.emplace(observer).second;
}

bool Observable::unregisterObserver(Observer* observer) {
	return static_cast<bool>(this->observers.erase(observer));
}

void Observable::notifyObservers(const int& dataToPush) {
	for(auto observer : this->observers) {
		observer->notify(dataToPush);
	}
}
