#ifndef _spatemp_backward_beta_process_
#define _spatemp_backward_beta_process_

#include <bpro1/bpro1_process.h>
#include <vil/vil_image_view.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
//#include <Spatemp/vis/bpro_spatemp_process_manager.h>

class spatemp_backward_beta_process:public bpro1_process
    {
    public:

        spatemp_backward_beta_process();
        virtual ~spatemp_backward_beta_process();

        vcl_string name();

        //: Clone the process
        virtual bpro1_process * clone() const;

        int input_frames();
        int output_frames();
        void set_forward_input_frames();


        vcl_vector< vcl_string > get_input_type();
        vcl_vector< vcl_string > get_output_type();

          virtual bool non_causal() const { return true; }

        bool execute();
        bool finish();



    private:


    };
#endif
