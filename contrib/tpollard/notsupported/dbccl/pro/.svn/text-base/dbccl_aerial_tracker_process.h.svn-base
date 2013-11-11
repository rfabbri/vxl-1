#ifndef dbccl_aerial_tracker_process_h_
#define dbccl_aerial_tracker_process_h_

//:
// \file
// \brief A process for dbccl_aerial_tracker
// \author Thomas Pollard
// \date 8/29/06
//

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include "../dbccl_aerial_tracker.h"


class dbccl_aerial_tracker_process : public bpro1_process {

public:

  dbccl_aerial_tracker_process();
  ~dbccl_aerial_tracker_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();
  
  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

protected:
  
  dbccl_aerial_tracker tracker;

private:

};

#endif // dbccl_aerial_tracker_process_h_
