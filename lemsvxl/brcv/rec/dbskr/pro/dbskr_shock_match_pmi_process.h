// This is dbskr/pro/dbskr_shock_match_pmi_process.h
#ifndef dbskr_shock_matching_pmi_process_h_
#define dbskr_shock_matching_pmi_process_h_

//:
// \file
// \brief A process to take 2 extrinsic shock graphs and match them using appearance cost based
//        edit distance algorithms
//  
// \author Ozge Can Ozcanli
// \date 12/11/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <dbskr/pro/dbskr_shock_match_process.h>
#include <dbskr/pro/dbskr_shock_match_storage.h>
#include <dbskr/pro/dbskr_shock_match_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <vcl_vector.h>

//: This process is for matching extrinsic shock graphs
class dbskr_shock_match_pmi_process : public dbskr_shock_match_process
{
public:
  dbskr_shock_match_pmi_process();
  
  //: Clone the process
  virtual bpro1_process* clone() const;
  
  virtual vcl_string name() {
    return "Match Shock Graphs using MI";
  }

  virtual vcl_vector< vcl_string > get_input_type();
  virtual vcl_vector< vcl_string > get_output_type();
  
  virtual bool execute();
};

#endif // dbskr_shock_match_pmi_process_h_
