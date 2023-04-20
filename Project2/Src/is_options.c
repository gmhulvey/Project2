#include "is_options.h"

volatile int run=1;
int invert=0, invert_rect=0;
int update_target_color=0;
int show_data=1;
int highlight_matches=1;
int color_threshold=900;
volatile int extend_threshold=0; 
float ServoTiltDegree = 90;
float ServoPanDegree = 90;
int imstab_servo = 0;
int imstab_digital = 0;
int chroma_subsample_sep = 2; // TODO change back to 2; // going below 2 has no effect, since chroma is 1/2 subsampled already
