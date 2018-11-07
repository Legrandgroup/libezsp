#include "IAsyncDataInputObservable.h"

using namespace std;

IAsyncDataInputObservable::IAsyncDataInputObservable() {

}

IAsyncDataInputObservable::~IAsyncDataInputObservable() {
}

bool IAsyncDataInputObservable::registerObserver(IAsyncDataInputObserver* observer) {
	return this->observers.emplace(observer).second;
}

bool IAsyncDataInputObservable::unregisterObserver(IAsyncDataInputObserver* observer) {
	return static_cast<bool>(this->observers.erase(observer));
}

void IAsyncDataInputObservable::notifyObservers(const std::vector<unsigned char>& inputData) {
	for(auto observer : this->observers) {
		observer->handleInputData(inputData);
	}
}
