#include "CIMatch.h"
//#include "msgout.h"
#include <fstream>
#include <cstdio>

#define MAX_LEN 1024

void CurveImageMatch::OpenCIM (const char* filename)
{
  if (!filename || !strcmp(filename,"")) {
    std::cout<< " No File name given"<<std::endl;
    return;
  }

  //get file extension
  char *file_extension;
  file_extension=(char *)filename;
  while(file_extension[0]!='\0') file_extension++;
  file_extension -=4;

  std::ifstream fp(filename,std::ios::in);
  if (!fp){
    std::cout<<" : Unable to Open "<<filename<<std::endl;
    return;
  }

  //Before open the new file, free memory from the old one.
  if (DPTable)
    freeDPMemory ();

  char buffer[MAX_LEN];
  int version =1;  //default version
  int x, y, ii;
  double dd;
  char fname[MAX_LEN];

  std::cout<< "\nRead settings from "<<filename<<"...\n";
  if (!strncmp(file_extension,".cim",4)) {
    while (fp.getline(buffer,MAX_LEN)) {
      if (!strncmp(buffer, "# CIM File v1.0", sizeof("# CIM File v1.0")-1)){
        version = 1;
        std::cout <<"# CIM File v1.0"<<std::endl;
      }
      else if (!strncmp(buffer, "startPointX: ", sizeof("startPointX: ")-1)){
        std::sscanf(buffer,"startPointX: %d", &x);
        std::cout <<"startPointX: "<< x <<std::endl;
      }
      else if (!strncmp(buffer, "startPointY: ", sizeof("startPointY: ")-1)){
        sscanf(buffer,"startPointY: %d", &y);
        std::cout <<"startPointY: "<< y <<std::endl;
      }
      else if (!strncmp(buffer, "Alpha: ", sizeof("Alpha: ")-1)){
        sscanf(buffer,"Alpha: %lf", &dd);
        setAlpha (dd);
        std::cout <<"Alpha: "<< _alpha <<std::endl;
      }
      else if (!strncmp(buffer, "SearchWindowLeft: ", sizeof("SearchWindowLeft: ")-1)){
        sscanf(buffer,"SearchWindowLeft: %d", &ii);
        _SearchWindowLeft = ii;
        std::cout <<"SearchWindowLeft: "<< _SearchWindowLeft <<std::endl;
      }
      else if (!strncmp(buffer, "SearchWindowTop: ", sizeof("SearchWindowTop: ")-1)){
        sscanf(buffer,"SearchWindowTop: %d", &ii);
        _SearchWindowTop = ii;
        std::cout <<"SearchWindowTop: "<< _SearchWindowTop <<std::endl;
      }
      else if (!strncmp(buffer, "SearchWindowRight: ", sizeof("SearchWindowRight: ")-1)){
        sscanf(buffer,"SearchWindowRight: %d", &ii);
        _SearchWindowRight = ii;
        std::cout <<"SearchWindowRight: "<< _SearchWindowRight <<std::endl;
      }
      else if (!strncmp(buffer, "SearchWindowBottom: ", sizeof("SearchWindowBottom: ")-1)){
        sscanf(buffer,"SearchWindowBottom: %d", &ii);
        _SearchWindowBottom = ii;
        std::cout <<"SearchWindowBottom: "<< _SearchWindowBottom <<std::endl;
      }
      else if (!strncmp(buffer, "SearchWindowWidth: ", sizeof("SearchWindowWidth: ")-1)){
        sscanf(buffer,"SearchWindowWidth: %d", &ii);
        _SearchWindowWidth = ii;
        std::cout <<"SearchWindowWidth: "<< _SearchWindowWidth <<std::endl;
      }
      else if (!strncmp(buffer, "SearchWindowHeight: ", sizeof("SearchWindowHeight: ")-1)){
        sscanf(buffer,"SearchWindowHeight: %d", &ii);
        _SearchWindowHeight = ii;
        std::cout <<"SearchWindowHeight: "<< _SearchWindowHeight <<std::endl;
      }
      else if (!strncmp(buffer, "VirtualTemplateSize: 1", sizeof("VirtualTemplateSize: 1")-1)){
        _VirtualTemplateSize = CIM_TEMPLATE_1;
        std::cout <<"VirtualTemplateSize: 1" <<std::endl;
      }
      else if (!strncmp(buffer, "VirtualTemplateSize: 3x3", sizeof("VirtualTemplateSize: 3x3")-1)){
        _VirtualTemplateSize = CIM_TEMPLATE_3X3;
        std::cout <<"VirtualTemplateSize: 3x3" <<std::endl;
      }
      else if (!strncmp(buffer, "VirtualTemplateSize: 5x5", sizeof("VirtualTemplateSize: 5x5")-1)){
        _VirtualTemplateSize = CIM_TEMPLATE_3X3;
        std::cout <<"VirtualTemplateSize: 5x5" <<std::endl;
      }
      else if (!strncmp(buffer, "VirtualTemplateSize: 7x7", sizeof("VirtualTemplateSize: 7x7")-1)){
        _VirtualTemplateSize = CIM_TEMPLATE_3X3;
        std::cout <<"VirtualTemplateSize: 7x7" <<std::endl;
      }
      else if (!strncmp(buffer, "TemplateSize: 3x3", sizeof("TemplateSize: 3x3")-1)){
        _TemplateSize = CIM_TEMPLATE_3X3;
        std::cout <<"TemplateSize: 3x3" <<std::endl;
      }
      else if (!strncmp(buffer, "TemplateSize: 5x5", sizeof("TemplateSize: 5x5")-1)){
        _TemplateSize = CIM_TEMPLATE_5X5;
        std::cout <<"TemplateSize: 5x5" <<std::endl;
      }
      else if (!strncmp(buffer, "TemplateSize: 7x7", sizeof("TemplateSize: 7x7")-1)){
        _TemplateSize = CIM_TEMPLATE_7X7;
        std::cout <<"TemplateSize: 7x7" <<std::endl;
      }
      else if (!strncmp(buffer, "DPDepth: ", sizeof("DPDepth: ")-1)){
        sscanf(buffer,"DPDepth: %d", &ii);
        _TemplateDepth = ii;
        std::cout <<"DPDepth: "<< _TemplateDepth <<std::endl;
      }
      else if (!strncmp(buffer, "inputCurveCONFile: ", sizeof("inputCurveCONFile: ")-1)){
        sscanf(buffer,"inputCurveCONFile: %s", fname);
        _curveFileName = fname;
        std::cout <<"inputCurveCONFile: "<< _curveFileName <<std::endl;
      }
      else if (!strncmp(buffer, "inputImageFile: ", sizeof("inputImageFile: ")-1)){
        sscanf(buffer,"inputImageFile: %s", fname);
        _imageFileName = fname;
        std::cout <<"inputImageFile: "<< _imageFileName <<std::endl;
      }
      else if (!strncmp(buffer, "nBestMatch: ", sizeof("nBestMatch: ")-1)){
        sscanf(buffer,"nBestMatch: %d", &ii);
        _nBestMatch = ii;
        std::cout <<"nBestMatch: "<< _nBestMatch <<std::endl;
      }

    }//end while
    
    assert (_SearchWindowWidth == _SearchWindowRight-_SearchWindowLeft+1);
    assert (_SearchWindowHeight == _SearchWindowBottom-_SearchWindowTop+1);
    setStartX (x);
    setStartY (y);
  }

   //close file
  std::cout <<std::endl;
   fp.close();
}

void CurveImageMatch::SaveCIM (const char* filename)
{
   if (!filename){
      std::cout<< " No File name given"<<std::endl;
      return;
   }
   std::ofstream fp(filename);
   if (!fp){
      std::cout<<" : Unable to Open "<<filename<<std::endl;
      return;
   }

   // output header information
   fp <<"# CIM File v1.0"<<std::endl;
   fp <<"startPointX: "<< _startPointX <<std::endl;
   fp <<"startPointY: "<< _startPointY <<std::endl;
   fp <<"Alpha: "<< _alpha <<std::endl;
   fp <<"SearchWindowLeft: "<< _SearchWindowLeft <<std::endl;
   fp <<"SearchWindowTop: "<< _SearchWindowTop <<std::endl;
   fp <<"SearchWindowRight: "<< _SearchWindowRight <<std::endl;
   fp <<"SearchWindowBottom: "<< _SearchWindowBottom <<std::endl;
  assert (_SearchWindowWidth == _SearchWindowRight-_SearchWindowLeft+1);
   fp <<"SearchWindowWidth: "<< _SearchWindowWidth <<std::endl;
  assert (_SearchWindowHeight == _SearchWindowBottom-_SearchWindowTop+1);
   fp <<"SearchWindowHeight: "<< _SearchWindowHeight <<std::endl;
   fp <<std::endl;

   fp <<"# Option"<<std::endl;
   fp <<"TemplateSize: ";
  switch (_TemplateSize) {
  case CIM_TEMPLATE_3X3: fp<<"3x3"; break;
  case CIM_TEMPLATE_5X5: fp<<"5x5"; break;
  case CIM_TEMPLATE_7X7: fp<<"7x7"; break;
  }
   fp <<std::endl;
   fp <<"DPDepth: "<< _TemplateDepth << std::endl;
  fp <<std::endl;
   fp <<"# Curve and Image Files"<<std::endl;
  fp <<"inputCurveCONFile: "<< _curveFileName <<std::endl;
  fp <<"inputImageFile: "<<_imageFileName <<std::endl;
  fp <<std::endl;

   //close file
   fp.close();
}

void CurveImageMatch::ExportDPTable (const char* filename)
{
   if (!filename){
      std::cout<< " No File name given"<<std::endl;
      return;
   }
   std::ofstream fp(filename);
   if (!fp){
      std::cout<<" : Unable to Open "<<filename<<std::endl;
      return;
   }

   // output header information
   fp <<"# DP Table File v1.0"<<std::endl;
   fp <<"Layer: "<< _inputCurveLength <<std::endl;
   fp <<"Width: "<< _SearchWindowWidth <<std::endl;
   fp <<"Height: "<< _SearchWindowHeight <<std::endl;
  int j, y, x;

   fp <<"# DP Table"<<std::endl;
  for (j=0; j<_inputCurveLength; j++) {
    for (y=0; y<_SearchWindowHeight; y++) {
      for (x=0; x<_SearchWindowWidth; x++) {
        fp << DPTable[j][y][x] << ", ";
      }
      fp << std::endl;
    }
    fp << std::endl;
  }

   fp <<"# DPTangent Table"<<std::endl;
  for (j=0; j<_inputCurveLength; j++) {
    for (y=0; y<_SearchWindowHeight; y++) {
      for (x=0; x<_SearchWindowWidth; x++) {
        fp << DPPrevTangent[j][y][x] << ", ";
      }
      fp << std::endl;
    }
    fp << std::endl;
  }

   fp.close();
}
