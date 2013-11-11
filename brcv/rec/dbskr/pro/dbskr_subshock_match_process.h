// This is dbskr/pro/dbskr_subshock_match_process.h
#ifndef dbskr_subshock_match_process_h_
#define dbskr_subshock_match_process_h_

//:
// \file
// \brief A process to take a graph (with no cycles) and match it to subgraphs (with no cycles) of the second graph
//  
// \author Ozge Can Ozcanli
// \date 12/26/06
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <dbskr/pro/dbskr_shock_match_storage.h>
#include <dbskr/pro/dbskr_shock_match_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <vcl_vector.h>

//: This process is for matching extrinsic shock graphs
class dbskr_subshock_match_process : public bpro1_process
{
public:
  dbskr_subshock_match_process();
  virtual ~dbskr_subshock_match_process() {}
  
  virtual vcl_string name() {
    return "Match Shock Subgraphs";
  }

  //: Clone the process
  virtual bpro1_process* clone() const;
  
  virtual vcl_vector< vcl_string > get_input_type();
  virtual vcl_vector< vcl_string > get_output_type();
  
  int input_frames() {
    return 1;
  }
  int output_frames() {
    return 1;
  }
  
  virtual bool execute();
  bool finish() {
    return true;
  }

};

#endif // dbskr_subshock_match_process_h_
