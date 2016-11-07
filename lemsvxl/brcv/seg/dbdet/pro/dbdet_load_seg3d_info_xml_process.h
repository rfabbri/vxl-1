// This is dbdet/pro/dbdet_load_seg3d_info_xml_process.h
#ifndef dbdet_load_seg3d_info_xml_process_h_
#define dbdet_load_seg3d_info_xml_process_h_

// ----------------------------------------------------------------------------
//:
// \file
// \brief A process to load an 3d image (representated as 2d slices) for 
// segmentation whose info is saved in an .xml file
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date May 30, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bpro1/bpro1_process.h>

//: Loads an image into a video frame by creating a 
// vidpro1_image_storage class at that frame

class dbdet_load_seg3d_info_xml_process : public bpro1_process
{  
public: 
  //: Constructor
  dbdet_load_seg3d_info_xml_process();

  //: Destructor
 ~dbdet_load_seg3d_info_xml_process();

  //: Clone the process
  virtual bpro1_process* clone() const;
  
  //: Clear the output vector and then resize the empty vector
  virtual void clear_output(int resize = -1);
  
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

 
  // =========================================================
  int num_frames() const {return this->num_frames_; }
  void set_num_frames(int num_frames ){this->num_frames_ = num_frames; }


 protected:
   int num_frames_;
};

#endif //dbdet_load_seg3d_info_xml_process_h_
