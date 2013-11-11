// This is brcv/rec/dbskr/vis/dbskr_shock_patch_match_tableau.h
#ifndef dbskr_shock_patch_match_tableau_h_
#define dbskr_shock_patch_match_tableau_h_

//:
// \file
// \brief A tableau to display dbskr_shock_patch_match_storage objects.
// \author Ozge Can Ozcanli
// \date March 17 2007
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_parent_child_link.h>
#include <dbsk2d/dbsk2d_base_gui_geometry.h>

#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_event_condition.h>

#include <dbskr/vis/dbskr_shock_patch_match_tableau_sptr.h>

#include <dbskr/dbskr_shock_patch_sptr.h>
#include <dbskr/algo/dbskr_shock_patch_match_sptr.h>
#include <dbskr/pro/dbskr_shock_patch_match_storage.h>
#include <dbskr/vis/dbskr_shock_match_tableau.h>
#include <dbskr/vis/dbskr_shock_patch_tableau.h>

#include <vil/vil_image_view.h>

//: A tableau to display dbskr_shock_patch_match_storage objects
class dbskr_shock_patch_match_tableau : public dbskr_shock_match_tableau //,
                                        //public dbskr_shock_patch_tableau
{
  private:
  GLenum gl_mode;

public:
  
  dbskr_shock_patch_match_tableau();
  virtual ~dbskr_shock_patch_match_tableau() {}
  
  virtual bool handle( const vgui_event & );
  
  //: set the shock patch matching correspondence
  void set_match(dbskr_shock_patch_match_sptr m);  

  //: set the shock patch matching correspondence
  void set_curve_match(dbskr_shock_patch_curve_match_sptr m);  

  //: get the shock matching correspondence
  dbskr_shock_patch_match_sptr get_match(void) { return match_; }
  
  //: get the shock matching correspondence
  dbskr_shock_patch_curve_match_sptr get_curve_match(void) { return curve_match_; }
  
  //: Generate the popup menu for this tableau
  void get_popup(const vgui_popup_params& params, vgui_menu &menu);

  void draw_render();
  void draw_patch(dbskr_shock_patch_sptr shock_patch, int off_x, int off_y);
  void draw_patch_POLYGON(dbskr_shock_patch_sptr shock_patch, int off_x, int off_y, float r, float g, float b);
  void draw_patch_LINE_STRIP(dbskr_shock_patch_sptr shock_patch, int off_x, int off_y, float r, float g, float b, float line_width);
  void draw_boxes();
  void draw_poly_POLYGON(vsol_polygon_2d_sptr poly, int off_x, int off_y, float r, float g, float b);
  void draw_poly_LINE_STRIP(vsol_polygon_2d_sptr poly, int off_x, int off_y, float r, float g, float b, float line_width);

  void set_display_left(bool v) { display_left_ = v; }
  void set_display_right(bool v) { display_right_ = v; }
  void set_display_shocks(bool v) { display_shocks_ = v; }
  void set_display_curve_match(bool v) { display_curve_match_ = v; }

  void set_offset_x3(int x) { offset_x3 = x; }
  void set_offset_y3(int y) { offset_y3 = y; }

protected:
  dbskr_shock_patch_match_sptr match_;
  dbskr_shock_patch_curve_match_sptr curve_match_;
  
  vsol_polygon_2d_sptr trans_poly_;
  bool use_shock_points_to_compute_homography_;
  bool use_ransac_to_compute_homography_;
  bool use_similarity_transform_;
  unsigned sampling_interval_;

  bool display_left_, display_right_, display_shocks_, display_curve_match_;
  
  bool display_on_query_, display_boxes_;
  double offset_x3, offset_y3; /// < offset of the display on model

  bool display_outer_boundary_;   /// < display the outer polygon
  bool display_traced_boudary_;
  bool display_real_boundaries_;

  vgui_event_condition gesture_select_point_;
  vgui_event_condition gesture_select_match_triplet_;  // push left patch, right patch and transformed model patch
  vgui_event_condition gesture_clear_match_triplets_;
  vgui_event_condition next_patch_, next_match_;
  vgui_event_condition gesture_create_match_storage_; 
  //vgui_event_condition create_storage_;

  vgl_point_2d<double> mouse_pt_;
  float ix_, iy_;


  //double scurve_sample_ds_;
  bool match_with_circular_completions_, use_combined_edit_;

  vsol_box_2d_sptr model_box_, image_box_, trans_model_box_;
  vsol_polygon_2d_sptr model_box_poly_;
  vsol_polygon_2d_sptr model_poly_, trans_model_poly_;

  vcl_vector<vsol_polygon_2d_sptr> left_patches_;
  vcl_vector<vsol_polygon_2d_sptr> right_patches_;
  vcl_vector<vsol_polygon_2d_sptr> transformed_models_;

public:
  vsol_box_2d_sptr detection_box_;
  float threshold_;
  int N_, k_;  // require at least k model patches to have top N matches less than threshold

};

//: Create a smart-pointer to a dbsk2d_shock_tableau.
struct dbskr_shock_patch_match_tableau_new : public dbskr_shock_patch_match_tableau_sptr
{
  typedef dbskr_shock_patch_match_tableau_sptr base;

  //: Constructor - creates a pointer to a dbskr_shock_match_tableau
  dbskr_shock_patch_match_tableau_new() : base(new dbskr_shock_patch_match_tableau()) { }
};

#endif //dbskr_shock_patch_match_tableau_h_
