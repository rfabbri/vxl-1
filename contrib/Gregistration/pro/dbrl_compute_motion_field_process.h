/**************************************************************/
/*  Name: Vishal Jain (vj)
//  File: dbrl_compute_motion_field_process.h
//  Asgn: bvis
//  Date: Mon July 12 16:01:53 EDT 2003
***************************************************************/

#ifndef dbrl_compute_motion_field_process_header
#define dbrl_compute_motion_field_process_header

#include <bpro1/bpro1_process.h>
#include <bpro1/bpro1_parameters.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vil/vil_load.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_resource_sptr.h>
#include <vcl_vector.h>
#include <vsol/vsol_point_2d.h>
#include <georegister/dbrl_match_set_sptr.h>
#include <georegister/dbrl_match_set.h>

#include <dbdet/edge/dbdet_edgemap_sptr.h>

class dbrl_compute_motion_field_process : public bpro1_process {

public:

  dbrl_compute_motion_field_process();
  ~dbrl_compute_motion_field_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  vcl_string name();

  int input_frames();
  int output_frames();
  
  vcl_vector< vcl_string > get_input_type();
  vcl_vector< vcl_string > get_output_type();

  bool execute();
  bool finish();

  

private:
    vcl_vector<vsol_spatial_object_2d_sptr> feature_to_vsol(vcl_vector<dbrl_feature_sptr> & f);
    vcl_vector<int> framenums_;
dbrl_match_set_sptr compute_motion_field(vcl_vector<dbrl_feature_sptr> tgt_orig,
                                                        vcl_vector<dbrl_feature_sptr> src_orig);

    vcl_vector<vcl_vector<dbrl_feature_sptr> > get_features_from_image();
    vcl_vector<vcl_vector<dbrl_feature_sptr> > get_features_from_vsol();
    vcl_vector<dbrl_feature_sptr> get_features_from_edge_map(dbdet_edgemap_sptr edgemap);


    vgl_point_2d<double> center_of_mass(vcl_vector<dbrl_feature_sptr> & f);
    void normalize_cm(vcl_vector<dbrl_feature_sptr> & f,double xref,double yref);
    vcl_vector<dbrl_feature_sptr> copy_features(vcl_vector<dbrl_feature_sptr> f);

    vcl_vector<vsol_spatial_object_2d_sptr> make_grid_from_points(vcl_vector<dbrl_feature_sptr> features,int xmin,int xmax,int ymin,int ymax);
    vcl_vector<dbrl_feature_sptr> get_grid_points(int xmin,int xmax,int ymin,int ymax);
    void  get_box(vcl_vector<dbrl_feature_sptr> f,int & xmin,int &ymin,int &xmax,int &ymax);



};

#endif
