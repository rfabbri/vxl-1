// This is seg/dbsks/dbsks_shock_graph_stats.h
#ifndef dbsks_shock_graph_stats_h_
#define dbsks_shock_graph_stats_h_

//:
// \file
// \brief A class to store, compute, and save statistics about a shock graph
//        
// \author Nhon Trinh ( ntrinh@lems.brown.edu)
// \date May 18, 2008
//
// \verbatim
//  Modifications
// \endverbatim


#include <vnl/vnl_vector.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_string.h>
#include <vcl_map.h>
#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsks/dbsks_shapelet_grid.h>
#include <dbsks/dbsks_shapelet_stats.h>


//: Parameters necessary to sample a distribution of shapelet
struct dbsks_shapelet_sampling_params
{
  float center_x;
  float center_y;
  float step_x;
  float step_y;
  int num_x;
  int num_y;
  
  int num_psi;
  int num_phiA;
  int num_phiB;
  int num_m;
  int num_len;
  int num_rA;
};






// ============================================================================
// dbsks_shock_graph_stats
// ============================================================================

// Statistics of a shock graph
class dbsks_shock_graph_stats
{
public:
  // Constructor - default -----------------------------------------------------
  dbsks_shock_graph_stats(){};

  //: Constructor 2
  dbsks_shock_graph_stats(const dbsksp_shock_graph_sptr& graph) :
  shock_graph_(graph) {};

  //: Clear all existing statistics
  void clear();

  // Access member variables ---------------------------------------------------

  //: The underlying shock graph
  dbsksp_shock_graph_sptr shock_graph() const {return this->shock_graph_; }

  //: Set the shock graph
  void set_shock_graph(const dbsksp_shock_graph_sptr& sg)
  { this->shock_graph_ = sg; }

  //: Size of the graph the statistics corresponds to
  double graph_size() const {return this->graph_size_; }
  void set_graph_size(double graph_size) {this->graph_size_ = graph_size; }

  // Utilities -----------------------------------------------------------------

  //: Compute the shock graph parameter statistics from a set of shock graphs
  // REQUIREMENT: all shock graphs have exactly the same topology
  bool compute_stats(const vcl_vector<dbsksp_shock_graph_sptr >& shock_graph_list);


  //: Return statistics of one fragment
  dbsks_shapelet_stats shapelet_stats(unsigned int edge_id) const;

  //: Set statistics of one fragment
  void set_shapelet_stats(unsigned int edge_id, 
    const dbsks_shapelet_stats& frag_stats);

  //: Generate a shapelet grid based on the computed stats and sampling parameters
  // (number of samples per parameters, etc)
  // "target_graph_size" is the size of the graph this grid corresponds to
  // Use 0 to keep the size as in the stats.
  dbsks_shapelet_grid build_shapelet_grid(unsigned int edge_id,
    const dbsks_shapelet_sampling_params& sampling_params,
    double target_graph_size = 0) const;

  //: Print summary
  vcl_ostream& print(vcl_ostream& str) const;


  // Member variables ----------------------------------------------------------
protected:
  dbsksp_shock_graph_sptr shock_graph_;


  //: Size of the graph the statistics corresponds to
  // Use of the internal graph's size is not reliable in general because the 
  // graph can be changed by outside functions
  double graph_size_;

  // A mapping between the edge id and the statistics of its corresponding fragment
  vcl_map<unsigned int, dbsks_shapelet_stats > shapelet_stats_map_;
};


#endif // seg/dbsks/dbsks_shock_graph_stats.h


