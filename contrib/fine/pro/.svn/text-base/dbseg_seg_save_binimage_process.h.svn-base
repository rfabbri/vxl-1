// This is contrib/fine/pro/dbseg_seg_save_binimage_process.h

#ifndef dbseg_seg_save_binimage_process_h_
#define dbseg_seg_save_binimage_process_h_

//:
// \file
// \brief A process for saving a binary image of a region from a segmentation structure 
// \author Eli Fine
// \date 11/21/08
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <vcl_list.h>
#include <vil/vil_image_resource_sptr.h>

#include <vgl/vgl_polygon.h>
#include <fstream>

class dbseg_seg_save_binimage_process : public bpro1_process
{
 public:

  dbseg_seg_save_binimage_process();
 ~dbseg_seg_save_binimage_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  void clear_output();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();



};

#endif // dbseg_seg_save_binimage_process


