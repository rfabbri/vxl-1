//this is /contrib/bm/dsm2/dsm2_state_machine_updater_mahalan.h
#ifndef DSM2_STATE_MACHINE_UPDATER_MAHALAN_H_
#define DSM2_STATE_MACHINE_UPDATER_MAHALAN_H_
#include<dsm2/dsm2_feature.h>
#include<dsm2/dsm2_feature_base_sptr.h>
#include<dsm2/dsm2_state_machine_base_sptr.h>
#include<dsm2/dsm2_state_machine.h>
#include<dsm2/algo/dsm2_num_obs.h>

#include<vnl/vnl_diag_matrix_fixed.h>
#include<vnl/vnl_matrix_fixed.h>

template<class stateMachineT>
class dsm2_state_machine_updater_mahalan
{
public:
	typedef typename stateMachineT::node_class_type node_class_type;

	typedef typename stateMachineT::math_type math_type;

	typedef typename stateMachineT::vector_type vector_type;

	typedef typename stateMachineT::covar_type matrix_type;

	enum{dimension = stateMachineT::dimension};

	typedef typename stateMachineT::transition_class_type transition_class_type;

	typedef typename stateMachineT::node_id_type node_id_type;

	typedef typename stateMachineT::transition_table_type transition_table_type;

	typedef typename stateMachineT::time_type time_type;

	typedef typename stateMachineT::graph_type graph_type;

	typedef typename stateMachineT::graph_type::iterator graph_type_iterator;

	dsm2_state_machine_updater_mahalan<stateMachineT>
		( matrix_type const& min_std,
		  matrix_type const& init_std,
		  double mahalan_thresh = math_type(2.5) ):
	mahalan_thresh(mahalan_thresh), min_std(min_std), init_std(init_std) {}

	virtual ~dsm2_state_machine_updater_mahalan(){}

	virtual bool update( dsm2_state_machine_base_sptr& state_machine_base_sptr,
		dsm2_feature_base_sptr const& feature_base_sptr );

	double mahalan_thresh;

	matrix_type min_std;

	matrix_type init_std;

	graph_type_iterator add_node( dsm2_state_machine_base_sptr& state_machine_base_sptr,
					dsm2_feature_base_sptr const& feature_base_sptr );
	
};




#endif //DSM2_STATE_MACHINE_UPDATER_MAHALAN_H_