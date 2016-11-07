// This is brcv/seg/dbacm/dbacm_geodesic_active_contour.h
#ifndef dbacm_geodesic_active_contour_h_
#define dbacm_geodesic_active_contour_h_
//:
// \file
// \brief A class implementing geodesic active contour model
// Reference: Caselles, V., et al. "Geodesic Active Contours." IJCV97.
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 11/28/2005
//
// \verbatim
//  Modifications
// \endverbatim


#include <dbacm/dbacm_levelset_driver.h>


//: A class implementing geodesic active contour model
// Reference: Caselles, V., et al. "Geodesic Active Contours." IJCV97.
class dbacm_geodesic_active_contour : public dbacm_levelset_driver
{
 public:
  //: Constructor
  dbacm_geodesic_active_contour();
 
  //: Destructor
  virtual ~dbacm_geodesic_active_contour() {}

  // ================================================================
  // DATA ACCESS
  // ================================================================


  //: Return gradient norm
  float gradient_norm() const {return this->gradient_norm_; }

  //: Set gradient norm
  void set_gradient_norm(float new_norm){ this->gradient_norm_=new_norm; }

  //: Return inflation weight (constant velocity)
  float inflation_weight() const { return this->inflation_weight_; };

  //: Return curvature weight
  float curvature_weight() const { return this->curvature_weight_; }

  //: Set weights
  void set_weights(float inflation_weight, float curvature_weight, bool use_geodesic )
  { this->inflation_weight_=inflation_weight; 
  this->curvature_weight_ = curvature_weight; 
  this->use_geodesic_ = use_geodesic; }

  //: Return direction to approach the object
  // Return 1 if going OUT  (initial guess is INSIDE object)
  //        -1 if going IN (initial guess is OUTSIDE object)
  int direction(){ return this->direction_; };
  
  //: Set direction
  void set_direction(int dir){ this->direction_ = (dir > 0) ? 1:-1; }

  //: Time step
  float timestep() const { return this->timestep_; }

  //: Set time step
  void set_timestep(float timestep){ this->timestep_ = timestep; }


  // ================================================================
  // UTILITIES
  // ================================================================

  //: Set parameters of levelset function
  void compute_levelset_func_params();

  //: Set initial levelset surface
  virtual void init_levelset_surf(const vil_image_view<float >& init_phi);

  //: Initialize levelset surface with binary mask
  void init_levelset_surf_with_binary_mask(const vil_image_view<bool >& binary_mask);

  //: Evolve the levelset function till convergence or reaching maximum number
  // of iterations.
  virtual void evolve(bool verbose=false);


protected:
  // value used to normalize gradient
  float gradient_norm_;

  int direction_;

  // weight of constant velocity term
  float inflation_weight_;

  // curvature weight
  float curvature_weight_;

  // true if geodesic formulation is used
  bool use_geodesic_;

  // time step
  float timestep_;
  
};

#endif // dbacm_geodesic_active_contour_h_
