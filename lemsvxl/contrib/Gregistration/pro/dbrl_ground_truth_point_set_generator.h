#ifndef _dbrl_ground_truth_point_set_generator_h_
#define _dbrl_ground_truth_point_set_generator_h_

#include <bpro1/bpro1_process.h>

class dbrl_ground_truth_point_set_generator:public bpro1_process
    {
    public:

        dbrl_ground_truth_point_set_generator();
        ~dbrl_ground_truth_point_set_generator();

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
        
        float tx;
        float ty;

         float scale;      
    };
#endif
