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

#include "fxhammer.h"

// #define VERSION "version 1.0.0"

static void init(void);
static void cleanup(void);
static void display_help(void);
static int  handle_args(int argc, char * argv[]);
static bool increment_if_next_arg_available(int * i, int argc, char * opt_str);
static void warn_if_arg_looks_like_option(char * arg, char * opt_str, char * usage_str);

uint8_t * p_buf_in  = NULL;
size_t    buf_in_len = 0;


bool show_help_and_exit = false;



static void init(void) {
}


// Registered as an atexit handler
static void cleanup(void) {

    // fxhammer_cleanup();

    if (p_buf_in != NULL) {
        free(p_buf_in);
        p_buf_in = NULL;
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
            p_buf_in = file_read_into_buffer(opt.infilename, &buf_in_len);

            if (p_buf_in) {
                if (fxhammer_load(p_buf_in, buf_in_len)) {
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
    "Usage: fxhammer2data   [options] INPUT_FILE_NAME.SAV"
    "  -o, --out            output file name\n"
    "  -i, --identifier     source identifier\n"
    "  -n, --number         effect number or \"all\"\n"
    "  -m, --system         target system GB/PSG (default \"GB\")\n"
    "  -d, --delay          delay size\n"
    "  -b, --bank           BANK number (default AUTO=255)\n"    
    "  -c, --cut            cut all used sound channels at the end\n"
    "  -p, --no-pan         disable channel panning\n"
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


        } else if ((strstr(argv[i], "-c") == argv[i]) || (strstr(argv[i], "--cut") == argv[i])) {
            opt.cut_sound = true;
        } else if ((strstr(argv[i], "-p") == argv[i]) || (strstr(argv[i], "--no-pan") == argv[i])) {
            opt.use_pan = false;


        // == Output Filename ==
        } else if (strstr(argv[i], "-o") == argv[i]) {
            if (! increment_if_next_arg_available(&i, argc, "-o")) return false;
            warn_if_arg_looks_like_option(argv[i], "-o", "Usage: -o filename");
            snprintf(opt.outfilename, sizeof(opt.outfilename), "%s", argv[i]);

        } else if (strstr(argv[i], "--out=") == argv[i]) {
            snprintf(opt.outfilename, sizeof(opt.outfilename), "%s", argv[i] + strlen("--out="));

        // == Output Identifier ==
        } else if (strstr(argv[i], "-i") == argv[i]) {
            if (! increment_if_next_arg_available(&i, argc, "-i")) return false;
            warn_if_arg_looks_like_option(argv[i], "-i", "Usage: -i identifier");
            snprintf(opt.identifier, sizeof(opt.identifier), "%s", argv[i]);

        } else if (strstr(argv[i], "--identifier=") == argv[i]) {
            snprintf(opt.identifier, sizeof(opt.identifier), "%s", argv[i] + strlen("--identifier="));

        // == Bank ==
        } else if (strstr(argv[i], "-b") == argv[i]) {
            if (! increment_if_next_arg_available(&i, argc, "-b")) return false;
            warn_if_arg_looks_like_option(argv[i], "-b", "Usage: -b bank number");
            opt.bank = strtol(argv[i], NULL, 10);

        } else if (strstr(argv[i], "--bank=") == argv[i]) {
            opt.bank = strtol(argv[i] + strlen("--bank="), NULL, 10);

        // == Delay ==
        } else if (strstr(argv[i], "-d") == argv[i]) {
            if (! increment_if_next_arg_available(&i, argc, "-d")) return false;
            warn_if_arg_looks_like_option(argv[i], "-d", "Usage: -d delay");
            opt.delay = strtol(argv[i], NULL, 10);

        } else if (strstr(argv[i], "--delay=") == argv[i]) {
            opt.delay = strtol(argv[i] + strlen("--delay="), NULL, 10);

        // == Number ==
        } else if (strstr(argv[i], "-n") == argv[i]) {
            if (! increment_if_next_arg_available(&i, argc, "-n")) return false;
            warn_if_arg_looks_like_option(argv[i], "-n", "Usage: -n number or \"all\"");
            if (strstr(argv[i], "all") == argv[i]) {
                opt.effectnum = EFFECTNUM_USE_ALL;
            } else {
                opt.effectnum = strtol(argv[i], NULL, 10);
            }

        } else if (strstr(argv[i], "--number=") == argv[i]) {
            if (strstr(argv[i] + strlen("--number="), "all") == argv[i]) {
                opt.effectnum = EFFECTNUM_USE_ALL;
            } else {
                opt.effectnum = strtol(argv[i] + strlen("--number="), NULL, 10);
            }

        // == System ==
        } else if (strstr(argv[i], "-m") == argv[i]) {
            if (! increment_if_next_arg_available(&i, argc, "-m")) return false;
            warn_if_arg_looks_like_option(argv[i], "-m", "Usage: -m GB or PSG");
            strtolower(argv[i]);
            if      (strstr(argv[i], "gb")  == argv[i]) opt.system = SYSTEM_GB;
            else if (strstr(argv[i], "psg") == argv[i]) opt.system = SYSTEM_PSG;
            else {
                log_error("Error: unknown target system, should be in [GB, PSG]\n\n");
                display_help();
                return false;
            }

        } else if (strstr(argv[i], "--system=") == argv[i]) {
            char * strarg = argv[i] + strlen("--system=");
            if      (strstr(strarg, "gb")  == strarg) opt.system = SYSTEM_GB;
            else if (strstr(strarg, "psg") == strarg) opt.system = SYSTEM_PSG;
            else {
                log_error("Error: unknown target system, should be in [GB, PSG]\n\n");
                display_help();
                return false;
            }


        } else if (argv[i][0] == '-') {
            log_error("Unknown argument: %s\n\n", argv[i]);
            display_help();
            return false;

        } else {
            // Set input filename if no dash present
            snprintf(opt.infilename, sizeof(opt.infilename), "%s", argv[i]);
        }
    }

    // == Validation and reporting ==

    // If no output filename specified then base it on input filename 
    if (opt.infilename[0] == '\0') {
        log_error("Error: Input file name required\n");
        display_help();
        show_help_and_exit = true;
        return false; // Abort
    }

    // Validate bank number
    if ((opt.bank < BANK_NUM_MIN) || (opt.bank > BANK_NUM_MAX)) {
        log_error("Error: Invalid bank number specified with --bank=%d\n", opt.bank);
        display_help();
        show_help_and_exit = true;
        return false; // Abort
    }

    // Validate effectnum
    if ((opt.effectnum != EFFECTNUM_USE_ALL) && (opt.effectnum > EFFECTNUM_MAX)) {
        log_error("Error: Invalid effect number specified (%d)\n", opt.effectnum);
        display_help();
        show_help_and_exit = true;
        return false; // Abort
    }

    // If no output filename specified then base it on input filename 
    if (opt.outfilename[0] == '\0') {
        strcpy(opt.outfilename, opt.infilename);
        filename_remove_extension(opt.outfilename);
        strcat(opt.outfilename, ".c");
    }
    
    // If no identifier specified then base it on input filename 
    if (opt.identifier[0] == '\0') {
        strcpy(opt.identifier, get_filename_from_path(opt.infilename));
        filename_remove_extension(opt.identifier);
    }

    return true;
}
