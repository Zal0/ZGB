#!/usr/bin/env python3
import sys
import wave
from pathlib import Path
from optparse import OptionParser
from PIL import Image
from functools import reduce

def extract_tile(pixels, x, y, bits, height, pal = None):
    tile = []
#    pal = [3, 2, 1, 0]
    width = 8
    for dy in range(height):
        a = b = c = d = 0
        w = 0
        for dx in range(8):
            px = pixels[(x * 8) + dx, (y * 8) + dy]
            if (pal != None):
                f, idx = (True, pal[px]) if px in range(len(pal)) else (False, 0)
            else:
                f, idx = (True, px) if px in range(1 << bits) else (False, 0)            
            a = a << 1 | (idx >> 0) & 0x01
            b = b << 1 | (idx >> 1) & 0x01
            c = c << 1 | (idx >> 2) & 0x01
            d = d << 1 | (idx >> 3) & 0x01
            w += 1 if f else 0
        width = min(width, w)
        tile.append(a); 
        if (bits >= 2): 
            tile.append(b)
        if (bits == 4): 
            tile.append(c)
            tile.append(d)
    return width, tile

def extract_metatile(pixels, x, y, dx, dy, bits, height):
    metatile = []
    for i in range(dy):
      for j in range(dx):
         width, tile = extract_tile(pixels, x + j, y + i, bits, height)
         metatile.append(tile)
    return reduce(lambda x,y:x+y, metatile)

def append_metatile(storage, metatile):
    for i in range(len(storage)):
        if storage[i] == metatile: 
            return True, i
    storage.append(metatile)
    return False, len(storage) - 1

def output_array(outf, name, array, width):
    outf.write(bytes("static const uint8_t {0:s}[] = {{\n".format(name),"ascii"))
    idx = 0
    for v in array:
        outf.write(bytes("0x{:02x},".format(v), "ascii"))
        if idx >= (width - 1):
            outf.write(b"\n")
            idx = 0
        else:
            idx += 1
    if (idx != 0): outf.write(b"\n")
    outf.write(b"};\n")

def main(argv=None):
    parser = OptionParser("Usage: png2metatiles.py [options] INPUT_FILE_NAME.PNG")
    parser.add_option("-o", '--out',        dest='outfilename',                                      help='output file name')
    parser.add_option("-i", '--identifier', dest='identifier',                                       help='source identifier')
    parser.add_option("-p", '--bpp',        dest='bpp',         default="2",                         help='bits per pixel: 1, 2 or 4')
    parser.add_option("-x", '--width',      dest='width',       default="2",                         help='metatile width in tiles')
    parser.add_option("-y", '--height',     dest='height',      default="2",                         help='metatile height in tiles')
    parser.add_option("-l", '--tileheight', dest='tileheight',  default="8",                         help='tile height in pixels: 8 or 16')    

    parser.add_option("-b", '--bank',       dest='bank',        default="255",                       help='BANK number (default AUTO=255)')    

    (options, args) = parser.parse_args()

    if (len(args) == 0):
        parser.print_help()
        parser.error("Input file name required\n")
    
    infilename = Path(args[0])
    
    if options.outfilename == None:
        outfilename = infilename.with_suffix('.c')
    else:
        outfilename = Path(options.outfilename)
        
    if options.identifier == None:
        identifier = str(Path(infilename.name).with_suffix(''))
    else: 
        identifier = options.identifier

    if not int(options.bpp) in [1, 2, 4]:
        parser.print_help()
        parser.error("Supported bit per pixel values are: 1, 2 or 4")

    if not int(options.tileheight) in [8, 16]:
        parser.print_help()
        parser.error("Supported tile height values are: 8, 16")

    with Image.open(infilename) as source:
        w, h = source.size
        w //= 8; h //= int(options.tileheight)

        pixels = source.load()

        indexes, metatiles = [], []
           
        idx = 0
        for y in range(h // int(options.height)):
            for x in range(w // int(options.width)):
                found, idx = append_metatile(metatiles, 
                                             extract_metatile(pixels, x * int(options.width), y * int(options.height), 
                                                              int(options.width), int(options.height), 
                                                              int(options.bpp), 
                                                              int(options.tileheight)
                                                             )
                                            )
                indexes.append(idx)
       
        # output C source file
        with open(str(outfilename), "wb") as outf:                
            outf.write(bytes("#pragma bank {1:s}\n\n"
                             "#include <stdint.h>\n"
                             "#include \"gbdk/platform.h\"\n\n"
                             "BANKREF({0:s})\n\n".format(identifier, options.bank), "ascii"))
            
            for i in range(len(metatiles)):
                output_array(outf, "{:s}{:d}_tiles".format(identifier, i), metatiles[i], 8 * int(options.bpp))
            
            outf.write(bytes("const uint8_t * const {:s}[] = {{\n".format(identifier), "ascii"))
            for i in range(len(indexes)):
                outf.write(bytes("{:s}{:s}{:d}_tiles".format("" if i == 0 else ",", identifier, indexes[i]), "ascii"))
            outf.write(bytes("\n};\n", "ascii"))
            
        # output C header file
        if outfilename.suffix == ".c":
            with open(str(outfilename.with_suffix('.h')), "wb") as hdrf:
                hdrf.write(bytes(("#ifndef __{0:s}_INCLUDE__\n"
                                  "#define __{0:s}_INCLUDE__\n\n"
                                  "#include <stdint.h>\n"
                                  "#include \"gbdk/platform.h\"\n\n"
                                  "BANKREF_EXTERN({0:s})\n\n"
                                  "#define {0:s}_WIDTH {1:d}\n"
                                  "#define {0:s}_HEIGHT {2:d}\n"
                                  "#define {0:s}_META_WIDTH {3:d}\n"
                                  "#define {0:s}_META_HEIGHT {4:d}\n\n"
                                  "extern const uint8_t * const {0:s}[];\n\n"
                                  "#endif\n").format(identifier, 
                                                     int(options.width), int(options.height), 
                                                     w // int(options.width), h // int(options.height)
                                                    ), "ascii"))

if __name__=='__main__':
    main()
