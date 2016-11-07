//this is /contrib/bm/dbcl/dbcl_state_machine_view.h
#ifndef DBCL_STATE_MACHINE_VIEW_H_
#define DBCL_STATE_MACHINE_VIEW_H_

#include"dbcl_image_classifier.h"
#include"dbcl_state_machine_image_classifier.h"
#include"dbcl_state_machine_classifier_sptr.h"

#include<vcl_ctime.h>
#include<vcl_iostream.h>
#include<vcl_string.h>

#include<vil/vil_image_view.h>
#include<vil/vil_save.h>

#include<vidl/vidl_image_list_istream.h>

#include<vul/vul_file.h>

class dbcl_state_machine_view
{
public:
    typedef dbcl_image_classifier::target_classifier_map_type target_classifier_map_type;

	//unsigned
	typedef dbcl_state_machine_classifier::node_id_type node_id_type;

	//vcl_map<unsigned,vnl_vector<double> >
	typedef dbcl_state_machine_classifier::node_id_mean_map_type node_id_mean_map_type;

	//vcl_map<unsigned,node_id_mean_map_type>
	typedef dbcl_state_machine_classifier::frame_mean_map_type frame_mean_map_type;

	//vcl_map<unsigned,vnl_matrix<double> >
	typedef dbcl_state_machine_classifier::node_id_covar_map_type node_id_covar_map_type;
			
	//vcl_map<unsigned,node_id_covar_map_type>
	typedef dbcl_state_machine_classifier::frame_covar_map_type frame_covar_map_type;

	typedef dbcl_state_machine_classifier::transition_table_type transition_table_type;

	//vcl_map<unsigned,dbcl_state_machine_node::transition_table_type>
	typedef dbcl_state_machine_classifier::frame_transition_table_map_type frame_transition_table_map_type;

	//vcl_map<unsigned,double>
	typedef dbcl_state_machine_classifier::node_id_mixture_weight_map_type node_id_mixture_weight_map_type;

	//vcl_map<unsigned, vcl_map<unsigned,double> >
	typedef dbcl_state_machine_classifier::frame_mixture_weight_map_type frame_mixture_weight_map_type;

	//vcl_map<unsigned, dbcl_temporal_feature_sptr>
	typedef dbcl_classifier::feature_map_type feature_map_type;

	//vcl_map<unsigned,unsigned>
	typedef dbcl_state_machine_classifier::frame_state_map_type frame_state_map_type;

    static void save_change_maps(vcl_string result_dir, dbcl_image_classifier& image_classifier);

	static void save_change_maps(vcl_string result_dir, vcl_string video_glob, dbcl_image_classifier& image_classifier);

	static void save_change_maps(vcl_string result_dir, vcl_string video_glob, dbcl_state_machine_image_classifier& image_classifier);

	static void save_target_temporal_prob_dist( vcl_string result_dir, dbcl_state_machine_classifier_sptr& classifier );

    //dbcl_state_machine_view( vcl_ofstream& of, dbcl_image_classifier& image_classifier);

    ~dbcl_state_machine_view(){}
protected:
    dbcl_state_machine_view(){}

};

#endif //DBCL_STATE_MACHINE_VIEW_H_