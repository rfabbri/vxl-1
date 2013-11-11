// This is brcv/seg/dbcll/dbcll_sim3d_cluster.cxx
//:
// \file
// \brief A cluster on 3D similarity Lie group manifold
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 3/25/08



#include "dbcll_sim3d_cluster.h"
#include <vcl_cassert.h>
#include <vcl_limits.h>
#include <vnl/algo/vnl_svd.h>




//: compute similarity with another cluster
double dbcll_sim3d_cluster::similarity(const dbcll_cluster& other) const
{
  // Cast to the appropriate type
  assert(dynamic_cast<const dbcll_sim3d_cluster*>(&other));
  const dbcll_sim3d_cluster& c2 =
    static_cast<const dbcll_sim3d_cluster&>(other);

#if 0
  double d=0.0;
  for(vcl_vector<unsigned>::const_iterator i=members_.begin();
      i!=members_.end(); ++i)
  {
    d += c2.similarity((*xforms_)[*i]);
  }
  d /= members_.size();
  return d;
#else

  return -(this->var_ + c2.var_ +
          (this->mean_.inverse()*c2.mean_).lie_algebra_basis().squared_magnitude() );
#endif
}


//: compute similarity with a point
double dbcll_sim3d_cluster::similarity(const dbgl_similarity_3d<double>& pt) const
{
#if 0
  dbgl_similarity_3d<double> pti = pt.inverse();
  double d = 0.0;
  for(vcl_vector<unsigned>::const_iterator i=members_.begin();
      i!=members_.end(); ++i)
  {
    d += (pti*(*xforms_)[*i]).lie_algebra_basis().squared_magnitude();
  }
  d /= members_.size();
  return -d;
#else

  return -(this->var_ + (this->mean_.inverse()*pt).lie_algebra_basis().squared_magnitude());
#endif
}


//: merge the other cluster into this one
void dbcll_sim3d_cluster::merge(const dbcll_cluster& other)
{
  // Cast to the appropriate type
  assert(dynamic_cast<const dbcll_sim3d_cluster*>(&other));
  const dbcll_sim3d_cluster& c2 =
    static_cast<const dbcll_sim3d_cluster&>(other);

  double n = static_cast<double>(this->size());
  double m = static_cast<double>(c2.size());


  vnl_vector_fixed<double,7> geodesic((this->mean_.inverse()*c2.mean_).lie_algebra_basis());
  geodesic *= m/(n+m);
  this->mean_ = this->mean_ * dbgl_similarity_3d<double>(geodesic);


  dbcll_cluster::merge(other);

  compute_stats();

}


//: compute the mean and variance
void dbcll_sim3d_cluster::compute_stats()
{
  assert(xforms_);
  const unsigned num = members_.size();
  assert(num>0);

  // compute mean scale in closed form and mean rotation iteratively
  vcl_vector<vgl_rotation_3d<double> > rotations;
  double ls = 0.0;
  for(vcl_vector<unsigned>::const_iterator i=members_.begin();
      i!=members_.end(); ++i)
  {
    const dbgl_similarity_3d<double>& sim = (*xforms_)[*i];
    rotations.push_back(sim.rotation());
    ls += vcl_log(sim.scale());
  }
  ls /= num;
  mean_.set_scale(vcl_exp(ls));

  vgl_rotation_3d<double> mean_Ri(mean_.rotation().inverse());
  vnl_vector_fixed<double,3> r(0.0);
  for(unsigned j=0; j<20; ++j){
    mean_Ri = vgl_rotation_3d<double>(-r)*mean_Ri;
    r.fill(0.0);
    for(vcl_vector<vgl_rotation_3d<double> >::const_iterator i=rotations.begin();
        i!=rotations.end(); ++i)
    {
      r += (mean_Ri*(*i)).as_rodrigues();
    }
    r /= num;
    //vcl_cout << "merge dm: "<<r<<vcl_endl;
    if( r.squared_magnitude() < vcl_numeric_limits<double>::epsilon() )
      break;
  }

  mean_.set_rotation(mean_Ri.inverse());

  // compute translation components in closed form
  dbgl_similarity_3d<double> mi(mean_.inverse());
  mi.set_translation(vgl_vector_3d<double>(0,0,0));

  vnl_double_3x3 Ac(0.0);
  vnl_double_3 t(0.0,0.0,0.0);
  for(vcl_vector<unsigned>::const_iterator i=members_.begin();
      i!=members_.end(); ++i)
  {
    const dbgl_similarity_3d<double>& sim = (*xforms_)[*i];
    dbgl_similarity_3d<double> sim2 = mi*sim;
    Ac += sim2.lie_log_t_matrix();
    t += sim2.lie_algebra_basis().extract(3,4);
  }
  t /= num;
  t = vnl_svd<double>(1.0/num*Ac).solve(t);
  t = mean_.scale()*(mean_.rotation()*t);

  mean_.set_translation(vgl_vector_3d<double>(t[0],t[1],t[2]));

  // compute variance
  mi = mean_.inverse();
  var_ = 0.0;
  for(vcl_vector<unsigned>::const_iterator i=members_.begin();
      i!=members_.end(); ++i)
  {
    var_ += ((mi*(*xforms_)[*i]).lie_algebra_basis()).squared_magnitude();
  }

  var_ /= num;

  //var_ *= 10;
}


//: Generate a vector of single element clusters
vcl_vector<dbcll_cluster_sptr> 
dbcll_init_sim3d_clusters(const vcl_vector<dbgl_similarity_3d<double> >& pts)
{
  vcl_vector<dbcll_cluster_sptr> results;
  for(unsigned i=0; i<pts.size(); ++i)
    results.push_back(new dbcll_sim3d_cluster(pts,i));
  return results;
}


