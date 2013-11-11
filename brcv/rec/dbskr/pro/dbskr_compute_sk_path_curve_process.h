// This is brcv/rec/dbskr/pro/dbskr_compute_sk_path_curve_process.h
#ifndef dbskr_compute_sk_path_curve_process_h_
#define dbskr_compute_sk_path_curve_process_h_
//:
// \file
// \brief This process computes the sk_path_curve for a given path (mostly for debug)
//
// \author Amir Tamrakar
// \date 09/25/05
//
// \verbatim
//  Modifications
//
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>

#include <dbskr/dbskr_scurve_sptr.h>

class dbskr_compute_sk_path_curve_process : public bpro1_process 
{
public:

  dbskr_compute_sk_path_curve_process();
  virtual ~dbskr_compute_sk_path_curve_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  int input_frames();
  int output_frames();

  bool execute();
  bool finish();

private:
  vcl_vector<int> parseLineForNumbers(vcl_string s);

  void get_path_from_ids (dbsk2d_shock_graph_sptr shock_graph,
                          dbsk2d_shock_node_sptr & start_node,
                          vcl_vector<dbsk2d_shock_edge_sptr> & path1, 
                          int start_node_id, vcl_string sh_path);

  dbskr_scurve_sptr dbskr_compute_scurve_Thomas(dbsk2d_shock_node_sptr start_node,
                            vcl_vector<dbsk2d_shock_edge_sptr> path,
                            bool binterpolate, bool bsub_sample);

  
  void compare_curves(dbskr_scurve_sptr c1, dbskr_scurve_sptr c2);
};

#endif //dbskr_compute_sk_path_curve_process_h_
