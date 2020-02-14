/**
 * @file IAsyncDataInputObserver.h
 *
 * @brief Abstract interface to which must conforms implementations of classes that get asynchronous notifications on incoming data
 *
 * Used as a dependency inversion paradigm
 */
#pragma once

#include <cstdlib>	// For size_t

#include <ezsp/export.h>

namespace NSSPI {

/**
 * @brief Observer that gets asynchronous notification of input data from GenericAsyncDataInputObservable objects
 */
class LIBEXPORT IAsyncDataInputObserver {
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
