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
#include "fxhammer_psg.h"


static uint8_t make_frame_psg(uint8_t channel, uint8_t b, uint16_t c, int (*cache)[2], FILE * FOUT);


const uint16_t note_freqs_psg[] = {
    0x0357, 0x0327, 0x02FA, 0x02CF, 0x02A8, 0x0280, 0x025C, 0x023B, 0x021B, 0x01FC, 0x01E0, 0x01C5,
    0x01AC, 0x0193, 0x017D, 0x0167, 0x0153, 0x0140, 0x012E, 0x011D, 0x010D, 0x00FE, 0x00F0, 0x00E3,
    0x00D6, 0x00CA, 0x00BE, 0x00B4, 0x00AA, 0x00A0, 0x0097, 0x008F, 0x0087, 0x007F, 0x0078, 0x0071,
    0x006B, 0x0065, 0x005F, 0x005A, 0x0055, 0x0050, 0x004C, 0x0047, 0x0043, 0x0040, 0x003C, 0x0039,
    0x0035, 0x0032, 0x0030, 0x002D, 0x002A, 0x0028, 0x0026, 0x0024, 0x0022, 0x0020, 0x001E, 0x001C,
    0x001B, 0x0019, 0x0018, 0x0016, 0x0015, 0x0014, 0x0013, 0x0012, 0x0011, 0x0010, 0x000F, 0x000E,
};



static uint8_t make_frame_psg(uint8_t channel, uint8_t b, uint16_t c, int (*cache)[2], FILE * FOUT) {

    int count = 0;
    if (b != (*cache)[0]) {
        fprintf(FOUT, "0x%02x", 0b10010000 | ((channel & 0x03) << 5) | (b >> 4) );
        (*cache)[0] = b;
        count += 1;
    }
    if (channel == FX_CHAN_1) {
        if (c != (*cache)[1]) {
            fprintf(FOUT, "%s0x%02x,0x%02x", (count > 0) ? "," : "", 0b10000000 | ((channel & 0x03) << 5) | (c & 0x0f), (c >> 4) & 0x7f );
            (*cache)[1] = c;
            count += 2;
        }
    } else if (channel == FX_CHAN_3) {
        if (c != (*cache)[1]) {
            fprintf(FOUT, "%s0x%02x", (count > 0) ? "," : "", 0b10000100 | ((channel & 0x03) << 5) | ((c & 0xC0) >> 6) );
            (*cache)[1] = c;
            count += 1;
        }
    }

    return count;
}


uint8_t write_effect_data_psg(char * identifier, uint8_t channels, uint8_t * p_buf_in, FILE * FOUT) {

    sfx_data_t *sfx = (sfx_data_t *)p_buf_in;

    // TODO: names for these constants
    uint8_t ch_mask = ((channels & 0xf0) ? 0x02 : 0) | ((channels & 0x0f) ? 0x08 : 0);

    fprintf(FOUT, "BANKREF(%s)\n"
                   "const uint8_t %s[] = {\n",identifier, identifier);

    // Initialize channels and old_pan as unset (-1, 0xff)
    int ch2_cache[2] = {CACHE_UNSET, CACHE_UNSET};
    int ch4_cache[2] = {CACHE_UNSET, CACHE_UNSET};

    for (int i = 0; i < SFX_ENTRY_REC_COUNT; i++) {

        // Break out if record is unpopulated
        if (sfx->recs[i].duration == 0)
            break;

        int count = 0;
        // Write Placeholder value for Count into the file
        size_t f_addr_count = ftell(FOUT);
        fprintf(FOUT, "0x%02x,", 0);  // Shim value to allocate file space, overwritten later

        if (sfx->recs[i].ch2pan != 0) {
            uint16_t freq = note_freqs_psg[ (sfx->recs[i].ch2note - 0x40) >> 1];
            int cnt = make_frame_psg(FX_CHAN_1, sfx->recs[i].ch2vol, freq, &ch2_cache, FOUT);
            count += cnt;
            if (cnt > 0) fprintf(FOUT, ",");
        }

        if (sfx->recs[i].ch4pan != 0) {
            int cnt = make_frame_psg(FX_CHAN_3, sfx->recs[i].ch4vol, sfx->recs[i].ch4freq, &ch4_cache, FOUT);
            count += cnt;
            if (cnt > 0) fprintf(FOUT, ",");
        }

        fprintf(FOUT, "\n");

        uint8_t delay = MAX(0, (((int)opt.delay * (int)(sfx->recs[i].duration)) - 1));
        uint8_t delta = MIN(15, delay);
        // Write resulting Count values (at start of current effect data)
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


    int count = 0;
    if (opt.cut_sound) {
        if (ch_mask & 2) count += 1;
        if (ch_mask & 8) count += 1;
    }

    if (count != 0) {
        fprintf(FOUT, "0x%02x,", count);
    }

    if (opt.cut_sound) {
        if (ch_mask & 2)
            fprintf(FOUT, "0x%02x,", 0b10111111);

        if (ch_mask & 8)
            fprintf(FOUT, "0x%02x,", 0b11111111);
    }

    if (count != 0) {
        fprintf(FOUT, "\n");
    }

    fprintf(FOUT, "0x00\n};\n");
    fprintf(FOUT, "void AT(0b%s) __mute_mask_%s;\n\n", char2strbin(ch_mask), identifier);

    return ch_mask;
}

