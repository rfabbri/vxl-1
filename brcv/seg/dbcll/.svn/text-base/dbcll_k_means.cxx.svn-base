// This is brcv/seg/dbcll/dbcll_k_means.cxx
//:
// \file
// \brief K-Means Clustering
// \author Matthew Leotta, (mleotta@lems.brown.edu)
// \date 2/29/08
//
//

#include "dbcll_k_means.h"
#include <vnl/vnl_random.h>



//: Pick a new mean index using the K-Means++ D^2 weighing scheme
unsigned dbcll_random_sample_d2(const vcl_vector<double >& min_dists)
{
  double sum = 0.0;
  typedef vcl_vector<double>::const_iterator ditr;
  for(ditr v = min_dists.begin(); v!=min_dists.end(); ++v)
    sum += (*v) * (*v);

  vnl_random rand;
  double r = rand.drand64()*sum;

  unsigned i=0;
  for(i=0; i<min_dists.size(); ++i){
    double v = min_dists[i] * min_dists[i];
    if(r <= v)
      break;
    else
      r -= v;
  }
  return i;
}


//: Pick a new mean index using the D weighing scheme
// equivalent to D^2 if min_dists is squared distance
unsigned dbcll_random_sample_d(const vcl_vector<double >& min_dists)
{
  double sum = 0.0;
  typedef vcl_vector<double>::const_iterator ditr;
  for(ditr v = min_dists.begin(); v!=min_dists.end(); ++v)
    sum += (*v);

  vnl_random rand;
  double r = rand.drand64()*sum;

  unsigned i=0;
  for(i=0; i<min_dists.size(); ++i){
    const double& v = min_dists[i];
    if(r <= v)
      break;
    else
      r -= v;
  }
  return i;
}

