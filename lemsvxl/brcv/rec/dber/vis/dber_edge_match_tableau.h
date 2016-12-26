// This is brcv/rec/dbskr/vis/dber_edge_match_tableau.h
#ifndef dber_edge_match_tableau_h_
#define dber_edge_match_tableau_h_

//:
// \file
// \brief A tableau to display dber_edge_match_storage objects.
// \author Ozge Can Ozcanli
// \date Oct 23 2006
//
// \verbatim
//  Modifications
//    O. C. Ozcanli  10/23/06       Made into a tool to display costs
//                                  Left click on an edge point to display its matching cost and rays
//
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_event_condition.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_2d.h>

#include <dber/vis/dber_edge_match_tableau_sptr.h>
#include <dber/pro/dber_edge_match_storage.h>

#include <vsol/vsol_box_2d_sptr.h>

//: A tableau to display dber_edge_match_storage objects
class dber_edge_match_tableau : public vgui_tableau 
{
private:
  GLenum gl_mode;
protected:
  
  float rnd_colormap[101][3];               ///< random colormap
  float rnd_colormap2[5000][3];             ///< random colormap

 // dbskr_sm_cor_sptr sm_cor_;

public:
  
  dber_edge_match_tableau(vcl_vector<vsol_line_2d_sptr>& l1, 
                          vcl_vector<vsol_line_2d_sptr>& l2,
                          vcl_vector<unsigned>& a);
  virtual ~dber_edge_match_tableau() {}
  
  virtual bool handle( const vgui_event & );

  //: Generate the popup menu for this tableau
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  void draw_render();

  void set_dominant_dir1(vgl_line_2d<double> d) { dominant_dir1_ = d; }
  void set_dominant_dir2(vgl_line_2d<double> d) { dominant_dir2_ = d; }

protected:

  vcl_vector<vsol_line_2d_sptr>& lines1_;
  vcl_vector<vsol_line_2d_sptr>& lines2_;
  vcl_vector<unsigned>& assign_;

  vgl_line_2d<double> dominant_dir1_, dominant_dir2_;

  vsol_box_2d_sptr box1_; // to help offseting

  double offset_x1, offset_y1; ///< offset of the first edgel set
  double offset_x2, offset_y2; ///< offset of the second edgel set
  unsigned int select_color_;

  float ix_, iy_;
  vgl_point_2d<double> mouse_pt_;

  bool selected_edgel_;
  vgl_point_2d<double> selected_pt1_;
  vgl_point_2d<double> selected_pt2_;

  vgui_event_condition gesture_select;
  vgui_event_condition gesture_splice1;
  vgui_event_condition gesture_splice2;

};


//: Create a smart-pointer to a dbsk2d_shock_tableau.
struct dber_edge_match_tableau_new : public dber_edge_match_tableau_sptr
{
  typedef dber_edge_match_tableau_sptr base;

  //: Constructor - creates a pointer to a dber_edge_match_tableau
  dber_edge_match_tableau_new(vcl_vector<vsol_line_2d_sptr>& l1, 
                              vcl_vector<vsol_line_2d_sptr>& l2,
                              vcl_vector<unsigned>& a) : base(new dber_edge_match_tableau(l1, l2, a)) { }
};

#endif //dber_edge_match_tableau_h_
