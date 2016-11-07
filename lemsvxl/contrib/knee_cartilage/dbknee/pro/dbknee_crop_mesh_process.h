// This is dbknee/pro/dbknee_crop_mesh_process.h
#ifndef dbknee_crop_mesh_process_h_
#define dbknee_crop_mesh_process_h_

//:
// \file
// \brief A process to crop a mesh
// 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date October 10, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h> 

//: Process that builds a shock graph from a vsol polyline 
class dbknee_crop_mesh_process : public bpro1_process 
{

public:
  //: Constructor
  dbknee_crop_mesh_process();
  
  //: Destructor
  virtual ~dbknee_crop_mesh_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  vcl_string name() {  return "Crop mesh"; }

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  // Support function
  bool crop_mesh(const vcl_string& data_file,
                  const vcl_string& crop_direction,
                  float crop_ratio,
                  const vcl_string& cropped_mesh_1,
                  const vcl_string& cropped_mesh_2);
};

#endif
