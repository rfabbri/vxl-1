/*-----------------------------------------------------------------------------
File: ProbMap.hh - a probability map
Created: M. Maurer
------------------------------------------------------------------------------*/
#ifndef _PROBMAP_HH
#define _PROBMAP_HH

const double fVariance = 64.0; // variance
const double fPI = 3.1415926535897931;


class ProbMap
{
public:
  //default constructor
  ProbMap() : nXSize(0), nYSize(0), pTable(0), pImage(0) {}; 

  //constructor for an image with the given size 
  ProbMap(int nSizeX, int nSizeY, unsigned char *pImg);
  
  // destructor
  ~ProbMap() {if(pTable) {delete pTable; delete pImage;} };

  //returns size of the map
  void GetSize(int &nSizeX, int &nSizeY) { nSizeX = nXSize; nSizeY = nYSize;}; 

  //returns  the map
  unsigned char *GetMap(int &nSizeX, int &nSizeY) const 
   {nSizeX = nXSize; nSizeY = nYSize; return pTable;}; 

  // returns value nI-vertical index 
  inline unsigned char  &GetElement(int nI, int nJ) const; 
protected:
  int nXSize, nYSize;
  unsigned char *pTable; // a linear array yet
  unsigned char *pImage; // an associated image  
  // allocate fills pTable with the values
  unsigned  char *fill();  
}; 

// returns value nI-vertical index 
inline   unsigned char &ProbMap::GetElement(int nI, int nJ) const
{
#ifdef DEBUG
  if(!pTable)
  {
    cout << "ProbMap::GetValue: NULL pTable" << endl;
    return 0.0;
  }
  if((nI < 0) || (nJ < 0) || (nJ >= nXSize) || (nI >= nYSize))
  {
    cout << "ProbMap::GetValue: index out of boundaries" << endl;
    return 0.0;
  }
#endif
  return pTable[nI*nXSize + nJ];
} 

#endif 


