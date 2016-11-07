// This is dbsks/pro/dbsks_compute_shock_graph_stats_process.h
#ifndef dbsks_compute_shock_graph_stats_process_h_
#define dbsks_compute_shock_graph_stats_process_h_

//:
// \file
// \brief Compute statistics from a set of similar shock graphs
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date May 18, 2007
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <vcl_vector.h>
#include <vcl_string.h>


//: 
class dbsks_compute_shock_graph_stats_process : public bpro1_process 
{

public:
  //: Constructor
  dbsks_compute_shock_graph_stats_process();
  
  //: Destructor
  virtual ~dbsks_compute_shock_graph_stats_process();

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

  bool parse_lines_from_file(vcl_string fname, vcl_vector<vcl_string>& strings);
};

#endif
