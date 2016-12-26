// This is brcv/seg/dbdet/vis/dbdet_lvwr_fit.h
#ifndef dbdet_lvwr_fit_h_
#define dbdet_lvwr_fit_h_
//:
// \file
// \brief Class that traces livewire contour using input polygon as
//        successive free points
//
// \author Ozge C Ozcanli Ozbay, (ozge@lems.brown.edu)
// \date 07/11/05
//
// \verbatim
//  Modifications
//      
// \endverbatim

#include <vil/vil_image_resource_sptr.h>
#include <dbdet/lvwr/dbdet_lvwr.h>

#include <bdgl/bdgl_curve_algs.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>

class dbdet_lvwr_fit 
{
public:

  dbdet_lvwr_fit(const dbdet_lvwr_params iparams, const osl_canny_ox_params params);
  ~dbdet_lvwr_fit() {}

  //: set image and polygon
  //  if crop is true crops the input image and moves the polygon
  //  if random is set, choose initial seed point randomly as one of polygon points,
  //  otherwise use the first point
  bool initialize(vsol_polygon_2d_sptr poly, vil_image_resource_sptr img_r, bool crop = false, bool random = false);
  
  //: use input polygons vertices as free mouse points to trace a contour on the image
  vsol_polygon_2d_sptr fit(bool smooth = false, float sigma = 1.0);

  //: smooth the output contour
  bool smooth(float sigma);
  
protected:
 
  dbdet_lvwr intsciss_;
  int seed_x_;
  int seed_y_;
  double topx_;
  double topy_;
  unsigned int start_index_;
  vsol_polygon_2d_sptr poly_;
  vsol_polygon_2d_sptr output_poly_;

  vcl_vector<vsol_point_2d_sptr> contour_;
  vcl_vector<vcl_pair<int, int> > cor_;
  int last_x, last_y, last_xx, last_yy;

private:
  

};


#endif // dbdet_lvwr_fit_h_
