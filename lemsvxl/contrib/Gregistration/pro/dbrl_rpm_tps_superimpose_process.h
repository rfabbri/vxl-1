#ifndef _dbrl_rpm_tps_superimpose_process_
#define _dbrl_rpm_tps_superimpose_process_

#include <bpro1/bpro1_process.h>
#include <georegister/dbrl_id_point_2d_sptr.h>
#include <georegister/dbrl_feature_sptr.h>

class dbrl_rpm_tps_superimpose_process:public bpro1_process
    {
    public:

        dbrl_rpm_tps_superimpose_process();
        ~dbrl_rpm_tps_superimpose_process();

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
      std::vector<std::vector<dbrl_id_point_2d_sptr> >   id_point_set_list_;
      std::vector<std::vector<dbrl_feature_sptr> >   point_set_list_;

    };
#endif
