#ifndef _dbru_show_tps_process_
#define _dbru_show_tps_process_

#include <bpro1/bpro1_process.h>
//#include <vnl/vnl_matrix.h>
//#include <georegister/dbrl_thin_plate_spline_transformation.h>
//#include <vsol/vsol_spatial_object_2d_sptr.h>

class dbru_show_tps_process:public bpro1_process
    {
    public:

        dbru_show_tps_process();
        ~dbru_show_tps_process();

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
      //std::vector<dbrl_feature_sptr> gridify_points(std::vector<dbrl_feature_sptr> points, float i0,float j0,float i1,float j1);
      //void remove_points_on_the_border(std::vector<dbrl_feature_sptr> point1,std::vector<dbrl_feature_sptr> point2,
       //                                float i0,float j0,float i1,float j1,dbrl_correspondence *M);

      //std::vector<std::vector<dbrl_id_point_2d_sptr> >   id_point_set_list_;
      //std::vector<std::vector<dbrl_feature_sptr> >   point_set_list_;

      //std::vector<int> framenums_;

    };
#endif
