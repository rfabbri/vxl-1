// This is dbknee/pro/dbknee_compute_cylinder_cs_process.h
#ifndef dbknee_compute_cylinder_cs_process_h_
#define dbknee_compute_cylinder_cs_process_h_

//:
// \file
// \brief A process to compute cylinder-based coordinate system for knee
// cartilage from a bone/cartilage surface mesh
// 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 9, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h> 

//: Process that builds a shock graph from a vsol polyline 
class dbknee_compute_cylinder_cs_process : public bpro1_process 
{

public:
  //: Constructor
  dbknee_compute_cylinder_cs_process();
  
  //: Destructor
  virtual ~dbknee_compute_cylinder_cs_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  vcl_string name() {  return "Compute cylinder CS"; }

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  // Support function
  
  //: Take in the data (mesh file, param file) and execute
  static bool compute_cylinder_cs(const vcl_string& data_file,
    const vcl_string& param_file, 
    const vcl_string& cs_file,
    const vcl_string& cropped_mesh_file);

};

#endif
