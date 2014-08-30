/* original version by megazig */
#include "crypto.h"
#include "minlib.h"

#define REG_AESCNT     ((volatile uint32_t*)0x10009000)
#define REG_AESBLKCNT  ((volatile uint32_t*)0x10009004)
#define REG_AESWRFIFO  ((volatile uint32_t*)0x10009008)
#define REG_AESRDFIFO  ((volatile uint32_t*)0x1000900C)
#define REG_AESKEYSEL  ((volatile uint8_t *)0x10009010)
#define REG_AESKEYCNT  ((volatile uint8_t *)0x10009011)
#define REG_AESCTR     ((volatile uint32_t*)0x10009020)
#define REG_AESKEYFIFO ((volatile uint32_t*)0x10009108)

#define AES_BIG_INPUT      1
#define AES_LITTLE_INPUT   0
#define AES_NORMAL_INPUT   4
#define AES_REVERSED_INPUT 0
#define AES_BLOCK_SIZE 0x10

#define AES_CCM_DECRYPT_MODE (0 << 27)
#define AES_CCM_ENCRYPT_MODE (1 << 27)
#define AES_CTR_MODE         (2 << 27)
#define AES_CTR_MODE         (2 << 27)
#define AES_CBC_DECRYPT_MODE (4 << 27)
#define AES_CBC_ENCRYPT_MODE (5 << 27)

#define AES_CNT_START         0x80000000
#define AES_CNT_INPUT_ORDER   0x02000000
#define AES_CNT_OUTPUT_ORDER  0x01000000
#define AES_CNT_INPUT_ENDIAN  0x00800000
#define AES_CNT_OUTPUT_ENDIAN 0x00400000
#define AES_CNT_FLUSH_READ    0x00000800
#define AES_CNT_FLUSH_WRITE   0x00000400

void setup_aeskey(uint32_t keyno, int value, void* key)
{
	volatile uint32_t* aes_regs[] =
	{
		(volatile uint32_t*)0x19009060,
		(volatile uint32_t*)0x10009090,
		(volatile uint32_t*)0x100090C0,
		(volatile uint32_t*)0x100090F0
	};
	uint32_t * _key = (uint32_t*)key;
	*REG_AESCNT = (*REG_AESCNT & ~(AES_CNT_INPUT_ENDIAN|AES_CNT_INPUT_ORDER)) | (value << 23);
	if (keyno > 3)
	{
		if (keyno > 0x3F)
			return;
		*REG_AESKEYCNT = (*REG_AESKEYCNT >> 6 << 6) | (u8)keyno | 0x80;
		*REG_AESKEYFIFO = _key[0];
		*REG_AESKEYFIFO = _key[1];
		*REG_AESKEYFIFO = _key[2];
		*REG_AESKEYFIFO = _key[3];
	}
	else
	{
		volatile uint32_t* aes_reg = aes_regs[keyno];
		if (value & 0x4)
		{
			aes_reg[0] = _key[3];
			aes_reg[1] = _key[2];
			aes_reg[2] = _key[1];
			aes_reg[3] = _key[0];
		}
		else
		{
			aes_reg[0] = _key[0];
			aes_reg[1] = _key[1];
			aes_reg[2] = _key[2];
			aes_reg[3] = _key[3];
		}
	}
}

void use_aeskey(uint32_t keyno)
{
	if (keyno > 0x3F)
		return;
	*REG_AESKEYSEL = keyno;
	*REG_AESCNT    = *REG_AESCNT | 0x04000000; /* mystery bit */
}

void set_ctr(int mode, void* iv)
{
	uint32_t * _iv = (uint32_t*)iv;
	*REG_AESCNT = (*REG_AESCNT & ~(AES_CNT_INPUT_ENDIAN|AES_CNT_INPUT_ORDER)) | (mode << 23);
	if (mode & AES_NORMAL_INPUT)
	{
		*(REG_AESCTR + 0) = _iv[3];
		*(REG_AESCTR + 1) = _iv[2];
		*(REG_AESCTR + 2) = _iv[1];
		*(REG_AESCTR + 3) = _iv[0];
	}
	else
	{
		*(REG_AESCTR + 0) = _iv[0];
		*(REG_AESCTR + 1) = _iv[1];
		*(REG_AESCTR + 2) = _iv[2];
		*(REG_AESCTR + 3) = _iv[3];
	}
}

void add_ctr(void* ctr, u32 carry)
{
	u32 counter[4];
	u8 *outctr = (u8 *) ctr;
	u32 sum;
	int i;

	for(i=0; i<4; i++){
		counter[i] = (outctr[i*4+0]<<24) | (outctr[i*4+1]<<16) | (outctr[i*4+2]<<8) | (outctr[i*4+3]<<0);}

	for(i=3; i>=0; i--)
	{
		sum = counter[i] + carry;
		if (sum < counter[i]){
			carry = 1;}
		else{
			carry = 0;}
		counter[i] = sum;
	}

	for(i=0; i<4; i++)
	{
		outctr[i*4+0] = counter[i]>>24;
		outctr[i*4+1] = counter[i]>>16;
		outctr[i*4+2] = counter[i]>>8;
		outctr[i*4+3] = counter[i]>>0;
	}
}

//IV = CTR everywhere here
void aes_decrypt(void* inbuf, void* outbuf, void* iv, size_t size)
{
	uint32_t in  = (uint32_t)inbuf;
	uint32_t out = (uint32_t)outbuf;
	size_t block_count = size;
	size_t blocks;
	while (block_count != 0)
	{
		blocks = (block_count >= 0xFFFF) ? 0xFFFF : block_count;
		_decrypt(AES_CTR_MODE, (void*)in, (void*)out, blocks);
		in  += blocks * AES_BLOCK_SIZE;
		out += blocks * AES_BLOCK_SIZE;
		block_count -= blocks;
	}
}

void _decrypt(uint32_t value, void* inbuf, void* outbuf, size_t blocks)
{
	*REG_AESCNT = 0;
	*REG_AESBLKCNT = blocks << 16;
	*REG_AESCNT = value |
		AES_CNT_START |
		AES_CNT_INPUT_ORDER |
		AES_CNT_OUTPUT_ORDER |
		AES_CNT_INPUT_ENDIAN |
		AES_CNT_OUTPUT_ENDIAN |
		AES_CNT_FLUSH_READ |
		AES_CNT_FLUSH_WRITE;
	aes_fifos(inbuf, outbuf, blocks);
}

void aes_fifos(void* inbuf, void* outbuf, size_t blocks)
{
	uint32_t in  = (uint32_t)inbuf;
	uint32_t out = (uint32_t)outbuf;
	size_t curblock = 0;
	while (curblock != blocks)
	{
		if (in)
		{
			while (aescnt_checkwrite()) ;
			int ii = 0;
			for (ii = in; ii != in + AES_BLOCK_SIZE; ii += 4)
			{
				set_aeswrfifo( *(uint32_t*)(in) );
			}
			if (out)
			{
				while (aescnt_checkread()) ;
				for (ii = out; ii != out + AES_BLOCK_SIZE; ii += 4)
				{
					*(uint32_t*)ii = read_aesrdfifo();
				}
			}
		}
		curblock++;
	}
}

void set_aeswrfifo(uint32_t value)
{
	*REG_AESWRFIFO = value;
}

uint32_t read_aesrdfifo(void)
{
	return *REG_AESRDFIFO;
}

uint32_t aes_getwritecount()
{
	return *REG_AESCNT & 0x1F;
}

uint32_t aes_getreadcount()
{
	return (*REG_AESCNT >> 5) & 0x1F;
}

uint32_t aescnt_checkwrite()
{
	size_t ret = aes_getwritecount();
	return (ret > 0xF);
}

uint32_t aescnt_checkread()
{
	size_t ret = aes_getreadcount();
	return (ret <= 3);
}

