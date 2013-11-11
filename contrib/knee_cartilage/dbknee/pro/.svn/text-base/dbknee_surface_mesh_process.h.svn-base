// This is dbknee/pro/dbknee_surface_mesh_process.h
#ifndef dbknee_surface_mesh_process_h_
#define dbknee_surface_mesh_process_h_

//:
// \file
// \brief A process to mesh a point cloud

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
class dbknee_surface_mesh_process : public bpro1_process 
{

public:
  //: Constructor
  dbknee_surface_mesh_process();
  
  //: Destructor
  virtual ~dbknee_surface_mesh_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  vcl_string name() {  return "Surface mesh from a point cloud"; }

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  // Support function
  void surface_mesh(const vcl_string& input_p3d_file,
    const vcl_string& output_ply_file);

};

#endif
