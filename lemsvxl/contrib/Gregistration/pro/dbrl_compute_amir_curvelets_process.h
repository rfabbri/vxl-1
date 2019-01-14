#ifndef _dbrl_compute_amir_curvelets_process_
#define _dbrl_compute_amir_curvelets_process_

#include <bpro1/bpro1_process.h>
#include <georegister/dbrl_support.h>
#include <vil/vil_image_view.h>
#include <georegister/dbrl_id_point_2d_sptr.h>
#include <vsol/vsol_line_2d_sptr.h>

class dbrl_compute_amir_curvelets_process:public bpro1_process
    {
    public:

        dbrl_compute_amir_curvelets_process();
        ~dbrl_compute_amir_curvelets_process();

        std::string name();

        //: Clone the process
        virtual bpro1_process* clone() const;

        int input_frames();
        int output_frames();

        std::vector< std::string > get_input_type();
        std::vector< std::string > get_output_type();

        bool execute();
        bool finish();


        bool compute_curvelets_from_edges(std::vector<vsol_line_2d_sptr> lines);
    private:


    };
#endif
