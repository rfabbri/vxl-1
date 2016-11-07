// This is dbdet/pro/dbdet_new_seg3d_info_xml_process.h
#ifndef dbdet_new_seg3d_info_xml_process_h_
#define dbdet_new_seg3d_info_xml_process_h_

// ----------------------------------------------------------------------------
//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date June 2, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>

//: 
class dbdet_new_seg3d_info_xml_process : public bpro1_process
{  
public: 
  //: Constructor
  dbdet_new_seg3d_info_xml_process();

  //: Destructor
 ~dbdet_new_seg3d_info_xml_process();

  //: Clone the process
  virtual bpro1_process* clone() const;
  
  //: The name of the video process
  virtual vcl_string name();

  //: Returns the number of frames needed for input
  // (prior to and including the current frame)
  virtual int input_frames();
  //: Returns the number of frames used for output
  // (prior to and including the current frame)
  virtual int output_frames();

  //: Returns a vector of strings describing the required storage class input types
  virtual vcl_vector< vcl_string > get_input_type();
  //: Returns a vector of strings describing the storage class output types produced
  virtual vcl_vector< vcl_string > get_output_type();

  //: Execute the process on using the storage classes already set
  virtual bool execute();
  //: Perform any clean up or final computation after computing on all frames
  virtual bool finish();

};

#endif //dbdet_new_seg3d_info_xml_process_h_
