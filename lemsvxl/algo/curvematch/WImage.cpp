//----------------------------------------------------------------------------
// WImage.CPP
// image support for WSearch.CPP
// interface image format: width, height, and RAW array
// internal image format: customized image class
// Dll using static link
// Date: 11/24/1997
// Author: MingChing Chang
//----------------------------------------------------------------------------

//#include <vcl.h>
//#pragma hdrstop

#include "WSearch.h"
#include <vcl_iostream.h>
#include <vcl_cstring.h>

/////////////////////////////////////////////////////////////////////
// IMAGE.CPP
/////////////////////////////////////////////////////////////////////

Image::Image (int w, int h)
{
   width = w;
   height = h;
   imagesize = w*h;
   balloc = true;
   imagedata = malloc (imagesize); //(-1)NOTICE!!
   if (imagedata==NULL) { vcl_cout << "Memory not enough !"; exit(1); }
   memset (imagedata, 0, imagesize); //clear
}

Image::Image (int w, int h, void* imd)
{
   width = w;
   height = h;
   imagesize = w*h;
   imagedata = (unsigned char*) imd;
  balloc = false;
}

Image::Image (int w, int h, void* imd, bool bcopy)
{
   width = w;
   height = h;
   imagesize = w*h;
  balloc = true;
   imagedata = (unsigned char*) malloc (imagesize); //(-1)NOTICE!!
   if (imagedata==NULL) { vcl_cout << "Memory not enough !"; exit(1); }
   memcpy (imagedata, imd, imagesize); //copy the source image
}


//new an new image, of imd (640x480),
//which starts in the upper-left point (sx,sy)
Image::Image (int w, int h, void* imd, int sx, int sy)
{
   width = w;
   height = h;
   imagesize = w*h;
   balloc = true;
   imagedata = malloc (imagesize); //(-1)NOTICE!!
   if (imagedata==NULL) { vcl_cout << "Memory not enough !"; exit(1); }

   unsigned char *s, *d;
   d = (unsigned char*) imagedata;
   for (int y=sy; y<sy+h; y++) {
      s = (unsigned char*)imd + y*640 + sx;
      memcpy (d, s, w); //copy the source image
    d += w;
   }
}

void Image::CopyImage (Image* im)
{
  memcpy (imagedata, im->imagedata, im->imagesize);
}

// draw horizontal or vertical line on image im pixel by pixel...
void imHVLine (Image* im, int x1, int y1, int x2, int y2, int color)
{
   int i;
   int w = im->width;
   unsigned char *s;

   if (x1==x2) { // vertical line
      s = B_PIX_REF (im, x1, y1);
      for (i=y1; i<=y2; i++, s+=w) (*s)=(unsigned char)color;
   }

   if (y1==y2) { // horizontal line
      s = B_PIX_REF (im, x1, y1);
      for (i=x1; i<=x2; i++, s++) (*s)=(unsigned char)color;
   }
}

