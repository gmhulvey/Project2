#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "is_options.h"
#include "image_proc.h"

#define BITPLANE_SIZE (1382400)

uint8_t bpin[BITPLANE_SIZE], bpref[BITPLANE_SIZE], bpout[BITPLANE_SIZE];

int main(int argc, char** argv) {
  char fnin[] = "Media/image.yuv";
  char fnout[] = "Media/output.yuv";
  size_t nb;
  FILE * fin, * fout;
  struct timespec t1, t2;
  long t;
  int loop = 0;
  double t_sum_ms = 0;
  long t_min = 12345678, t_max = 0;

  // Open files
  fin  = fopen(fnin, "rb");
  if (!fin) {
    printf("Couldn't open input file %s\n", fnin);
    return 0;
  }
  fout = fopen(fnout, "wb");
  if (!fout) {
    printf("Couldn't open output file %s\n", fnout);
    return 0;
  }   
  printf("Assuming %s is YUV420 image W: %d, S: %d, H: %d\n", fnin, ISS_WIDTH, ISS_STRIDE, ISS_HEIGHT);
  fseek(fin, 0, SEEK_SET);
  nb = fread(bpref, 1, BITPLANE_SIZE, fin);
  printf("Read %d bytes from %s.\n", nb, fnin);

  for (int f = 0; f < 200; f++) {
    //   copy frame into memory
    memcpy(bpin, bpref, BITPLANE_SIZE);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t1);
    //   Call processing code
    process_image(bpin, bpout);
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t2);
    t = t2.tv_nsec - t1.tv_nsec;
    if (t<0)
      t += 1000000000;
    t_max = t > t_max ? t : t_max;
    t_min = t < t_min ? t : t_min;
    t_sum_ms += t/1000000.0;
    loop++;
  }
  
  //   Save frame to file as yuv (later convert to jpg or png for viewing)
  nb = fwrite(bpin, 1, BITPLANE_SIZE, fout);
  printf("Wrote %d bytes to %s.\n", nb, fnout);	       
  fclose(fout);
  fclose(fin);

  printf("Frame proc. time (ms): last %6.4f (min %6.4f, avg. %6.4f, max %6.4f)\n",
       t/1000000.0, t_min/1000000.0, (float) (t_sum_ms/loop), t_max/1000000.0);

  return 0;
}

