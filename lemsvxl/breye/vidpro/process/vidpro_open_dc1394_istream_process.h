// This is brl/vidpro/process/vidpro_open_dc1394_istream_process.h
#ifndef vidpro_open_dc1394_istream_process_h_
#define vidpro_open_dc1394_istream_process_h_

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
#include <vidpro/storage/vidpro_istream_storage.h>
#include <vidpro/storage/vidpro_istream_storage_sptr.h>

//: Loads an input stream by creating a 
// vidpro_istream_storage  global class

class vidpro_open_dc1394_istream_process : public bpro_process
{
 public:
 
  vidpro_open_dc1394_istream_process();
 ~vidpro_open_dc1394_istream_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();

   //: Use vgui dialogs to open an image list istream
  static vidpro_istream_storage_sptr dc1394_istream();
 
 protected:
 vcl_vector<vcl_string> choices_;
 vcl_vector<int> choice_codes_;

};

#endif //vidpro_open_dc1394_istream_process_h_

