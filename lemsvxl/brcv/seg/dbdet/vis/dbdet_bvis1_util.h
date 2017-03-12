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

  //: Loads list of curve fragments into all frames. Doesn't add frames.
  // The idea is that the frames are added first, then many functions of this
  // kind can be used to populate the frames over and over
  // TODO: std::vector<std::string> parameter for names
  static void
  load_curve_frags_into_frames(const vcl_vector<vcl_string> &cfrags_fnames, bool use_filenames=false);

  //: Loads lists of images, edges and fragments into all frames.
  // - it will load all images, one per frame
  // - it then gets back to the first frame
  // - if there are any edges, it will load all of them, one per frame
  // - if there are any curve fragments, it will load all of them, one per frame
  // - if repeat_img is true, it will use the same images on all frames
  // - the number of images or edges or fragments doesn't have to be the same
  static void
  load_img_edg(
      const std::vector<std::string> &imgs, 
      const std::vector<std::string> &edges, 
      const std::vector<std::string> &frags,
      bool repeat_img);
};


#endif // dbdet_bvis1_util_h
