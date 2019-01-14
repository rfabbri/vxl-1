/**************************************************************/
/*  Name: Vishal Jain (vj)
//  File: superimpose_frames_process.h
//  Asgn: bvis
//  Date: Mon July 12 16:01:53 EDT 2003
***************************************************************/

#ifndef superimpose_frames_process_header
#define superimpose_frames_process_header

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
#include <georegister/dbrl_id_point_2d.h>
#include <georegister/dbrl_id_point_2d_sptr.h>
class superimpose_frames_process : public bpro1_process {

public:

  superimpose_frames_process();
  ~superimpose_frames_process();

  //: Clone the process
  virtual bpro1_process* clone() const;

  std::string name();

  int input_frames();
  int output_frames();
  
  std::vector< std::string > get_input_type();
  std::vector< std::string > get_output_type();

  bool execute();
  bool finish();

  //: to store the list of images which would be processed in finish
  std::vector<vil_image_resource_sptr>  image_list_;
  std::vector<dbrl_match_set_sptr>  match_set_list_;
  std::vector<std::vector<dbrl_id_point_2d_sptr> > id_point_list_;
  

private:
    std::vector<vsol_spatial_object_2d_sptr> feature_to_vsol(std::vector<dbrl_feature_sptr> & f);
    std::vector<int> framenums_;
    std::vector<dbrl_feature_sptr>registeredges(std::vector<dbrl_feature_sptr> tgt,
                                                std::vector<dbrl_feature_sptr> src,
                                                std::vector<dbrl_transformation_sptr>& xforms);

    std::vector<std::vector<dbrl_feature_sptr> > get_features_from_image();
    std::vector<std::vector<dbrl_feature_sptr> > get_features_from_vsol();
    std::vector<std::vector<dbrl_feature_sptr> > get_features_from_edge_map();


    vgl_point_2d<double> center_of_mass(std::vector<dbrl_feature_sptr> & f);
    void normalize_cm(std::vector<dbrl_feature_sptr> & f,double xref,double yref);
    std::vector<dbrl_feature_sptr> copy_features(std::vector<dbrl_feature_sptr> f);

    std::vector<vsol_spatial_object_2d_sptr> make_grid_from_points(std::vector<dbrl_feature_sptr> features,int xmin,int xmax,int ymin,int ymax);
    std::vector<dbrl_feature_sptr> get_grid_points(int xmin,int xmax,int ymin,int ymax);
    void  get_box(std::vector<dbrl_feature_sptr> f,int & xmin,int &ymin,int &xmax,int &ymax);



};

#endif
