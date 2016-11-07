//this is contrib/bm/non_compact_neighborhood_experiments/westin_classification1.cxx

#include"dbcl_image_classifier.h"

#include"dncn_factory.h"


int main(int argc, char* argv[] )
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

    vcl_cout << argv[1] << vcl_endl;
    vcl_cout << argv[2] << vcl_endl;
    
    dncn_image_point_2d target1;

    target1.img_point.set(383,456);
    
    for(unsigned i = 0; i < 7; ++i)
        target1.frame_label[i] = 0;
    
    //first car
    target1.frame_label[9] = 1;
    target1.frame_label[10] = 1;
    target1.frame_label[11] = 1;

    //second car
    target1.frame_label[27] = 2;
    target1.frame_label[26] = 2;
    target1.frame_label[25] = 2;

    dncn_factory::point_vector_type pv;

    pv.push_back(target1);

    dncn_factory factory(video_glob,pv);

    factory.load_entropy_bin(entropy_bin);

    factory.build_no_entropy();

    factory.write_neighborhood_mfile( neighborhood_mfile );
    
    dbcl_image_classifier classifier(LINEAR_CLASSIFIER,factory.target_list_2d_sptr());

    classifier.classify();

    classifier.draw_classifiers_vrml( result_dir );

    return 0;
}