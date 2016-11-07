// This is dber/pro/dber_edge_db_match_process.h
#ifndef dber_edge_db_matching_process_h_
#define dber_edge_db_matching_process_h_

//:
// \file
// \brief A process to take edgel sets of a video and match them to a given db instance
//  
// \author Ozge Can Ozcanli
// \date 09/30/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>
#include <dber/pro/dber_edge_match_storage.h>
#include <dber/pro/dber_edge_match_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vcl_vector.h>

//: This process is for matching extrinsic shock graphs
class dber_edge_db_match_process : public bpro1_process
{
public:
  dber_edge_db_match_process();
  virtual ~dber_edge_db_match_process() {}
  
  virtual vcl_string name() {
    return "Match Edgel Sets Video";
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

#endif // dber_edge_db_match_process_h_
