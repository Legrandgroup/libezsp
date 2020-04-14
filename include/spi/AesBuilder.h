#ifndef __AESBUILDER_H__
#define __AESBUILDER_H__

#include <cstdint>
#include <memory>

#include <ezsp/export.h>
#include <spi/IAes.h>

namespace NSSPI {

class LIBEXPORT AesBuilder {
public:
	static std::unique_ptr<IAes> create();
};

} // namespace NSSPI

#endif
