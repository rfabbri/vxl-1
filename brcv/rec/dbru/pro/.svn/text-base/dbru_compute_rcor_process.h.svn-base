//---------------------------------------------------------------------
// This is brcv/rec/dbru/pro/dbru_compute_rcor_process.h
//:
// \file
// \brief process to compute corresponce between regions of interest in input images
//
// \author
//  Amir Tamrakar         12/04/05
//
// \verbatim
//  Modifications
//   Amir Tamrakar        Duplicated the dbru_dbinfo_process to compute 
//                        just the region correspondence
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef dbru_compute_rcor_process_h_
#define dbru_compute_rcor_process_h_

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbskr/dbskr_tree_sptr.h>

class dbru_compute_rcor_process : public bpro1_process 
{
public:
  dbru_compute_rcor_process();
  virtual ~dbru_compute_rcor_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Compute Region Correspondence";
  }

  vcl_vector< vcl_string > get_input_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vsol2D" );   // 0
    to_return.push_back( "shock" );    // 1
    to_return.push_back( "image" );    // 2
    to_return.push_back( "vsol2D" );   // 3
    to_return.push_back( "shock" );    // 4
    to_return.push_back( "image" );    // 5
    to_return.push_back( "region_cor" );    // 6
    return to_return;
  }

  vcl_vector< vcl_string > get_output_type() {
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "region_cor" );
    to_return.push_back( "image" );  // correspondence and used pixel images
    to_return.push_back( "image" );
    to_return.push_back( "image" );
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
  float rms, restricted_cvmatch_ratio, R;
  //bool even_matching;//bool geno_interpolation;
  bool shock_matching;
  bool line_intersections, line_intersections2, line_intersections3, line_intersections4;
  bool save_histograms, use_sil_cor;
  bool distance_transform, dt2;
  float scurve_sample_ds;
  float prune_threshold;
  float increment; 
  bool load1, load2, use_shgm;
  vcl_string esf_file1;
  vcl_string esf_file2;
  vcl_string shgm_file;
  dbskr_tree_sptr tree1, tree2;

};

#endif //dbru_compute_rcor_process_h_
