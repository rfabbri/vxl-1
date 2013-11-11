#ifndef _dbrl_superimpose_2ndtier_
#define _dbrl_superimpose_2ndtier_

#include <bpro1/bpro1_process.h>

#include <georegister/dbrl_id_point_2d_sptr.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>

class dbrl_superimpose_2ndtier:public bpro1_process
    {
    public:

        dbrl_superimpose_2ndtier();
        ~dbrl_superimpose_2ndtier();

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
      
        
    };
#endif
