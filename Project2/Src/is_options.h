#ifndef IS_OPTIONS_H
#define IS_OPTIONS_H
#include <stdint.h>
#include <stdio.h>

#define ISS_WIDTH (1280)
#define ISS_STRIDE (1280)
#define ISS_HEIGHT (720)

#define MAX_NUM_FRAMES (9)

extern volatile int run;
extern int invert, invert_rect;
extern int update_target_color;
extern int show_data;
extern int highlight_matches;
extern int color_threshold;
extern volatile int extend_threshold;
extern int imstab_servo;
extern int imstab_digital;
extern int chroma_subsample_sep;

extern float ServoTiltDegree;
extern float ServoPanDegree;

extern FILE * log_fp;

#endif // IS_OPTIONS_H
