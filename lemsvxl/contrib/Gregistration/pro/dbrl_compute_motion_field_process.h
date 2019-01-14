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
#include <vector>
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

  std::string name();

  int input_frames();
  int output_frames();
  
  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

  

private:
    std::vector<vsol_spatial_object_2d_sptr> feature_to_vsol(std::vector<dbrl_feature_sptr> & f);
    std::vector<int> framenums_;
dbrl_match_set_sptr compute_motion_field(std::vector<dbrl_feature_sptr> tgt_orig,
                                                        std::vector<dbrl_feature_sptr> src_orig);

    std::vector<std::vector<dbrl_feature_sptr> > get_features_from_image();
    std::vector<std::vector<dbrl_feature_sptr> > get_features_from_vsol();
    std::vector<dbrl_feature_sptr> get_features_from_edge_map(dbdet_edgemap_sptr edgemap);


    vgl_point_2d<double> center_of_mass(std::vector<dbrl_feature_sptr> & f);
    void normalize_cm(std::vector<dbrl_feature_sptr> & f,double xref,double yref);
    std::vector<dbrl_feature_sptr> copy_features(std::vector<dbrl_feature_sptr> f);

    std::vector<vsol_spatial_object_2d_sptr> make_grid_from_points(std::vector<dbrl_feature_sptr> features,int xmin,int xmax,int ymin,int ymax);
    std::vector<dbrl_feature_sptr> get_grid_points(int xmin,int xmax,int ymin,int ymax);
    void  get_box(std::vector<dbrl_feature_sptr> f,int & xmin,int &ymin,int &xmax,int &ymax);



};

#endif
