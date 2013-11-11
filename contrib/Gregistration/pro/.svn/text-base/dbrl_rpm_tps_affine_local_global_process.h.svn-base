#ifndef _dbrl_rpm_tps_affine_local_global_process_
#define _dbrl_rpm_tps_affine_local_global_process_

#include <bpro1/bpro1_process.h>
#include <georegister/dbrl_id_point_2d_sptr.h>
#include <georegister/dbrl_feature_sptr.h>
#include <georegister/dbrl_correspondence.h>

#include <vnl/vnl_matrix.h>
#include <georegister/dbrl_thin_plate_spline_transformation.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

class dbrl_rpm_tps_affine_local_global_process:public bpro1_process
    {
    public:

        dbrl_rpm_tps_affine_local_global_process();
        ~dbrl_rpm_tps_affine_local_global_process();

        vcl_string name();

        //: Clone the process
        virtual bpro1_process* clone() const;

        int input_frames();
        int output_frames();

        vcl_vector< vcl_string > get_input_type();
        vcl_vector< vcl_string > get_output_type();

        bool execute();
        bool finish();

    private:
      vcl_vector<dbrl_feature_sptr> gridify_points(vcl_vector<dbrl_feature_sptr> points, float i0,float j0,float i1,float j1);
      void remove_points_on_the_border(vcl_vector<dbrl_feature_sptr> point1,vcl_vector<dbrl_feature_sptr> point2,
                                       float i0,float j0,float i1,float j1,dbrl_correspondence *M);

      vcl_vector<vcl_vector<dbrl_id_point_2d_sptr> >   id_point_set_list_;
      vcl_vector<vcl_vector<dbrl_feature_sptr> >   point_set_list_;

      vcl_vector<int> framenums_;

    };
#endif
