// This is brl/vidpro/process/vidpro_open_live_istream_process.h
#ifndef vidpro_open_live_istream_process_h_
#define vidpro_open_live_istream_process_h_

//:
// \file
// \brief A process for loading an image into a frame
// \author Amir Tamrakar
// \date 06/06/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <vector>
#include <string>
#include <bpro/bpro_process.h>
#include <brdb/brdb_database_manager.h>
#include <vidpro/vidpro_process_manager.h>
#include <vidpro/storage/vidpro_istream_storage.h>
#include <vidpro/storage/vidpro_istream_storage_sptr.h>

//: Loads an input stream by creating a 
// vidpro_istream_storage  global class

class vidpro_open_live_istream_process : public bpro_process
{
 public:
  
  enum stream_type {DSHOW_LIVE};
  vidpro_open_live_istream_process();
 ~vidpro_open_live_istream_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();


  //: Use vgui dialogs to open an image list istream
  static vidpro_istream_storage_sptr dshow_live_istream(std::string const& ,std::string const&);

 protected:
 std::vector<std::string> choices_;
 std::vector<int> choice_codes_;

 void add_parameters();
 vidpro_istream_storage_sptr open_live_istream(int, std::string const&, std::string const&);

};

#endif //vidpro_open_live_istream_process_h_

