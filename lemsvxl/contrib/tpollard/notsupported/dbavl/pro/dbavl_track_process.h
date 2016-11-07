#ifndef dbavl_track_process_h_
#define dbavl_track_process_h_

//:
// \file
// \brief A process for dbavl_tracker
// \author Thomas Pollard
// \date 8/29/06
//

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include "../dbavl_tracker.h"


class dbavl_track_process : public bpro1_process {

public:

  dbavl_track_process();
  ~dbavl_track_process();

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
  
  dbavl_tracker tracker;

private:

};

#endif // dbavl_track_process_h_
