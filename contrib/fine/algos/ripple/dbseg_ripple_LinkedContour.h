/*-----------------------------------------------------------------------------
LinkedContour - a contour as a double linked list of ContElements 
------------------------------------------------------------------------------*/
#ifndef __LINKEDCONTOUR_HH
#define __LINKEDCONTOUR_HH

#include <iostream.h> 
#include "mveDList.hh"

struct ContElement
{
  friend class ostream &operator<<(ostream &os,  ContElement &cont);
  ContElement(int ii, int jj) : i(ii), j(jj) {};
  int i,j;
};


class LinkedContour : public mveDList<ContElement>
{
  friend class ostream &operator<<(ostream &os,  LinkedContour &cont);
public:
  LinkedContour(void) {};    
  ~LinkedContour(void) {};
  //makes a contour from a rectangle given lower-left and upper right coords.
  //i-vertical, j-horiz.
  void MakeRectangle(int nLLI, int nLLJ, int nURI, int nURJ);
  // creates a binary image of a given size with 0 corresponding to 
  //  the contour elements
  unsigned char *GetContourImage(int nXsize, int nYsize);
  // adds contour of a given color to a given image  
  unsigned char *AddContourToImage(unsigned char *pImg, int nXsize, int nYsize,
   unsigned char nCol);

};

#endif


