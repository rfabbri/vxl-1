// This is brcv/trk/dbinfo/pro/dbinfo_save_tracks_process.h

#ifndef dbinfo_save_tracks_process_h_
#define dbinfo_save_tracks_process_h_

//:
// \file
// \brief A process for saving track storage
// \author J.L. Mundy
// \date 10/24/2005
//
// \verbatim
//  Modifications
// \endverbatim


#include <vector>
#include <string>
#include <bpro1/bpro1_process.h>

class dbinfo_save_tracks_process : public bpro1_process
{
 public:

  dbinfo_save_tracks_process();
 ~dbinfo_save_tracks_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  void clear_output();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
};

#endif // dbinfo_save_tracks_process
