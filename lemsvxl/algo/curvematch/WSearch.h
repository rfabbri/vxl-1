//----------------------------------------------------------------------------
// WSearch.H
// class definition for WSearch.CPP
// import definition for WSearch.DLL
// interface image format using width, height, and pointer*
// internal image format using class Image (32-bit putimage)
// Date: 11/24/1997
// Author: MingChing Chang
//----------------------------------------------------------------------------

//#include <eh.h>
//#include <except.h>
//#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <stdarg.h>
#include <stdlib.h>
//#include <string.h>

#define DllImport __declspec( dllimport )
#define DllExport __declspec( dllexport )

#define MAX(a, b)  (((a) > (b)) ? (a) : (b)) 
#define MIN(a, b)  (((a) < (b)) ? (a) : (b)) 

#define  IMAGEX 640
#define IMAGEY 480
#define IMAGESIZE 640*480


/////////////////////////////////////////////////////////////////////
// Export Functions
/////////////////////////////////////////////////////////////////////

//DllExport bool WDPNCS (
//      int patternwidth, int patternheight, void* patternpointer,
//      int searchwidth, int searchheight, void* searchpointer,
//      int searchx1, int searchy1, int searchx2, int searchy2);


/////////////////////////////////////////////////////////////////////
// Image Definition
/////////////////////////////////////////////////////////////////////

//typedef struct tagImageHeader {
//short int width, height, width1, height1;
//  short int zero, width2, eight, one;
//} IMAGEHEADER;

#define B_PIX(im, x, y) \
  ((unsigned char)*((unsigned char*)im->imagedata+(y)*(im->width)+(x)))

#define B_PIX_REF(im, x, y) \
  ((unsigned char*)im->imagedata+(y)*(im->width)+(x))

#define PIX(imgbuf, width, x, y) \
  ((unsigned char)*((unsigned char*)imgbuf+(y)*(width)+(x)))

class Image
{
  public:
    int width, height, imagesize;
    void* imagedata;
    bool balloc;
    
    Image (int w, int h);
    Image (int w, int h, void* imd);
    Image (int w, int h, void* imd, bool bcopy);
    Image (int w, int h, void* imd, int sx, int sy);

    ~Image () { if (balloc) free(imagedata); }
    
    inline void CopyImage (Image* im);
};

/////////////////////////////////////////////////////////////
//Structure for Search...
/////////////////////////////////////////////////////////////
//CFstr, correlation coefficient structure
//after sorting, in largest first order...
// cf: the correlation value in double
// x:  the coordinate x
// y:  the coordinate y
typedef struct tagCFstr {
  double cf;
  int x, y;
} CFstr;

typedef struct tagCFSstr {
  double cf;
  double x, y;
} CFSstr;

typedef enum tagSearchOption {
  COARSE_ONLY,    //coarse search only
  SEARCH_ONE_FAST,  //the max CF value to do fine search
  SEARCH_ONE,      //max 3 CF value to do fine search
  SEARCH_MN,      //dSearchMinCF=0.2, nCoarseTarget=5, nSearchTarget=3
  SEARCH_MC,      //dSearchMinCF=0.2, nCoarseTarget=5, dSearchMinCF=0.8
  AUTO_SEARCH      //the same as SEARCH_MC
} SEARCHOPTION;



typedef enum tagSearchSubPixel {
  PIXEL_LEVEL,
  SUBPIXEL_LEVEL,   //interpolation, oversampling
  ACCURATE_SUBPIXEL
} SEARCHSUBPIXEL;

typedef enum tagAutoModel {
  ONE_MODEL,
  TWO_MODEL
} AUTOMODEL;
