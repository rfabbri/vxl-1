/*
 *  xv.h  -  header file for xv, but you probably guessed as much
 *
 */

#ifdef __cplusplus
extern "C" {
#endif


#ifndef __XV_H
#define __XV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG 0

#undef PARM 
#define PARM(a) a

/* MONO returns total intensity of r,g,b triple (i = .33R + .5G + .17B) */
#define MONO(rd,gn,bl) ( ((int)(rd)*11 + (int)(gn)*16 + (int)(bl)*5) >> 5)

/* RANGE forces a to be in the range b..c (inclusive) */
#define RANGE(a,b,c) { if (a < b) a = b;  if (a > c) a = c; }


#define F_FULLCOLOR 0
#define F_GIF         0

#define True         1
#define False        0

/* indicies into conv24MB */
#define CONV24_8BIT  0
#define CONV24_24BIT 1
#define CONV24_SEP1  2
#define CONV24_LOCK  3
#define CONV24_SEP2  4
#define CONV24_FAST  5
#define CONV24_SLOW  6
#define CONV24_BEST  7
#define CONV24_MAX   8

/* values 'picType' can take */
#define PIC8  CONV24_8BIT
#define PIC24 CONV24_24BIT


typedef unsigned char byte;
/* info structure filled in by the LoadXXX() image reading routines */
typedef struct { byte *pic;                  /* image data */
                 int   w, h;                 /* pic size */
                 int   type;                 /* PIC8 or PIC24 */
                 byte  r[256],g[256],b[256]; /* colormap, if PIC8 */
                 int   normw, normh;         /* 'normal size' of image file
                                                (normally eq. w,h, except when
                                                doing 'quick' load for icons */
                 int   frmType;              /* def. Format type to save in */
                 int   colType;              /* def. Color type to save in */
                 char  fullInfo[128];        /* Format: field in info box */
                 char  shrtInfo[128];        /* short format info */
                 char *comment;              /* comment text */
                 int   numpages;             /* # of page files, if >1 */
                 char  pagebname[64];        /* basename of page files */
               } PICINFO;

char     *BaseName              PARM((char *));
void     FatalError             PARM((char *));
void     xvbcopy                PARM((char *, char *, size_t));
FILE     *xv_fopen              PARM((char *, char *));
void    xvbzero                PARM((char *,size_t));

int LoadGIF(char *fname, PICINFO *pinfo, int *imagesize);
int WriteGIF(FILE *, byte *, int, int, int, byte *, byte *, byte *, int, int,
             char *);

byte *Conv24to8(byte *, int, int, int, byte *, byte *, byte *);
byte *Conv8to24(byte *, int, int, byte *, byte *, byte *);


#endif

#ifdef __cplusplus
}
#endif

