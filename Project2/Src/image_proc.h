#ifndef IMAGE_PROC_H
#define IMAGE_PROC_H


#define W_INVERT 1280
#define H_INVERT 720

#define ASCII_ESC 27
// #define NUM_DISPLAY


extern FILE * fp;

void process_image(uint8_t * src_bitplanes, uint8_t * dst_bitplanes);

#endif
