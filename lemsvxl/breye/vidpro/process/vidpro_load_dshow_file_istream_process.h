// This is brl/vidpro/process/vidpro_load_dshow_file_istream_process.h
#ifndef vidpro_load_dshow_file_istream_process_h_
#define vidpro_load_dshow_file_istream_process_h_

//:
// \file
// \brief A process for loading an image into a frame
// \author Amir Tamrakar
// \date 06/06/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro/bpro_process.h>
#include <brdb/brdb_database_manager.h>
#include <vidpro/vidpro_process_manager.h>

//: Loads an input stream by creating a 
// vidpro_istream_storage  global class

class vidpro_load_dshow_file_istream_process : public bpro_process
{
 public:
  
  vidpro_load_dshow_file_istream_process();
 ~vidpro_load_dshow_file_istream_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();
 
 protected:
};

#endif //vidpro_load_dshow_file_istream_process_h_
