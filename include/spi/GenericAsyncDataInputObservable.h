/**
 * @file GenericAsyncDataInputObservable.h
 *
 * @brief Allows asynchronous notification of input data
 */

#pragma once

#include <set>
#include <cstdlib>	// For size_t
#include <ezsp/export.h>
#include <spi/IAsyncDataInputObserver.h>

namespace NSSPI {

/**
 * @brief An observable class that will invoke method handleInputData() on all its observers when new async data is received
 * 
 * This class and implementation atogether with the NSSPI::IAsyncDataInputObserver interface defined in IAsyncDataInputObserver.h,
 * define an observable/observer pattern.
 * This is a pushing observer implementation, meaning that the data that is observed will be directly passed as argument of the observer callback.
 * This avoids any caching in the observable that would be required if the observer was only notified about data, and then pulling data.
 * However, this also means that (read) data buffers are copied as many times as there are observers.
 * 
 * To used this observable/observer implementation, you should define two types of objects:
 * * One observable (the object that is informed about new asynchronously available data), that will either be derived from NSSPI::GenericAsyncDataInputObservable defined here or that will encapsulate a NSSPI::GenericAsyncDataInputObservable member.
 * * One or more observers that will derive from NSSPI::IAsyncDataInputObserver and will implement the required interface method handleInputData() (see IAsyncDataInputObserver.h).
 *   That method handleInputData() will be invoked for every observers, when a new data is made avaible, passing the received bytes as argument.
 * 
 * The observable class should invoke its notifyObservers() method when new received bytes are to be pushed to its observers.
 * All observers will then receive these bytes in their handleInputData(), but only if they have registered to the observable beforehand using the observable's registerObserver() method with themselves as the observer reference.
 * 
 * Here is an example:
 * 
 * An object that can be observed is an instance of GenericAsyncDataInputObservable and that sends one 0x00 byte every second using a thread can be defined using:
 * @code
 * class SampleObserved : public NSSPI::GenericAsyncDataInputObservable {
 *   public:
 *     // Methods
 *     SampleObserved() :
 *       terminate(false),
 *       simulateRxThread([this]() {
 *         while(!this->terminate) {
 *           std::this_thread::sleep_for(std::chrono::milliseconds(500));
 *           this->notifyObservers(&(this->byte0), 1);
 *         });
 *     {
 *     }
 *     ~SampleObserved() {
 *       this->terminate = true;
 *       this->simulateRxThread.join();
 *     }
 * 
 *     // Attributes
 *     static const unsigned char byte0 = 0x00;
 *     bool terminate;
 *     std::thread simulateRxThread;
 * };
 * @endcode
 * 
 * An observer that writes to the console all bytes recevied from an observable:
 * @code
 * class SampleObserver : public NSSPI::IAsyncDataInputObserver {
 *   public:
 *     // Methods
 *     SampleObserver() = default;
 *     ~SampleObserved() = default;
 *     handleInputData(const unsigned char* dataIn, const size_t dataLen) {
 *       std::cout << Logger::byteSequenceToString(static_cast<const uint8_t*>(dataIn), dataLen);
 *     }
 * };
 * @endcode
 * 
 * And finally the main code instanciating and binding observable and observers together:
 * @code
 * SampleObserver observer;
 * SampleObserved observed;
 * observed.registerObserver(&observer);
 * std::this_thread::sleep_for(std::chrono::seconds(5));
 * @endcode
 */
class LIBEXPORT GenericAsyncDataInputObservable {
public:
	GenericAsyncDataInputObservable();
	virtual ~GenericAsyncDataInputObservable() = default;

	/**
	 * @brief Register a new observer
	 *
	 * @param observer The new observer to add to the notification list
	 * @return true if the observer was successfully added
	 */

	bool registerObserver(NSSPI::IAsyncDataInputObserver* observer);

	/**
	 * @brief Register a new observer
	 *
	 * @param observer The new observer to remove from the notification list
	 * @return true if the observer was successfully removed
	 */
	bool unregisterObserver(NSSPI::IAsyncDataInputObserver* observer);

	/**
	 * @brief Trigger a new notification to all registerd observers
	 *
	 * @param inputData A pointer to the incoming bytes buffer
	 * @param inputDataLen A size of the data to read inside buffer inputData
	 */
	void notifyObservers(const unsigned char* inputData, const size_t inputDataLen);

private:
	std::set<NSSPI::IAsyncDataInputObserver*> observers;	/*!< The list of registered observers */
};

} // namespace NSEZSP
