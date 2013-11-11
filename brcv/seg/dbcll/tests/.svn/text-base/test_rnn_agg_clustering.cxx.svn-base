#include <testlib/testlib_test.h>
#include <dbcll/dbcll_rnn_agg_clustering.h>
#include <dbcll/dbcll_euclidean_cluster.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_random.h>
#include <vcl_iostream.h>

namespace{
  
vcl_vector<vnl_vector_fixed<double,3> > random_vectors(unsigned num)
{
  vnl_random rand;
  vcl_vector<vnl_vector_fixed<double,3> > pts;
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
    vcl_vector<dbcll_cluster_sptr> clusters;
    clusters.push_back(new dbcll_euclidean_cluster<2>(vnl_double_2(0.0, 0.0),0));
    clusters.push_back(new dbcll_euclidean_cluster<2>(vnl_double_2(10.0, 8.0),1));
    clusters.push_back(new dbcll_euclidean_cluster<2>(vnl_double_2(0.2, 0.3),2));
    clusters.push_back(new dbcll_euclidean_cluster<2>(vnl_double_2(9.7, 8.3),3));
    clusters.push_back(new dbcll_euclidean_cluster<2>(vnl_double_2(-5.0, 3.4),4));

    dbcll_remainder_set remain(clusters.begin(), clusters.end());
    vcl_cout << "remaining set contains "<<remain.size() << vcl_endl;
    clusters.clear();
    dbcll_rnn_agg_clustering(remain, clusters, -1.0);
    TEST("number of clusters", clusters.size(), 3);
    
    for(unsigned i=0; i<clusters.size(); ++i){
      const dbcll_euclidean_cluster<2>* c = 
        static_cast<const dbcll_euclidean_cluster<2>*>(clusters[i].ptr());
      vcl_cout << "mean: "<<c->mean()<< "  var: " << c->var()
               << "  members: "<< c->members()[0];
      for(unsigned j=1; j<c->size(); ++j)
        vcl_cout << ", "<< c->members()[j];
      vcl_cout << vcl_endl;
    }
  }
  
  {
    vcl_vector<vnl_vector_fixed<double,3> > rvs = random_vectors(1000);
    vcl_vector<dbcll_cluster_sptr> clusters = dbcll_init_euclidean_clusters(rvs);
    dbcll_remainder_heap remain(clusters.begin(), clusters.end());
    dbcll_rnn_agg_clustering(remain, clusters, -25.0);
    vcl_cout << "number of clusters " << clusters.size() << vcl_endl;
    
    for(unsigned i=0; i<clusters.size(); ++i){
      const dbcll_euclidean_cluster<3>* c = 
        static_cast<const dbcll_euclidean_cluster<3>*>(clusters[i].ptr());
      vcl_cout << "size: "<< c->size()
               << "  mean: "<<c->mean()<< "  var: "
               << c->var() << vcl_endl;
    }
    
    vcl_vector<unsigned> all_idx;
    for(unsigned i=0; i<clusters.size(); ++i){
      const dbcll_euclidean_cluster<3>* c = 
        static_cast<const dbcll_euclidean_cluster<3>*>(clusters[i].ptr());
      vcl_vector<unsigned> tmp, members(c->members());
      vcl_merge(all_idx.begin(), all_idx.end(), 
                members.begin(), members.end(), vcl_back_inserter(tmp));
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


