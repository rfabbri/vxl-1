#ifndef _dbrl_support_subpixel_process_
#define _dbrl_support_subpixel_process_

#include <bpro1/bpro1_process.h>
#include <georegister/dbrl_support.h>
#include <vil/vil_image_view.h>
#include <georegister/dbrl_id_point_2d_sptr.h>

class dbrl_support_subpixel_process:public bpro1_process
    {
    public:

        dbrl_support_subpixel_process();
        ~dbrl_support_subpixel_process();

        std::string name();

        //: Clone the process
        virtual bpro1_process* clone() const;

        int input_frames();
        int output_frames();

        std::vector< std::string > get_input_type();
        std::vector< std::string > get_output_type();

        bool execute();
        bool finish();

bool compute_temporal_support(double sigma_d, double sigma_a,double thresh,std::vector< std::vector< dbrl_id_point_2d_sptr > >pts);
        bool compute_spatial_support(double sigma_d,double thresh,std::vector< std::vector< dbrl_id_point_2d_sptr > > pts);


    private:

        std::vector<vil_image_view<unsigned char> >  image_list_;
        std::vector<std::vector<dbrl_id_point_2d_sptr> > points_list_;
        std::vector<int> framenums_;

    };
#endif
