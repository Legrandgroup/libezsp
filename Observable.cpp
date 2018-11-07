#include "IObservable.h"

using namespace std;

IObservable::IObservable() {

}

IObservable::~IObservable() {
}


bool IObservable::registerObserver(IObserver* observer) {
	return this->observers.emplace(observer).second;
}

bool IObservable::unregisterObserver(IObserver* observer) {
	return static_cast<bool>(this->observers.erase(observer));
}

void IObservable::notifyObservers(const int& dataToPush) {
	for(auto observer : this->observers) {
		observer->notify(dataToPush);
	}
}
