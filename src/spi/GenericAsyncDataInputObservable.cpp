#include "GenericAsyncDataInputObservable.h"

GenericAsyncDataInputObservable::GenericAsyncDataInputObservable() : observers() {

}

GenericAsyncDataInputObservable::~GenericAsyncDataInputObservable() {
}

bool GenericAsyncDataInputObservable::registerObserver(IAsyncDataInputObserver* observer) {
	return this->observers.emplace(observer).second;
}

bool GenericAsyncDataInputObservable::unregisterObserver(IAsyncDataInputObserver* observer) {
	return static_cast<bool>(this->observers.erase(observer));
}

void GenericAsyncDataInputObservable::notifyObservers(const unsigned char* inputData, const size_t inputDataLen) {
	for(auto observer : this->observers) {
		observer->handleInputData(inputData, inputDataLen);
	}
}
