/**************************************************************
*    Name: Daniel Spinosa
*    File: bfrag2D.h
*    Asgn: 
*    Date: 
*
*      A bfrag2D is a representation of a (possibly) double sided,
*  2 dimensional fragment.  Each side has a curve associated
*  with it, which this class stores and can load as either a 
*  .con or a .cem file.  Both sides may also have images
*  associated with them, also loaded and sotred by this class
*  with a little help from (vil ?).
*
*  *Only a top curve is absolutely necessary (?)*
*
***************************************************************/

#ifndef __BFRAG2D_H__
#define __BFRAG2D_H__

#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <vnl/vnl_math.h>

#include <vil/vil_rgb.h>
#include <vil/vil_rgba.h>
#include <vil/vil_load.h>
#include <vil/vil_image_view.h>

#include "bfrag_curve.h"

class bfrag2D 
{
public:
  bfrag2D();
  bfrag2D( int index, std::string s = "unnamed" );
  virtual ~bfrag2D();

  unsigned frag_id_;

  //loading functions return 0 on success, NEG on failure
  int loadContour( std::string fn, bool top = 1 );
  int loadImage( std::string fn, bool top = 1 );

  vil_image_view_base_sptr getTopRGBImage(){ return topRGBImage; };
  vil_image_view_base_sptr getBotRGBImage(){ return botRGBImage; };
  vsol_polygon_2d_sptr getTopPolygon(){ return topPolygon; };
  vsol_polygon_2d_sptr getBotPolygon(){ return botPolygon; };

  void setName( std::string n ){ myName = n; };

  void popupInfo(bool &drawTopImage, bool &drawBotCurve, bool &drawBotImage);

  //accessor stuff
  bfrag_curve getTopContourAsCurve();
  bfrag_curve getBotContourAsCurve();
  int getIndex(){ return myIndex; };
  std::string getName(){ return myName; };
  std::string getTopCurveFn(){ return topCurveFn; };
  std::string getBotCurveFn(){ return botCurveFn; };
  std::string getTopImageFn(){ return topImageFn; };
  std::string getBotImageFn(){ return botImageFn; };
  int centerX(){ return _centerX; };
  int centerY(){ return _centerY; };
  int maxX(){ return _maxX; };
  int minX(){ return _minX; };
  int maxY(){ return _maxY; };
  int minY(){ return _minY; };

//private:

  int myIndex;
  std::string myName;

  vil_image_view_base_sptr topRGBImage, botRGBImage;
  vsol_polygon_2d_sptr topPolygon, botPolygon;

  std::string topCurveFn, botCurveFn, topImageFn, botImageFn;

  int loadCEM( std::string fn, vsol_polygon_2d_sptr &givenContour, bfrag_curve &givenCurve );
  int loadCON( std::string fn, vsol_polygon_2d_sptr &givenContour, bfrag_curve &givenCurve );

  bool topContourCurveMade, botContourCurveMade;
  bfrag_curve topContourCurve, botContourCurve;

  void calcCenter();

  int _centerX, _centerY, _maxX, _minX, _maxY, _minY;
};

#endif
