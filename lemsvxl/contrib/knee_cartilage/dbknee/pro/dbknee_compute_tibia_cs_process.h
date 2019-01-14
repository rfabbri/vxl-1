// This is dbknee/pro/dbknee_compute_tibia_cs_process.h
#ifndef dbknee_compute_tibia_cs_process_h_
#define dbknee_compute_tibia_cs_process_h_

//:
// \file
// \brief A process to compute tibial coordinate system
// 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date July 7, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string> 
#include <dbknee/dbknee_tibia_cs.h>

//: Process that builds a shock graph from a vsol polyline 
class dbknee_compute_tibia_cs_process : public bpro1_process 
{

public:
  //: Constructor
  dbknee_compute_tibia_cs_process();
  
  //: Destructor
  virtual ~dbknee_compute_tibia_cs_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  std::string name() {  return "Compute tibia CS"; }

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  // Support function
  
  //: Take in the data (mesh file, param file) and execute
  bool compute_tibia_cs(const std::string& data_file,
    const std::string& cs_file);

  bool region_thickness_face_based(const std::string& thickness_mesh_file, 
    dbknee_tibia_cs coord);

};

#endif
