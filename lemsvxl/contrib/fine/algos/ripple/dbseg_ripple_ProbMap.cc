/*-----------------------------------------------------------------------------
File: ProbMap.cc - a probability map
Created: M. Maurer
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <iostream.h>
#include "dbseg_ripple_ProbMap.h" 
//constructor for a given size and image
ProbMap::ProbMap(int nSizeX, int nSizeY, unsigned char *pImg) : pTable(0)
{

  if(!pImg)
  {
#ifdef DEBUG
    cout << "ProbMap::ProbMap: Null pointer" << endl;
#endif
    nXSize = nYSize = 0; 
  }
  pImage =  new  unsigned char[nSizeX*nSizeY];
  if(!pImage)
  {
#ifdef DEBUG
    cout << "ProbMap::ProbMap: Failed to allocate image" << endl;
#endif
    nXSize = nYSize = 0; 
    return;
  }
  memcpy(pImage, pImg, sizeof(unsigned char)*nSizeX*nSizeY);

  nXSize = nSizeX;
  nYSize = nSizeY;

  pTable =  fill(); 

  if(!pTable)
  {
#ifdef DEBUG
    cout << "ProbMap::ProbMap: Failed to allocate table" << endl;
#endif
    delete pImage;
    pImage = 0;
    nXSize = nYSize = 0; 
  }


}
  
  
// allocates pTable and fills it in with the values
unsigned char *ProbMap::fill() 
{
  unsigned char  *pTmp;
  int nMidI = nYSize/2;
  int nMidJ = nXSize/2;
  double fVal;
  double fIm;

  pTmp = new unsigned char [nYSize*nXSize];
  if(!pTmp)
  {
#ifdef DEBUG
    cout << "ProbMap::fill: Failed to allocate table" << endl;
#endif
    return 0;
  }
  
  for(int i=0; i<nYSize; i++) 
  {
    for(int j=0; j<nXSize; j++) 
    {
      fIm = pImage[i*nXSize + j];
      fVal = 1.0 - 1.0 / (sqrt(2.0*fPI*fVariance*fVariance) * 
        exp(-0.5/(fVariance*fVariance)*(fIm*fIm))); 
      fVal = 1.0 - fVal;
      fVal *= 255.0;
      pTmp[i*nXSize + j] = (unsigned char)fVal; 
    }

  }

  return pTmp;
  
}








