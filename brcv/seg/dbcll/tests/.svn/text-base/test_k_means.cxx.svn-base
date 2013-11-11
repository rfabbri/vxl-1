#include <testlib/testlib_test.h>
#include <dbcll/dbcll_k_means.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_random.h>
#include <vcl_iostream.h>
#include <vul/vul_timer.h>

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

vcl_vector<vnl_vector_fixed<double,128> > random_vectors128(unsigned np, unsigned nc)
{
  vnl_random rand;
  vcl_vector<vnl_vector_fixed<double,128> > centers;
  for(unsigned i=0; i<nc; ++i){
    vnl_vector_fixed<double,128> c;
    for(unsigned j=0; j<128; ++j)
      c[j] = rand.normal64();
    centers.push_back(c);
  }

  vcl_vector<vnl_vector_fixed<double,128> > pts;
  for(unsigned i=0; i<np; ++i){
    // choose a center
    unsigned j = static_cast<unsigned>(vcl_floor(rand.drand64()*nc));

    vnl_vector_fixed<double,128> p;
    for(unsigned k=0; k<128; ++k)
      p[k] = rand.normal64()*0.1 + centers[j][k];

    pts.push_back(p);
  }
  
  return pts;
}
  
};


MAIN( test_k_means )
{
  {
    vcl_vector<vnl_vector_fixed<double,2> > points;
    points.push_back(vnl_double_2(0.0, 0.0));
    points.push_back(vnl_double_2(10.0, 8.0));
    points.push_back(vnl_double_2(0.2, 0.3));
    points.push_back(vnl_double_2(9.7, 8.3));
    points.push_back(vnl_double_2(-5.0, 3.4));
    points.push_back(vnl_double_2(-20.0, -20.4));
    points.push_back(vnl_double_2(5.0, -10.4));
    points.push_back(vnl_double_2(-1.0, 1.2));

    vcl_vector<vnl_vector_fixed<double,2> > means;
    means.push_back(points[0]);
    means.push_back(points[1]);
    means.push_back(points[2]);
    vcl_vector<vnl_vector_fixed<double,2> > means2(means);

    vcl_vector<vcl_vector<unsigned> > clusters, clusters2;

    unsigned num = dbcll_k_means(points,clusters,means);
    vcl_cout << "converged in " << num << " iterations" << vcl_endl;

    unsigned num2 = dbcll_fast_k_means(points,clusters2,means2);
    vcl_cout << "fast converged in " << num2 << " iterations" << vcl_endl;

    bool same = true;
    for(unsigned i=0; i<means.size(); ++i){
      if(means[i] != means2[i]){
        same = false;
        break;
      }
      vcl_cout << "mean: " << means[i] << " members: ";
      for(unsigned j=0; j<clusters[i].size(); ++j){
        vcl_cout << clusters[i][j] << " ";
        if(clusters[i][j] != clusters2[i][j]){
          same = false;
          break;
        }
      }
      vcl_cout << vcl_endl;
    }
    TEST("k means fast has same results",same,true);
  }

  {
    vcl_vector<vnl_vector_fixed<double,3> > points = random_vectors(10000);
    vcl_vector<vnl_vector_fixed<double,3> > means = dbcll_init_k_means_rand(points,3);
    vcl_vector<vnl_vector_fixed<double,3> > means2(means);

    vcl_vector<vcl_vector<unsigned> > clusters, clusters2;

    vul_timer t;
    unsigned num = dbcll_k_means(points,clusters,means);
    unsigned time1 = t.user();
    vcl_cout << "converged in " << num << " iterations, "<< time1<<" msec" << vcl_endl;

    t.mark();
    unsigned num2 = dbcll_fast_k_means(points,clusters2,means2);
    unsigned time2 = t.user();
    vcl_cout << "fast converged in " << num2 << " iterations, "<<time2<<" msec" << vcl_endl;

    bool same = true;
    for(unsigned i=0; i<means.size(); ++i){
      if(means[i] != means2[i]){
        same = false;
        break;
      }
      vcl_cout << "mean: " << means[i] << " size: "<< clusters[i].size() <<vcl_endl;
      for(unsigned j=0; j<clusters[i].size(); ++j){
        if(clusters[i][j] != clusters2[i][j]){
          same = false;
          break;
        }
      }
    }
    TEST("k means fast has same results",same,true);
  }

  
  {
    vcl_vector<vnl_vector_fixed<double,128> > points = random_vectors128(100000,100);
    vul_timer t;
    vcl_vector<vnl_vector_fixed<double,128> > means = dbcll_init_k_means_d2(points,100);
    unsigned itime = t.user();
    vcl_cout << "init in "<< itime <<" msec" << vcl_endl;
    vcl_vector<vnl_vector_fixed<double,128> > means2(means);

    vcl_vector<vcl_vector<unsigned> > clusters, clusters2;

    t.mark();
    unsigned num = dbcll_k_means(points,clusters,means);
    unsigned time1 = t.user();
    vcl_cout << "converged in " << num << " iterations, "<< time1<<" msec" << vcl_endl;

    t.mark();
    unsigned num2 = dbcll_fast_k_means(points,clusters2,means2);
    unsigned time2 = t.user();
    vcl_cout << "fast converged in " << num2 << " iterations, "<<time2<<" msec" << vcl_endl;

    bool same = true;
    for(unsigned i=0; i<means.size(); ++i){
      if(means[i] != means2[i]){
        same = false;
        break;
      }
      vcl_cout << " size: "<< clusters[i].size() <<vcl_endl;
      for(unsigned j=0; j<clusters[i].size(); ++j){
        if(clusters[i][j] != clusters2[i][j]){
          same = false;
          break;
        }
      }
    }
    // This test can fail if a cluster ever becomes empty because it will be
    // reseeded with a new center chosen at random
    TEST("k means fast has same results",same,true);
  }

  SUMMARY();
}


