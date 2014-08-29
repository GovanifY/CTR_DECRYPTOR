#!/usr/bin/python
# 3DS Buildscript by Kane40
import sys
import os
import ftplib


BASE_ADDRESS = 0x080C3EE0
CC = "arm-none-eabi-gcc"
CP = "arm-none-eabi-g++"
OC = "arm-none-eabi-objcopy" 
LD = "arm-none-eabi-ld"


def run(cmd):
	#print cmd
	os.system(cmd)

cwd = os.getcwd() 
run("rm obj/*.o")
run("rm bin/*.elf")
run(CC+" -s -Os -g source/*.c -c -mcpu=arm946e-s -march=armv5te -mlittle-endian -fshort-wchar");
#run(CC+" -s -Os -g source/fatfs/*.c -c -mcpu=arm946e-s -march=armv5te -mlittle-endian -fshort-wchar");
run(CC+" -g source/bootloader.s -c -mcpu=arm946e-s -march=armv5te -mlittle-endian -o bootloader.o -fshort-wchar");

run(CC+" -nostdlib -T 3ds.ld *.o libgcc.a libc.a")# -I include/fatfs")
run("cp -r *.o obj/ ")
#run(LD+" -T 3ds.ld *.o")
run("cp a.out bin/homebrew.elf ")
run(OC+" -O binary a.out payload.bin -S")
#run(cwd+"/lib/p3ds/3dsploit.py Launcher.dat "+cwd+"/payload.bin > /dev/null")
run("python "+cwd+"/lib/p3ds/3dsploit.py Launcher2.dat "+cwd+"/payload.bin ")
#run("openssl enc -aes-128-cbc -K 580006192800C5F0FBFB04E06A682088 -iv 00000000000000000000000000000000 -in Launcher2.dat -out Launcher.dat")
#run("cp Launcher.dat F:/MsetForBoss.dat")
#upload("Launcher.dat")
run("rm *.o")
run("rm *.out")
#run("rm Launcher2.dat")
#run("del payload.bin")

