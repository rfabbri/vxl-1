#ifndef _dbrl_superresolution_multiple_objects_
#define _dbrl_superresolution_multiple_objects_

#include <bpro1/bpro1_process.h>
#include <georegister/dbrl_id_point_2d_sptr.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <dbbgm/dbbgm_distribution_image.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage_sptr.h>
#include <dbbgm/pro/dbbgm_image_storage_sptr.h>

#include<dbinfo/pro/dbinfo_track_storage.h>
#include<dbinfo/pro/dbinfo_track_storage_sptr.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <georegister/dbrl_feature_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <vil/vil_image_view.h>
//#include <dbbgm/dbbgm_image_of.h>
#include <dbru/dbru_multiple_instance_object.h>
#include <dbru/dbru_multiple_instance_object_sptr.h>
class dbrl_superresolution_multiple_objects:public bpro1_process
    {
    public:

        dbrl_superresolution_multiple_objects();
        ~dbrl_superresolution_multiple_objects();

        std::string name();

        //: Clone the process
        virtual bpro1_process* clone() const;

        int input_frames();
        int output_frames();

        std::vector< std::string > get_input_type();
        std::vector< std::string > get_output_type();

        bool execute();
        bool finish();

    private:
      
        vsol_polygon_2d_sptr dilate_poly(vsol_polygon_2d_sptr poly, double rad);

         std::vector<dbinfo_track_sptr> tracks_;
        //dbbgm_distribution_image_storage_sptr model_storage_;
        dbbgm_image_storage_sptr model_storage_;
        std::vector<dbrl_id_point_2d_sptr> get_foreground_edges_from_observ(dbinfo_observation_sptr obs, dbbgm_image_storage_sptr  inpmodel);
        std::vector<vsol_line_2d_sptr> compute_curve_model_support(std::vector< std::vector< dbrl_id_point_2d_sptr > > pts, int ncols,int nrows);

        std::vector<vsol_spatial_object_2d_sptr> estimate_support(std::vector<dbrl_id_point_2d_sptr> point_list,
                                                                 std::vector<dbrl_id_point_2d_sptr> & supportpoints,
                                                                 float rad, float std, float numiter, float thresh);
        std::vector<vsol_line_2d_sptr>  compute_spatial_support(double sigma_d,double thresh,
                                                     std::vector< std::vector< dbrl_id_point_2d_sptr > > pts);
        std::vector<dbrl_feature_sptr> get_grid_points(std::vector<dbrl_feature_sptr> f2, double spacing);
        vsol_polygon_2d_sptr compute_convex_hull( std::vector<vsol_line_2d_sptr> edges);
        std::vector<dbrl_id_point_2d_sptr> get_edges_from_observ(dbinfo_observation_sptr obs);
        std::vector<dbrl_id_point_2d_sptr> get_foreground_edges_from_observ(dbinfo_observation_sptr obs,dbbgm_distribution_image<float> * model);

        std::vector<dbrl_feature_sptr> get_curvature_edges(std::vector<dbdet_edgel* > all_edgels);
        std::vector<dbrl_feature_sptr> get_neighbor_edges(std::vector<dbdet_edgel* > all_edgels);


        std::vector<std::vector<dbru_multiple_instance_object_sptr> > multiobs;

        std::vector<dbrl_feature_sptr> get_features(std::vector<dbrl_id_point_2d_sptr> points,std::string type);

        std::vector<dbdet_edgel*> edge_detector(vil_image_view<unsigned char> imgview);

        //std::vector<dbdet_edgel*> fg_detect(std::vector<dbdet_edgel*> edges, dbbgm_image_base * model);

        void super_resolute(std::map<dbrl_feature_sptr,unsigned char> fmap,dbinfo_observation_sptr obs,std::string superimgname);

        //vsl_b_ofstream * ofile;

        //bool open_file;
    };
#endif
