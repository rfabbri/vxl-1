// This is dbsks/pro/dbsks_trace_opt_graph_bnd_process.h
#ifndef dbsks_trace_opt_graph_bnd_process_h_
#define dbsks_trace_opt_graph_bnd_process_h_

//:
// \file
// \brief 
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Dec 10, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vector>
#include <string>

//: 
class dbsks_trace_opt_graph_bnd_process : public bpro1_process 
{

public:
  //: Constructor
  dbsks_trace_opt_graph_bnd_process();
  
  //: Destructor
  virtual ~dbsks_trace_opt_graph_bnd_process();

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
