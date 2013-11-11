//this is /contrib/bm/dsm/tests/dsm_test_manager.cxx
//:
// \file
// \brief Test for dsm_manager class
//
// \author Brandon A. Mayer b.mayer1@gmail.com
//
// \date  March 23, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
#include"../dsm_manager.h"
#include"../io/dsm_io_manager.h"
#include<vbl/io/vbl_io_smart_ptr.h>

#include<vcl_string.h>
#include<vcl_vector.h>

#include<vnl/vnl_random.h>
#include<vnl/vnl_vector.h>

#include<testlib/testlib_test.h>

template<int T>
void test_manager()
{
    unsigned num_state_machines = 5;
    unsigned nobs_per_class = 30;
    unsigned nclasses = 3;
    dsm_manager_base_sptr manager_sptr = new dsm_manager<T>();

    vnl_random rand;
    vcl_vector<vnl_vector<double> > observations;
    for(unsigned i = 0; i < nclasses; ++i)
    {
        for(unsigned j = 0; j < nobs_per_class; ++j)
        {
            vnl_vector<double> obs(T,0.0);
            for( unsigned k = 0; k < T; ++k )
            {
                obs[k] = rand.drand64((double(i)*50)-1.0,(double(i)*50)+1.0);
            }//end dimension iteration
            observations.push_back(obs);
        }//end point iteration
    }//end class iteration



    for(unsigned i = 0; i < num_state_machines; ++i)
    {
        dsm_state_machine_base_sptr state_machine_sptr = new dsm_state_machine<T>();
        dsm_state_machine<T>* sm = static_cast<dsm_state_machine<T>*>(state_machine_sptr.as_pointer());
        sm->set_init_covar(vnl_diag_matrix<double>(T,3));
        vcl_vector<vnl_vector<double> >::const_iterator obs_itr, obs_end = observations.end();
        for( obs_itr = observations.begin(); obs_itr!=obs_end; ++obs_itr )
        {
            state_machine_sptr->classify((*obs_itr));   
        }//end observation iteration
        manager_sptr->insert_state_machine(vgl_point_2d<unsigned>(i,i),state_machine_sptr);
    }//end state machine creation
    
    dsm_manager<T>* manager_ptr = static_cast<dsm_manager<T>*>(manager_sptr.as_pointer());

    vcl_string filename("test_manager_io.bin");
    vsl_b_ofstream os(filename.c_str(), vcl_ios::out|vcl_ios::binary);
    vsl_b_write(os,manager_ptr);
    //vsl_b_write(os,manager_sptr);
    os.close();

    dsm_manager<T>* manager_ptr2 = new dsm_manager<T>();
    vsl_b_ifstream is(filename.c_str(), vcl_ios::out|vcl_ios::binary);
    vsl_b_read(is,manager_ptr2);
    is.close();

    dsm_manager_base_sptr manager_sptr2(manager_ptr2);

    vcl_map<vgl_point_2d<unsigned>, dsm_state_machine_base_sptr, dsm_vgl_point_2d_coord_compare<unsigned> > write_map, read_map;
    write_map = manager_sptr->target_stateMachine_map();
    read_map = manager_sptr2->target_stateMachine_map();

    TEST_NEAR("TEST MANAGER WRITTEN AND READ HAS THE SAME TARGET/STATE MACHINE MAP SIZE", write_map.size(), read_map.size(), 0.5);
    
	vcl_map<vgl_point_2d<unsigned>, dsm_state_machine_base_sptr, dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator smitr, smend=write_map.end();
	for(smitr = write_map.begin(); smitr!= smend; ++smitr)
	{
		vgl_point_2d<unsigned> pt;
		pt = smitr->first;

        dsm_state_machine<T>* smp1 = static_cast<dsm_state_machine<T>*>(manager_sptr->state_machine(pt).as_pointer());
        dsm_state_machine<T>* smp2 = static_cast<dsm_state_machine<T>*>(manager_sptr2->state_machine(pt).as_pointer());

        unsigned state_machine_size1 = smp1->size();
        unsigned state_machine_size2 = smp2->size();

        TEST_NEAR("TEST STATE MACHINE SIZES ARE EQUAL: ", state_machine_size1, state_machine_size2, 0.5);
        
        for( unsigned i = 0; i < state_machine_size1; ++i )
        {
            vcl_map<unsigned, dsm_node_base_sptr> graph1 = smp1->graph();
            vcl_map<unsigned, dsm_node_base_sptr> graph2 = smp2->graph();

            vcl_map<unsigned, dsm_node_base_sptr>::const_iterator g_itr, g_end = graph1.end();

            for(g_itr = graph1.begin(); g_itr != g_end; ++g_itr)
            {
                dsm_node<T>* np1 = static_cast<dsm_node<T>*>(g_itr->second.as_pointer());
                dsm_node<T>* np2 = static_cast<dsm_node<T>*>(graph2[g_itr->first].as_pointer());

                for(unsigned midx = 0; midx < T; ++midx)
                    TEST_NEAR("TEST NODE MEANS ARE EQUAL: ", np1->mean()(midx), np1->mean()(midx), 0.0001);

                for(unsigned k = 0; k < T; ++k)
                    for(unsigned z = 0; z < T; ++z)
                        TEST_NEAR("TEST COVARIANCES ARE EQUAL: ", np1->covariance()(k,z),np2->covariance()(k,z),0.0001);

            }//end graph iteration
        }//end state machine iteration
		
	}//end state machine map iteration

}//end test_manager()

static void dsm_test_manager()
{
   test_manager<2>();
    return;
}//end dsm_test_manager

TESTMAIN(dsm_test_manager);