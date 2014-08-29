#ifndef CRYPTO_H
#define CRYPTO_H

#include "3dstypes.h"
typedef unsigned int size_t;

void add_ctr(void* ctr, u32 carry);

void decrypt(void* key, void* iv, void* inbuf, void* outbuf, size_t size);
void setup_aeskey(uint32_t keyno, int value, void* key);
void use_aeskey(uint32_t keyno);
void set_ctr(int mode, void* iv);
void aes_decrypt(void* inbuf, void* outbuf, void* iv, size_t size);
void _decrypt(uint32_t value, void* inbuf, void* outbuf, size_t blocks);
void aes_fifos(void* inbuf, void* outbuf, size_t blocks);
void set_aeswrfifo(uint32_t value);
uint32_t read_aesrdfifo(void);
uint32_t aes_getwritecount();
uint32_t aes_getreadcount();
uint32_t aescnt_checkwrite();
uint32_t aescnt_checkread();

#endif /* end of include guard: CRYPTO_H */
