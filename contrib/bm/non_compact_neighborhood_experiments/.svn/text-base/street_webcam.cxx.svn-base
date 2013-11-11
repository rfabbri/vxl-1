//this is /contrib/bm/non_compact_neighborhood_experiments/street_webcam.cxx

#include"dbcl_classifier_factory.h"

#include"dncn_factory.h"

int main(int argc, char* argv[])
{
        //user should provide video glob and results directory respectively
    if( argc < 2)
    {
        vcl_cerr << "error dncn_westin_cropped::must provide input glob and output directory" << vcl_endl;
        return 1;
    }
    vcl_string video_glob = argv[1];

    vcl_string result_dir = argv[2];

    vcl_string entropy_dat = result_dir +" \\entropy_dat.dat";

    vcl_string entropy_bin = result_dir + "\\entropy.bin";

    vcl_string reduced_feature_mfile = result_dir + "\\reduced_features_mfile.m";

    vcl_string neighborhood_mfile = result_dir + "\\neighborhood_mfile.m";

    dncn_image_point_2d target1;

    target1.img_point.set(178,167);


    dncn_factory::point_vector_type pv;
    
    pv.push_back(target1);

    dncn_factory factory(video_glob,pv);



    return 0;
}