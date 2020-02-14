#include "spi/aes/AesBuilder.h"

#ifdef USE_AESCUSTOM
#include "spi/aes/custom/custom-aes.h"
typedef NSSPI::CAes Aes;
#endif

using NSSPI::AesBuilder;
using NSSPI::IAes;

IAes *AesBuilder::create()
{
#ifndef DYNAMIC_ALLOCATION
	static Aes instance;

	return &instance;
#else //DYNAMIC_ALLOCATION
  return new Aes();	/* When dynamic, how do we delete the newly created object: there is a memory leak here */
#endif
}
