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
