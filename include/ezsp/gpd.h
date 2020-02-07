#ifndef __GPD_H__
#define __GPD_H__

namespace NSEZSP {
/**
 * @brief enumetion for possible status of a gpd key from an incomming gpd
 */
enum class CGpdKeyStatus {
    Valid,      /*<! key is known for this gpd and it's the good one */
    Invalid,    /*<! key is known for this gpd but decryption failed, wrong key ? */
    Undefined   /*<! it's not possible to know the status of key for this gpd. */
};

} // namespace NSEZSP

#endif
