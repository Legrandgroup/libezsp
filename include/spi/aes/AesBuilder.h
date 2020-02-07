#ifndef __AESBUILDER_H__
#define __AESBUILDER_H__

#include <cstdint>

#include "spi/aes/IAes.h"

namespace NSSPI {

class AesBuilder
{
public:
  static IAes *create();
};

} // namespace NSSPI

#endif
