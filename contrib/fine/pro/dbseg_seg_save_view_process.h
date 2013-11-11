// This is contrib/fine/pro/dbseg_seg_save_view_process.h

#ifndef dbseg_seg_save_view_process_h_
#define dbseg_seg_save_view_process_h_

//:
// \file
// \brief A process for saving a view of a segmentation structure (i.e. the picture)
// \author Eli Fine
// \date 8/14/08
//
// \verbatim
//  Modifications
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <vcl_list.h>
#include <vil/vil_image_resource_sptr.h>

class dbseg_seg_save_view_process : public bpro1_process
{
 public:

  dbseg_seg_save_view_process();
 ~dbseg_seg_save_view_process();

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

#endif // dbseg_seg_save_view_process


