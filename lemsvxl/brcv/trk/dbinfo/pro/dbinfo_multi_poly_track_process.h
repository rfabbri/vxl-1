// This is brl/blem/brcv/trk/dbinfo/dbinfo_multi_poly_track_process.h
#ifndef dbinfo_multi_poly_track_process_h_
#define dbinfo_multi_poly_track_process_h_

//:
// \file
// \brief A bpro1 process that tracks uand initializes polygons from different frames.
// \author Vishal Jain
// \date Aug 14th, 2006
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <dbinfo/dbinfo_multi_tracker.h>


class dbinfo_multi_poly_track_process : public bpro1_process
{
public:

  dbinfo_multi_poly_track_process();
  dbinfo_multi_poly_track_process(const dbinfo_multi_poly_track_process& other);
  ~dbinfo_multi_poly_track_process();

  //: Clone the process
  virtual bpro1_process* clone() const; 
  
  //: The name of the video process
  virtual vcl_string name() {return "dbinfo_multi_poly_track_process";}

  int input_frames();
  int output_frames();
  
  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();
  
  void set_track_storage_name(vcl_string const& name){name_ = name;}
private:
  //: set the parameters from menu assigned values
  void set_tracker_parameters();
  //First frame processed 
  vcl_string name_;
  bool first_frame_;
  dbinfo_multi_tracker tracker_;
  bpro1_filepath track_storage_filepath_;
};

#endif // dbinfo_multi_poly_track_process_h_
