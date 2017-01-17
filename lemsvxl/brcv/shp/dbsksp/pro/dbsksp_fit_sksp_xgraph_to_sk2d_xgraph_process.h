// This is dbsksp/pro/dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process.h
#ifndef dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process_h_
#define dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process_h_

//:
// \file
// \brief Fit a generative dbsksp_xshock_graph to an extrinsic shock graph of type dbsk2d_shock_graph
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Oct 22, 2009
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Process that builds a shock graph from a vsol polyline 
class dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process : public bpro1_process 
{
public:
  //: Constructor
  dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process();
  
  //: Destructor
  virtual ~dbsksp_fit_sksp_xgraph_to_sk2d_xgraph_process();

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
