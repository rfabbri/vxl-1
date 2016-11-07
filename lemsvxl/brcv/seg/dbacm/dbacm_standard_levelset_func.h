// This is brcv/seg/dbacm/dbacm_standard_levelset_func.h

#ifndef dbacm_standard_levelset_func_h_
#define dbacm_standard_levelset_func_h_

//:
// \file
// \brief A levelset function that compute data at all levels
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 11/28/2005
//
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbacm/dbacm_levelset_func.h>

//: A level set function which computed data at all levels
// Improvements to this include narrow-band scheme, sparse-field algo, etc.
class dbacm_standard_levelset_func : public dbacm_levelset_func
{ 
public:
  //: Constructor
  dbacm_standard_levelset_func() {}
  
  //: Destructor
  virtual ~dbacm_standard_levelset_func() {}

  // ================================================================
  // DATA ACCESS
  // ================================================================

  

  // =================================================================
  // UTILITIES
  // =================================================================

  //: Reinitialize levelset surface (to make sure it is a distance transform)
  virtual void reinitialize_levelset_surf();

  //: evolve the levelset function given a time step
  virtual void evolve_one_timestep(float timestep);

protected:

  //: Compute all internal data necessary to evolve the levelset surface
  void compute_internal_data();


  // ------------- INTERNAL DATA (computed from other member variables)----
  // Saving these variable internally will reduce time to allocate memory
  // when evolving the surface

  //: Hamilton-Jacobi flux
  vil_image_view<float > hj_flux_;

  //: Curvature image
  vil_image_view<float > curvature_;

  // 1st derivative by finite differences
  // \TODO we may not need all these
  // x-direction
  vil_image_view<float > dxp_;
  vil_image_view<float > dxm_;
  vil_image_view<float > dxc_;

  // y-direction
  vil_image_view<float > dyp_;
  vil_image_view<float > dym_;
  vil_image_view<float > dyc_;

  // 2nd derivative by finite difference
  // \TODO: we may not need all these
  vil_image_view<float > dxx_;
  vil_image_view<float > dyy_;
  vil_image_view<float > dxy_;
};



#endif // dbacm_standard_levelset_func_h_
