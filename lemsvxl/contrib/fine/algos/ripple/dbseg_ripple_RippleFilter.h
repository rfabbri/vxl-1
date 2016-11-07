/*-----------------------------------------------------------------------------
File: RippleFilter.hh - a ripple filter
Created: M. Maurer
------------------------------------------------------------------------------*/
#ifndef _RIPLLEFILTER_HH
#define _RIPLLEFILTER_HH

#include "dbseg_ripple_ProbMap.h"
#include "dbseg_ripple_LinkedContour.h"

class RippleFilter
{
public:
  //default constructor
  RippleFilter() : nIter(0), nXSize(0), nYSize(0), pMap(0), pCont(0) {}; 

  //constructor for an image with the given size 
  RippleFilter(int nSizeX, int nSizeY, unsigned char *pImg);

  // destructor
  ~RippleFilter() {CleanUp();}

  //attaches an image
  unsigned char *AttachImage(int nSizeX, int nSizeY, unsigned char *pImg);

  //one step
  void step();
  
  // perform the whole calculation
  void SetIterations(int nIt) {nIter = nIt;}; 
  
  //returns the current image
  unsigned char *GetImage(int &nSizeX, int &nSizeY) const
   {nSizeX = nXSize; nSizeY = nYSize; return pImage;};

protected:
  int nXSize, nYSize;
  int nIter;
  unsigned char *pImage;
  ProbMap *pMap;
  LinkedContour *pCont;

  //cleans up
  void CleanUp() {if(pImage) {delete pImage; delete pMap; 
    delete pCont; nXSize = nYSize = 0;}};

}; 

#endif



