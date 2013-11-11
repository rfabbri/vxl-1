// This is brcv/seg/dbacm3d/dbacm3d_edge_lowmem_levelset_func.h

#ifndef dbacm3d_edge_lowmem_levelset_func_h_
#define dbacm3d_edge_lowmem_levelset_func_h_

//:
// \file
// \brief A levelset function that computes data at all levels
// \author Jesse Funaro (jdfunaro@lems.brown.edu)
// \date 12/22/2005
// \verbatim
//  Modifications
// \endverbatim

#include <dbacm3d/dbacm3d_levelset_func.h>
#include <vil3d/vil3d_image_view.h>
#include <vnl/vnl_vector.h>

//: A level set function which computed data at all levels
// Improvements to this include narrow-band scheme, sparse-field algo, etc.
class dbacm3d_edge_lowmem_levelset_func : public dbacm3d_levelset_func
{ 
public:
  //: Constructor
  dbacm3d_edge_lowmem_levelset_func(const vil3d_image_view<float> &edges,
      const vil3d_image_view<bool> &no_go_image 
      ):edges_(edges),no_go_image_(no_go_image) { }
  
  //: Destructor
  virtual ~dbacm3d_edge_lowmem_levelset_func() {}

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

  virtual void set_edge_troughs(vil3d_image_view<float>& edges){ edges_ = edges;}

  void set_twoD_hack(bool hack){this->twoD_hack_ = hack;}
  void set_edgeT(float T){this->edgeT_ = T;}

protected:

  //: Compute all internal data necessary to evolve the levelset surface
  void compute_internal_data();

  inline vnl_vector<float> closest_edge(const float * pixel_edges,
    const  vcl_ptrdiff_t & pstep_edges,
    const  vcl_ptrdiff_t & kstep_edges,
    const  vcl_ptrdiff_t & jstep_edges,
    const  vcl_ptrdiff_t & istep_edges);

  void compute_measures(float* pixel_phi,
                const vcl_ptrdiff_t& istep, const vcl_ptrdiff_t& jstep, const vcl_ptrdiff_t& kstep,
                const unsigned& i, const unsigned& j, const unsigned& k,
                const unsigned& ni, const unsigned& nj, const unsigned& nk, 
                float& hj_flux, float& curvature,
                float& dxc, float& dyc, float& dzc);

  // ------------- INTERNAL DATA (computed from other member variables)----
  // Saving these variable internally will reduce time to allocate memory
  // when evolving the surface

  bool twoD_hack_;
  bool edgeT_;

  //: temporary storage for phi calculation
  vil3d_image_view<float> phi2_;

  //: Edge Map
  vil3d_image_view<float> edges_;

  //:  Binary no-go zone
  vil3d_image_view<bool> no_go_image_; 

};



#endif // dbacm3d_edge_lowmem_levelset_func_h_
