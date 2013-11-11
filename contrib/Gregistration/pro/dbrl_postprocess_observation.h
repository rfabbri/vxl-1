#ifndef _dbrl_postprocess_observation_
#define _dbrl_postprocess_observation_

#include <bpro1/bpro1_process.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <vil/vil_image_resource_sptr.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>

class dbrl_postprocess_observation:public bpro1_process
    {
    public:

        dbrl_postprocess_observation();
        ~dbrl_postprocess_observation();

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
      
        void compute_clips_from_observ(dbinfo_observation_sptr obs,
                                       vil_image_resource_sptr img);

        bool compute_curvelets_from_observation(vcl_vector<dbinfo_observation_sptr> obs);
    };
#endif
