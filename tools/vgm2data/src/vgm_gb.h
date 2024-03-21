#ifndef _VGM_GB_H
#define _VGM_GB_H


// NR1x
#define NR10_REG 0x10
#define NR11_REG 0x11 
#define NR12_REG 0x12 
#define NR13_REG 0x13 
#define NR14_REG 0x14 

// NR2x
#define NR20_REG 0x15
#define NR21_REG 0x16
#define NR22_REG 0x17
#define NR23_REG 0x18
#define NR24_REG 0x19 

// NR3x
#define NR30_REG 0x1A
#define NR31_REG 0x1B
#define NR32_REG 0x1C
#define NR33_REG 0x1D
#define NR34_REG 0x1E

// NR4x
#define NR40_REG 0x1F
#define NR41_REG 0x20
#define NR42_REG 0x21
#define NR43_REG 0x22
#define NR44_REG 0x23

// RE5x
#define NR50_REG 0x24
#define NR51_REG 0x25
#define NR52_REG 0x26

#define PCM_SAMPLE 0x30 // PCM wave pattern
#define PCM_LENGTH 0x10 // PCM wave pattern


// NR1x index entries
#define NR10 0
#define NR11 1 
#define NR12 2 
#define NR13 3 
#define NR14 4 

// NR2x
#define NR20 0
#define NR21 1
#define NR22 2
#define NR23 3
#define NR24 4 

// NR3x
#define NR30 0
#define NR31 1
#define NR32 2
#define NR33 3
#define NR34 4

// NR4x
#define NR40 0
#define NR41 1
#define NR42 2
#define NR43 3
#define NR44 4

// NR5x
#define NR50 0
#define NR51 1
#define NR52 2


// Indexes into data struct
#define NR1x    0
#define NR2x    1
#define NR3x    2
#define NR4x    3
#define NR5x    4
#define PCMDATA 5

#define NR1234_SZ  5  // 5  registers per channel
#define NR5_SZ     3  // 3  registers per channel
#define PCMDATA_SZ 16 // 16 registers per channel

#define CHANNELS_SZ ((PCMDATA - NR1x) + 1)
#define CH_REGS_SZ (16) // Set channel reg array size to fit max num per channel, 16, wave channel data


uint8_t vgm_process_gb_sound_data(uint8_t * p_buf_in, size_t buf_len_in, FILE * FOUT);


#endif // _VGM_GB_H
