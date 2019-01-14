// This is brl/vidpro/process/vidpro_load_dc1394_istream_process.h
#ifndef vidpro_load_dc1394_istream_process_h_
#define vidpro_load_dc1394_istream_process_h_

//:
// \file
// \brief A process for loading video input stream using libdc1394
// \author Isabel Restrepo
// \date 03/28/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <vector>
#include <string>
#include <bpro/bpro_process.h>

//: Loads an input stream by creating a 
// vidpro_istream_storage  global class

class vidpro_load_dc1394_istream_process : public bpro_process
{
 public:
  
  vidpro_load_dc1394_istream_process();
 ~vidpro_load_dc1394_istream_process();

  //: Clone the process
  virtual bpro_process* clone() const;

  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();
 
 protected:
};

#endif //vidpro_load_dc1394_istream_process_h_
