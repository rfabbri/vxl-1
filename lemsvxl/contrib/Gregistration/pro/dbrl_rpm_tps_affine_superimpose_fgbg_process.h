#ifndef _dbrl_rpm_tps_affine_superimpose_fgbg_process_
#define _dbrl_rpm_tps_affine_superimpose_fgbg_process_

#include <bpro1/bpro1_process.h>
#include <georegister/dbrl_id_point_2d_sptr.h>
#include <georegister/dbrl_feature_sptr.h>
#include <vnl/vnl_matrix.h>
#include <georegister/dbrl_thin_plate_spline_transformation.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <georegister/dbrl_correspondence.h>
#include <vil/vil_image_view.h>

class dbrl_rpm_tps_affine_superimpose_fgbg_process:public bpro1_process
    {
    public:

        dbrl_rpm_tps_affine_superimpose_fgbg_process();
        ~dbrl_rpm_tps_affine_superimpose_fgbg_process();

        vcl_string name();

        //: Clone the process
        virtual bpro1_process* clone() const;

        int input_frames();
        int output_frames();

        vcl_vector< vcl_string > get_input_type();
        vcl_vector< vcl_string > get_output_type();

        bool execute();
        bool finish();


        vcl_vector<dbrl_feature_sptr> generate_grid();
        vcl_vector<dbrl_feature_sptr> warp_grid(dbrl_thin_plate_spline_transformation * tpstform,
                                                vcl_vector<dbrl_feature_sptr> &f);


        vcl_vector<vsol_spatial_object_2d_sptr>feature_to_vsol(vcl_vector<dbrl_feature_sptr> & f);
        vcl_vector<vsol_spatial_object_2d_sptr> draw_grid(vcl_vector<dbrl_feature_sptr> grid_pts, int rows, int cols);


        void get_corresponding_pairs_fg(vcl_vector<dbrl_feature_sptr> &f1in,vcl_vector<dbrl_feature_sptr> &f2in, 
                                        dbrl_correspondence * M,vcl_vector<dbrl_feature_sptr> &f1,
                                        vcl_vector<dbrl_feature_sptr> &f2);
        void get_corresponding_pairs_bg(vil_image_view<unsigned char> & img1,vil_image_view<unsigned char> & img2,
                                        vcl_vector<dbrl_feature_sptr> &f1,vcl_vector<dbrl_feature_sptr> &f2);



    private:
      vcl_vector<vil_image_view<unsigned char> >   bg_image_list_;
      vcl_vector<vcl_vector<dbrl_feature_sptr> >   fg_point_set_list_;
      vcl_vector<int> framenums_;

    };
#endif
