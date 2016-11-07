/*-----------------------------------------------------------------------------
File: RippleFilter.cc - a ripple filter
Created: M. Maurer
------------------------------------------------------------------------------*/
#include <iostream.h> 
#include "mveDListIter.hh"
#include "dbseg_ripple_RippleFilter.h"


//constructor for an image with the given size 
RippleFilter::RippleFilter(int nSizeX, int nSizeY, unsigned char *pImg) : 
  nXSize(0), nYSize(0)
{
  CleanUp();
  pImage = AttachImage(nSizeX, nSizeY, pImg);

  if(!pImage)
  {
#ifdef DEBUG
    cout << "RippleFilter::RippleFilter(): Failed to attach" << endl;
#endif
    return; 
  } 
}

//attaches an image
unsigned char  *RippleFilter::AttachImage(int nSizeX, int nSizeY,
  unsigned char *pImg)
{
  unsigned char *pMapImg, *pTmp;

  pMap = new ProbMap(nSizeX, nSizeY, pImg);
  if(!pMap)
  {
#ifdef DEBUG
    cout << "RippleFilter::AttachImage(): Failed to create ProbMap" << endl;
#endif
    return NULL; 
  } 
  pCont =  new LinkedContour;
  if(!pCont)
  {
#ifdef DEBUG
    cout << "RippleFilter::AttachImage(): Failed to create Contour" << endl;
#endif
    delete pMap;
    pMap = NULL;
    return NULL; 
  } 
  pMapImg =  pMap->GetMap(nXSize, nYSize);
  if(!pMapImg)
  {
#ifdef DEBUG
    cout << "RippleFilter::AttachImage(): pMap->GetMap() returned NULL map"
         << endl;
#endif
    delete pMap;
    pMap = NULL;
    delete pCont;
    pCont = NULL;
    return NULL; 
  }
  
  pTmp = (unsigned char*)malloc(sizeof(unsigned char)*nXSize*nYSize);
  if(!pTmp)
  {
#ifdef DEBUG
    cout << "RippleFilter::AttachImage(): failed to allocate image"
         << endl;
#endif
    delete pMap;
    pMap = NULL;
    delete pCont;
    pCont = NULL;
    return NULL;
  }


  memcpy(pTmp, pMapImg, sizeof(unsigned char)*nXSize*nYSize);

  return pTmp;

}

// performs one step 
void  RippleFilter::step()
{
  LinkedContour cont;
  ContElement *pEl;
  mveDListIter<ContElement> iter(*pCont);
  unsigned char nVal;
  while(pEl = iter())
  {
    // actual stuff
    cont.append(*pEl);
    nVal = pMap->GetElement(pEl->i, pEl->j);
  } 
  *pCont = cont; 
}












