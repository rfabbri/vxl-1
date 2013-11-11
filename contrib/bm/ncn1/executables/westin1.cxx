//this is contrib/bm/ncn1/executables/westin1.cxx

#include"../ncn1_factory.h"

#include"..\ncn1_utilities.h"

#include"dbcl_classifier_factory.h"

#include<vil/vil_convert.h>

#include<vil/vil_save.h>

int main(int argc, char *argv[])
{
    //user should provide video glob and results directory respectively

    if(argc < 2)
    {
        vcl_cerr << "error westin1.exe::must provide input glob and output directory" << vcl_endl;
        return 1;
    }

    vcl_string video_glob = argv[1];

    vcl_string result_dir = argv[2];

    vcl_string entropy_bin_filename = result_dir + "\\entropy_cropped_view.bin";

    vcl_string entropy_matlab_filename = result_dir + "\\entropy_cropped_view.dat";

    vcl_string resave_entropy_bin = result_dir + "\\entropy_resave.bin";

    vcl_string resave_entropy_matlab = result_dir + "\\entropy_resave.dat";

    vcl_string pivot_pixel_candidate_filename = result_dir + "\\pivot_pixel_samples.dat";

    vcl_string neighborhood_mfilename = result_dir + "\\neighborhood.m";

    vcl_string feature_mfilename = result_dir + "\\features_mfile.m";

    vcl_string reduced_feature_filename = result_dir + "\\reduced_features_mfile.m";

    ncn1_factory factory(video_glob,100,200,10);

    //factory.calculate_temporal_entropy();

    //factory.save_binary_temporal_entropy(entropy_bin_filename);

    //factory.save_matlab_temporal_entropy(entropy_matlab_filename);

    factory.load_binary_temporal_entropy(entropy_bin_filename);

    //vil_image_view<double> temporal_entropy = factory.temporal_entropy();

    //factory.save_binary_temporal_entropy(resave_entropy_bin);

    //factory.save_matlab_temporal_entropy(resave_entropy_matlab);

    pivot_pixel_list_type pivot_pixel_candidates = factory.sample_pivot_pixel_candidates();

    factory.set_pivot_pixel_candidates(pivot_pixel_candidates);

    ncn1_utilities::save_pivot_pixel_candidate_matlab(pivot_pixel_candidate_filename,pivot_pixel_candidates);

    //target pixels
    vgl_point_2d<unsigned> rp1,rp2,wp1,hp1,hp2;

    rp1.set(20,38);
    rp2.set(120,16);
    wp1.set(76,12);
    hp1.set(88,32);
    hp2.set(95,23);

    target_pixel_list_type target_pixel_list;

    target_pixel_list.push_back(rp1);
    target_pixel_list.push_back(rp2);
    target_pixel_list.push_back(wp1);
    target_pixel_list.push_back(hp1);
    target_pixel_list.push_back(hp2);

    factory.set_target_pixel_list(target_pixel_list);
   
    factory.build_neighborhood();

    factory.extract_features();

    ncn1_utilities::save_neighborhood_dat(neighborhood_mfilename,factory.neighborhood_sptr());

    //ncn1_utilities::save_feature_dat(feature_mfilename,factory.neighborhood_sptr());
    factory.save_feature_dat(feature_mfilename);

    ncn1_feature_sptr feature_sptr = factory.feature_sptr();

    feature_sptr->reduce_features_2d();

    feature_sptr->save_2d_pts_dat( reduced_feature_filename );

    feature_typedefs::frame_reduced_feature_map_2d_type frame_feature_map_2d = feature_sptr->feature_map_2d();


    return 0;
}