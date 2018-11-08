#include "GenericAsyncDataInputObservable.h"

GenericAsyncDataInputObservable::GenericAsyncDataInputObservable() {

}

GenericAsyncDataInputObservable::~GenericAsyncDataInputObservable() {
}

bool GenericAsyncDataInputObservable::registerObserver(IAsyncDataInputObserver* observer) {
	return this->observers.emplace(observer).second;
}

bool GenericAsyncDataInputObservable::unregisterObserver(IAsyncDataInputObserver* observer) {
	return static_cast<bool>(this->observers.erase(observer));
}

void GenericAsyncDataInputObservable::notifyObservers(const std::vector<unsigned char>& inputData) {
	for(auto observer : this->observers) {
		observer->handleInputData(inputData);
	}
}
