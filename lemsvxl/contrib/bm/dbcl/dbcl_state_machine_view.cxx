//this is /contrib/bm/dbcl/dbcl_state_machine_view.cxx

#include"dbcl_state_machine_view.h"

void dbcl_state_machine_view::save_change_maps( vcl_string result_dir, dbcl_image_classifier& image_classifier )
{
    vcl_cout << "Saving change maps..." << vcl_endl;

    target_classifier_map_type target_classifier_map = image_classifier.target_classifier_map();

    unsigned ncols = image_classifier.cols();
    unsigned nrows = image_classifier.rows();
    unsigned nframes = image_classifier.nframes();

    target_classifier_map_type::const_iterator target_map_itr;
    target_classifier_map_type::const_iterator target_map_end = target_classifier_map.end();

	vcl_string change_map_delete_glob = result_dir + "\\*.jpg";
	vul_file::delete_file_glob(change_map_delete_glob);

    vcl_string base_filename = result_dir + "\\change_map_";
    
    for( unsigned t = 0; t < nframes; ++t )
    {
        vcl_cout << '\t' << t/double(nframes)*100 << "% complete..." << vcl_endl;
        vcl_stringstream ss;
        ss << base_filename << vcl_setfill('0') << vcl_setw(5) << t << ".jpg";

        vil_image_view<vxl_byte> curr_change_map(ncols,nrows,1);

        for( unsigned x = 0; x < ncols; ++x )
        {
            for( unsigned y = 0; y < nrows; ++y )
            {
                
                vgl_point_2d<unsigned> query_point(x,y);
                target_map_itr = target_classifier_map.find(query_point);
                unsigned label = 0;
                if( target_map_itr != target_map_end )
                {
                    dbcl_classifier_sptr classifier_sptr = target_map_itr->second;
                    dbcl_state_machine_classifier_sptr state_machine_classifier_sptr = dynamic_cast<dbcl_state_machine_classifier*>( classifier_sptr.as_pointer() );
                    vcl_map<unsigned,bool> frame_change_map = state_machine_classifier_sptr->frame_change_map();
                    if( frame_change_map[t] == 1 )
                        label = 255;
                    else
                        label = 0;
                }
                curr_change_map(x,y,0) = label;
            }
        }//end pixel iteration

        vcl_string curr_filename = ss.str();

        vil_save(curr_change_map,curr_filename.c_str());
        
    }//end frame iteration

    
    

}//end dbcl_state_machine_view::dbcl_state_machine_view

void dbcl_state_machine_view::save_change_maps( vcl_string result_dir, vcl_string video_glob, dbcl_image_classifier& image_classifier )
{
	vidl_image_list_istream video_stream(video_glob);

	vcl_cout << "Saving change maps..." << vcl_endl;

	target_classifier_map_type target_classifier_map = image_classifier.target_classifier_map();

	unsigned ncols = image_classifier.cols();
	unsigned nrows = image_classifier.rows();
	unsigned nframes = image_classifier.nframes();

	vcl_map<unsigned, vil_image_view<vxl_byte> > change_map_seq;

		for( unsigned t = 0; t < nframes; ++t )
		{
			vil_image_view<vxl_byte> grey_img,curr_img;
			
			video_stream.seek_frame(t);
			vidl_convert_to_view(*video_stream.current_frame(),curr_img);
			vil_convert_planes_to_grey(curr_img,grey_img);
			change_map_seq[t] = grey_img;

		}//end frame iteration

		target_classifier_map_type::iterator classifier_itr;
		target_classifier_map_type::iterator classifier_end = target_classifier_map.end();

		vcl_string coord_dat = result_dir + "\\coors.dat";
		unsigned nclassifiers = target_classifier_map.size();
		vcl_ofstream of(coord_dat.c_str(),vcl_ios::out);
		of << "x = zeros(" << nclassifiers << ",1);\n";
		of << "y = zeros(" << nclassifiers << ",2);\n";


		unsigned indx = 1;
		for( classifier_itr = target_classifier_map.begin(); classifier_itr != classifier_end; ++classifier_itr, ++indx )
		{
			dbcl_state_machine_classifier_sptr curr_classifier = dynamic_cast<dbcl_state_machine_classifier*>(classifier_itr->second.as_pointer());
			vcl_map<unsigned, bool> frame_change_map = curr_classifier->frame_change_map();
			vgl_point_2d<unsigned> target = classifier_itr->first;

			of << "x(" << indx << ") = " << target.x() << ";\n";
			of << "y(" << indx << ") = " << target.y() << ";\n";

			for( unsigned t = 0; t < nframes; ++ t )
			{
				unsigned label = 0;
				if( frame_change_map[t] == 1 )
					label = 255;

				change_map_seq[t](target.x(),target.y(),0) = label;
				
			}
		}

		vcl_string base_filename = result_dir + "\\change_map_";

		for( unsigned t = 0; t < nframes; ++t )
		{
			vcl_stringstream ss;
			ss << base_filename << vcl_setfill('0') << vcl_setw(5) << t << ".jpg";
			vcl_string curr_filename = ss.str();
			vil_save(change_map_seq[t],curr_filename.c_str());

		}
	

}//end dbcl_state_machine_view::save_change_maps


void dbcl_state_machine_view::save_change_maps(vcl_string result_dir, vcl_string video_glob, dbcl_state_machine_image_classifier& image_classifier)
{
	vidl_image_list_istream video_stream(video_glob);

	vcl_cout << "Saving change maps..." << vcl_endl;

	dbcl_state_machine_image_classifier::classifier_map_type classifier_map = image_classifier.classifier_map();

	unsigned ncols = image_classifier.ncols();
	unsigned nrows = image_classifier.nrows();
	unsigned nframes = image_classifier.nframes();

	vcl_map<unsigned, vil_image_view<vxl_byte> > change_map_seq;

	for( unsigned t = 0; t < nframes; ++t)
	{
		vil_image_view<vxl_byte> grey_img, curr_img;

		video_stream.seek_frame(t);
		vidl_convert_to_view(*video_stream.current_frame(),curr_img);
		vil_convert_planes_to_grey(curr_img,grey_img);
		change_map_seq[t] = grey_img;
	}//end frame iteration

	dbcl_state_machine_image_classifier::classifier_map_type::iterator classifier_itr;
	dbcl_state_machine_image_classifier::classifier_map_type::iterator classifier_end = classifier_map.end();

	for( classifier_itr = classifier_map.begin(); classifier_itr != classifier_end; ++classifier_itr )
	{
		unsigned indx = classifier_itr->first;
		dbcl_state_machine_classifier_sptr curr_classifier_sptr = classifier_itr->second;
		vcl_map<unsigned,bool> frame_change_map = curr_classifier_sptr->frame_change_map();
		vgl_point_2d<unsigned> target = image_classifier.linear_to_cartesian(indx);
		
		for( unsigned t = 0; t < nframes; ++t )
		{
			unsigned label = 0;
			if( frame_change_map[t] == 1 )
				label = 255;
			change_map_seq[t](target.x(),target.y(),0) = label;
			
		}//end frame iteration
	}//end classifier map iteration



	vcl_string base_filename = result_dir + "\\change_maps";
	//vcl_stringstream s1;
	//s1 << base_filename << image_classifier.prob_thresh();
	//vcl_string new_directory = s1.str();
	vul_file::make_directory(base_filename.c_str());
	

	for( unsigned t = 0; t < nframes; ++t )
	{
		vcl_stringstream ss;
		ss << base_filename << "\\" << "frame_" << vcl_setfill('0') << vcl_setw(5) << t << ".jpg";
		vcl_string curr_filename = ss.str();
		vil_save(change_map_seq[t],curr_filename.c_str());
	}//end frame iteration

}//end dbcl_state_machine_view::save_change_maps

void dbcl_state_machine_view::save_target_temporal_prob_dist( vcl_string result_dir, dbcl_state_machine_classifier_sptr& classifier_sptr )
{

	vcl_cout << "Saving Probability Distribution Mfile..." << vcl_endl;

	frame_mean_map_type frame_mean_map = classifier_sptr->frame_mean_map();
	frame_covar_map_type frame_covar_map = classifier_sptr->frame_covar_map();
	//frame_transition_table_map_type frame_transition_table = classifier_sptr->frame_transition_table_map();
	frame_mixture_weight_map_type frame_mixture_weight_map = classifier_sptr->frame_mixture_weight_map();
	feature_map_type test_feature_map = classifier_sptr->test_data();
	feature_map_type training_feature_map = classifier_sptr->training_data();
	frame_state_map_type frame_test_state_map = classifier_sptr->frame_test_state_map();
	frame_state_map_type frame_training_state_map = classifier_sptr->frame_training_state_map();



	unsigned nframes = classifier_sptr->nframes();

	feature_map_type::iterator test_feature_itr;
	feature_map_type::iterator test_feature_end = test_feature_map.end();
	unsigned ndims = test_feature_map.begin()->second->feature_dim();

	vcl_ofstream of( result_dir.c_str(), vcl_ios::out );

	time_t rawtime;
	struct tm * timeinfo;
	time( &rawtime );
	timeinfo = localtime(&rawtime);

	of << "%Experiment Run at " << asctime(timeinfo) << "\n"
	   << "clear all\n"
	   << "close all\n"
	   << "mean_cell = cell(" << nframes << ",2);\n"
	   << "covar_cell = cell(" << nframes << ",2);\n"
	   << "mixture_weight_cell = cell(" << nframes << ",1);\n";

	of << "test_features = [";
	for( test_feature_itr = test_feature_map.begin(); test_feature_itr != test_feature_end; ++test_feature_itr )
	{
		dbcl_temporal_feature_sptr curr_test_feature_sptr = test_feature_itr->second;
		vnl_vector<double> curr_vector = curr_test_feature_sptr->feature_vector();
		vnl_vector<double>::iterator curr_vect_itr;
		vnl_vector<double>::iterator curr_vect_end = curr_vector.end();
		for( curr_vect_itr = curr_vector.begin(); curr_vect_itr != curr_vect_end; ++curr_vect_itr )
			of << *curr_vect_itr << ' ';
		of << ";\n";
	}//end test feature iteration
	of << "];\n\n";

	feature_map_type::iterator training_feature_itr;
	feature_map_type::iterator training_feature_end = training_feature_map.end();

	of << "training_features = [";
	for( training_feature_itr = training_feature_map.begin(); training_feature_itr != training_feature_end; ++training_feature_itr )
	{
		dbcl_temporal_feature_sptr  curr_training_feature_sptr = training_feature_itr->second;
		vnl_vector<double> curr_vector = curr_training_feature_sptr->feature_vector();
		vnl_vector<double>::iterator curr_vect_itr;
		vnl_vector<double>::iterator curr_vect_end = curr_vector.end();
		for( curr_vect_itr = curr_vector.begin(); curr_vect_itr != curr_vect_end; ++curr_vect_itr )
			of << *curr_vect_itr << ' ';
		of << ";\n";
	}//end training feature iteration
	of << "];\n\n";

	frame_state_map_type::iterator test_state_map_itr;
	frame_state_map_type::iterator test_state_map_end = frame_test_state_map.end();
	of << "% The first column is the frame number, the second column is the state\n";
	of << "test_state_map = [";
	for( test_state_map_itr = frame_test_state_map.begin(); test_state_map_itr != test_state_map_end; ++test_state_map_itr )
	{
		of << test_state_map_itr->first << ' ' << test_state_map_itr->second << ";\n";
	}//end frae test state map iteration
	of << "];\n\n";

	frame_state_map_type::iterator training_state_map_itr;
	frame_state_map_type::iterator training_state_map_end = frame_training_state_map.end();
	of << "% The first column is the frame number, the second column is the state\n";
	of << "training_state_map = [";
	for( training_state_map_itr = frame_training_state_map.begin(); training_state_map_itr != training_state_map_end; ++training_state_map_itr )
	{
		of << training_state_map_itr->first << ' ' << training_state_map_itr->second << ";\n";
	}//end training state map iteration
	of << "];\n\n";


	for( unsigned t = 0; t < nframes; ++t )
	{
		//matlab indexing starts from 1.
		unsigned matlab_frame_indx = t + 1; 

		//write the means
		node_id_mean_map_type::iterator node_id_mean_itr;
		node_id_mean_map_type::iterator node_id_mean_end = frame_mean_map[t].end();
		of << "mean_cell{" << matlab_frame_indx << ",1} = [";
		for( node_id_mean_itr = frame_mean_map[t].begin(); node_id_mean_itr != node_id_mean_end; ++node_id_mean_itr )
		{
			of << node_id_mean_itr->first << ' ';
		}//end node id / mean vector iteration
		of << "];\n";

		of << "mean_cell{" << matlab_frame_indx << ",2} = [";
		for(node_id_mean_itr = frame_mean_map[t].begin(); node_id_mean_itr != node_id_mean_end; ++node_id_mean_itr )
		{
			vnl_vector<double> curr_mean_vector = node_id_mean_itr->second;
			for( unsigned i = 0; i < curr_mean_vector.size(); ++i )
				of << curr_mean_vector[i] << ' ';

			of << ";\n";
		}//end node id / mean vector iteration

		of << "];\n";

		//write the covariances
		of << "covar_cell{" << matlab_frame_indx << ",1} = [";
		node_id_covar_map_type::iterator node_id_covar_itr;
		node_id_covar_map_type::iterator node_id_covar_end = frame_covar_map[t].end();

		unsigned num_nodes = 0;
		unsigned nrows = frame_covar_map[t].begin()->second.rows();
		unsigned ncols = frame_covar_map[t].begin()->second.cols();
		for( node_id_covar_itr = frame_covar_map[t].begin(); node_id_covar_itr != node_id_covar_end; ++node_id_covar_itr, ++num_nodes )
		{
			of << node_id_covar_itr->first << ' ';
		}//end node id / covar iteration
		of << "];\n";

		of << "covar_cell{" << matlab_frame_indx << ",2} = zeros(" << nrows << ',' << ncols << ',' << num_nodes << ");\n";
		unsigned node_indx = 1;
		for( node_id_covar_itr = frame_covar_map[t].begin(); node_id_covar_itr != node_id_covar_end; ++node_id_covar_itr, ++node_indx )		
		{
			vnl_matrix<double> curr_covar_matrix = node_id_covar_itr->second;
			for( unsigned row = 0; row < curr_covar_matrix.rows(); ++row )
			{
				for(unsigned col = 0; col < curr_covar_matrix.cols(); ++col )
				{
					of << "covar_cell{" << matlab_frame_indx << ",2}(" << row+1 << ',' << col+1 << ',' << node_indx <<") = " << curr_covar_matrix(row,col) << ";\n";
				}
			}//end matrix iteration
		}//end node id /covar iteration

		//write mixture components
		node_id_mixture_weight_map_type::iterator node_id_mixture_weight_itr;
		node_id_mixture_weight_map_type::iterator node_id_mixture_weight_end = frame_mixture_weight_map[t].end();

		of << "mixture_weight_cell{" << matlab_frame_indx << ",1} = [";
		for( node_id_mixture_weight_itr = frame_mixture_weight_map[t].begin(); node_id_mixture_weight_itr != node_id_mixture_weight_end; ++node_id_mixture_weight_itr )
		{
			of << node_id_mixture_weight_itr->first << ' ';
		}//end node_id_mixture_weight iteration

		of << "];\n";

		of << "mixture_weight_cell{ " << matlab_frame_indx << ",2} = [";
		for( node_id_mixture_weight_itr = frame_mixture_weight_map[t].begin(); node_id_mixture_weight_itr != node_id_mixture_weight_end; ++node_id_mixture_weight_itr )
		{
			of << node_id_mixture_weight_itr->second << ' ';
		}//end node_id_mixture_weight iteration
		of << "];\n";

	}//end frame iteration
	
	of << "\n\nmax_training = max(training_features);\n"
	   << "min_training = min(training_features);\n"
	   << "max_test = max(test_features);\n"
	   << "min_test = min(test_features);\n"
	   << "x_y_max = max([max_training; max_test]);\n"
	   << "x_y_min = min([min_training; min_test]);\n"
	   << "x_y_max = x_y_max + 10;\n"
	   << "x_y_min = x_y_min - 10;\n\n";

	of << "oldFolder = pwd;\n"
	   << "if(~isdir(\'prob_dist\'))\n"
	   << "\t mkdir(\'prob_dist\')\n"
	   << "end\n"
	   << "cd(\'prob_dist\');\n"
	   << "delete(\'mixture_frame_*.png\')\n"
	   << "for i = 1:"<<nframes<<",\n"
	   << "		mu = mean_cell{i,2};\n"
	   << "		sigma = covar_cell{i,2};\n"
	   << "		p = mixture_weight_cell{i,2};\n"
	   << "		obj = gmdistribution(mu,sigma,p);\n"
	   << "		h = figure;\n"
	   << "		ezsurf(@(x,y)pdf(obj,[x y]),[x_y_min(1) x_y_max(1)],[x_y_min(1) x_y_max(2)]);\n"
	   << "		title([\'Frame\' num2str(i)]);\n"
	   << "		frame_number = sprintf('%04d',i);\n"
	   << "		filename = ['mixture_' frame_number '.png'];\n"
	   << "		view(-16,44);\n"
	   << "		print(h,\'-dpng\',filename);\n"
	   << "		close(h)\n"
	   << "end\n"
	   << "cd(oldFolder)\n\n";

	of << "if(~isdir(\'train_contour\'))\n"
	   << "		mkdir(\'train_contour\')\n"
	   << "end\n"
	   << "cd(\'train_contour\')\n"
	   << "delete('train_contour_*.png')\n"
	   << "for i = 1:" << nframes << "\n"
	   << "		mu = mean_cell{i,2};\n"
	   << "		sigma = covar_cell{i,2};\n"
	   << "		p = mixture_weight_cell{i,2};\n"
	   << "		obj = gmdistribution(mu,sigma,p);\n"
	   << "		h = figure;\n"
	   << "		ezcontour(@(x,y)pdf(obj,[x y]),[x_y_min(1) x_y_max(1)],[x_y_min(2) x_y_max(2)],200);\n"
	   << "		hold on, plot(training_features(1:i,1),training_features(1:i,2),\'kx\')\n"
	   << "		hold on, plot(training_features(i,1),training_features(i,2),\'rx\')\n"
	   << "		xlabel('PCA 1')\n"
	   << "     ylabel('PCA 2')\n"
	   << "		title([\'Frame \' num2str(i)])\n"
	   << "		frame_number = sprintf('%04d',i);\n"
	   << "		filename = [\'train_\' frame_number \'.png\'];\n"
	   << "		print(h,\'-dpng\',filename);\n"
	   << "		close(h);\n"
	   << "end\n"
	   << "cd(oldFolder)\n\n";

    of << "if(~isdir(\'test_contour\'))\n"
	   << "		mkdir(\'test_contour\')\n"
	   << "end\n"
	   << "cd(\'test_contour\')\n"
	   << "delete('test_*.png')\n"
	   << "figure,stairs(1:length(test_state_map),test_state_map(:,2),'k-');\n"
	   << "for i = 1:" << nframes << "\n"
	   << "		mu = mean_cell{i,2};\n"
	   << "		sigma = covar_cell{i,2};\n"
	   << "		p = mixture_weight_cell{i,2};\n"
	   << "		obj = gmdistribution(mu,sigma,p);\n"
	   << "		h = figure;\n"
	   << "		ezcontour(@(x,y)pdf(obj,[x y]),[x_y_min(1) x_y_max(1)],[x_y_min(2) x_y_max(2)],200);\n"
	   << "		hold on, plot(test_features(1:i,1),test_features(1:i,2),\'kx\')\n"
	   << "		hold on, plot(test_features(i,1),test_features(i,2),\'rx\')\n"
	   << "		xlabel('PCA 1')\n"
	   << "		ylabel('PCA 2')\n"
	   << "		title([\'Frame \' num2str(i)])\n"
	   << "		frame_number = sprintf('%04d',i);\n"
	   << "		filename = [\'test_\' frame_number \'.png\'];\n"
	   << "		print(h,\'-dpng\',filename);\n"
	   << "		close(h);\n"
	   << "end\n"
	   << "cd(oldFolder)";
	
	of.close();
	
}//end dbcl_state_machine_view::save_target_temporal_prob_dist