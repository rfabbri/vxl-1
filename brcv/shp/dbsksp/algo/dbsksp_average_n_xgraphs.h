// This is brcv/shp/dbsksp/algo/dbsksp_average_n_xgraphs.h

#ifndef dbsksp_average_n_xgraphs_h_
#define dbsksp_average_n_xgraphs_h_


//:
// \file
// \brief A class to compute the average of two shock graphs
//
// \author Nhon H. Trinh (ntrinh@lems.brown.edu)
// \date Nov 12, 2009
// \verbatim
//   Modifications
//     Nhon Trinh     Nov 12, 2009     Initial version
//
// \endverbatim
//

#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbsksp/algo/dbsksp_xshock_directed_tree_sptr.h>
#include <dbsksp/algo/dbsksp_edit_distance.h>
#include <vcl_vector.h>

//==============================================================================
// dbsksp_average_n_xgraphs
//==============================================================================

//: A class to compute an average n shock graphs
class dbsksp_average_n_xgraphs
{
public:
  // Constructors / Destructors--------------------------------------------------

  //: Constructor
  dbsksp_average_n_xgraphs();

  //: Destructor
  virtual ~dbsksp_average_n_xgraphs() {}

  // Data access----------------------------------------------------------------

  //: Return parent xgraph, index =0 or =1
  dbsksp_xshock_graph_sptr parent_xgraph(int index) const
  {
    return this->parent_xgraphs_[index];
  }

  //: Set list of parent xgraphs
  void set_parent_xgraphs(const vcl_vector<dbsksp_xshock_graph_sptr >& xgraphs,
    const vcl_vector<vcl_string >& names = vcl_vector<vcl_string >(0) );

  //: Return scurve_matching_R
  float scurve_matching_R() const
  { return this->scurve_matching_R_; }

  //: Set curve matching R
  void set_scurve_matching_R(float R)
  { this->scurve_matching_R_ = R; }

  //: Return sampling param
  float scurve_sample_ds() const
  {
    return this->scurve_sample_ds_;
  }

  //: Set sample ds
  void set_scurve_sample_ds(float ds)
  {
    this->scurve_sample_ds_ = ds;
  }

  //: Set tolerance for relative error (ratio err / original distance)
  void set_relative_error_tol(double tol)
  {
    this->relative_error_tol_ = tol;
  }

  //: Set base name to save intermediate data
  void set_base_name(const vcl_string& base_name)
  { this->base_name_ = base_name; }


  //: Average xgraph
  dbsksp_xshock_graph_sptr average_xgraph() const
  {
    return this->average_xgraph_;
  }

  //: Computed distance to parent shapes
  void get_distance_parents_to_average(vcl_vector<double >& distance_parents_to_average) const
  {
    distance_parents_to_average = this->distance_parent_to_avg_;
  }

  void get_deform_cost_parents_to_average(vcl_vector<double >& deform_cost_parents_to_average) const
  {
    deform_cost_parents_to_average = this->deform_cost_parent_to_avg_;
  }



  //: Flag to print intermediate results
  bool debugging() const
  {
    return this->base_name_ != "";
  }


  //: Print debug info to a file
  void print_debug_info(const vcl_string& str) const;
  

  // UTILITIES------------------------------------------------------------------

  //: Execute the averaging function
  virtual bool compute() = 0;

  //: Compute weighted average of two xgraphs
  bool compute_average_xgraph(const dbsksp_xshock_graph_sptr& xgraph1,
    const dbsksp_xshock_graph_sptr xgraph2,
    double weight1,
    double weight2,
    const vcl_string& name1,
    const vcl_string& name2,
    dbsksp_xshock_graph_sptr& average_xgraph,
    double& relative_error);
  
protected:
  //: Initialize distances
  void init();

  //: Compute distance between two xgraphs - keeping the intermediate work
  double compute_edit_distance(const dbsksp_xshock_graph_sptr& xgraph1, 
    const dbsksp_xshock_graph_sptr& xgraph2,
    dbsksp_edit_distance& work);

  // Member variables
protected:
  
  // User-input-----------------------------------------------------------------

  //: The two parent xgraphs
  vcl_vector<dbsksp_xshock_graph_sptr > parent_xgraphs_;

  //: Name of the parent objects
  vcl_vector<vcl_string > parent_names_;

  //: sampling params
  float scurve_sample_ds_;

  //: weight param
  float scurve_matching_R_;

  //: Tolerance for relative error
  double relative_error_tol_;

  // Ouput----------------------------------------------------------------------

  //: Average xgraph
  dbsksp_xshock_graph_sptr average_xgraph_;

  //: Distance the two original xgraph
  vcl_vector<double > distance_parent_to_avg_;

  vcl_vector<double > deform_cost_parent_to_avg_;

  // Intermediate results-------------------------------------------------------

  // base filename to save relevant data (mostly for debugging purpose)
  vcl_string base_name_;

};



//==============================================================================
// dbsksp_average_n_xgraphs_iterative_merge
//==============================================================================

//: A class to compute an average n shock graphs by iterative merging of nearest pairs
class dbsksp_average_n_xgraphs_iterative_merge: public dbsksp_average_n_xgraphs
{
public:

  // Constructor
  dbsksp_average_n_xgraphs_iterative_merge(){}

  //: Destructor:
  ~dbsksp_average_n_xgraphs_iterative_merge(){}

  //: Compute average
  virtual bool compute();


};











//==============================================================================
// dbsksp_average_n_xgraphs_by_curve_shortening
//==============================================================================

//: A class to compute an average n shock graphs by curve shortening approach
class dbsksp_average_n_xgraphs_by_curve_shortening: public dbsksp_average_n_xgraphs
{
public:

  // Constructor
  dbsksp_average_n_xgraphs_by_curve_shortening(){}

  //: Destructor:
  ~dbsksp_average_n_xgraphs_by_curve_shortening(){}

  //: Compute average
  virtual bool compute();

  //: Generate a random permutation of the squence (0, 1, ..., n-1)
  vcl_vector<unsigned > random_permutation(unsigned n);


};




#endif // dbsksp_average_n_xgraphs_h_

