//this is /contrib/bm/dsm2/algo/dsm2_km_updater.txx
#ifndef DSM2_KM_UPDATER_TXX_
#define DSM2_KM_UPDATER_TXX_
#include<bsta/algo/bsta_gaussian_updater.h>

#include<dsm2/algo/dsm2_km_updater.h>

#include<vnl/vnl_diag_matrix_fixed.h>

template<class nodeT>
void dsm2_km_updater<nodeT>::update(dsm2_state_machine_base_sptr const&
                                    state_machine_base_sptr,
                                    dsm2_feature_base_sptr const& fb,
                                    matrix_type const& init_covar,
                                    matrix_type const& min_covar,
                                    double const& mahalan_thresh)
{
    dsm2_feature<math_type,dimension>* feature_ptr =
        dynamic_cast<dsm2_feature<math_type,dimension>*>(fb.as_pointer());

    if( feature_ptr == 0 )
    {
        vcl_cerr << "----ERROR---- dsm2_km_updater<mathT,dim>::update\n"
                 << "\t feature base smart pointer is not "
                 << "compatible with the updater.\n"
                 << __FILE__ << '\n'
                 << __LINE__ << '\n' << vcl_flush;
        exit(1);
    }//if the dynamic cast is unsuccessfull

    dsm2_km_state_machine<nodeT>* state_machine_km_ptr = 
                    dynamic_cast<dsm2_km_state_machine<nodeT>*>(
                        state_machine_base_sptr.as_pointer() );

    if( state_machine_km_ptr == 0)
    {
        vcl_cerr << "----ERROR---- dsm2_km_updater<mathT,dim>::update\n"
                 << "\t state machine type is not compatible "
                 << "with the updater.\n"
                 << __FILE__ << '\n'
                 << __LINE__ << '\n' << vcl_flush;
        exit(1);
    }//end if the state machine dynamic cast is unsuccessfull.

    if( state_machine_base_sptr->graph.empty() )
    {
        state_machine_km_ptr->curr_node_itr =
            state_machine_base_sptr->add_node();
        
        state_machine_km_ptr->rel_time_curr = 0;

        node_type* node_ptr =
            static_cast<node_type*>(
                state_machine_km_ptr->curr_node_itr->second.as_pointer());

        node_ptr->set_covar(init_covar);

        node_ptr->set_mean(feature_ptr->v);
    }//end if graph.empty
    else//graph is not empty
    {
        vcl_map<double, vcl_map<unsigned,dsm2_node_base_sptr>::iterator>
            weight_node_map;

        vcl_map<unsigned, dsm2_node_base_sptr>::iterator 
            g_itr, g_end = state_machine_km_ptr->graph.end();

        unsigned curr_node_id = state_machine_km_ptr->curr_node_itr->first;
        unsigned rel_time_curr = state_machine_km_ptr->rel_time_curr;

        //1. traverse the graph and check within threshold
        for( g_itr = state_machine_km_ptr->graph.begin(); 
                g_itr != g_end; ++g_itr )
        {
            
            node_type* node_ptr =
                static_cast<node_type*>(g_itr->second.as_pointer());

            double sqr_mahalan = node_ptr->sqr_mahalanobis_dist(feature_ptr->v);

            if( sqr_mahalan < mahalan_thresh )
            {
                double transition_prob = 
                    state_machine_km_ptr->transition_probability(
                        curr_node_id, g_itr->first, rel_time_curr );

                double covar_det = node_ptr->det_covar();

                double discriminant =
                    -5*sqr_mahalan + vcl_log(transition_prob) - .5*covar_det;

                weight_node_map.insert(vcl_make_pair(discriminant,g_itr));
            }//end if sqr_mahalan < mahalan_thresh        
        }//end graph iteration

        if( weight_node_map.empty() )
        {
            //there are no nodes whoes
            //distance to the query is below the threshold
            unsigned old_node_id = state_machine_km_ptr->curr_node_itr->first;

            state_machine_km_ptr->curr_node_itr =
                state_machine_km_ptr->add_node();

            state_machine_km_ptr->transition_map.inc_frequency( 
                old_node_id, 
                    state_machine_km_ptr->curr_node_itr->first, 
                        state_machine_km_ptr->rel_time_curr );

            state_machine_km_ptr->rel_time_curr = 0;

            //initialize the new node
            node_type* node_ptr = static_cast<node_type*>(
                state_machine_km_ptr->curr_node_itr->second.as_pointer());

            node_ptr->set_covar(init_covar);
            node_ptr->set_mean(feature_ptr->v);
        }//end if weight_node_map.empty
        else//weight node map is not empty
        {
            //most probable node is the node that maximizes the discriminant
            //maps stored from lower to higher key so the most probable node
            //will be stored at the back of the map.
            vcl_map<unsigned,dsm2_node_base_sptr>::iterator 
                most_prob_itr = weight_node_map.rbegin()->second;

            unsigned old_node_id = state_machine_km_ptr->curr_node_itr->first;
            unsigned old_rel_time = state_machine_km_ptr->rel_time_curr;
            state_machine_km_ptr->transition_map.inc_frequency(old_node_id,
                                       most_prob_itr->first, old_rel_time);

            //if the most probable node is the current inc the rel_time_curr
            if( most_prob_itr == state_machine_km_ptr->curr_node_itr )
            {
                ++state_machine_km_ptr->rel_time_curr;
            }
            else
            {
                state_machine_km_ptr->rel_time_curr = 0;
                state_machine_km_ptr->curr_node_itr = most_prob_itr;
            }

            //update the current node
            node_type* node_ptr =
                static_cast<node_type*>(
                state_machine_km_ptr->curr_node_itr->second.as_pointer());

            ++node_ptr->num_obs;
            
            bsta_update_gaussian(*node_ptr,
                                 math_type(1/(node_ptr->num_obs)),
                                 feature_ptr->v,
                                 min_covar);

        }//end else weight_node_map.empty
    }//end else graph.empty

    state_machine_base_sptr->notify("update");
    return;
}//end update

#define DSM2_KM_UPDATER_INSTANTIATE(...)\
template class dsm2_km_updater<__VA_ARGS__>
#endif //DSM2_KM_UPDATER_TXX_
