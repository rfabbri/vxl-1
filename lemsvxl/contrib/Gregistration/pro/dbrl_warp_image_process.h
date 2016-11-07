#ifndef _dbrl_warp_image_process_
#define _dbrl_warp_image_process_

#include <bpro1/bpro1_process.h>
#include <georegister/dbrl_id_point_2d_sptr.h>
#include <georegister/dbrl_feature_sptr.h>
#include <georegister/dbrl_match_set_sptr.h>

#include <vnl/vnl_matrix.h>
#include <georegister/dbrl_thin_plate_spline_transformation.h>
#include <vil/vil_image_resource_sptr.h>

class dbrl_warp_image_process:public bpro1_process
    {
    public:

        dbrl_warp_image_process();
        ~dbrl_warp_image_process();

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

                vcl_vector<dbrl_feature_sptr> warp_grid(dbrl_thin_plate_spline_transformation * tpstform,
                                                vcl_vector<dbrl_feature_sptr> &f);
                vcl_vector<dbrl_match_set_sptr >   match_set_list_;
                      vcl_vector<int> framenums_;

                vcl_vector<vil_image_resource_sptr>   image_list_;

    };
#endif
