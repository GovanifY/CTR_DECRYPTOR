#define NDS
#define ENABLEWR
#include "main.h"
#include "memory.h"
#include "2d.h"
#include "math.h"
#include "3dstypes.h"
#include "constants.h"
#include <stdio.h>
#include <string.h>
#include "crypto.h"
#include "FS.h"

void createpad(void *counter, void *keyY, void *filename, u32 megabytes, u8 padnum){
	u8 ctr[16];
	u8 buffer[4*1024];
	u8 zeroed_inp[16];
	
	memcpy( ctr, counter, 16);

	#define BLOCK_SIZE 0x1000 //only 4KB, unless we can find a larger open spot in ARM9 mem.
	#define AES_BIG_INPUT      1
	#define AES_NORMAL_INPUT   4

	print("Creating pad ", 190, (padnum)*10, 0, 0, 255);
	print_u8(padnum, 300, (padnum)*10, 0, 0, 255);

	u32 i = 0;
	u32 j = 0;
	u32 t;

	memset( zeroed_inp, 0, 16 );

	u32 handle = FileOpen(filename);
	setup_aeskey( 0x2C, AES_BIG_INPUT|AES_NORMAL_INPUT, keyY);
	use_aeskey( 0x2C);

	print("Completed", 00, 210, 255, 0, 0);
	print("Out of", 00, 220, 255, 0, 0);
	print_u32(megabytes*1024*1024,  100, 220, 255, 0, 0);

	for( i=0; i<megabytes*1024*1024; i+=BLOCK_SIZE )
	{
		for( j=0; j<BLOCK_SIZE; j+=16 )
		{
				set_ctr( AES_BIG_INPUT|AES_NORMAL_INPUT, ctr);
				aes_decrypt( zeroed_inp, (void *)(buffer+j), ctr, 1 );
				add_ctr( ctr, 1);
		}
		FileWriteOffset( handle, buffer, BLOCK_SIZE, i );

		if(!(i&0xFFFFF))
		{
			t = i;
			print_u32(t,  100, 210, 255, 0, 0);
		}
	}

	blank(00,210,200,20);
	print( "Done!", 340,(padnum)*10,0,0,255);
}

int main()
{ 

	blank(0,0,400,240);

	u8 ncchinfo[2004]; //enough room for 20 file infos

	u32 entries[4];
	u32 handle = FileOpen( L"/ncchinfo.bin");
	FileRead( handle, entries, 16);
	
	if(!entries[0] || entries[0]>20)
	{
		print( "Nothing to do. :/",80,10,0,0,255);
		return 0;
	}
	FileRead( handle, ncchinfo, entries[0]*100+4);
/*
4 bytes number of entries

entry:
        16 bytes CTR
        16 bytes keyY
        4 bytes length in megabytes(round up)
        64 bytes output filename in utf16(could use something like "/ncchcode1.romfs.xorpad")
*/

	#define entry_ctr 0+4 //+4 because first int in the array is not an entry
	#define entry_keyY 16+4
	#define entry_megabytes 32+4
	#define entry_filename 36+4
	u8 i = 0;
	u32 e = 0;
	for(i = 0; i< entries[0]; i++)
	{
		e = i*100;
		createpad((void *)(ncchinfo+e+entry_ctr), (void *)(ncchinfo+e+entry_keyY), (void *)(ncchinfo+e+entry_filename), *(u32 *)(ncchinfo+e+entry_megabytes), i+1);
	}

	print( "Finished!", 10,10,0,0,255);

	return 0;
}
