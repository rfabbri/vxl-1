// This is dbsksp/pro/dbsksp_convert_shock_graph_to_xshock_graph_process.h
#ifndef dbsksp_convert_shock_graph_to_xshock_graph_process_h_
#define dbsksp_convert_shock_graph_to_xshock_graph_process_h_

//:
// \file
// \brief Convert from intrinsic generative shock graph to extrinsic generative shock graph
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Oct 20, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

//: Process that builds a shock graph from a vsol polyline 
class dbsksp_convert_shock_graph_to_xshock_graph_process : public bpro1_process 
{

public:
  //: Constructor
  dbsksp_convert_shock_graph_to_xshock_graph_process();
  
  //: Destructor
  virtual ~dbsksp_convert_shock_graph_to_xshock_graph_process();

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
};

#endif
