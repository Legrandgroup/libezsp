/**
 * @file IAsyncDataInputObserver.h
 *
 * @brief Abstract interface to which must conforms implementations of classes that get asynchronous notifications on incoming data
 *
 * Used as a dependency inversion paradigm
 */
#pragma once

#include <vector>
#include <cstdlib>	// For size_t

#ifdef USE_RARITAN
/**** Start of the official API; no includes below this point! ***************/
#include <pp/official_api_start.h>
#endif // USE_RARITAN
namespace NSSPI {

class GenericAsyncDataInputObservable;

/**
 * @brief Observer that gets asynchronous notification of input data from GenericAsyncDataInputObservable objects
 */
class IAsyncDataInputObserver {
public:
	/**
	 * @brief Default constructor
	 */
	IAsyncDataInputObserver() = default;

	/**
	 * @brief Default destructor
	 */
	virtual ~IAsyncDataInputObserver() = default;

	/**
	 * @brief Handler invoked for each input data
	 *
	 * @param dataIn The pointer to the incoming bytes buffer
	 * @param dataLen The size of the data to read inside dataIn
	 */
	virtual void handleInputData(const unsigned char* dataIn, const size_t dataLen) = 0;
};

} // namespace NSSPI
#ifdef USE_RARITAN
#include <pp/official_api_end.h>
#endif // USE_RARITAN
