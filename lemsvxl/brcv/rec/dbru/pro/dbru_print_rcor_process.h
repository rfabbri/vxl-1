//---------------------------------------------------------------------
// This is brcv/rec/dbru/pro/dbru_print_rcor_process.h
//:
// \file
// \brief process to print sparse correspondences between regions of interest in input images
//        compute_rcor_process should be run first to create rcor data structure
//
// \author
//  Ozge Can Ozcanli         03/12/06
//
// \verbatim
//  Modifications
//
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef dbru_print_rcor_process_h_
#define dbru_print_rcor_process_h_

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
//#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbskr/dbskr_tree_sptr.h>

class dbru_print_rcor_process : public bpro1_process 
{
public:
  dbru_print_rcor_process();
  virtual ~dbru_print_rcor_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Print Region Correspondence";
  }

  vcl_vector< vcl_string > get_input_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "region_cor" );    // 0
    to_return.push_back( "shock_match" );    // 1
    return to_return;
  }

  vcl_vector< vcl_string > get_output_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );
    to_return.push_back( "vsol2D" );
    return to_return;
  }

  int input_frames() { return 1; }
  int output_frames() { return 1; }
  bool execute();
  bool finish() { return true; }

  //utility functions
  void get_parameters();

protected:

  //various parameters
  bool curve_matching;
  bool print_boundary;
  bool curve_cont_matching;
  bool rcor_storage;
  bool shock_match_storage;
  float increment; 
  vcl_string out_file;
};

#endif //dbru_print_rcor_process_h_
