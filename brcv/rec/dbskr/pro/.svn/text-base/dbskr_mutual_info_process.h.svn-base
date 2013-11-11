//---------------------------------------------------------------------
// This is brcv/rec/dbskr/pro/dbskr_mutual_info_process.h
//:
// \file
// \brief process to find mutual information between regions of interest in input images
//
// \author
//  O.C. Ozcanli - June 08, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef dbskr_mutual_info_process_h_
#define dbskr_mutual_info_process_h_

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbsk2d/pro/dbsk2d_shock_storage.h>
#include <dbsk2d/pro/dbsk2d_shock_storage_sptr.h>

//#include <vsol/vsol_spatial_object_2d.h>
//#include <vsol/vsol_spatial_object_2d_sptr.h>
//#include <vsol/vsol_point_2d.h>
//#include <vsol/vsol_line_2d.h>

#include <dbskr/dbskr_tree_sptr.h>
#include <dbskr/dbskr_scurve_sptr.h>

typedef vcl_pair<vcl_pair<int, int>, vcl_pair<int, int> > pathtable_key;

class dbskr_mutual_info_process : public bpro1_process 
{
public:
  dbskr_mutual_info_process();
  virtual ~dbskr_mutual_info_process() {}

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name() {
    return "Mutual Info with Shock Matching";
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
  bool finish() {
    return true;
  }

  float get_total_info()        { return total_info_; }
  float get_int_mutual_info()   { return int_mutual_info_; }
  float get_grad_mutual_info()  { return grad_mutual_info_; }
  float get_color_mutual_info() { return color_mutual_info_; }

protected:
  float total_info_;
  float int_mutual_info_;
  float grad_mutual_info_;
  float color_mutual_info_;

private:
  vcl_vector<pathtable_key> read_shgm(dbskr_tree_sptr tree1, dbskr_tree_sptr tree2, vcl_string fname); 
  void get_correspondence(dbskr_tree_sptr tree1, dbskr_tree_sptr tree2, 
                          vcl_vector<pathtable_key>& path_map, 
                          vcl_vector<dbskr_scurve_sptr>& curve_list1, 
                          vcl_vector<dbskr_scurve_sptr>& curve_list2, 
                          vcl_vector<vcl_vector<vcl_pair<int, int> > >& map_list);

  

};

#endif
