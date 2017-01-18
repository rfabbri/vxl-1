// This is basic/dbvidl2/pro/dbvidl2_multi_source.cxx

//:
// \file

#include "dbvidl2_multi_source.h"
#include <vidl/vidl_istream.h>


//: Run the process on the current frame
bprod_signal 
dbvidl2_multi_source::execute()
{
  bool all_passed = true;
  for(unsigned int i=0; i<istreams_.size(); ++i){
    if(istreams_[i])
      all_passed = istreams_[i]->advance() && all_passed;
  }
  if(!all_passed)
    return BPROD_EOS;

  vidl_frame_sptr frame = NULL;
  for(unsigned int i=0; i<istreams_.size(); ++i){
    if(istreams_[i]){
      frame = istreams_[i]->current_frame();
      all_passed = bool(frame) && all_passed;
      output(i, frame);
    }
  }
  if(!all_passed)
    return BPROD_INVALID;

  return BPROD_VALID;
}




