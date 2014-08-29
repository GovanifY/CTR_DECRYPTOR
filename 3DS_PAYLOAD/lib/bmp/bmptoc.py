import sys;
import os
import pprint
from PIL import Image,ImageDraw



cwd = os.getcwd() 

if "\\" not in sys.argv[1]:
	sys.argv[1] = cwd+"\\"+sys.argv[1]

trans = (255,0,255)
grey  = (192,192,192)
black  = (0,0,0)
white  = (255,255,255)

filename = os.path.basename(sys.argv[1] )
extension = os.path.splitext(filename)[1][1:]
file_base = os.path.basename(os.path.splitext(sys.argv[1])[0])

org = Image.open(sys.argv[1])  

pixels = org.load()

colors = {}

size = (org.size[0],org.size[1])             # size of the image to create

width = org.size[0]
height = org.size[1]

bits = ""
num_components = len(pixels[0,0])

byte_array = [0 for x in range(org.size[0]*org.size[1])]

def get_color(_pixel):

	pixel = _pixel[0:3]

	#if (len(pixel) > 3):
	#	if (pixel[3] < 128):
	#			return "00";
		

	#print pixel

	if (pixel == trans):
		return "00"
	if (pixel == black):
		return "01"
	if (pixel == white):
		return "10"
	if (pixel == grey):
		return "11"
	return "00"

for j in range(org.size[1]):
	for i in range(org.size[0]):
		r,g,b = 0,0,0
		if num_components == 4:
			px = pixels[i,j]
			r = (pixels[i,j][0] // 32)
			g = (pixels[i,j][1] // 32)
			b = (pixels[i,j][2] // 64)

			#if px[3] > 128:
				#npixels[i,j] = (r*32,g*32,b*64) 
			#else: 
				#npixels[i,j] = (0,0,0)

		if num_components == 3:
			px = pixels[i,j]
			r = (pixels[i,j][0] // 32)
			g = (pixels[i,j][1] // 32)
			b = (pixels[i,j][2] // 64)
			#npixels[i,j] = (r*32,g*32,b*62)

		if num_components == 1:
			px = pixels[i,j]
			r = (pixels[i,j][0] // 32)
			g = (pixels[i,j][1] // 32)
			b = (pixels[i,j][2] // 64)
			#npixels[i,j] = (r,g,b)

		bits = bits + get_color(pixels[i,j])
		#byte_array[(j*width+i)] = get_color(pixels[i,j])
		#print "org "+str(r)+"    "+str(g)+"    "+str(b)
		#print "org "+str(pixels[i,j])
		#print "red "+str(r)+" "+str(g)+" "+str(b)
		#byte =  str(bin(r)[2:]).zfill(3)+""+str(bin(g)[2:]).zfill(3)+""+str(bin(b)[2:]).zfill(2)
		#print pixels[i,j]
			#bit_array[j*org.size[0]+i] = 0
			#bit_array[j*org.size[0]+i] = 1
		#hex(int(byte,2))
		#print "bi2 0b"+str((bin(r//32)[2:5]).zfill(3))+""+str((bin(g//32)[2:5]).zfill(3))+""+str((bin(g//64)[2:4]).zfill(2))

#for b in byte_array:
#	bits = bits + b


im = Image.new('RGB', size) # create the image
npixels = im.load();

print len(bits)
print (height*width)*2
print len(byte_array)

#for y in range(height):
#	for x in range(width):
#	
#		#print ((y*width+x)*2)
#		if  bits[((y*width+x)*2):((y*width+x)*2)+2]  == "01":
#			npixels[x,y] = black
#		if  bits[((y*width+x)*2):((y*width+x)*2)+2] == "10":
#			npixels[x,y] = white
#		if  bits[((y*width+x)*2):((y*width+x)*2)+2] == "11":
#			npixels[x,y] = grey





f = open(file_base+".c",'w+');
f.write("/*\nC Source File for "+file_base+"\n*/\n");

#include "bmp_file.h"

#static bmp_file font = {0,1,{0,1}};
f.write("#include \"bmp_file.h\"\n");
f.write("static bmp_file "+file_base+" ={"+str(org.size[0])+","+str(org.size[1])+",{\n")
#f.write("unsigned static char "+file_base[0:10]+"{\n");
#f.write("\nunsigned static int "+file_base[0:10]+"_size = "+str(len(byte_array)/4)+";\n");
#f.write("unsigned static int "+file_base[0:10]+"_width = "+str(org.size[0])+";\n");
#f.write("unsigned static int "+file_base[0:10]+"_height = "+str(org.size[1])+";\n");


hex_values = []

i = 0


print bits

while (i < len(bits)-7):
	bitstring = int(bits[i:i+8])
	hex_values.append(bitstring)
	f.write(hex(int(str(bitstring),2))+",");
	if (i%128 == 0):
		f.write("\n")
	#f.write(hex(int(bitstring,2))+",");
	#print int(bits[i:i+8])
	i = i+8


while (i < len(byte_array)-1):
	bitstring = byte_array[i]+byte_array[i+1]+byte_array[i+2]+byte_array[i+3]
	
	#hex_values.append (hex(int(bitstring,2)))
	if (i % 16 == 0):
		f.write("\n");	
	i = i+4

print len(hex_values)


def paint(x,y, color):
	npixels[x,y] = color	
	#print "painting %d + %d - %s" % (x,y,str(color))
i = 0
while i < (len(hex_values)):

	x = (i*4%width)
	y = (i*4/width) 
	bstring = str(hex_values[i]).zfill(8)

	b1 = int(bstring[0:2],2)
	b2 = int(bstring[2:4],2)
	b3 = int(bstring[4:6],2)
	b4 = int(bstring[6:8],2)

	#print str(b1)+" "+str(b2)+" "+str(b3)+" "+str(b4)

	if (b1 == 0):
		paint(x,y,trans)
	if (b1 == 1):
		paint(x,y,black)
	if (b1 == 2):
		paint(x,y,white)
	if (b1 == 3):
		paint(x,y,grey)

	if (b2 == 0):
		paint(x+1,y,trans)
	if (b2 == 1):
		paint(x+1,y,black)
	if (b2 == 2):
		paint(x+1,y,white)
	if (b2 == 3):
		paint(x+1,y,grey)

	if (b3 == 0):
		paint(x+2,y,trans)
	if (b3 == 1):
		paint(x+2,y,black)
	if (b3 == 2):
		paint(x+2,y,white)
	if (b3 == 3):
		paint(x+2,y,grey)

	if (b4 == 0):
		paint(x+3,y,trans)
	if (b4 == 1):
		paint(x+3,y,black)
	if (b4 == 2):
		paint(x+3,y,white)
	if (b4 == 3):
		paint(x+3,y,grey)

	i = i+1;
#	print 8;

	#f.write(str(+",");
#	if (i % 10 == 0):
#		f.write("\n")
f.write("}};")
os.system("del tst.png")
im.save("tst.png")





		#print str(r)+" "+str(g)+" "+str(b)
		

#print len(colors)