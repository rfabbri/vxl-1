#include <testlib/testlib_test.h>
#include <dbcll/dbcll_rnn_agg_clustering.h>
#include <dbcll/dbcll_euclidean_cluster.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_random.h>
#include <iostream>

namespace{
  
std::vector<vnl_vector_fixed<double,3> > random_vectors(unsigned num)
{
  vnl_random rand;
  std::vector<vnl_vector_fixed<double,3> > pts;
  for(unsigned i=0; i<num; ++i){
    // choose a distribution
    double dist = rand.drand64();
    vnl_double_3 pt(rand.normal64(), rand.normal64(), rand.normal64());
    if(dist < 0.2)
      pts.push_back(0.5*pt);
    else if(dist < 0.5)
      pts.push_back(2.0*pt + vnl_double_3(10.0,-5.0,0.0));
    else
      pts.push_back(pt + vnl_double_3(-4.0,4.0,4.0));              
  }
  
  return pts;
}
  
};


MAIN( test_rnn_agg_clustering )
{
  {
    std::vector<dbcll_cluster_sptr> clusters;
    clusters.push_back(new dbcll_euclidean_cluster<2>(vnl_double_2(0.0, 0.0),0));
    clusters.push_back(new dbcll_euclidean_cluster<2>(vnl_double_2(10.0, 8.0),1));
    clusters.push_back(new dbcll_euclidean_cluster<2>(vnl_double_2(0.2, 0.3),2));
    clusters.push_back(new dbcll_euclidean_cluster<2>(vnl_double_2(9.7, 8.3),3));
    clusters.push_back(new dbcll_euclidean_cluster<2>(vnl_double_2(-5.0, 3.4),4));

    dbcll_remainder_set remain(clusters.begin(), clusters.end());
    std::cout << "remaining set contains "<<remain.size() << std::endl;
    clusters.clear();
    dbcll_rnn_agg_clustering(remain, clusters, -1.0);
    TEST("number of clusters", clusters.size(), 3);
    
    for(unsigned i=0; i<clusters.size(); ++i){
      const dbcll_euclidean_cluster<2>* c = 
        static_cast<const dbcll_euclidean_cluster<2>*>(clusters[i].ptr());
      std::cout << "mean: "<<c->mean()<< "  var: " << c->var()
               << "  members: "<< c->members()[0];
      for(unsigned j=1; j<c->size(); ++j)
        std::cout << ", "<< c->members()[j];
      std::cout << std::endl;
    }
  }
  
  {
    std::vector<vnl_vector_fixed<double,3> > rvs = random_vectors(1000);
    std::vector<dbcll_cluster_sptr> clusters = dbcll_init_euclidean_clusters(rvs);
    dbcll_remainder_heap remain(clusters.begin(), clusters.end());
    dbcll_rnn_agg_clustering(remain, clusters, -25.0);
    std::cout << "number of clusters " << clusters.size() << std::endl;
    
    for(unsigned i=0; i<clusters.size(); ++i){
      const dbcll_euclidean_cluster<3>* c = 
        static_cast<const dbcll_euclidean_cluster<3>*>(clusters[i].ptr());
      std::cout << "size: "<< c->size()
               << "  mean: "<<c->mean()<< "  var: "
               << c->var() << std::endl;
    }
    
    std::vector<unsigned> all_idx;
    for(unsigned i=0; i<clusters.size(); ++i){
      const dbcll_euclidean_cluster<3>* c = 
        static_cast<const dbcll_euclidean_cluster<3>*>(clusters[i].ptr());
      std::vector<unsigned> tmp, members(c->members());
      std::merge(all_idx.begin(), all_idx.end(), 
                members.begin(), members.end(), std::back_inserter(tmp));
      all_idx.swap(tmp);
    }
    TEST("Merged member count",all_idx.size(),rvs.size());
    bool all_idx_valid = true;
    for(unsigned i=0; i<all_idx.size(); ++i){
      if(all_idx[i] != i){
        all_idx_valid = false;
        break;
      }
    }
    TEST("Merged members accounted for",all_idx_valid,true);
  }
  
  SUMMARY();
}


