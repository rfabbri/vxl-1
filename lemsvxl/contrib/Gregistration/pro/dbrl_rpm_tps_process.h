#ifndef _dbrl_rpm_tps_process_
#define _dbrl_rpm_tps_process_

#include <bpro1/bpro1_process.h>

class dbrl_rpm_tps_process:public bpro1_process
    {
    public:

        dbrl_rpm_tps_process();
        ~dbrl_rpm_tps_process();

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
