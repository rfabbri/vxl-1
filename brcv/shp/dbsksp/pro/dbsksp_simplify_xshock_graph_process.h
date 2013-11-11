// This is dbsksp/pro/dbsksp_simplify_xshock_graph_process.h
#ifndef dbsksp_simplify_xshock_graph_process_h_
#define dbsksp_simplify_xshock_graph_process_h_

//:
// \file
// \brief Simplify an xshock graph while preserving its shape properties
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Jan 26, 2009
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Process that builds a shock graph from a vsol polyline 
class dbsksp_simplify_xshock_graph_process : public bpro1_process 
{
public:
  //: Constructor
  dbsksp_simplify_xshock_graph_process();
  
  //: Destructor
  virtual ~dbsksp_simplify_xshock_graph_process();

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
};

#endif
