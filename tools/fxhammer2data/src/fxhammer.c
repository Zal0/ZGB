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
#include "fxhammer_psg.h"


// Useful reference from Valentina/Coffee Bat's repo:
// https://github.com/coffeevalenbat/CBT-FX/blob/a2d20b8e74226b30d14a3928245f96e7bd986cd1/hammer2cbt.py#L4C4-L21C43

options_t opt = {
        .infilename  = "",
        .outfilename = "",
        .identifier  = "",
        .cut_sound   = false,
        .use_pan     = true,
        .effectnum   = EFFECTNUM_DEFAULT,
        .system      = SYSTEM_DEFAULT,
        .bank        = BANK_DEFAULT,
        .delay       = DELAY_DEFAULT
    };

size_t vgm_data_start = 0;
FILE * file_out_c = NULL;
FILE * file_out_header = NULL;


static bool process_and_write_data(uint8_t * p_buf, size_t buf_len);


void fxhammer_cleanup(void) {

    if (file_out_c)
        fclose(file_out_c);
    if (file_out_header)
        fclose(file_out_header);
}


bool fxhammer_load(uint8_t * p_buf, size_t buf_len) {

    bool ret = false;

// TODO: force min file size to full 60 effects? Can it be expected/required?

    if (buf_len < FXHAMMER_FILE_MIN_SZ) {
        log_error("Error: save file is shorter than minimum %d bytes", FXHAMMER_FILE_MIN_SZ);
        exit(EXIT_FAILURE);
    }

    if (memcmp(p_buf + ADDR_SIG_START, "FX HAMMER", strlen("FX HAMMER")) != 0) {
        log_error("Error: Invalid FX HAMMER file format");
        exit(EXIT_FAILURE);
    }

    ret = process_and_write_data(p_buf, buf_len);

    return ret;
}



static bool process_and_write_data(uint8_t * p_buf, size_t buf_len) {

    uint8_t ch_mask = 0;
    file_out_c = fopen(opt.outfilename, "w");
    filename_replace_extension(opt.outfilename, ".h", MAX_STR_LEN);
    file_out_header = fopen(opt.outfilename, "w");

    if ((file_out_c) && (file_out_header)) {

        // Header file preamble
        fprintf(file_out_header,
            "#ifndef __%s_INCLUDE__\n"
            "#define __%s_INCLUDE__\n\n"
            "#include <gbdk/platform.h>\n"
            "#include <stdint.h>\n\n",
            opt.identifier, opt.identifier);

        // C file preamble
        fprintf(file_out_c,
            "#pragma bank %d\n\n"
            "#include <gbdk/platform.h>\n"
            "#include <stdint.h>\n\n", opt.bank);

        for (unsigned int effectnum = 0; effectnum <= EFFECTNUM_MAX; effectnum++) {
            if ((opt.effectnum == EFFECTNUM_USE_ALL) || (effectnum == opt.effectnum)) {

                uint8_t channels  = p_buf[ADDR_CHAN_USED + effectnum];
                size_t data_start = ADDR_SFX_DATA + (effectnum * SFX_DATA_SZ);
                size_t data_end   = data_start + SFX_DATA_SZ - 1;
                char identifier[MAX_STR_LEN];

                int res;
                if (opt.effectnum == EFFECTNUM_USE_ALL)
                    res = snprintf(identifier, sizeof(opt.identifier), "%s_%02x", opt.identifier, effectnum);
                else
                    res = snprintf(identifier, sizeof(opt.identifier), "%s", opt.identifier);

                if (res)
                    printf("Warning: truncated identifier to:%s\n",identifier);


                if (channels != 0) {
                    if (opt.system == SYSTEM_GB)
                        ch_mask = write_effect_data_gb(identifier, channels, &p_buf[data_start], file_out_c);
                    else // SYSTEM_PSG
                        ch_mask = write_effect_data_psg(identifier, channels, &p_buf[data_start], file_out_c);

                    // per-effect C header output
                    fprintf(file_out_header,
                        "#define MUTE_MASK_%s 0b%s\n"
                        "BANKREF_EXTERN(%s)\n"
                        "extern const uint8_t %s[];\n"
                        "extern void __mute_mask_%s;\n\n",
                        identifier, char2strbin(ch_mask),
                        identifier, identifier, identifier);
                }
            }
        }

        fprintf(file_out_header,"#endif\n");

        fclose(file_out_header);
        file_out_header = NULL;

        fclose(file_out_c);
        file_out_c = NULL;

        return true;
    }
    else {
        return false; // Failed
    }
}


