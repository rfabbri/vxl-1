#include "bmcsd_stereo_driver.h"
#include <bmcsd/pro/bmcsd_fragment_tangents_filter.h>
#include <bmcsd/pro/bmcsd_load_curvelet_source.h>
#include <bmcsd/pro/bmcsd_stereo_filter.h>
#include <algorithm>

bool bmcsd_concurrent_stereo_driver::
init()
{
  bmcsd_concurrent_stereo_driver_base::init();

  curve_stereo_.reserve(f_.num_instances());
  //: Setup nodes that act on multiple frames
  for (unsigned i=0; i < f_.num_instances(); ++i) {
    bmcsd_stereo_filter *p = new bmcsd_stereo_filter();
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

bool bmcsd_concurrent_stereo_driver::
run(unsigned long timestamp)
{
  std::cout << "Running stereo driver.\n";
  assert(initialized_);
  bprod_signal retval = output_job_->run(timestamp);

  if (retval == BPROD_INVALID)
    return false;
  bmcsd_stereo_aggregator *o = dynamic_cast<bmcsd_stereo_aggregator *> (output_job_.ptr());
  crv3d_ = &o->crv3d_;
  attr_  = &o->attr_;
  corresp_  = &o->corresp_;
  return true;
}
