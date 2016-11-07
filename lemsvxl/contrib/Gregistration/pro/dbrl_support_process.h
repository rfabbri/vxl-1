#ifndef _dbrl_support_process_
#define _dbrl_support_process_

#include <bpro1/bpro1_process.h>
#include <georegister/dbrl_support.h>
#include <vil/vil_image_view.h>


class dbrl_support_process:public bpro1_process
    {
    public:

        dbrl_support_process();
        ~dbrl_support_process();

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

        vcl_vector<vil_image_view<unsigned char> >  image_list_;
        vcl_vector<int> framenums_;

    };
#endif
