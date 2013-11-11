// This is brcv/seg/dbdet/pro/dbacm_geodesic_active_contour_process.h
#ifndef dbacm_geodesic_active_contour_process_h_
#define dbacm_geodesic_active_contour_process_h_

//:
// \file
// \brief Process that uses geodesic active contour to segmentation
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date 12/01/2005
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>


//: Process to use Geodesic Active Contour
// The process takes in 3 images:
// Image 1: image to segment
// Image 2: binary image, initialization for segmentation.
// Image 3: binary feature image, e.g. Canny edge map.
// At the end of the process, Image 2 is updated with segmentation result.
// Note: direction = 1: contour moves OUTWARD, -1: INWARD. 
// This process does not return any contour. If object contours are interested, 
// use contour_tracer on segmentation result.
class dbacm_geodesic_active_contour_process : public bpro1_process 
{

public:

  dbacm_geodesic_active_contour_process();
  virtual ~dbacm_geodesic_active_contour_process();

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
