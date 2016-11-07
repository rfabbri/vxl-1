/*-----------------------------------------------------------------------------
LinkedContour - a contour as a double linked list of ContElements 
------------------------------------------------------------------------------*/
#include "dbseg_ripple_LinkedContour.h"
#include "mveDListIter.hh"

ostream &operator<<(ostream &os,  ContElement &cont)
{
  os << "ContElement: " << endl
       << "(" << cont.i << ", " << cont.j << ")" << endl;

  return os; 
} 

ostream &operator<<(ostream &os,  LinkedContour &cont)
{
  os << "Contour: " << endl
       << (mveDList<ContElement>)cont << endl;

  return os; 
} 

//makes a contour from a rectangle given lower-left and upper right coords.
  //i-vertical, j-horiz.
void LinkedContour::MakeRectangle(int nLLI, int nLLJ, int nURI, int nURJ)
{
  int i,j ;

  // left side
  for(i=nLLI; i>=nURI; i--)
  {
    append(ContElement(i, nLLJ));
    cout  << "1 adding:" << i << "," << nLLI << endl;
  }   

  // upper side 
  for(j=nLLJ; j<=nURJ; j++)
  {
    append(ContElement(nURI, j));
    cout  << "2 adding:" << nURI << "," << i << endl;
  }   

  // right side
  for(i=nURI; i<=nLLI; i++)
  {
    append(ContElement(i, nURJ));
    cout  << "3 adding:" << i << "," << nURI << endl;
  }   

  // bottom  side 
  for(j=nURJ; j>=nLLJ; j--)
  {
    append(ContElement(nLLI, j));
    cout  << "4 adding:" << nLLI << "," <<j << endl;
  }   

}

// creates a binary image of a given size with 0 corresponding to 
//    the contour elements
unsigned char *LinkedContour::GetContourImage(int nXsize, int nYsize)
{
  unsigned char *pIm;
  ContElement *pEl;  

  if((nXsize <=0) || (nYsize <=0) )
  {
    cout <<"LinkedContour::GetContourImage: zero image size" << endl; 
    return 0;
  } 
  
  pIm = (unsigned char*)malloc(sizeof(unsigned char)*nXsize*nYsize);

  if(!pIm)
  {
    cout <<"LinkedContour::GetContourImage: Failed to allocate image" << endl; 
    return 0;
  } 
  // clear the buffer
  memset(pIm, 255, nXsize*nYsize*sizeof(unsigned char));
  mveDListIter<ContElement> iter(*this);
  
  while(pEl=iter())
  {
    if((pEl->i < nYsize) && (pEl->j < nXsize))
      pIm[pEl->i*nXsize + pEl->j] = 0;
  }
  return pIm;
}

// adds contour of a given color to a given image
unsigned char *LinkedContour::AddContourToImage(unsigned char *pImg,
 int nXsize, int nYsize, unsigned char nCol)
{
  unsigned char *pImTmp;
  ContElement *pEl;  

  if((nXsize <=0) || (nYsize <=0) )
  {
    cout <<"LinkedContour::GetContourImage: zero image size" << endl; 
    return 0;
  } 
  
  pImTmp = (unsigned char*)malloc(sizeof(unsigned char)*nXsize*nYsize);

  if(!pImTmp)
  {
    cout <<"LinkedContour::GetContourImage: Failed to allocate image" << endl; 
    return 0;
  } 
  // copy images 
  memcpy(pImTmp, pImg, sizeof(unsigned char)*nXsize*nYsize);

  mveDListIter<ContElement> iter(*this);
  
  while(pEl=iter())
  {
    if((pEl->i < nYsize) && (pEl->j < nXsize))
      pImTmp[pEl->i*nXsize + pEl->j] = nCol;
  }

  return pImTmp;
}








