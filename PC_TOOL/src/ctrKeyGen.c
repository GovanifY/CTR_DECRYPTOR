/*
 * ctrKeyGen.c
 * 8/28/2014
 * Coded by Relys
 * Based on makerom by 3dsguy (structs and ctr method ripped from there)
 * Used for generating custom file format to be used with xor generator on 3ds coded by sbJFn5r.
 * xor generator based off of base libraries provided by Kane49. Interaction with the AES engine based off of crypto libs coded by megazig.
 * Special thanks to sm for providing file system libs.
 * xor generator coded by sbJFn5r, Relys
 */
 
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
#include <wchar.h>
 
typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;
typedef unsigned long long      u64;
 
typedef struct
{
        u8 signature[0x100];
        u8 magic[4];
        u8 ncchSize[4];
        u8 titleId[8];
        u8 makerCode[2];
        u8 formatVersion[2];
        u8 padding0[4];
        u8 programId[8];
        u8 padding1[0x10];
        u8 logoHash[0x20]; // SHA-256 over the entire logo region
        u8 productCode[0x10];
        u8 exhdrHash[0x20]; // SHA-256 over exhdrSize of the ExHeader region
        u8 exhdrSize[4];
        u8 padding2[4];
        u8 flags[8];
        u8 plainRegionOffset[4];
        u8 plainRegionSize[4];
        u8 logoOffset[4];
        u8 logoSize[4];
        u8 exefsOffset[4];
        u8 exefsSize[4];
        u8 exefsHashSize[4];
        u8 padding4[4];
        u8 romfsOffset[4];
        u8 romfsSize[4];
        u8 romfsHashSize[4];
        u8 padding5[4];
        u8 exefsHash[0x20];
        u8 romfsHash[0x20];
} ncch_hdr;
 
typedef struct
{
        u16 formatVersion;
        u32 exhdrOffset;
        u32 exhdrSize;
        u32 acexOffset;
        u32 acexSize;
        u64 logoOffset;
        u64 logoSize;
        u64 plainRegionOffset;
        u64 plainRegionSize;
        u64 exefsOffset;
        u64 exefsSize;
        u64 exefsHashDataSize;
        u64 romfsOffset;
        u64 romfsSize;
        u64 romfsHashDataSize;
        u8 titleId[8];
        u8 programId[8];
} ncch_info;
 
typedef enum
{
        BE = 0,
        LE = 1
} endianness_flag;
 
typedef enum
{
        ncch_exhdr = 1,
        ncch_exefs,
        ncch_romfs,
} ncch_section;
 
 
void endian_memcpy(u8 *destination, u8 *source, u32 size, int endianness)
{
        for (u32 i = 0; i < size; i++){
                switch (endianness){
                case(BE):
                        destination[i] = source[i];
                        break;
                case(LE):
                        destination[i] = source[((size-1)-i)];
                        break;
                }
        }
}
 
void GetNcchAesCounter(ncch_info *ctx, u8 counter[16], u8 type)
{
        u8 *titleId = ctx->titleId;
        u32 i;
        u32 x = 0;
 
        memset(counter, 0, 16);
 
        if (ctx->formatVersion == 2 || ctx->formatVersion == 0)
        {
                endian_memcpy(counter,titleId,8,LE);
                counter[8] = type;
        }
        else if (ctx->formatVersion == 1)
        {
                switch(type){
                case ncch_exhdr : x = ctx->exhdrOffset; break;
                case ncch_exefs : x = ctx->exefsOffset; break;
                case ncch_romfs : x = ctx->romfsOffset; break;
                }
                for(i=0; i<8; i++)
                        counter[i] = titleId[i];
                for(i=0; i<4; i++)
                        counter[12+i] = x>>((3-i)*8);
        }
 
        //memdump(stdout,"CTR: ",counter,16);
}
 
typedef struct
{
        u8 offset[4];
        u8 size[4];
} ncch_offsetsize;
 
typedef struct
{
        u8 signature[0x100];
        u8 magic[4];
        u8 mediaSize[4];
        u8 titleId[8];
        u8 padding0[0x10];
        ncch_offsetsize offset_sizeTable[8];
        u8 padding1[0x28];
        u8 flags[8];
        u8 ncchIdTable[8][8];
        u8 padding2[0x30];
} cci_hdr;
 
int roundUp(int numToRound, int multiple)
{
    if (multiple == 0)
        return numToRound;
 
    int remainder = abs(numToRound) % multiple;
    if (remainder == 0)
        return numToRound;
    if (numToRound < 0)
        return -(abs(numToRound) - remainder);
    return numToRound + multiple - remainder;
}
 
 
int main(int argc, char *argv[])
{
 
        //Checks for input arguments
        if(argc<2)
        {
                printf("ctrKeyGen.exe <input.3ds>");
                return 0;
        }
 
        //file setup;
        FILE * pFile;
        long lSize;
 
        //Opens file for reading
        pFile = fopen ( argv[1] , "rb" );
        if (pFile==NULL) { printf("Error: Couldn't open input file"); return 1;}
 
        // obtain file size for input file:
        fseek (pFile , 0 , SEEK_END);
        lSize = ftell (pFile);
        rewind (pFile);
       
                //file setup for output;
        FILE * pFileOut;
                //Opens file for reading
        pFileOut = fopen ( "ncchinfo.bin" , "wb" );
        if (pFileOut==NULL) { printf("Error: Couldn't output file"); return 1;}
       
        //setup counter for entries (# of xor pads to be generated)
        int entries=0;
        fwrite(&entries,sizeof(int),1,pFileOut);
       
 
        //setup NCSD header;
        cci_hdr ncsd;
        fseek(pFile,0x0,SEEK_SET);
        fread(&ncsd, sizeof(cci_hdr), 1, pFile);
 
        //Go through lists of ncch header offsets found in the ncsd table
        for(int ii=0;ii<8;ii++)
        {
       
                //grab the offset of the current ncch header for the current partition
                ncch_offsetsize ncchOffset;
                ncchOffset=ncsd.offset_sizeTable[ii];  
 
                //checks if the header exists for the current partition.
                if((*(u32*)ncchOffset.offset)!=0)
                {
                        //Prints out offset for NCCH header in the current partition
                        printf("\nNCCH Offset:\n");
                        printf(" %08X ",(*(u32*)ncchOffset.offset)*0x200);
 
                        //Time to read the ncch header into a struct. Seek position according to the ncch header table defined in the ncsd header
                        fseek(pFile,(*(u32*)ncchOffset.offset)*0x200,SEEK_SET);
                       
                        //header to read into
                        ncch_hdr hdr;
 
                        //read header
                        fread(&hdr, sizeof(ncch_hdr), 1, pFile);
 
                        //print out product code
                        printf("\nProduct Code:\n");
                        printf("%.*s",0x10,hdr.productCode);
                       
                        //print out current partition
                        printf("\nPartition #:\n");
                        printf("%d",ii);
                        //print out keyY which is first 16 bytes of the ncch RSA signature
                        printf("\nKeyY:\n");
                        for(int iii=0;iii<16;iii++)
                        {
                                printf(" %02X ", hdr.signature[iii] );
                        }
 
                        //Print out titleID
                        printf("\nTitle ID:\n");
                        for(int iii=0;iii<8;iii++)
                        {
                                printf(" %02X ", hdr.titleId[iii] );
                        }
 
                        //Print out Format version
                        printf("\nFormat Version:\n");
                        for(int iii=0;iii<2;iii++)
                        {
                                printf(" %02X ", hdr.formatVersion[iii] );
                        }
                        //Define exhdrOffset. This is a constant variable from what I can tell in makerom
                        //ctx->exhdrOffset=0x200;
                        u64 exhdrOffset=0x200;
                       
                        //Print out exhdr offset
                        if((*(u32*)hdr.exhdrSize))
                        {
                        printf("\nExHeader Offset:\n");
                        printf(" %02X ", exhdrOffset);
                        }
                        //print of exefs offset
                        printf("\nExeFS Offset:\n");
                        for(int iii=0;iii<4;iii++)
                        {
                                printf(" %02X ", hdr.exefsOffset[iii] );
                        }
 
                        //printout romfs offset
                        printf("\nRomFS Offset:\n");
                        for(int iii=0;iii<4;iii++)
                        {
                                printf(" %02X ", hdr.romfsOffset[iii] );
                        }
 
                        //Put the ncch header into the ncch info struct to be used with the makerom functions.
                        ncch_info ctx;
                        memcpy(&ctx.titleId,&hdr.titleId,8);
                        memcpy(&ctx.formatVersion,&hdr.formatVersion,2);
                        memcpy(&ctx.exhdrOffset,&exhdrOffset,4);
                        memcpy(&ctx.exefsOffset,&hdr.exefsOffset,4);
                        memcpy(&ctx.romfsOffset,&hdr.romfsOffset,4);
                       
                        //declare ctr for exhdr
                        u8 exhdr_ctr[16];
                       
                        //calculate ctr for exhdr
                        GetNcchAesCounter(&ctx,exhdr_ctr,ncch_exhdr);
 
                        //Grab size of exhdr and grab megabytes for xor to generate
                        u32 exhdrSize = (*(u32*)hdr.exhdrSize);
                        u32 exhdrSizeMegabytes=roundUp(exhdrSize*0x200, 1024*1024) / (1024*1024);
                       
                        //Check if the exhdr exists in the current partition
                        if(exhdrSize!=0)
                        {
                        entries++;
                                printf("\nExHeaderFS CTR:\n");
                                for(int iii=0;iii<16;iii++)
                                {
                                        printf(" %02X ", exhdr_ctr[iii] );
                                }
                                printf("\nExHeader Megabytes (rounded up):\n");
                                printf("%d",exhdrSizeMegabytes);
                                fwrite(&exhdr_ctr,16,1,pFileOut);
                                fwrite(&hdr.signature,16,1,pFileOut);
                                fwrite(&exhdrSizeMegabytes,4,1,pFileOut);
                               
                                wchar_t exhdrDirWide[32];
                                //Initialize exhdr
                                for(int jj=0;jj<32;jj++)
                                {
                                exhdrDirWide[jj]=0;
                                }
                               
                                exhdrDirWide[0]='/';
                                for(int jj=0;jj<16;jj++)
                                {
                                exhdrDirWide[1+jj]=hdr.productCode[jj];
                                }
                                char exhdrBuff[1];
                                sprintf(exhdrBuff, "%d", ii);
                                exhdrDirWide[11]=exhdrBuff[0];
                                exhdrDirWide[12]='.';
                                exhdrDirWide[13]='e';
                                exhdrDirWide[14]='x';
                                exhdrDirWide[15]='h';
                                exhdrDirWide[16]='.';
                                exhdrDirWide[17]='x';
                                exhdrDirWide[18]='o';
                                exhdrDirWide[19]='r';
                                exhdrDirWide[20]='p';
                                exhdrDirWide[21]='a';
                                exhdrDirWide[22]='d';
                               
                                //swprintf(exhdrDirWide, 32, L"%hs", "/TEST.BIN");
                                fwrite(&exhdrDirWide,32,2,pFileOut);
                        }
 
                        //declare ctr for exefs
                        u8 exefs_ctr[16];
                       
                        //calculate exefs for exhdr
                        GetNcchAesCounter(&ctx,exefs_ctr,ncch_exefs);
 
                        //Grab size of exefs and grab megabytes for xor to generate
                        u32 exefsSize = (*(u32*)hdr.exefsSize);
                        u32 exefsSizeMegabytes=roundUp(exefsSize*0x200, 1024*1024) / (1024*1024);
                       
                        //Check if the exefs exists in the current partition
                        if(exefsSize!=0)
                        {
                        entries++;
                                printf("\nExeFS CTR:\n");
                                for(int iii=0;iii<16;iii++)
                                {
                                        printf(" %02X ", exefs_ctr[iii] );
                                }
                                printf("\nExeFS Megabytes (rounded up):\n");
                                printf("%d",exefsSizeMegabytes);
                                fwrite(&exefs_ctr,16,1,pFileOut);
                                fwrite(&hdr.signature,16,1,pFileOut);
                                fwrite(&exefsSizeMegabytes,4,1,pFileOut);
                               
                                wchar_t exefsDirWide[32];
                                //Initialize exefs
                                for(int jj=0;jj<32;jj++)
                                {
                                exefsDirWide[jj]=0;
                                }
                               
                                exefsDirWide[0]='/';
                                for(int jj=0;jj<16;jj++)
                                {
                                exefsDirWide[1+jj]=hdr.productCode[jj];
                                }
                                char exefsBuff[1];
                                sprintf(exefsBuff, "%d", ii);
                                exefsDirWide[11]=exefsBuff[0];
                                exefsDirWide[12]='.';
                                exefsDirWide[13]='e';
                                exefsDirWide[14]='x';
                                exefsDirWide[15]='e';
                                exefsDirWide[16]='f';
                                exefsDirWide[17]='s';
                                exefsDirWide[18]='.';
                                exefsDirWide[19]='x';
                                exefsDirWide[20]='o';
                                exefsDirWide[21]='r';
                                exefsDirWide[22]='p';
                                exefsDirWide[23]='a';
                                exefsDirWide[24]='d';
                               
                                //swprintf(exefsDirWide, 32, L"%hs", "/TEST.BIN");
                                fwrite(&exefsDirWide,32,2,pFileOut);
                        }
 
                        //declare ctr for romfs
                        u8 romfs_ctr[16];
                       
                        //calculate romfs for exhdr
                        GetNcchAesCounter(&ctx,romfs_ctr,ncch_romfs);
 
                        //Grab size of romfs and grab megabytes for xor to generate
                        u32 romfsSize = (*(u32*)hdr.romfsSize);
                        u32 romfsSizeMegabytes=roundUp(romfsSize*0x200, 1024*1024) / (1024*1024);
                       
                        //Check if the romfs exists in the current partition
                        if(romfsSize!=0)
                        {
                        entries++;
                                printf("\nRomFS CTR:\n");
                                for(int iii=0;iii<16;iii++)
                                {
                                        printf(" %02X ", romfs_ctr[iii] );
                                }
                                printf("\nRomFS Megabytes (rounded up):\n");
                                printf("%d",romfsSizeMegabytes);
                                fwrite(&romfs_ctr,16,1,pFileOut);
                                fwrite(&hdr.signature,16,1,pFileOut);
                                fwrite(&romfsSizeMegabytes,4,1,pFileOut);
                               
                                wchar_t romfsDirWide[32];
                               
                                //Initialize romfs
                                for(int jj=0;jj<32;jj++)
                                {
                                romfsDirWide[jj]=0;
                                }
                               
                                romfsDirWide[0]='/';
                                for(int jj=0;jj<16;jj++)
                                {
                                romfsDirWide[1+jj]=hdr.productCode[jj];
                                }
                                char romfsBuff[1];
                                sprintf(romfsBuff, "%d", ii);
                                romfsDirWide[11]=romfsBuff[0];
                                romfsDirWide[12]='.';
                                romfsDirWide[13]='r';
                                romfsDirWide[14]='o';
                                romfsDirWide[15]='m';
                                romfsDirWide[16]='f';
                                romfsDirWide[17]='s';
                                romfsDirWide[18]='.';
                                romfsDirWide[19]='x';
                                romfsDirWide[20]='o';
                                romfsDirWide[21]='r';
                                romfsDirWide[22]='p';
                                romfsDirWide[23]='a';
                                romfsDirWide[24]='d';
                               
                                //swprintf(romfsDirWide, 32, L"%hs", "/TEST.BIN");
                               
                                fwrite(&romfsDirWide,32,2,pFileOut);
                        }
 
                printf("\n=======\n");
                }
 
               
        }
       
        //print out total entries recorded.
        printf("\nTotal Entries:\n");
        printf("%d",entries);
       
        //Seek back to beginning of file and update the number of entries to the final value.
        fseek(pFileOut,0x0,SEEK_SET);
        fwrite(&entries,sizeof(int),1,pFileOut);
       
       
        //Don't forget to close files kiddies. :p
        fclose (pFile);
        fclose (pFileOut);
 
 
        return 0;
}