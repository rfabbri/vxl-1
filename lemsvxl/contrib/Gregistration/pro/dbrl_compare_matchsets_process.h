#ifndef _dbrl_compare_matchsets_process_
#define _dbrl_compare_matchsets_process_

#include <bpro1/bpro1_process.h>

class dbrl_compare_matchsets_process:public bpro1_process
    {
    public:

        dbrl_compare_matchsets_process();
        ~dbrl_compare_matchsets_process();

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
      
    };
#endif
