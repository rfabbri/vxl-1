// This is seg/dbsks/dbsks_shapelet_stats.h
#ifndef dbsks_shapelet_stats_h_
#define dbsks_shapelet_stats_h_

//:
// \file
// \brief A class to store, compute, and save statistics about a shape fragment
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
#include <dbsksp/dbsksp_shapelet_sptr.h>


// ============================================================================
// dbsks_shapelet_stats
// ============================================================================

// Statistics of a shock graph
class dbsks_shapelet_stats
{
public:
  // Constructors --------------------------------------------------------------
  dbsks_shapelet_stats() : mean_(vnl_vector<double >(5, 0)),
    var_(vnl_vector<double >(5, 0)),
    min_(vnl_vector<double >(5, 0)),
    max_(vnl_vector<double >(5, 0))
  {};

  // Access member variables ---------------------------------------------------

  //: Retrieve statistics about rA
  void stats_log2_rA(double& mean, double& var, double& min, double& max)
  { this->get_stats(0, mean, var, min, max); }

  void set_stats_log2_rA(double mean, double var, double min, double max)
  { this->set_stats(0, mean, var, min, max); }
  

  //: Retrieve statistics about each parameter
  void stats_phiA(double& mean, double& var, double& min, double& max)
  { this->get_stats(1, mean, var, min, max); }

  void set_stats_phiA(double mean, double var, double min, double max)
  { this->set_stats(1, mean, var, min, max); }

  //: Retrieve statistics about each parameter
  void stats_phiB(double& mean, double& var, double& min, double& max)
  { this->get_stats(2, mean, var, min, max); }

  void set_stats_phiB(double mean, double var, double min, double max)
  { this->set_stats(2, mean, var, min, max); }

  //: Retrieve statistics about each parameter
  void stats_m(double& mean, double& var, double& min, double& max)
  { this->get_stats(3, mean, var, min, max); }

  void set_stats_m(double mean, double var, double min, double max)
  { this->set_stats(3, mean, var, min, max); }

  //: Retrieve statistics about each parameter
  void stats_log2_len(double& mean, double& var, double& min, double& max)
  { this->get_stats(4, mean, var, min, max); }

  void set_stats_log2_len(double mean, double var, double min, double max)
  { this->set_stats(4, mean, var, min, max); }


  // Utilities -----------------------------------------------------------------

  //: Compute the shock graph parameter statistics from a set of shock graphs
  // with exactly the same topology
  bool compute_stats(const vcl_vector<dbsksp_shapelet_sptr >& shock_graph_list);

  // Extract statistics of a parameter
  void get_stats(unsigned index, double& mean, double& var, double& min, double& max) const;

  // Set statistics of a parameter
  void set_stats(unsigned index, double mean, double var, double min, double max);

  //: Print summary
  vcl_ostream& print(vcl_ostream& str) const {return str; };

  // Member variables ----------------------------------------------------------

  // 5x1 vector for 5 intrinsic parameters
  // log2_rA
  // phiA
  // phiB
  // m
  // log2_len
  vnl_vector<double > mean_;
  vnl_vector<double > var_;
  vnl_vector<double > max_;
  vnl_vector<double > min_;
};


#endif // seg/dbsks/dbsks_shapelet_stats.h


