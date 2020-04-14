#include "spi/AesBuilder.h"

#undef __AES_SPI_FOUND__ //NOSONAR
#define __AES_SPI_FOUND__
#ifdef USE_AESCUSTOM
#include "spi/custom-aes/custom-aes.h"
typedef NSSPI::CustomAes Aes;
#endif
#ifndef __AES_SPI_FOUND__
# error At least one AES SPI should be selected
#endif
#undef __AES_SPI_FOUND__ //NOSONAR

using NSSPI::AesBuilder;
using NSSPI::IAes;

std::unique_ptr<IAes> AesBuilder::create() {
	/* TODO: When using a C++14 compliant compiler, the line below should be replaced with:
	 * return std::make_unique<IAes>();
	 */
	return std::unique_ptr<IAes>(new Aes());
}
