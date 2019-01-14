// This is brl/blem/brcv/trk/dbinfo/dbinfo_multi_track_process.h
#ifndef dbinfo_multi_track_process_h_
#define dbinfo_multi_track_process_h_

//:
// \file
// \brief A bpro1 process that tracks using mutual information
// \author J. L. Mundy
// \date May 21, 2005
//
// \verbatim
//  Modifications
// \endverbatim


#include <vector>
#include <string>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <dbinfo/dbinfo_multi_tracker.h>


class dbinfo_multi_track_process : public bpro1_process
{
public:

  dbinfo_multi_track_process();
  dbinfo_multi_track_process(const dbinfo_multi_track_process& other);
  ~dbinfo_multi_track_process();

  //: Clone the process
  virtual bpro1_process* clone() const; 
  
  //: The name of the video process
  virtual std::string name() {return "dbinfo_multi_track_process";}

  int input_frames();
  int output_frames();
  
  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();
  
  void set_track_storage_name(std::string const& name){name_ = name;}
private:
  //: set the parameters from menu assigned values
  void set_tracker_parameters();
  //First frame processed 
  std::string name_;
  bool first_frame_;
  dbinfo_multi_tracker tracker_;
  bpro1_filepath track_storage_filepath_;
};

#endif // dbinfo_multi_track_process_h_
