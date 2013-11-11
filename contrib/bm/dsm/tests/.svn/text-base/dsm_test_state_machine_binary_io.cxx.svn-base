//this is /contrib/bm/dsm/tests/dsm_test_state_machine_binary_io.cxx
//:
// \file
// \brief Test for Binary IO for dsm_state_machine<T>
//
// \author Brandon A. Mayer b.mayer1@gmail.com
//
// \date  March 16, 2011
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
#include"../dsm_state_machine.h"

#include"../io/dsm_io_state_machine.h"

#include<vcl_string.h>

#include<vnl/vnl_random.h>

#include<testlib/testlib_test.h>

template<int T>
void test_state_machine_binary_io()
{
    vcl_string filename("test_dsm_state_machine_io.bin");
    unsigned nclasses = 5;
    unsigned npts = 20;
    dsm_state_machine_base_sptr state_machine_sptr = new dsm_state_machine<T>(5051);
    vnl_random rand;
    vcl_vector<vnl_vector<double> > observations;

    for( unsigned i = 0; i < nclasses; ++i )
    {
        for( unsigned j = 0; j < npts; ++j )
        {
            vnl_vector<double> obs(T,0.0);
            for( unsigned k = 0; k < T; ++k )
            {
                obs[k] = rand.drand64((double(i)*50)-1.0,(double(i)*50)+1.0);
            }//end dimension iteration
            observations.push_back(obs);
            state_machine_sptr->classify(obs);
        }//end point iteration
    }//end class iteration

    vsl_b_ofstream os(filename.c_str(), vcl_ios::out|vcl_ios::binary);
    //state_machine_sptr->b_write(os);
    dsm_state_machine<T>* p1 = static_cast<dsm_state_machine<T>*>(state_machine_sptr.as_pointer());
    vsl_b_write(os,p1);
    os.close();

    dsm_state_machine_base_sptr state_machine_sptr2 = new dsm_state_machine<T>();

    vsl_b_ifstream is(filename.c_str(), vcl_ios::in|vcl_ios::binary);
    //state_machine_sptr2->b_read(is);
    dsm_state_machine<T>* p2 = static_cast<dsm_state_machine<T>*>(state_machine_sptr2.as_pointer());
    vsl_b_read(is,p2);
    is.close();
    
    TEST_NEAR("TEST STATE MACHINE NDIMS MATCH:",state_machine_sptr->ndims(), state_machine_sptr2->ndims(),1);
    TEST_NEAR("TEST STATE MACHINE CLASSIFIER ID MATCH:", state_machine_sptr->classifier_id(), state_machine_sptr2->classifier_id(),1);
    TEST_NEAR("TEST STATE MACHINE CLASSIFIER CHANGE MATCH:", state_machine_sptr->change(), state_machine_sptr2->change(),1);
    TEST_NEAR("TEST STATE MACHINE CLASSIFIER MAHALAN DISTANCE FACTOR MATCH:", state_machine_sptr->mahalan_dist_factor(), state_machine_sptr2->mahalan_dist_factor(),1);
    TEST_NEAR("TEST STATE MACHINE MAX PROB MATCH:", state_machine_sptr->max_prob(), state_machine_sptr2->max_prob(),.001);
    TEST_NEAR("TEST STATE MACHINE SIZES MATCH:", state_machine_sptr->size(), state_machine_sptr2->size(), 0.5);
    
    vcl_map<unsigned, dsm_node_base_sptr> graph1, graph2;
    graph1 = state_machine_sptr->graph();
    graph2 = state_machine_sptr2->graph();

    for( vcl_map<unsigned, dsm_node_base_sptr>::const_iterator itr = graph1.begin(); itr != graph1.end(); ++itr )
    {
        dsm_node_base_sptr node1 = itr->second;
        dsm_node_base_sptr node2 = graph2[itr->first];
        dsm_node<T>* node1_ptr = static_cast<dsm_node<T>*>(node1.as_pointer());
        dsm_node<T>* node2_ptr = static_cast<dsm_node<T>*>(node2.as_pointer());

        for(unsigned i = 0; i < T; ++i)
         TEST_NEAR("TEST WRITTEN NODE MEAN IS THE SAME AS READ:", node1_ptr->mean()(i), node2_ptr->mean()(i) ,0.001);


        for(unsigned i = 0; i < T; ++i)
            for(unsigned j = 0; j < T; ++j)
                TEST_NEAR("TEST WRITTEN COVARIANCE IS THE SAME AS READ:", node1_ptr->covariance()(i,j), node2_ptr->covariance()(i,j),0.00001);
    }//end graph iteration

    vcl_map<unsigned,unsigned> fsm1 = state_machine_sptr->frame_state_map(), fsm2=state_machine_sptr2->frame_state_map();
    for(vcl_map<unsigned,unsigned>::const_iterator itr = fsm1.begin(); itr != fsm1.end(); ++itr)
    {
        TEST_NEAR("TEST FRAME STATE MAPS MATCH:", itr->second, fsm2[itr->first],.5);
    }


}//end test_state_machine_binary_io

static void dsm_test_state_machine_binary_io()
{
    
    test_state_machine_binary_io<2>();
    return;
}

TESTMAIN(dsm_test_state_machine_binary_io);