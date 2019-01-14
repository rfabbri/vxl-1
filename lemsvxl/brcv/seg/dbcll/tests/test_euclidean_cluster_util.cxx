//:
// \file
// \author Isabel Restrepo
// \date 14-Sep-2011


#include <testlib/testlib_test.h>
#include <dbcll/dbcll_euclidean_cluster_util.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <iostream>



MAIN( test_euclidean_cluster_util )
{
  {
    std::vector<vnl_vector_fixed<double,2> > means;
    
    means.push_back(vnl_double_2(0.0, 0.0));
    means.push_back(vnl_double_2(1.0, 1.0));
    means.push_back(vnl_double_2(2.0, 0.0));
    means.push_back(vnl_double_2(-1.0, 0.0));

    std::vector<vnl_vector_fixed<double,2> > points;
    
    points.push_back(vnl_double_2(0.1, 0.0));
    points.push_back(vnl_double_2(1.2, 1.1));
    points.push_back(vnl_double_2(2.0, 0.1));
    points.push_back(vnl_double_2(-1.3, 0.0));
    
    dbcll_euclidean_cluster_util<2> cl_util(means);
    
    for(unsigned i=0; i<4; ++i){
      
      double dist;
      TEST("Closest Mean", cl_util.closest_euclidean_cluster(points[i], dist), i);

    }
    
    SUMMARY();
  }
}


