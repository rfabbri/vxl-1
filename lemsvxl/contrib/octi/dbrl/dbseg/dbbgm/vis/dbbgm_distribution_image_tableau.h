// This is brcv/seg/dbbgm/vis/dbbgm_distribution_image_tableau.h
#ifndef dbbgm_distribution_image_tableau_h_
#define dbbgm_distribution_image_tableau_h_
//:
// \file
// \brief  Tableau which renders the given image using an image_renderer.
// \author Matt Leotta
//
//  Contains classes  dbbgm_distribution_image_tableau  dbbgm_distribution_image_tableau_new
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgui/vgui_image_tableau.h>
#include "dbbgm_distribution_image_tableau_sptr.h"
#include <dbbgm/dbbgm_distribution_image_sptr.h>
#include <dbsta/dbsta_functor.h>


struct dbbgm_distribution_image_tableau_new;

//: Tableau which renders a distribution image in various ways
class dbbgm_distribution_image_tableau
  : public vgui_image_tableau
{
  //: The distribution image
  dbbgm_distribution_image_sptr dimg_;
  
  //: Active index into a mixture
  // \note currently does not support nested mixtures
  unsigned int active_idx_;
  
  //: The functor used to generate the current display image
  vbl_smart_ptr<dbsta_functor<float> > active_functor_f_;
  vbl_smart_ptr<dbsta_functor<double> > active_functor_d_;

 public:
 
  //: the draw color 
  float fail_color_[3];  
  
  //: Convert YUV to RGB for display
  bool color_space_YUV_;
 
  //: Returns the type of this tableau ('dbbgm_distribution_image_tableau').
  vcl_string type_name() const;

  //: Apply the functor to the distribution image and display the result
  bool functor_image(const dbsta_functor<float>& functor);
  bool functor_image(const dbsta_functor<double>& functor);
  
  dbbgm_distribution_image_sptr dist_image() const { return dimg_; }
  
  //: Return the active mixture index
  unsigned int active_idx() const { return active_idx_; }
  
  //: Set the active mixture index
  void set_active_idx(unsigned int idx) { active_idx_ = idx; }
  
  //: Set the active mixture functor
  void set_active_functor(const vbl_smart_ptr<dbsta_functor<float> >& functor) { active_functor_f_ = functor;
                                                                                 active_functor_d_ = NULL; }
  void set_active_functor(const vbl_smart_ptr<dbsta_functor<double> >& functor) { active_functor_f_ = NULL;
                                                                                  active_functor_d_ = functor; }

  //: Use the active functor to update the display image
  bool update();
  
  //: Handle all events sent to this tableau.
  //  In particular, use draw events to render the image contained in
  //  this tableau.
  virtual bool handle( vgui_event const& e );
  
  //: Generate the popup menu
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);

 protected:
  friend struct dbbgm_distribution_image_tableau_new;

  //: Constructor - don't use this, use dbbgm_distribution_image_tableau_new.
  //  Creates a tableau with the given distribution image
  dbbgm_distribution_image_tableau(const dbbgm_distribution_image_sptr& dimg = NULL);
  
  

 protected:
  //: Destructor - called by dbbgm_distribution_image_tableau_sptr.
  virtual ~dbbgm_distribution_image_tableau();
};


//: Creates a smart-pointer to a dbbgm_distribution_image_tableau.
struct dbbgm_distribution_image_tableau_new
  : public dbbgm_distribution_image_tableau_sptr
{
  //:  Constructor - creates a tableau displaying the given distribution image.
  dbbgm_distribution_image_tableau_new( const dbbgm_distribution_image_sptr& dimg = NULL )
    : dbbgm_distribution_image_tableau_sptr( new dbbgm_distribution_image_tableau(dimg) )
    { }
};

#endif // dbbgm_distribution_image_tableau_h_
