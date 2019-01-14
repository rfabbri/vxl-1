// This is dbsksp/pro/dbsksp_build_from_xshock_graph_process.h
#ifndef dbsksp_build_from_xshock_graph_process_h_
#define dbsksp_build_from_xshock_graph_process_h_

//:
// \file
// \brief A process to build a generative shock graph from a static 
// extrinsic shock graph 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date April 7, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

//: Process that builds a shock graph from a vsol polyline 
class dbsksp_build_from_xshock_graph_process : public bpro1_process 
{

public:
  //: Constructor
  dbsksp_build_from_xshock_graph_process();
  
  //: Destructor
  virtual ~dbsksp_build_from_xshock_graph_process();

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
  std::vector<std::string > build_type_descriptions_;

};

#endif
