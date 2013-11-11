#include <testlib/testlib_test.h>
#include <dbcll/dbcll_euclidean_cluster.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vcl_iostream.h>



MAIN( test_euclidean_cluster )
{
  {
    vcl_vector<dbcll_euclidean_cluster<2> > c(4);
  
    c[0] = dbcll_euclidean_cluster<2>(vnl_double_2(0.0, 0.0),0);
    c[1] = dbcll_euclidean_cluster<2>(vnl_double_2(1.0, 1.0),1);
    c[2] = dbcll_euclidean_cluster<2>(vnl_double_2(2.0, 0.0),2);
    c[3] = dbcll_euclidean_cluster<2>(vnl_double_2(-1.0, 1.0),3);
    
    TEST("Similarity (single)", c[0].similarity(c[2]), -4.0);
    
    vnl_vector_fixed<double,2> m = c[0].mean();
    double v = 0.0;
    for(unsigned i=1; i<4; ++i){
      m += c[i].mean();
      v += c[i].mean().squared_magnitude();
    }
    m /= 4.0;
    v -= 4.0* m.squared_magnitude();
    v /= 4.0;

    
    dbcll_euclidean_cluster<2> cm(c[0]);
    cm.merge(c[1]);
    cm.merge(c[2]);
    cm.merge(c[3]);
    TEST("Merged mean", cm.mean(), m);
    TEST("Merged var", cm.var(), v);
    
    bool valid_members = true;
    if(cm.size() == 4){
      for(unsigned i=0; i<4; ++i)
        if(cm.members()[i] != i)
          valid_members = false;
    }
    else
      valid_members = false;
    TEST("Merged members", valid_members, true);
    
    vcl_vector<dbcll_euclidean_cluster<2> > c2(4);
    
    c2[0] = dbcll_euclidean_cluster<2>(vnl_double_2(2.0, 2.0),4);
    c2[1] = dbcll_euclidean_cluster<2>(vnl_double_2(3.0, 1.0),5);
    c2[2] = dbcll_euclidean_cluster<2>(vnl_double_2(4.0, 2.0),6);
    c2[3] = dbcll_euclidean_cluster<2>(vnl_double_2(1.0, 3.0),7);
    
    dbcll_euclidean_cluster<2> cm2(c2[0]);
    cm2.merge(c2[1]);
    cm2.merge(c2[2]);
    cm2.merge(c2[3]);
    
    double sim = cm.similarity(cm2);
    double sim_check = 0.0;
    for(unsigned i=0; i<4; ++i)
      for(unsigned j=0; j<4; ++j)
        sim_check += c[i].similarity(c2[j]);
    sim_check /= 16.0;
    
    TEST("Group average similarity", sim, sim_check);

  }
  SUMMARY();
}


