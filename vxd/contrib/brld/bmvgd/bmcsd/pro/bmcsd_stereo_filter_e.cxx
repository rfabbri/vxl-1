#include "bmcsd_stereo_filter_e.h"
#include <bmcsd/bmcsd_view_set.h>
#include <bbld/bbld_subsequence.h>


void bmcsd_stereo_filter_e::
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

void bmcsd_stereo_filter_e::
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

bprod_signal bmcsd_stereo_aggregator::
run(unsigned long timestamp,
    bprod_debug_observer* const debug)
{
  // notify the debugger if available
  if (debug) debug->notify_enter(this, timestamp);

  if(!this->enabled()){
    // notify the debugger if available
    if (debug) debug->notify_exit(this, timestamp);
    return BPROD_VALID;
  }
  
  update_mutex_.lock();

  if(timestamp > this->timestamp_){
    this->timestamp_ = timestamp;
    this->last_signal_ = this->request_inputs_serial(timestamp,debug);
    if(this->last_signal_ == BPROD_VALID){
      if (debug){
        debug->notify_pre_exec(this);
        this->last_signal_ = this->execute();
        debug->notify_post_exec(this);
      }
      else
        this->last_signal_ = this->execute();
    }
    this->clear();
  }
  
  update_mutex_.unlock();

  // notify the debugger if available
  if (debug) debug->notify_exit(this, timestamp);

  return this->last_signal_;
}
