#!/usr/bin/env python3
import sys
from struct import unpack
from pathlib import Path
from optparse import OptionParser

CHIP_UNSUP, CHIP_GB, CHIP_PSG = 0, 1, 2

# --- Game Boy ------------------------------------------------

NR10_REG, NR11_REG, NR12_REG, NR13_REG, NR14_REG = 0x10, 0x11, 0x12, 0x13, 0x14 # NR1x
NR20_REG, NR21_REG, NR22_REG, NR23_REG, NR24_REG = 0x15, 0x16, 0x17, 0x18, 0x19 # NR2x
NR30_REG, NR31_REG, NR32_REG, NR33_REG, NR34_REG = 0x1A, 0x1B, 0x1C, 0x1D, 0x1E # NR3x
NR40_REG, NR41_REG, NR42_REG, NR43_REG, NR44_REG = 0x1F, 0x20, 0x21, 0x22, 0x23 # NR4x
NR50_REG, NR51_REG, NR52_REG = 0x24, 0x25, 0x26                                 # RE5x
PCM_SAMPLE  = 0x30    # PCM wave pattern
PCM_LENGTH  = 0x10    # PCM wave pattern

reg_names = {
    NR10_REG: "NR10_REG", NR11_REG: "NR11_REG", NR12_REG: "NR12_REG", NR13_REG: "NR13_REG", NR14_REG: "NR14_REG", 
                          NR21_REG: "NR21_REG", NR22_REG: "NR22_REG", NR23_REG: "NR23_REG", NR24_REG: "NR24_REG", 
    NR30_REG: "NR30_REG", NR31_REG: "NR31_REG", NR32_REG: "NR32_REG", NR33_REG: "NR33_REG", NR34_REG: "NR34_REG", 
                          NR41_REG: "NR41_REG", NR42_REG: "NR42_REG", NR43_REG: "NR43_REG", NR44_REG: "NR44_REG", 
    NR50_REG: "NR50_REG", NR51_REG: "NR51_REG", NR52_REG: "NR52_REG", 
    PCM_SAMPLE +  0: "PCM[0]", PCM_SAMPLE +  1: "PCM[1]", PCM_SAMPLE +  2: "PCM[2]", PCM_SAMPLE +  3: "PCM[3]", 
    PCM_SAMPLE +  4: "PCM[4]", PCM_SAMPLE +  5: "PCM[5]", PCM_SAMPLE +  6: "PCM[6]", PCM_SAMPLE +  7: "PCM[7]", 
    PCM_SAMPLE +  8: "PCM[8]", PCM_SAMPLE +  9: "PCM[9]", PCM_SAMPLE + 10: "PCM[A]", PCM_SAMPLE + 11: "PCM[B]", 
    PCM_SAMPLE + 12: "PCM[C]", PCM_SAMPLE + 13: "PCM[D]", PCM_SAMPLE + 14: "PCM[E]", PCM_SAMPLE + 15: "PCM[F]"
}

NR1x, NR2x, NR3x, NR4x, NR5x, PCMDATA = 0, 1, 2, 3, 4, 5

chn_names = { NR1x: "NR1x", NR2x: "NR2x", NR3x: "NR3x", NR4x: "NR4x", NR5x: "NR5x", PCMDATA: "PCM" }

def inclusive(start, end):
    return range(start, end + 1)

def parse_gameboy(inf, outf, options):
    channel_mute_mask = 0
    outf.write(bytes(("#pragma bank {1:s}\n\n"
                      "#include <gbdk/platform.h>\n"
                      "#include <stdint.h>\n\n"
                      "BANKREF({0:s})\n"
                      "const uint8_t {0:s}[] = {{\n").format(options.identifier, options.bank), "ascii"))

    row = {}
    data = inf.read(1)
    while (data):
        if data == b'\xb3':
            addr, data = unpack('BB', inf.read(2))
            addr += NR10_REG
            if (options.verbose): print("{:s} = 0b{:08b}".format(reg_names[addr], data))
            if addr in inclusive(NR10_REG, NR14_REG):
                row.setdefault(NR1x, {})[addr - NR10_REG] = data
            elif addr in inclusive(NR21_REG, NR24_REG):
                row.setdefault(NR2x, {})[addr - NR20_REG] = data
            elif addr in inclusive(NR30_REG, NR34_REG):
                row.setdefault(NR3x, {})[addr - NR30_REG] = data
            elif addr in inclusive(NR41_REG, NR44_REG):
                row.setdefault(NR4x, {})[addr - NR40_REG] = data
            elif addr in inclusive(NR50_REG, NR52_REG):
                row.setdefault(NR5x, {})[addr - NR50_REG] = data
            elif addr in range(PCM_SAMPLE, PCM_SAMPLE + PCM_LENGTH):
                row.setdefault(PCMDATA, {})[addr - PCM_SAMPLE] = data
            else:
                print("ERROR: Invalid register address: 0x{:02x}".format(addr))
                sys.exit(1)
            value = data
        elif (data == b'\x66') or (data >= b'\x61' and data <= b'\x63') or (data >= b'\x70' and data <= b'\x7f'):
            if data == b'\x61': 
                inf.seek(2, 1)                  
                
            if (options.verbose): print("PACKET (reason: {}): {}".format(hex(data[0]), row))
                
            result = ""
            count = 0
            # NR5x regs:
            ch = row.pop(NR5x, None)
            if (ch) and (not options.no_nr5x):
                val = ch.pop(2, -1)
                if (val != -1) and (not options.no_init):
                    count += 1
                    result = "{}0b{:08b},0x{:02x},".format(result, 0b00100100, val)
                mask = NR5x
                tmp = ""
                for i in range(0, 2):
                    val = ch.pop(i, -1)
                    if (val != -1):
                        mask |= 1 << (7 - i)
                        tmp = "{}0x{:02x},".format(tmp, val)
                if (mask != 4):
                    count += 1
                    result = "{}0b{:08b},{}".format(result, mask, tmp)

            # AUD3WAVE[]
            ch = row.pop(PCMDATA, None)
            if (ch) and (not options.no_wave):
                mask = PCMDATA
                tmp = ""
                for i in range(0, 16):
                    val = ch.pop(i, 0)
                    tmp = "{}0x{:02x},".format(tmp, val)
                count += 1
                result = "{}0b{:08b},{}".format(result, mask, tmp)                    

            # NR1x, NR2x, NR3x, NR4x regs:
            for j in inclusive(NR1x, NR4x):
                ch = row.pop(j, None)
                if (ch) and (not j in options.disabled_channels):
                    mask = j
                    tmp = ""
                    for i in range(0, 5):
                        val = ch.pop(i, -1)
                        if (val != -1):
                            mask |= 0b10000000 >> i
                            tmp = "{}0x{:02x},".format(tmp, val)
                    if (mask != j) and ((mask & 0b00001000) != 0):
                        count += 1
                        result = "{}0b{:08b},{}".format(result, mask, tmp)
                        channel_mute_mask |= (1 << j)

            # optional delay
            count |= max(0, int(options.delay) - 1) << 4

            # output result
            result = "0x{:02x},{}\n".format(count, result)
            # Debug line breaks between packets out
            # result = "{}\n".format(result)
            outf.write(bytes(result, "ascii"))

            # reset row
            row = {}

            if data == b'\x66':
                # write terminate sequence and exit
                outf.write(bytes("0x01,0b{:08b}\n}};\n".format(7), "ascii"))
                break;
        else:
            print("ERROR: unsupported command 0x{:02x}".format(unpack('B', data)[0]))
            sys.exit(1)
        data = inf.read(1)
        
    outf.write(bytes("void AT(0b{1:08b}) __mute_mask_{0:s};".format(options.identifier, channel_mute_mask), "ascii"))
    return channel_mute_mask

# --- PSG -----------------------------------------------------

def parse_psg(inf, outf, options):
    channel_mute_mask = 0
    outf.write(bytes(("#pragma bank {1:s}\n\n"
                      "#include <gbdk/platform.h>\n"
                      "#include <stdint.h>\n\n"
                      "BANKREF({0:s})\n"
                      "const uint8_t {0:s}[] = {{\n").format(options.identifier, options.bank), "ascii"))

    row = []
    data = inf.read(1)
    while (data):
        if data == b'\x50':
            value = inf.read(1)[0]
            if ((value & 0x80) != 0):
                channel_mute_mask |= (1 << ((value >> 5) & 3))
            row.append(value)
            if len(row) > 15:
                print("ERROR: Too many PSG writes per frame")
                sys.exit(1)                
            count = 0
        elif (data == b'\x66') or (data >= b'\x61' and data <= b'\x63') or (data >= b'\x70' and data <= b'\x7f'):
            if data == b'\x61': 
                inf.seek(2, 1)                  

            count += 1

            ahead = inf.read(1)
            inf.seek(-1, 1)
            if (data == b'\x66') or not ((ahead >= b'\x61' and ahead <= b'\x63') or (ahead >= b'\x70' and ahead <= b'\x7f')):
                # output packet
                if (options.verbose): print("PACKET (reason: {}): {}".format(hex(data[0]), row))
                    
                # delay + packet length
                count = (min(15, ((count - 1) * max(1, int(options.delay)))) << 4) | (len(row) & 0x0f)

                result = ""
                for v in row:
                    result = "{}0x{:02x},".format(result, v)

                # output result
                result = "0x{:02x},{}\n".format(count, result)
                outf.write(bytes(result, "ascii"))

                # reset row
                row = []

                if data == b'\x66':
                    # write terminate sequence and exit
                    outf.write(bytes("0x00\n};\n", "ascii"))
                    break;
        else:
            print("ERROR: unsupported command 0x{:02x}".format(unpack('B', data)[0]))
            sys.exit(1)
        data = inf.read(1)
        
    outf.write(bytes("void AT(0b{1:08b}) __mute_mask_{0:s};".format(options.identifier, channel_mute_mask), "ascii"))
    return channel_mute_mask

# --- Main ----------------------------------------------------

def main(argv=None):
    parser = OptionParser("Usage: vgm2data.py [options] INPUT_FILE_NAME.VGM")
    parser.add_option("-o", '--out',        dest='outfilename',                                      help='output file name')
    parser.add_option("-i", '--identifier', dest='identifier',                                       help='C source identifier')

    parser.add_option("-1", "--no-nr1x",    dest="no_nr1x",     action="store_true",  default=False, help='disable channel 0')
    parser.add_option("-2", "--no-nr2x",    dest="no_nr2x",     action="store_true",  default=False, help='disable channel 1')
    parser.add_option("-3", "--no-nr3x",    dest="no_nr3x",     action="store_true",  default=False, help='disable channel 2')
    parser.add_option("-4", "--no-nr4x",    dest="no_nr4x",     action="store_true",  default=False, help='disable channel 3')
    parser.add_option("-5", "--no-nr5x",    dest="no_nr5x",     action="store_true",  default=False, help='disable NR5X manipulation')
    parser.add_option("-s", "--no-init",    dest="no_init",     action="store_true",  default=False, help='disable sound init')
    parser.add_option("-w", "--no-wave",    dest="no_wave",     action="store_true",  default=False, help='disable waveform loading')

    parser.add_option("-v", "--verbose",    dest="verbose",     action="store_true",  default=False, help='verbose output')

    parser.add_option("-d", "--delay",      dest='delay',                             default=1,     help='delay size')
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
        options.identifier = str(Path(infilename.name).with_suffix(''))
                
    options.disabled_channels = set()
    if (options.no_nr1x): 
        options.disabled_channels.add(0)
        print("Channel 1 disabled")
    if (options.no_nr2x): 
        options.disabled_channels.add(1)
        print("Channel 2 disabled")
    if (options.no_nr3x): 
        options.disabled_channels.add(2)
        print("Channel 3 disabled")
    if (options.no_nr4x): 
        options.disabled_channels.add(3)
        print("Channel 4 disabled")
    if (options.no_nr5x):
        print("NR5x registers disabled")
    if (options.no_wave):
        print("waveform data update disabled")
               
    with open(str(infilename), "rb") as inf:
        soundchip = CHIP_UNSUP
        channel_mute_mask = 0

        # check VGM format
        if inf.read(4) != b'Vgm ':
            print("invalid file format")
            sys.exit(1)
        inf.seek(0x08)
        file_version = unpack('<I', inf.read(4))[0]
        if (file_version < 0x161):
            print("VGM version too low: {:04X}".format(file_version))
            sys.exit(1)
        else:
            if (options.verbose): print("VGM file version: {:04X}".format(file_version))            
        inf.seek(0x80)
        if (unpack('<I', inf.read(4))[0] != 0):
            soundchip = CHIP_GB
            if (options.verbose): print("Game Boy data detected")            
        else:
            inf.seek(0x0C)
            if (unpack('<I', inf.read(4))[0] != 0):
                soundchip = CHIP_PSG
                if (options.verbose): print("PSG data detected")            
            else:
                print("VGM must contain GameBoy data")
                sys.exit(1)
            
        # output C source file
        with open(str(outfilename), "wb") as outf:                
            inf.seek(0x34)
            offset = unpack('<I', inf.read(4))[0]
            inf.seek(offset - 4, 1)

            if (options.verbose): print("data start position: {:d}".format(inf.seek(0, 1)))

            channel_mute_mask = parse_gameboy(inf, outf, options) if soundchip == CHIP_GB else parse_psg(inf, outf, options)

        # output C header file
        if outfilename.suffix == ".c":
            with open(str(outfilename.with_suffix('.h')), "wb") as hdrf:
                hdrf.write(bytes(("#ifndef __{0:s}_INCLUDE__\n"
                                  "#define __{0:s}_INCLUDE__\n\n"
                                  "#include <gbdk/platform.h>\n"
                                  "#include <stdint.h>\n\n"
                                  "#define MUTE_MASK_{0:s} 0b{1:08b}\n\n"
                                  "BANKREF_EXTERN({0:s})\n"
                                  "extern const uint8_t {0:s}[];\n"
                                  "extern void __mute_mask_{0:s};\n\n"
                                  "#endif\n").format(options.identifier, channel_mute_mask), "ascii"))

if __name__=='__main__':
    main()
