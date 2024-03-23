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

#include "vgm.h"
#include "vgm_gb.h"
#include "vgm_psg.h"

// https://vgmrips.net/wiki/VGM_Specification

options_t vgm_opt = {
        .infilename = "",
        .outfilename = "",
        .identifier = "",
        .channel_enabled = {true, true, true, true, true, true},
        .wave_enabled = true,
        .init_enabled = true,
        .bank = BANK_DEFAULT,
        .delay = DELAY_DEFAULT
    };

unsigned int soundchip = CHIP_UNSUP;
unsigned int channel_mute_mask = 0;
size_t vgm_data_start = 0;
FILE * file_out = NULL;


static bool vgm_parse_header(uint8_t * p_buf, size_t buf_len);


void vgm_cleanup(void) {

    if (file_out)
        fclose(file_out);
}


static bool vgm_parse_header(uint8_t * p_buf, size_t buf_len) {

    // All header sizes are valid for all versions from 1.50 on, as long as header has at least 64 bytes. If the VGM data starts at an offset that is lower than 0x100, all overlapping header bytes have to be handled as they were zero.
    //
    // Min header required is amount large enough to contain gb clock field
    const size_t min_header_sz_gb  = FIELD_SIZEOF(vgm_header_t, gb_dmg_clock) + offsetof(vgm_header_t, gb_dmg_clock);
    const size_t min_header_sz_psg = FIELD_SIZEOF(vgm_header_t, sn76489_clock) + offsetof(vgm_header_t, sn76489_clock);

    if (buf_len < VGM_MIN_HEADER_SZ) {
        log_error("File size too small to contain minimum header (64 bytes, current file size is %d bytes)\n", buf_len);
        return false;
    }

    const vgm_header_t * vgm_hdr = (vgm_header_t *)p_buf;

    if ((vgm_hdr->ident[0] != 'V') ||
        (vgm_hdr->ident[1] != 'g') ||
        (vgm_hdr->ident[2] != 'm') ||
        (vgm_hdr->ident[3] != ' ')) {
        log_error("Invalid file format\n");
        return false;
    }

    if (vgm_hdr->version < 0x161) {
        log_error("VGM version too low: %04x\n", vgm_hdr->version);
        return false;
    } else
        log_verbose("VGM file version: %04x\n", vgm_hdr->version);           

    // Ensure file was large enough to contain the gb header
    if (buf_len >= min_header_sz_gb) {
        if (vgm_hdr->gb_dmg_clock != 0) {
            soundchip = CHIP_GB;
            log_verbose("Game Boy data detected\n");
        }
    }
    // Only check for PSG if GB is not detected
    if (soundchip != CHIP_GB) {
        // Ensure file was large enough to contain the gb header
        if (buf_len >= min_header_sz_psg) {
            if (vgm_hdr->sn76489_clock != 0) {
                soundchip = CHIP_PSG;
                log_verbose("PSG data detected\n");
            }
        }
    }

    if (soundchip == CHIP_UNSUP) {
        log_error("Error: VGM must contain GameBoy or PSG data\n");
        return false;
    }

    // "All pointer offsets are written as relative to the current position in the file, 
    // so for example the GD3 offset at 0x14 in the header is the file position of the GD3 tag minus 0x14."
    vgm_data_start = vgm_hdr->vgm_data_offset + offsetof(vgm_header_t, vgm_data_offset);
    log_standard("data start position: %d\n", vgm_data_start);

    if (buf_len < (vgm_data_start + 1)) {
        log_error("File size %d bytes too small, missing expected VGM Data at 0x%x\n", buf_len, vgm_data_start);
        return false;
    }

    return true;
}


static bool vgm_process_gameboy(uint8_t * p_buf, size_t buf_len) {

    file_out = fopen(vgm_opt.outfilename, "w");

    if (file_out) {

        fprintf(file_out, "#pragma bank %d\n\n"
                          "#include <gbdk/platform.h>\n"
                          "#include <stdint.h>\n\n"
                          "BANKREF(%s)\n"
                          "const uint8_t %s[] = {\n",
                          vgm_opt.bank, vgm_opt.identifier, vgm_opt.identifier);
    
        channel_mute_mask = vgm_process_gb_sound_data(p_buf, buf_len, file_out);
        fprintf(file_out, "void AT(0b%s) __mute_mask_%s;\n", char2strbin(channel_mute_mask), vgm_opt.identifier);

        fclose(file_out);
        file_out = NULL;
        return true;
    }

    return false; // Failed
}


static bool vgm_process_psg(uint8_t * p_buf, size_t buf_len) {

    file_out = fopen(vgm_opt.outfilename, "w");

    if (file_out) {
        fprintf(file_out, "#pragma bank %d\n\n"
                          "#include <gbdk/platform.h>\n"
                          "#include <stdint.h>\n\n"
                          "BANKREF(%s)\n"
                          "const uint8_t %s[] = {\n",
                          vgm_opt.bank, vgm_opt.identifier, vgm_opt.identifier);
    
        channel_mute_mask = vgm_process_psg_sound_data(p_buf, buf_len, file_out);
        fprintf(file_out, "void AT(0b%s) __mute_mask_%s;\n", char2strbin(channel_mute_mask), vgm_opt.identifier);

        fclose(file_out);
        file_out = NULL;
        return true;
    }

    return false; // Failed
}


static bool vgm_write_c_header_file(void) {

    if (matches_extension(vgm_opt.outfilename, ".c")) {
    
        filename_replace_extension(vgm_opt.outfilename, ".h", MAX_STR_LEN);
        file_out = fopen(vgm_opt.outfilename, "w");

        if (file_out) {
            fprintf(file_out, "#ifndef __%s_INCLUDE__\n"
                              "#define __%s_INCLUDE__\n\n"
                              "#include <gbdk/platform.h>\n"
                              "#include <stdint.h>\n\n"
                              "#define MUTE_MASK_%s 0b%s\n\n"
                              "BANKREF_EXTERN(%s)\n"
                              "extern const uint8_t %s[];\n"
                              "extern void __mute_mask_%s;\n\n"
                              "#endif\n",
                              vgm_opt.identifier, vgm_opt.identifier,
                              vgm_opt.identifier, char2strbin(channel_mute_mask),
                              vgm_opt.identifier,
                              vgm_opt.identifier, vgm_opt.identifier);
            fclose(file_out);
            file_out = NULL;
            return true;
        }
        else {
            return false; // Failed
        }
    }
    return true;
}


bool vgm_load(uint8_t * p_buf, size_t buf_len) {

    bool ret = false;
    if (vgm_parse_header(p_buf, buf_len)) {

        if (soundchip == CHIP_GB)
            ret = vgm_process_gameboy(p_buf + vgm_data_start, buf_len - vgm_data_start);
        else
            ret = vgm_process_psg(p_buf + vgm_data_start, buf_len - vgm_data_start);

        if (ret)
            ret = vgm_write_c_header_file();

        return ret;
    } else
        return false;
}

