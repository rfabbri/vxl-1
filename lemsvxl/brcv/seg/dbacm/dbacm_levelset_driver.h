// This is brcv/seg/dbacm/dbacm_levelset_driver.h
#ifndef dbacm_levelset_driver_h_
#define dbacm_levelset_driver_h_
//:
// \file
// \brief A segmentation driver using levelset function
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 11/28/2005
//
// \verbatim
//  Modifications
// \endverbatim


#include <vbl/vbl_ref_count.h>
#include <vgl/vgl_point_2d.h>
#include <vil/vil_image_view.h>
#include <dbacm/dbacm_levelset_func.h>
#include <dbacm/dbacm_levelset_func_sptr.h>


//: A segmentation driver using levelset function
class dbacm_levelset_driver : public vbl_ref_count
{
 public:
  //: Constructor
   dbacm_levelset_driver(){}
 
  //: Destructor
  virtual ~dbacm_levelset_driver() {}

  // ================================================================
  // DATA ACCESS
  // ================================================================

  //: Return image to segment
  vil_image_view<float > image(){ return this->image_; }

  //: Set image to segment
  void set_image(const vil_image_view<float >& image )
  { this->image_ = image; }

  //: Return smart pointer to the levelset function
  dbacm_levelset_func_sptr levelset_func(){ return this->levelset_func_; }
  const dbacm_levelset_func_sptr levelset_func() const 
  {return this->levelset_func_;}

  //: Return level set surface
  vil_image_view<float > levelset_surf() const
  {return this->levelset_func()->levelset_surf(); }

  
  //: Return (maximum) number of iterations to run
  unsigned int num_iterations() const {return this->num_iterations_; }

  //: Set number of iterations
  void set_num_iterations(unsigned int num_iters)
  {this->num_iterations_ = num_iters; }

  //: feature map given by user
  vil_image_view<bool > feature_map(){ return this->fmap_; }

  //: set feature map
  void set_feature_map(vil_image_view<bool >& new_fmap)
  { this->fmap_ = new_fmap; }


  // ================================================================
  // UTILITIES
  // ================================================================

  //: Set initial levelset function
  virtual void init_levelset_surf(const vil_image_view<float >& init_phi) = 0;

  //: Evolve the levelset function till convergence or reaching maximum number
  // of iterations.
  virtual void evolve(bool verbose) = 0;

protected:
  //: Levelset function - the mechanism to evolve the surface
  dbacm_levelset_func_sptr levelset_func_;

  //: image to run segmentation on
  vil_image_view<float > image_;

  //: feature map given by user
  vil_image_view<bool > fmap_;


  //: (maximum) number of iterations
  unsigned int num_iterations_;

  // RMS error (to be implemented) ...
};

#endif // dbacm_levelset_driver_h_

