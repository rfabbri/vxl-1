//this is /contrib/bm/non_compact_neighborhood_experiments/state_machine_classifier_executable.cxx

#include"dbcl_classifier_factory.h"
#include"dbcl_state_machine_view.h"

#include"dncn_factory.h"

int main( int argc, char* argv[] )
{
        //user should provide video glob and results directory respectively
    if( argc < 2)
    {
        vcl_cerr << "error dncn_westin_cropped::must provide input glob and output directory" << vcl_endl;
        return 1;
    }

	unsigned num_pivot_pixels = 1000;
	unsigned num_particles = 5000;
	unsigned num_neighbors = 10;
	unsigned t_forget = 200;
	double prob_thresh = .01;
	double mahalan_dist_factor;
	vnl_matrix_fixed<double,2,2> init_covar;

	init_covar(0,0) = 800;
	init_covar(0,1) = 0;
	init_covar(1,0) = 0;
	init_covar(1,1) = 800;

	if( argc > 3)
		mahalan_dist_factor = vcl_atof(argv[3]);		
	else
		mahalan_dist_factor = 1;

	if( argc > 4 )
		t_forget = vcl_atof(argv[4]);
	else
		t_forget = 200;

    vcl_string video_glob = argv[1];

    

    vidl_image_list_istream video_stream_temp;



	vcl_stringstream ss;
	ss << argv[2] << "\\num_pivot_" << num_pivot_pixels << "_num_particles_" << num_particles;
	ss << "_num_neighbors_" << num_neighbors << "_t_forget_" << t_forget << "_mahalan_dist_factor_" << mahalan_dist_factor;

	vul_file::make_directory(ss.str().c_str());

	vcl_string result_dir = ss.str();

	vcl_string base_result_dir = argv[2];

    vcl_string entropy_bin = base_result_dir + "\\entropy.bin";

	vcl_string entropy_dat = result_dir + "\\entropy.dat";

	//vcl_string factory_bin = result_dir + "\\factory_bin.bin";

	vcl_string neighborhood_mfile = result_dir + "\\neighborhoods_mfile.m";

	vcl_string pivot_pixels_mfile = result_dir + "\\pivot_pixels_mfile.m";

	vcl_string probability_distribution_mfile = result_dir + "\\probability_distribution_mfile.m";

	vcl_string reduced_feature_mfile = result_dir + "\\reduced_feature_mfile.m";

    video_stream_temp.open(video_glob.c_str());
    unsigned nrows = video_stream_temp.height();
    unsigned ncols = video_stream_temp.width();
    video_stream_temp.close();

    dncn_factory::point_vector_type pv;

    //for( unsigned x = 160; x < 183; ++x )
    //{
    //    for( unsigned y = 157; y < 173; ++y )
    //    {
    //        dncn_image_point_2d target;
    //        target.img_point.set(x,y);
    //        pv.push_back(target);
    //    }
    //}

	dncn_image_point_2d target;
	target.img_point.set(160,157);
	pv.push_back(target);
   

    //factory.save_entropy_bin(entropy_bin);
    //factory.load_entropy_bin(entropy_bin);

    //build the non-compact neighborhood
 //   factory.build();

	//factory.save_entropy_bin(entropy_bin);

	dncn_factory factory( video_glob, pv, num_pivot_pixels, num_particles, num_neighbors );

	factory.load_entropy_bin(entropy_bin);

	factory.save_entropy_dat(entropy_dat);

	factory.build_no_entropy();

	factory.write_neighborhood_mfile(neighborhood_mfile);
	
	factory.save_pivot_pixels_mfile(pivot_pixels_mfile);

	factory.write_reduced_feature_mfile( reduced_feature_mfile );

	//factory.save_factory_bin(factory_bin);

	factory.extract_sift_features(1,4);

	//dncn_factory factory;
	//factory.read_factory_bin(factory_bin);

    //classify
    //dbcl_image_classifier image_classifier(TEMPORAL_STATE_MACHINE,factory.target_list_2d_sptr());

	dbcl_state_machine_image_classifier image_classifier(factory.target_list_2d_sptr(),t_forget,prob_thresh,mahalan_dist_factor,init_covar);

	image_classifier.classify();

    //output changemaps
    dbcl_state_machine_view::save_change_maps(result_dir,video_glob,image_classifier);

	dbcl_state_machine_view::save_target_temporal_prob_dist( probability_distribution_mfile,image_classifier.classifier_sptr(160,157) );



    return 0;
}//end main