// This is dbknee/pro/dbknee_export_thickness_map_process.h
#ifndef dbknee_export_thickness_map_process_h_
#define dbknee_export_thickness_map_process_h_

//:
// \file
// \brief A process to save the thickness map (thickness + cylindrical 
// coordinates of a knee cartilage to a text file

// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Mar 15, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h> 

//: Process that builds a shock graph from a vsol polyline 
class dbknee_export_thickness_map_process : public bpro1_process 
{

public:
  //: Constructor
  dbknee_export_thickness_map_process();
  
  //: Destructor
  virtual ~dbknee_export_thickness_map_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  vcl_string name() {  return "Export thickness map"; }

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  // Support function
  
  //: Take in the data and execute
  static bool export_thickness_map_to_file(
    const vcl_string& thickness_mesh_file,
    const vcl_string& cs_file,
    double band_width_ratio,
    double start_angle,
    double end_angle,
    const vcl_string& thickness_map_file,
    bool show_gui);


  //: Take in the data and execute
  static bool export_full_thickness_map_to_file(
    const vcl_string& thickness_mesh_file,
    const vcl_string& cs_file,
    const vcl_string& thickness_map_file);


};

#endif
