//this is /contrib/bm/dbcl/dbcl_image_classifier.cxx
#include"dbcl_image_classifier.h"

dbcl_image_classifier::dbcl_image_classifier( registered_classifiers const& classifier_type,dncn_target_list_2d_sptr target_list_sptr )
{
    target_classifier_map_type target_classifier_map;

    dncn_target_list_2d::target_list_type::iterator target_list_itr;
    dncn_target_list_2d::target_list_type::iterator target_list_end = target_list_sptr->target_list_end();

    this->rows_ = target_list_sptr->rows();
    this->cols_ = target_list_sptr->cols();
    this->nframes_ = target_list_sptr->frames();

    //iterate through the targets (pixels in roi)
    for( target_list_itr = target_list_sptr->target_list_begin(); target_list_itr != target_list_end; ++target_list_itr )
    {
		dncn_target_2d_sptr curr_target_sptr = *target_list_itr;

        //set the training and test data
        dbcl_classifier::feature_map_type training_feature_map;
        dbcl_classifier::feature_map_type test_feature_map;

        //iterate through the features of the target (the frames in which the target is observed)
        dncn_target_2d::feature_map_type::iterator target_feature_itr;
		dncn_target_2d::feature_map_type::iterator target_feature_end = curr_target_sptr->feature_map_end();

        for( target_feature_itr = curr_target_sptr->feature_map_begin(); target_feature_itr != target_feature_end; ++target_feature_itr )
        {
            vnl_vector<double> reduced_feature_vector = target_feature_itr->second->feature_vector_reduced();
            unsigned label = target_feature_itr->second->label();
            unsigned frame = target_feature_itr->second->frame();
            dbcl_temporal_feature_sptr feature_sptr = new dbcl_temporal_feature(frame,reduced_feature_vector,label);

            if( target_feature_itr->second->is_gt() == true )
                training_feature_map[frame] = feature_sptr;
            else
                test_feature_map[frame] = feature_sptr;
        }

        dbcl_classifier_factory factory(classifier_type,training_feature_map,test_feature_map);
        dbcl_classifier_sptr classifier_sptr = factory.classifier();

		this->target_classifier_map_[curr_target_sptr->target()] = classifier_sptr;
    }

}//end dbcl_image_classifier::dbcl_image_classifier

void dbcl_image_classifier::classify()
{
    target_classifier_map_type::const_iterator classifier_itr;
    target_classifier_map_type::const_iterator classifier_end = this->target_classifier_map_.end();

	float num_classifiers = this->target_classifier_map_.size();
	unsigned cnt = 0;
    for( classifier_itr = this->target_classifier_map_.begin(); classifier_itr != classifier_end; ++classifier_itr, ++cnt )
	{
		vcl_cout << (cnt/num_classifiers)*100 << "% targets classified." << vcl_endl;
		//vcl_cout << (classifier_itr - this->target_classifier_map_.begin()) / float(this->target_classifier_map_.size()) << "% of targets classified." << vcl_endl;
        classifier_itr->second->classify();      
	}

}//end dbcl_image_classifier::draw_classifiers_vrml

void dbcl_image_classifier::draw_classifiers_vrml( vcl_string result_glob )
{
    vcl_stringstream ss;

    target_classifier_map_type::const_iterator classifier_itr;
    target_classifier_map_type::const_iterator classifier_end = this->target_classifier_map_.end();

    for( classifier_itr = this->target_classifier_map_.begin(); classifier_itr != classifier_end; ++classifier_itr )
    {
        ss << result_glob << "\\target_x_" << classifier_itr->first.x() << "_y" << classifier_itr->first.y() << ".wrl";
        vcl_string filename = ss.str();
        vcl_ofstream of( filename.c_str(), vcl_ios::out );
        dbcl_linear_classifier_vrml_view::vrml_view( of,classifier_itr->second );
    }

}//end dbcl_image_classifier::draw_classifiers_vrml