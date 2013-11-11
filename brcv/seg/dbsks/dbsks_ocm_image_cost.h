// This is seg/dbsks/dbsks_ocm_image_cost.h
#ifndef dbsks_ocm_image_cost_h_
#define dbsks_ocm_image_cost_h_

//:
// \file
// \brief A class for oriented chamfer matching of a shock graph
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date Dec 18, 2007
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <vil/vil_image_view.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <vbl/vbl_array_1d.h>
#include <dbsks/dbsks_circ_arc_grid.h>

// ============================================================================
class dbsks_ocm_image_cost : public vbl_ref_count
{
public:
  enum ocm_type {L2_DISTANCE_ANGLE, SHOTTON_OCM};

  // Constructor / destructor -------------------------------------------------
  
  //: constructor
  dbsks_ocm_image_cost(): ocm_type_in_use_(SHOTTON_OCM), tolerance_near_zero_(0.0f) {};

  //: destructor
  virtual ~dbsks_ocm_image_cost(){};

  // Access member variables --------------------------------------------------

  //: Set the member variables
  void set_edgemap(const vil_image_view<float > edgemap, float edge_threshold);
  void set_sigma(float sigma_distance, float sigma_angle)
  {
    this->sigma_distance_ = sigma_distance;
    this->sigma_angle_ = sigma_angle;
  }

  //: Set the value of tolerance near zero
  void set_tolerance_near_zero(float tolerance)
  { this->tolerance_near_zero_ = tolerance; }

  //: Return the type of ocm cost currently applied
  ocm_type ocm_type_in_use() const {return this->ocm_type_in_use_; }

  //: Set the ocm type used
  void set_ocm_type_in_use(ocm_type type) {this->ocm_type_in_use_ = type;}

  // Maximum value for ocm cost (depends on ocm_type)
  float max_cost() const;


  // User-interface functions -------------------------------------------------

  //: (Normalized) Oriented Chamfer Matching of a set of (point-tangent)s
  float f(const vcl_vector<vgl_point_2d<double > >& pts,
    const vcl_vector<vgl_vector_2d<double > >& tangents) const;

  //: Compute Oriented Chamfer Matching cost for a grid of circular arcs
  void f(const dbsks_circ_arc_grid& grid, float ds,
    vbl_array_1d<vnl_matrix<float > >& image_cost);

  //: (Normalized) Oriented Chamfer Matching of a shapelet
  float f(const dbsksp_shapelet_sptr& s, 
    bool include_front_arc,
    bool include_rear_arc,
    float ds = 1);
  
protected:

  //: (Normalized) L^2 - Oriented Chamfer Matching of a set of (point-tangent)s
  float f_L2(const vcl_vector<vgl_point_2d<double > >& pts,
    const vcl_vector<vgl_vector_2d<double > >& tangents) const;

  //: (Normalized) L^2 - Oriented Chamfer Matching of a set of (point-tangent)s
  float f_shotton_ocm(const vcl_vector<vgl_point_2d<double > >& pts,
    const vcl_vector<vgl_vector_2d<double > >& tangents) const;

  // Utility functions --------------------------------------------------------
public:

  //: Compute distance transform of an edgemap
  static void compute_dt(const vil_image_view<float >& edgemap,
                              float edge_threshold,
                              vil_image_view<float >& dt_edgemap);

  //: Compute gradient of an image using Gaussian kernel
  static void compute_gradient(vil_image_view<float >& dt,
                         vil_image_view<float >& dt_grad_x,
                         vil_image_view<float >& dt_grad_y,
                         vil_image_view<float >& dt_grad_mag);

  //: Compute Chamfer Matching cost image from DT and its gradient
  vil_image_view<float > compute_chamfer_cost();

  //: transform function, tolerance when near edge
  float chamfer_cost_w_near_zero_tolerance(float distance, 
    float sigma_distance,
    float tolerance, float clip_cost_value) const;

  //: Compute point-tangent samples of the shapelet, given sampling rate "ds"
  static void compute_boundary_samples_uniform(const dbsksp_shapelet_sptr& sh, double ds, 
                                 vcl_vector<vgl_point_2d<double > >& pts,
                                 vcl_vector<vgl_vector_2d<double > >& tangents);


  // Member variables ---------------------------------------------------------
  vil_image_view<float > edgemap_;
  float edge_threshold_;
  float sigma_distance_;
  float sigma_angle_;

  float tolerance_near_zero_; // due to discretization

  ocm_type ocm_type_in_use_;

  // Cached variables
  vil_image_view<float > dt_;
  vil_image_view<float > dt_grad_x_;
  vil_image_view<float > dt_grad_y_;
  vil_image_view<float > dt_grad_mag_;
};


#endif // seg/dbsks/dbsks_ocm_image_cost.h


