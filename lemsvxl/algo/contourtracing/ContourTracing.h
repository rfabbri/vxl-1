//ContourTracing.h
#include "points.h"
#include <vil/vil_image_resource.h>
#include <vbl/vbl_ref_count.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vcl_vector.h>

/*
class ContourPoint
{
private:
  Point2D<double> _pixelPoint;
  double _pixelDirection;

  ContourPoint* _prevPoint;
  ContourPoint* _nextPoint;

public:
  void setValues (const Point2D<double> &pixel_pt,
    const double &pixel_dir,
    ContourPoint* prevpt,
    ContourPoint* nextpt) {
    _pixelPoint = pixel_pt;
    _pixelDirection = pixel_dir;

    _prevPoint = prevpt;
    _nextPoint = nextpt;
  }

  ContourPoint (const Point2D<double> &pixel_pt,
    const double &pixel_dir,
    ContourPoint* prevpt,
    ContourPoint* nextpt) {
    setValues (pixel_pt, pixel_dir, prevpt, nextpt);
  }
  ~ContourPoint() {}

  ContourPoint(const ContourPoint &old) {
  setValues (old._pixelPoint, old._pixelDirection,
    old._prevPoint, old._nextPoint);
  }

  ContourPoint& operator=(const ContourPoint &old) {
    if (this != &old)
      setValues (old._pixelPoint, old._pixelDirection,
     old._prevPoint, old._nextPoint);
    return *this;
  }

  Point2D<double> PixelPoint() const { return _pixelPoint; }
  double PixelDirection() const { return _pixelDirection; }
  
  ContourPoint* PrevPoint() const { return _prevPoint; }
  void SetPrevPoint (ContourPoint* prv) {_prevPoint = prv;}
  ContourPoint* NextPoint() const { return _nextPoint; }
  void SetNextPoint (ContourPoint* nxt) {_nextPoint = nxt;}
};
*/

//030325 Contour Tracing interface By Ming:
//Input:
//1)RGBImage.
//2)empty vector ContourPoint to store the detected subpixel contour points
//3)sigma, default = 2
class ContourTracing : public vbl_ref_count
{
protected:
  float sigma_;
  vil_image_resource_sptr image_;

  int nPoints_;
//  ContourPoint* PointsListHead_;

  vcl_vector<vsol_point_2d_sptr> points_;

  char input_file_name[1024];

public:

  ContourTracing()
  { sigma_ = 2;//1;
    nPoints_ = 0;
//    PointsListHead_ = NULL;
    //For output file!
    strcpy (input_file_name, "CON.");
  }

  ~ContourTracing()
  {
//    ContourPoint* cur = PointsListHead_;
//    while (cur) {
//      PointsListHead_ = PointsListHead_->NextPoint();
//        delete cur;
//        cur = PointsListHead_;
//    }
  }

  void setSigma (float new_sigma) {sigma_ = new_sigma;}
  void setImageFileName (vcl_string name) {strcpy(input_file_name, name.c_str());}
  void setImage (vil_image_resource_sptr new_image) {image_ = new_image;}
  int nPoints() {return nPoints_;}

  //return number of points
  virtual int detectContour (void);

//  ContourPoint* PointsListHead (void) {return PointsListHead_;}

  vcl_vector<vsol_point_2d_sptr>& getResult (void);
};
