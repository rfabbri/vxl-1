// This is brcv/seg/dbdet/pro/dbdet_generic_multicue_edge_detector_process.h
#ifndef dbdet_generic_multicue_edge_detector_process_h_
#define dbdet_generic_multicue_edge_detector_process_h_

//:
// \file
// \brief A process to compute a subpixel edges using multidimensional derivatives from color images followed by NMS.
//        This method is described in the DiZenzo '86 and Cumani '89, '93.
// \author Amir Tamrakar
// \date 01/24/07
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <vil/vil_image_view.h>

//: This process allows the user to select a gradient computation method and use
//  it to detect the location and orientation of subpixel edges in colored images.
//  It also allows the user to transform the RGB color image into other color spaces.
class dbdet_generic_multicue_edge_detector_process : public bpro1_process 
{
public:

  dbdet_generic_multicue_edge_detector_process();
  virtual ~dbdet_generic_multicue_edge_detector_process();

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
