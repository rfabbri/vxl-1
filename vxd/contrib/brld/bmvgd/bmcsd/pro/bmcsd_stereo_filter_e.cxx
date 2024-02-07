#include "bmcsd_stereo_filter_e.h"
#include <bmcsd/bmcsd_view_set.h>
#include <bbld/bbld_subsequence.h>

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
