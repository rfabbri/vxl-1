#include "bmcsd_stereo_driver_e.h"
#include <bmcsd/pro/bmcsd_stereo_filter_e.h>

bool bmcsd_concurrent_stereo_driver_e::
init()
{
  bmcsd_concurrent_stereo_driver_base::init();

  curve_stereo_.reserve(f_.num_instances());
  //: Setup nodes that act on multiple frames
  for (unsigned i=0; i < f_.num_instances(); ++i) {
    bmcsd_stereo_filter_e *p = new bmcsd_stereo_filter_e();
    curve_stereo_.push_back(p);

    p->setup_inputs(
        f_.instance(i),
        cam_src_,
        edg_src_,
        edg_dt_,
        frag_src_,
        cvlet_src_,
        frag_tangents_);
    
  }
  update_stereo_params();
  init_end();
  return true;
}


// In anil's original, but not used
bool bmcsd_concurrent_stereo_driver_e::
init(std::vector<bdifd_camera> &cams,
     std::vector<sdet_edgemap_sptr> &em,
     std::vector<std::vector< vsol_polyline_2d_sptr > > &curves,
     std::vector<std::vector<std::vector<double> > > &tangents,
     std::vector<vil_image_view<vxl_uint_32> > &dts,
     std::vector<vil_image_view<unsigned> > &labels)
{ 
  std::cout << "Initializing stereo driver w nviews = " << nviews() << std::endl;

  //: Setup nodes that act on multiple frames
  for (unsigned i=0; i < f_.num_instances(); ++i) {
    bmcsd_stereo_filter_e *p = new bmcsd_stereo_filter_e();
    curve_stereo_.push_back(p);

    p->load_inputs(f_.instance(i),
		   cams,
		   em,
		   curves,
		   tangents,
		   dts,
		   labels
		   );

    p->isFirstRun_ = this->isFirstRun_;
    p->mate_curves_v1_ = this->mate_curves_v1_;
    p->usedCurves_ = this->usedCurves_;
  }

  update_stereo_params();

  curve_stereo_jobs_.reserve(static_cast<unsigned>(std::ceil(curve_stereo_.size()/max_concurrent_matchers_)));
  //: Setup the nodes to do simultaneous processing
  for (unsigned i=0; i < curve_stereo_.size(); ++i) {
    if (i % max_concurrent_matchers_ == 0) {
      unsigned num_matchers
       = std::min(static_cast<unsigned long>(max_concurrent_matchers_), 
           static_cast<unsigned long>(curve_stereo_.size()-i));

      curve_stereo_jobs_.push_back(new bmcsd_stereo_jobs_e(num_matchers));
    }

    bprod_process_sptr p = curve_stereo_jobs_.back();
    unsigned idx = 3*(i % max_concurrent_matchers_);
    p->connect_input(idx,   curve_stereo_[i], 0);
    p->connect_input(idx+1, curve_stereo_[i], 1);
    p->connect_input(idx+2, curve_stereo_[i], 2);
  }

  //: Now connect all curve_stereo_jobs_ into a single output job.
  output_job_ = new bmcsd_stereo_aggregator_e(curve_stereo_jobs_.size());
  for (unsigned i=0; i < curve_stereo_jobs_.size(); ++i) {
    output_job_->connect_input(3*i,   curve_stereo_jobs_[i], 0 /* crv_3d */);
    output_job_->connect_input(3*i+1, curve_stereo_jobs_[i], 1 /* attr */);
    output_job_->connect_input(3*i+2, curve_stereo_jobs_[i], 2 /* corresp */);
  }

  initialized_ = true;
  return true;
}

bool bmcsd_concurrent_stereo_driver_e::
run(unsigned long timestamp)
{
  std::cout << "Running stereo driver_e.\n";
  assert(initialized_);
  bprod_signal retval = output_job_->run(timestamp);

  if (retval == BPROD_INVALID)
    return false;
  bmcsd_stereo_aggregator_e *o = dynamic_cast<bmcsd_stereo_aggregator_e *> (output_job_.ptr());
  crv3d_ = &o->crv3d_;
  attr_  = &o->attr_;
  corresp_  = &o->corresp_;
  return true;
}
