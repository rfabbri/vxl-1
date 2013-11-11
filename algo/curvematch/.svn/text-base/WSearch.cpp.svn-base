//----------------------------------------------------------------------------
// WSearch.CPP
// create WSearch.DLL
// export WDPNCS(), WRPDPNCS(), WSRPDPNCS()
// interface image format: width, height, and RAW array
// internal image format: customized image class
// Dll using static link
// Date: 11/24/1997
// Author: MingChing Chang
//----------------------------------------------------------------------------

//#include <vcl.h>
//#pragma hdrstop

#include "WSearch.h"
#include <vcl_string.h>
#include <vcl_iostream.h>
 
bool DPNCS (Image* pim, Image* image, int sx1, int sy1, int sx2, int sy2);
bool RPDPNCS (Image* pim, Image* image, int sx1, int sy1, int sx2, int sy2);
bool SRPDPNCS (Image* pim, Image* image, int sx1, int sy1, int sx2, int sy2);
 
/////////////////////////////////////////////////////////////////////
// DLL Export Functions and variables
/////////////////////////////////////////////////////////////////////

double matchCF;
int matchX, matchY;
double smatchX, smatchY;

int getMatchX (void)
{ return matchX; }
int getMatchY (void)
{ return matchY; }
double getMatchCF (void)
{ return matchCF; }
double getSMatchX (void)
{ return smatchX; }
double getSMatchY (void)
{ return smatchY; }

//return TRUE:  SUCCESS
//       FALSE: FAIL
bool 
WDPNCS (int patternwidth, int patternheight, void* patternpointer,
    int searchwidth, int searchheight, void* searchpointer,
    int searchx1, int searchy1, int searchx2, int searchy2)
{
   bool r;

   Image* pim = new Image (patternwidth, patternheight, patternpointer);
   Image* sim = new Image (searchwidth,  searchheight,  searchpointer);
   r = DPNCS (pim, sim, searchx1, searchy1, searchx2, searchy2);
   delete pim, sim;
   return r;
}

bool  //BOOL far _export
WRPDPNCS (int patternwidth, int patternheight, void* patternpointer,
      int searchwidth, int searchheight, void* searchpointer,
      int searchx1, int searchy1, int searchx2, int searchy2)
{
   bool r;

   Image* pim = new Image (patternwidth, patternheight, patternpointer);
   Image* sim = new Image (searchwidth,  searchheight,  searchpointer);
   r = RPDPNCS (pim, sim, searchx1, searchy1, searchx2, searchy2);
   delete pim, sim;
   return r;
}

bool  //BOOL far _export
WSRPDPNCS (int patternwidth, int patternheight, void* patternpointer,
       int searchwidth, int searchheight, void* searchpointer,
       int searchx1, int searchy1, int searchx2, int searchy2)
{
   bool r;

   Image* pim = new Image (patternwidth, patternheight, patternpointer);
   Image* sim = new Image (searchwidth,  searchheight,  searchpointer);
   r = SRPDPNCS (pim, sim, searchx1, searchy1, searchx2, searchy2);
   delete pim, sim;
   return r;
}


/////////////////////////////////////////////////////////////////////
// SEARCH.CPP
/////////////////////////////////////////////////////////////////////

SEARCHOPTION   SearchOption=SEARCH_ONE_FAST;  //default
SEARCHSUBPIXEL SearchSubPixel=PIXEL_LEVEL; //default no subpixel
AUTOMODEL      AutoModelOption=TWO_MODEL;

int    BASE=0;   //subpixel BASE

int    nSearchTarget=5;
int    nCoarseTarget=10;
double dSearchMinCF=0.5;
double dCoarseMinCF=0.2;


//////////////////////////////////////////////////////////////////////
// Dynamic Programming NCS (Normalized Cross Correlation)
/////////////////////////////////////////////////////////////////////
// 1) initilize the search image sim in a continuous memory
// 2) construct the dynamic programming table for sim, sim^2,
//   and the correlation function table: double CF[y][x]
// 3) compute the pattern image SUM SUM M(i,j) and M^2(i,j)
// 4) compute the correlation function table
//   CF[sh-ph+1][sw-pw+1]
//   NOTE: i) in this program, table starts in 1,
//     but image starts in 0
//    ii) be careful to avoid overflow, must in double
// 5) compute the max value of CF table
// 6) free the allocated memory
//
// NOTE (11/13): use memcpy() to do memory copy

unsigned int **DPsim, **DPSsim;
double CF[640][480]; //static array
//double matchCF;
//int matchX, matchY; //match position in search image

bool DPNCS (Image* pim, Image* image, int sx1, int sy1, int sx2, int sy2)
{
   Image* sim;
   unsigned char *s, *d;
   int sw = sx2-sx1+1;   //search width
   int sh = sy2-sy1+1;   //search height
   int pw = pim->width;   //pattern width
   int ph = pim->height;   //pattern height
   int x, y, t;
   unsigned char *ss, *sp; //source search image, source pattern image
   int sx, sy;

   //
   // 1) initilize the search image sim in a continuous memory
   //
   sim = new Image (sw, sh);
   d = (unsigned char*) sim->imagedata;
   for (y=sy1; y<=sy2; y++) {
      s = B_PIX_REF(image, sx1, y);
      memcpy (d, s, sw);
      d += sw;
   }

   //
   // 2) construct the dynamic programming table for sim, sim^2, and CF
   //
   //DPsim[y+1][x+1], DPSsim[y+1][x+1], CF[y+1][x+1]
   try {
    DPsim = new unsigned int*[sh+1];
    DPSsim = new unsigned int*[sh+1];
    //CF = new double*[sh+1];
    for (y=0; y<sh+1; y++) {
       DPsim[y] = new unsigned int[sw+1];
       DPSsim[y] = new unsigned int[sw+1];
       //CF[y] = new double[sw+1];
      }
   }
   catch ( ... ) {
      vcl_cout << "Memory not enough !"; exit(1);
   }
   //First row and column set to zero...
   for (y=0; y<sh+1; y++) {
      DPsim[y][0]=0;
      DPSsim[y][0]=0;
   }
   for (x=1; x<sw+1; x++) {
      DPsim[0][x]=0;
      DPSsim[0][x]=0;
   }
   //Fill the table...
   d = (unsigned char*) sim->imagedata;
   for (y=1; y<sh+1; y++)
      for (x=1; x<sw+1; x++) {
       t = (unsigned int) (*d);
       DPsim[y][x] = DPsim[y-1][x]    + DPsim[y][x-1] -
                   DPsim[y-1][x-1]  + t;
       DPSsim[y][x]= DPSsim[y-1][x]   + DPSsim[y][x-1] -
                   DPSsim[y-1][x-1] + (t*t);
       d++;
    }
   //writefileintblock ("DPsim", DPsim, sw+1, sh+1);
   //writefileintblock ("DPSsim", DPSsim, sw+1, sh+1);

   //
   // 3) compute the pattern image SUM SUM M(i,j) and M^2(i,j)
   //
   unsigned int PM=0, PMS=0;
   d = (unsigned char*) pim->imagedata;
   for (y=0; y<ph; y++)
    for (x=0; x<pw; x++) {
       t = (*d);
       PM += t;
       PMS += t*t;
       d++;
      }
   //showMsg (RED, "PM=%d, PMS=%d", PM, PMS); getch();

   //
   // 4) compute the correlation function table,
   //   in the range (0,0)-(sh-ph+1,sw-pw+1)
   //
   int N=ph*pw;
   unsigned int DPs, DPSs;
   unsigned int sumpx; //sum 2 for loop in 4
   double tn; //numerator, must in double
   double td; //denominator, must in double

   for (sy=0; sy<sh-ph+1; sy++)
      for (sx=0; sx<sw-pw+1; sx++) {
          sp = (unsigned char*) pim->imagedata;
       DPs  = DPsim[sy+ph][sx+pw]  - DPsim[sy+ph][sx] -
            DPsim[sy][sx+pw]     + DPsim[sy][sx];
       DPSs = DPSsim[sy+ph][sx+pw] - DPSsim[sy+ph][sx] -
            DPSsim[sy][sx+pw]    + DPSsim[sy][sx];
       sumpx=0;
       for (y=0; y<ph; y++) {
          ss = B_PIX_REF(sim, sx, sy+y);
          for (x=0; x<pw; x++) {
             sumpx += (*ss)*(*sp);
             ss++; sp++;
            }
         }
       //putpixel (sx,sy,LIGHTRED);
       tn = (double)((double)N*sumpx - (double)DPs*PM);
       if (tn>0) tn=tn*tn;
       else tn=0;

       td = (double)((double)N*DPSs - (double)DPs*DPs)*
          ((double)N*PMS - (double)PM*PM);
     CF[sy][sx] = (double) tn/td;
       //if (td==0.0) CF[sy][sx] = 0.0;
       //else CF[sy][sx] = (double) tn/td;

       //if ((kbhit() || mMouseClick())) {
     //   showMsg (LIGHTRED, "Break by USER!");
     //   for (y=0; y<sh+1; y++)
         //      { delete[] DPsim[y]; delete[] DPSsim[y]; delete[] CF[y]; }
     //   delete[] DPsim; delete[] DPSsim; delete[] CF; delete sim;
     //   return FALSE;
         //} 
       //showMsg (RED, "sy=%d sx=%d DPs=%d DPSs=%d sumpx=%d tn=%1.3f td=%1.3f",
       //    sy, sx, DPs, DPSs, sumpx, tn, td); getch();
       //showMsg (RED, "sy=%d sx=%d tn=%d td=%d", sy, sx, tn, td); getch();
      }

   //
   // 5) compute the max value of CF table
   //
   matchCF=-1;
   for (sy=0; sy<sh-ph+1; sy++)
      for (sx=0; sx<sw-pw+1; sx++)
          if (matchCF < CF[sy][sx]) {
          matchCF=CF[sy][sx];
          matchX=sx; matchY=sy;
         } 
   //if (bShowCFImage) ShowCFImage (sw-pw+1, sh-ph+1);

   //
   // 6) free the allocated memory...
   //
   for (y=0; y<sh+1; y++) {
      delete[] DPsim[y];
    delete[] DPSsim[y];
      //delete[] CF[y];
   }
   delete[] DPsim;
   delete[] DPSsim;
   //delete[] CF;
   delete sim;

   return true;
}

//Image Half Sub-sampling Function
//new a sub-image sim, and return it
//Sub-sampling method: average gray level
Image* sub2sampling (Image* im)
{
   int x, y, t;

   int sw = (im->width/4)*2; //subsampling image width, even
   int sh = im->height/2;   //subsampling image height
   Image* sim = new Image (sw, sh);

   //unsigned char *s1 = (unsigned char*) im->imagedata;
   //unsigned char *s2 = (unsigned char*) im->imagedata+(im->width);
   unsigned char *s1, *s2;
   unsigned char *d = (unsigned char*) sim->imagedata;

   //image subsamling...
   for (y=0;y<sh; y++) {
     s1=B_PIX_REF(im,0,y<<1);
    s2=B_PIX_REF(im,0,y<<1)+(im->width);
    for (x=0;x<sw; x++) {
       t = (int)(*s1)+(int)(*(s1+1))+(int)(*s2)+(int)(*(s2+1));
       *d = (unsigned char)(t>>2);
       d++;
       s1+=2;
       s2+=2;
      }
   }

   return sim;
}

Image* over2sampling (Image* im)
{
   int x, y;
   int t1, t2, t3, t4;

   int ow = im->width*2;
   int oh = im->height*2;
   Image* oim = new Image (ow, oh);

   unsigned char *s1, *s2, *s3, *s4, *s5;
   unsigned char *s = (unsigned char*) im->imagedata;

   //image over-copying...
   for (y=0; y<oh; y+=2) {
      s1=B_PIX_REF(oim,0,y);
      for (x=0; x<ow; x+=2) {
         (*s1)=(*s);
         s1+=2;
       s++;
      }
   }

   //image oversampling, for pixel on the oversampling image...
   //the last two lines (v&h) are not set here...
   for (y=0; y<oh-2; y+=2) {
      s1=B_PIX_REF(oim,0,y);
      s2=B_PIX_REF(oim,0,y+2);
      s3=s1+2;
      s4=s2+2;
      s5=B_PIX_REF(oim,0,y+1);
      for (x=0; x<ow-2; x+=2) {
         t1=*(s1);
         t2=*(s2);
         t3=*(s3);
         t4=*(s4);
         *(s1+1)=(unsigned char)((t1+t3)>>1);
         *(s2+1)=(unsigned char)((t2+t4)>>1);
         *(s5)=(unsigned char)((t1+t2)>>1);
         *(s5+2)=(unsigned char)((t3+t4)>>1);
         *(s5+1)=(unsigned char)((t1+t2+t3+t4)>>2);
         s1+=2;
         s2+=2;
         s3+=2;
         s4+=2;
         s5+=2;
      }
   }
   //set the last two lines (h) (extrapolation)...
   for (x=0; x<ow; x+=2) {
      s1=B_PIX_REF(oim,x,oh-2);
    s5=B_PIX_REF(oim,x,oh-1);
    t1=*(s1);
    *(s1+1)=(unsigned char)t1;
    *(s5)=(unsigned char)t1;
    *(s5+1)=(unsigned char)t1;
   }
   //set the last two lines (v) (extrapolation)...
   for (y=0; y<oh; y+=2) {
      s1=B_PIX_REF(oim,ow-2,y);
      s5=B_PIX_REF(oim,ow-2,y+1);
      t1=*(s1);
      *(s1+1)=(unsigned char)t1;
      *(s5)=(unsigned char)t1;
      *(s5+1)=(unsigned char)t1;
   }
   return oim;
}

//Selection Sort to find first fN values which CF[] >= minCF,
//return to a struct CFstr[], list in largest first order
//return: NCFm, CFmax[0 ~ NCFm-1], (NCFm<=fN)
//CFstr define in ICINSP.H
//remember to delete CFmax after call CFSelSort()
//
// 1) initilize CFmax[fn]
// 2) selection sort, change swap to fill 0 in CF[][]
// 3) after sorting, recover CFmax[] to corresponding CF[][]
//
//////*******Multi-Return
int  NCFm;
CFstr CFmax[100]; //CFmax[fN]
CFSstr CFSmax[100]; //for subpixel
void CFSelSort (double minCF, int fN, int wcf, int hcf)
{
   int i, x, y, maxx, maxy;
   double max;

   //
   // 1) initilize CFmax[fN]
   //
   //try {
   //   CFmax = new CFstr[fN];
   //}
   //catch ( ... ){
   //   //closegraph (); vcl_cout << "Memory not enough !"; exit(1);
   //}
   //
   // 2) selection sort, change swap to fill 0 in CF[][]
   //
   NCFm=0;
   for (i=0; i<fN; i++) {
      max=-1;
      for (y=0; y<hcf; y++) {
         for (x=0; x<wcf; x++)
            if (max<CF[y][x]) {
         max=CF[y][x];
         maxx=x; maxy=y;
            }
    }
      if (max<minCF) break;
      CFmax[i].y=maxy;
      CFmax[i].x=maxx;
      CFmax[i].cf=max;
      NCFm++;
      CF[maxy][maxx]=0;
   }
   //
   // 3) after sorting, recover CF[][] to corresponding CFmax[]
   //
   for (i=0; i<NCFm; i++) {
      CF[CFmax[i].y][CFmax[i].x]=CFmax[i].cf;
   }
}

//re-sort the CFMax[0..NCFm] array in the descending order...
//selection sort
void re_sort_CFMax (void)
{
  int i, j;
  double max;
  int maxindex;
  CFstr temp;

  for (i=0; i<NCFm; i++) {
    max = -1;
    for (j=i; j<NCFm; j++) 
      if (max<CFmax[j].cf) {
        max=CFmax[j].cf;
        maxindex=j;
      }
    
    //swap max and i
    temp = CFmax[i];
    CFmax[i] = CFmax[maxindex];
    CFmax[maxindex] = temp;
  }
}

//subpixel CFSmax
void re_sort_CFSMax (void)
{
  int i, j;
  double max;
  int maxindex;
  CFSstr temp;

  for (i=0; i<NCFm; i++) {
    max = -1;
    for (j=i; j<NCFm; j++) 
      if (max<CFSmax[j].cf) {
        max=CFSmax[j].cf;
        maxindex=j;
      }
    
    //swap max and i
    temp = CFSmax[i];
    CFSmax[i] = CFSmax[maxindex];
    CFSmax[maxindex] = temp;
  }
}


/////////////////////////////////////////////////////////////////////
// Fixed Resolution Pyramid DPNCS
/////////////////////////////////////////////////////////////////////
// Fixed Resolution Pyramid Method:
// 1) determine the resolution layers needed by pattern image size:
//   1) one layer:   1x1 ~ 11x11
//   2) two layers:   12x12 ~ 23x23
//   3) three layers: 24x24 ~ 47x47
//   4) four layers:  48x48 ~ 95x95
//   5) five layers:  96x96 ~ 191x191
//   6) six layers:   192x192 ~ 383x383
//   7) seven layers: 384x384 -
//
// 2) initilize the search image sim in a continuous memory
// 3) sub-sampling the subsim[] and subpim[]
// 4) allocate the dynamic programming table for subim[0], subim[0]^2,
//   and the correlation function table: double CF[y][x].
//   The other layers use table of subim[0].
// 5) construct the DP table for the coarse search:
//   subsim[RLayer-1], subsim[RLayer-1]^2
//   and pattern SUM SUM M,M^2
// 6) compute the CF table for the coarse search
//   in the range (0,0)-(subsh-subph+1,subsw-subpw+1)
//
// 7) switch SearchOption, use FindMax or SelectSort
//   results in matchX, matchY, matchCF,
//      matX[20], matY[20], matCF[20]
//   7-1) COARSE_ONLY:
//      coarse search only, (X,Y) in 2 power of coarse search
//   7-2) SEARCH_ONE_FAST:
//        find the max CF to fine search
//   7-3) SEARCH_ONE:
//        find max 3 CF to fine search, and get the largest CF
//   7-4) SEARCH_MN:
//        dSearchMinCF=0.2, nCoarseTarget=5, nSearchTarget=3
//   7-5) SEARCH_MC:
//        dSearchMinCF=0.2, nCoarseTarget=5, dSearchMinCF=0.8
//   7-6) AUTO_SEARCH:
//        the same as SEARCH_MC
//
//
// 8) free the allocated memory and images

//Image* sim;
//unsigned int **DPsim, **DPSsim;
//double **CF;
//double matchCF;
//int matchX, matchY; //match position in search image
int RLayer;
//int matX[20], matY[20]; //match positions
//double matCF[20];   //match CFs

//!!! GLOBAL TO FIX THE BUGG !!!
int dpwidth, dpheight;   //dynamic programming table size
int subsw, subsh, subpw, subph;
int submtX, submtY, tmtX, tmtY;
double submtCF;
Image* subsim[15];   //sub-sampling image, subsim[0]=sim
Image* subpim[15];   //sub-sampling pattern, subpim[0]=pim

bool RPDPNCS (Image* pim, Image* image, int sx1, int sy1, int sx2, int sy2)
{
   unsigned char *s, *d;   //source, destination
   int sw = sx2-sx1+1;   //search width
   int sh = sy2-sy1+1;   //search height
   int pw = pim->width;   //pattern width
   int ph = pim->height;   //pattern height
   int min;
   int x, y, t, i, j, k;
   unsigned char *ss, *sp;  //source search image, source pattern image
   int  sx, sy;
   unsigned int PM, PMS;
   int N;
   unsigned int DPs, DPSs;
   unsigned int sumpx;   //sum 2 for loop in 4
   double tn; //numerator, must in double
   double td; //denominator, must in double

   //
   // 1) determine the resolution layers needed by pattern image size:
   //
   if (pw<ph) min=pw;
   else   min=ph;

   if (min<12) RLayer=1;
   else if (min<24) RLayer=2;
   else if (min<48) RLayer=3;
   else if (min<96) RLayer=4;
   else if (min<192) RLayer=5;
   else if (min<384) RLayer=6;
   else RLayer=7;
   //int psize = pw*ph;
   //if (psize<256) RLayer=1;
   //else if (psize<1023) RLayer=2;
   //else if (psize<4095) RLayer=3;
   //else if (psize<16383) RLayer=4;
   //else if (psize<65535) RLayer=5;
   //else RLayer=6;

   //
   // 2) initilize the search image subim[0] in a continuous memory
   //
   subsim[0] = new Image (sw, sh);
   d = (unsigned char*) subsim[0]->imagedata;
   for (y=sy1; y<=sy2; y++) {
      s = B_PIX_REF(image, sx1, y);
      memcpy (d, s, sw);
      d += sw;
   }
   //putimage (bx, by, subsim[0]->image, COPY_PUT);

   //
   // 3) sub-sampling the subsim[] and subpim[]
   //
   subpim[0]=pim;
   for (i=1; i<RLayer; i++) {
      subsim[i] = sub2sampling(subsim[i-1]);
      subpim[i] = sub2sampling(subpim[i-1]);
   //putimage (bx+200*i, by, subsim[i]->image, COPY_PUT);
   //putimage (bx+200*i, by+200, subpim[i]->image, COPY_PUT);
   }

   //
   // 4) allocate the dynamic programming table for sim, sim^2, and CF
   //
   //DPsim[y+1][x+1], DPSsim[y+1][x+1], CF[y+1][x+1]
   dpwidth  = sw+1;
   dpheight = sh+1;
   //replace sh+1 with dpwidth
   //replace sw+1 with dpheight
   try {
      DPsim = new unsigned int*[dpheight];
      DPSsim = new unsigned int*[dpheight];
      //CF = new double*[dpheight];
      for (y=0; y<dpheight; y++) {
         DPsim[y] = new unsigned int[dpwidth];
         DPSsim[y] = new unsigned int[dpwidth];
         //CF[y] = new double[dpwidth];
      }
   }
   catch ( ... ) {
      vcl_cout << "Memory not enough !"; exit(1);
   }
   //First row and column set to zero...
   for (y=0; y<dpheight; y++) {
      DPsim[y][0]=0;
      DPSsim[y][0]=0;
   }
   for (x=1; x<dpwidth; x++) {
      DPsim[0][x]=0;
      DPSsim[0][x]=0;
   }

   //
   // 5) construct the DP table for the coarse search:
   //   subsim[RLayer-1], subsim[RLayer-1]^2
   //   and pattern SUM SUM M,M^2
   //
   //Fill the table...
   //d = (unsigned char*) subsim[RLayer-1]->imagedata;
   subsw = subsim[RLayer-1]->width;
   subsh = subsim[RLayer-1]->height;
   for (y=1; y<subsh+1; y++) {
      d = B_PIX_REF(subsim[RLayer-1], 0, y-1);
      for (x=1; x<subsw+1; x++) {
         t = (unsigned int) (*d);
         DPsim[y][x] = DPsim[y-1][x]   + DPsim[y][x-1] -
                       DPsim[y-1][x-1] + t;
         DPSsim[y][x]= DPSsim[y-1][x]  + DPSsim[y][x-1] -
                   DPSsim[y-1][x-1] + (t*t);
       d++;
      }
   }
   subpw = subpim[RLayer-1]->width;
   subph = subpim[RLayer-1]->height;
   PM=0; PMS=0;
   for (y=0; y<subph; y++) {
      d = B_PIX_REF(subpim[RLayer-1], 0, y);
      for (x=0; x<subpw; x++) {
         t = (*d);
         PM += t;
         PMS += t*t;
         d++;
      }
   }
   //writefileintblock ("DPsim", DPsim, subsw+1, subsh+1);
   //writefileintblock ("DPSsim", DPSsim, subsw+1, subsh+1);
   //writefilecharblock ("SUBPIM", subpim[RLayer-1]->imagedata, subpw, subph);
   //writefilecharblock ("SUBSIM", subsim[RLayer-1]->imagedata, subsw, subsh);
   //showMsg (RED, "subsw=%d subsh=%d subpw=%d subph=%d PM=%d PMS=%d",
   //    subsw, subsh, subpw, subph, PM, PMS); getch();
 
   //
   // 6) compute the CF table for the coarse search
   //   in the range (0,0)-(subsh-subph+1,subsw-subpw+1)
   //
   N=subph*subpw;
   for (sy=0; sy<subsh-subph+1; sy++)
      for (sx=0; sx<subsw-subpw+1; sx++) {
         DPs  = DPsim[sy+subph][sx+subpw]  - DPsim[sy+subph][sx] -
                DPsim[sy][sx+subpw]        + DPsim[sy][sx];
         DPSs = DPSsim[sy+subph][sx+subpw] - DPSsim[sy+subph][sx] -
                DPSsim[sy][sx+subpw]       + DPSsim[sy][sx];
         sumpx=0;
     sp = (unsigned char*) subpim[RLayer-1]->imagedata;
     ss = B_PIX_REF(subsim[RLayer-1], sx, sy);
         for (y=0; y<subph; y++) {
           
            for (x=0; x<subpw; x++) {
               sumpx += (*ss)*(*sp);
               ss++; sp++;
            }
      ss += (subsw-subpw); //CODE1
         }
         tn = (double)((double)N*sumpx - (double)DPs*PM);
         if (tn>0) tn=tn*tn;
         else tn=0;

         td = (double)((double)N*DPSs - (double)DPs*DPs)*
                      ((double)N*PMS - (double)PM*PM);
         if (tn==0.0 && td==0.0) CF[sy][sx] = 0.0;
         else CF[sy][sx] = (double) tn/td;
         //putpixel (sx,150+sy,LIGHTRED);
         //showMsg (RED, "sy=%d sx=%d DPs=%d DPSs=%d sumpx=%d",
         //    sy, sx, DPs, DPSs, sumpx); getch();
         //showMsg (RED, "sy=%d sx=%d tn=%f td=%f", sy, sx, tn, td); getch();
      }

  // 7) switch SearchOption, use FindMax or SelectSort
  //    results in matchX, matchY, matchCF,
  //      CFMax[0..NCFm].x, y, cf
  switch (SearchOption) {
    case COARSE_ONLY:
      matchCF=-1;
      for (sy=0; sy<subsh-subph+1; sy++)
        for (sx=0; sx<subsw-subpw+1; sx++)
          if (matchCF < CF[sy][sx]) {
            matchCF=CF[sy][sx];
            matchX=sx; matchY=sy;
          }
      //if (bDebug) {
          //   putimage (0, 0, subsim[RLayer-1]->image, COPY_PUT);
      //   putimage (subsw+10,0, subpim[RLayer-1]->image, COPY_PUT);
      //   putpixel (matchX, matchY, LIGHTRED);
      //}
      //if (bShowCFImage) ShowCFImage (subsw-subpw+1, subsh-subph+1);
      matchX = matchX << (RLayer-1);
      matchY = matchY << (RLayer-1);
      break;
    case SEARCH_ONE_FAST:
      submtCF=-1;
      for (sy=0; sy<subsh-subph+1; sy++)
              for (sx=0; sx<subsw-subpw+1; sx++)
          if (submtCF < CF[sy][sx]) {
            submtCF=CF[sy][sx];
            submtX=sx; submtY=sy;
          }
      //if (bDebug) {
        //  putimage (0, 0, subsim[RLayer-1]->image, COPY_PUT);
        //  putimage (subsw+10,0, subpim[RLayer-1]->image, COPY_PUT);
        //   putpixel (submtX, submtY, LIGHTRED);
      //}
      //if (bShowCFImage) ShowCFImage (subsw-subpw+1, subsh-subph+1);

      //for each subsampling layer, small to large
      for (i=RLayer-2; i>=0; i--) {
              submtX = submtX << 1;
        submtY = submtY << 1;
        subsw = subpim[i]->width+2; //subsim[i]->width;
        subsh = subpim[i]->height+2;//subsim[i]->height;
        /////BUGG!,GLOBAL TO FIX!///////////////////////////////////////
        //showMsg (RED, "submtX=%d submtY=%d", submtX, submtY); getch();
        for (y=1; y<subsh+1; y++) {
          d = B_PIX_REF(subsim[i], submtX-1, submtY-1+y-1); //0, y-1
          for (x=1; x<subsw+1; x++) {
            t = (unsigned int) (*d);
            DPsim[y][x] = DPsim[y-1][x] + DPsim[y][x-1] -
                    DPsim[y-1][x-1] + t;
            DPSsim[y][x]= DPSsim[y-1][x] + DPSsim[y][x-1] -
                    DPSsim[y-1][x-1] + (t*t);
            d++;
          }
        }
        subpw = subpim[i]->width;
        subph = subpim[i]->height;
        PM=0; PMS=0;
        for (y=0; y<subph; y++) {
          d = B_PIX_REF(subpim[i], 0, y);
          for (x=0; x<subpw; x++) {
            t = (*d);
            PM += t;
            PMS += t*t;
            d++;
          }
        }
        N=subph*subpw;
        for (sy=0; sy<subsh-subph+1; sy++) //3
          for (sx=0; sx<subsw-subpw+1; sx++) { //3
            sp = (unsigned char*) subpim[i]->imagedata;
            DPs  = DPsim[sy+subph][sx+subpw]  - DPsim[sy+subph][sx] -
                 DPsim[sy][sx+subpw]        + DPsim[sy][sx];
            DPSs = DPSsim[sy+subph][sx+subpw] - DPSsim[sy+subph][sx] -
                 DPSsim[sy][sx+subpw]              + DPSsim[sy][sx];
            sumpx=0;
            for (y=0; y<subph; y++) {
              ss = B_PIX_REF(subsim[i], submtX-1+sx, submtY-1+sy+y); //sx, sy+y);
              for (x=0; x<subpw; x++) {
                sumpx += (*ss)*(*sp);
                ss++; sp++;
              }
            }
            tn = (double)((double)N*sumpx - (double)DPs*PM);
            if (tn>0) tn=tn*tn;
            else tn=0;

            td = (double)((double)N*DPSs - (double)DPs*DPs)*
                   ((double)N*PMS - (double)PM*PM);
            //AVOID BUGG!!
            if (tn==0.0 && td==0.0) CF[sy][sx] = 0.0;
            else CF[sy][sx] = (double) tn/td;
            //putpixel (sx,150+sy,LIGHTRED);
            //showMsg (RED, "sy=%d sx=%d DPs=%d DPSs=%d sumpx=%d",
            //    sy, sx, DPs, DPSs, sumpx); getch();
            //showMsg (RED, "sy=%d sx=%d tn=%f td=%f", sy, sx, tn, td); getch();
            //showMsg (RED, "CF[%d][%d]=%1.3f", sy,sx,CF[sy][sx]); getch();
          }
        submtCF=-1;
        for (sy=0; sy<subsh-subph+1; sy++)
          for (sx=0; sx<subsw-subpw+1; sx++)
            if (submtCF < CF[sy][sx]) {
              submtCF=CF[sy][sx];
              tmtX=sx; tmtY=sy;
            }
        submtX=submtX+tmtX-1;
        submtY=submtY+tmtY-1;
        //if (bDebug) {
        //   putimage (0, 0, subsim[i]->image, COPY_PUT);
        //   putimage (subsim[i]->width+10,0, subpim[i]->image, COPY_PUT);
        //   putpixel (submtX, submtY, LIGHTRED);
        //   ShowCFImage (subsw-subpw+1, subsh-subph+1);
        //   getch();
        //}
      } //end for each layer
      matchX = MAX(0,submtX); //submtX, avoid (0,0) (-1,-1) bug
      matchY = MAX(0,submtY); //submtY  avoid (0,0) (-1,-1) bug
      matchCF=submtCF;
    break;

    case SEARCH_ONE:
    case SEARCH_MN:
    case SEARCH_MC:
    case AUTO_SEARCH:
      CFSelSort (dCoarseMinCF, nCoarseTarget, subsw-subpw+1, subsh-subph+1);

      //for each target...
      for (j=0; j<NCFm; j++) { 
        submtX = CFmax[j].x;
        submtY = CFmax[j].y;

        //for each subsampling layer, small to large
        for (i=RLayer-2; i>=0; i--) {
          submtX = submtX << 1;
          submtY = submtY << 1;
          subsw = subpim[i]->width+2; //subsim[i]->width;
          subsh = subpim[i]->height+2;//subsim[i]->height;
            
          for (y=1; y<subsh+1; y++) {
            d = B_PIX_REF(subsim[i], submtX-1, submtY-1+y-1); //0, y-1
            for (x=1; x<subsw+1; x++) {
              t = (unsigned int) (*d);
              DPsim[y][x] = DPsim[y-1][x] + DPsim[y][x-1] -
                      DPsim[y-1][x-1] + t;
              DPSsim[y][x]= DPSsim[y-1][x] + DPSsim[y][x-1] -
                      DPSsim[y-1][x-1] + (t*t);
              d++;
            }
          }
          subpw = subpim[i]->width;
          subph = subpim[i]->height;
          PM=0; PMS=0;
          for (y=0; y<subph; y++) {
            d = B_PIX_REF(subpim[i], 0, y);
            for (x=0; x<subpw; x++) {
              t = (*d);
              PM += t;
              PMS += t*t;
              d++;
            }
          }
          N=subph*subpw;
          for (sy=0; sy<subsh-subph+1; sy++) //3
            for (sx=0; sx<subsw-subpw+1; sx++) { //3
              sp = (unsigned char*) subpim[i]->imagedata;
              DPs  = DPsim[sy+subph][sx+subpw]  - DPsim[sy+subph][sx] -
                   DPsim[sy][sx+subpw]        + DPsim[sy][sx];
              DPSs = DPSsim[sy+subph][sx+subpw] - DPSsim[sy+subph][sx] -
                   DPSsim[sy][sx+subpw]              + DPSsim[sy][sx];
              sumpx=0;
              for (y=0; y<subph; y++) {
                ss = B_PIX_REF(subsim[i], submtX-1+sx, submtY-1+sy+y); //sx, sy+y);
                for (x=0; x<subpw; x++) {
                  sumpx += (*ss)*(*sp);
                  ss++; sp++;
                }
              }
              tn = (double)((double)N*sumpx - (double)DPs*PM);
              if (tn>0) tn=tn*tn;
              else tn=0;

              td = (double)((double)N*DPSs - (double)DPs*DPs)*
                     ((double)N*PMS - (double)PM*PM);
              //AVOID BUGG!!
              if (tn==0.0 && td==0.0) CF[sy][sx] = 0.0;
              else CF[sy][sx] = (double) tn/td;
            }
          submtCF=-1;
          for (sy=0; sy<subsh-subph+1; sy++)
            for (sx=0; sx<subsw-subpw+1; sx++)
              if (submtCF < CF[sy][sx]) {
                submtCF=CF[sy][sx];
                tmtX=sx; tmtY=sy;
              }
          submtX=submtX+tmtX-1;
          submtY=submtY+tmtY-1;
        } //end for each layer

        CFmax[j].x  = MAX(0,submtX);
        CFmax[j].y  = MAX(0,submtY);
        CFmax[j].cf = submtCF;

      } //end for each target

      //re-sort the CFMax[0..NCFm] array in the descending order...
      re_sort_CFMax ();

      //////////////////////////////////////////////////////////////
      // Multiple Search...
      switch (SearchOption) { 
        case SEARCH_ONE: //do nothing! output: matchX, matchY, matchCF...
          break;
        case SEARCH_MN:
          NCFm = nSearchTarget; //trim
          break;
        case SEARCH_MC:
        case AUTO_SEARCH: //trim at dSearchMinCF...
          min=0;
          for (k=0; k<NCFm; k++) {
            if (CFmax[k].cf>dSearchMinCF) 
              min=k;
            else
              break;
          }
          NCFm = min;
          break;
      }

      matchX = CFmax[0].x;   //the most matching one
      matchY = CFmax[0].y;
      matchCF= CFmax[0].cf;
      break;
  } //end switch SearchOption

   //
   // 8) free the allocated memory and images...
   //
   for (y=0; y<dpheight; y++) {
      delete[] DPsim[y];
      delete[] DPSsim[y];
      //delete[] CF[y];
   }
   delete[] DPsim;
   delete[] DPSsim;
   //delete[] CF;

   delete subsim[0];
   for (i=1; i<RLayer; i++) {
      delete subsim[i];
      delete subpim[i];
   }
   //delete[] CFmax; static global

   return true;
}


/////////////////////////////////////////////////////////////////////
// SUBPIXEL SEARCH...
/////////////////////////////////////////////////////////////////////
// Subpixel Fixed Resolution Pyramid Method:
// 1) determine the resolution layers needed by pattern image size:
//   1) one layer:   1x1 ~ 11x11
//   2) two layers:   12x12 ~ 23x23
//   3) three layers: 24x24 ~ 47x47
//   4) four layers:  48x48 ~ 95x95
//   5) five layers:  96x96 ~ 191x191
//   6) six layers:   192x192 ~ 383x383
//   7) seven layers: 384x384 -
// 1') determine the BASE
//
// 2) initilize the search image sim in a continuous memory
// 3) sub-sampling the subsim[] and subpim[]
// 3')over-sampling the subpixel layer
// 4) allocate the dynamic programming table for subim[0], subim[0]^2,
//   and the correlation function table: double CF[y][x].
//   The other layers use table of subim[0].
//   DP table size:
//   dpwidth  = max(sw, (pw*2^BASE)+3),
//   dpheight = max(sh, (ph*2^BASE)+3)
//   subpim,subsim[RLayer+BASE-1]    : coarse search
//   subpim,subsim[RLayer+BASE-2] ~ [BASE]: fine search
//   subpim,subsim[BASE-1] ~ [0]    : subpixel search
//
// 5) construct the DP table for the coarse search:
//   subsim[RLayer-1], subsim[RLayer-1]^2
//   and pattern SUM SUM M,M^2
// 6) compute the CF table for the coarse search
//   in the range (0,0)-(subsh-subph+1,subsw-subpw+1)
//
// 7) switch SearchOption, use FindMax or SelectSort
//   results in matchX, matchY, matchCF,
//      matX[20], matY[20], matCF[20]
//   7-1) COARSE_ONLY:
//      coarse search only, (X,Y) in 2 power of coarse search
//   7-2) SEARCH_ONE_FAST:
//        find the max CF to fine search
//   7-3) SEARCH_ONE:
//        find max 3 CF to fine search, and get the largest CF
//   7-4) SEARCH_MN:
//        dSearchMinCF=0.2, nCoarseTarget=5, nSearchTarget=3
//   7-5) SEARCH_MC:
//        dSearchMinCF=0.2, nCoarseTarget=5, dSearchMinCF=0.8
//   7-6) AUTO_SEARCH:
//        the same as SEARCH_MC
//
//
// 8) free the allocated memory and images

////Image* sim;
////unsigned int **DPsim, **DPSsim;
////double **CF;
////double matchCF;
////int matchX, matchY; //match position in search image
//int RLayer;
//int matX[20], matY[20]; //match positions
//double matCF[20];   //match CFs

//!!! GLOBAL TO FIX THE BUGG !!!
//int subsw, subsh, subpw, subph;
//int submtX, submtY, tmtX, tmtY;
//double submtCF;
//Image* subsim[15];   //sub-sampling image, subsim[0]=sim
//Image* subpim[15];   //sub-sampling pattern, subpim[0]=pim

bool SRPDPNCS (Image* pim, Image* image, int sx1, int sy1, int sx2, int sy2)
{ 
  unsigned char *s, *d;   //source, destination
  int sw = sx2-sx1+1;   //search width
  int sh = sy2-sy1+1;   //search height
  int pw = pim->width;   //pattern width
  int ph = pim->height;   //pattern height
  int min;
  int x, y, t, i, j, k;
  unsigned char *ss, *sp;  //source search image, source pattern image
  int  sx, sy;
  unsigned int PM, PMS;
  int N;
  unsigned int DPs, DPSs;
  unsigned int sumpx;   //sum 2 for loop in 4
  double tn; //numerator, must in double
  double td; //denominator, must in double

  //
  // 1) determine the resolution layers needed by pattern image size:
  //
  if (pw<ph) min=pw;
  else   min=ph;

  if (min<12) RLayer=1;
  else if (min<24) RLayer=2;
  else if (min<48) RLayer=3;
  else if (min<96) RLayer=4;
  else if (min<192) RLayer=5;
  else if (min<384) RLayer=6;
  else RLayer=7;

  // 1') determine the BASE
  switch (SearchSubPixel) {
    case PIXEL_LEVEL:    BASE=0; break;
    case SUBPIXEL_LEVEL:  BASE=2; break;
    default:        BASE=0; break;
  }

  //
  // 2) initilize the search image subim[BASE] in a continuous memory
  //
  subsim[BASE] = new Image (sw, sh);
  d = (unsigned char*) subsim[BASE]->imagedata;
  for (y=sy1; y<=sy2; y++) {
    s = B_PIX_REF(image, sx1, y);
    memcpy (d, s, sw);
    d += sw;
  }
  
  //
  // 3) sub-sampling the subsim[] and subpim[]
  //
  subpim[BASE]=pim;
  for (i=BASE+1; i<BASE+RLayer; i++) {
    subsim[i] = sub2sampling(subsim[i-1]);
    subpim[i] = sub2sampling(subpim[i-1]);
  }
  // 3')over-sampling the subpixel layer
  for (i=BASE; i>0; i--) {
    subsim[i-1] = over2sampling(subsim[i]);
    subpim[i-1] = over2sampling(subpim[i]);
  }

  //
  // 4) allocate the dynamic programming table for sim, sim^2, and CF
  //
  // DP table size:
  //   dpwidth  = max(sw, (pw*2^BASE)+3),
  //   dpheight = max(sh, (ph*2^BASE)+3)
  //DPsim[y+1][x+1], DPSsim[y+1][x+1], CF[y+1][x+1]
  dpwidth  = MAX(sw, (int)((pw*pow(2,BASE))+3))+1;
  dpheight = MAX(sh, (int)((ph*pow(2,BASE))+3))+1;
  //showMsg (RED, "dpw=%d, dph=%d", dpwidth, dpheight); getch();
  //replace sh+1 with dpwidth
  //replace sw+1 with dpheight
  try {
    DPsim = new unsigned int*[dpheight];
    DPSsim = new unsigned int*[dpheight];
    //CF = new double*[dpheight];
    for (y=0; y<dpheight; y++) {
      DPsim[y] = new unsigned int[dpwidth];
      DPSsim[y] = new unsigned int[dpwidth];
      //CF[y] = new double[dpwidth];
    }
  }
  catch ( ... ) {
    //closegraph (); vcl_cout << "Memory not enough !"; exit(1);
  }
  //First row and column set to zero...
  for (y=0; y<dpheight; y++) {
    DPsim[y][0]=0;
    DPSsim[y][0]=0;
  }
  for (x=1; x<dpwidth; x++) {
    DPsim[0][x]=0;
    DPSsim[0][x]=0;
  }

  //
  // 5) construct the DP table for the coarse search:
  //   subsim[RLayer+BASE-1], subsim[RLayer+BASE-1]^2
  //   and pattern SUM SUM M,M^2
  //
  //Fill the table...
  //d = (unsigned char*) subsim[RLayer+BASE-1]->imagedata;
  subsw = subsim[RLayer+BASE-1]->width;
  subsh = subsim[RLayer+BASE-1]->height;
  for (y=1; y<subsh+1; y++) {
    d = B_PIX_REF(subsim[RLayer+BASE-1], 0, y-1);
    for (x=1; x<subsw+1; x++) {
      t = (unsigned int) (*d);
      DPsim[y][x] = DPsim[y-1][x] + DPsim[y][x-1] -
              DPsim[y-1][x-1] + t;
      DPSsim[y][x]= DPSsim[y-1][x] + DPSsim[y][x-1] -
              DPSsim[y-1][x-1] + (t*t);
      d++;
    }
  }
  subpw = subpim[RLayer+BASE-1]->width;
  subph = subpim[RLayer+BASE-1]->height;
  PM=0; PMS=0;
  for (y=0; y<subph; y++) {
    d = B_PIX_REF(subpim[RLayer+BASE-1], 0, y);
    for (x=0; x<subpw; x++) {
      t = (*d);
      PM += t;
      PMS += t*t;
      d++;
    }
  }

  //
  // 6) compute the CF table for the coarse search
  //   in the range (0,0)-(subsh-subph+1,subsw-subpw+1)
  //
  N=subph*subpw;
  for (sy=0; sy<subsh-subph+1; sy++)
    for (sx=0; sx<subsw-subpw+1; sx++) {
      sp = (unsigned char*) subpim[RLayer+BASE-1]->imagedata;
      DPs  = DPsim[sy+subph][sx+subpw]  - DPsim[sy+subph][sx] -
           DPsim[sy][sx+subpw]   + DPsim[sy][sx];
      DPSs = DPSsim[sy+subph][sx+subpw] - DPSsim[sy+subph][sx] -
           DPSsim[sy][sx+subpw]   + DPSsim[sy][sx];
      sumpx=0;
      for (y=0; y<subph; y++) {
        ss = B_PIX_REF(subsim[RLayer+BASE-1], sx, sy+y);
        for (x=0; x<subpw; x++) {
          sumpx += (*ss)*(*sp);
          ss++; sp++;
        }
      }
    tn = (double)((double)N*sumpx - (double)DPs*PM);
    if (tn>0) tn=tn*tn;
    else tn=0;

    td = (double)((double)N*DPSs - (double)DPs*DPs)*
           ((double)N*PMS - (double)PM*PM);
    if (tn==0.0 && td==0.0) CF[sy][sx] = 0.0;
    else CF[sy][sx] = (double) tn/td;
  }

  // 7) switch SearchOption, use FindMax or SelectSort
  //    results in matchX, matchY, matchCF,
  //      CFMax[0..NCFm].x, y, cf
  switch (SearchOption) {
    case COARSE_ONLY:
      matchCF=-1;
      for (sy=0; sy<subsh-subph+1; sy++)
        for (sx=0; sx<subsw-subpw+1; sx++)
          if (matchCF < CF[sy][sx]) {
            matchCF=CF[sy][sx];
            matchX=sx; matchY=sy;
          }
      matchX = matchX << (RLayer-1);
      matchY = matchY << (RLayer-1);
      break;
    case SEARCH_ONE_FAST:
      submtCF=-1;
      for (sy=0; sy<subsh-subph+1; sy++)
        for (sx=0; sx<subsw-subpw+1; sx++)
          if (submtCF < CF[sy][sx]) {
            submtCF=CF[sy][sx];
            submtX=sx; submtY=sy;
          }
   
      //for each subsampling layer, small to large
      //[BASE-1,BASE-2,...,0]: subpixel layer
      for (i=RLayer+BASE-2; i>=0; i--) {
        submtX = submtX << 1;
        submtY = submtY << 1;
        subsw = subpim[i]->width+2; //subsim[i]->width;
        subsh = subpim[i]->height+2;//subsim[i]->height;
     
        for (y=1; y<subsh+1; y++) {
          d = B_PIX_REF(subsim[i], submtX-1, submtY-1+y-1); //0, y-1
          for (x=1; x<subsw+1; x++) {
            t = (unsigned int) (*d);
            DPsim[y][x] = DPsim[y-1][x] + DPsim[y][x-1] -
                    DPsim[y-1][x-1] + t;
            DPSsim[y][x]= DPSsim[y-1][x] + DPSsim[y][x-1] -
                    DPSsim[y-1][x-1] + (t*t);
            d++;
          }
        }
        subpw = subpim[i]->width;
        subph = subpim[i]->height;
        PM=0; PMS=0;
        for (y=0; y<subph; y++) {
          d = B_PIX_REF(subpim[i], 0, y);
          for (x=0; x<subpw; x++) {
            t = (*d);
            PM += t;
            PMS += t*t;
            d++;
          }
        }
        N=subph*subpw;
        for (sy=0; sy<subsh-subph+1; sy++) //3
          for (sx=0; sx<subsw-subpw+1; sx++) { //3
            sp = (unsigned char*) subpim[i]->imagedata;
            DPs  = DPsim[sy+subph][sx+subpw]  - DPsim[sy+subph][sx] -
            DPsim[sy][sx+subpw]   + DPsim[sy][sx];
            DPSs = DPSsim[sy+subph][sx+subpw] - DPSsim[sy+subph][sx] -
            DPSsim[sy][sx+subpw]   + DPSsim[sy][sx];
            sumpx=0;
            for (y=0; y<subph; y++) {
              ss = B_PIX_REF(subsim[i], submtX-1+sx, submtY-1+sy+y); //sx, sy+y);
              for (x=0; x<subpw; x++) {
                sumpx += (*ss)*(*sp);
                ss++; sp++;
              }
            }
            tn = (double)((double)N*sumpx - (double)DPs*PM);
            if (tn>0) tn=tn*tn;
            else tn=0;

            td = (double)((double)N*DPSs - (double)DPs*DPs)*
                   ((double)N*PMS - (double)PM*PM);
            //AVOID BUGG!!
            if (tn==0.0 && td==0.0) CF[sy][sx] = 0.0;
            else CF[sy][sx] = (double) tn/td;
          }
        submtCF=-1;
        for (sy=0; sy<subsh-subph+1; sy++)
          for (sx=0; sx<subsw-subpw+1; sx++)
            if (submtCF < CF[sy][sx]) {
              submtCF=CF[sy][sx];
              tmtX=sx; tmtY=sy;
            }
        submtX=submtX+tmtX-1;
        submtY=submtY+tmtY-1;
      } //end for each layer
      matchX = MAX(0,submtX); //submtX, avoid (0,0) (-1,-1) bug
      matchY = MAX(0,submtY); //submtY  avoid (0,0) (-1,-1) bug
      matchCF=submtCF;
      smatchX = ((double)matchX)/pow(2,BASE); //subpixel
      smatchY = ((double)matchY)/pow(2,BASE);
      matchX = matchX >> BASE;
      matchY = matchY >> BASE;
      break;
    case SEARCH_ONE:
    case SEARCH_MN:
    case SEARCH_MC:
    case AUTO_SEARCH:
      CFSelSort (dCoarseMinCF, nCoarseTarget, subsw-subpw+1, subsh-subph+1);

      //for each target...
      for (j=0; j<NCFm; j++) { 
        submtX = CFmax[j].x;
        submtY = CFmax[j].y;

        //for each subsampling layer, small to large
        //[BASE-1,BASE-2,...,0]: subpixel layer
        for (i=RLayer+BASE-2; i>=0; i--) {
          submtX = submtX << 1;
          submtY = submtY << 1;
          subsw = subpim[i]->width+2; //subsim[i]->width;
          subsh = subpim[i]->height+2;//subsim[i]->height;
            
          for (y=1; y<subsh+1; y++) {
            d = B_PIX_REF(subsim[i], submtX-1, submtY-1+y-1); //0, y-1
            for (x=1; x<subsw+1; x++) {
              t = (unsigned int) (*d);
              DPsim[y][x] = DPsim[y-1][x] + DPsim[y][x-1] -
                      DPsim[y-1][x-1] + t;
              DPSsim[y][x]= DPSsim[y-1][x] + DPSsim[y][x-1] -
                      DPSsim[y-1][x-1] + (t*t);
              d++;
            }
          }
          subpw = subpim[i]->width;
          subph = subpim[i]->height;
          PM=0; PMS=0;
          for (y=0; y<subph; y++) {
            d = B_PIX_REF(subpim[i], 0, y);
            for (x=0; x<subpw; x++) {
              t = (*d);
              PM += t;
              PMS += t*t;
              d++;
            }
          }
          N=subph*subpw;
          for (sy=0; sy<subsh-subph+1; sy++) //3
            for (sx=0; sx<subsw-subpw+1; sx++) { //3
              sp = (unsigned char*) subpim[i]->imagedata;
              DPs  = DPsim[sy+subph][sx+subpw]  - DPsim[sy+subph][sx] -
                   DPsim[sy][sx+subpw]        + DPsim[sy][sx];
              DPSs = DPSsim[sy+subph][sx+subpw] - DPSsim[sy+subph][sx] -
                   DPSsim[sy][sx+subpw]              + DPSsim[sy][sx];
              sumpx=0;
              for (y=0; y<subph; y++) {
                ss = B_PIX_REF(subsim[i], submtX-1+sx, submtY-1+sy+y); //sx, sy+y);
                for (x=0; x<subpw; x++) {
                  sumpx += (*ss)*(*sp);
                  ss++; sp++;
                }
              }
              tn = (double)((double)N*sumpx - (double)DPs*PM);
              if (tn>0) tn=tn*tn;
              else tn=0;

              td = (double)((double)N*DPSs - (double)DPs*DPs)*
                     ((double)N*PMS - (double)PM*PM);
              //AVOID BUGG!!
              if (tn==0.0 && td==0.0) CF[sy][sx] = 0.0;
              else CF[sy][sx] = (double) tn/td;
            }
          submtCF=-1;
          for (sy=0; sy<subsh-subph+1; sy++)
            for (sx=0; sx<subsw-subpw+1; sx++)
              if (submtCF < CF[sy][sx]) {
                submtCF=CF[sy][sx];
                tmtX=sx; tmtY=sy;
              }
          submtX=submtX+tmtX-1;
          submtY=submtY+tmtY-1;
        } //end for each layer

        CFmax[j].x   = MAX(0,submtX);
        CFmax[j].y   = MAX(0,submtY);
        CFmax[j].cf  = submtCF;

        CFSmax[j].x  = CFmax[j].x/pow(2,BASE);
        CFSmax[j].y  = CFmax[j].y/pow(2,BASE);
        CFSmax[j].cf = CFmax[j].cf/pow(2,BASE);

         CFmax[j].x   = CFmax[j].x >> BASE;
        CFmax[j].y   = CFmax[j].y >> BASE;
        
      } //end for each target

      //re-sort the CFMax[0..NCFm] array in the descending order...
      re_sort_CFMax ();
      re_sort_CFSMax ();

      //////////////////////////////////////////////////////////////
      // Multiple Search...
      switch (SearchOption) { 
        case SEARCH_ONE: //do nothing! output: matchX, matchY, matchCF...
          break;
        case SEARCH_MN:
          NCFm = nSearchTarget; //trim
          break;
        case SEARCH_MC:
        case AUTO_SEARCH: //trim at dSearchMinCF...
          min=0;
          for (k=0; k<NCFm; k++) {
            if (CFSmax[k].cf>dSearchMinCF)
              min=k;
            else
              break;
          }
          NCFm = min;
          break;
      }

      matchCF= CFSmax[0].cf;
      smatchX = CFSmax[0].x; //((double)matchX)/pow(2,BASE); //subpixel
      smatchY = CFSmax[0].y; //((double)matchY)/pow(2,BASE);
      matchX = CFmax[0].x;
      matchY = CFmax[0].y;
      break;
  } //end SearchOption

  //
  // 8) free the allocated memory and images...
  //
  for (y=0; y<dpheight; y++) {
    delete[] DPsim[y];
    delete[] DPSsim[y];
    //delete[] CF[y];
  }
  delete[] DPsim;
  delete[] DPSsim;
  //delete[] CF;

  delete subsim[BASE];
  for (i=BASE+1; i<BASE+RLayer; i++) {
    delete subsim[i];
    delete subpim[i];
  }
  for (i=BASE; i>0; i--) {
    delete subsim[i-1];
    delete subpim[i-1];
  }
  //delete[] CFmax; //for selsort

  return true;
}

void* getsubCF (void)
{
  return NULL;
}

int getsubCFX (void)
{
  return subsw;
}

int getsubCFY (void)
{
  return subsh;
}


/////////////////////////////////////////////////////////////////////
// AUTO_MODEL Function
/////////////////////////////////////////////////////////////////////
// 1) determine the resolution layers needed by model image size:
//   1) one layer:   1x1 ~ 11x11
//   2) two layers:   12x12 ~ 23x23
//   3) three layers: 24x24 ~ 47x47
//   4) four layers:  48x48 ~ 95x95
//   5) five layers:  96x96 ~ 191x191
//   6) six layers:   192x192 ~ 383x383
//   7) seven layers: 384x384 -
//
// 2) initilize the search image sim in a continuous memory
// 3) sub-sampling the subsim[] and subpim[]
//    for subpim: keep space only
//    (because the imagdata will change)
// 4) allocate the subPosScore[], subUniqueScore[] array,
//    size the same as subsim[RLayer-1]
// 5) find the position score at 3x3 neighbor,
//    full subsim local 3x3 search,
//    copy each imagedata to subpim[]
//    call RPDPNCS to search
//    subpim[RLayer-1] at subsim[RLayer-1]
//    getsubCF(): return all subsample CF table 3x3
//    position score := middleCF - max8CF
//
// 6) determine the nModel: # of Model candidate to reuturn
//    autoModelX[0..nModel-1], autoModelY[0..nModel-1]
//    nModelCandidate := nModel*3
// 7) sort the position score at the descending order
//    find the max nModelCandidate one to do the uniqueness test
//    ModelCandidateX[0..nModelCandidate]
//    ModelCandidateY[0..nModelCandidate]

// 8) uniqueness test: 
//    global search for each ModelCandidate
//    copy each imagedata to subpim[]
//    call RPDPNCS to search: subpim[RLayer-1] at subsim[RLayer-1]
//    SEARCHOPTION:  SEARCH_M_N_N, nSearchTarget=2, nCoarseTarget=5
//    uniqueness score := maxCF-secondCF
// 9) sort the uniqueness score at the descending order
//    find the max nModel one to return
//    autoModelX[0..nModel], autoModelY[0..nModel]
//
// 10) free the allocated memory and images

int nModel;
int autoModelX[10];
int autoModelY[10];

int nModelCandidate;
int ModelCandidateX[30];
int ModelCandidateY[30];

bool AutoModel (Image* sim, int sx1, int sy1, int sx2, int sy2,
        int pw, int ph, int n_target)
{
   unsigned char *s, *d;   //source, destination
   int sw = sx2-sx1+1;   //search width
   int sh = sy2-sy1+1;   //search height
   int min;
   int x, y, t, i;
   unsigned char *ss, *sp;  //source search image, source pattern image
   int  sx, sy;
   unsigned int PM, PMS;
   int N;
   unsigned int DPs, DPSs;
   unsigned int sumpx;   //sum 2 for loop in 4
   double tn; //numerator, must in double
   double td; //denominator, must in double

   //
   // 1) determine the resolution layers needed by pattern image size:
   //
   if (pw<ph) min=pw;
   else   min=ph;

   if (min<12) RLayer=1;
   else if (min<24) RLayer=2;
   else if (min<48) RLayer=3;
   else if (min<96) RLayer=4;
   else if (min<192) RLayer=5;
   else if (min<384) RLayer=6;
   else RLayer=7;
   
   //
   // 2) initilize the search image subim[0] in a continuous memory
   //
   subsim[0] = new Image (sw, sh);
   d = (unsigned char*) subsim[0]->imagedata;
   for (y=sy1; y<=sy2; y++) {
      s = B_PIX_REF(sim, sx1, y);
      memcpy (d, s, sw);
      d += sw;
   }
   
   //
   // 3) sub-sampling the subsim[] and subpim[]
   //
   //subpim[0]=pim;
   //for (i=1; i<RLayer; i++) {
   //   subsim[i] = sub2sampling(subsim[i-1]);
   //   subpim[i] = sub2sampling(subpim[i-1]);
   //}


   //
   // 10) free the allocated memory and images...
   //
   for (y=0; y<dpheight; y++) {
      delete[] DPsim[y];
      delete[] DPSsim[y];
      //delete[] CF[y];
   }
   delete[] DPsim;
   delete[] DPSsim;
   //delete[] CF;

   delete subsim[0];
   for (i=1; i<RLayer; i++) {
      delete subsim[i];
      delete subpim[i];
   }
   //delete[] CFmax; //for selsort

   return true;
}


//the best model
int getAutoModel1X (void)
{
  return autoModelX[0];
}

int getAutoModel1Y (void)
{
  return autoModelY[0];
}

//the second model
int getAutoModel2X (void)
{
  return autoModelX[1];
}

int getAutoModel2Y (void)
{
  return autoModelY[1];
}


/////////////////////////////////////////////////////////////////////
// Show Search Result (One or Multiple)...
/////////////////////////////////////////////////////////////////////
/*
void WShowSearch (CDC* pDC, int pw, int ph, CRect rangeRect)
{
  int i;
  CRect prect; //the match pattern rect
  CBrush pbrush0, pbrush1[3], rbrush;
  pbrush0.CreateSolidBrush(RGB(255,0,0)); //the most matching: red
  pbrush1[0].CreateSolidBrush(RGB(255,255,0)); //2nd: yellow
  pbrush1[1].CreateSolidBrush(RGB(0,255,255)); //3rd: cyan
  pbrush1[2].CreateSolidBrush(RGB(0,0,255)); //4th: blue
  rbrush.CreateSolidBrush(RGB(0,255,0));  //range

  switch (SearchOption) {
    case SEARCH_MN:
    case SEARCH_MC:
    case AUTO_SEARCH:
      for (i=1; i<NCFm; i++) { //for each other matching...
        //draw pattern rect
        prect.SetRect ((int)rangeRect.left + CFmax[i].x,
                 (int)rangeRect.top  + CFmax[i].y,
                 (int)rangeRect.left + CFmax[i].x + pw,
                 (int)rangeRect.top  + CFmax[i].y + ph); 
        if (i<3)
          pDC->FrameRect (prect, &pbrush1[i-1]);
        else
          pDC->FrameRect (prect, &pbrush1[2]);
      }
      //no break;
    case COARSE_ONLY:
    case SEARCH_ONE_FAST:
    case SEARCH_ONE:
      //draw pattern & range rect
      prect.SetRect ((int)rangeRect.left + matchX,
               (int)rangeRect.top  + matchY,
               (int)rangeRect.left + matchX + pw,
               (int)rangeRect.top  + matchY + ph); 
      pDC->FrameRect (prect, &pbrush0);
      pDC->FrameRect (rangeRect, &rbrush);
      break;
  }
}
  */

/////////////////////////////////////////////////////////////////////
// Test Search, Show Sub-sampling, and Show Pyramid
/////////////////////////////////////////////////////////////////////
/*
void ShowSubSampling (void)
{
  if (!bSearchTeach) {
   showMsg (RED, "Teach first!");
   return;
   }
   int x, y;

   clearIm (0);
   putimage (bx, by+20, patim->image, COPY_PUT);

   Image* sub2 = sub2sampling(patim);
   putimage (bx+200,by+20,sub2->image, COPY_PUT);

   Image* sub4 = sub2sampling(sub2);
   putimage (bx+300,by+20,sub4->image, COPY_PUT);

   Image* sub8 = sub2sampling(sub4);
   putimage (bx+350,by+20,sub8->image, COPY_PUT);

   Image* sub16 = sub2sampling(sub8);
   putimage (bx+400,by+20,sub16->image, COPY_PUT);

   for (y=0; y<patim->height; y+=2)
   for (x=0; x<patim->width; x+=2)
   putpixel (bx+200+x/2, by+220+y/2, B_PIX(patim,x,y));
   for (y=0; y<patim->height; y+=4)
   for (x=0; x<patim->width; x+=4)
   putpixel (bx+300+x/4, by+220+y/4, B_PIX(patim,x,y));
   for (y=0; y<patim->height; y+=8)
   for (x=0; x<patim->width; x+=8)
   putpixel (bx+350+x/8, by+220+y/8, B_PIX(patim,x,y));
   for (y=0; y<patim->height; y+=16)
    for (x=0; x<patim->width; x+=16)
   putpixel (bx+400+x/16, by+220+y/16, B_PIX(patim,x,y));

   setcolor (YELLOW);
  settextjustify(LEFT_TEXT, BOTTOM_TEXT);
   settextstyle(MAIN_WIN_FONT, HORIZ_DIR, 2);
   outtextxy (bx+20, by+450, "Show 5 layers subsampling...");
  settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
   outtextxy (bx+200, by+15, "average pixel value");
   outtextxy (bx+200, by+215, "left-top pixel only");

   showMsg (RED, "<%d,%d> <%d,%d> <%d,%d> <%d,%d> <%d,%d>",
     patim->width, patim->height, sub2->width, sub2->height,
     sub4->width, sub4->height, sub8->width, sub8->height,
     sub16->width, sub16->height);

   delete sub2, sub4, sub8, sub16;
}

//Show the resolution pyramid RPDPNCS use
//pattern image patim
void ShowPyramid (void)
{
   //
   // 1) check if taught
   //
   if (!bSearchTeach) {
   showMsg (RED, "Teach Search First!");
   return;
   }

   int sw = sSx2-sSx1+1;   //search width
   int sh = sSy2-sSy1+1;   //search height
   int pw = patim->width;   //pattern width
   int ph = patim->height;   //pattern height
   int psize;
   int x, y, i, h;
   char buf[24];

   int RLayer;
   Image* subsim[10];   //sub-sampling image, subsim[0]=sim
   Image* subpim[10];   //sub-sampling pattern, subpim[0]=pim

   clearIm (0);
   psize = pw*ph;
  if (psize<256) RLayer=1;
   else if (psize<1023) RLayer=2;
   else if (psize<4095) RLayer=3;
   else if (psize<16383) RLayer=4;
   else if (psize<65535) RLayer=5;
   else RLayer=6;

   //
   // 2) initilize the search image subim[0] in a continuous memory
   //
   subsim[0] = new Image (sw, sh);
   unsigned char* s;
   unsigned char* d = (unsigned char*) subsim[0]->imagedata;
   for (y=sSy1; y<=sSy2; y++) {
   s = B_PIX_REF(icim, 0, y);
   for (x=sSx1; x<=sSx2; x++, d++)
   *d = *(s+x);
   }

   //
   // 3) sub-sampling the subsim[] and subpim[]
   //
   setcolor (YELLOW);
   settextjustify(LEFT_TEXT, BOTTOM_TEXT);
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
   subpim[0]=patim;
   x=bx; y=by; h=sh/2;
   if (sw<256) { //if to large, emit the original one
   putimage (x, y+20, subsim[0]->image, COPY_PUT);
   sprintf (buf, "%dx%d", subsim[0]->width, subsim[0]->height);
   outtextxy (x, y+15, buf);
   putimage (x, y+40+sh, subpim[0]->image, COPY_PUT);
   sprintf (buf, "%dx%d", subpim[0]->width, subpim[0]->height);
   outtextxy (x, y+35+sh, buf);
   x += sw+20;
    h=sh;
   }
   for (i=1; i<RLayer; i++) {
   subsim[i] = sub2sampling(subsim[i-1]);
   subpim[i] = sub2sampling(subpim[i-1]);
   putimage (x, y+20, subsim[i]->image, COPY_PUT);
   sprintf (buf, "%dx%d", subsim[i]->width, subsim[i]->height);
   outtextxy (x, y+15, buf);
   putimage (x, y+40+h, subpim[i]->image, COPY_PUT);
   sprintf (buf, "%dx%d", subpim[i]->width, subpim[i]->height);
   outtextxy (x, y+35+h, buf);
   x += ((sw>>i)+20);
   }

   showMsg (BLUE, "%d layers pyramid", RLayer);

   delete subsim[0];
   for (i=1; i<RLayer; i++) {
   delete subsim[i];
   delete subpim[i];
   }
}

//This function must called during DPNCS() or RPDPNCS()
//or the CF[][] will be free and error will occcur
// 1) initilize the pseudo palette
// 2) show CF image
// 3) show the index color
// 4) re-load the system palette
void ShowCFImage (int width, int height)
{
   int x, y;

   // 1) initilize the pseudo palette
   vWritePal(0, 255, vPseudoPal);

   // 2) show CF image
   clearIm (0);
  showMsg (BLUE, "the CF image: %dx%d", width, height);
   for (y=0; y<height; y++)
   for (x=0; x<width; x++) {
   if (CF[y][x]<0) //16~63: 48
     putpixel (bx+sSx1+x, by+sSy1+y, (int)((CF[y][x]+1)*48+16));
   else //64~255: 240
     putpixel (bx+sSx1+x, by+sSy1+y, (int)(CF[y][x]*192+63));
   }
   // 3) show the index color
   for (y=16; y<256; y++) {
   setcolor (y);
   line (bx+600, by+y+30, bx+615, by+y+30);
   }
   setcolor (WHITE);
   settextjustify(LEFT_TEXT, CENTER_TEXT);
   settextstyle(DEFAULT_FONT, HORIZ_DIR, 1);
   outtextxy (bx+620, by+30+16, "-1");
   outtextxy (bx+620, by+30+63, "0");
   outtextxy (bx+620, by+30+255, "+1");

   if (bGrabScreen) GrabScreen ();
   else getch();

   // 4) re-load the system palette
  vWritePal(0, 255, vPal);
  showIm (icim);
}

void ShowOversampling (void)
{
   putimage (bx,by,patim->image,COPY_PUT);
   Image* t = over2sampling (patim);
   putimage (bx+100,by,t->image,COPY_PUT);
   Image* tt = over2sampling (t);
   putimage (bx+350,by,tt->image,COPY_PUT);
   Image* ttt = over2sampling (tt);
   putimage (bx,by+150,ttt->image,COPY_PUT);

  getch();
  delete t, tt, ttt;
}
  */
/*
void testDPNCS (void)
{
   if (!bSearchTeach) {
   showMsg (RED, "Teach Search First!");
   return;
   }

   showIm (icim);
   showMsg (BLUE, "test DPNCS, this may take a mean while...");
   t1 = biostime(0, 0L);
   int r=DPNCS (patim, icim, sSx1, sSy1, sSx2, sSy2);
   t2 = biostime(0, 0L);
   second = (double) (t2-t1) / _BIOS_CLK_TCK;
   wMessageBeep ();
   drawCross (sSx1+matchX, sSy1+matchY, YELLOW);
   if (r)
   showMsg (BLUE, "%3.0f ms, %dx%d in %dx%d CF=%1.3f, (%d,%d)",
     second*1000, sPx2-sPx1+1, sPy2-sPy1+1,
     sSx2-sSx1+1, sSy2-sSy1+1, matchCF, matchX, matchY);
}

void testRPDPNCS (void)
{
   if (!bSearchTeach) {
   showMsg (RED, "Teach Search First!");
   return;
   }

   showIm (icim);
   showMsg (BLUE, "test RPDPNCS...");
   switch (SearchSubPixel) {
   case PIXEL_LEVEL:
   t1 = biostime(0, 0L);
   RPDPNCS (patim, icim, sSx1, sSy1, sSx2, sSy2);
   t2 = biostime(0, 0L);
   second = (double) (t2-t1) / _BIOS_CLK_TCK;
   //wMessageBeep ();
   drawCross (sSx1+matchX, sSy1+matchY, YELLOW);
   showMsg (BLUE, "%3.0f ms L:%d, %dx%d in %dx%d CF=%1.3f, (%d,%d)",
      second*1000, RLayer, sPx2-sPx1+1, sPy2-sPy1+1,
      sSx2-sSx1+1, sSy2-sSy1+1, matchCF, matchX, matchY);
   break;
   case SUBPIXEL_LEVEL:
   t1 = biostime(0, 0L);
   SRPDPNCS (patim, icim, sSx1, sSy1, sSx2, sSy2);
   t2 = biostime(0, 0L);
   second = (double) (t2-t1) / _BIOS_CLK_TCK;
   //wMessageBeep ();
   drawCross (sSx1+matchX, sSy1+matchY, YELLOW);
   showMsg (BLUE, "%3.0f ms L:%d, CF=%1.3f, (%d,%d) (%3.4f,%3.4f)",
      second*1000, RLayer, matchCF, matchX, matchY,
      smatchX, smatchY);
   break;
   }
}


void testRPDPNCS_N (int n)
{
   int i;

   if (!bSearchTeach) {
   showMsg (RED, "Teach Search First!");
   return;
   }

   showIm (icim);
   showMsg (BLUE, "test RPDPNCS %d times...", n);
   t1 = biostime(0, 0L);
   for (i=0; i<n; i++)
   RPDPNCS (patim, icim, sSx1, sSy1, sSx2, sSy2);
   t2 = biostime(0, 0L);
   second = (double) (t2-t1) / _BIOS_CLK_TCK;
   //wMessageBeep ();
   drawCross (sSx1+matchX, sSy1+matchY, YELLOW);
  showMsg (BLUE, "%3.1f ms L:%d, %dx%d in %dx%d CF=%1.3f, (%d,%d)",
     second*1000/n, RLayer, sPx2-sPx1+1, sPy2-sPy1+1,
     sSx2-sSx1+1, sSy2-sSy1+1, matchCF, matchX, matchY);
}

void testSearchCounti (void)
{
   if (!bSearchTeach) {
   showMsg (RED, "Teach Search First!");
   return;
   }

   showMsg (BLUE, "test RPDPNCS Search Countinuously...");
   switch (SearchSubPixel) {
   case PIXEL_LEVEL:
   while (!(kbhit() || mMouseClick())) {
     MilGrabImageShow ();
     t1 = biostime(0, 0L);
     RPDPNCS (patim, icim, sSx1, sSy1, sSx2, sSy2);
     t2 = biostime(0, 0L);
     second = (double) (t2-t1) / _BIOS_CLK_TCK;
     drawCross (sSx1+matchX, sSy1+matchY, YELLOW);
     showMsg (BLUE, "%3.0f ms L:%d, %dx%d in %dx%d CF=%1.3f, (%d,%d)",
       second*1000, RLayer, sPx2-sPx1+1, sPy2-sPy1+1,
       sSx2-sSx1+1, sSy2-sSy1+1, matchCF, matchX, matchY);
   }
   break;
   case SUBPIXEL_LEVEL:
   while (!(kbhit() || mMouseClick())) {
     MilGrabImageShow ();
     t1 = biostime(0, 0L);
     SRPDPNCS (patim, icim, sSx1, sSy1, sSx2, sSy2);
     t2 = biostime(0, 0L);
     second = (double) (t2-t1) / _BIOS_CLK_TCK;
     drawCross (sSx1+matchX, sSy1+matchY, YELLOW);
     showMsg (BLUE, "%3.0f ms L:%d, CF=%1.3f, (%d,%d) (%3.4f,%3.4f)",
       second*1000, RLayer, matchCF, matchX, matchY,
       smatchX, smatchY);
   }
   break;
   }
}

void testSearch2 (void)
{
   unsigned char pat[30]={
  0,  0,255,255,  0,  0,
  0,  0,255,255,  0,  0,
   255,255,255,255,255,255,
  0,  0,255,255,  0,  0,
  0,  0,255,255,  0,  0
   };
   unsigned char sea1[56]={
  0,  0,  0,255,255,  0,  0,  0,
  0,  0,  0,255,255,  0,  0,  0,
  0,  0,  0,255,255,  0,  0,  0,
   255,255,255,255,255,255,255,255,
  0,  0,  0,255,255,  0,  0,  0,
  0,  0,  0,255,255,  0,  0,  9,
  0,  0,  0,255,255,  0,  0,  0,
   };

   unsigned char sea2[56]={
  9,  9,  9,128,128,  9,  9,  9,
  9,  9,  9,128,128,  9,  9,  9,
  9,  9,  9,128,128,  9,  9,  9,
   128,128,128,128,128,128,128,128,
  9,  9,  9,128,128,  9,  9,  9,
  9,  9,  9,128,128,  9,  9,  9,
  9,  9,  9,128,128,  9,  9,  9,
   };

   Image* pim = new Image (6, 5);
   copyblock (pim->imagedata, pat, pim->width*pim->height);
   Image* sim1 = new Image (8, 7);
   copyblock (sim1->imagedata, sea1, sim1->width*sim1->height);
  Image* sim2 = new Image (8, 7);
   copyblock (sim2->imagedata, sea2, sim2->width*sim2->height);

   putimage (0, 0, pim->image, COPY_PUT);
  putimage (10, 0, sim1->image, COPY_PUT);
   putimage (20, 0, sim2->image, COPY_PUT);

   t1 = biostime(0, 0L);
   RPDPNCS (pim, sim1, 0, 0, 7, 6);
   t2 = biostime(0, 0L);
   second = (double) (t2-t1) / _BIOS_CLK_TCK;
   //wMessageBeep ();
   //putpixel (bx+icS1L+matchX, by+icS1T+matchY, LIGHTGREEN);
   showMsg (BLUE, "%3.0f ms, %dx%d in %dx%d CF=%f, (%d,%d)",
     second*1000, 6, 5, 8, 7, matchCF, matchX, matchY);
}

void testSearch3 (void)
{
   //20x20 cross
   int pw=20,ph=20;
   unsigned char pat[400]={
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  };
   //24x24 cross
   int sw=24,sh=24;
   unsigned char sea[576]={
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
   255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,255,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,255,255,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
   };

  Image* pim = new Image (pw, ph);
   copyblock (pim->imagedata, pat, pim->width*pim->height);
   Image* sim = new Image (sw, sh);
   copyblock (sim->imagedata, sea, sim->width*sim->height);

   putimage (0, 0, pim->image, COPY_PUT);
   putimage (30, 0, sim->image, COPY_PUT);

  t1 = biostime(0, 0L);
  RPDPNCS (pim, sim, 0, 0, sw-1, sh-1);
  t2 = biostime(0, 0L);
  second = (double) (t2-t1) / _BIOS_CLK_TCK;
  //wMessageBeep ();
  showMsg (BLUE, "%3.0f ms, %dx%d in %dx%d CF=%f, (%d,%d)",
     second*1000, pw, ph, sw, sh, matchCF, matchX, matchY);
}

  */





