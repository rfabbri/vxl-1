#ifndef show_contours_process_h_
#define show_contours_process_h_

//:
// \file
// \brief A process for loading an image into a frame along with associated .CON files
// \author Ricardo Fabbri
// \date 04/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <vector>
#include <string>
#include <bpro1/bpro1_process.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>


//: Loads an image into a video frame by creating a 
// vidpro1_image_storage class at that frame

class show_contours_process : public bpro1_process
{
 public:
  
  show_contours_process();
 ~show_contours_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();


  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();

  bool loadCON (std::string filename, float scale);
 
 protected:
};

#endif //show_contours_process_h_
