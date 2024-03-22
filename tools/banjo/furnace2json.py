#!/usr/bin/env python3

# furnace2json
# Joe Kennedy 2024

import sys
import struct
import json
import zlib
import math
from pathlib import Path
from optparse import OptionParser

MAGIC_STRING = "-Furnace module-"

song = {
    'format_version': 0,
    'song_info_pointer': 0,
    'song_name': "",
    'song_author': "",
    
    'time_base': 0,
    'speed_1': 0,
    'speed_2': 0,
    'tics_per_second': 0,
    
    'pattern_length': 0,
    'orders_length': 0,
    
    'sound_chips': [],
    'channel_count': 0,
    
    'instrument_count': 0,
    'instrument_pointers': [],
    'instruments': [],
    
    'wavetable_count': 0,
    'wavetable_pointers': [],
    'wavetables': [],
    
    'sample_count': 0,
    'sample_pointers': [],
    'samples': [],
    
    'pattern_count': 0,
    'pattern_pointers': [],
    'patterns': [],
    
    'orders': [],
    
    'effect_columns': [],
}

# number of channels for each chip indexed by its id
chip_channels = {
    0x00:  0, # end of list
    0x01: 17, # YMU759 - 17 channels
    0x02: 10, # Genesis - 10 channels (compound!)
    0x03:  4, # SMS (SN76489) - 4 channels
    0x04:  4, # Game Boy - 4 channels
    0x05:  6, # PC Engine - 6 channels
    0x06:  5, # NES - 5 channels
    0x07:  3, # C64 (8580) - 3 channels
    0x08: 13, # Arcade (YM2151+SegaPCM) - 13 channels (compound!)
    0x09: 13, # Neo Geo CD (YM2610) - 13 channels
    0x42: 13, # Genesis extended - 13 channels
    0x43: 13, # SMS (SN76489) + OPLL (YM2413) - 13 channels (compound!)
    0x46: 11, # NES + VRC7 - 11 channels (compound!)
    0x47:  3, # C64 (6581) - 3 channels
    0x49: 16, # Neo Geo CD extended - 16 channels
    0x80:  3, # AY-3-8910 - 3 channels
    0x81:  4, # Amiga - 4 channels
    0x82:  8, # YM2151 - 8 channels
    0x83:  6, # YM2612 - 6 channels
    0x84:  2, # TIA - 2 channels
    0x85:  4, # VIC-20 - 4 channels
    0x86:  1, # PET - 1 channel
    0x87:  8, # SNES - 8 channels
    0x88:  3, # VRC6 - 3 channels
    0x89:  9, # OPLL (YM2413) - 9 channels
    0x8a:  1, # FDS - 1 channel
    0x8b:  3, # MMC5 - 3 channels
    0x8c:  8, # Namco 163 - 8 channels
    0x8d:  6, # YM2203 - 6 channels
    0x8e: 16, # YM2608 - 16 channels
    0x8f:  9, # OPL (YM3526) - 9 channels
    0x90:  9, # OPL2 (YM3812) - 9 channels
    0x91: 18, # OPL3 (YMF262) - 18 channels
    0x92: 28, # MultiPCM - 28 channels (UNAVAILABLE)
    0x93:  1, # Intel 8253 (beeper) - 1 channel
    0x94:  4, # POKEY - 4 channels
    0x95:  8, # RF5C68 - 8 channels
    0x96:  4, # WonderSwan - 4 channels
    0x97:  6, # Philips SAA1099 - 6 channels
    0x98:  8, # OPZ (YM2414) - 8 channels
    0x99:  1, # Pokemon Mini - 1 channel
    0x9a:  3, # AY8930 - 3 channels
    0x9b: 16, # SegaPCM - 16 channels
    0x9c:  6, # Virtual Boy - 6 channels
    0x9d:  6, # VRC7 - 6 channels
    0x9e: 16, # YM2610B - 16 channels
    0x9f:  6, # ZX Spectrum (beeper, tildearrow engine) - 6 channels
    0xa0:  9, # YM2612 extended - 9 channels
    0xa1:  5, # Konami SCC - 5 channels
    0xa2: 11, # OPL drums (YM3526) - 11 channels
    0xa3: 11, # OPL2 drums (YM3812) - 11 channels
    0xa4: 20, # OPL3 drums (YMF262) - 20 channels
    0xa5: 14, # Neo Geo (YM2610) - 14 channels
    0xa6: 17, # Neo Geo extended (YM2610) - 17 channels
    0xa7: 11, # OPLL drums (YM2413) - 11 channels
    0xa8:  4, # Atari Lynx - 4 channels
    0xa9:  5, # SegaPCM (for DefleMask compatibility) - 5 channels
    0xaa:  4, # MSM6295 - 4 channels
    0xab:  1, # MSM6258 - 1 channel
    0xac: 17, # Commander X16 (VERA) - 17 channels
    0xad:  2, # Bubble System WSG - 2 channels
    0xae: 42, # OPL4 (YMF278B) - 42 channels (UNAVAILABLE)
    0xaf: 44, # OPL4 drums (YMF278B) - 44 channels (UNAVAILABLE)
    0xb0: 16, # Seta/Allumer X1-010 - 16 channels
    0xb1: 32, # Ensoniq ES5506 - 32 channels
    0xb2: 10, # Yamaha Y8950 - 10 channels
    0xb3: 12, # Yamaha Y8950 drums - 12 channels
    0xb4:  5, # Konami SCC+ - 5 channels
    0xb5:  8, # tildearrow Sound Unit - 8 channels
    0xb6:  9, # YM2203 extended - 9 channels
    0xb7: 19, # YM2608 extended - 19 channels
    0xb8:  8, # YMZ280B - 8 channels
    0xb9:  3, # Namco WSG - 3 channels
    0xba:  8, # Namco C15 - 8 channels
    0xbb:  8, # Namco C30 - 8 channels
    0xbc:  8, # MSM5232 - 8 channels
    0xbd: 11, # YM2612 DualPCM extended - 11 channels
    0xbe:  7, # YM2612 DualPCM - 7 channels
    0xbf:  4, # T6W28 - 4 channels
    0xc0:  1, # PCM DAC - 1 channel
    0xc1: 10, # YM2612 CSM - 10 channels
    0xc2: 18, # Neo Geo CSM (YM2610) - 18 channels (UNAVAILABLE)
    0xc3: 10, # YM2203 CSM - 10 channels (UNAVAILABLE)
    0xc4: 20, # YM2608 CSM - 20 channels (UNAVAILABLE)
    0xc5: 20, # YM2610B CSM - 20 channels (UNAVAILABLE)
    0xc6:  2, # K007232 - 2 channels
    0xc7:  4, # GA20 - 4 channels
    0xc8:  3, # SM8521 - 3 channels
    0xc9: 16, # M114S - 16 channels (UNAVAILABLE)
    0xca:  5, # ZX Spectrum (beeper, QuadTone engine) - 5 channels
    0xcb:  3, # Casio PV-1000 - 3 channels
    0xcc:  4, # K053260 - 4 channels
    0xcd:  2, # TED - 2 channels
    0xce: 24, # Namco C140 - 24 channels
    0xcf: 16, # Namco C219 - 16 channels
    0xd0: 32, # Namco C352 - 32 channels (UNAVAILABLE)
    0xd1: 18, # ESFM - 18 channels
    0xd2: 32, # Ensoniq ES5503 (hard pan) - 32 channels (UNAVAILABLE)
    0xd4:  4, # PowerNoise - 4 channels
    0xde: 19, # YM2610B extended - 19 channels
    0xe0: 19, # QSound - 19 channels
    0xfc:  1, # Pong - 1 channel
    0xfd:  8, # Dummy System - 8 channels
    0xfe:  0, # reserved for development
    0xff:  0 # reserved for development
}

# conversion for size of macro data from the macro size type
macro_word_size_convert = [1, 1, 2, 4]
macro_word_struct_string = ["B", "b", "<h", "<i"]

def main(argv=None):
    print("furnace2json - Joe Kennedy 2024")

    parser = OptionParser("Usage: furnace2json.py [options] INPUT_FILE_NAME.FUR")
    parser.add_option("-o", '--out',        dest='outfilename',                                      help='output file name')

    (options, args) = parser.parse_args()

    if (len(args) == 0):
        parser.print_help()
        parser.error("Input file name required\n")
    
    infilename = Path(args[0])
    out_filename = Path(options.outfilename) if (options.outfilename) else infilename.with_suffix('.json')
        
    # open source file 
    try:
        infile = open(str(infilename), "rb")
        # read into variable
        data = infile.read()
        infile.close()
    except OSError:
        print("Error reading input file: " + str(infilename), file=sys.stderr)
        sys.exit(1)
        
    print ("Loaded " + str(infilename))
    
    # file is zlib compressed, decompress it
    if data[0:1].decode("utf-8") == "x":
        print (".fur is zlib compresed, decompressing")
        data = zlib.decompress(data)
    
    # check that the file is valid
    if data[0:16].decode("utf-8") != MAGIC_STRING:
        print("file is not a valid furnace .fur", file=sys.stderr)
        sys.exit(1)
    
    # get song info
    song['format_version'] = struct.unpack("<H", data[16:18])[0]
    song['song_info_pointer'] = struct.unpack("<I", data[20:24])[0]
    
    sip = song['song_info_pointer']
    
    song['time_base'] = struct.unpack("B", data[sip + 8 : sip + 9])[0]
    song['speed_1'] = struct.unpack("B", data[sip + 9 : sip + 10])[0]
    song['speed_2'] = struct.unpack("B", data[sip + 10 : sip + 11])[0]
    song['tics_per_second'] = struct.unpack("f", data[sip + 12 : sip + 16])[0]
    
    song['pattern_length'] = struct.unpack("<H", data[sip + 16 : sip + 18])[0]
    song['orders_length'] = struct.unpack("<H", data[sip + 18 : sip + 20])[0]
    
    song['instrument_count'] = struct.unpack("<H", data[sip + 22 : sip + 24])[0]
    song['wavetable_count'] = struct.unpack("<H", data[sip + 24 : sip + 26])[0]
    song['sample_count'] = struct.unpack("<H", data[sip + 26 : sip + 28])[0]
    song['pattern_count'] = struct.unpack("<I", data[sip + 28 : sip + 32])[0]
    
    song['sound_chips'] = struct.unpack_from("B" * 32, data[sip + 32 : sip + 64])
    song['sound_chips'] = list(filter(lambda x: x > 0, song['sound_chips']))
    
    # calculate number of channels in this song
    song['channel_count'] = 0
    
    for chip in song['sound_chips']:    
        song['channel_count'] += chip_channels[chip]
    
    # get song name from null terminated string
    song_name_start = sip + 256
    song_name_end = sip + 256
    
    while struct.unpack("B", data[song_name_end:song_name_end+1])[0] != 0:
        song_name_end += 1
    
    # get song author from null terminated string
    song_author_start = song_name_end + 1
    song_author_end = song_author_start
    
    while struct.unpack("B", data[song_name_end:song_name_end+1])[0] != 0:
        song_author_end += 1
    
    song['song_name'] = data[song_name_start:song_name_end].decode("utf-8")
    song['song_author'] = data[song_author_start:song_author_end].decode("utf-8")
    
    
    # instrument pointers start 24 bytes after the song author
    # pointers are 32 bit uints
    instrument_pointers = song_author_end + 1 + 24
    instrument_pointers_end = instrument_pointers + (song['instrument_count'] * 4)
    song['instrument_pointers'] = list(struct.unpack("<" + ("I" * song['instrument_count']), data[instrument_pointers:instrument_pointers_end]))
    
    # wavetable pointers
    # pointers are 32 bit uints
    wavetable_pointers = instrument_pointers_end
    wavetable_pointers_end = wavetable_pointers + (song['wavetable_count'] * 4)
    song['wavetable_pointers'] = list(struct.unpack("<" + ("I" * song['wavetable_count']), data[wavetable_pointers:wavetable_pointers_end]))
    
    # sample pointers
    # pointers are 32 bit uints
    sample_pointers = wavetable_pointers_end
    sample_pointers_end = sample_pointers + (song['sample_count'] * 4)
    song['sample_pointers'] = list(struct.unpack("<" + ("I" * song['sample_count']), data[sample_pointers:sample_pointers_end]))
    
    # pattern pointers
    # pointers are 32 bit uints
    pattern_pointers = sample_pointers_end
    pattern_pointers_end = pattern_pointers + (song['pattern_count'] * 4)
    song['pattern_pointers'] = list(struct.unpack("<" + ("I" * song['pattern_count']), data[pattern_pointers:pattern_pointers_end]))
    
    # order pointers
    # channel_count * orders_length array of pattern numbers
    order_pointers = pattern_pointers_end
    order_pointers_end = order_pointers + (song['orders_length'] * song['channel_count'])
    
    for i in range (0, song['channel_count']):
        order_array_start = order_pointers + (i * song['orders_length'])
        order_array_end = order_pointers + ((i + 1) * song['orders_length'])
        song['orders'].append(list(struct.unpack("B" * song['orders_length'], data[order_array_start:order_array_end])))
    
    # number of effect columns each channel has
    effect_columns_start = order_pointers_end
    effect_columns_end = effect_columns_start + song['channel_count']
    song['effect_columns'] = list(struct.unpack("B" * song['channel_count'], data[effect_columns_start:effect_columns_end]))
    
    # init pattern arrays
    for i in range(0, song['channel_count']):
        song['patterns'].append([])
    
    # load patterns
    print ("furnace format version: " + str(song['format_version']))
    
    # legacy patttern format
    if song['format_version'] < 157:
    
        for i in range (0, song['pattern_count']):
    
            pattern_pointer = song['pattern_pointers'][i]
    
            pattern_header = struct.unpack("<IIHHHH", data[pattern_pointer : pattern_pointer + 16])
    
            pattern_channel = pattern_header[2]
    
            pattern = {
                'channel': pattern_channel,
                'index': pattern_header[3],
                'data': [],
            }
            
            # size of each row in the pattern
            pattern_row_size = (4 + (song['effect_columns'][pattern_channel] * 2)) * 2
            
            # get data for each row
            pattern_data_pointer = pattern_pointer + 16
            
            for j in range (0, song['pattern_length']):
    
                pattern_row_pointer = pattern_data_pointer + (pattern_row_size * j)
    
                pattern_row_data = struct.unpack("<hhhh", data[pattern_pointer : pattern_pointer + 8])
                
                line = {
                    'note': pattern_row_data[0],
                    'octave': pattern_row_data[1],
                    'instrument': pattern_row_data[2],
                    'volume': pattern_row_data[3],
                    'effects': []
                }
                
                # get effects for row
                for k in range (0, song['effect_columns'][pattern_channel]):
                    
                    effect_data_pointer = pattern_row_pointer + 8 + (k * 2)
    
                    effect = struct.unpack("bb", data[effect_data_pointer:effect_data_pointer + 2])
    
                    # only add to json if the effect is valid
                    if (effect[0] != -1 or effect[1] != -1):
                        line['effects'].append(effect[0])
                        line['effects'].append(effect[1])
                
                # add line to pattern
                pattern['data'].append(line)
    
            # add to patterns list
            song['patterns'][pattern_channel].append(pattern)
    
    # newer pattern format
    else:
        
        for i in range(0, song['pattern_count']):
    
            pattern_pointer = song['pattern_pointers'][i]
    
            pattern_header = struct.unpack("<IIBBH", data[pattern_pointer + 0 : pattern_pointer + 12])
    
            pattern = {
                #'_debug': [],
                'channel': pattern_header[3],
                'index': pattern_header[4],
                'name': "",
                'data': []
            }       
    
            # get data for each row
            pattern_data_pointer = pattern_pointer + 12
    
            pattern_data_pointer_start = pattern_data_pointer
    
            # pattern name
            while True:
                read_byte = struct.unpack("B", data[pattern_data_pointer:pattern_data_pointer+1])[0]
                pattern_data_pointer +=  1
    
                # done when reach end of null terminated string
                if read_byte == 0:
                    break
                
                pattern['name'] += read_byte
            
            # pattern_data_pointer now pointing at pattern data
            while True:
    
                read_byte = struct.unpack("B", data[pattern_data_pointer:pattern_data_pointer+1])[0]
                pattern_data_pointer += 1
    
                # done when we hit 0xff
                if read_byte == 0xff:
                    break
    
                line = {
                    'note': -1,
                    'octave': -1,
                    'instrument': -1,
                    'volume': -1,
                    'effects': []
                }
    
                # if bit 7 is set, then read bit 0-6 as "skip N+2 rows".
                if read_byte & 0x80:
                    
                    for j in range(0, (read_byte & 0x7f) + 2):
                        pattern['data'].append(line)
                
                # this line has something on it
                else:
    
                    # read the extra effect bytes if they're present
                    #
                    # - if bit 5 is set, read another byte:
                    # extra effects 0-3
                    # - if bit 6 is set, read another byte:
                    # extra effects 4-7
                    effect_bytes = []
                    extra_effect_byte_count = 0
    
                    if (read_byte & 0x20):
                        extra_effect_byte_count += 1
    
                    if (read_byte & 0x40):
                        extra_effect_byte_count += 1
    
                    # get effect specification bytes
                    effect_bytes = list(data[pattern_data_pointer:pattern_data_pointer + extra_effect_byte_count])
                    pattern_data_pointer += extra_effect_byte_count
    
                    # bit 0: note present
                    if (read_byte & 0x1):
                    
                        line['note'] = struct.unpack("B", data[pattern_data_pointer:pattern_data_pointer+1])[0]
                        pattern_data_pointer += 1
    
                        # convert note data to version compatible with older (< 157) pattern
                        if (line['note'] >= 0) and (line['note'] <= 179):
                        
                            line['octave'] = int(math.floor(line['note'] / 12) - 5)
                            line['note'] = line['note'] % 12
                        
                        else:
    
                            line['note'] = line['note'] - 80
                    
                    # bit 1: ins present
                    if (read_byte & 0x2):
                    
                        line['instrument'] = struct.unpack("B", data[pattern_data_pointer:pattern_data_pointer+1])[0]
                        pattern_data_pointer += 1
                    
                    # bit 2: volume present
                    if (read_byte & 0x4):
                    
                        line['volume'] = struct.unpack("B", data[pattern_data_pointer:pattern_data_pointer+1])[0]
                        pattern_data_pointer += 1
    
                    # if bit 5 is present, we'll get the bytes for effect 0 at the next stage
                    # otherwise load effect 0 now if it's present
                    if ((read_byte & 0x20) == 0):
                    
                        # bit 3: effect 0 present
                        # bit 4: effect value 0 present
                        if ((read_byte & 0x8) and (read_byte & 0x10)):
                        
                            line['effects'].append(struct.unpack("B", data[pattern_data_pointer:pattern_data_pointer+1])[0])
                            pattern_data_pointer += 1
                            line['effects'].append(struct.unpack("B", data[pattern_data_pointer:pattern_data_pointer+1])[0])
                            pattern_data_pointer += 1
                        
                        elif (read_byte & 0x8):
                        
                            line['effects'].append(struct.unpack("B", data[pattern_data_pointer:pattern_data_pointer+1])[0])
                            pattern_data_pointer += 1
    
                            line['effects'].append(-1)
                        
                        elif (read_byte & 0x10):
                        
                            line['effects'].append(-1)
                            
                            line['effects'].append(struct.unpack("B", data[pattern_data_pointer:pattern_data_pointer+1])[0])
                            pattern_data_pointer += 1
    
                    # get effect data
                    for j in range(0, extra_effect_byte_count):
                    
                        # byte says which extra effects are in use
                        fx_byte = effect_bytes[j]
    
                        # parse them in pairs
                        for k in range(0, 8, 2):
                        
                            if ((fx_byte & 0x3) == 0x3):
                            
                                line['effects'].append(struct.unpack("B", data[pattern_data_pointer:pattern_data_pointer+1])[0])
                                pattern_data_pointer += 1
                                line['effects'].append(struct.unpack("B", data[pattern_data_pointer:pattern_data_pointer+1])[0])
                                pattern_data_pointer += 1
                            
                            elif (fx_byte & 0x1):
                            
                                line['effects'].append(struct.unpack("B", data[pattern_data_pointer:pattern_data_pointer+1])[0])
                                pattern_data_pointer += 1
    
                                line['effects'].append(-1)
                            
                            elif (fx_byte & 0x2):
                            
                                line['effects'].append(-1)
    
                                line['effects'].append(struct.unpack("B", data[pattern_data_pointer:pattern_data_pointer+1])[0])
                                pattern_data_pointer += 1
                            
                            fx_byte = fx_byte >> 2
    
                    # append completed line
                    pattern['data'].append(line)
            
            #pattern['_debug'] = list(data[pattern_data_pointer_start:pattern_data_pointer+1])
    
    
            # we've hit a 0xff to say the data is over but we might not have
            # pattern data for all rows - so fill in rest of pattern
            while len(pattern['data']) < song['pattern_length']:
                pattern['data'].append({
                    'note': -1,
                    'octave': -1,
                    'instrument': -1,
                    'volume': -1,
                    'effects': []
                })
    
            # add to patterns list
            song['patterns'][pattern['channel']].append(pattern)        
    
    
    # load instruments
    for i in range (0, song['instrument_count']):
        
        instrument_pointer = song['instrument_pointers'][i]
    
        instrument_header = struct.unpack("<IIHH", data[instrument_pointer:instrument_pointer + 12])
    
        instrument = {
            'name': "",
            'type': instrument_header[3],
            'size': instrument_header[1],
            'macros': [],
            'features': [],
        }
    
        # go through features and add them to the instrument
        feature_pointer = instrument_pointer + 12
        
        while (feature_pointer < instrument_pointer + instrument['size']):
        
            feature_header = struct.unpack("<HH", data[feature_pointer:feature_pointer + 4])
    
            feature = {
                'code': data[feature_pointer:feature_pointer + 2].decode(),
                'length': feature_header[1],
                'data': [],
            }
            
            # move pointer past header
            feature_pointer += 4
            
            # Instrument name feature
            if (feature['code'] == "NA"):
            
                instrument['name'] = data[feature_pointer : feature_pointer + feature['length'] - 1].decode()
            
            # End features if we reach this code
            elif (feature['code'] == "EN"):
            
                break
            
            # OPL drums 
            elif (feature['code'] == "LD"):
            
                opll_drum_data = struct.unpack("<BHHH", data[feature_pointer:feature_pointer + 7])
    
                instrument['opl_drums'] = {
                    'fixed_freq': opll_drum_data[0],
                    'kick_freq': opll_drum_data[1],
                    'snare_hat_freq': opll_drum_data[2],
                    'tom_top_freq': opll_drum_data[3],
                }
            
            # Macros
            elif (feature['code'] == "MA"):
            
                header_length = struct.unpack("<H", data[feature_pointer:feature_pointer + 2])[0]
    
                macro_pointer = feature_pointer + 2
                
                while (macro_pointer < feature_pointer + feature['length']):
                
                    macro_data = struct.unpack("BBBBBBBB", data[macro_pointer : macro_pointer + 8])
    
                    macro = {
                        'code': macro_data[0],
                        'length': macro_data[1],                        
                        'loop': macro_data[2],
                        'release': macro_data[3],
                        'mode': macro_data[4],
                        'word_size': macro_word_size_convert[(macro_data[5] >> 6) & 0x3],
                        'type': (macro_data[5] >> 1) & 0x3,
                        'delay': macro_data[6],
                        'speed': macro_data[7],
                        'data': []
                    }
                    
                    # end of macros
                    if (macro['code'] == 255):
    
                        break
                    
                    
                    # advance pointer to macro data
                    macro_pointer += header_length
                    
                    # get macro data
                    for j in range(0, macro['length']):
                        macro_data_pointer = macro_pointer + (j * macro['word_size'])
                        macro['data'].append(
                            struct.unpack(
                                macro_word_struct_string[macro['word_size']],
                                data[macro_data_pointer : macro_data_pointer + macro['word_size']]
                            )[0]
                        )
                    
                    # add macro to list
                    if (macro['code'] == 0):
                    
                        instrument['volume_macro'] = macro
                    
                    else:
                    
                        instrument['macros'].append(macro)
                    
                    
                    # advance pointer
                    macro_pointer += macro['length'] * macro['word_size']
    
            # FM patch
            elif (feature['code'] == "FM"):
            
                fm_pointer = feature_pointer
                fm_data = struct.unpack("BBBB", data[fm_pointer : fm_pointer + 4])
                
                instrument['fm'] = {
                    'op_count': fm_data[0] & 0xf,
                    'op_enabled': fm_data[0] >> 4,
                    
                    'feedback': fm_data[1] & 0x7,
                    'algorithm': (fm_data[1] >> 4) & 0x7,
                    
                    'fms': fm_data[2] & 0x7,
                    'ams': (fm_data[2] >> 3) & 0x3,
                    'fms2': (fm_data[2] >> 5) & 0x7,
                    
                    'opll_patch': fm_data[3] & 0x1f,
                    'am2': (fm_data[3] >> 6) & 0x3,
                    
                    'operator_data': [],
                }
                
                operator_pointer = feature_pointer + 4
                
                # get operator data for each operator
                for j in range (0, instrument['fm']['op_count']): 
                
                    operator_data = struct.unpack("BBBBBBBB", data[operator_pointer : operator_pointer + 8])
    
                    operator = {
                        'mult': operator_data[0] & 0xf,
                        'dt': (operator_data[0] >> 4) & 0x7,
                        'ksr': operator_data[0] >> 7,
                        
                        'tl': operator_data[1] & 0x7f,
                        'sus': operator_data[1] >> 7,
                        
                        'ar': operator_data[2] & 0x1f,
                        'vib': (operator_data[2] >> 5) & 0x1,
                        'rs': (operator_data[2] >> 6) & 0x3,
                        
                        'dr': operator_data[3] & 0x1f,
                        'ksl': (operator_data[3] >> 5) & 0x3,
                        'am': operator_data[3] >> 7,
                        
                        'd2r': operator_data[4] & 0x1f,
                        'kvs': (operator_data[4] >> 5) & 0x3,
                        'egt': operator_data[4] >> 7,
                        
                        'rr': operator_data[5] & 0xf,
                        'sl': operator_data[5] >> 4,
                        
                        'ssg': operator_data[6] & 0xf,
                        'dvb': operator_data[6] >> 4,
                        
                        'ws': operator_data[7] & 0x7,
                        'dt2': (operator_data[7] >> 3) & 0x3,
                        'dam': (operator_data[7] >> 5) & 0x7,
                    }
                                        
                    instrument['fm']['operator_data'].append(operator)
                                        
                    operator_pointer += 8
                
    
            else:
            
                feature['data'] = list(data[feature_pointer:feature_pointer + feature['length']])
                
                instrument['features'].append(feature)
            
            
            # advance pointer
            feature_pointer += feature['length']
        
        
        # add instrument
        song['instruments'].append(instrument)
    
    # wavetables
    for i in range(0, song['wavetable_count']):
        
        wavetable_pointer = song['wavetable_pointers'][i]
    
        wavetable = {
            'size': struct.unpack("<I", data[wavetable_pointer + 4:wavetable_pointer + 8])[0],
            'name': "",
            'width': 0,
            'height': 0,
            'data': []
        }
    
        # get wavetable name
        wavetable_pointer += 8
    
        while True:
    
            read_byte = struct.unpack("B", data[wavetable_pointer])[0]
            wavetable_pointer += 1
    
            if read_byte == 0:
                break
    
            wavetable['name'] += read_byte
    
        wavetable['width'] = struct.unpack("<I", data[wavetable_pointer:wavetable_pointer + 4])[0]
        wavetable['height'] = struct.unpack("<I", data[wavetable_pointer + 8:wavetable_pointer + 12])[0]
    
        # point at data
        wavetable_pointer += 12
        wavetable_size = wavetable['width'] * 4
        wavetable['data'].append(list(struct.unpack("<" + ("I" * wavetable['width']), data[wavetable_pointer:wavetable_pointer + wavetable_size])))
    
        song['wavetables'].append(wavetable)
    
    
    
    
    try:
        outfile = open(str(out_filename), 'w')
        # write out json file
        json.dump(song, outfile, indent=1)
        outfile.close()
    except OSError:
        print ("Error writing to output file: " + str(out_filename), file=sys.stderr)
        sys.exit(1)
        
    print ("Written output file: " + str(out_filename))

if __name__=='__main__':
    main()
