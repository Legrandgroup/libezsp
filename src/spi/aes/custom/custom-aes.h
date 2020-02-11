/**
 * @brief Utility functions for aes 256
 */
/*
 ---------------------------------------------------------------------------
 Copyright (c) 2013, Igor Saric. All rights reserved.

 LICENSE TERMS

 The redistribution and use of this software (with or without changes)
 is allowed without the payment of fees or royalties provided that:

  1. source code distributions include the above copyright notice, this
     list of conditions and the following disclaimer;

  2. binary distributions include the above copyright notice, this list
     of conditions and the following disclaimer in their documentation;

  3. the name of the copyright holder is not used to endorse products
     built using this software without specific written permission.

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness
 and/or fitness for purpose.
 ---------------------------------------------------------------------------
 ---------------------------------------------------------------------------
 Copyright (c) 1998-2008, Brian Gladman, Worcester, UK. All rights reserved.

 LICENSE TERMS

 The redistribution and use of this software (with or without changes)
 is allowed without the payment of fees or royalties provided that:

  1. source code distributions include the above copyright notice, this
     list of conditions and the following disclaimer;

  2. binary distributions include the above copyright notice, this list
     of conditions and the following disclaimer in their documentation;

  3. the name of the copyright holder is not used to endorse products
     built using this software without specific written permission.

 DISCLAIMER

 This software is provided 'as is' with no explicit or implied warranties
 in respect of its properties, including, but not limited to, correctness
 and/or fitness for purpose.
 ---------------------------------------------------------------------------
 */

#pragma once

#include <cstdint>

#include "spi/aes/IAes.h"

constexpr uint8_t  N_MAX_ROUNDS =         14;


namespace NSSPI {

typedef struct
{
    uint8_t ksch[(N_MAX_ROUNDS + 1) * IAes::AES_BLOCK_SIZE];
    uint8_t rnd;
}aes_context;

class CAes : public IAes
{
    public:

        CAes();

        void set_key( const uint8_t key[IAes::AES_KEY_SIZE] );
        bool encrypt( const unsigned char in[IAes::AES_BLOCK_SIZE], unsigned char out[IAes::AES_BLOCK_SIZE] );

        //bool decrypt( const unsigned char in[IAes::AES_BLOCK_SIZE], unsigned char out[IAes::AES_BLOCK_SIZE], const aes_context ctx[1] ); // \TODO rewrite with class context
        bool cbc_encrypt(const unsigned char *in, unsigned char *out, unsigned long size, unsigned char iv[IAes::AES_BLOCK_SIZE]);
        //bool cbc_decrypt(const unsigned char *in, unsigned char *out, unsigned long size, unsigned char iv[IAes::AES_BLOCK_SIZE], const aes_context ctx[1] ); // \TODO rewrite with class context

        // helper functions
        void xor_block( void *d, const void *s );


    private:
        // context for this instance
        aes_context context;

        // helper functions
        void copy_and_key( void *d, const void *s, const void *k );
        void add_round_key( uint8_t d[IAes::AES_BLOCK_SIZE], const uint8_t k[IAes::AES_BLOCK_SIZE] );
        void shift_sub_rows( uint8_t st[IAes::AES_BLOCK_SIZE] );
        void inv_shift_sub_rows( uint8_t st[IAes::AES_BLOCK_SIZE] );
        void mix_sub_columns( uint8_t dt[IAes::AES_BLOCK_SIZE] );
        void inv_mix_sub_columns( uint8_t dt[IAes::AES_BLOCK_SIZE] );
};

} // namespace NSSPI
