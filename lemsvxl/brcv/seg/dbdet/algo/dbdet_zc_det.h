// This is brcv/seg/dbdet/algo/dbdet_zc_det.h
#ifndef dbdet_zc_det_h
#define dbdet_zc_det_h
//:
//\file
//\brief A Zero crossing detector class that can work with the dbdet_edge_map class.
//
// For every pixel in an image, a line fit is applied along the
// gradient direction, and the zero crossing of this line is used
// to get the sub-pixel location of the curve. 
//
// Below drawing shows the face numbers at a pixel and  
// is for author's own reference.
//
//       .---->x
//       |
//       |
//       v y
//
//          6    7
//        -----------
//      5 |    |    | 8
//        |    |    |
//        -----------
//      4 |    |    | 1
//        |    |    |
//        -----------
//          3     2
//
//\author Amir Tamrakar (adapted from Can's dbdet_nms class)
//\date 12/12/06
//
//\verbatim
//  Modifications
//\endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_vector.h>
#include <vbl/vbl_array_2d.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

//: parameters for ZC
class dbdet_zc_det_params
{
public:
  double thresh_;
  unsigned margin_;
  unsigned option_; //0: both max and min, 1: max only, 2: min only

  dbdet_zc_det_params(double thresh=1.0, unsigned margin=1, unsigned option=0): 
    thresh_(thresh), margin_(margin), option_(option){}
  ~dbdet_zc_det_params(){}
};

class dbdet_zc_det
{
protected:
  double thresh_;    ///< threshold
  unsigned margin_;  //size of margin to ignore
  unsigned option_;  //0: both max and min, 1: max only, 2: min only

  //references to the data passed to this algo
  const vil_image_view<double>& dir_x_; 
  const vil_image_view<double>& dir_y_;
  const vil_image_view<double>& grad_mag_;
  const vil_image_view<double>& mask_;

  //: 2d array holding the magnitude of the detected local maxima points
  vbl_array_2d<double> mag_;

public:

  //: default constructor
  dbdet_zc_det();

  //: Constructor from a parameter block, gradient magnitudes given as an image and gradients given as component images
  dbdet_zc_det(const dbdet_zc_det_params& zcp, 
            const vil_image_view<double>& dir_x, 
            const vil_image_view<double>& dir_y,
            const vil_image_view<double>& grad_mag,
            const vil_image_view<double>& mask);

  //: Destructor
  ~dbdet_zc_det(){}

  //Accessors
  unsigned width() { return mag_.cols(); }
  unsigned height() { return mag_.rows(); }

  //: return the array containing the suppressed non maximas(i.e., only maximas remain)
  vbl_array_2d<double>& mag(){ return mag_; } 

  //: apply NMS to the given data
  void apply(bool collect_tokens, vcl_vector<vgl_point_2d<double> > &loc, vcl_vector<double> &orientation, vcl_vector<double> &mag);

  void clear();

  //intermediate functions
  int intersected_face_number(const vgl_vector_2d<double>& direction);
  double intersection_parameter(const vgl_vector_2d<double>& direction, int face_num);
  void f_values(int x, int y, const vgl_vector_2d<double>& direction, double s, int face_num, double *f);
  // get the corners related to the given face
  void get_relative_corner_coordinates(int face_num, int *corners);
  // used for 3 points linear fit
  double subpixel_s(double *s, double *f);
  // used for 9 points parabola fit
  double subpixel_s(int x, int y, const vgl_vector_2d<double>& direction);
  void find_distance_s_and_f_for_point(int x, int y, vgl_homg_line_2d<double> line, 
                                       double &d, double &s, const vgl_vector_2d<double>& direction);
};


#endif // dbdet_zc_det_h
