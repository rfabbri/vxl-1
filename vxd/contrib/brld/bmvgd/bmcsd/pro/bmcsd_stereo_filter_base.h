// This is bmcsdstereo_filter.h
#ifndef bmcsdstereo_filter_base_h
#define bmcsdstereo_filter_base_h
//:
//\file
//\brief Dbpro process for curve stereo
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@gmail.com)
//\date 09/01/2009 08:31:17 AM PDT
//
#include <bprod/bprod_process.h>
#include <bmcsd/bmcsd_discrete_corresp.h>
#include <bmcsd/bmcsd_curve_3d_attributes.h>
#include <bmcsd/bmcsd_stereo_views_sptr.h>
#include <bmcsd/algo/bmcsd_odt_curve_stereo.h>
#include <bmcsd/pro/bmcsd_load_camera_source.h>
#include <bmcsd/pro/bmcsd_load_edg_source.h>
#include <bmcsd/pro/bmcsd_load_vsol_polyline_source.h>
#include <bmcsd/pro/bmcsd_fragment_tangents_filter.h>

//: This process takes matches 2 views using a number of confirmation views.
class bmcsd_stereo_filter_base : public bprod_filter {
protected: // This is is only intended as a base class
  //  bmcsd_stereo_filter_base  () 
  //    : has_cvlet_(false)
  //  {}
  bmcsd_stereo_filter_base(bmcsd_odt_curve_stereo *s)
  :
    s_(s),
    has_cvlet_(false)
  {}

public:
  typedef enum { CAM_ID, EDG_ID, DT_ID, LBL_ID, CVLET_ID } confirmation_input_id;
  typedef enum { CAM_ID0, FRAG_ID0, TGT_ID0,  
                 CAM_ID1, FRAG_ID1, TGT_ID1 } keyframes_input_id;

  // default, must be called explicitly in derived classes
  bprod_signal execute() override
  {
    get_cameras();
    get_edgemaps();
    get_curves_and_tangents();
    get_dt_label();
    if (has_cvlet_)
      get_curvelets();
    return BPROD_VALID;
  }

  void setup_inputs(
        const bmcsd_stereo_views_sptr &views,
        std::vector<bprod_process_sptr> &cam_src, 
        std::vector<bprod_process_sptr> &edg_src, 
        std::vector<bprod_process_sptr> &edg_dt, 
        std::vector<bprod_process_sptr> &frag_src,
        std::vector<bprod_process_sptr> &cvlet_src,
        std::vector<bprod_process_sptr> &frag_tangents
        );

  bmcsd_stereo_views_sptr v_;
  bmcsd_odt_curve_stereo *s_;

private:
  unsigned sources_per_confirmation_view_;
  unsigned confirmation_view_input_offset_;
  bool has_cvlet_;

  void get_cameras();
  void get_edgemaps();
  void get_curves_and_tangents();
  void get_dt_label();
  void get_curvelets();
};

#endif // bmcsdstereo_filter_base_h
