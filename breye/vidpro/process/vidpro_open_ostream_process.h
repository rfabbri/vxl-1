// This is brl/vidpro/process/vidpro_open_ostream_process.h
#ifndef vidpro_open_ostream_process_h_
#define vidpro_open_ostream_process_h_

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
#include <vidpro/storage/vidpro_ostream_storage.h>
#include <vidpro/storage/vidpro_ostream_storage_sptr.h>

//: Loads an input stream by creating a 
// vidpro_istream_storage  global class

class vidpro_open_ostream_process : public bpro_process
{
 public:
  
  enum stream_type {IMAGE_LIST};
  vidpro_open_ostream_process();
 ~vidpro_open_ostream_process();

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
 vcl_vector<vcl_string> choices_;
 vcl_vector<vcl_string> fmt_choices_;
 vcl_vector<int> choice_codes_;

 void add_parameters();
 vidpro_ostream_storage_sptr open_ostream(int type, 
                                           vcl_string const& directory,
                                           vcl_string const &name_format,
                                           vcl_string const &imagefile_fmt,
                                           unsigned int start_index);
 //: Use vgui dialogs to open an image list istream
  static vidpro_ostream_storage_sptr image_list_ostream(vcl_string const &directory,
                                                 vcl_string const &name_format,
                                                 vcl_string const &imagefile_fmt,
                                                 unsigned int start_index);
  
  
};

#endif //vidpro_open_ostream_process_h_
