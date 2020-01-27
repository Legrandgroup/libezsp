/**
 * @file IAes.h
 *
 * @brief Abstract interface to encrypt/decrypt AES block
 *
 */

#ifndef __IAES_H__
#define __IAES_H__

constexpr uint8_t  AES_KEY_SIZE  = 16;
constexpr uint8_t  N_ROW        =          4;
constexpr uint8_t  N_COL        =          4;
constexpr uint8_t  N_BLOCK      =   (N_ROW * N_COL);

namespace NSSPI {

class IAes
{
    public:

        IAes() = default;

        virtual void set_key( const uint8_t key[AES_KEY_SIZE] ) = 0;
        virtual bool encrypt( const unsigned char in[N_BLOCK], unsigned char out[N_BLOCK] ) = 0;

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
