#include <testlib/testlib_test.h>
#include <dbcll/dbcll_sim3d_cluster.h>
#include <vcl_iostream.h>
#include <vcl_limits.h>
#include <vnl/vnl_random.h>

static double pi = 3.141592653;

namespace{

vcl_vector<dbgl_similarity_3d<double> > random_xforms(unsigned num)
{
  vnl_random rand;
  vcl_vector<dbgl_similarity_3d<double> > xforms;
  for(unsigned i=0; i<num; ++i){
    double s = vcl_exp(rand.normal64());
    vgl_rotation_3d<double> R(vnl_double_3(rand.normal64(),rand.normal64(),rand.normal64()));
    vgl_vector_3d<double> t(rand.normal64(),rand.normal64(),rand.normal64());
    xforms.push_back(dbgl_similarity_3d<double>(s,R,t));
  }
  return xforms;
}

};


MAIN( test_sim3d_cluster )
{
  {
    vcl_vector<dbgl_similarity_3d<double> > sim(4);

    sim[0] = dbgl_similarity_3d<double>(1.0, vgl_rotation_3d<double>(),
                                        vgl_vector_3d<double>(0.0, 0.0, 0.0));
    sim[1] = dbgl_similarity_3d<double>(2.0, vgl_rotation_3d<double>(pi/4, 0.0, 0.0),
                                        vgl_vector_3d<double>(1.0, 2.0, 3.0));
    sim[2] = dbgl_similarity_3d<double>(0.9, vgl_rotation_3d<double>(0.0, pi/6, 0.0),
                                        vgl_vector_3d<double>(0.0, -0.5, 0.0));
    sim[3] = dbgl_similarity_3d<double>(1.5, vgl_rotation_3d<double>(pi/4, 0.0, pi/2),
                                        vgl_vector_3d<double>(-0.4, 2.0, -2.0));

    vcl_vector<dbcll_sim3d_cluster> c(4);
    c[0] = dbcll_sim3d_cluster(sim,0);
    c[1] = dbcll_sim3d_cluster(sim,1);
    c[2] = dbcll_sim3d_cluster(sim,2);
    c[3] = dbcll_sim3d_cluster(sim,3);

    vcl_cout << "sim 0->1: "<<c[0].similarity(c[1]) << vcl_endl;
    vcl_cout << "sim 0->2: "<<c[0].similarity(c[2]) << vcl_endl;
    vcl_cout << "sim 0->3: "<<c[0].similarity(c[3]) << vcl_endl;
    vcl_cout << "sim 1->1: "<<c[1].similarity(c[1]) << vcl_endl;
    vcl_cout << "sim 1->2: "<<c[1].similarity(c[2]) << vcl_endl;
    vcl_cout << "sim 1->3: "<<c[1].similarity(c[3]) << vcl_endl;
    vcl_cout << "sim 2->3: "<<c[2].similarity(c[3]) << vcl_endl;
    
    TEST("Similarity (single)", c[0].similarity(c[2]), c[2].similarity(c[0]));
    
    c[0].merge(c[2]);
    c[1].merge(c[3]);

    vcl_cout << "var = " << c[0].var() << vcl_endl;
    vcl_cout << "var = " << c[1].var() << vcl_endl;
    c[0].merge(c[1]);
    vcl_cout << "var = " << c[0].var() << vcl_endl;

  }

  {
    unsigned num = 100;
    vcl_vector<dbgl_similarity_3d<double> > sim = random_xforms(num);
    vcl_vector<dbcll_sim3d_cluster> c;
    vcl_vector<unsigned> inds;
    for(unsigned i=0; i<num; ++i){
      c.push_back(dbcll_sim3d_cluster(sim,i));
      inds.push_back(i);
    }

    dbcll_sim3d_cluster super_c(sim,inds);
    vcl_cout << "var: " << super_c.var() << vcl_endl;
    dbgl_similarity_3d<double> mi = super_c.mean().inverse();
    vnl_vector_fixed<double,7> mt(0.0);
    for(unsigned i=0; i<num; ++i){
      mt += (mi*sim[i]).lie_algebra_basis();
    }
    vcl_cout << mt << vcl_endl;
    TEST_NEAR("Mean vector in the tanget of mean()",mt.inf_norm(),0.0,1e-5);

    vnl_matrix<double> D(num,num);
    for(unsigned i=0; i<num; ++i){
      D(i,i) = -vcl_numeric_limits<double>::infinity();
      for(unsigned j=i+1; j<num; ++j)
        D(i,j) = D(j,i) = c[i].similarity(c[j]);
    }

    bool reducible = true;
    
    unsigned remaining = num;
    while(remaining > 1){
      vcl_cout << "remaining: "<< remaining << vcl_endl;
      unsigned i1=0, i2=0, i3=0;
      double s = -vcl_numeric_limits<double>::infinity();
      do{
        i1 = i3;
        for(unsigned i=0; i<remaining; ++i){
          if(D(i1,i) > s){
            i2 = i;
            s = D(i1,i);
          }
        }
        for(unsigned i=0; i<remaining; ++i){
          if(D(i2,i) > s){
            i3 = i;
            s = D(i2,i);
          }
        }
      }while(i3 != i1);

      if(i1 > i2){
        i1 = i2;
        i2 = i3;
      }
      vcl_cout << "using "<<i1<<"("<<c[i1].size()<<") and "<<i2
               << "("<<c[i2].size()<<")"<<vcl_endl;
      vcl_cout << "nearest neighbor dist: " << D(i1,i2) << vcl_endl;
      c[i1].merge(c[i2]);
      vcl_cout << "variance: " <<c[i1].var() << vcl_endl;
      for(unsigned i=0; i<remaining; ++i){
        if(i == i1 || i == i2)
          continue;
        double new_s = c[i1].similarity(c[i]);
        double old_s = vcl_max(D(i1,i),D(i2,i));
        if(new_s > old_s ){
          reducible = false;
          vcl_cout << "new similarity "<<i<<"("<<c[i].size()<<") is less and is " <<new_s
                  << " ("<<D(i1,i)<<", "<<D(i2,i)<<") ";
          vcl_cout << "angle diff: "<<(c[i].mean().rotation().inverse()*c[i1].mean().rotation()).angle()<<vcl_endl;
        }
        D(i1,i) = D(i,i1) = new_s;
        //D(i2,i) = D(i,i2) = -vcl_numeric_limits<double>::infinity();
      }

      remaining--;
      vnl_matrix<double> D2(remaining,remaining);
      D2.update(D.extract(i2,i2));
      D2.update(D.extract(i2,remaining-i2,0,i2+1),0,i2);
      D2.update(D.extract(remaining-i2,i2,i2+1,0),i2,0);
      D2.update(D.extract(remaining-i2,remaining-i2,i2+1,i2+1),i2,i2);
      D.swap(D2);

      c.erase(c.begin()+i2);
    }

    TEST("Reducibility property holds", reducible, true);
    
  }
  SUMMARY();
}


