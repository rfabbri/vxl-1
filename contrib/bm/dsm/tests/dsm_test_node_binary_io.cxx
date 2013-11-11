//this is /contrib/bm/dsm/tests/dsm_test_node_binary_io.cxx
#include<testlib/testlib_test.h>
#include"../dsm_node.h"
#include<vcl_string.h>

template<int T>
void test_node_binary_io()
{
    vcl_string filename("test_dsm_node_binary_io.bin");
    const unsigned id = 26;
    vnl_vector_fixed<double,T> mean;
    vnl_matrix_fixed<double,T,T> covar;

    {
        dsm_node_base_sptr node_sptr = new dsm_node<T>(id);
        vnl_matrix<double> init_covar(T,T,0.1);
        vnl_matrix<double> min_covar(T,T,0.0001);
        vnl_vector<double> init_obs(T,0.0);
        vnl_vector<double> obs1(T,1.0);
        vnl_vector<double> obs2(T,10.5);

        node_sptr->init_model(init_obs,init_covar);
        node_sptr->update_model(obs1,1,min_covar);
        node_sptr->update_model(obs2,2,min_covar);

        dsm_node<T>* node_ptr = static_cast<dsm_node<T>*>(node_sptr.as_pointer());

        mean = node_ptr->mean();
        covar = node_ptr->covariance();

        vsl_b_ofstream os(filename.c_str(), vcl_ios::out|vcl_ios::binary);
        node_sptr->b_write(os);
        //vsl_b_write(os,node_sptr);
        os.close();
    }

    dsm_node_base_sptr node_sptr2 = new dsm_node<T>(unsigned(0));
    dsm_node<T>* node_ptr2 = static_cast<dsm_node<T>*>(node_sptr2.as_pointer());
    vsl_b_ifstream is(filename.c_str(), vcl_ios::in|vcl_ios::binary);
    node_sptr2->b_read(is);
    is.close();

    vcl_cout << vcl_endl << "Value of id = " << id << vcl_endl;
    vcl_cout << vcl_endl << "Value of node_sptr2->id() = " << node_sptr2->id() << vcl_endl;
    TEST_NEAR("TEST WRITTEN ID IS THE SAME AS READ", id, node_sptr2->id(),1);

    for(unsigned i = 0; i < T; ++i)
         TEST_NEAR("TEST WRITTEN MEAN IS THE SAME AS READ", mean(i), node_ptr2->mean()(i),0.001);
   
    for(unsigned i = 0; i < T; ++i)
        for(unsigned j = 0; j < T; ++j)
            TEST_NEAR("TEST WRITTEN COVARIANCE IS THE SAME AS READ", covar(i,j), node_ptr2->covariance()(i,j),0.00001);
   
   
    
    //TEST_NEAR("TEST WRITTEN COVARIANCE IS THE SAME A READ", covar, node_ptr2->covariance(), 0.001);
        
}//end test_node_binary_io<T>

static void dsm_test_node_binary_io()
{
	 test_node_binary_io<int(2)>();
     return;
}

TESTMAIN(dsm_test_node_binary_io);