// This is brcv/seg/dbacm/dbacm_levelset_func.h
#ifndef dbacm_levelset_func_h_
#define dbacm_levelset_func_h_

//:
// \file
// \brief A levelset function which can evolve given a time step
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 11/28/2005
//
// \verbatim
//  Modifications
// \endverbatim

#include <vbl/vbl_ref_count.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_point_2d.h>


//: A levelset function which can evolve given a time step
class dbacm_levelset_func : public vbl_ref_count
{ 
public:
  //: Constructor
  dbacm_levelset_func() {}
  
  //: Destructor
  virtual ~dbacm_levelset_func() {}

  // ================================================================
  // DATA ACCESS
  // ================================================================
  
  //: Return reference to levelset set surface
  vil_image_view<float > levelset_surf() const {return this->phi_;}

  //: Set the level set function
  void set_levelset_surf(const vil_image_view<float >& phi)
  { this->phi_ = phi; }

  //: Return advection weight
  float advection_weight() const {return this->advection_weight_; }
  
  //: Set advection weight
  void set_avection_weight(float new_weight)
  { this->advection_weight_ = new_weight; }

  //: Return inflation_weight
  float inflation_weight() const {return this->inflation_weight_; }
  
  //: Set inflation weight
  void set_inflation_weight(float new_weight)
  { this->inflation_weight_ = new_weight; }

  //: Return curvature weight
  float curvature_weight() const {return this->curvature_weight_; }
  
  //: Set curvature weight
  void set_curvature_weight(float new_weight)
  { this->curvature_weight_ = new_weight; }



  //: Return advection images
  vil_image_view<float > advection_x() const { return this->gx_; }
  vil_image_view<float > advection_y() const { return this->gy_; }

  //: Set advection images
  void set_advection(const vil_image_view<float >& gx, 
    const vil_image_view<float >& gy)
  { this->gx_ = gx; this->gy_ = gy; }

  //: Return inflation image
  vil_image_view<float > inflation() const { return this->gmap_; }

  //: Set inflation image
  void set_inflation(const vil_image_view<float >& inflation)
  { this->gmap_ = inflation; }

  //: Return curvature modifier image
  vil_image_view<float > curvature_modifier() const { return this->kmap_; }

  //: Set curvature modifier image
  void set_curvature_modifier(const vil_image_view<float >& kmap)
  { this->kmap_ = kmap; }


  // ===============================================================
  // UTILITIES
  // ===============================================================

  //: Reinitialize levelset surface (to make sure it is a distance transform)
  virtual void reinitialize_levelset_surf(){};

  //: Evolve level set function given a time step
  virtual void evolve_one_timestep(float timestep) = 0;
  
protected:
  vil_image_view<float > phi_;
  float advection_weight_;
  float inflation_weight_;
  float curvature_weight_;

  // ----------- USER-SUPPLY DATA ----------------------------------
  
  //: advection images - needed for geodesic contour
  vil_image_view<float > gx_;
  vil_image_view<float > gy_;

  //: inflation image
  vil_image_view<float > gmap_;

  //: curvature modifier image
  vil_image_view<float > kmap_; 
  
};

#endif // dbacm_levelset_func_h_

