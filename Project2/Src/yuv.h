#ifndef YUV_H
#define YUV_H
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>

typedef struct {
  uint8_t y;
  uint8_t u;
  uint8_t v;
} YUV_T;

extern YUV_T black, white, green, orange, blue, light_brown, pink, dark_pink;

typedef struct {
  int w, s, h, half_w, half_h, half_s;
  uint8_t *bY;
  uint8_t *bU;
  uint8_t *bV;
} YUV_IMAGE_T;


void YUV_Image_Init(YUV_IMAGE_T * i, unsigned char * buffer, int width, int stride, int height);
void YUV_Image_Info(YUV_IMAGE_T * i);
void Set_Pixel(YUV_IMAGE_T * i, int px, int py, uint8_t lum, uint8_t u, uint8_t v);
inline void Set_Pixel_yuv(YUV_IMAGE_T * i, int px, int py, YUV_T * yuv);
inline void Get_Pixel_yuv(YUV_IMAGE_T * i, int px, int py, YUV_T * yuv);


inline int Sq_UV_Difference_yuv(YUV_T * c1, YUV_T * c2);
int Alt_UV_Difference_yuv(YUV_T * c1, YUV_T * c2);
void Draw_Line(YUV_IMAGE_T * i, int p1X, int p1Y, int p2X, int p2Y, YUV_T * color);
void Draw_Circle(YUV_IMAGE_T * i, int pcX, int pcY, int radius, YUV_T * c, int filled);
void Draw_Rectangle(YUV_IMAGE_T * i, int pcX, int pcY, int dX, int dY, YUV_T * c, int filled);

void YUV_Image_Copy(YUV_IMAGE_T * dst, YUV_IMAGE_T * src);
void YUV_Image_Fill(YUV_IMAGE_T * dst, YUV_T * c);

void YUV_Translate_Image(YUV_IMAGE_T * dst, YUV_IMAGE_T * src, int dX, int dY, float dTheta);
void Translate_Image(int dX, int dY, float dTheta);
#endif // YUV_H

