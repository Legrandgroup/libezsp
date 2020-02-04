/**
 * @file GenericAsyncDataInputObservable.h
 *
 * @brief Allows asynchronous notification of input data
 */

#pragma once

#include <set>
#include <cstdlib>	// For size_t
#include "spi/IAsyncDataInputObserver.h"

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN
namespace NSSPI {

/**
 * @brief An observable class that will invoke method handleInputData() on all its observers when new async data is received
 */
class GenericAsyncDataInputObservable {
public:
	GenericAsyncDataInputObservable();
	virtual ~GenericAsyncDataInputObservable() = default;

	/**
	 * @brief Register a new observer
	 *
	 * @param observer The new observer to add to the notification list
	 * @return true if the observer was successfully added
	 */

	bool registerObserver(IAsyncDataInputObserver* observer);

	/**
	 * @brief Register a new observer
	 *
	 * @param observer The new observer to remove from the notification list
	 * @return true if the observer was successfully removed
	 */
	bool unregisterObserver(IAsyncDataInputObserver* observer);

	/**
	 * @brief Trigger a new notification to all registerd observers
	 *
	 * @param inputData A pointer to the incoming bytes buffer
	 * @param inputDataLen A size of the data to read inside buffer inputData
	 */
	void notifyObservers(const unsigned char* inputData, const size_t inputDataLen);

private:
	std::set<IAsyncDataInputObserver*> observers;	/*!< The list of registered observers */
};

} // namespace NSSPI

#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
