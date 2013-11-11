#include"smw_graph.h"
#include<vpdl/vpdl_gaussian.h>
#include<testlib/testlib_test.h>
#include<vcl_cstdlib.h>
#include<time.h>
static void test_smw_graph()
{

    //create a vector of observations
    //first five will belong to the first node
    //second five will be explained by another node
    //second five will be explained by yet another node
    vnl_vector<float> observations(15,0.0f);
    observations[0] = 0.2f;
    observations[1] = 1.3f;
    observations[2] = -0.7f;
    observations[3] = 2.0f;
    observations[4] = -0.4f;
    observations[5] = 100.0f;
    observations[6] = 99.7f;
    observations[7] = 105.4f;
    observations[8] = 97.4f;
    observations[9] = 100.3;
    observations[10] = 50.4f;
    observations[11] = 45.0f;
    observations[12] = 57.3f;
    observations[13] = 48.2f;
    observations[14] = 51.7f;
                                
    //create a node graph
    smw_graph test_graph;
    for(unsigned i = 0; i < observations.size(); ++i)
    {
        vcl_cout << "======== " << i + 1 << "th observation ======="
                 << vcl_endl;
        vcl_cout << "updating graph with observation: "
                 << observations[i] << vcl_endl;
        test_graph.update(observations[i]);
        test_graph.print_full_graph();
    }
}

TESTMAIN( test_smw_graph )
