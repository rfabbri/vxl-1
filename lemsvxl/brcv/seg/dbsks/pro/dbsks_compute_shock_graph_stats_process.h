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
#include <vector>
#include <string>


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
  std::string name();

  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

  bool parse_lines_from_file(std::string fname, std::vector<std::string>& strings);
};

#endif
