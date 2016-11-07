// This is seg/dbsks/dbsks_gray_ocm.h
#ifndef dbsks_gray_ocm_h_
#define dbsks_gray_ocm_h_

//:
// \file
// \brief A class for oriented chamfer matching with gray-value edge map
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Nov 11, 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include <vil/vil_image_view.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vbl/vbl_array_2d.h>
#include <vbl/vbl_array_3d.h>
#include <vnl/vnl_vector.h>


// ============================================================================
class dbsks_gray_ocm
{
public:
  // Constructor / destructor -------------------------------------------------
  
  //: constructor
  dbsks_gray_ocm(){};

  //: destructor
  virtual ~dbsks_gray_ocm(){};

  // User-interface functions -------------------------------------------------

  //: Set the image
  void set_image(const vil_image_view<float >& image)
  {
    this->image_ = image;
  }

  //: Set the edgemap
  void set_edgemap(const vil_image_view<float >& edgemap, float edge_val_threshold = 10, 
    float max_edge_val = 255)
  {
    this->edge_val_threshold_ = edge_val_threshold;
    this->edgemap_ = edgemap;
    this->max_edge_val_ = max_edge_val;
  }

  //: Set related parameters to compute ocm cost
  // "nchannel" is the number of bins to conver [0, pi]
  void set_ocm_params(float distance_threshold, float tol_near_zero, int nchannel)
  {
    this->distance_threshold_ = distance_threshold;
    this->tol_near_zero_ = tol_near_zero;
    this->nchannel_ = nchannel;
  }

  //: Set lambda (\in [0, 1]) - linear combination weight
  void set_lambda(float lambda)
  {
    this->lambda_ = lambda;
  }

  //: Compute Chamfer Matching cost image from DT and its gradient
  void compute();

  // Utility functions --------------------------------------------------------

  //: cost of an oriented point, represented by its index
  float f(int i, int j, int orient_channel);

  //: cost of an oriented point, represented by its index
  float f(vcl_vector<int >& x, vcl_vector<int >& y, vcl_vector<int >& orient_channel);

protected:

  // \todo remove these /////////////////////////////////////////////////////////
  // copied from dbsks_shotton_ocm

  //: Compute distance transform of an edgemap
  void compute_dt(const vil_image_view<float >& edgemap,
                              float edge_threshold,
                              vil_image_view<float >& dt_edgemap);

  //: Compute gradient of an image using Gaussian kernel
  void compute_gradient(vil_image_view<float >& dt,
                         vil_image_view<float >& dt_grad_x,
                         vil_image_view<float >& dt_grad_y,
                         vil_image_view<float >& dt_grad_mag);

    // Cached variables
  vil_image_view<float > dt_;
  vil_image_view<float > dt_grad_x_;
  vil_image_view<float > dt_grad_y_;
  vil_image_view<float > dt_grad_mag_;
  //////////////////////////////////////////////////////////////////////////////

  //: Compute gradient of an image using Gaussian kernel
  void compute_gradient();

  //: Compute gray OCM cost by brute-force checking every pixel in the neighborhood
  void compute_gray_ocm_brute_force();

  //: Compute gray OCM cost by brute-force checking every pixel in the neighborhood
  void compute_gray_ocm_brute_force_closest_edge();

  // Member variables ---------------------------------------------------------

  vil_image_view<float > image_;
  vil_image_view<float > edgemap_;
  float max_edge_val_;
  float edge_val_threshold_;
  float distance_threshold_;
  float tol_near_zero_; // due to discretization
  float lambda_; // parameter determing weights of chamfer cost and distance cost
  int nchannel_;

  // Cached variables
  vil_image_view<float > grad_x_;
  vil_image_view<float > grad_y_;
  vil_image_view<float > grad_mag_;
  vbl_array_3d<float > ocm_cost_;  
};


#endif // seg/dbsks/dbsks_gray_ocm.h


