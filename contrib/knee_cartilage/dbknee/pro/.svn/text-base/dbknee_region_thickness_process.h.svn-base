// This is dbknee/pro/dbknee_region_thickness_process.h
#ifndef dbknee_region_thickness_process_h_
#define dbknee_region_thickness_process_h_

//:
// \file
// \brief A process to regional thickness given a thickness map

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
class dbknee_region_thickness_process : public bpro1_process 
{

public:
  //: Constructor
  dbknee_region_thickness_process();
  
  //: Destructor
  virtual ~dbknee_region_thickness_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  vcl_string name() {  return "Region thickness"; }

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  // Support function
  
  //: Take in the data and execute
  static bool region_thickness_vertex_based(const vcl_string& thickness_mesh_file,
    const vcl_string& cs_file,
    double band_width_ratio,
    double start_angle,
    double end_angle,
    double& bot_thickness,
    double& top_thickness);

  //: Take in the data and execute
  // compute thickness as weighted average of closest distance to the faces
  // the weights are the areas of the triangle
  static bool region_thickness_face_based(const vcl_string& thickness_mesh_file,
    const vcl_string& cs_file,
    double band_width_ratio,
    double start_angle,
    double end_angle,
    double& bot_thickness,
    double& top_thickness);


  static bool crop_mesh_using_cyl_cs(const vcl_string& thickness_mesh_file,
    const vcl_string& cs_file,
    double band_width_ratio,
    double start_angle,
    double end_angle,
    const vcl_string& top_mesh_file,
    const vcl_string& bot_mesh_file);


};

#endif
