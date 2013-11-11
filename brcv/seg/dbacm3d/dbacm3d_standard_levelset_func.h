// This is brcv/seg/dbacm3d/dbacm3d_standard_levelset_func.h

#ifndef dbacm3d_standard_levelset_func_h_
#define dbacm3d_standard_levelset_func_h_

//:
// \file
// \brief A levelset function that computes data at all levels
// \author Jesse Funaro (jdfunaro@lems.brown.edu)
//         (after dbacm/dbacm_standard_levelset_func by  Nhon Trinh (ntrinh@lems.brown.edu))
// \date 12/22/2005
// \verbatim
//  Modifications
// \endverbatim

#include <dbacm3d/dbacm3d_levelset_func.h>
#include <vil3d/vil3d_image_view.h>

//: A level set function which computes data at all levels
// Improvements to this include narrow-band scheme, sparse-field algo, etc.
class dbacm3d_standard_levelset_func : public dbacm3d_levelset_func
{ 
public:
  //: Constructor
  dbacm3d_standard_levelset_func() {}
  
  //: Destructor
  virtual ~dbacm3d_standard_levelset_func() {}

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
  vil3d_image_view<float> hj_flux_;

  //: Curvature image
  vil3d_image_view<float> curvature_;

  // 1st derivative by finite differences
  // (center differences)
  vil3d_image_view<float> dxc_;
  vil3d_image_view<float> dyc_;
  vil3d_image_view<float> dzc_;
};



#endif // dbacm3d_standard_levelset_func_h_
