// This is brcv/seg/dbdet/vis/dbdet_edgemap_tableau.h
#ifndef dbdet_edgemap_tableau_h_
#define dbdet_edgemap_tableau_h_

//:
// \file
// \brief A tableau to display dbdet_edgemap_storage objects.
// \author Amir Tamrakar
// \date 09/11/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgui/vgui_gl.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <vgui/vgui_event_condition.h>

#include <vnl/vnl_vector_fixed.h>

#include "dbdet_edgemap_tableau_sptr.h"
#include <dbdet/edge/dbdet_edgemap_sptr.h>

class dbdet_edgemap_parameter_bars;

//: A tableau to display dbdet_edgemap_storage objects
class dbdet_edgemap_tableau : public vgui_tableau 
{
public:

  //: Constructor
  dbdet_edgemap_tableau();
  virtual ~dbdet_edgemap_tableau();

  //: handle the render and select events
  virtual bool handle( const vgui_event & );

  //: return the edgemap
  dbdet_edgemap_sptr get_edgemap() { return edgemap_; }
  //: set the edgeamp
  void set_edgemap( dbdet_edgemap_sptr new_edgemap ) { edgemap_ = new_edgemap; }

  //: Generate the popup menu for this tableau
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  // rendering funtions

  //: render the edgels
  void draw_edgels();

  void set_style(vnl_vector_fixed<float,3> color, float point_size, float line_width, float line_length)
  {curr_color_=color; point_size_=point_size; line_width_=line_width; line_length_=line_length;}

  float point_size(){ return point_size_; }
  float line_width(){ return line_width_; }
  float line_length(){ return line_length_; }
  float curr_color(unsigned i) { return curr_color_[i]; }

  void set_mixed_threshold(float l1, float l2, float mix_thresh)
  { l1_ = l1; l2_ = l2; mix_thresh_ = mix_thresh; }
  
  void get_mixed_threshold(float& l1, float& l2, float& mix_thresh)
  { l1 = l1_; l2 = l2_; mix_thresh = mix_thresh_; }

  float get_threshold() { return threshold_; }
  float get_d2f_threshold() { return d2f_thresh_; }

  void set_threshold(float th) { threshold_ = th; }
  void set_d2f_threshold(float th) { d2f_thresh_ = th; }

  void display_image_grid();

private:

  dbdet_edgemap_sptr edgemap_;  ///< the edge map class

  bool display_anchor_points_; ///< display edgel grid anchor points
  bool display_points_;  ///< display the edgel centers
  float point_size_;     ///< size of the points displayed
  float line_width_;     ///< width of the edges
  float line_length_;   ///< length of the edges
  vnl_vector_fixed<float,3> curr_color_; ///< color of the edges

  float threshold_;     ///< to mask the edges by their strength
  float d2f_thresh_;    ///< to mask the edges by their saliency
  
  bool use_mix_thresh_;
  float l1_, l2_, mix_thresh_; ///< mixture threshold params

  //these are for querying the sel object 
  vgui_event_condition left_click_;
  dbdet_edgel* cur_edgel;  ///< currently selected edgel

  bool display_bars;
  dbdet_edgemap_parameter_bars* bars;

  int local_zoom_factor;
  bool display_image_grid_;
  bool display_uncertainty_;
};


//: Create a smart-pointer to a dbdet_edgemap_tableau.
struct dbdet_edgemap_tableau_new : public dbdet_edgemap_tableau_sptr
{
  typedef dbdet_edgemap_tableau_sptr base;

  //: Constructor - creates a pointer to a dbdet_edgemap_tableau
  dbdet_edgemap_tableau_new() : base(new dbdet_edgemap_tableau()) { }
};

#endif //dbdet_edgemap_tableau_h_
