#include <testlib/testlib_test.h>
#include <dbcll/dbcll_euclidean_cluster.h>
#include <dbcll/dbcll_similarity_matrix.h>
#include <dbcll/dbcll_rnn_agg_clustering.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_random.h>
#include <vcl_iostream.h>


namespace{
double eps = 1e-12;
  
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


bool less_key_index(const dbcll_cluster_sptr& c1,
                    const dbcll_cluster_sptr& c2)
{
  return c1->key_index() < c2->key_index();
}

};

MAIN( test_similarity_matrix )
{
  {
    vcl_vector<dbcll_cluster_sptr> c(4);
  
    c[0] = new dbcll_euclidean_cluster<2>(vnl_double_2(0.0, 0.0),0);
    c[1] = new dbcll_euclidean_cluster<2>(vnl_double_2(1.0, 1.0),1);
    c[2] = new dbcll_euclidean_cluster<2>(vnl_double_2(2.0, 0.0),2);
    c[3] = new dbcll_euclidean_cluster<2>(vnl_double_2(-1.0, 1.0),3);

    vcl_vector<dbcll_cluster_sptr> c2 = dbcll_precompute_similarity(c);

    TEST("Precomputed same number of clusters", c2.size(), c.size());

    bool same = true;
    for(unsigned i=0; i<c.size(); ++i){
      if(c[i]->key_index() != c2[i]->key_index()){
        same = false;
        break;
      }
    }
    TEST("Precomputed same indices", same, true);

    same = true;
    for(unsigned i=0; i<c.size(); ++i){
      if(c[i]->size() != c2[i]->size()){
        same = false;
        break;
      }
    }
    TEST("Precomputed same sizes", same, true);

    same = true;
    for(unsigned i=0; i<c.size(); ++i){
      for(unsigned j=0; j<c.size(); ++j){
        if(vcl_abs(c[i]->similarity(*c[j]) - c2[i]->similarity(*c2[j])) > eps){
          same = false;
          break;
        }
      }
    }
    TEST("Same initial similarities", same, true);

    c[2]->merge(*c[1]);  c.erase(c.begin()+1);
    c2[2]->merge(*c2[1]);  c2.erase(c2.begin()+1);

    same = true;
    for(unsigned i=0; i<c.size(); ++i){
      if(c[i]->key_index() != c2[i]->key_index()){
        same = false;
        break;
      }
    }
    TEST("Same indices after merge", same, true);

    same = true;
    for(unsigned i=0; i<c.size(); ++i){
      for(unsigned j=0; j<c.size(); ++j){
        if(i==j) continue;
        vcl_cout << i<<","<<j<<": "<<c[i]->similarity(*c[j])<<", "<<c2[i]->similarity(*c2[j]) <<vcl_endl;
        if(vcl_abs(c[i]->similarity(*c[j]) - c2[i]->similarity(*c2[j])) > eps){
          same = false;
          break;
        }
      }
    }
    TEST("Same similarities after merge", same, true);

    c[1]->merge(*c[0]);  c.erase(c.begin());
    c2[1]->merge(*c2[0]);  c2.erase(c2.begin());


    same = true;
    for(unsigned i=0; i<c.size(); ++i){
      if(c[i]->key_index() != c2[i]->key_index()){
        same = false;
        break;
      }
    }
    TEST("Same indices after merge", same, true);

    same = true;
    for(unsigned i=0; i<c.size(); ++i){
      for(unsigned j=0; j<c.size(); ++j){
        if(i==j) continue;
        vcl_cout << i<<","<<j<<": "<<c[i]->similarity(*c[j])<<", "<<c2[i]->similarity(*c2[j]) <<vcl_endl;
        if(vcl_abs(c[i]->similarity(*c[j]) - c2[i]->similarity(*c2[j])) > eps){
          same = false;
          break;
        }
      }
    }
    TEST("Same similarities after merge", same, true);

    same = true;
    for(unsigned i=0; i<c.size(); ++i){
      if(c[i]->size() != c2[i]->size()){
        same = false;
        break;
      }
    }
    TEST("Same sizes after merge", same, true);

  }

  {
    vcl_vector<vnl_vector_fixed<double,3> > rvs = random_vectors(1000);
    vcl_vector<dbcll_cluster_sptr> clusters = dbcll_init_euclidean_clusters(rvs);
    vcl_vector<dbcll_cluster_sptr> clusters2 = dbcll_precompute_similarity(clusters);

    dbcll_remainder_heap remain(clusters.begin(), clusters.end());
    dbcll_rnn_agg_clustering(remain, clusters, -25.0);
    vcl_cout << "number of clusters " << clusters.size() << vcl_endl;
    vcl_sort(clusters.begin(),clusters.end(),less_key_index);

    dbcll_remainder_set remain2(clusters2.begin(), clusters2.end());
    dbcll_rnn_agg_clustering(remain2, clusters2, -25.0);
    vcl_cout << "number of clusters " << clusters2.size() << vcl_endl;
    vcl_sort(clusters2.begin(),clusters2.end(),less_key_index);

    TEST("Same number of clusters after clustering", clusters.size(), clusters2.size());

    bool same = true;
    for(unsigned i=0; i<clusters.size(); ++i){
      if(clusters[i]->key_index() != clusters2[i]->key_index()){
        same = false;
        break;
      }
    }
    TEST("Same indices after clustering", same, true);

    same = true;
    for(unsigned i=0; i<clusters.size(); ++i){
      for(unsigned j=0; j<clusters.size(); ++j){
        if(i==j) continue;
        if(vcl_abs(clusters[i]->similarity(*clusters[j])
                 - clusters2[i]->similarity(*clusters2[j])) > eps){
          same = false;
          break;
        }
      }
    }
    TEST("Same similarities after clustering", same, true);

    same = true;
    for(unsigned i=0; i<clusters.size(); ++i){
      if(clusters[i]->size() != clusters2[i]->size()){
        same = false;
        break;
      }
    }
    TEST("Same sizes after clustering", same, true);
  }
  SUMMARY();
}


