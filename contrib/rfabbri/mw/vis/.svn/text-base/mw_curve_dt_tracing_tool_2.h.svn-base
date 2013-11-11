// This is mw_curve_dt_tracing_tool_2.h
#ifndef mw_curve_dt_tracing_2_h
#define mw_curve_dt_tracing_2_h
//:
//\file
//\brief Tool for exploring multiview curve correspondence
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 06/21/09 22:29:25 EDT
//

#include <mw/vis/mw_curve_tracing_tool_2.h>

class mw_odt_curve_stereo;

//: Same as \c mw_curve_tracing_tool, but reads in distance transform / label
// images as computed by the distance transform process from binarized edge maps
// and uses them for matching
class mw_curve_dt_tracing_tool_2 : public mw_curve_tracing_tool_2 {
public:
  //: Constructor
  mw_curve_dt_tracing_tool_2();

  //: Destructor
  ~mw_curve_dt_tracing_tool_2() {}

  //: Handle events.
  bool handle( const vgui_event & e,
               const bvis1_view_tableau_sptr& view );
 
  //: Returns the string name of this tool
  vcl_string name() const;

  void activate();
  void deactivate();

  virtual void get_popup( const vgui_popup_params& params, vgui_menu &menu );

protected:

  //: Common work to all constructors
  void init();

private:
  //: Retrieves the images from the repository
  void get_images();
  //: Retrieves the edgemaps from the repository
  void get_edgemaps();
  //: Retrieves the tangents at each curve fragment sample from the repository
  void get_tangents();

  typedef enum {MW_DISTANCE_DT, MW_ORIENTED_DT, MW_ORIENTED_DT_EXTRAS} stereo_matcher_type;

  //: Matches the selected subcurve to another subcurve in its epipolar beam such
  // that the overlaps reproject with the maximum number of inliers. The inliers
  // are measured from the distance map.
  //: \param[in] oriented: true if orientation should be used in matching.
  bool match_using_dt(stereo_matcher_type t = MW_DISTANCE_DT);

  //: Compute tangent edgels to the selected curves and reprojections
  // tgts[iview][isample]
  bool compute_selected_tangents(vcl_vector<vcl_vector<vsol_line_2d_sptr> > *tgts) const;

  //: Compute & display tangent edgels to the selected curves and reprojections.
  void display_selected_tangents();

  //: Pointer to the leaf implementation class; contents are destroyed by the
  // base GUI class throught the s_ pointer.
  mw_odt_curve_stereo *s_dt_;

  //: Best curve fragment after a stereo match
  bgui_vsol_soview2D_polyline *best_match_soview_;

  //: Display objects of tangents to each selected curve and their reprojections
  vcl_vector<vcl_vector<bgui_vsol_soview2D_line_seg* > > tgts_soview_; 

  //: see mw_dt_curve_stereo::set_distance_thresh(); this is not squared.
  double tau_distance_;
  //: the threshold in angular difference for wich an edgel is considered
  // an inlier to the reprojected curve point-tangent sample in each view, in
  // degrees.
  double tau_dtheta_;
  //: see mw_odt_curve_stereo:: set_min_inliers_perview()
  double tau_min_inliers_per_view_;
  //: see mw_dt_curve_stereo::set_distance_thresh(); this is not squared.
  double tau_min_total_inliers_;

  double tau_min_epiangle_;
};

#endif // mw_curve_dt_tracing_2_h
