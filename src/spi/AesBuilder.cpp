#include "spi/AesBuilder.h"

#undef __AES_SPI_FOUND__
#define __AES_SPI_FOUND__
#ifdef USE_AESCUSTOM
#include "spi/custom-aes/custom-aes.h"
typedef NSSPI::CustomAes Aes;
#endif
#ifndef __AES_SPI_FOUND__
# error At least one AES SPI should be selected
#endif
#undef __AES_SPI_FOUND__

using NSSPI::AesBuilder;
using NSSPI::IAes;

IAes *AesBuilder::create()
{
#ifndef DYNAMIC_ALLOCATION
	static Aes instance;

	return &instance;
#else //DYNAMIC_ALLOCATION
	return new Aes();
#endif
}
