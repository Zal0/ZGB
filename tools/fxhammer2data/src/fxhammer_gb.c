// See LICENSE  file for license details

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>


#include "common.h"
#include "logging.h"
#include "path_ops.h"

#include "fxhammer.h"
#include "fxhammer_gb.h"


static void make_frame_gb(uint8_t channel, 
                   uint8_t a, uint8_t b, uint8_t c, uint8_t d, 
                   int (*cache)[2], FILE * FOUT);


const uint16_t note_freqs_gb[] = {
    44,     156,    262,    363,    457,    547,    631,    710,    786,    854,    923,    986,
    1046,   1102,   1155,   1205,   1253,   1297,   1339,   1379,   1417,   1452,   1486,   1517,
    1546,   1575,   1602,   1627,   1650,   1673,   1694,   1714,   1732,   1750,   1767,   1783,
    1798,   1812,   1825,   1837,   1849,   1860,   1871,   1881,   1890,   1899,   1907,   1915,
    1923,   1930,   1936,   1943,   1949,   1954,   1959,   1964,   1969,   1974,   1978,   1982,
    1985,   1988,   1992,   1995,   1998,   2001,   2004,   2006,   2009,   2011,   2013,   2015
};



static void make_frame_gb(uint8_t channel, 
                   uint8_t a, uint8_t b, uint8_t c, uint8_t d, 
                   int (*cache)[2], FILE * FOUT) {

    // First Mask
    uint8_t mask = 0b01001000 | channel;

    if (b != (*cache)[0])
        mask |= 0b00100000;

    if (c != (*cache)[1])
        mask |= 0b00010000;

    fprintf(FOUT, ",0b%s", char2strbin(mask));

    // Then remaining output
    fprintf(FOUT, ",0x%02x", a);

    if (b != (*cache)[0]) {
        fprintf(FOUT, ",0x%02x", b);
        (*cache)[0] = b;
    }

    if (c != (*cache)[1]) {
        fprintf(FOUT, ",0x%02x", c);
        (*cache)[1] = c;
    }
    fprintf(FOUT, ",0x%02x", d);
}


uint8_t write_effect_data_gb(char * identifier, uint8_t channels, uint8_t * p_buf_in, FILE * FOUT) {

    sfx_data_t *sfx = (sfx_data_t *)p_buf_in;

    // TODO: names for these constants        
    uint8_t ch_mask = ((channels & 0xf0) ? 0x02 : 0) | ((channels & 0x0f) ? 0x08 : 0);

    fprintf(FOUT, "BANKREF(%s)\n"
                   "const uint8_t %s[] = {\n",identifier, identifier);

    // Initialize channels and old_pan as unset (-1, 0xff)
    int ch2_cache[2] = {-1, -1};
    int ch4_cache[2] = {-1, -1};
    uint8_t old_pan = 0xff;


    for (int i = 0; i < SFX_ENTRY_REC_COUNT; i++) {
            
        // Break out if record is unpopulated
        if (sfx->recs[i].duration == 0)
            break;

        int count = 0;
        // Write Placeholder value for Count and Delta into the file
        size_t f_addr_count = ftell(FOUT);
        fprintf(FOUT, "0x%02x", 0);  // Shim value to allocate file space, overwritten later

        if (opt.use_pan) {
            uint8_t current_pan = 0b01010101 | sfx->recs[i].ch2pan | sfx->recs[i].ch4pan;
            if (old_pan != current_pan) {
                count += 1;
                fprintf(FOUT, ",0b%s,0x%0x", char2strbin(0b01000100), current_pan);
                old_pan = current_pan;
            }
        }

        if (sfx->recs[i].ch2pan != 0) {
            count += 1;
            uint16_t freq = note_freqs_gb[ (sfx->recs[i].ch2note - 0x40) >> 1];
            make_frame_gb(FX_CHAN_1, sfx->recs[i].ch2duty, sfx->recs[i].ch2vol, freq & 0xff, ((freq >> 8) | 0x80) & 0xff, &ch2_cache, FOUT);
        }

        if (sfx->recs[i].ch4pan != 0) {
            count += 1;
            make_frame_gb(FX_CHAN_3, 0x2a, sfx->recs[i].ch4vol, sfx->recs[i].ch4freq, 0x80, &ch4_cache, FOUT);
        }
        fprintf(FOUT, ",\n");

        uint8_t delay = MAX(0, (((int)opt.delay * (int)(sfx->recs[i].duration)) - 1));
        uint8_t delta = MIN(15, delay);
        // Write resulting Count and Delta values (at start of current effect data)
        //
        // Save current file address
        size_t f_addr_current = ftell(FOUT);
        // Rewind to count entry and overwrite it
        fseek(FOUT, f_addr_count, SEEK_SET);
        fprintf(FOUT, "0x%02x,", ((delta & 0x0f) << 4) | count); 
        // Restore current file address
        fseek(FOUT, f_addr_current, SEEK_SET);

    
        delay -= delta;
        while (delay > 0) {
            delta = MIN(15, delay);
            fprintf(FOUT, "0x%02x,\n", (delta & 0x0f) << 4);
            delay -= delta;
        }
    }


    int count = 1;
    if (opt.cut_sound) {
        if (ch_mask & 2) count += 1;
        if (ch_mask & 8) count += 1;
    }
    if (opt.use_pan) count += 1;
    fprintf(FOUT, "0x%02x", count);    

    if (opt.cut_sound) {
        if (ch_mask & 2)
            fprintf(FOUT, ",0b%s,0x%02x,0x%02x", char2strbin(0b00101001), 0, 0xc0);

        if (ch_mask & 8)
            fprintf(FOUT, ",0b%s,0x%02x,0x%02x", char2strbin(0b00101011), 0, 0xc0);
    }

    if (opt.use_pan)
        fprintf(FOUT, ",0b%s,0x%02x", char2strbin(0b01000100), 0xff);

    fprintf(FOUT, ",0b%s\n};\n", char2strbin(7));    
    fprintf(FOUT, "void AT(0b%s) __mute_mask_%s;\n\n", char2strbin(ch_mask), identifier);
    
    return ch_mask;
}

