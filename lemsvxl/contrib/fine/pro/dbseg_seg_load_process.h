// This is contrib/fine/pro/dbseg_seg_load_process.h
#ifndef dbseg_seg_load_process_h_
#define dbseg_seg_load_process_h_

//:
// \file
// \brief A process for loading a segmentation structure
// \author Eli Fine
// \date 08/14/08
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <vul/vul_timer.h>

#include <structure/dbseg_seg_storage.h>
#include <structure/dbseg_seg_storage_sptr.h>

//using namespace std;
//#include <list>
//#include <set>

#include <structure/dbseg_seg_object.h>

//: Loads an image into a video frame by creating a 
// vidpro1_image_storage class at that frame

class dbseg_seg_load_process : public bpro1_process
{
 public:
  
  dbseg_seg_load_process();
 ~dbseg_seg_load_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();
  
  bool execute();
  bool finish();

  static dbseg_seg_object_base* static_execute(bpro1_filepath seg_path);

 
 protected:
};

#endif //dbseg_seg_load_process_h_


