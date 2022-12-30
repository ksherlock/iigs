#
# extract icons from an icon file.
# outputs rez
#
# See FTN $CA

import sys
import getopt
import struct


class Icon:
# Icon:
# uint16_t IconType ($8000 = color, $0000 = b/w)
# uint16_t IconSize
# uint16_t IconHeight
# uint16_t IconWidth
# uint18_t IconImage[height * width / 2]
# uint18_t IconMask[height * width / 2]

	def __init__(self):
		self.type = 0
		self.size = 0
		self.height = 0
		self.width = 0
		self.image = []
		self.mask = []

	def unpack_from(self, buffer, offset):
		x = struct.unpack_from("<HHHH", buffer, offset)
		self.type = x[0]
		self.size = x[1]
		self.height = x[2]
		self.width = x[3]

		offset = offset + 8
		size = self.size
		height = self.height
		width = (self.width + 1) >> 1

		data = buffer[offset:offset+size]
		self.image = [ data[x*width:(x+1)*width] for x in range(0, height)]
		offset += size

		data = buffer[offset:offset+size]
		self.mask = [ data[x*width:(x+1)*width] for x in range(0, height)]
		offset += size

		return offset

class IconData:

# uint16_t iDataLen
# uint8_t iDataBoss[64]
# uint8_t iDataName[16]
# uint16_t iDataType
# uint16_t iDataAux
# Icon iDataBig
# Icon iDataSmall
#


	def __init__(self):
		self.len = 0
		self.boss = ""
		self.name = ""
		self.type = 0
		self.aux = 0
		self.big = None
		self.small = None

	def unpack_from(self, buffer, offset):

		l, = struct.unpack_from("<H", buffer, offset)
		if l == 0:
			self.len = 0
			return offset

		x = struct.unpack_from("<H64s16sHH", buffer, offset)
		self.len = x[0]
		self.boss = x[1]
		self.name = x[2]
		self.type = x[3]
		self.aux = x[4]

		offset = offset + 86

		self.big = Icon()
		self.small = Icon()

		offset = self.big.unpack_from(buffer, offset)
		offset = self.small.unpack_from(buffer, offset)

		return offset

ResID = 100

def rez(icon):
	global ResID

	print("resource rIcon({}) {{".format(ResID))
	ResID = ResID + 1

	print("\t${:04x}, // type".format(icon.type))
	print("\t{}, // height".format(icon.height))
	print("\t{}, // width".format(icon.width))

	# print(icon.image)
	# print(icon.mask)
	print()
	for x in icon.image:
		print("\t$\"{}\"".format(x.hex()))
	print("\t,")
	for x in icon.mask:
		print("\t$\"{}\"".format(x.hex()))
	print()
	print("};")
	print()


def decompile(file):
	with open(file, "rb") as io:
		buffer = io.read()

	offset = 26
	# don't care about first 26-bytes

	while True:

		# print("offset:", offset)
		icon = IconData()
		offset = icon.unpack_from(buffer, offset)
		if icon.len == 0: break;

		rez(icon.big)
		rez(icon.small)



def usage(rv):
	print("Usage: icon.py file")
	sys.exit(rv)

try:
	opt, arg = getopt.getopt(sys.argv[1:], "")
except getopt.GetoptError:
    usage(2)

if not len(arg):
	usage(2)

for file in arg:
	decompile(file)
