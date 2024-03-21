// See LICENSE  file for license details

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>


#include "common.h"
#include "files.h"
#include "path_ops.h"
#include "logging.h"

#include "vgm.h"
#include "vgm_gb.h"

// #define VERSION "version 1.0.0"

static void init(void);
static void cleanup(void);
static void display_help(void);
static int  handle_args(int argc, char * argv[]);
static bool increment_if_next_arg_available(int * i, int argc, char * opt_str);
static void warn_if_arg_looks_like_option(char * arg, char * opt_str, char * usage_str);

uint8_t * p_vgm_buf_in  = NULL;
size_t    vgm_buf_in_len = 0;


bool show_help_and_exit = false;



static void init(void) {
}


// Registered as an atexit handler
static void cleanup(void) {

    vgm_cleanup();

    if (p_vgm_buf_in != NULL) {
        free(p_vgm_buf_in);
        p_vgm_buf_in = NULL;
    }
}


int main( int argc, char *argv[] )  {

    int ret = EXIT_FAILURE; // Default to failure on exit

    // Register cleanup with exit handler
    atexit(cleanup);
    init();


    if (handle_args(argc, argv)) {

        if (show_help_and_exit) {
            ret = EXIT_SUCCESS;
        }
        else {
            p_vgm_buf_in = file_read_into_buffer(vgm_opt.infilename, &vgm_buf_in_len);

            if (p_vgm_buf_in) {
                if (vgm_load(p_vgm_buf_in, vgm_buf_in_len)) {
                    ret = EXIT_SUCCESS;
                }
            }
        }
    }

    #ifdef DRAG_AND_DROP_MODE
        // Wait for input to keep the console window open after processing
        log_standard("\n\nPress Any Key to Continue\n");
        getchar();
    #endif

    return ret;
}



static void display_help(void) {
    log_standard(
    "\n"
    "Usage: vgm2data.py [options] INPUT_FILE_NAME.VGM\n"
    "\n"
    "Options:\n"
    "  -h, --help            show this help message and exit\n"
    "  -o OUTFILENAME, --out=OUTFILENAME\n"
    "                        output file name\n"
    "  -i IDENTIFIER, --identifier=IDENTIFIER\n"
    "                        C source identifier\n"
    "  -1, --no-nr1x         disable channel 0\n"
    "  -2, --no-nr2x         disable channel 1\n"
    "  -3, --no-nr3x         disable channel 2\n"
    "  -4, --no-nr4x         disable channel 3\n"
    "  -5, --no-nr5x         disable NR5X manipulation\n"
    "  -s, --no-init         disable sound init\n"
    "  -w, --no-wave         disable waveform loading\n"
    "  -v, --verbose         verbose output\n"
    "  -d DELAY, --delay=DELAY\n"
    "                        delay size\n"
    "  -b BANK, --bank=BANK  BANK number (default AUTO=255)\n"
    "\n"
   );
}


// Check to see if the next argument is present, if so increment
// If not, set help flag and return error
static bool increment_if_next_arg_available(int * i, int argc, char * opt_str) {

    if (*i < (argc -1)) {
        *i += 1; // Move to next argument if one is available
        return true;
    }
    else {
        log_error("Error: %s specified but argument value is missing\n", opt_str);
        show_help_and_exit = true;
        return false; // Abort
    }
}


static void warn_if_arg_looks_like_option(char * arg, char * opt_str, char * usage_str) {

    if (*arg == '-')
        log_standard("Warning: %s specified but argument value (\"%s\") has dash and looks like an option argument. %s\n", opt_str, arg, usage_str);
}


static int handle_args(int argc, char * argv[]) {

    int i;

    if( argc < 2 ) {
        display_help();
        return false;
    }

    // Start at first optional argument, argc is zero based
    for (i = 1; i <= (argc -1); i++ ) {

    // log_standard("%d %s\n", i, argv[i]);

        if ((strstr(argv[i], "-h") == argv[i]) || (strstr(argv[i], "-?") == argv[i])) {
            display_help();
            show_help_and_exit = true;
            return true;  // Don't parse further input when -h is used

        } else if ((strstr(argv[i], "-1") == argv[i]) || (strstr(argv[i], "--no-nr1x") == argv[i])) {
            vgm_opt.channel_enabled[NR1x] = false;
        } else if ((strstr(argv[i], "-2") == argv[i]) || (strstr(argv[i], "--no-nr2x") == argv[i])) {
            vgm_opt.channel_enabled[NR2x] = false;
        } else if ((strstr(argv[i], "-3") == argv[i]) || (strstr(argv[i], "--no-nr3x") == argv[i])) {
            vgm_opt.channel_enabled[NR3x] = false;
        } else if ((strstr(argv[i], "-4") == argv[i]) || (strstr(argv[i], "--no-nr4x") == argv[i])) {
            vgm_opt.channel_enabled[NR4x] = false;
        } else if ((strstr(argv[i], "-5") == argv[i]) || (strstr(argv[i], "--no-nr5x") == argv[i])) {
            vgm_opt.channel_enabled[NR5x] = false;

        } else if ((strstr(argv[i], "-s") == argv[i]) || (strstr(argv[i], "--no-init") == argv[i])) {
            vgm_opt.init_enabled = false; // no_init = true;
        } else if ((strstr(argv[i], "-w") == argv[i]) || (strstr(argv[i], "--no-wave") == argv[i])) {
            vgm_opt.wave_enabled = false; // no_wave = true;
        } else if ((strstr(argv[i], "-v") == argv[i]) || (strstr(argv[i], "--verbose") == argv[i])) {
            log_set_level(OUTPUT_LEVEL_VERBOSE);


        // == Output Filename ==
        } else if (strstr(argv[i], "-o") == argv[i]) {
            if (! increment_if_next_arg_available(&i, argc, "-o")) return false;
            warn_if_arg_looks_like_option(argv[i], "-o", "Usage: -o filename");
            snprintf(vgm_opt.outfilename, sizeof(vgm_opt.outfilename), "%s", argv[i]);

        } else if (strstr(argv[i], "--out=") == argv[i]) {
            snprintf(vgm_opt.outfilename, sizeof(vgm_opt.outfilename), "%s", argv[i] + strlen("--out="));

        // == Output Identifier ==
        } else if (strstr(argv[i], "-i") == argv[i]) {
            if (! increment_if_next_arg_available(&i, argc, "-i")) return false;
            warn_if_arg_looks_like_option(argv[i], "-i", "Usage: -i identifier");
            snprintf(vgm_opt.identifier, sizeof(vgm_opt.identifier), "%s", argv[i]);

        } else if (strstr(argv[i], "--identifier=") == argv[i]) {
            snprintf(vgm_opt.identifier, sizeof(vgm_opt.identifier), "%s", argv[i] + strlen("--identifier="));

        // == Bank ==
        } else if (strstr(argv[i], "-b") == argv[i]) {
            if (! increment_if_next_arg_available(&i, argc, "-b")) return false;
            warn_if_arg_looks_like_option(argv[i], "-b", "Usage: -b bank number");
            vgm_opt.bank = strtol(argv[i], NULL, 10);

        } else if (strstr(argv[i], "--bank=") == argv[i]) {
            vgm_opt.bank = strtol(argv[i] + strlen("--bank="), NULL, 10);

        // == Delay ==
        } else if (strstr(argv[i], "-b") == argv[i]) {
            if (! increment_if_next_arg_available(&i, argc, "-d")) return false;
            warn_if_arg_looks_like_option(argv[i], "-d", "Usage: -d delay");
            vgm_opt.delay = strtol(argv[i], NULL, 10);

        } else if (strstr(argv[i], "--delay=") == argv[i]) {
            vgm_opt.delay = strtol(argv[i] + strlen("--delay="), NULL, 10);


        } else if (argv[i][0] == '-') {
            log_error("Unknown argument: %s\n\n", argv[i]);
            display_help();
            return false;

        } else {
            // Set input filename if no dash present
            snprintf(vgm_opt.infilename, sizeof(vgm_opt.infilename), "%s", argv[i]);
        }
    }

    // == Validation and reporting ==

    // If no output filename specified then base it on input filename 
    if (vgm_opt.infilename[0] == '\0') {
        log_error("Error: Input file name required\n");
        display_help();
        show_help_and_exit = true;
        return false; // Abort
    }

    // Validate bank number
    if ((vgm_opt.bank < BANK_NUM_MIN) || (vgm_opt.bank > BANK_NUM_MAX)) {
        log_error("Error: Invalid bank number specified with --bank=%d\n", vgm_opt.bank);
        display_help();
        show_help_and_exit = true;
        return false; // Abort
    }

    // If no output filename specified then base it on input filename 
    if (vgm_opt.outfilename[0] == '\0') {
        strcpy(vgm_opt.outfilename, vgm_opt.infilename);
        filename_remove_extension(vgm_opt.outfilename);
        strcat(vgm_opt.outfilename, ".c");
    }
    
    // If no identifier specified then base it on input filename 
    if (vgm_opt.identifier[0] == '\0') {
        strcpy(vgm_opt.identifier, get_filename_from_path(vgm_opt.infilename));
        filename_remove_extension(vgm_opt.identifier);
    }

    for (int c = CHANNELS_MIN; c <= CHANNELS_MAX; c++) {
        if (vgm_opt.channel_enabled[c] == false) {
            if (c < 5)
                log_standard("Channel %d disabled\n", c);
            else
                log_standard("NR5x registers disabled\n");
        }
    }

    if (vgm_opt.wave_enabled == false)
        log_standard("waveform data update disabled\n");

    if (vgm_opt.init_enabled == false)
        log_standard("init disabled\n");

    return true;
}
