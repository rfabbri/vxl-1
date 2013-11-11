// This is brcv/seg/dbdet/pro/dbacm_knee_cartilage_seg_process.h
#ifndef dbacm_knee_cartilage_seg_process_h_
#define dbacm_knee_cartilage_seg_process_h_

//:
// \file
// \brief Process that to do knee segmentation cartilage.
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 12/08/2005
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
//#include <vcl_vector.h>
//#include <vcl_string.h>
//#include <vil/vil_image_view.h>

//: Process to do knee cartilage segmentation
// This process takes in 2 vsol2Ds and 1 image
// vsol2D 1: Closed polygon of inner cartilage inner contour
// vsol2d 2: user-input points marking the outer cartilage (at least 4 pts)
class dbacm_knee_cartilage_seg_process : public bpro1_process 
{

public:

  dbacm_knee_cartilage_seg_process();
  virtual ~dbacm_knee_cartilage_seg_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

};

#endif
