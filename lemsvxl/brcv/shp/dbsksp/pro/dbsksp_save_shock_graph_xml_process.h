// This is dbsksp/pro/dbsksp_save_shock_graph_xml_process.h
#ifndef dbsksp_save_shock_graph_xml_process_h_
#define dbsksp_save_shock_graph_xml_process_h_

//:
// \file
// \brief A process to save a shock graph to an XML file
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Feb 8, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

//: Process that builds a shock graph from a vsol polyline 
class dbsksp_save_shock_graph_xml_process : public bpro1_process 
{

public:
  //: Constructor
  dbsksp_save_shock_graph_xml_process();
  
  //: Destructor
  virtual ~dbsksp_save_shock_graph_xml_process();

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

protected:
  std::vector<std::string > shock_graph_types_;
};

#endif
