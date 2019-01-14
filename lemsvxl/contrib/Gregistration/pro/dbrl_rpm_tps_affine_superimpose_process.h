#ifndef _dbrl_rpm_tps_affine_superimpose_process_
#define _dbrl_rpm_tps_affine_superimpose_process_

#include <bpro1/bpro1_process.h>
#include <georegister/dbrl_id_point_2d_sptr.h>
#include <georegister/dbrl_feature_sptr.h>
#include <georegister/dbrl_match_set_sptr.h>

#include <vnl/vnl_matrix.h>
#include <georegister/dbrl_thin_plate_spline_transformation.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

class dbrl_rpm_tps_affine_superimpose_process:public bpro1_process
    {
    public:

        dbrl_rpm_tps_affine_superimpose_process();
        ~dbrl_rpm_tps_affine_superimpose_process();

        std::string name();

        //: Clone the process
        virtual bpro1_process* clone() const;

        int input_frames();
        int output_frames();

        std::vector< std::string > get_input_type();
        std::vector< std::string > get_output_type();

        bool execute();
        bool finish();


        std::vector<dbrl_feature_sptr> generate_grid();
        std::vector<dbrl_feature_sptr> warp_grid(dbrl_thin_plate_spline_transformation * tpstform,
                                                std::vector<dbrl_feature_sptr> &f);


        std::vector<vsol_spatial_object_2d_sptr>feature_to_vsol(std::vector<dbrl_feature_sptr> & f);
        std::vector<vsol_spatial_object_2d_sptr> draw_grid(std::vector<dbrl_feature_sptr> grid_pts, int rows, int cols);

        vnl_vector<double> center_of_mass(std::vector<dbrl_feature_sptr> & f);


    private:
      std::vector<std::vector<dbrl_id_point_2d_sptr> >   id_point_set_list_;
      std::vector<std::vector<dbrl_feature_sptr> >   point_set_list_;
      std::vector<dbrl_match_set_sptr> match_set_list_;

      std::vector<int> framenums_;
      bool from_points_;
      bool from_match_set_;


    };
#endif
