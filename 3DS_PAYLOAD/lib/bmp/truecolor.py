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

print filename

org = Image.open(sys.argv[1])  

pixels = org.load()

colors = {}

size = (org.size[0],org.size[1])             # size of the image to create

width = org.size[0]
height = org.size[1]

bits = ""
num_components = len(pixels[0,0])

byte_array = []

for j in range(org.size[1]):
	for i in range(org.size[0]):
		color = pixels[i,j][0:3]
		r = color[0]//32
		g = color[1]//32
		b = color[2]//64
		value = "%s%s%s" % (bin(r)[2:].zfill(3),bin(g)[2:].zfill(3),bin(b)[2:].zfill(2))
		byte_array.append(int(value,2));
		
i = 0

cp = file_base.upper()+"_C"

im = Image.new('RGB', size) # create the image
npixels = im.load();

f = open(file_base+".c",'w+');
f.write("/*\nC Source File for "+file_base+"\n*/\n");
f.write("#ifndef "+cp+"\n")
f.write("#define "+cp+"\n")
f.write("#include \"bmp_file.h\"\n");
f.write("static bmp_file "+file_base+" ={"+str(org.size[0])+","+str(org.size[1])+",{\n")


#for y in range(org.size[1]):
#	for x in range(org.size[0]):
#		b = byte_array[y*org.size[0]+x]
#		bi = bin(b)[2:].zfill(8)
#		r = int(bi[0:3],2)*32
#		g = int(bi[3:6],2)*32
#		_b = int(bi[6:8],2)*64
#		npixels[x,y] = (r,g,_b)


for b in byte_array:
	bi = bin(b)[2:].zfill(8)
	r = int(bi[0:3],2)*32
	g = int(bi[3:6],2)*32
	_b = int(bi[6:8],2)*64

	#print "%d %d %d" % (r,g,b)
	x = i%width
	y = (i-x)//width
	npixels[x,y] = (r,g,_b)
	
	f.write(hex(b)+",")
	if (i % 16 == 0): 
		f.write("\n")
	i=i+1


f.write("}};\n")
f.write("#endif \n")

os.system("del tst.png")
im.save("tst.png")


print "Save"


		#print str(r)+" "+str(g)+" "+str(b)
		

#print len(colors)