// This is bvis1_util.h
#ifndef bvis1_util_h
#define bvis1_util_h
//:
//\file
//\brief Various Utilities
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri.github.io)
//\date 05/14/2008 03:19:50 AM EDT
//
#include <string>
#include <iostream>
#include <vector>

struct bvis1_util {

  //: Removes prefixes such as file:// before the filename.
  static void handle_file_name_prefix(std::string &file_name)
  {
    if (file_name.size() >= 7) {
      std::string p(file_name);
      std::cout << "Original file name: " << p << std::endl;
      std::string file_prefix("file://");
    
      file_name.clear();
      bool has_prefix = true;
      unsigned i;
      for (i=0; i < 7 && i < p.size(); ++i) {
        if (p[i] != file_prefix[i]) {
          has_prefix=false;
          break;
        }
      }
    
      if (has_prefix && (i < p.size() || i == 7)) {
        // has 'file://'   now copy:
        for (unsigned k=7; k < p.size(); ++k) {
          file_name.push_back(p[k]);
        }
      } else 
        file_name = p;
    
      std::cout << "Final file name: " << file_name << std::endl;
    }
  }

  //: Loads list of images into all frames. Doesn't add frames.
  // The idea is that the frames are added first, then many functions of this
  // kind can be used to populate the frames over and over
  // TODO: std::vector<std::string> parameter for names
  static void
  load_imgs_into_frames(const std::vector<std::string> &imgs_fnames, bool use_filenames=false);
};


#endif // bvis1_util_h
