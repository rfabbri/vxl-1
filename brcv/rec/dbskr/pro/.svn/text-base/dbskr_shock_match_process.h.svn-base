// This is dbskr/pro/dbskr_shock_match_process.h
#ifndef dbskr_shock_matching_process_h_
#define dbskr_shock_matching_process_h_

//:
// \file
// \brief A process to take 2 extrinsic shock graphs and match them
//  
// \author Ozge Can Ozcanli
// \date 09/30/05
//
// \verbatim
//  Modifications
//
// Anil Usumezbas 12/03/09 - Added the use_approx parameter
//
// Anil Usumezbas 01/11/10 - Added the mirror-matching feature
//                           The process now tries matching shock graph 1
//                           to the mirror of shock graph 2 as well as itself
//                           and chooses the pair with the minimum cost for
//                           reporting as a result. This is done only when 
//                           option for using Sebastian's settings is chosen
//                           (doesn't work for combined edit, coarse edit etc.)
//
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <dbskr/pro/dbskr_shock_match_storage.h>
#include <dbskr/pro/dbskr_shock_match_storage_sptr.h>
#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>
#include <vcl_vector.h>

//: This process is for matching extrinsic shock graphs
class dbskr_shock_match_process : public bpro1_process
{
public:
  dbskr_shock_match_process();
  virtual ~dbskr_shock_match_process() {}
  
  virtual vcl_string name() {
    return "Match Shock Graphs";
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

#endif // dbskr_shock_match_process_h_
