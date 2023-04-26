#include "yuv.h"
#include <stdint.h>
#include "is_options.h"
#include "arm_neon.h"

// #define BLANK_EDGES

YUV_T black={0, 128, 128}, white={255, 128, 128}, green={128, 135, 64}, blue={80, 160, 80},
  orange={128, 101, 192}, light_brown={128, 90, 158}, pink={180, 200, 200}, dark_pink={30, 200, 200};

void YUV_Image_Init(YUV_IMAGE_T * i, unsigned char * buffer, int width, int stride, int height) {
  // Assumes U, V are 1/4 size of Y
  i->bY = (uint8_t *) buffer;
  i->bU = &(i->bY[stride*height]);
  i->bV = &(i->bU[stride*height/4]);
  i->w = width;
  i->half_w = width/2;
  i->s = stride;
  i->half_s = stride/2;
  i->h = height;
  i->half_h = height/2;
}

void YUV_Image_Info(YUV_IMAGE_T * i) {
  printf("YUV_IMAGE_T at 0x%x:\n\tw=%d \th=%d \tbY=0x%x \tbU=0x%x \tbV=0x%x\n",
	 (unsigned int) i, i->w, i->h, (unsigned int) i->bY, (unsigned int) i->bU, (unsigned int) i->bV);
}

void Set_Pixel(YUV_IMAGE_T * i, int px, int py, uint8_t lum, uint8_t u, uint8_t v){
  int half_px = px/2, half_py = py/2;
  i->bY[px + py*i->s] = lum;
  i->bU[half_px+half_py*i->half_s] = u;
  i->bV[half_px+half_py*i->half_s] = v;
}

void Set_Pixel_yuv(YUV_IMAGE_T * i, int px, int py, YUV_T * yuv) {
  int half_px = px/2, half_py = py/2;
  // i->bY[px + py*i->s] = yuv->y;
  i->bU[half_px+half_py*i->half_s] = yuv->u;
  i->bV[half_px+half_py*i->half_s] = yuv->v;
}

void Get_Pixel_yuv(YUV_IMAGE_T * i, int px, int py, YUV_T * yuv) {
  int half_px = px/2, half_py = py/2;
  //yuv->y = i->bY[px + py*i->s];
  yuv->u = i->bU[half_px+half_py*i->half_s];
  yuv->v = i->bV[half_px+half_py*i->half_s];
}

int Sq_UV_Difference_yuv(YUV_T * c1, YUV_T * c2) {
  int du, dv;
  
  du = (int) c1->u - (int) c2->u;
  dv = (int) c1->v - (int) c2->v;
  return du*du + dv*dv;

}

int Alt_UV_Difference_yuv(YUV_T * c1, YUV_T * c2){
  int du, dv;
  // TBD
  return 0;
}

void YUV_Image_Copy(YUV_IMAGE_T * dst, YUV_IMAGE_T * src) {
  // Copy bitplanes
  // Warning: assumes images are same size. Should check for this.

  /*  printf("Copy: dst: 0x%x, src: 0x%x, w: %d h: %d, len: %d\n",dst->bY, src->bY, src->w, src->h, (src->w*src->h));
  YUV_Image_Info( src);
  YUV_Image_Info( dst);
  */
  
  memcpy(dst->bY, src->bY, 1.5*(src->s * src->h));
}

void YUV_Image_Fill(YUV_IMAGE_T * dst, YUV_T * c) {
  // Copy bitplanes
  /* printf("Fill: dst: 0x%x (%d), 0x%x (%d) 0x%x, w: %d h: %d, len: %d\n",dst->bY, (dst->bU - dst->bY) ,dst->bU,
     (dst->bV - dst->bU), dst->bV, dst->w, dst->h, dst->w*dst->h);
     YUV_Image_Info( dst);
  */
  
  memset(dst->bY, c->y, dst->s * dst->h);
  memset(dst->bU, c->u, dst->half_s * dst->half_h);
  memset(dst->bV, c->v, dst->half_s * dst->half_h);
}

void YUV_Translate_Image(YUV_IMAGE_T * dst, YUV_IMAGE_T * src, int dX, int dY, float dTheta) {
  // Not ported to use stride yet!!

#if 0
  int src1X=0, dst1X=0, len=0;
  int y, incY=0, startY=0;
  int n, n1=0;
  int h = src->h, w = src->w, half_w = src->half_w;
  int n_bytes = w;

  // make x, y var names consistent
  if (show_data > 0)
    printf("YUV_Translate_Image(%d, %d, %f)\n", dX, dY, dTheta);
  
  if (dY < 0) {
    if (dY < -h/2 + 1)
      dY = -h/2 + 1;
    incY = 1;
    startY = 0;
    n1 = h + dY;
  } else if (dY > 0)  {
    if (dY > h/2 - 1)
      dY = h/2 - 1;
    incY = -1;
    startY = h-1;
    n1 = h - dY;
  }

  if (dX > 0) {
    // add dX clipping
    n_bytes = w - dX;
    src1X = 0;
    dst1X = dX;
  } else if (dX < 0) {
    n_bytes = w + dX;
    src1X = -dX;
    dst1X = 0;
  }

  if (show_data > 1) 
    printf("n_bytes: %d src1X: %d dst1X: %d \t\tstartY: %d n1: %d incY: %d\n",
	   n_bytes, src1X, dst1X, startY, n1, incY);
  
  if ((dY != 0) || (dX != 0)) {
    if (show_data > 1)
      printf("dX, dY = %d, %d\n", dX, dY);
    for (n = 0, y = startY; n < n1; n++, y += incY)
      memcpy(&(dst->bY[y*w + dst1X]), &(src->bY[(y-dY)*w + src1X]), n_bytes); // w   
    for (n = 0, y = startY/2; n < n1/2; n++, y += incY) { 
      memcpy(&(dst->bU[y*half_w + dst1X/2]), &(src->bU[(y-dY/2)*half_w + src1X/2]), n_bytes/2); // half_2
      memcpy(&(dst->bV[y*half_w + dst1X/2]), &(src->bV[(y-dY/2)*half_w + src1X/2]), n_bytes/2); // half_2
    }
  }
#endif
}


void Translate_Image(int dX, int dY, float dTheta) {
 #if 0
  int src1X, src2X, dst1X, dst2X, fwd_copy, len=0;
  int y, incY, startY, end1Y, end2Y;
  int n, n1, n2;
  int n_bytes = w;

  
  if (show_data > 0)
    printf("Translate_Image(%d, %d, %f)\n", dX, dY, dTheta);
  
  if (dY < 0) {
    if (dY < -h/2 + 1)
      dY = -h/2 + 1;
    incY = 1;
    startY = 0;
    n1 = h + dY;
    n2 = h;
  } else if (dY > 0)  {
    if (dY > h/2 - 1)
      dY = h/2 - 1;
    incY = -1;
    startY = h-1;
    n1 = h - dY;
    n2 = h;
  }

  if (dX > 0) {
    n_bytes = w - dX;
    src1X = 0;
    dst1X = dX;
  } else if (dX < 0) {
    n_bytes = w + dX;
    src1X = -dX;
    dst1X = 0;
  }

#if 0
  if ((dY != 0) || (dX != 0)) {
    if (show_data > 1)
      printf("dX, dY = %d, %d\n", dX, dY);
    for (n = 0, y = startY; n < n1; n++, y += incY)
      memcpy(&(bY[y*w + dst1X]), &(bY[(y-dY)*w + src1X]), n_bytes);
#ifdef BLANK_EDGES
    for (; n < n2; n++, y += incY)
      memset(&(bY[y*w]), 0, w);
#endif
    
    for (n = 0, y = startY/2; n < n1/2; n++, y += incY) { 
      memcpy(&(bU[y*half_w]), &(bU[(y-dY/2)*half_w]), half_w);
      memcpy(&(bV[y*half_w]), &(bV[(y-dY/2)*half_w]), half_w);
    }
#ifdef BLANK_EDGES
    for (; n < n2/2; n++, y += incY) {
      memset(&(bU[y*half_w]), 0, half_w);
      memset(&(bV[y*half_w]), 0, half_w);
    }
#endif
  }
#else  // y-only
  if ((dY != 0) || (dX != 0)) {
    if (show_data > 1)
      printf("dX, dY = %d, %d\n", dX, dY);
    for (n = 0, y = startY; n < n1; n++, y += incY)
      memcpy(&(bY[y*w]), &(bY[(y-dY)*w]), w);
#ifdef BLANK_EDGES
    for (; n < n2; n++, y += incY)
      memset(&(bY[y*w]), 0, w);
#endif
    
    for (n = 0, y = startY/2; n < n1/2; n++, y += incY) { 
      memcpy(&(bU[y*half_w]), &(bU[(y-dY/2)*half_w]), half_w);
      memcpy(&(bV[y*half_w]), &(bV[(y-dY/2)*half_w]), half_w);
    }
#ifdef BLANK_EDGES
    for (; n < n2/2; n++, y += incY) {
      memset(&(bU[y*half_w]), 0, half_w);
      memset(&(bV[y*half_w]), 0, half_w);
  }
#endif
}
#endif
#endif
  
}
	
void Draw_Line(YUV_IMAGE_T * img, int p1X, int p1Y, int p2X, int p2Y, YUV_T * color)
// Scan line conversion code from Michael Abrash
{
  int pX, pY;
#if DRAW_RUNS_AS_LINES
  int peX, peY;
#endif
	
  int Temp, AdjUp, AdjDown, ErrorTerm, XAdvance, XDelta, YDelta;	 
  int WholeStep, InitialPixelCount, FinalPixelCount, i,j, RunLength;
  int XStart;
  int YStart;
  int XEnd;
  int YEnd;

  XStart = p1X;
  YStart = p1Y;
  XEnd = p2X;
  YEnd = p2Y;

  /* We'll always draw top to bottom, to reduce the number of cases we have to
  ** handle, and to make lines between the same endpoints draw the same pixels
  */
  if (YStart > YEnd) {
    Temp = YStart;
    YStart = YEnd;
    YEnd = Temp;
    Temp = XStart;
    XStart = XEnd;
    XEnd = Temp;
  }

  /* Figure out whethere we're going left or right, and how far we're going
  ** horizontally  */
  XDelta = XEnd - XStart;
  if (XDelta < 0) {
    XAdvance = -1;
    XDelta = -XDelta;
  } else {
    XAdvance = 1;
  }

  /* Figure out how far we're going vertically */
  YDelta = YEnd - YStart;

  /* Special-case horizontal, vertical, and diagonal lines, for speed and
  ** to avoid nasty boundary conditions and division by 0
  */

  pX = XStart;
  pY = YStart;
	 
  /* Vertical Line case */
  if (XDelta == 0) {
    /* Vertical line */
#if DRAW_RUNS_AS_LINES
    pe.X = XEnd;
    pe.Y = YEnd;
    LCD_Fill_Rectangle(&p, &pe, color);
#else		
    for (i = 0; i <= YDelta; i++) {
      pY++;
      Set_Pixel_yuv(img, pX, pY, color);
    }
#endif
    return;
  }
  
  /* Horizontal Line Case */
  if (YDelta == 0) {
    /* Horizontal line */
#if DRAW_RUNS_AS_LINES
    pe.X = XEnd;
    peY = YEnd;
    LCD_Fill_Rectangle(&p, &pe, color);
#else
    for (i = 0; i <= XDelta; i++) {
      Set_Pixel_yuv(img, pX, pY, color);
      pX += XAdvance;
    }  
#endif
    return;
  }
   
  /* Diagonal Case */
  if (XDelta == YDelta) {
    /* Diagonal line */
    for (i = 0; i <= XDelta; i++)
      {
	Set_Pixel_yuv(img, pX, pY, color); 
	pX += XAdvance;
	pY++;
      }
    return;
  }
   
   
  /* Determine whether the line is X or Y major, and handle accordingly */
  if (XDelta >= YDelta) {
    /* X major line */
    /* Minimum # of pixels in a run in this line */
    WholeStep = XDelta / YDelta;													/* DIV */
    
    /* Error term adjust eacn time Y steps by 1; used to tell when one
    ** extra pixel should be drawn as part of a run, to account for
    ** fractional steps along the X axis per 1-pixel steps along Y
    */
    AdjUp = (XDelta % YDelta) * 2;													/* DIV */
    
    /* Error term adjust when the error term turns over, used to factor
    ** out the X step made at that time
    */
    AdjDown = YDelta * 2;
    
    /* Initial error term; reflects an initial step of 0.5 along the Y axis 
     */
    ErrorTerm = (XDelta % YDelta) - (YDelta * 2);											/* DIV */
    
    /* The initial and last runs are partial, because Y advancse only 0.5
    ** for these runs, rather than 1.  Divide one full run, plus the
    ** initial pixel, between the initial and last runs.
    */
    InitialPixelCount = (WholeStep / 2) + 1;
    FinalPixelCount = InitialPixelCount;
    
    /* If the basic run length is even and there's no fractional
    ** advance, we have one pixel that could go to either the initial
    ** or last partial run, which we'll arbitrarily allocate to the
    ** last run
    */
    if ((AdjUp == 0) && ((WholeStep & 0x01) == 0))
      InitialPixelCount--;
    
    /* If there's an odd number of pixels per run, we have 1 pixel that can't
    ** be allocated to either the initial run or last partial run, so we'll add 0.5
    ** to error term so this pixel will be handled by the normal full-run loop
    */
    if ((WholeStep & 0x01) != 0)
      ErrorTerm += YDelta;
    
    /* Draw the first, partial run of pixels */
#if DRAW_RUNS_AS_LINES // TODO: Fixing here
    pX = XStart;
    pY = peY = YStart;
    if (XAdvance > 0) {
      pe.X = XStart + InitialPixelCount;
      LCD_Fill_Rectangle(&p, &pe, color);
    } else {
      pe.X = XStart - InitialPixelCount;
      LCD_Fill_Rectangle(&pe, &p, color);
    }
    pX = peX;
#else
    for (j = 0; j < InitialPixelCount; j++) {
      Set_Pixel_yuv(img, pX, pY, color); 
      pX += XAdvance;
    }
#endif
    pY++;
    
    /* Draw all full runs */
    for (i = 0; i < (YDelta - 1); i++) {
      RunLength = WholeStep; /* run is at least this long */
      /* Advance the error term and add an extra pixel if the error
      ** term so indicates
      */
      if ((ErrorTerm += AdjUp) > 0) {
	RunLength++;
	ErrorTerm -= AdjDown; /* reset the error term */
      }

      /* Draw this scan line's run */
#if DRAW_RUNS_AS_LINES
      peY = pY;
      pe.X = pX + RunLength;
      LCD_Fill_Rectangle(&p, &pe, color);
      pX = pe.X;
#else
      for (j = 0; j < RunLength; j++) {
	Set_Pixel_yuv(img, pX, pY, color);
	pX += XAdvance;
      }
#endif
      pY++;
    }
    
    /* Draw the final run of pixels */
#if DRAW_RUNS_AS_LINES
    peY = pY;
    pe.X = pe.X + FinalPixelCount;
    LCD_Fill_Rectangle(&p, &pe, color);
    pX = pe.X;
#else
    for (j = 0; j < FinalPixelCount; j++) {
      Set_Pixel_yuv(img, pX, pY, color);
      pX += XAdvance;
    }
#endif
    pY++;
    
    return;
  } else {
    /* Y major line */
    
    /* Minimum # of pixels in a run in this line */
    WholeStep = YDelta / XDelta;										/* DIV */
    
    /* Error term adjust each time X steps by 1; used to tell when 1 extra
    ** pixel should be drawn as part of a run, to account for
    ** fractional steps along the Y axis per 1-pixel steps along X
    */
    AdjUp = (YDelta % XDelta) * 2;										/* DIV */
    
    /* Error term adjust when the error term turns over, used to factor
    ** out the Y step made at that time
    */
    AdjDown = XDelta * 2;
    
    /* Initial error term; reflects initial step of 0.5 along the X axis */
    ErrorTerm = (YDelta % XDelta) - (XDelta * 2);								/* DIV */
    
    /* The initial and last runs are partial, because X advances only 0.5
    ** for these runs, rather than 1.  Divide ony full run, plus the
    ** initial pixel, between the initial and last runs
    */
    
    InitialPixelCount = (WholeStep / 2) + 1;
    FinalPixelCount = InitialPixelCount;
    
    /* If the basic run length is even and there's no fractional
    ** advance, we have one pixel that could go to either the initial
    ** or last partial run, which we'll arbitrarily allocate to the
    ** last run
    */
    if ((AdjUp == 0) && ((WholeStep & 0x01) == 0)) {
      InitialPixelCount--;
    }
      
    /* If there's an odd number of pixels per run, we have 1 pixel that can't
    ** be allocated to either the initial run or last partial run, so we'll add 0.5
    ** to error term so this pixel will be handled by the normal full-run loop
    */
    if ((WholeStep & 0x01) != 0) {
      ErrorTerm += XDelta;
    }
      
    /* Draw the first, partial run of pixels */
#if DRAW_RUNS_AS_LINES
    pX = pe.X = XStart;
    pY = YStart;
    peY = YStart + InitialPixelCount;
    LCD_Fill_Rectangle(&p, &pe, color);
#else
    for (j = 0; j < InitialPixelCount; j++) {
      Set_Pixel_yuv(img, pX, pY, color);
      pY++;
    }
    /* Update x,y position */
    pX += XAdvance;
#endif
      
    /* Draw all full runs */
    for (i = 0; i < (XDelta - 1); i++) {
      RunLength = WholeStep; /* run is at least this long */
	  
      /* Advance the error term and add an extra pixel if the error
      ** term so indicates
      */
      if ((ErrorTerm += AdjUp) > 0)	{
	RunLength++;
	ErrorTerm -= AdjDown; /* reset the error term */
      }
				
      /* Draw this scan line's run */
#if DRAW_RUNS_AS_LINES
      pX = pe.X = XStart;
      pY = YStart;
      peY = YStart + RunLength;
      LCD_Fill_Rectangle(&p, &pe, color);
#else
      for (j = 0; j < RunLength; j++) {
	Set_Pixel_yuv(img, pX, pY, color); 
	pY++;
      }
      /* Update x,y position */
      pX += XAdvance;
#endif
    }
      
    /* Draw the final run of pixels */
#if DRAW_RUNS_AS_LINES
    pX = pe.X= XStart;
    pY = YStart;
    peY = YStart + FinalPixelCount;
    LCD_Fill_Rectangle(&p, &pe, color);
#else
    for (j= 0; j < FinalPixelCount; j++) {
      Set_Pixel_yuv(img, pX, pY, color);
      pY++;
    }
    /* Update x,y position */
    pX += XAdvance;
#endif    
    return;
  }
}

  /* Draw a rectangle at coordinates xm, ym with specified sides and color c. */
void Draw_Rectangle(YUV_IMAGE_T * i, int pcX, int pcY, int dX, int dY, YUV_T * c, int filled) {
    if (filled > 0) {
      // Not implemented yet
    } else {
      Draw_Line(i, pcX-dX/2, pcY-dY/2, pcX+dX/2, pcY-dY/2, c);
      Draw_Line(i, pcX+dX/2, pcY-dY/2, pcX+dX/2, pcY+dY/2, c);
      Draw_Line(i, pcX+dX/2, pcY+dY/2, pcX-dX/2, pcY+dY/2, c);
      Draw_Line(i, pcX-dX/2, pcY+dY/2, pcX-dX/2, pcY-dY/2, c);    
    }
  }

  /* Draw a circle at coordinates xm, ym with radius r and specified color c. */
void Draw_Circle(YUV_IMAGE_T * i, int pcX, int pcY, int radius, YUV_T * c, int filled) {
    int p1X, p1Y, p2X, p2Y;
    int x = -radius, y = 0, err = 2-2*radius; /* II. Quadrant */ 
    if (filled>0) { 
      do {
	//setPixel(xm+x, ym-y); /* III. Quadrant */
	p1X=pcX+x;
	p1Y=pcY-y;
	p2X=pcX-x;
	p2Y=pcY-y;
	Draw_Line(i, p1X, p1Y, p2X, p2Y, c);
	p1Y=pcY+y;
	p2Y=pcY+y;
	Draw_Line(i, p1X, p1Y, p2X, p2Y, c);

	radius = err;
	if (radius <= y) 
	  err += ++y*2+1;           /* e_xy+e_y < 0 */
	if (radius > x || err > y) 
	  err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
      } while (x < 0); 
    } else {
      do {
	//setPixel(xm-x, ym+y); /*   I. Quadrant */
	p1X=pcX-x;
	p1Y=pcY+y;
	Set_Pixel_yuv(i, p1X, p1Y, c);
	//setPixel(xm-y, ym-x); /*  II. Quadrant */
	p1X=pcX-y;
	p1Y=pcY-x;
	Set_Pixel_yuv(i, p1X, p1Y, c);
	//setPixel(xm+x, ym-y); /* III. Quadrant */
	p1X=pcX+x;
	p1Y=pcY-y;
	Set_Pixel_yuv(i, p1X, p1Y, c);
	//setPixel(xm+y, ym+x); /*  IV. Quadrant */
	p1X=pcX+y;
	p1Y=pcY+x;
	Set_Pixel_yuv(i, p1X, p1Y, c);
	radius = err;
	if (radius <= y) err += ++y*2+1;           /* e_xy+e_y < 0 */
	if (radius > x || err > y) 
	  err += ++x*2+1; /* e_xy+e_x > 0 or no 2nd y-step */
      } while (x < 0);
    }
  }

