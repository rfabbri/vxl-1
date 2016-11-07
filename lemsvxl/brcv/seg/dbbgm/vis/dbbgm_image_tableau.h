// This is brcv/seg/dbbgm/vis/dbbgm_image_tableau.h
#ifndef dbbgm_image_tableau_h_
#define dbbgm_image_tableau_h_
//:
// \file
// \brief  Tableau which renders a background model
// \author Matt Leotta
// \data   1/30/06
//
//  Contains classes  dbbgm_image_tableau  dbbgm_image_tableau_new
//
// \verbatim
//  Modifications
// \endverbatim

#include <bgui/bgui_image_tableau.h>
#include "dbbgm_image_tableau_sptr.h"
#include <bbgm/bbgm_image_sptr.h>
#include <bbgm/bbgm_viewer_sptr.h>


struct dbbgm_image_tableau_new;

//: Tableau which renders a distribution image in various ways
class dbbgm_image_tableau
  : public bgui_image_tableau
{
  //: The distribution image
  bbgm_image_sptr dimg_;

  //: Active index into a mixture
  // \note currently does not support nested mixtures
  unsigned int active_idx_;

  //: The viewer used to generate the current display image
  bbgm_viewer_sptr active_viewer_;

  //: A vector of all viewers to try
  vcl_vector<bbgm_viewer_sptr> all_viewers_;

 public:

  //: the draw color 
  float fail_color_[3];  

  //: Convert YUV to RGB for display
  bool color_space_YUV_;

  //: Returns the type of this tableau ('dbbgm_image_tableau').
  vcl_string type_name() const;

  //: Apply the viewer to the distribution image and display the result
  bool functor_image(const bbgm_viewer_sptr& viewer);

  bbgm_image_sptr dist_image() const { return dimg_; }

  //: Return the active mixture index
  unsigned int active_idx() const { return active_idx_; }

  //: Set the active mixture index
  void set_active_idx(unsigned int idx);

  //: Set the active mixture functor
  void set_active_viewer(const bbgm_viewer_sptr& viewer);

  //: Use the active functor to update the display image
  bool update();

  //: Handle all events sent to this tableau.
  //  In particular, use draw events to render the image contained in
  //  this tableau.
  virtual bool handle( vgui_event const& e );

  //: Generate the popup menu
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  const vcl_vector<bbgm_viewer_sptr>& viewers() const { return all_viewers_; }

 protected:
  friend struct dbbgm_image_tableau_new;

  //: Constructor - don't use this, use dbbgm_image_tableau_new.
  //  Creates a tableau with the given distribution image
  dbbgm_image_tableau(const bbgm_image_sptr& dimg = NULL);



 protected:
  //: Destructor - called by dbbgm_image_tableau_sptr.
  virtual ~dbbgm_image_tableau();
};


//: Creates a smart-pointer to a dbbgm_image_tableau.
struct dbbgm_image_tableau_new
  : public dbbgm_image_tableau_sptr
{
  //:  Constructor - creates a tableau displaying the given distribution image.
  dbbgm_image_tableau_new( const bbgm_image_sptr& dimg = NULL )
    : dbbgm_image_tableau_sptr( new dbbgm_image_tableau(dimg) )
    { }
};

#endif // dbbgm_image_tableau_h_
