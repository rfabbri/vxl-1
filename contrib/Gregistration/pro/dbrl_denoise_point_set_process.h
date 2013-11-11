#ifndef _dbrl_denoise_point_set_process_h_
#define _dbrl_denoise_point_set_process_h_

#include <bpro1/bpro1_process.h>

class dbrl_denoise_point_set_process:public bpro1_process
    {
    public:

        dbrl_denoise_point_set_process();
        ~dbrl_denoise_point_set_process();

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
