// This is contrib/fine/pro/dbseg_seg_process.h
#ifndef dbseg_seg_process_h_
#define dbseg_seg_process_h_

//:
// \file
// \brief A process that takes a segmented image and creates a segmentation structure
//        Inputs are 2 images
//            First is the segmented image
//            Second is the original image
// \author Eli Fine
// \date 08/4/08
//
// \verbatim
//  Modifications
//   
// \endverbatim


#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>
#include <structure/dbseg_seg_object.h>
#include <structure/dbseg_seg_storage.h>
#include <structure/dbseg_seg_storage_sptr.h>
#include <structure/dbseg_seg_object_base.h>
#include <vil/vil_rgb.h>
#include <set>
#include <map>
#include <list>


class dbseg_seg_process : public bpro1_process {

public:

  dbseg_seg_process();
  virtual ~dbseg_seg_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
  
    static dbseg_seg_object_base* static_execute(vil_image_view_base_sptr labeled_image, vil_image_view_base_sptr original, bool altFill);

protected:
    static void fillRegion(int x, int y, int r, int g, int b, int count, bool** done, bool** curr, vil_image_view<int>* labeled, vil_image_view<vxl_byte>& label);
    //vil_image_view<int>* labeled;
    //vil_image_view<vxl_byte> label;

    bool map_compare(vil_rgb<vxl_byte> p1, vil_rgb<vxl_byte> p2);

    //bool** done;
    //bool** curr;
    //vil_image_view<vxl_byte> orig;
private:

};

#endif // dbseg_seg_process_h_


