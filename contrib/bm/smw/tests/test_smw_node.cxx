//this is contrib/bm/smw/tests/test_node.cxx
#include"smw_node.h"
#include<testlib/testlib_test.h>
#include<vnl/vnl_math.h>

static void test_smw_node()
{
    
    float tol = vnl_math::float_eps;

    //START("test_smw_node");
    
    //create a node
    smw_node test_node;

    test_node.update(4.5f);
    
    TEST_NEAR("mean() returns correct value", test_node.mean(),
              4.5f, tol);

    TEST_NEAR("sigma() returns correct value", test_node.sigma(),
              10.0f, tol);
    
    test_node.update(1.2);
    test_node.update(7.23);

    TEST_NEAR("sequence: 4.5,1.2,7.23",
              test_node.mean(), 4.31f, tol );

//    TEST_NEAR("sigma() returns correct value after sequence: 4.5,1.2,7.23",
//              test_node.sigma(),2.4654,tol);

    vcl_cout << "test_node.sigma() = " << test_node.sigma() << vcl_endl;

}

TESTMAIN( test_smw_node )
