#ifndef PALETTE_H
#define PALETTE_H


                          
#define PALETTE_FROM_HEADER(PAL) {                                \
	PAL##CGBPal0c0, PAL##CGBPal0c1, PAL##CGBPal0c2, PAL##CGBPal0c3, \
	PAL##CGBPal1c0, PAL##CGBPal1c1, PAL##CGBPal1c2, PAL##CGBPal1c3, \
	PAL##CGBPal2c0, PAL##CGBPal2c1, PAL##CGBPal2c2, PAL##CGBPal2c3, \
	PAL##CGBPal3c0, PAL##CGBPal3c1, PAL##CGBPal3c2, PAL##CGBPal3c3, \
	PAL##CGBPal4c0, PAL##CGBPal4c1, PAL##CGBPal4c2, PAL##CGBPal4c3, \
	PAL##CGBPal5c0, PAL##CGBPal5c1, PAL##CGBPal5c2, PAL##CGBPal5c3, \
	PAL##CGBPal6c0, PAL##CGBPal6c1, PAL##CGBPal6c2, PAL##CGBPal6c3, \
	PAL##CGBPal7c0, PAL##CGBPal7c1, PAL##CGBPal7c2, PAL##CGBPal7c3} 

//RGB defined in cgb.h has a << 0 that kills the compiler
#define RGB2(r, g, b) ((UINT16)(r)) | (((UINT16)(g)) <<  5) | ((((UINT16)(b)) << 8) << 2);

#define PAL_RED(C)   (((C)      ) & 0x1F)
#define PAL_GREEN(C) (((C) >>  5) & 0x1F)
#define PAL_BLUE(C)  (((C) >> 10) & 0x1F)


#endif