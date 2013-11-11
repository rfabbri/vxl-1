// This is brcv/seg/dbacm3d/dbacm3d_levelset_driver.h
#ifndef dbacm3d_levelset_driver_h_
#define dbacm3d_levelset_driver_h_
//:
// \file
// \brief A segmentation driver using levelset function
// \author Jesse Funaro (jdfunaro@lems.brown.edu)
//         (after dbacm/dbacm_levelset_driver by  Nhon Trinh (ntrinh@lems.brown.edu))
// \date 12/22/2005
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_2d.h>
#include <vil3d/vil3d_image_view.h>
#include <dbacm3d/dbacm3d_levelset_func.h>
#include <dbacm3d/dbacm3d_levelset_func_sptr.h>


//: A segmentation driver using levelset function
class dbacm3d_levelset_driver : public vbl_ref_count
{
 public:
  //: Constructor
   dbacm3d_levelset_driver():vbl_ref_count(){}
 
  //: Destructor
  virtual ~dbacm3d_levelset_driver() {}

  // ================================================================
  // DATA ACCESS
  // ================================================================

  //: Return image to segment
  vil3d_image_view<float > image(){ return this->image_; }

  //: Set image to segment
  void set_image(const vil3d_image_view<float >& image )
  { this->image_ = image; }

  //: Return smart pointer to the levelset function
  dbacm3d_levelset_func_sptr levelset_func(){ return this->levelset_func_; }
  const dbacm3d_levelset_func_sptr levelset_func() const 
  {return this->levelset_func_;}

  //: Return level set surface
  vil3d_image_view<float > levelset_surf() const
  {return this->levelset_func()->levelset_surf(); }

  
  //: Return (maximum) number of iterations to run
  unsigned int num_iterations() const {return this->num_iterations_; }

  //: Set number of iterations
  void set_num_iterations(unsigned int num_iters)
  {this->num_iterations_ = num_iters; }

  //: feature map given by user
  vil3d_image_view<bool > feature_map(){ return this->fmap_; }

  //: set feature map
  void set_feature_map(vil3d_image_view<bool >& new_fmap)
  { this->fmap_ = new_fmap; }


  // ================================================================
  // UTILITIES
  // ================================================================

  //: Set initial levelset function
  virtual void init_levelset_surf(const vil3d_image_view<float >& init_phi) = 0;

  //: Evolve the levelset function till convergence or reaching maximum number
  // of iterations.
  virtual void evolve(bool verbose) = 0;

protected:
  //: Levelset function - the mechanism to evolve the surface
  dbacm3d_levelset_func_sptr levelset_func_;

  //: image to run segmentation on
  vil3d_image_view<float > image_;

  //: feature map given by user
  vil3d_image_view<bool > fmap_;

  //: (maximum) number of iterations
  unsigned int num_iterations_;

  // RMS error (to be implemented) ...
};

#endif // dbacm3d_levelset_driver_h_

