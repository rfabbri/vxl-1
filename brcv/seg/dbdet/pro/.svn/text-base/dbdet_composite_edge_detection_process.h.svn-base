// This is brcv/seg/dbdet/pro/dbdet_composite_edge_detection_process.h
#ifndef dbdet_composite_edge_detection_process_h_
#define dbdet_composite_edge_detection_process_h_

//:
// \file
// \brief A process to compute a subpixel edges using the third order edge detector and add them onto the same frame
// \author Amir Tamrakar
// \date 11/10/06
//
// \verbatim
//  Modifications
//    Amir Tamrakar   moved this out of the thirdorder edge detection process
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//: Third order edge detector
class dbdet_composite_edge_detection_process : public bpro1_process 
{
public:

  dbdet_composite_edge_detection_process();
  virtual ~dbdet_composite_edge_detection_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();

  virtual void clear_output(int resize=-1);

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

protected:
  int num_frames_;

};

#endif
