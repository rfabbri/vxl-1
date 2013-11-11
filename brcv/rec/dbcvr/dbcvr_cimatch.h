#ifndef _CIMATCH_H
#define _CIMATCH_H

#include <assert.h>
#include "dbcvr_cvmatch.h"

#define CIM_COST_INFINITY      1E6
#define CIM_TANCODE_NA        -1
#define CIM_TANCODE_0        0  //TanCode for no movement

typedef enum {
  BOGUS_CIM_TEMPLATE_SIZE,
  CIM_TEMPLATE_1 = 1,
  CIM_TEMPLATE_3X3 = 2,
  CIM_TEMPLATE_5X5 = 3,
  CIM_TEMPLATE_7X7 = 4,
} CIM_TEMPLATE_SIZE;

//#include "rgbimage.h"
#include "vcl_vector.h"

#define min2(x,y)   ((x)>(y)?(y):(x))
#define max2(x,y)   ((x)>(y)?(x):(y))

inline double min3 (double x, double y, double z)
{
  double minof2 = min2 (x,y);
  return min2 (minof2, z);
}

class dbcvr_cimatch
{
protected:
  //Inputs:
  bsol_intrinsic_curve_2d_sptr    _inputCurve;
  //RGBImage        *_image;
  int          _startPointX, _startPointY;

  int          _inputCurveLength;
  int          _imageWidth;
  int          _imageHeight;

  int          _SearchWindowLeft, _SearchWindowTop, _SearchWindowRight, _SearchWindowBottom;
  int          _SearchWindowWidth, _SearchWindowHeight;

  //canny
  short int      *nmsbuf;
  int          max_nms;
  float          *tanbuf;
  //EdgeImage      *_edgeMagImage;

  //Processing Datastructures:
  CIM_TEMPLATE_SIZE _VirtualTemplateSize;

  CIM_TEMPLATE_SIZE  _TemplateSize;
  int          _TemplateDepth;
  double        _alpha, _beta;
  /////3x3/////
  double        ***DPTable;      //Here, 3 dimensional table
  int          ***DPPrevTangent; //To keep track of path

  //Results:
  int          _nBestMatch;
  double        *_BestMatchingCost;
  FinalMapType    *_fmap;
  bsol_intrinsic_curve_2d*    _outputCurve;

public:
  vcl_string        _curveFileName, _imageFileName, _dirName;
  bsol_intrinsic_curve_2d_sptr  inputCurve() { return _inputCurve; }
  void setInputCurve (bsol_intrinsic_curve_2d_sptr c1) { _inputCurve = c1; }

  CIM_TEMPLATE_SIZE VirtualTemplateSize () { return _VirtualTemplateSize; }
  void SetVirtualTemplateSize (CIM_TEMPLATE_SIZE newsize) {
    _VirtualTemplateSize = newsize;
  }  
  CIM_TEMPLATE_SIZE TemplateSize () { return _TemplateSize; }
  void SetTemplateSize (CIM_TEMPLATE_SIZE newsize) {
    _TemplateSize = newsize;
  }
  int TemplateDepth () { return _TemplateDepth; }
  void SetTemplateDepth (int newdepth) {
    _TemplateDepth = newdepth;
  }

  double alpha() { return _alpha; }
  void setAlpha (double a) {
    _alpha = a; _beta = 1-a;
  }

  //RGBImage*  image() { return _image; }
  //EdgeImage*  edgeMagImage() { return _edgeMagImage; }
  //void setImage (RGBImage* image) { _image = image; }
  int  startX() { return _startPointX+_SearchWindowLeft; }
  void setStartX (int startx)
  {
    assert (startx-_SearchWindowLeft>=0);
    assert (startx<=_SearchWindowRight);
    _startPointX = startx-_SearchWindowLeft; //_startPointX is relative now!!
  }
  int  startY() { return _startPointY+_SearchWindowTop; }
  void setStartY (int starty)
  {
    assert (starty-_SearchWindowTop>=0);
    assert (starty<=_SearchWindowBottom);
    _startPointY = starty-_SearchWindowTop; //_startPointY is relative now!!
  }
  int searchWindowLeft() { return _SearchWindowLeft; }
  int searchWindowTop() { return _SearchWindowTop; }
  int searchWindowRight() { return _SearchWindowRight; }
  int searchWindowBottom() { return _SearchWindowBottom; }
  int searchWindowWidth() { return _SearchWindowWidth; }
  int searchWindowHeight() { return _SearchWindowHeight; }
  void setSearchWindow (int left, int top, int right, int bottom)
  {
    _SearchWindowLeft = left;
    _SearchWindowTop = top;
    _SearchWindowRight = right;
    _SearchWindowBottom = bottom;
    _SearchWindowWidth = right-left+1;
    _SearchWindowHeight = bottom-top+1;
    assert (_SearchWindowWidth>0);
    assert (_SearchWindowHeight>0);
  }

  void allocateDPMemory ();
  void freeDPMemory ();

  double stretchCost (bsol_intrinsic_curve_2d_sptr curve, int i, int ip) {
    return curve->arcLength(i) - curve->arcLength(ip);
  }
  double bendCost (bsol_intrinsic_curve_2d_sptr curve, int i, int ip) {
    return curve_angleDiff (curve->angle(i), curve->angle(ip));
  }

  double computeCost (int j1, int j2, int i1x, int i1y, int i2x, int i2y, int prevTangent);
  double computeImageCost (int x, int y, int dir);
  void computeCannyMagnitudeDir (void);
  double computeImageCostCanny (int x, int y, int dir);

  //Output:
  double BestMatchingCost(int i) {
    assert (i<_nBestMatch);
    return _BestMatchingCost[i];
  }
  double*** getDPTable() { return DPTable; }
  
  
  FinalMapType* fmap(int i) {
    assert (i<_nBestMatch);
    return &(_fmap[i]);
  }

  bsol_intrinsic_curve_2d*  outputCurve(int i) {
    assert (i<_nBestMatch);
    return &_outputCurve[i];
  }

  dbcvr_cimatch ();
  ~dbcvr_cimatch ();

  void Match ();

  void OpenCIM (const char* filename);
  void SaveCIM (const char* filename);
  void ExportDPTable (const char* filename);
};

typedef enum {
  BOGUS_RESAMPLE_CURVE_TYPE,
  RESAMPLE_CURVE_OFF = 1,
  RESAMPLE_CURVE_UNIT = 2,
  RESAMPLE_CURVE_GRID = 3,
} RESAMPLE_CURVE_TYPE;

void ResampleCurve (RESAMPLE_CURVE_TYPE resample_type, bsol_intrinsic_curve_2d* inputCurve, bsol_intrinsic_curve_2d* outputCurve);

class NCMatch
{
public:
  vcl_string  _patternFileName, _searchFileName;
  double  _MatchingScore;
  int    _MatchingX, _MatchingY;
  double  _SubpixelMatchingX, _SubpixelMatchingY;
  
  //RGBImage        *_patternImage;
  //RGBImage        *_searchImage;


  NCMatch () 
  {
    //_patternImage = NULL;
    //_searchImage = NULL;
  }
};


#endif
