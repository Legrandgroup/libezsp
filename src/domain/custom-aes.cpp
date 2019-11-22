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

#include "custom-aes.h"

#include <string.h>

CAes::CAes() :
    context()
{
}


// algorithm
void CAes::xor_block( void *d, const void *s )
{
    (static_cast<uint32_t *>(d))[ 0] ^= (static_cast<const uint32_t *>(s))[ 0];
    (static_cast<uint32_t *>(d))[ 1] ^= (static_cast<const uint32_t *>(s))[ 1];
    (static_cast<uint32_t *>(d))[ 2] ^= (static_cast<const uint32_t *>(s))[ 2];
    (static_cast<uint32_t *>(d))[ 3] ^= (static_cast<const uint32_t *>(s))[ 3];
}

void CAes::copy_and_key( void *d, const void *s, const void *k )
{
    (static_cast<uint32_t *>(d))[ 0] = (static_cast<const uint32_t *>(s))[ 0] ^ (static_cast<const uint32_t *>(k))[ 0];
    (static_cast<uint32_t *>(d))[ 1] = (static_cast<const uint32_t *>(s))[ 1] ^ (static_cast<const uint32_t *>(k))[ 1];
    (static_cast<uint32_t *>(d))[ 2] = (static_cast<const uint32_t *>(s))[ 2] ^ (static_cast<const uint32_t *>(k))[ 2];
    (static_cast<uint32_t *>(d))[ 3] = (static_cast<const uint32_t *>(s))[ 3] ^ (static_cast<const uint32_t *>(k))[ 3];
}

void CAes::add_round_key( uint8_t d[N_BLOCK], const uint8_t k[N_BLOCK] )
{
    xor_block(d, k);
}

void CAes::shift_sub_rows( uint8_t st[N_BLOCK] )
{
    uint8_t tt;

    st[ 0] = s_box(st[ 0]); st[ 4] = s_box(st[ 4]);
    st[ 8] = s_box(st[ 8]); st[12] = s_box(st[12]);

    tt = st[1]; st[ 1] = s_box(st[ 5]); st[ 5] = s_box(st[ 9]);
    st[ 9] = s_box(st[13]); st[13] = s_box( tt );

    tt = st[2]; st[ 2] = s_box(st[10]); st[10] = s_box( tt );
    tt = st[6]; st[ 6] = s_box(st[14]); st[14] = s_box( tt );

    tt = st[15]; st[15] = s_box(st[11]); st[11] = s_box(st[ 7]);
    st[ 7] = s_box(st[ 3]); st[ 3] = s_box( tt );
}

void CAes::inv_shift_sub_rows( uint8_t st[N_BLOCK] )
{
    uint8_t tt;

    st[ 0] = is_box(st[ 0]); st[ 4] = is_box(st[ 4]);
    st[ 8] = is_box(st[ 8]); st[12] = is_box(st[12]);

    tt = st[13]; st[13] = is_box(st[9]); st[ 9] = is_box(st[5]);
    st[ 5] = is_box(st[1]); st[ 1] = is_box( tt );

    tt = st[2]; st[ 2] = is_box(st[10]); st[10] = is_box( tt );
    tt = st[6]; st[ 6] = is_box(st[14]); st[14] = is_box( tt );

    tt = st[3]; st[ 3] = is_box(st[ 7]); st[ 7] = is_box(st[11]);
    st[11] = is_box(st[15]); st[15] = is_box( tt );
}

void CAes::mix_sub_columns( uint8_t dt[N_BLOCK] )
{
    uint8_t st[N_BLOCK];
    block_copy(st, dt);

    dt[ 0] = gfm2_sb(st[0]) ^ gfm3_sb(st[5]) ^ s_box(st[10]) ^ s_box(st[15]);
    dt[ 1] = s_box(st[0]) ^ gfm2_sb(st[5]) ^ gfm3_sb(st[10]) ^ s_box(st[15]);
    dt[ 2] = s_box(st[0]) ^ s_box(st[5]) ^ gfm2_sb(st[10]) ^ gfm3_sb(st[15]);
    dt[ 3] = gfm3_sb(st[0]) ^ s_box(st[5]) ^ s_box(st[10]) ^ gfm2_sb(st[15]);

    dt[ 4] = gfm2_sb(st[4]) ^ gfm3_sb(st[9]) ^ s_box(st[14]) ^ s_box(st[3]);
    dt[ 5] = s_box(st[4]) ^ gfm2_sb(st[9]) ^ gfm3_sb(st[14]) ^ s_box(st[3]);
    dt[ 6] = s_box(st[4]) ^ s_box(st[9]) ^ gfm2_sb(st[14]) ^ gfm3_sb(st[3]);
    dt[ 7] = gfm3_sb(st[4]) ^ s_box(st[9]) ^ s_box(st[14]) ^ gfm2_sb(st[3]);

    dt[ 8] = gfm2_sb(st[8]) ^ gfm3_sb(st[13]) ^ s_box(st[2]) ^ s_box(st[7]);
    dt[ 9] = s_box(st[8]) ^ gfm2_sb(st[13]) ^ gfm3_sb(st[2]) ^ s_box(st[7]);
    dt[10] = s_box(st[8]) ^ s_box(st[13]) ^ gfm2_sb(st[2]) ^ gfm3_sb(st[7]);
    dt[11] = gfm3_sb(st[8]) ^ s_box(st[13]) ^ s_box(st[2]) ^ gfm2_sb(st[7]);

    dt[12] = gfm2_sb(st[12]) ^ gfm3_sb(st[1]) ^ s_box(st[6]) ^ s_box(st[11]);
    dt[13] = s_box(st[12]) ^ gfm2_sb(st[1]) ^ gfm3_sb(st[6]) ^ s_box(st[11]);
    dt[14] = s_box(st[12]) ^ s_box(st[1]) ^ gfm2_sb(st[6]) ^ gfm3_sb(st[11]);
    dt[15] = gfm3_sb(st[12]) ^ s_box(st[1]) ^ s_box(st[6]) ^ gfm2_sb(st[11]);
  }

void CAes::inv_mix_sub_columns( uint8_t dt[N_BLOCK] )
{
    uint8_t st[N_BLOCK];
    block_copy(st, dt);

    dt[ 0] = is_box(gfm_e(st[ 0]) ^ gfm_b(st[ 1]) ^ gfm_d(st[ 2]) ^ gfm_9(st[ 3]));
    dt[ 5] = is_box(gfm_9(st[ 0]) ^ gfm_e(st[ 1]) ^ gfm_b(st[ 2]) ^ gfm_d(st[ 3]));
    dt[10] = is_box(gfm_d(st[ 0]) ^ gfm_9(st[ 1]) ^ gfm_e(st[ 2]) ^ gfm_b(st[ 3]));
    dt[15] = is_box(gfm_b(st[ 0]) ^ gfm_d(st[ 1]) ^ gfm_9(st[ 2]) ^ gfm_e(st[ 3]));

    dt[ 4] = is_box(gfm_e(st[ 4]) ^ gfm_b(st[ 5]) ^ gfm_d(st[ 6]) ^ gfm_9(st[ 7]));
    dt[ 9] = is_box(gfm_9(st[ 4]) ^ gfm_e(st[ 5]) ^ gfm_b(st[ 6]) ^ gfm_d(st[ 7]));
    dt[14] = is_box(gfm_d(st[ 4]) ^ gfm_9(st[ 5]) ^ gfm_e(st[ 6]) ^ gfm_b(st[ 7]));
    dt[ 3] = is_box(gfm_b(st[ 4]) ^ gfm_d(st[ 5]) ^ gfm_9(st[ 6]) ^ gfm_e(st[ 7]));

    dt[ 8] = is_box(gfm_e(st[ 8]) ^ gfm_b(st[ 9]) ^ gfm_d(st[10]) ^ gfm_9(st[11]));
    dt[13] = is_box(gfm_9(st[ 8]) ^ gfm_e(st[ 9]) ^ gfm_b(st[10]) ^ gfm_d(st[11]));
    dt[ 2] = is_box(gfm_d(st[ 8]) ^ gfm_9(st[ 9]) ^ gfm_e(st[10]) ^ gfm_b(st[11]));
    dt[ 7] = is_box(gfm_b(st[ 8]) ^ gfm_d(st[ 9]) ^ gfm_9(st[10]) ^ gfm_e(st[11]));

    dt[12] = is_box(gfm_e(st[12]) ^ gfm_b(st[13]) ^ gfm_d(st[14]) ^ gfm_9(st[15]));
    dt[ 1] = is_box(gfm_9(st[12]) ^ gfm_e(st[13]) ^ gfm_b(st[14]) ^ gfm_d(st[15]));
    dt[ 6] = is_box(gfm_d(st[12]) ^ gfm_9(st[13]) ^ gfm_e(st[14]) ^ gfm_b(st[15]));
    dt[11] = is_box(gfm_b(st[12]) ^ gfm_d(st[13]) ^ gfm_9(st[14]) ^ gfm_e(st[15]));
  }

// Set the cipher key for the pre-keyed version
void CAes::aes_set_key( const uint8_t key[AES_KEY_SIZE] )
{
    uint8_t cc, rc, hi;
    aes_context *ctx = &context;
    // 128 only
    uint8_t keylen = AES_KEY_SIZE;

    block_copy_nn(ctx->ksch, key, keylen);
    hi = static_cast<uint8_t>(static_cast<uint8_t>(keylen + 28) << 2);
    ctx->rnd = static_cast<uint8_t>((hi >> 4) - 1);
    for( cc = keylen, rc = 1; cc < hi; cc = static_cast<uint8_t>(cc + 4) )
    {   uint8_t tt, t0, t1, t2, t3;

        t0 = ctx->ksch[cc - 4];
        t1 = ctx->ksch[cc - 3];
        t2 = ctx->ksch[cc - 2];
        t3 = ctx->ksch[cc - 1];
        if( cc % keylen == 0 )
        {
            tt = t0;
            t0 = s_box(t1) ^ rc;
            t1 = s_box(t2);
            t2 = s_box(t3);
            t3 = s_box(tt);
            rc = static_cast<uint8_t>(f2(rc));
        }
        else if( keylen > 24 && cc % keylen == 16 )
        {
            t0 = s_box(t0);
            t1 = s_box(t1);
            t2 = s_box(t2);
            t3 = s_box(t3);
        }
        tt = static_cast<uint8_t>(cc - keylen);
        ctx->ksch[cc + 0] = ctx->ksch[tt + 0] ^ t0;
        ctx->ksch[cc + 1] = ctx->ksch[tt + 1] ^ t1;
        ctx->ksch[cc + 2] = ctx->ksch[tt + 2] ^ t2;
        ctx->ksch[cc + 3] = ctx->ksch[tt + 3] ^ t3;
    }
}

// Encrypt a single block of 16 bytes
aes_result CAes::aes_encrypt( const unsigned char in[N_BLOCK], unsigned char out[N_BLOCK] )
{
    if( context.rnd )
    {
        uint8_t s1[N_BLOCK], r;
        copy_and_key( s1, in, context.ksch );

        for( r = 1 ; r < context.rnd ; ++r )
        {
            mix_sub_columns( s1 );
            add_round_key( s1, context.ksch + r * N_BLOCK);
        }
        shift_sub_rows( s1 );
        copy_and_key( out, s1, context.ksch + r * N_BLOCK );
    }
    else
        return false;
    return true;
}

// CBC encrypt a number of blocks (input and return an IV)
/*
aes_result CAes::aes_cbc_encrypt(const unsigned char *in, unsigned char *out, unsigned long size, unsigned char iv[N_BLOCK], const aes_context ctx[1] )
{
    if (size % 16 != 0)
        return EXIT_FAILURE;

    unsigned long n_block = size / 16;

    while(n_block--)
    {
        xor_block(iv, in);
        if(aes_encrypt(iv, iv, ctx) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        memcpy(out, iv, N_BLOCK);
        in += N_BLOCK;
        out += N_BLOCK;
    }
    return EXIT_SUCCESS;
}
*/
// Decrypt a single block of 16 bytes
/*
aes_result CAes::aes_decrypt( const unsigned char in[N_BLOCK], unsigned char out[N_BLOCK], const aes_context ctx[1] )
{
    if( ctx->rnd )
    {
        uint8_t s1[N_BLOCK], r;
        copy_and_key( s1, in, ctx->ksch + ctx->rnd * N_BLOCK );
        inv_shift_sub_rows( s1 );

        for( r = ctx->rnd ; --r ; )
        {
            add_round_key( s1, ctx->ksch + r * N_BLOCK );
            inv_mix_sub_columns( s1 );
        }
        copy_and_key( out, s1, ctx->ksch );
    }
    else
        return -1;
    return 0;
}
*/

// CBC decrypt a number of blocks (input and return an IV)
/*
aes_result CAes::aes_cbc_decrypt( const unsigned char *in, unsigned char *out, unsigned long size, unsigned char iv[N_BLOCK], const aes_context ctx[1] )
{
    if (size % 16 != 0)
        return EXIT_FAILURE;

    unsigned long n_block = size / 16;

    while (n_block--)
    {
        uint8_t tmp[N_BLOCK];

        memcpy(tmp, in, N_BLOCK);
        if(aes_decrypt(in, out, ctx) != EXIT_SUCCESS)
            return EXIT_FAILURE;
        xor_block(out, iv);
        memcpy(iv, tmp, N_BLOCK);
        in += N_BLOCK;
        out += N_BLOCK;
    }
    return EXIT_SUCCESS;
}
*/
