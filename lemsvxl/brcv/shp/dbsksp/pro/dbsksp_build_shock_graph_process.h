// This is dbsksp/pro/dbsksp_build_shock_graph_process.h
#ifndef dbsksp_build_shock_graph_process_h_
#define dbsksp_build_shock_graph_process_h_

//:
// \file
// \brief Build a shock graph from a different sources 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 28, 2006
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Process that builds a shock graph from a vsol polyline 
class dbsksp_build_shock_graph_process : public bpro1_process 
{

public:
  //: Constructor
  dbsksp_build_shock_graph_process();
  
  //: Destructor
  virtual ~dbsksp_build_shock_graph_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  //: Returns the name of this process
  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

protected:
  vcl_vector<vcl_string > build_type_descriptions_;
  vcl_vector<vcl_string > shock_graph_types_;

};

#endif
