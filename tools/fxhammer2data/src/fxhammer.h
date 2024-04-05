#ifndef _FXHAMMER_H
#define _FXHAMMER_H


#define BANK_DEFAULT  255

#define EFFECTNUM_USE_ALL 999
#define EFFECTNUM_MAX     59
#define EFFECTNUM_DEFAULT (EFFECTNUM_USE_ALL)

#define DELAY_DEFAULT     1

#define FX_CHAN_1   1
#define FX_CHAN_3   3

#define CACHE_UNSET -1


/*
FX HAMMER DATA LOCATIONS:

Header:
SFX Priority:   0x200 + fxnum
CH Used:        0x300 + fxnum (0x30 = CH2 used, 0x03 = CH4 used)

SFX data:       0x400 + (fxnum * 256)
- Frame duration    
- CH2 pan       NR51 format
- CH2 vol       NR22 format
- CH2 duty      NR21 format
- CH2 note      Note = (this - 0x40) / 2
- CH4 pan       NR51 format
- CH4 vol       NR42 format
- CH4 freq      NR43 format

WARNING: FX Hammer pan values are inverted
*/

#define SYSTEM_UNSUP    0
#define SYSTEM_GB       1
#define SYSTEM_PSG      2
#define SYSTEM_DEFAULT  (SYSTEM_GB)


#define ADDR_PRIORITY     0x200
#define ADDR_CHAN_USED    0x300
#define ADDR_SFX_DATA     0x400

#define ADDR_SIG_START    0x09
#define ADDR_SIG_END      0x12

#define SFX_DATA_SZ       0x100 // Size for each SFX entry
#define FXHAMMER_FILE_MIN_SZ   (ADDR_SFX_DATA + SFX_DATA_SZ)

// TODO: Require 60 as min?
// #define SFX_COUNT_REQ          (EFFECTNUM_MAX + 1)
// #define FXHAMMER_FILE_MIN_SZ   (ADDR_SFX_DATA + (SFX_DATA_SZ * SFX_COUNT_REQ))


// Up to 32 records per sfx entry (some may be unpopulated)
#define SFX_ENTRY_REC_COUNT 32

// Expect up to 
typedef struct sfx_data_rec_t {
    uint8_t duration;
    uint8_t ch2pan;
    uint8_t ch2vol;
    uint8_t ch2duty;
    uint8_t ch2note;
    uint8_t ch4pan;
    uint8_t ch4vol;
    uint8_t ch4freq;
} sfx_data_rec_t;

typedef struct sfx_data_t {
    sfx_data_rec_t recs[SFX_ENTRY_REC_COUNT];
} sfx_data_t;

typedef struct options_t {
    char infilename[MAX_STR_LEN];
    char outfilename[MAX_STR_LEN];
    char identifier[MAX_STR_LEN];

    bool cut_sound;
    bool use_pan;
    unsigned int effectnum;
    unsigned int system;

    unsigned int bank;
    unsigned int delay;
} options_t;

extern options_t opt;


void fxhammer_cleanup(void);
bool fxhammer_load(uint8_t * buf, size_t buf_len);


#endif // _FXHAMMER_H
