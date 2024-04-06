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

#include "vgm.h"
#include "vgm_gb.h"


// String names for registers, 0 index = NR10 (0x10)
const char * regstr[] = {
    "NR10_REG", "NR11_REG", "NR12_REG", "NR13_REG", "NR14_REG",
    "NR20_REG", "NR21_REG", "NR22_REG", "NR23_REG", "NR24_REG",   
    "NR30_REG", "NR31_REG", "NR32_REG", "NR33_REG", "NR34_REG",
    "NR40_REG", "NR41_REG", "NR42_REG", "NR43_REG", "NR44_REG",
    "NR50_REG", "NR51_REG", "NR52_REG",
    // 0x27 - 0x2F
    "", "", "", "", "", "", "", "", "",
    // String names for wave data, 0 index = PCM_SAMPLE (0x30)
    "PCM[0]", "PCM[1]", "PCM[2]", "PCM[3]",
    "PCM[4]", "PCM[5]", "PCM[6]", "PCM[7]",
    "PCM[8]", "PCM[9]", "PCM[A]", "PCM[B]",
    "PCM[C]", "PCM[D]", "PCM[E]", "PCM[F]"
};


// Register entry within a channel
typedef struct reg_t {
    uint8_t data;
    bool    has_data;
} reg_t;

// Channels have multiple registers
typedef struct channel_t {
    reg_t regs[CH_REGS_SZ];
    bool    has_data;
} channel_t;


static channel_t gb_audio[CHANNELS_SZ];

static uint8_t * p_buf;
static size_t buf_len;


static uint8_t get_byte(void) {

    if (buf_len > 0) {
        buf_len--;
        return *p_buf++;
    } else {
        log_error("Error: unexpected end of data\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}


// Setting a given register per channel will flag that channel as having data
// in addition to the register itself
static void channel_reg_set(int channel, int reg, uint8_t value) {

    if ((channel < CHANNELS_SZ) && (reg < CH_REGS_SZ)) {
        gb_audio[channel].has_data = true;
        gb_audio[channel].regs[reg].data = value;
        gb_audio[channel].regs[reg].has_data = true;
    } else
        log_standard("Warning: tried to set channel %d row %d that doesn't exist to %d\n", channel, reg, value);
}


static void channel_reset_and_clear(int channel) {

    gb_audio[channel].has_data = false;
    for (int reg = 0; reg < CH_REGS_SZ; reg++) {
        gb_audio[channel].regs[reg].has_data = false;
        gb_audio[channel].regs[reg].data = 0;
    }
}


static void channels_reset_all(void) {

    for (int channel = 0; channel < CHANNELS_SZ; channel++) {
        channel_reset_and_clear(channel);
    }
}


static void log_packet_verbose(uint8_t cmd) {

    log_verbose("PACKET (reason: 0x%02x): {\n", cmd);

    for (int channel = 0; channel < CH_REGS_SZ; channel++) {
        if (gb_audio[channel].has_data) {

            int regcount = 0;
            log_verbose("%d: {", channel);
            for (int reg = 0; reg < CH_REGS_SZ; reg++) {
                if (gb_audio[channel].regs[reg].has_data) {
                    if (regcount++ > 0) log_verbose(", ");
                    log_verbose("%d: %d", reg, gb_audio[channel].regs[reg].data);
                }
            }
            log_verbose("},\n");
        }
    }
    log_verbose("}\n");
}


uint8_t vgm_process_gb_sound_data(uint8_t * p_buf_in, size_t buf_len_in, FILE * FOUT) {

    // Set global vars for reading source data
    p_buf   = p_buf_in;
    buf_len = buf_len_in;

    uint8_t   cmd, addr, data;
    uint8_t   channel_mute_mask = 0;
    int       count = 0;

    channels_reset_all();

    // Process all data
    while(buf_len) {

        cmd = get_byte();

        if (cmd == CMD_DMG_REG) {

            addr = get_byte() + NR10_REG;
            data = get_byte();

            if ((addr - NR10_REG) < ARRAY_LEN(regstr))
                log_verbose("%s = 0b%s (0x%02x)\n", regstr[addr - NR10_REG], char2strbin(data), data);

            // Example:
            //              (N + NR10_REG_addr)
            // CMD_DMG_REG, NR52_REG         <value>
            // 0xB3,        0x16,            0x00
            // 0xB3,        0x16,            0x80

            if (IN_RANGE(NR10_REG, NR14_REG, addr)) {
                channel_reg_set(NR1x, addr - NR10_REG, data);
            } else if (IN_RANGE(NR21_REG, NR24_REG, addr)) {
                channel_reg_set(NR2x, addr - NR20_REG, data);
            } else if (IN_RANGE(NR30_REG, NR34_REG, addr)) {
                channel_reg_set(NR3x, addr - NR30_REG, data);
            } else if (IN_RANGE(NR41_REG, NR44_REG, addr)) {
                channel_reg_set(NR4x, addr - NR40_REG, data);
            } else if (IN_RANGE(NR50_REG, NR52_REG, addr)) {
                channel_reg_set(NR5x, addr - NR50_REG, data);
            } else if (IN_RANGE(PCM_SAMPLE, PCM_SAMPLE + PCM_LENGTH, addr)) {
                channel_reg_set(PCMDATA, addr - PCM_SAMPLE, data);
            } else {
                log_error("ERROR: Invalid register address: %02x\n", addr);
                exit(EXIT_FAILURE);
            }
        }
        else if ((cmd == CMD_END_SND_DATA) ||
                 (IN_RANGE(CMD_WAIT_NN, CMD_WAIT_50TH, cmd)) ||
                 (IN_RANGE(CMD_WAIT_N_MIN, CMD_WAIT_N_MAX, cmd))) {

            if (cmd == CMD_WAIT_NN) {
                get_byte(); // Discard wait byte info
                get_byte();
            }

            log_packet_verbose(cmd);

            // Write Placeholder value for Count into the file
            size_t f_addr_count = ftell(FOUT);
            fprintf(FOUT, "0x%02x,", 0);  // Shim value to allocate file space, overwritten later

            count = 0;

            // NR5x registers
            if (gb_audio[NR5x].has_data && (vgm_opt.channel_enabled[NR5x])) {

                // NR52
                // Note: NR52 is a special case. It must be written first or else other NR5x writes get lost since APU may be disabled 
                if ((gb_audio[NR5x].regs[NR52].has_data) && (vgm_opt.init_enabled)) {
                    count += 1;
                    fprintf(FOUT, "0b%s,0x%02x,", char2strbin(0b00100100), gb_audio[NR5x].regs[NR52].data);
                }

                // NR50, NR51
                // Only process if at least one of the two registers has data
                if (gb_audio[NR5x].regs[NR50].has_data || gb_audio[NR5x].regs[NR51].has_data) {
                    count += 1;
                    uint8_t mask = NR5x |
                           ((gb_audio[NR5x].regs[NR50].has_data) ? 0x80 : 0) |
                           ((gb_audio[NR5x].regs[NR51].has_data) ? 0x40 : 0);
                    fprintf(FOUT, "0b%s,", char2strbin(mask));

                    if (gb_audio[NR5x].regs[NR50].has_data)
                        fprintf(FOUT, "0x%02x,", gb_audio[NR5x].regs[NR50].data); 

                    if (gb_audio[NR5x].regs[NR51].has_data)
                        fprintf(FOUT, "0x%02x,", gb_audio[NR5x].regs[NR51].data); 
                }
            }
            channel_reset_and_clear(NR5x);


            // AUD3WAVE[]
            if (gb_audio[PCMDATA].has_data && (vgm_opt.wave_enabled)) {
                uint8_t mask = PCMDATA;
                fprintf(FOUT, "0b%s,", char2strbin(mask));

                for (int reg = 0; reg < PCMDATA_SZ; reg++) {
                    uint8_t val = (gb_audio[PCMDATA].regs[reg].has_data) ? gb_audio[PCMDATA].regs[reg].data : 0;
                    fprintf(FOUT, "0x%02x,", val); 
                }
                count += 1;
            }
            channel_reset_and_clear(PCMDATA);


            // NR1x, NR2x, NR3x, NR4x regs
            // For these NRx4 must be written last since it has the trigger flag
            for (int chan = NR1x; chan <= NR4x; chan++) {
                if ((gb_audio[chan].has_data) && (vgm_opt.channel_enabled[chan])) {
                    uint8_t mask = chan |
                           ((gb_audio[chan].regs[0].has_data) ? 0x80 : 0) |
                           ((gb_audio[chan].regs[1].has_data) ? 0x40 : 0) |
                           ((gb_audio[chan].regs[2].has_data) ? 0x20 : 0) |
                           ((gb_audio[chan].regs[3].has_data) ? 0x10 : 0) |
                           ((gb_audio[chan].regs[4].has_data) ? 0x08 : 0);

                    if ((mask != chan) && ((mask & 0x08u) != 0)) {
                        fprintf(FOUT, "0b%s,", char2strbin(mask));

                        for (int reg = 0; reg < NR1234_SZ; reg++) {
                            if (gb_audio[chan].regs[reg].has_data) {
                                fprintf(FOUT, "0x%02x,", gb_audio[chan].regs[reg].data); 
                            }
                        }
                        count += 1;
                        channel_mute_mask |= (1 << chan);
                    }
                }
                channel_reset_and_clear(chan);
            }

            // optional delay
            count |= MAX(0, ((int)vgm_opt.delay - 1)) << 4;

            // Write resulting Count value (at start of current command data)
            //
            // Save current file address
            size_t f_addr_current = ftell(FOUT);
            // Rewind to count entry and overwrite it
            fseek(FOUT, f_addr_count, SEEK_SET);
            fprintf(FOUT, "0x%02x,", count); 
            // Restore current file address
            fseek(FOUT, f_addr_current, SEEK_SET);

            // TODO: Debug line breaks between packets out 
            // fprintf(FOUT, "\n\n"); 

            // reset all channels
            channels_reset_all();

            if (cmd == CMD_END_SND_DATA) {
                // write terminate sequence and exit
                fprintf(FOUT, "0x01,0b%s\n};\n", char2strbin(7));
                break;
            }
        }
        else {
            log_error("Error: unsupported command %02x\n", cmd);
            exit(EXIT_FAILURE);
        }
    }

    return channel_mute_mask;
}

