// This is dbknee/pro/dbknee_compute_thickness_process.h
#ifndef dbknee_compute_thickness_process_h_
#define dbknee_compute_thickness_process_h_

//:
// \file
// \brief A process to compute distance between meshes at every vertex

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
class dbknee_compute_thickness_process : public bpro1_process 
{

public:
  //: Constructor
  dbknee_compute_thickness_process();
  
  //: Destructor
  virtual ~dbknee_compute_thickness_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  vcl_string name() {  return "Compute thickness"; }

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  // Support function
  
  //: Take in the data and execute
  static bool compute_thickness(const vcl_string& inner_mesh_file,
    const vcl_string& outer_mesh_file, 
    const vcl_string& thickness_mesh_file,
    bool show_gui);

};

#endif
