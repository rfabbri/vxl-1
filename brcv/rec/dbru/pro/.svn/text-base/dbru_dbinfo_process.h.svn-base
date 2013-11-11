//---------------------------------------------------------------------
// This is brcv/rec/dbru/pro/dbru_dbinfo_process.h
//:
// \file
// \brief process to find mutual information between regions of interest in input images
//
// \author
//  O.C. Ozcanli - November 11, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef dbcvr_dbinfo_process_h_
#define dbcvr_dbinfo_process_h_

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

//#include <dbcvr/dbcvr_cvmatch.h>
//#include <dbcvr/dbcvr_cvmatch_sptr.h>
//#include <dbcvr/dbcvr_clsd_cvmatch.h>
//#include <dbcvr/dbcvr_clsd_cvmatch_sptr.h>

#include <dbru/algo/dbru_object_matcher.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

//#include <dbcvr/pro/dbcvr_curvematch_storage.h>
//#include <dbcvr/pro/dbcvr_curvematch_storage_sptr.h>

#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

class dbru_dbinfo_process : public bpro1_process 
{
public:
  dbru_dbinfo_process();
  virtual ~dbru_dbinfo_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Observation Info Matching";
  }

  vcl_vector< vcl_string > get_input_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );   // 0
    to_return.push_back( "shock" );    // 1
    to_return.push_back( "image" );    // 2
    to_return.push_back( "vsol2D" );   // 3
    to_return.push_back( "shock" );    // 4
    to_return.push_back( "image" );    // 5
    return to_return;
  }
  vcl_vector< vcl_string > get_output_type() {
    vcl_vector< vcl_string > to_return;
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

  bool finish() { return true; }

  void get_parameters();

  float get_total_info()        { return total_info_; }
  float get_int_mutual_info()   { return int_mutual_info_; }
  float get_grad_mutual_info()  { return grad_mutual_info_; }
  float get_color_mutual_info() { return color_mutual_info_; }

protected:
  float total_info_;
  float int_mutual_info_;
  float grad_mutual_info_;
  float color_mutual_info_;

  float rms, restricted_cvmatch_ratio, R;
  //bool even_matching;//bool geno_interpolation;
  bool shock_matching;
  bool line_intersections;
  bool distance_transform;
  float scurve_sample_ds;
  int increment; 
  bool load1, load2;
  vcl_string esf_file1;
  vcl_string esf_file2;
  vcl_string shgm_file;
  dbskr_tree_sptr tree1, tree2;

  bool elastic_splice_cost;

};

#endif
