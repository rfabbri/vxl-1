#include <testlib/testlib_test.h>
#include <bnld/algo/bnld_eno.h>
#include <bnld/algo/bnld_eno_zerox.h>
#include <bnld/algo/bnld_eno_zerox_label.h>
#include <iostream>
#include <limits>
#include <cmath>

MAIN( test_eno_zerox )
{
   START ("eno_zerox");

   double tol = std::numeric_limits<double>::epsilon()*1000;

   {

     std::cout << "\n Testing bnld_eno_zero_crossing\n";

     bnld_eno_poly p(2);

     //  This is just a test... the user should NOT access "data" members
     // directly

     std::cout << std::endl << "Part 1:" << std::endl;
     p[bnld_eno_poly::zero_order_index]   = -4;
     p[bnld_eno_poly::first_order_index]  =  0;
     p[bnld_eno_poly::second_order_index] =  1;

     bnld_eno_zero_crossing z1;
     z1.compute(p);
     z1.print();
     TEST("Quantity", z1.number(), 2);
     TEST_NEAR("Location", z1.location(0), -2, std::numeric_limits<double>::epsilon()*100);
     TEST_NEAR("Location", z1.location(1),  2, std::numeric_limits<double>::epsilon()*100);


     std::cout << std::endl << "Part 2:" << std::endl;

     bnld_eno_zero_crossing z2;
     p[bnld_eno_poly::zero_order_index]   =  0;
     p[bnld_eno_poly::first_order_index]  =  0;
     p[bnld_eno_poly::second_order_index] =  1000;

     // two roots equal to 0 represents one root of multiplicity 2
     z2.compute(p);
     z2.print();
     TEST("Quantity", z2.number(), 2);
     TEST_NEAR("Location", fabs(z2.location(0)), 0, std::numeric_limits<double>::epsilon()*100);
     TEST_NEAR("Location", fabs(z2.location(1)), 0, std::numeric_limits<double>::epsilon()*100);

     std::cout << "Part 3" << std::endl;
     z2.remove_duplicates();
     
     bnld_eno_zero_crossing z;
     z.combine(&z1,&z2);
     TEST("Quantity after Combine",z.number(),3);
     z.print();
     z.sort();
     TEST("Sort order",z.location(0) <= z.location (1) && z.location(1) <= z.location(2),true);

     z.print();
   }

   {
     std::cout << "\n\n----------------------------------------------------------------------\n";
     std::cout << "Testing bnld_eno_zerox_vector and bnld_eno_zerox_label\n";


     // Define data
     //   - samples along two lines
     vnl_vector<double> data(7);

     // These are samples of:
     //   l(x) = line between points (0,0) and (3,5),  if x < 3
     //   l(x) = line between points (3,5) and (6,-4), if x >= 3
     //   roots: x = 0  and 14/3 ~ 4.667
     //
     data(0) = 0;
     double a=5.0/3.0;
     data(1) = a;
     data(2) = a*2;
     data(3) = a*3;

     a=-3.0;
     double b=14.0;
     data(4) = a*4 + b;
     data(5) = a*5 + b;
     data(6) = a*6 + b;
     
     // data = 0 1.66667 3.33333 5 2 -1 -4


     std::cout << "Data: " << std::endl;
     std::cout << data << std::endl;

     {
     bnld_eno_1d e;
     e.interpolate(&data);
     
     std::cout << "Computing zeros" << std::endl;
     bnld_eno_zerox_vector z(e);
     TEST_NEAR("Zero1, test 1",z[4].location(0),14.0/3.0,tol);
     TEST_NEAR("Zero2, test 1",z[0].location(0),0,tol);

     unsigned label[] = { 0, 11, 22, 33, 44, 55, 66 };
   
     bnld_eno_zerox_label zl(data.data_block(), label, z);

     zl.print(e, z, std::cout);
     TEST("Label1",zl[0][0],11);
     TEST("Label2",zl[4][0],44);
     std::cout << "  zl[4][0] ==  " << zl[4][0] << std::endl;
     }


     
     // ------------------


     { // The following test is only to stress the behavior of the code for this
       // specific example. It doesn't mean it is the correct behaviour.

     vnl_vector<double> data2(7);
     data2 = data - 5;
     // now 2 intervals will signal zeros at 3
     
     bnld_eno_1d e;
     e.interpolate(&data2);

     bnld_eno_zerox_vector z(e);
     TEST_NEAR("Zero1, test 2",z[2].location(0),3,tol);
     TEST_NEAR("Zero2, test 2",z[3].location(0),3,tol);

     unsigned label[] = { 0, 11, 22, 33, 44, 55, 66 };
     bnld_eno_zerox_label zl(data2.data_block(), label, z);

     zl.print(e, z, std::cout);
     TEST("Label1",zl[2][0],bnld_eno_zerox_label::unlabeled);
     TEST("Label2",zl[3][0],bnld_eno_zerox_label::unlabeled);
     }
     // TODO:
     //   - test cases where there are 2 zero crossings in an interval.

     // ------------------

     // OBS: other tests about zero crossings are being executed in test_eno_1d.cxx


   }

   SUMMARY();
}
