//this is /contrib/bm/dbcl/dbcl_state_machine_image_classifier.cxx

#include"dbcl_state_machine_image_classifier.h"

void dbcl_state_machine_image_classifier::linear_to_cartesian( unsigned const& linear_indx, unsigned& x, unsigned& y )
{ 
	x = linear_indx / nrows_; 
	y = linear_indx % nrows_; 
}//end dbcl_state_machine_image_classifier::linear_to_cartesian

vgl_point_2d<unsigned> dbcl_state_machine_image_classifier::linear_to_cartesian( unsigned const& indx )
{
	unsigned x,y;
	this->linear_to_cartesian(indx,x,y);
	return vgl_point_2d<unsigned>(x,y);
}//end dbcl_state_machine_image_classifier::linear_to_cartesian

unsigned dbcl_state_machine_image_classifier::cartesian_to_linear( vgl_point_2d<unsigned> const& point )
{
	return this->cartesian_to_linear(point.x(),point.y());
}//end dbcl_state_machine_image_classifier::cartesian_to_linear


unsigned dbcl_state_machine_image_classifier::cartesian_to_linear( unsigned const& x, unsigned const& y)
{
	return x*this->nrows_+y;
}//end dbcl_state_machine_image_classifier::cartesian_to_linear

dbcl_state_machine_classifier_sptr dbcl_state_machine_image_classifier::classifier_sptr(unsigned const& x, unsigned const& y)
{
	return this->classifier_map_[this->cartesian_to_linear(x,y)];
}//end dbcl_state_machine_classifier::classifier_sptr



dbcl_state_machine_image_classifier::dbcl_state_machine_image_classifier( dncn_target_list_2d_sptr target_list_sptr,
																		  unsigned const& t_forget,
																		  double const& prob_thresh,
																		  double const& mahalan_dist_factor,
																		  vnl_matrix_fixed<double,2,2> const& init_covar, 
																		  vnl_matrix_fixed<double,2,2> const& min_covar ):
t_forget_(t_forget),prob_thresh_(prob_thresh),mahalan_dist_factor_(mahalan_dist_factor),init_covar_(init_covar),min_covar_(min_covar)
{
	this->nrows_ = target_list_sptr->rows();
	this->ncols_ = target_list_sptr->cols();
	this->nframes_ = target_list_sptr->frames();

	dncn_target_list_2d::target_list_type target_list;

	dncn_target_list_2d::target_list_type::iterator target_list_itr;
	dncn_target_list_2d::target_list_type::iterator target_list_end = target_list_sptr->target_list_end();

	//iterating through pixels of interest
	for( target_list_itr = target_list_sptr->target_list_begin(); target_list_itr != target_list_end; ++target_list_itr )
	{
		dncn_target_2d_sptr curr_target_sptr = *target_list_itr;

		//set the test/training data
		dbcl_classifier::feature_map_type test_feature_map;
		dbcl_classifier::feature_map_type training_feature_map;

		//iterate through the features of the target (the frames in which the target is observed)
		dncn_target_2d::feature_map_type::iterator feature_itr;
		dncn_target_2d::feature_map_type::iterator feature_end = curr_target_sptr->feature_map_end();

		dncn_target_2d::feature_map_type::iterator target_feature_itr;
		dncn_target_2d::feature_map_type::iterator target_feature_end = curr_target_sptr->target_map_end();

		//vnl_vector<double> mean(2,0);
		//vnl_vector<double> sigma(2,0);
		//for( feature_itr = curr_target_sptr->feature_map_begin(); feature_itr != feature_end; ++feature_itr )
		//{
		//	vnl_vector<double> reduced_feature_vector = feature_itr->second->feature_vector_reduced();
		//	mean[0] += reduced_feature_vector.get(0);
		//	mean[1] += reduced_feature_vector.get(1);
		//}
		//
		//for( target_feature_itr = curr_target_sptr->target_map_begin(); target_feature_itr != target_feature_end; ++target_feature_itr )
		//{
		//	mean[0] += target_feature_itr->second->feature_vector_reduced().get(0);
		//	mean[1] += target_feature_itr->second->feature_vector_reduced().get(1);
		//}

		//double N = curr_target_sptr->feature_map().size() + curr_target_sptr->target_map().size();
		//mean[0] = mean[0] / N;
		//mean[1] = mean[1] / N;

		//for( feature_itr = curr_target_sptr->feature_map_begin(); feature_itr != feature_end; ++feature_itr )
		//{
		//	vnl_vector<double> reduced_feature_vector = feature_itr->second->feature_vector_reduced();
		//	sigma[0] = sigma[0] + vcl_pow( (reduced_feature_vector.get(0) - mean[0]) , 2);
		//	sigma[1] = sigma[1] + vcl_pow( (reduced_feature_vector.get(1) - mean[1]) , 2);
		//}

		//for( target_feature_itr = curr_target_sptr->target_map_begin(); target_feature_itr!=target_feature_end; ++target_feature_itr )
		//{
		//	vnl_vector<double> reduced_target_vector = target_feature_itr->second->feature_vector_reduced();
		//	sigma[0] = sigma[0] + vcl_pow( (reduced_target_vector.get(0) - mean[0]), 2);
		//	sigma[1] = sigma[1] + vcl_pow( (reduced_target_vector.get(1) - mean[1]), 2);
		//}

		

		for( feature_itr = curr_target_sptr->feature_map_begin(); feature_itr != feature_end; ++feature_itr)
		{
			vnl_vector<double> reduced_feature_vector = feature_itr->second->feature_vector_reduced();
			
			//reduced_feature_vector[0] = (reduced_feature_vector[0] - mean[0]) / sigma[0];
			//reduced_feature_vector[1] = (reduced_feature_vector[1] - mean[1]) / sigma[1];

			unsigned label = feature_itr->second->label();
			unsigned frame = feature_itr->second->frame();
			dbcl_temporal_feature_sptr feature_sptr = new dbcl_temporal_feature(frame,reduced_feature_vector,label);

			training_feature_map[frame] = feature_sptr;
			//if( target_feature_itr->second->is_gt() == true )
			//	training_feature_map[frame] = feature_sptr;
			//else
			//	test_feature_map[frame] = feature_sptr;
		}//end target/frame feature iteration

		

		for( target_feature_itr = curr_target_sptr->target_map_begin(); target_feature_itr != target_feature_end; ++target_feature_itr )
		{
			vnl_vector<double> reduced_target_vector = target_feature_itr->second->feature_vector_reduced();
			//reduced_target_vector[0] = ( reduced_target_vector[0] - mean[0] ) / sigma[0];
			//reduced_target_vector[1] = ( reduced_target_vector[1] - mean[1] ) / sigma[1];

			unsigned frame = target_feature_itr->second->frame();
			dbcl_temporal_feature_sptr feature_sptr = new dbcl_temporal_feature(frame,reduced_target_vector);
			test_feature_map[frame] = feature_sptr;
		}//end target/frame feature iteration
		

		dbcl_classifier_factory factory(TEMPORAL_STATE_MACHINE,training_feature_map,test_feature_map);
		dbcl_classifier_sptr classifier_sptr = factory.classifier();
		dbcl_state_machine_classifier_sptr state_machine_classifier_sptr = dynamic_cast<dbcl_state_machine_classifier*>(classifier_sptr.as_pointer());

		state_machine_classifier_sptr->set_t_forget(this->t_forget_);
		state_machine_classifier_sptr->set_prob_thresh(this->prob_thresh_);
		state_machine_classifier_sptr->set_init_covar(this->init_covar_);
		state_machine_classifier_sptr->set_min_covar(this->min_covar_);

		this->add_classifier(curr_target_sptr->target(),state_machine_classifier_sptr);
		
	}//end target_list iteration

}//end dbcl_state_machine_image_classifier::dbcl_state_machine_image_classifier

void dbcl_state_machine_image_classifier::add_classifier( vgl_point_2d<unsigned> target, dbcl_state_machine_classifier_sptr classifier_sptr )
{
	unsigned indx = this->cartesian_to_linear(target);
	this->classifier_map_[indx] = classifier_sptr;
}//end dbcl_state_machine_image_classifier::add_classifier

void dbcl_state_machine_image_classifier::classify()
{
	dbcl_state_machine_image_classifier::classifier_map_type::iterator classifier_itr;
	dbcl_state_machine_image_classifier::classifier_map_type::iterator classifier_end = this->classifier_map_.end();

	float num_classifiers = this->classifier_map_.size();
	unsigned cnt = 0;
	for( classifier_itr = this->classifier_map_.begin(); classifier_itr != classifier_end; ++classifier_itr, ++cnt )
	{
		vcl_cout << (cnt/num_classifiers)*100 << "% targets classified." << vcl_endl;
		classifier_itr->second->classify();
	}

}//end dbcl_state_machine_image_classifier::classifier()