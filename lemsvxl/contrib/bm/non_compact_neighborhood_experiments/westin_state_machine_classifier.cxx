//this is /contrib/bm/non_compact_neighborhood_experiments/westin_state_machine_classifier.cxx

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
	vnl_matrix_fixed<double,2,2> min_covar;
	init_covar(0,0) = 800;
	init_covar(0,1) = 0;
	init_covar(1,0) = 0;
	init_covar(1,1) = 800;

	min_covar(0,0) = 50;
	min_covar(0,1) = 0;
	min_covar(1,0) = 0;
	min_covar(1,1) = 50;




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

	vcl_vector<double> target_coord;
	target_coord.push_back(345);
	target_coord.push_back(411);

	vcl_stringstream ss;
	ss << argv[2] << "\\target_" << target_coord[0] << "_" << target_coord[1] << "_" << "num_pivot_" << num_pivot_pixels << "_num_particles_" << num_particles;
	ss << "_num_neighbors_" << num_neighbors << "_t_forget_" << t_forget << "_mahalan_dist_factor_" << mahalan_dist_factor;

		vul_file::make_directory(ss.str().c_str());

	vcl_string result_dir = ss.str();

	vcl_string base_result_dir = argv[2];

    vcl_string entropy_bin = base_result_dir + "\\westin_entropy.bin";

	vcl_string entropy_dat = result_dir + "\\westin_entropy.dat";

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

	//for( unsigned x = 18; x < 608; ++x )
 //   {
 //       for( unsigned y = 162; y < 697; ++y )
 //       {
 //           dncn_image_point_2d target;
 //           target.img_point.set(x,y);
 //           pv.push_back(target);
 //       }
 //   }

	//for( unsigned x = 787; x < 800; ++x )
	//	for( unsigned  y = 430; y < 439; ++y )
	//	{
	//		dncn_image_point_2d target;
	//		target.img_point.set(x,y);
	//		pv.push_back(target);
	//	}

	dncn_image_point_2d target;
	target.img_point.set(target_coord[0],target_coord[1]);
	pv.push_back(target);


	dncn_factory factory( video_glob, pv, num_pivot_pixels, num_particles, num_neighbors );

	factory.load_entropy_bin(entropy_bin);
	
	//factory.save_entropy_dat(entropy_dat);

	//factory.build_no_entropy_sift();

	factory.build_no_entropy();
		
	

	//factory.write_neighborhood_mfile(neighborhood_mfile);
	
	//factory.save_pivot_pixels_mfile(pivot_pixels_mfile);

	//factory.write_reduced_feature_mfile( reduced_feature_mfile );



	dbcl_state_machine_image_classifier image_classifier(factory.target_list_2d_sptr(),t_forget,prob_thresh,mahalan_dist_factor,init_covar,min_covar);

	image_classifier.classify();

    //output changemaps
    //dbcl_state_machine_view::save_change_maps(result_dir,video_glob,image_classifier);

	//dbcl_state_machine_view::save_target_temporal_prob_dist( probability_distribution_mfile,image_classifier.classifier_sptr(pv[0].img_point.x(),pv[0].img_point.y()) );

	return 0;
}