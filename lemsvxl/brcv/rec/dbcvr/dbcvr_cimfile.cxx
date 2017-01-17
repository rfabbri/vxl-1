#include "dbcvr_cimatch.h"
//#include "msgout.h"
#include <vcl_fstream.h>
#include <vcl_cstdio.h>
#include <vcl_cstring.h>
#define MAX_LEN 1024

void dbcvr_cimatch::OpenCIM (const char* filename)
{
  if (!filename || !vcl_strcmp(filename,"")) {
    vcl_cout<< " No File name given"<<vcl_endl;
    return;
  }

  //get file extension
  char *file_extension;
  file_extension=(char *)filename;
  while(file_extension[0]!='\0') file_extension++;
  file_extension -=4;

  vcl_ifstream fp(filename,vcl_ios::in);
  if (!fp){
    vcl_cout<<" : Unable to Open "<<filename<<vcl_endl;
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

  vcl_cout<< "\nRead settings from "<<filename<<"...\n";
  if (!vcl_strncmp(file_extension,".cim",4)) {
    while (fp.getline(buffer,MAX_LEN)) {
      if (!vcl_strncmp(buffer, "# CIM File v1.0", sizeof("# CIM File v1.0")-1)){
        version = 1;
        vcl_cout <<"# CIM File v1.0"<<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "startPointX: ", sizeof("startPointX: ")-1)){
        vcl_sscanf(buffer,"startPointX: %d", &x);
        vcl_cout <<"startPointX: "<< x <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "startPointY: ", sizeof("startPointY: ")-1)){
        sscanf(buffer,"startPointY: %d", &y);
        vcl_cout <<"startPointY: "<< y <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "Alpha: ", sizeof("Alpha: ")-1)){
        sscanf(buffer,"Alpha: %lf", &dd);
        setAlpha (dd);
        vcl_cout <<"Alpha: "<< _alpha <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "SearchWindowLeft: ", sizeof("SearchWindowLeft: ")-1)){
        sscanf(buffer,"SearchWindowLeft: %d", &ii);
        _SearchWindowLeft = ii;
        vcl_cout <<"SearchWindowLeft: "<< _SearchWindowLeft <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "SearchWindowTop: ", sizeof("SearchWindowTop: ")-1)){
        sscanf(buffer,"SearchWindowTop: %d", &ii);
        _SearchWindowTop = ii;
        vcl_cout <<"SearchWindowTop: "<< _SearchWindowTop <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "SearchWindowRight: ", sizeof("SearchWindowRight: ")-1)){
        sscanf(buffer,"SearchWindowRight: %d", &ii);
        _SearchWindowRight = ii;
        vcl_cout <<"SearchWindowRight: "<< _SearchWindowRight <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "SearchWindowBottom: ", sizeof("SearchWindowBottom: ")-1)){
        sscanf(buffer,"SearchWindowBottom: %d", &ii);
        _SearchWindowBottom = ii;
        vcl_cout <<"SearchWindowBottom: "<< _SearchWindowBottom <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "SearchWindowWidth: ", sizeof("SearchWindowWidth: ")-1)){
        sscanf(buffer,"SearchWindowWidth: %d", &ii);
        _SearchWindowWidth = ii;
        vcl_cout <<"SearchWindowWidth: "<< _SearchWindowWidth <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "SearchWindowHeight: ", sizeof("SearchWindowHeight: ")-1)){
        sscanf(buffer,"SearchWindowHeight: %d", &ii);
        _SearchWindowHeight = ii;
        vcl_cout <<"SearchWindowHeight: "<< _SearchWindowHeight <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "VirtualTemplateSize: 1", sizeof("VirtualTemplateSize: 1")-1)){
        _VirtualTemplateSize = CIM_TEMPLATE_1;
        vcl_cout <<"VirtualTemplateSize: 1" <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "VirtualTemplateSize: 3x3", sizeof("VirtualTemplateSize: 3x3")-1)){
        _VirtualTemplateSize = CIM_TEMPLATE_3X3;
        vcl_cout <<"VirtualTemplateSize: 3x3" <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "VirtualTemplateSize: 5x5", sizeof("VirtualTemplateSize: 5x5")-1)){
        _VirtualTemplateSize = CIM_TEMPLATE_3X3;
        vcl_cout <<"VirtualTemplateSize: 5x5" <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "VirtualTemplateSize: 7x7", sizeof("VirtualTemplateSize: 7x7")-1)){
        _VirtualTemplateSize = CIM_TEMPLATE_3X3;
        vcl_cout <<"VirtualTemplateSize: 7x7" <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "TemplateSize: 3x3", sizeof("TemplateSize: 3x3")-1)){
        _TemplateSize = CIM_TEMPLATE_3X3;
        vcl_cout <<"TemplateSize: 3x3" <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "TemplateSize: 5x5", sizeof("TemplateSize: 5x5")-1)){
        _TemplateSize = CIM_TEMPLATE_5X5;
        vcl_cout <<"TemplateSize: 5x5" <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "TemplateSize: 7x7", sizeof("TemplateSize: 7x7")-1)){
        _TemplateSize = CIM_TEMPLATE_7X7;
        vcl_cout <<"TemplateSize: 7x7" <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "DPDepth: ", sizeof("DPDepth: ")-1)){
        sscanf(buffer,"DPDepth: %d", &ii);
        _TemplateDepth = ii;
        vcl_cout <<"DPDepth: "<< _TemplateDepth <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "inputCurveCONFile: ", sizeof("inputCurveCONFile: ")-1)){
        sscanf(buffer,"inputCurveCONFile: %s", fname);
        _curveFileName = fname;
        vcl_cout <<"inputCurveCONFile: "<< _curveFileName <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "inputImageFile: ", sizeof("inputImageFile: ")-1)){
        sscanf(buffer,"inputImageFile: %s", fname);
        _imageFileName = fname;
        vcl_cout <<"inputImageFile: "<< _imageFileName <<vcl_endl;
      }
      else if (!vcl_strncmp(buffer, "nBestMatch: ", sizeof("nBestMatch: ")-1)){
        sscanf(buffer,"nBestMatch: %d", &ii);
        _nBestMatch = ii;
        vcl_cout <<"nBestMatch: "<< _nBestMatch <<vcl_endl;
      }

    }//end while
    
    assert (_SearchWindowWidth == _SearchWindowRight-_SearchWindowLeft+1);
    assert (_SearchWindowHeight == _SearchWindowBottom-_SearchWindowTop+1);
    setStartX (x);
    setStartY (y);
  }

   //close file
  vcl_cout <<vcl_endl;
   fp.close();
}

void dbcvr_cimatch::SaveCIM (const char* filename)
{
   if (!filename){
      vcl_cout<< " No File name given"<<vcl_endl;
      return;
   }
   vcl_ofstream fp(filename);
   if (!fp){
      vcl_cout<<" : Unable to Open "<<filename<<vcl_endl;
      return;
   }

   // output header information
   fp <<"# CIM File v1.0"<<vcl_endl;
   fp <<"startPointX: "<< _startPointX <<vcl_endl;
   fp <<"startPointY: "<< _startPointY <<vcl_endl;
   fp <<"Alpha: "<< _alpha <<vcl_endl;
   fp <<"SearchWindowLeft: "<< _SearchWindowLeft <<vcl_endl;
   fp <<"SearchWindowTop: "<< _SearchWindowTop <<vcl_endl;
   fp <<"SearchWindowRight: "<< _SearchWindowRight <<vcl_endl;
   fp <<"SearchWindowBottom: "<< _SearchWindowBottom <<vcl_endl;
  assert (_SearchWindowWidth == _SearchWindowRight-_SearchWindowLeft+1);
   fp <<"SearchWindowWidth: "<< _SearchWindowWidth <<vcl_endl;
  assert (_SearchWindowHeight == _SearchWindowBottom-_SearchWindowTop+1);
   fp <<"SearchWindowHeight: "<< _SearchWindowHeight <<vcl_endl;
   fp <<vcl_endl;

   fp <<"# Option"<<vcl_endl;
   fp <<"TemplateSize: ";
  switch (_TemplateSize) {
  case CIM_TEMPLATE_3X3: fp<<"3x3"; break;
  case CIM_TEMPLATE_5X5: fp<<"5x5"; break;
  case CIM_TEMPLATE_7X7: fp<<"7x7"; break;
  default: fp<<"3x3"; break;
  }
   fp <<vcl_endl;
   fp <<"DPDepth: "<< _TemplateDepth << vcl_endl;
  fp <<vcl_endl;
   fp <<"# Curve and Image Files"<<vcl_endl;
  fp <<"inputCurveCONFile: "<< _curveFileName <<vcl_endl;
  fp <<"inputImageFile: "<<_imageFileName <<vcl_endl;
  fp <<vcl_endl;

   //close file
   fp.close();
}

void dbcvr_cimatch::ExportDPTable (const char* filename)
{
   if (!filename){
      vcl_cout<< " No File name given"<<vcl_endl;
      return;
   }
   vcl_ofstream fp(filename);
   if (!fp){
      vcl_cout<<" : Unable to Open "<<filename<<vcl_endl;
      return;
   }

   // output header information
   fp <<"# DP Table File v1.0"<<vcl_endl;
   fp <<"Layer: "<< _inputCurveLength <<vcl_endl;
   fp <<"Width: "<< _SearchWindowWidth <<vcl_endl;
   fp <<"Height: "<< _SearchWindowHeight <<vcl_endl;
  int j, y, x;

   fp <<"# DP Table"<<vcl_endl;
  for (j=0; j<_inputCurveLength; j++) {
    for (y=0; y<_SearchWindowHeight; y++) {
      for (x=0; x<_SearchWindowWidth; x++) {
        fp << DPTable[j][y][x] << ", ";
      }
      fp << vcl_endl;
    }
    fp << vcl_endl;
  }

   fp <<"# DPTangent Table"<<vcl_endl;
  for (j=0; j<_inputCurveLength; j++) {
    for (y=0; y<_SearchWindowHeight; y++) {
      for (x=0; x<_SearchWindowWidth; x++) {
        fp << DPPrevTangent[j][y][x] << ", ";
      }
      fp << vcl_endl;
    }
    fp << vcl_endl;
  }

   fp.close();
}
