#ifndef __AESBUILDER_H__
#define __AESBUILDER_H__

#include <cstdint>

#include <ezsp/export.h>
#include <spi/aes/IAes.h>

namespace NSSPI {

class LIBEXPORT AesBuilder
{
public:
  static IAes *create();
};

} // namespace NSSPI

#endif
