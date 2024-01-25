#include "bmcsd_stereo_filter_base.h"
#include <bmcsd/bmcsd_view_set.h>
#include <bbld/bbld_subsequence.h>

void bmcsd_stereo_filter_base::
setup_inputs(
      const bmcsd_stereo_views_sptr &views,
      std::vector<bprod_process_sptr> &cam_src, 
      std::vector<bprod_process_sptr> &edg_src, 
      std::vector<bprod_process_sptr> &edg_dt, 
      std::vector<bprod_process_sptr> &frag_src,
      std::vector<bprod_process_sptr> &cvlet_src,
      std::vector<bprod_process_sptr> &frag_tangents
      )
{
  v_ = views;
  assert(v_->stereo0() < cam_src.size());
  assert(v_->stereo1() < cam_src.size());
  assert(cam_src.size() == edg_src.size());
  assert(cam_src.size() == edg_dt.size());
  assert(cam_src.size() == frag_src.size());
  assert(cam_src.size() == frag_tangents->size());

  // Connect inputs from the two views to be matched:
  connect_input(CAM_ID0, cam_src[v_->stereo0()], 0);
  connect_input(FRAG_ID0, frag_src[v_->stereo0()], 0);
  connect_input(TGT_ID0, frag_tangents[v_->stereo0()], 0);

  connect_input(CAM_ID1, cam_src[v_->stereo1()], 0);
  connect_input(FRAG_ID1, frag_src[v_->stereo1()], 0);
  connect_input(TGT_ID1, frag_tangents[v_->stereo1()], 0);
  if (cvlet_src.size() == cam_src.size())
    has_cvlet_ = true;


  sources_per_confirmation_view_ = 4;
  if (has_cvlet_)
    sources_per_confirmation_view_++;

  confirmation_view_input_offset_ = 6;

  // Connect inputs from all the other views
  for (unsigned i=0; i < v_->num_confirmation_views(); ++i) {
    unsigned v = v_->confirmation_view(i);
    assert (v < cam_src.size());

    unsigned offset = confirmation_view_input_offset_ 
                  + i*sources_per_confirmation_view_;
    connect_input(offset + CAM_ID,  cam_src[v], 0);
    connect_input(offset + EDG_ID,  edg_src[v], 0);
    connect_input(offset + DT_ID,   edg_dt[v], 0);
    connect_input(offset + LBL_ID,  edg_dt[v], 1);
    if (has_cvlet_)
      connect_input(offset + CVLET_ID,  cvlet_src[v], 0);
  }
  s_->set_nviews(2 + v_->num_confirmation_views());
}

void bmcsd_stereo_filter_base::
get_cameras()
{
  std::vector<bdifd_camera> cams(s_->nviews());

  assert(input_type_id(CAM_ID0) == typeid(vpgl_perspective_camera<double>));
  cams[0].set_p(input<vpgl_perspective_camera<double> >(CAM_ID0));
  assert(input_type_id(CAM_ID1) == typeid(vpgl_perspective_camera<double>));
  cams[1].set_p(input<vpgl_perspective_camera<double> >(CAM_ID1));

  for (unsigned i=0; i < v_->num_confirmation_views(); ++i) {
    unsigned offset = confirmation_view_input_offset_ 
                    + i*sources_per_confirmation_view_;
    assert(input_type_id(offset + CAM_ID) 
        == typeid(vpgl_perspective_camera<double>));

    cams[i+2].set_p(input<vpgl_perspective_camera<double> >(offset + CAM_ID));
  }

  s_->set_cams(cams);
}

void bmcsd_stereo_filter_base::
get_edgemaps()
{
  std::vector<sdet_edgemap_sptr> em(s_->nviews());
  
  for (unsigned i=0; i < v_->num_confirmation_views(); ++i) {
    unsigned offset = confirmation_view_input_offset_ 
                    + i*sources_per_confirmation_view_;
    assert( input_type_id(offset + EDG_ID) == typeid(sdet_edgemap_sptr) );
    em[i+2] = input< sdet_edgemap_sptr >(offset + EDG_ID);
  }
  s_->set_all_edgemaps(em);
}

void bmcsd_stereo_filter_base::
get_curvelets()
{
  std::vector<sdetd_sel_storage_sptr> sels(s_->nviews());
  
  for (unsigned i=0; i < v_->num_confirmation_views(); ++i) {
    unsigned offset = confirmation_view_input_offset_ 
                    + i*sources_per_confirmation_view_;
    assert( input_type_id(offset + CVLET_ID) == typeid(sdetd_sel_storage_sptr) );
    sels[i+2] = input< sdetd_sel_storage_sptr >(offset + CVLET_ID);
  }
  s_->set_all_sels(sels);
}

void bmcsd_stereo_filter_base::
get_dt_label()
{
  typedef vil_image_view<vxl_uint_32> dt_t;
  typedef vil_image_view<unsigned> label_t;
  std::vector< dt_t > dts(s_.nviews());
  std::vector< label_t > labels(s_.nviews());

  // dts[0] == null;
  // labels [0] == null;
  for (unsigned i=0; i < v_->num_confirmation_views(); ++i) {
    unsigned offset = confirmation_view_input_offset_ + i*sources_per_confirmation_view_;
    assert(input_type_id(offset + DT_ID) == typeid( dt_t ));
    assert(input_type_id(offset + LBL_ID) == typeid( label_t ));
    dts[i+2] = input< dt_t >(offset + DT_ID);
    labels[i+2] = input< label_t >(offset + LBL_ID);
  }
  s_.set_all_dt_label(dts, labels);
}


void bmcsd_stereo_filter_base::
get_curves_and_tangents()
{
  std::vector<std::vector< vsol_polyline_2d_sptr > > curves (s_.nviews()); 

  assert(input_type_id(FRAG_ID0) == typeid(std::vector< vsol_polyline_2d_sptr >));
  assert(input_type_id(FRAG_ID1) == typeid(std::vector< vsol_polyline_2d_sptr >));
  curves[0] = input<std::vector< vsol_polyline_2d_sptr > >(FRAG_ID0);
  curves[1] = input<std::vector< vsol_polyline_2d_sptr > >(FRAG_ID1);

  // don't read any frags in the other views
  //  for (unsigned i=0; i < v_->num_confirmation_views(); ++i) {
  //    unsigned offset = 6 + i*sources_per_confirmation_view_;
  //    assert(input_type_id(offset + FRAG_ID) 
  //        == typeid( std::vector< vsol_polyline_2d_sptr > ));

  //    curves[i+2] = input<std::vector< vsol_polyline_2d_sptr > >(offset + FRAG_ID);
  //  }
  // s_.set_curves_and_break_into_episegs(curves, sseq);

  typedef std::vector<std::vector<double> > view_tangents;
  std::vector< view_tangents > tangents(2);

  assert(input_type_id(TGT_ID0) == typeid(view_tangents));
  assert(input_type_id(TGT_ID1) == typeid(view_tangents));
  tangents[0] = input< view_tangents >(TGT_ID0);
  tangents[1] = input< view_tangents >(TGT_ID1);

  std::vector<bbld_subsequence_set> sseq;
  s_.set_curves(curves);
  s_.set_tangents(tangents);
  s_.break_into_episegs_and_replace_curve(&sseq);
}

