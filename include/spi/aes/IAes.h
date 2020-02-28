/**
 * @file IAes.h
 *
 * @brief Abstract interface to which must conforms implementations of classes that encrypt/decrypt AES blocks
 *
 * Used as a dependency inversion paradigm
 */

#ifndef __IAES_H__
#define __IAES_H__

#include <ezsp/ezsp-protocol/ezsp-enum.h>
#include <ezsp/export.h>

namespace NSSPI {

class LIBEXPORT IAes
{
    public:
        static constexpr uint8_t  AES_KEY_SIZE   = 16;
        static constexpr uint8_t  N_ROW          = 4;
        static constexpr uint8_t  N_COL          = 4;
        static constexpr uint8_t  AES_BLOCK_SIZE = (N_ROW * N_COL);

        IAes() = default;

        virtual void set_key( const uint8_t key[IAes::AES_KEY_SIZE] ) = 0;
        virtual void set_key( const NSEZSP::EmberKeyData& key ) = 0;
        virtual bool encrypt( const unsigned char in[IAes::AES_BLOCK_SIZE], unsigned char out[IAes::AES_BLOCK_SIZE] ) = 0;

        // encryption functions
        // \todo rewrite with class context
        /*
        bool decrypt( const unsigned char in[N_BLOCK], unsigned char out[N_BLOCK], const aes_context ctx[1] );
        bool cbc_encrypt(const unsigned char *in, unsigned char *out, unsigned long size, unsigned char iv[N_BLOCK], const aes_context ctx[1] );
        bool cbc_decrypt(const unsigned char *in, unsigned char *out, unsigned long size, unsigned char iv[N_BLOCK], const aes_context ctx[1] );
        */

        // helper functions
        virtual void xor_block( void *d, const void *s ) = 0;

};

} // namespace NSSPI

#endif
