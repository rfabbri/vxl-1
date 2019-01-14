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
#include <vector>
#include <string> 

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
  std::string name() {  return "Export thickness map"; }

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  // Support function
  
  //: Take in the data and execute
  static bool export_thickness_map_to_file(
    const std::string& thickness_mesh_file,
    const std::string& cs_file,
    double band_width_ratio,
    double start_angle,
    double end_angle,
    const std::string& thickness_map_file,
    bool show_gui);


  //: Take in the data and execute
  static bool export_full_thickness_map_to_file(
    const std::string& thickness_mesh_file,
    const std::string& cs_file,
    const std::string& thickness_map_file);


};

#endif
