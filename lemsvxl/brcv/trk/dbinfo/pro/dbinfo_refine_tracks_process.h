// This is brl/brcv/trk/dbinfo/pro/dbinfo_refine_tracks_process.h

#ifndef dbinfo_refine_tracks_process_h_
#define dbinfo_refine_tracks_process_h_

//:
// \file
// \brief A process for refining the position of the track
// \author J. L. Mundy
// \date 10/08/05
//
// \verbatim
//  Modifications <none>
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <dbinfo/dbinfo_track_sptr.h>
#include <dbinfo/pro/dbinfo_track_storage_sptr.h>
#include <dbinfo/dbinfo_match_optimizer_sptr.h>
class dbinfo_refine_tracks_process : public bpro1_process
{
 public:
  
  dbinfo_refine_tracks_process();
 ~dbinfo_refine_tracks_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  virtual void clear_output(int resize = -1);

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();
 
 protected:
  bool first_frame_;
  unsigned start_frame_;
  vcl_string name_;
  vcl_vector<dbinfo_track_sptr> optimized_tracks_;
  dbinfo_match_optimizer_sptr opt_;
};

#endif // dbinfo_refine_tracks_process
