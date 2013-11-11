// This is brcv/seg/dbacm3d/dbacm3d_edge_lowmem_contour.h
#ifndef dbacm3d_edge_lowmem_contour_h_
#define dbacm3d_edge_lowmem_contour_h_
//:
// \file
// \brief A class implementing nonmax active contour model
// Reference: Caselles, V., et al. "Geodesic Active Contours." IJCV97.
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 11/28/2005
//
// \verbatim
//  Modifications
// \endverbatim


#include <vil3d/vil3d_image_view.h>
#include <vbl/vbl_ref_count.h>
#include <dbacm3d/dbacm3d_levelset_driver.h>


//: A class implementing nonmax active contour model
// Reference: Caselles, V., et al. "Geodesic Active Contours." IJCV97.
class dbacm3d_edge_lowmem_contour : public dbacm3d_levelset_driver
{
 public:
  //: Constructor
  dbacm3d_edge_lowmem_contour(const vil3d_image_view<float> &edges, 
                              const vil3d_image_view<bool> &no_go_image);
 
  //: Destructor
  virtual ~dbacm3d_edge_lowmem_contour() {}

  // ================================================================
  // DATA ACCESS
  // ================================================================


  //: Return gradient norm
  float gradient_norm() const {return this->gradient_norm_; }

  //: Set gradient norm
  void set_gradient_norm(float new_norm){ this->gradient_norm_=new_norm; }

  float gradient_gauss_sigma() const {return this->sigma_; }
  void set_gradient_gauss_sigma(float s){ this->sigma_=s; }

  float malladi_exponent() const {return this->exponent_; }
  void set_malladi_exponent(int s){ this->exponent_=s; }

  float edgeT() const {return this->edgeT_; }
  void set_edgeT(float T){ this->edgeT_=T; }

  //: Return inflation weight (constant velocity)
  float inflation_weight() const { return this->inflation_weight_; };

  //: Return curvature weight
  float curvature_weight() const { return this->curvature_weight_; }

  //: Return flag indicating whether advection term is used
  float use_geodesic() const { return this->use_geodesic_; }

  bool twoD_hack(){return this->twoD_hack_;}

  //: Set weights
  void set_weights(float inflation_weight, float curvature_weight, bool use_geodesic )
  { this->inflation_weight_=inflation_weight; 
  this->curvature_weight_ = curvature_weight; 
  this->use_geodesic_ = use_geodesic; }

  //: Return direction to approach the object
  // Return 1 if going OUT  (initial guess is INSIDE object)
  //        -1 if going IN (initial guess is OUTSIDE object)
  int direction() const { return this->direction_; };
  
  //: Set direction
  void set_direction(int dir){ this->direction_ = (dir > 0) ? 1:-1; }

  //: Time step
  float timestep() const { return this->timestep_; }

  //: Set time step
  void set_timestep(float timestep){ this->timestep_ = timestep; }

  void set_twoD_hack(bool hack){this->twoD_hack_ = hack;}


  // ================================================================
  // UTILITIES
  // ================================================================

  //: Set parameters of levelset function
  void compute_levelset_func_params();

  //: Set initial levelset surface
  virtual void init_levelset_surf(const vil3d_image_view<float >& init_phi);

  //: Initialize levelset surface with binary mask
  void init_levelset_surf_with_binary_mask(const vil3d_image_view<bool >& binary_mask);

  //: Evolve the levelset function till convergence or reaching maximum number
  // of iterations.
  virtual void evolve(bool verbose=false);


protected:
  // value used to normalize gradient
  float gradient_norm_;

  int direction_;

  double sigma_;
  int exponent_;

  bool twoD_hack_;
  
  float edgeT_;

  // weight of constant velocity term
  float inflation_weight_;

  // curvature weight
  float curvature_weight_;

  // true if nonmax formulation is used
  bool use_geodesic_;

  // time step
  float timestep_;
  
};

#endif // dbacm3d_edge_lowmem_contour_h_
