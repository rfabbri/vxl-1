// This is contrib/fine/pro/dbseg_seg_save_contour_process.h

#ifndef dbseg_seg_save_contour_process_h_
#define dbseg_seg_save_contour_process_h_

//:
// \file
// \brief A process for saving a contour from a segmentation structure 
// \author Eli Fine
// \date 8/29/08
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <vcl_list.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_view.h>

#include <vgl/vgl_polygon.h>
#include <fstream>

class dbseg_seg_save_contour_process : public bpro1_process
{
 public:

  dbseg_seg_save_contour_process();
 ~dbseg_seg_save_contour_process();

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

  static bool static_execute(dbseg_seg_object_base* obj, bpro1_filepath file, int pixel_format = 1);
};

#endif // dbseg_seg_save_contour_process


