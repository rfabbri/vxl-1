//this is /contrib/bm/dsm2/algo/dsm2_km_updater.h
#ifndef DSM2_KM_UPDATER_H_
#define DSM2_KM_UPDATER_H_
#include<dsm2/dsm2_state_machine_base_sptr.h>
#include<dsm2/dsm2_km_state_machine.h>
#include<dsm2/dsm2_node_base_sptr.h>
#include<dsm2/dsm2_node_gaussian.h>
#include<dsm2/dsm2_feature_base_sptr.h>
#include<dsm2/dsm2_feature.h>
#include<dsm2/dsm2_transition_map.h>

template<class nodeT>
class dsm2_km_updater
{
public:
    typedef typename dsm2_km_state_machine<nodeT>::model_type model_type;
    
    typedef typename dsm2_km_state_machine<nodeT>::node_type node_type;

    typedef typename dsm2_km_state_machine<nodeT>::math_type math_type;

    typedef typename dsm2_km_state_machine<nodeT>::vector_type vector_type;

    typedef typename dsm2_km_state_machine<nodeT>::matrix_type matrix_type;

    enum{dimension = nodeT::dimension};

    typedef dsm2_transition_map<unsigned,unsigned,unsigned>
        transition_table_class;

    typedef dsm2_transition_map<unsigned,unsigned,unsigned>::
        transition_table_type transition_table_type;

    static void update(dsm2_state_machine_base_sptr const&
                       state_machine_base_sptr,
                       dsm2_feature_base_sptr const& fb,
                       matrix_type const& init_covar,
                       matrix_type const& min_covar,
                       double const& mahalan_thresh = 2.0);
                        
protected:
    dsm2_km_updater(){}

    ~dsm2_km_updater(){}
};

#endif //DSM2_KM_UPDATER_H_
