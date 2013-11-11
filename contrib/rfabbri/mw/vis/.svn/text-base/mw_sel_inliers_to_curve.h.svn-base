// This is brcv/seg/dbdet/vis/mw_sel_inliers_to_curve.h
#ifndef mw_sel_inliers_to_curve_h
#define mw_sel_inliers_to_curve_h
//:
//\file
//\brief Tool to plot inlier linking hypotheses to a vsol curve
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date Wed May  6 09:11:33 EDT 2009
//

#include <bvis1/bvis1_tool.h>
#include <dbdet/vis/dbdet_sel_explorer_tool.h>
#include <vil/vil_image_view.h>

//: Given an input vsol curve (polyline for now), show all curvelets that fall
// within a distance threshold.
class mw_sel_inliers_to_curve  : public dbdet_sel_explorer_tool {
public:
  //: Constructor
  mw_sel_inliers_to_curve();
 
  //: Destructor
  virtual ~mw_sel_inliers_to_curve() {}
  
  //: Returns the string name of this tool
  vcl_string name() const;

  //: Handle events.
  bool handle( const vgui_event & e, 
               const bvis1_view_tableau_sptr& view );

  void activate ();
  void deactivate ();

  virtual void get_popup( const vgui_popup_params& params, vgui_menu &menu );


protected:
  // ------ Data ------
  vcl_vector<vcl_vector<vsol_point_2d_sptr> > curves_;
  unsigned current_curve_id_;
  double tau_distance_;
  double tau_dtheta_;

  vil_image_view<vxl_uint_32> dt_; //: distance transform images 
  vil_image_view<vxl_uint_32> label_; //: closest label transform images 

  // ------ Methods ------
  void draw_curvelets(const curvelet_list &curvelets);
};


#endif // mw_sel_inliers_to_curve_h
