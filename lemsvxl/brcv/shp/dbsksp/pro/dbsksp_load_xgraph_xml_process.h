// This is dbsksp/pro/dbsksp_load_xgraph_xml_process.h
#ifndef dbsksp_load_xgraph_xml_process_h_
#define dbsksp_load_xgraph_xml_process_h_

//:
// \file
// \brief A process to load an xgraph XML file
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Feb 20, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

//: Process that builds a shock graph from a vsol polyline 
class dbsksp_load_xgraph_xml_process : public bpro1_process 
{

public:
  //: Constructor
  dbsksp_load_xgraph_xml_process();
  
  //: Destructor
  virtual ~dbsksp_load_xgraph_xml_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();
};

#endif
