//---------------------------------------------------------------------
// This is brcv/rec/dbru/pro/dbru_run_osl_process.h
//:
// \file
// \brief process to select objects from OSL and run region correspondence and mutual info algorithms
//
// \author
//  O.C. Ozcanli - Dec 13, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef dbru_run_osl_process_h_
#define dbru_run_osl_process_h_

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <dbru/pro/dbru_osl_storage.h>

//#include <vsol/vsol_spatial_object_2d.h>
//#include <vsol/vsol_spatial_object_2d_sptr.h>
//#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <dbskr/dbskr_tree_sptr.h>


class dbru_run_osl_process : public bpro1_process 
{
public:
  dbru_run_osl_process();
  virtual ~dbru_run_osl_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Run query on OSL";
  }

  vcl_vector< vcl_string > get_input_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "osl" );
    return to_return;
  }
  vcl_vector< vcl_string > get_output_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "region_cor" );
    to_return.push_back( "shock_match" );
    to_return.push_back( "image" );
    to_return.push_back( "image" );
    to_return.push_back( "image" );
    return to_return;
  }

  int input_frames() {
    return 1;
  }
  int output_frames() {
    return 1;
  }

  bool execute();
  bool finish() {
    return true;
  }

protected:
  bool shock_;
  bool shock_pmi_;
  bool dt_;
  bool line_;

  int query_object_id_;
  int database_object_id_;
  int query_polygon_id_;
  int database_polygon_id_;

  int increment_;
  float rms_; float max_value_;
  float R_;
  float restricted_cvmatch_ratio_;
  bool use_Amir_shock_extraction_;
  bool dynamic_pruning_;
  float pruning_threshold_;
  float shock_curve_ds_;
  //if the outlining bounding box length is 100, use threshold 0.2 (base_thres_)
  //  if this length halves, use 0.2/4
  //  if this length doubles use 0.2*4
  float base_thres_;
  bool rigid_alignment_;
  float dx_, dr_, ds_, ratio_; int Nob_;

  bool get_tree(dbskr_tree_sptr& tree, vsol_polygon_2d_sptr poly);
  
private:
  

};

#endif
