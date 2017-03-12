// This is dbdet_bvis1_util.h
#ifndef dbdet_bvis1_util_h
#define dbdet_bvis1_util_h
//:
//\file
//\brief Various Utilities
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri.github.io)
//\date 05/14/2008 03:19:50 AM EDT
//
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vector>

struct dbdet_bvis1_util {

  //: Loads list of edge maps into all frames. Doesn't add frames.
  // The idea is that the frames are added first, then many functions of this
  // kind can be used to populate the frames over and over
  // TODO: std::vector<std::string> parameter for names
  static void
  load_edgemaps_into_frames(const vcl_vector<vcl_string> &edgemaps_fnames, bool use_filename);
};


#endif // dbdet_bvis1_util_h
