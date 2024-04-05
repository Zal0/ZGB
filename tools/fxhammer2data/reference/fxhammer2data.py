#!/usr/bin/env python3
import sys
from pathlib import Path
from optparse import OptionParser

note_freqs_gb = [
    44,     156,    262,    363,    457,    547,    631,    710,    786,    854,    923,    986,
    1046,   1102,   1155,   1205,   1253,   1297,   1339,   1379,   1417,   1452,   1486,   1517,
    1546,   1575,   1602,   1627,   1650,   1673,   1694,   1714,   1732,   1750,   1767,   1783,
    1798,   1812,   1825,   1837,   1849,   1860,   1871,   1881,   1890,   1899,   1907,   1915,
    1923,   1930,   1936,   1943,   1949,   1954,   1959,   1964,   1969,   1974,   1978,   1982,
    1985,   1988,   1992,   1995,   1998,   2001,   2004,   2006,   2009,   2011,   2013,   2015
]

def make_frame_gb(ch, a, b, c, d, cache):
    mask = 0b01001000 | ch
    result = ",0x{:02x}".format(a)
    if b != cache[0]:
        mask |= 0b00100000
        result = "{:s},0x{:02x}".format(result, b)
        cache[0] = b
    if c != cache[1]:
        mask |= 0b00010000
        result = "{:s},0x{:02x}".format(result, c)
        cache[1] = c
    return "0b{:08b}{:s},0x{:02x}".format(mask, result, d)    

def write_effect_data_gb(cfile, hfile, identifier, ch, data, options):
    global note_freqs_gb

    if len(data) != 256:
        print("ERROR: Unexpected end of file")
        sys.exit(1)
        
    ch_mask = (0x02 if (ch & 0xf0) else 0) | (0x08 if (ch & 0x0f) else 0)

    cfile.write(bytes(("BANKREF({0:s})\n"
                       "const uint8_t {0:s}[] = {{\n").format(identifier), "ascii"))

    ch2_cache = [-1,-1]
    ch4_cache = [-1,-1]
    old_pan = 0xff

    for i in range(0, 32):
        duration, ch2pan, ch2vol, ch2duty, ch2note, ch4pan, ch4vol, ch4freq = data[i * 8:(i + 1) * 8]
            
        if duration == 0:
            break;

        result = ""
        count = 0

        if options.use_pan:
            current_pan = 0b01010101 | ch2pan | ch4pan
            if (old_pan != current_pan):
                count += 1
                result = "{:s},0b{:08b},0x{:02x}".format(result, 0b01000100, current_pan)
                old_pan = current_pan

        if (ch2pan != 0):
            count += 1
            freq = note_freqs_gb[(ch2note - 0x40) >> 1]
            result = "{:s},{:s}".format(result, make_frame_gb(1, ch2duty, ch2vol, freq & 0xff, ((freq >> 8) | 0x80) & 0xff, ch2_cache))

        if (ch4pan != 0):
            count += 1
            result = "{:s},{:s}".format(result, make_frame_gb(3, 0x2a, ch4vol, ch4freq, 0x80, ch4_cache))

        delay = max(0, (int(options.delay) * duration) - 1)
        delta = min(15, delay)
        
        cfile.write(bytes("0x{:02x}{:s},\n".format(((delta & 0x0f) << 4) | count, result), "ascii"))

        delay -= delta
        while (delay > 0):
            delta = min(15, delay)
            cfile.write(bytes("0x{:02x},\n".format((delta & 0x0f) << 4), "ascii"))
            delay -= delta;

    count = 1
    result = ""
    if options.cut_sound:
        if (ch_mask & 2):
            count += 1
            result = "{:s}0b{:08b},0x{:02x},0x{:02x},".format(result, 0b00101001, 0, 0xc0)

        if (ch_mask & 8):
            count += 1
            result = "{:s}0b{:08b},0x{:02x},0x{:02x},".format(result, 0b00101011, 0, 0xc0)

    if options.use_pan:
        count += 1
        result = "{:s}0b{:08b},0x{:02x},".format(result, 0b01000100, 0xff)
            
    cfile.write(bytes("0x{:02x},{:s}0b{:08b}\n}};\n".format(count, result, 7), "ascii"))    
    cfile.write(bytes("void AT(0b{1:08b}) __mute_mask_{0:s};\n\n".format(identifier, ch_mask), "ascii"))
    
    hfile.write(bytes(("#define MUTE_MASK_{0:s} 0b{1:08b}\n"
                       "BANKREF_EXTERN({0:s})\n"
                       "extern const uint8_t {0:s}[];\n"
                       "extern void __mute_mask_{0:s};\n\n").format(identifier, ch_mask), "ascii"))


note_freqs_psg = [
    0x0357, 0x0327, 0x02FA, 0x02CF, 0x02A8, 0x0280, 0x025C, 0x023B, 0x021B, 0x01FC, 0x01E0, 0x01C5,
    0x01AC, 0x0193, 0x017D, 0x0167, 0x0153, 0x0140, 0x012E, 0x011D, 0x010D, 0x00FE, 0x00F0, 0x00E3,
    0x00D6, 0x00CA, 0x00BE, 0x00B4, 0x00AA, 0x00A0, 0x0097, 0x008F, 0x0087, 0x007F, 0x0078, 0x0071,
    0x006B, 0x0065, 0x005F, 0x005A, 0x0055, 0x0050, 0x004C, 0x0047, 0x0043, 0x0040, 0x003C, 0x0039,
    0x0035, 0x0032, 0x0030, 0x002D, 0x002A, 0x0028, 0x0026, 0x0024, 0x0022, 0x0020, 0x001E, 0x001C,
    0x001B, 0x0019, 0x0018, 0x0016, 0x0015, 0x0014, 0x0013, 0x0012, 0x0011, 0x0010, 0x000F, 0x000E,
]

def make_frame_psg(ch, b, c, cache):
    count = 0
    result = ""
    if b != cache[0]:
        result = "0x{:02x}".format(0b10010000 | ((ch & 0x03) << 5) | (b >> 4))
        cache[0] = b
        count += 1
    if (ch == 1):
        if c != cache[1]:
            result = "{:s}{:s}0x{:02x},0x{:02x}".format(result, "," if (count > 0) else "", 0b10000000 | ((ch & 0x03) << 5) | (c & 0x0f), (c >> 4) & 0x7f)
            cache[1] = c
            count += 2
    elif (ch == 3):
        if c != cache[1]:
            result = "{:s}{:s}0x{:02x}".format(result, "," if (count > 0) else "", 0b10000100 | ((ch & 0x03) << 5) | ((c & 0xC0) >> 6))
            cache[1] = c
            count += 1
         
    return count, result

def write_effect_data_psg(cfile, hfile, identifier, ch, data, options):
    global note_freqs_psg

    if len(data) != 256:
        print("ERROR: Unexpected end of file")
        sys.exit(1)
        
    ch_mask = (0x02 if (ch & 0xf0) else 0) | (0x08 if (ch & 0x0f) else 0)

    cfile.write(bytes(("BANKREF({0:s})\n"
                       "const uint8_t {0:s}[] = {{\n").format(identifier), "ascii"))

    ch2_cache = [-1,-1]
    ch4_cache = [-1,-1]

    for i in range(0, 32):
        duration, ch2pan, ch2vol, ch2duty, ch2note, ch4pan, ch4vol, ch4freq = data[i * 8:(i + 1) * 8]
            
        if duration == 0:
            break;

        result = ""
        count = 0

        if (ch2pan != 0):
            freq = note_freqs_psg[(ch2note - 0x40) >> 1]
            cnt, tmp = make_frame_psg(1, ch2vol, freq, ch2_cache)
            count += cnt
            result = "{:s},{:s}".format(result, tmp) if (cnt > 0) else result

        if (ch4pan != 0):
            cnt, tmp = make_frame_psg(3, ch4vol, ch4freq, ch4_cache)
            count += cnt
            result = "{:s},{:s}".format(result, tmp) if (cnt > 0) else result

        delay = max(0, (int(options.delay) * duration) - 1)
        delta = min(15, delay)
        
        cfile.write(bytes("0x{:02x}{:s},\n".format(((delta & 0x0f) << 4) | count, result), "ascii"))

        delay -= delta
        while (delay > 0):
            delta = min(15, delay)
            cfile.write(bytes("0x{:02x},\n".format((delta & 0x0f) << 4), "ascii"))
            delay -= delta;

    count = 0
    result = ""
    if options.cut_sound:
        if (ch_mask & 2):
            count += 1
            result = "{:s}0x{:02x},".format(result, 0b10111111)

        if (ch_mask & 8):
            count += 1
            result = "{:s}0x{:02x},".format(result, 0b11111111)

    if (count != 0):
        cfile.write(bytes("0x{:02x},{:s}\n".format(count, result), "ascii"))    
    cfile.write(bytes("0x00\n};\n", "ascii"))    
    cfile.write(bytes("void AT(0b{1:08b}) __mute_mask_{0:s};\n\n".format(identifier, ch_mask), "ascii"))
    
    hfile.write(bytes(("#define MUTE_MASK_{0:s} 0b{1:08b}\n"
                       "BANKREF_EXTERN({0:s})\n"
                       "extern const uint8_t {0:s}[];\n"
                       "extern void __mute_mask_{0:s};\n\n").format(identifier, ch_mask), "ascii"))


def main(argv=None):
    parser = OptionParser("Usage: fxhammer2data.py [options] INPUT_FILE_NAME.SAV")
    parser.add_option("-o", '--out',        dest='outfilename',                                        help='output file name')
    parser.add_option("-i", '--identifier', dest='identifier',                                         help='source identifier')
    parser.add_option("-n", '--number',     dest='effectnum',                          default="all",  help='effect number or "all"')
    parser.add_option("-m", '--system',     dest='system',                             default="gb",   help='target system GB/PSG (default "GB")')

    parser.add_option("-d", "--delay",      dest='delay',                              default=1,      help='delay size')
    parser.add_option("-b", '--bank',       dest='bank',                               default="255",  help='BANK number (default AUTO=255)')    
    parser.add_option("-c", "--cut",        dest="cut_sound",    action="store_true",  default=False,  help='cut all used sound channels at the end')
    parser.add_option("-p", "--no-pan",     dest="use_pan",      action="store_false", default=True,   help='disable channel panning')

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

    if str(options.system).lower() == 'gb':
        converter = write_effect_data_gb
    elif str(options.system).lower() == 'psg':
        converter = write_effect_data_psg
    else:
        print("ERROR: unknown target system, should be in [GB, PSG]")
        sys.exit(1)
        

    with open(str(infilename), mode="rb") as f:
        data = f.read()

    if data[0x09:0x12] != b"FX HAMMER":
        print("ERROR: Invalid FX HAMMER file format")
        sys.exit(1)

    cfile = open(str(outfilename), "wb")
    cfile.write(bytes(("#pragma bank {0:s}\n\n"
                       "#include <gbdk/platform.h>\n"
                       "#include <stdint.h>\n\n").format(options.bank), "ascii"))

    hfile = open(str(outfilename.with_suffix('.h')), "wb")
    hfile.write(bytes(("#ifndef __{0:s}_INCLUDE__\n"
                       "#define __{0:s}_INCLUDE__\n\n"
                       "#include <gbdk/platform.h>\n"
                       "#include <stdint.h>\n\n").format(identifier), "ascii"))
    
    if str(options.effectnum).lower() == 'all':
        for effectnum in range(0, 0x3C):
            channels = data[0x300 + effectnum]
            if channels != 0:
                converter(cfile, hfile, 
                          "{:s}_{:02x}".format(identifier, effectnum), 
                          channels, 
                          data[0x400 + effectnum * 256:0x400 + (effectnum + 1) * 256], 
                          options)
    else:
        effectnum = max(0, int(options.effectnum, 10))
        converter(cfile, hfile, 
                  identifier, 
                  data[0x300 + effectnum], 
                  data[0x400 + effectnum * 256:0x400 + (effectnum + 1) * 256], 
                  options)
            
    hfile.write(b"#endif\n")

    cfile.close()
    hfile.close()

if __name__=='__main__':
    main()