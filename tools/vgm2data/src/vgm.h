#ifndef _VGM_H
#define _VGM_H


#define VGM_MIN_HEADER_SZ 64

#define BANK_DEFAULT  255
#define DELAY_DEFAULT 1

#define CHANNELS_MIN 0
#define CHANNELS_MAX 5
#define CHANNELS_AR_SZ  (CHANNELS_MAX + 1) // +1 so array can be 1 based and indexed of channel num

#define CHIP_UNSUP 0
#define CHIP_GB    1
#define CHIP_PSG   2


// 0x50 PSG (SN76489/SN76496) write value dd
#define CMD_PSG          0x50

// 0x61 [nn] [nn] Wait n samples, n can range from 0 to 65535 (approx 1.49 seconds). Longer pauses than this are represented by multiple wait commands.
// 0x62           Wait 735 samples (60th of a second), a shortcut for 0x61 0xdf 0x02
// 0x63           Wait 882 samples (50th of a second), a shortcut for 0x61 0x72 0x03
#define CMD_WAIT_NN      0x61
#define CMD_WAIT_60TH    0x62
#define CMD_WAIT_50TH    0x63

// 0x66   end of sound data 
#define CMD_END_SND_DATA 0x66

// 0x7[n]   wait n+1 samples, n can range from 0 to 15. 
#define CMD_WAIT_N_MIN   0x70
#define CMD_WAIT_N_MAX   0x7F

// 0xB3 [aa] [dd]  GameBoy DMG, write value dd to register aa
//                 Note: Register 00 equals GameBoy address FF10.
#define CMD_DMG_REG      0xB3


#pragma pack(push, 1)           // Ensure no padding between struct members

typedef struct {
    char ident[4];              // "Vgm "
    uint32_t eof_offset;        // End of file offset
    uint32_t version;           // Version number
    uint32_t sn76489_clock;     // SN76489 clock speed  // PSG

    uint32_t ym2413_clock;      // YM2413 clock speed
    uint32_t gd3_offset;        // GD3 offset
    uint32_t total_samples;     // Total number of samples
    uint32_t loop_offset;       // Loop offset

    uint32_t loop_samples;      // Loop number of samples
    uint32_t rate;              // Rate
    uint16_t sn_fb;             // SN FB
    uint8_t snw;                // SNW
    uint8_t sf;                 // SF
    uint32_t ym2612_clock;      // YM2612 clock speed

    uint32_t ym2151_clock;      // YM2151 clock speed
    uint32_t vgm_data_offset;   // VGM data offset
    uint32_t sega_pcm_clock;    // Sega PCM clock speed
    uint32_t spcm_interface;    // SPCM Interface

    uint32_t rf5c68_clock;      // RF5C68 clock speed
    uint32_t ym2203_clock;      // YM2203 clock speed
    uint32_t ym2608_clock;      // YM2608 clock speed
    uint32_t ym2610_clock;      // YM2610 clock speed

    uint32_t ym3812_clock;      // YM3812 clock speed
    uint32_t ym3526_clock;      // YM3526 clock speed
    uint32_t y8950_clock;       // Y8950 clock speed
    uint32_t ymf262_clock;      // YMF262 clock speed

    uint32_t ymf278b_clock;     // YMF278B clock speed
    uint32_t ymf271_clock;      // YMF271 clock speed
    uint32_t ymz280b_clock;     // YMZ280B clock speed
    uint32_t rf5c164_clock;     // RF5C164 clock speed

    uint32_t pwm_clock;         // PWM clock speed
    uint32_t ay8910_clock;      // AY8910 clock speed
    uint32_t ay_ayt_flags;      // AY, AYT Flags Flags
    uint32_t vm_x_lb_lm;        // VM, ***, LB,LM

    // 0x80
    uint32_t gb_dmg_clock;      // GB DMG clock speed
    uint32_t nes_apu_clock;     // NES APU clock speed
    // 0x88 .. 0xFF
    uint8_t remaining_bytes[(0xFF - 0x88) + 1];

} vgm_header_t;

#pragma pack(pop)               // Restore original packing


typedef struct options_t {
    char infilename[MAX_STR_LEN];
    char outfilename[MAX_STR_LEN];
    char identifier[MAX_STR_LEN];

    bool channel_enabled[CHANNELS_AR_SZ];
    bool wave_enabled;
    bool init_enabled;

    unsigned int bank;
    unsigned int delay;
} options_t;

extern options_t vgm_opt;


void vgm_cleanup(void);
bool vgm_load(uint8_t * buf, size_t buf_len);


#endif // _VGM_H
