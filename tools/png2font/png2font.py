#!/usr/bin/env python3

import sys
from pathlib import Path
from optparse import OptionParser
from PIL import Image
from functools import reduce

def extract_tile(pixels, x, y, bits, pal = None):
    tile = []
#    pal = [3, 2, 1, 0]
    width = 8
    for dy in range(8):
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

def output_array(outf, name, array, width, static = True):
    outf.write(bytes("{1:s}const unsigned char {0:s}[] = {{\n".format(name, "static " if static else ""),"ascii"))
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


def append_face(storage, face):
    for i in range(len(storage)):
        if storage[i] == face: 
            return True, i
    storage.append(face)
    return False, len(storage) - 1

def chunks(lst, n):
    for i in range(0, len(lst), n):
        yield tuple(lst[i:i + n])

def main(argv=None):
    parser = OptionParser("Usage: png2font.py [options] INPUT_FILE_NAME.PNG")
    parser.add_option("-o", '--out',        dest='outfilename',                                      help='output file name')
    parser.add_option("-i", '--identifier', dest='identifier',                                       help='source identifier')
    parser.add_option("-p", '--bpp',        dest='bpp',         default="1",                         help='bits per pixel: 1, 2 or 4')
    parser.add_option("-v", '--vwf',        dest='vwf',         default=False,  action="store_true", help='variable width font')
    parser.add_option("-s", '--start',      dest='start',       default="32",                        help='ascii code of the first symbol')    

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
        identifier = str(Path(infilename.name).with_suffix('')).translate(str.maketrans('', '', '!@#$+-><='))
    else: 
        identifier = options.identifier

    if not int(options.bpp) in [1, 2, 4]:
        parser.print_help()
        parser.error("Supported bit per pixel values are: 1, 2 or 4")

    with Image.open(infilename) as font:
        w, h = font.size
        w //= 8; h //= 8

        colors_per_palette = 1 << int(options.bpp)

        pixels = font.load()
        
        palette = list(chunks(font.getpalette(), 3))
        num_palettes = len(palette) // colors_per_palette
        palette = palette[0 : num_palettes * colors_per_palette]
        
        indexes, widths, faces = [0] * 256, [], []
        
        n_faces = 0
        idx = int(options.start)
        for y in range(h):
            for x in range(w):
                width, tile = extract_tile(pixels, x, y, int(options.bpp))
                found, indexes[idx] = append_face(faces, tile)
                if not found:
                    widths.append(width)
                    n_faces += 1
                idx += 1

        is7BIT = True if idx <= 128 else False
       
        if is7BIT:
            indexes = indexes[:128]
       
        # output C source file
        with open(str(outfilename), "wb") as outf:
            if int(options.bank) != 0:
                outf.write(bytes("#pragma bank {:s}\n\n".format(options.bank), "ascii"))
            outf.write(bytes("#include <gbdk/platform.h>\n"
                             "#include \"Font.h\"\n\n"
                             "BANKREF({:s})\n\n".format(identifier), "ascii"))

            outf.write(bytes("const palette_color_t {:s}_palettes[] = {{\n".format(identifier), "ascii"))
            for i in palette:
                outf.write(bytes("RGB8({:3d}, {:3d}, {:3d}),\n".format(*i), "ascii"))
            outf.write(bytes("};\n", "ascii"))

            output_array(outf, "{:s}_table".format(identifier), indexes, 16, False)
            if options.vwf:
                output_array(outf, "{:s}_widths".format(identifier), widths, 16, False)
            output_array(outf, "{:s}_tiles".format(identifier), reduce(lambda x,y :x+y ,faces), 8 * int(options.bpp), False)
                        
            outf.write(bytes("const font_desc_t {0:s}_font = {{\n"
                             "    .attr         = {1:s}{2:s}0,\n"
                             "    .recode_table = {0:s}_table,\n"
                             "{2:s}"
                             "    .faces        = {0:s}_tiles\n"
                             "}};\n\n".format(identifier, 
                                              "RECODE_7BIT | " if is7BIT else "", 
                                              "FONT_VWF | " if options.vwf else "",
                                              ".widths       = {0:s}_widths,\n" if options.vwf else "",
                                              ),
                             "ascii"))

            outf.write(bytes("#include \"TilesInfo.h\"\n"
                             "const struct TilesInfo {0:s} = {{\n"
                             "    .num_frames = {1:d},\n"
                             "    .data = {0:s}_tiles,\n"
                             "    .num_pals = {2:d},\n"
                             "    .pals = {3:s},\n"
                             "    .color_data = NULL\n"
                             "}};\n".format(identifier,
                                           n_faces,
                                           num_palettes,
                                           "NULL" if (num_palettes == 0) else "{:s}_palettes".format(identifier)
                                           ),
                             "ascii"))

        # output C header file
        if outfilename.suffix == ".c":
            with open(str(outfilename.with_suffix('.h')), "wb") as hdrf:
                hdrf.write(bytes(("#ifndef __{0:s}_INCLUDE__\n"
                                  "#define __{0:s}_INCLUDE__\n\n"
                                  "#include <gbdk/platform.h>\n"
                                  "#include \"Font.h\"\n"
                                  "#include \"TilesInfo.h\"\n\n"
                                  "BANKREF_EXTERN({0:s})\n\n"
                                  "extern const unsigned char {0:s}_table[];\n"
                                  "extern const font_desc_t {0:s}_font[];\n"
                                  "extern const struct TilesInfo {0:s};\n\n"
                                  "#endif\n").format(identifier), "ascii"))

if __name__=='__main__':
    main()
