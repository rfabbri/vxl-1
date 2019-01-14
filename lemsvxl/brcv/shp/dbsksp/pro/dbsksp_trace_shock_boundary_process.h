// This is dbsksp/pro/dbsksp_trace_shock_boundary_process.h
#ifndef dbsksp_trace_shock_boundary_process_h_
#define dbsksp_trace_shock_boundary_process_h_

//:
// \file
// \brief A process to trace the boundary of a shock graph
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Oct 8, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

//: Process that builds a shock graph from a vsol polyline 
class dbsksp_trace_shock_boundary_process : public bpro1_process 
{

public:
  //: Constructor
  dbsksp_trace_shock_boundary_process();
  
  //: Destructor
  virtual ~dbsksp_trace_shock_boundary_process();

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
