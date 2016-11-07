#include <iostream.h>
#include <fstream.h>
#include "pnmutil.h"
#include "LinkedContour.hh"
#include "ProbMap.hh"
#include "ProbMap.hh"
#include "RippleFilter.hh"


void main(int argc, char *argv[])
{
  unsigned char *pImageIn, *pImgCont, *pContTmp, *pContAddTmp, *pMap;
  int nXResImg, nYResImg, nXResCont, nYResCont;
  int nColorType, nPPMType;
  FILE *pFileIn, *pFileOut;
  LinkedContour  cont;
  char szTmp[256];
 
  //parse command line
  if(argc < 3)
  {
   cerr << "Usage: " << argv[0] << " srcimgfile destimgfile "
        <<  endl;
   exit (-1);
  }

  /* Open file */
  pFileIn=fopen(argv[1],"r");

  if(!pFileIn)
  {
    cerr << "Can't open input file " << argv[1] << endl;
    exit (-1);
  }
 
  if(ParseHeader(pFileIn, &nColorType, &nPPMType, &nXResImg, &nYResImg) < 0)
  {
     cerr << "Failed to parse header in "<< argv[1] << endl;
     fclose(pFileIn);
     exit(-1);
  }

  if(!(pImageIn = ReadPixels(pFileIn, nPPMType, nXResImg, nYResImg)))
  {
    cerr << "Failed to read "<< argv[1] << endl;
    fclose(pFileIn);
    exit(-1);
  }

  fclose(pFileIn);


  cont.MakeRectangle(nYResImg-20, 20, 20, nXResImg-20); 
  cout << "Contour:" << cont << endl;

  pContTmp = cont.GetContourImage(nXResImg, nYResImg);   

  pContAddTmp = cont.AddContourToImage(pImageIn, nXResImg, nYResImg, 255);   

  if(!pContTmp)
  {
    cerr << " Failed to create temp cont image"<< endl;
    exit(-1);

  }  

  ProbMap   Map(nXResImg, nYResImg, pImageIn);

  pMap =  Map.GetMap(nXResImg, nYResImg);

  RippleFilter filter;

  filter.AttachImage(nXResImg, nYResImg, pImageIn);
  filter.SetIterations(5);
  
  for(int i=0; i<5; i++)
  {
    filter.step();
  }

  pFileOut=fopen(argv[2], "w");

  if(!pFileOut)
  {
    cerr << "Can't open output file " << argv[2] << endl;
    exit (-1);
  }



  WriteFilePNM(pMap/*pContAddTmp*/, pFileOut, PGMRAW, nXResImg, nYResImg);

  fclose(pFileOut);  

#if 1 
  sprintf(szTmp, "/usr/local/bin/xv %s", argv[2]);
  FILE *pOut;
  pOut = popen(szTmp, "w");
  pclose(pOut);
#endif
}








