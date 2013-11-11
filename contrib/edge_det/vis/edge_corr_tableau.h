// This is vis/edge_corr_tableau.h
#ifndef edge_corr_tableau_h_
#define edge_corr_tableau_h_

//:
// \file
// \brief A tableau to display edge_corr_storage objects.
// \author Amir Tamrakar
// \date 12/14/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <vgui/vgui_gl.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <vgui/vgui_event_condition.h>

#include <vnl/vnl_vector_fixed.h>

#include "edge_corr_tableau_sptr.h"
#include <dbdet/edge/dbdet_edgemap_sptr.h>

#include <edge_det/pro/edge_corr_storage_sptr.h>

//: A tableau to display dbdet_edgemap_storage objects
class edge_corr_tableau : public vgui_tableau 
{
public:

  //: Constructor
  edge_corr_tableau(edge_corr_storage_sptr edge_corr);
  virtual ~edge_corr_tableau();

  //: handle the render and select events
  virtual bool handle( const vgui_event & );

  //: Generate the popup menu for this tableau
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  // rendering funtions

  //: render the edgels
  void draw_edgels();

  void set_style(vnl_vector_fixed<float,3> TPcolor, 
                 vnl_vector_fixed<float,3> FPcolor, 
                 vnl_vector_fixed<float,3> TNcolor,
                 float point_size, float line_width, float line_length);

  float point_size(){ return point_size_; }
  float line_width(){ return line_width_; }
  float line_length(){ return line_length_; }

public:

  dbdet_edgemap_sptr edgemap1_;  ///< the edge map class
  dbdet_edgemap_sptr edgemap2_;  ///< the edge map class

  vcl_vector<int> & matched1_;
  vcl_vector<int> & matched2_;

  bool display_points_;  ///< display the edgel centers
  float point_size_;     ///< size of the points displayed
  float line_width_;     ///< width of the edges
  float line_length_;   ///< length of the edges

  vnl_vector_fixed<float,3> TP_color_; ///< color of the edges
  vnl_vector_fixed<float,3> FP_color_; ///< color of the edges
  vnl_vector_fixed<float,3> TN_color_; ///< color of the edges

  float threshold1_;     ///< to mask the edges by their strength
  float threshold2_;    ///< 
  
  //these are for querying the sel object 
  vgui_event_condition left_click_;
  dbdet_edgel* cur_edgel;  ///< currently selected edgel

  int local_zoom_factor;

  bool disp_correspondence; 
};


//: Create a smart-pointer to a edge_corr_tableau.
struct edge_corr_tableau_new : public edge_corr_tableau_sptr
{
  typedef edge_corr_tableau_sptr base;

  //: Constructor - creates a pointer to a edge_corr_tableau
  edge_corr_tableau_new(edge_corr_storage_sptr edge_corr) : base(new edge_corr_tableau(edge_corr)) { }
};

#endif //edge_corr_tableau_h_
