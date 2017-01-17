// This is dbsksp/pro/dbsksp_average_two_xgraphs_process.h
#ifndef dbsksp_average_two_xgraphs_process_h_
#define dbsksp_average_two_xgraphs_process_h_

//:
// \file
// \brief Compute an average of two generative xshock graphs
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Nov 10, 2009
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>

//: Process that builds a shock graph from a vsol polyline 
class dbsksp_average_two_xgraphs_process : public bpro1_process 
{
public:
  //: Constructor
  dbsksp_average_two_xgraphs_process();
  
  //: Destructor
  virtual ~dbsksp_average_two_xgraphs_process();

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
