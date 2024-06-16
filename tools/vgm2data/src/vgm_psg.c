// See LICENSE  file for license details

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
// #include <stddef.h>
// #include <unistd.h>
#include <stdbool.h>
#include <stdint.h>

#include "common.h"
#include "logging.h"

#include "vgm.h"
#include "vgm_gb.h"


#define      ROW_BUF_SZ 15
#define      ROW_MAX    (ROW_BUF_SZ - 1)
uint8_t      row_data[ROW_BUF_SZ];
unsigned int row_count; 

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


static void row_data_push(uint8_t value) {

    if (row_count < ROW_BUF_SZ) {
        row_data[row_count] = value;
        row_count++;
    } else {
        log_error("Error: Too many PSG writes per frame\n");
        exit(EXIT_FAILURE);        
    }
}


static void row_data_reset(void) {
    row_count = 0;
}


static void log_packet_verbose(uint8_t cmd) {

    log_verbose("PACKET (reason: 0x%02x): [", cmd);

    for (unsigned int row = 0; row < row_count; row++) {
        log_verbose("%d", row_data[row]);
        if (row != (row_count - 1)) log_verbose(", ");
    }
    log_verbose("]\n");
}


const char * const ch_names[4] = {"PSG_CH0", "PSG_CH1", "PSG_CH2", "PSG_CH3"};

uint8_t vgm_process_psg_sound_data(uint8_t * p_buf_in, size_t buf_len_in, FILE * FOUT) {

    // Set global vars for reading source data
    p_buf   = p_buf_in;
    buf_len = buf_len_in;

    uint8_t   cmd;
    uint8_t   channel_mute_mask = 0;
    int       count = 0;
    uint8_t   last_channel = 0;

    row_data_reset();

    // Process all data
    while(buf_len) {

        cmd = get_byte();

        if (cmd == CMD_PSG) {
            row_data_push(get_byte());
            count = 0;

        } else if ((cmd == CMD_END_SND_DATA) ||
                 (IN_RANGE(CMD_WAIT_NN, CMD_WAIT_50TH, cmd)) ||
                 (IN_RANGE(CMD_WAIT_N_MIN, CMD_WAIT_N_MAX, cmd))) {

            if (cmd == CMD_WAIT_NN) {
                get_byte(); // Discard wait byte info
                get_byte();
            }

            count += 1;

            uint8_t look_ahead = *p_buf;

            if ((cmd == CMD_END_SND_DATA) ||
                (!(IN_RANGE(CMD_WAIT_NN, CMD_WAIT_50TH, look_ahead))) ||
                  (IN_RANGE(CMD_WAIT_N_MIN, CMD_WAIT_N_MAX, look_ahead))) {

                // output packet
                log_packet_verbose(cmd);

                // delay + packet length
                count = (MIN(15, ((count - 1) * MAX(1, vgm_opt.delay))) << 4) | (row_count & 0x0f);

                // Write Placeholder value for Count into the file
                fprintf(FOUT, "0x%02x,", count);

                // output result
                for (unsigned int row = 0; row < row_count; row++) {
                    uint8_t command = row_data[row];
                    if (command & 0b10000000) {
                        last_channel = ((command & 0b01100000) >> 5) & 3;
                        if (vgm_opt.channel_enabled[last_channel]) {
                            channel_mute_mask |= (1 << last_channel);
                            fprintf(FOUT, "PSG_LATCH|%s|", ch_names[last_channel]);
                            if (command & 0b00010000) fprintf(FOUT, "PSG_VOLUME|");
                            fprintf(FOUT, "0x%02x,", command & 0x00001111);
                        }
                    } else {
                        if (vgm_opt.channel_enabled[last_channel]) fprintf(FOUT, "0x%02x,", command);
                    }
                }

                fprintf(FOUT, "\n");

                // reset row
                row_count = 0;

                if (cmd == CMD_END_SND_DATA) {
                    // write terminate sequence and exit
                    fprintf(FOUT, "0x00\n};\n");
                    break;
                }
            }
        }
        else {
            log_error("Error: unsupported command %02x\n", cmd);
            exit(EXIT_FAILURE);
        }
    }

    return channel_mute_mask;
}

