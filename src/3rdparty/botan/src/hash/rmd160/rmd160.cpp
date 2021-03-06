/*
* RIPEMD-160
* (C) 1999-2007 Jack Lloyd
*
* Distributed under the terms of the Botan license
*/

#include <botan/rmd160.h>
#include <botan/loadstor.h>
#include <botan/rotate.h>

namespace Botan {

namespace {

/*
* RIPEMD-160 F1 Function
*/
inline void F1(u32bit& A, u32bit B, u32bit& C, u32bit D, u32bit E,
               u32bit msg, u32bit shift)
   {
   A += (B ^ C ^ D) + msg;
   A  = rotate_left(A, shift) + E;
   C  = rotate_left(C, 10);
   }

/*
* RIPEMD-160 F2 Function
*/
inline void F2(u32bit& A, u32bit B, u32bit& C, u32bit D, u32bit E,
               u32bit msg, u32bit shift, u32bit magic)
   {
   A += (D ^ (B & (C ^ D))) + msg + magic;
   A  = rotate_left(A, shift) + E;
   C  = rotate_left(C, 10);
   }

/*
* RIPEMD-160 F3 Function
*/
inline void F3(u32bit& A, u32bit B, u32bit& C, u32bit D, u32bit E,
               u32bit msg, u32bit shift, u32bit magic)
   {
   A += (D ^ (B | ~C)) + msg + magic;
   A  = rotate_left(A, shift) + E;
   C  = rotate_left(C, 10);
   }

/*
* RIPEMD-160 F4 Function
*/
inline void F4(u32bit& A, u32bit B, u32bit& C, u32bit D, u32bit E,
               u32bit msg, u32bit shift, u32bit magic)
   {
   A += (C ^ (D & (B ^ C))) + msg + magic;
   A  = rotate_left(A, shift) + E;
   C  = rotate_left(C, 10);
   }

/*
* RIPEMD-160 F5 Function
*/
inline void F5(u32bit& A, u32bit B, u32bit& C, u32bit D, u32bit E,
               u32bit msg, u32bit shift, u32bit magic)
   {
   A += (B ^ (C | ~D)) + msg + magic;
   A  = rotate_left(A, shift) + E;
   C  = rotate_left(C, 10);
   }

}

/*
* RIPEMD-160 Compression Function
*/
void RIPEMD_160::compress_n(const byte input[], size_t blocks)
   {
   const u32bit MAGIC2 = 0x5A827999, MAGIC3 = 0x6ED9EBA1,
                MAGIC4 = 0x8F1BBCDC, MAGIC5 = 0xA953FD4E,
                MAGIC6 = 0x50A28BE6, MAGIC7 = 0x5C4DD124,
                MAGIC8 = 0x6D703EF3, MAGIC9 = 0x7A6D76E9;

   for(size_t i = 0; i != blocks; ++i)
      {
      load_le(&M[0], input, M.size());

      u32bit A1 = digest[0], A2 = A1, B1 = digest[1], B2 = B1,
             C1 = digest[2], C2 = C1, D1 = digest[3], D2 = D1,
             E1 = digest[4], E2 = E1;

      F1(A1,B1,C1,D1,E1,M[ 0],11       );  F5(A2,B2,C2,D2,E2,M[ 5], 8,MAGIC6);
      F1(E1,A1,B1,C1,D1,M[ 1],14       );  F5(E2,A2,B2,C2,D2,M[14], 9,MAGIC6);
      F1(D1,E1,A1,B1,C1,M[ 2],15       );  F5(D2,E2,A2,B2,C2,M[ 7], 9,MAGIC6);
      F1(C1,D1,E1,A1,B1,M[ 3],12       );  F5(C2,D2,E2,A2,B2,M[ 0],11,MAGIC6);
      F1(B1,C1,D1,E1,A1,M[ 4], 5       );  F5(B2,C2,D2,E2,A2,M[ 9],13,MAGIC6);
      F1(A1,B1,C1,D1,E1,M[ 5], 8       );  F5(A2,B2,C2,D2,E2,M[ 2],15,MAGIC6);
      F1(E1,A1,B1,C1,D1,M[ 6], 7       );  F5(E2,A2,B2,C2,D2,M[11],15,MAGIC6);
      F1(D1,E1,A1,B1,C1,M[ 7], 9       );  F5(D2,E2,A2,B2,C2,M[ 4], 5,MAGIC6);
      F1(C1,D1,E1,A1,B1,M[ 8],11       );  F5(C2,D2,E2,A2,B2,M[13], 7,MAGIC6);
      F1(B1,C1,D1,E1,A1,M[ 9],13       );  F5(B2,C2,D2,E2,A2,M[ 6], 7,MAGIC6);
      F1(A1,B1,C1,D1,E1,M[10],14       );  F5(A2,B2,C2,D2,E2,M[15], 8,MAGIC6);
      F1(E1,A1,B1,C1,D1,M[11],15       );  F5(E2,A2,B2,C2,D2,M[ 8],11,MAGIC6);
      F1(D1,E1,A1,B1,C1,M[12], 6       );  F5(D2,E2,A2,B2,C2,M[ 1],14,MAGIC6);
      F1(C1,D1,E1,A1,B1,M[13], 7       );  F5(C2,D2,E2,A2,B2,M[10],14,MAGIC6);
      F1(B1,C1,D1,E1,A1,M[14], 9       );  F5(B2,C2,D2,E2,A2,M[ 3],12,MAGIC6);
      F1(A1,B1,C1,D1,E1,M[15], 8       );  F5(A2,B2,C2,D2,E2,M[12], 6,MAGIC6);

      F2(E1,A1,B1,C1,D1,M[ 7], 7,MAGIC2);  F4(E2,A2,B2,C2,D2,M[ 6], 9,MAGIC7);
      F2(D1,E1,A1,B1,C1,M[ 4], 6,MAGIC2);  F4(D2,E2,A2,B2,C2,M[11],13,MAGIC7);
      F2(C1,D1,E1,A1,B1,M[13], 8,MAGIC2);  F4(C2,D2,E2,A2,B2,M[ 3],15,MAGIC7);
      F2(B1,C1,D1,E1,A1,M[ 1],13,MAGIC2);  F4(B2,C2,D2,E2,A2,M[ 7], 7,MAGIC7);
      F2(A1,B1,C1,D1,E1,M[10],11,MAGIC2);  F4(A2,B2,C2,D2,E2,M[ 0],12,MAGIC7);
      F2(E1,A1,B1,C1,D1,M[ 6], 9,MAGIC2);  F4(E2,A2,B2,C2,D2,M[13], 8,MAGIC7);
      F2(D1,E1,A1,B1,C1,M[15], 7,MAGIC2);  F4(D2,E2,A2,B2,C2,M[ 5], 9,MAGIC7);
      F2(C1,D1,E1,A1,B1,M[ 3],15,MAGIC2);  F4(C2,D2,E2,A2,B2,M[10],11,MAGIC7);
      F2(B1,C1,D1,E1,A1,M[12], 7,MAGIC2);  F4(B2,C2,D2,E2,A2,M[14], 7,MAGIC7);
      F2(A1,B1,C1,D1,E1,M[ 0],12,MAGIC2);  F4(A2,B2,C2,D2,E2,M[15], 7,MAGIC7);
      F2(E1,A1,B1,C1,D1,M[ 9],15,MAGIC2);  F4(E2,A2,B2,C2,D2,M[ 8],12,MAGIC7);
      F2(D1,E1,A1,B1,C1,M[ 5], 9,MAGIC2);  F4(D2,E2,A2,B2,C2,M[12], 7,MAGIC7);
      F2(C1,D1,E1,A1,B1,M[ 2],11,MAGIC2);  F4(C2,D2,E2,A2,B2,M[ 4], 6,MAGIC7);
      F2(B1,C1,D1,E1,A1,M[14], 7,MAGIC2);  F4(B2,C2,D2,E2,A2,M[ 9],15,MAGIC7);
      F2(A1,B1,C1,D1,E1,M[11],13,MAGIC2);  F4(A2,B2,C2,D2,E2,M[ 1],13,MAGIC7);
      F2(E1,A1,B1,C1,D1,M[ 8],12,MAGIC2);  F4(E2,A2,B2,C2,D2,M[ 2],11,MAGIC7);

      F3(D1,E1,A1,B1,C1,M[ 3],11,MAGIC3);  F3(D2,E2,A2,B2,C2,M[15], 9,MAGIC8);
      F3(C1,D1,E1,A1,B1,M[10],13,MAGIC3);  F3(C2,D2,E2,A2,B2,M[ 5], 7,MAGIC8);
      F3(B1,C1,D1,E1,A1,M[14], 6,MAGIC3);  F3(B2,C2,D2,E2,A2,M[ 1],15,MAGIC8);
      F3(A1,B1,C1,D1,E1,M[ 4], 7,MAGIC3);  F3(A2,B2,C2,D2,E2,M[ 3],11,MAGIC8);
      F3(E1,A1,B1,C1,D1,M[ 9],14,MAGIC3);  F3(E2,A2,B2,C2,D2,M[ 7], 8,MAGIC8);
      F3(D1,E1,A1,B1,C1,M[15], 9,MAGIC3);  F3(D2,E2,A2,B2,C2,M[14], 6,MAGIC8);
      F3(C1,D1,E1,A1,B1,M[ 8],13,MAGIC3);  F3(C2,D2,E2,A2,B2,M[ 6], 6,MAGIC8);
      F3(B1,C1,D1,E1,A1,M[ 1],15,MAGIC3);  F3(B2,C2,D2,E2,A2,M[ 9],14,MAGIC8);
      F3(A1,B1,C1,D1,E1,M[ 2],14,MAGIC3);  F3(A2,B2,C2,D2,E2,M[11],12,MAGIC8);
      F3(E1,A1,B1,C1,D1,M[ 7], 8,MAGIC3);  F3(E2,A2,B2,C2,D2,M[ 8],13,MAGIC8);
      F3(D1,E1,A1,B1,C1,M[ 0],13,MAGIC3);  F3(D2,E2,A2,B2,C2,M[12], 5,MAGIC8);
      F3(C1,D1,E1,A1,B1,M[ 6], 6,MAGIC3);  F3(C2,D2,E2,A2,B2,M[ 2],14,MAGIC8);
      F3(B1,C1,D1,E1,A1,M[13], 5,MAGIC3);  F3(B2,C2,D2,E2,A2,M[10],13,MAGIC8);
      F3(A1,B1,C1,D1,E1,M[11],12,MAGIC3);  F3(A2,B2,C2,D2,E2,M[ 0],13,MAGIC8);
      F3(E1,A1,B1,C1,D1,M[ 5], 7,MAGIC3);  F3(E2,A2,B2,C2,D2,M[ 4], 7,MAGIC8);
      F3(D1,E1,A1,B1,C1,M[12], 5,MAGIC3);  F3(D2,E2,A2,B2,C2,M[13], 5,MAGIC8);

      F4(C1,D1,E1,A1,B1,M[ 1],11,MAGIC4);  F2(C2,D2,E2,A2,B2,M[ 8],15,MAGIC9);
      F4(B1,C1,D1,E1,A1,M[ 9],12,MAGIC4);  F2(B2,C2,D2,E2,A2,M[ 6], 5,MAGIC9);
      F4(A1,B1,C1,D1,E1,M[11],14,MAGIC4);  F2(A2,B2,C2,D2,E2,M[ 4], 8,MAGIC9);
      F4(E1,A1,B1,C1,D1,M[10],15,MAGIC4);  F2(E2,A2,B2,C2,D2,M[ 1],11,MAGIC9);
      F4(D1,E1,A1,B1,C1,M[ 0],14,MAGIC4);  F2(D2,E2,A2,B2,C2,M[ 3],14,MAGIC9);
      F4(C1,D1,E1,A1,B1,M[ 8],15,MAGIC4);  F2(C2,D2,E2,A2,B2,M[11],14,MAGIC9);
      F4(B1,C1,D1,E1,A1,M[12], 9,MAGIC4);  F2(B2,C2,D2,E2,A2,M[15], 6,MAGIC9);
      F4(A1,B1,C1,D1,E1,M[ 4], 8,MAGIC4);  F2(A2,B2,C2,D2,E2,M[ 0],14,MAGIC9);
      F4(E1,A1,B1,C1,D1,M[13], 9,MAGIC4);  F2(E2,A2,B2,C2,D2,M[ 5], 6,MAGIC9);
      F4(D1,E1,A1,B1,C1,M[ 3],14,MAGIC4);  F2(D2,E2,A2,B2,C2,M[12], 9,MAGIC9);
      F4(C1,D1,E1,A1,B1,M[ 7], 5,MAGIC4);  F2(C2,D2,E2,A2,B2,M[ 2],12,MAGIC9);
      F4(B1,C1,D1,E1,A1,M[15], 6,MAGIC4);  F2(B2,C2,D2,E2,A2,M[13], 9,MAGIC9);
      F4(A1,B1,C1,D1,E1,M[14], 8,MAGIC4);  F2(A2,B2,C2,D2,E2,M[ 9],12,MAGIC9);
      F4(E1,A1,B1,C1,D1,M[ 5], 6,MAGIC4);  F2(E2,A2,B2,C2,D2,M[ 7], 5,MAGIC9);
      F4(D1,E1,A1,B1,C1,M[ 6], 5,MAGIC4);  F2(D2,E2,A2,B2,C2,M[10],15,MAGIC9);
      F4(C1,D1,E1,A1,B1,M[ 2],12,MAGIC4);  F2(C2,D2,E2,A2,B2,M[14], 8,MAGIC9);

      F5(B1,C1,D1,E1,A1,M[ 4], 9,MAGIC5);  F1(B2,C2,D2,E2,A2,M[12], 8       );
      F5(A1,B1,C1,D1,E1,M[ 0],15,MAGIC5);  F1(A2,B2,C2,D2,E2,M[15], 5       );
      F5(E1,A1,B1,C1,D1,M[ 5], 5,MAGIC5);  F1(E2,A2,B2,C2,D2,M[10],12       );
      F5(D1,E1,A1,B1,C1,M[ 9],11,MAGIC5);  F1(D2,E2,A2,B2,C2,M[ 4], 9       );
      F5(C1,D1,E1,A1,B1,M[ 7], 6,MAGIC5);  F1(C2,D2,E2,A2,B2,M[ 1],12       );
      F5(B1,C1,D1,E1,A1,M[12], 8,MAGIC5);  F1(B2,C2,D2,E2,A2,M[ 5], 5       );
      F5(A1,B1,C1,D1,E1,M[ 2],13,MAGIC5);  F1(A2,B2,C2,D2,E2,M[ 8],14       );
      F5(E1,A1,B1,C1,D1,M[10],12,MAGIC5);  F1(E2,A2,B2,C2,D2,M[ 7], 6       );
      F5(D1,E1,A1,B1,C1,M[14], 5,MAGIC5);  F1(D2,E2,A2,B2,C2,M[ 6], 8       );
      F5(C1,D1,E1,A1,B1,M[ 1],12,MAGIC5);  F1(C2,D2,E2,A2,B2,M[ 2],13       );
      F5(B1,C1,D1,E1,A1,M[ 3],13,MAGIC5);  F1(B2,C2,D2,E2,A2,M[13], 6       );
      F5(A1,B1,C1,D1,E1,M[ 8],14,MAGIC5);  F1(A2,B2,C2,D2,E2,M[14], 5       );
      F5(E1,A1,B1,C1,D1,M[11],11,MAGIC5);  F1(E2,A2,B2,C2,D2,M[ 0],15       );
      F5(D1,E1,A1,B1,C1,M[ 6], 8,MAGIC5);  F1(D2,E2,A2,B2,C2,M[ 3],13       );
      F5(C1,D1,E1,A1,B1,M[15], 5,MAGIC5);  F1(C2,D2,E2,A2,B2,M[ 9],11       );
      F5(B1,C1,D1,E1,A1,M[13], 6,MAGIC5);  F1(B2,C2,D2,E2,A2,M[11],11       );

      C1        = digest[1] + C1 + D2;
      digest[1] = digest[2] + D1 + E2;
      digest[2] = digest[3] + E1 + A2;
      digest[3] = digest[4] + A1 + B2;
      digest[4] = digest[0] + B1 + C2;
      digest[0] = C1;

      input += hash_block_size();
      }
   }

/*
* Copy out the digest
*/
void RIPEMD_160::copy_out(byte output[])
   {
   for(size_t i = 0; i != output_length(); i += 4)
      store_le(digest[i/4], output + i);
   }

/*
* Clear memory of sensitive data
*/
void RIPEMD_160::clear()
   {
   MDx_HashFunction::clear();
   zeroise(M);
   digest[0] = 0x67452301;
   digest[1] = 0xEFCDAB89;
   digest[2] = 0x98BADCFE;
   digest[3] = 0x10325476;
   digest[4] = 0xC3D2E1F0;
   }

}
