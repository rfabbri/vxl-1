// This is brl/brcv/trk/dbinfo/pro/dbinfo_load_tracks_process.h

#ifndef dbinfo_load_tracks_process_h_
#define dbinfo_load_tracks_process_h_

//:
// \file
// \brief A process for loading a track storage file
// \author J. L. Mundy
// \date 9/25/05
//
// \verbatim
//  Modifications <none>
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <dbinfo/pro/dbinfo_track_storage_sptr.h>

class dbinfo_load_tracks_process : public bpro1_process
{
 public:
  
  dbinfo_load_tracks_process();
 ~dbinfo_load_tracks_process();

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
 
  int num_frames() {return num_frames_;}
 protected:

 int num_frames_;
 dbinfo_track_storage_sptr track_storage_;
};

#endif // dbinfo_load_tracks_process
