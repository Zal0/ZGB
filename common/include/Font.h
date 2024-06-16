#ifndef __FONT_H_INCLUDE__
#define __FONT_H_INCLUDE__

#define RECODE_7BIT  1
#define FONT_VWF     2

typedef struct font_desc_t {
    unsigned char attr;
    const unsigned char * recode_table;
    const unsigned char * widths;
    const unsigned char * faces;
    unsigned char num_faces;
} font_desc_t;

#endif