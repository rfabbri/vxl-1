// This is file seg/dbsks/dbsks_shapelet_stats.cxx

//:
// \file

#include "dbsks_shapelet_stats.h"
#include <vnl/vnl_math.h>
#include <dbsksp/dbsksp_shapelet.h>

//#include <dbsta/dbsta_gaussian_indep.h>
//#include <dbsta/algo/dbsta_gaussian_stats.h>


// =============================================================================
// dbsks_shapelet_stats
// =============================================================================

//: Compute the shock graph parameter statistics from a set of shock graphs
// with exactly the same topology
bool dbsks_shapelet_stats::
compute_stats(const vcl_vector<dbsksp_shapelet_sptr >& shapelet_list)
{
  // Assume Gaussian distribution for each intrinsic parameters
  unsigned num_params = 5;

  // Collect data from training shapelets
  vcl_vector<vnl_vector<double > > data;
  data.reserve(shapelet_list.size());

  for (unsigned i =0; i < shapelet_list.size(); ++i)
  {
    dbsksp_shapelet_sptr s = shapelet_list[i];
    vnl_vector<double > p(5, 0);
    // log2 for length-related parameters
    p(0) = vcl_log(s->radius_start()) / vnl_math::ln2;
    p(1) = s->phi_start();
    p(2) = s->phi_end();
    p(3) = s->m_start();
    p(4) = vcl_log(s->len()) / vnl_math::ln2;
    data.push_back(p);
  }
  
  // Compute mean
  this->mean_.set_size(num_params);
  this->mean_.fill(0);
  for (unsigned i =0; i < data.size(); ++i)
  {
    this->mean_ += data[i];
  }
  this->mean_ /= data.size();
  
  // Compute variance for each
  this->var_.set_size(num_params);
  this->var_.fill(0);
  for (unsigned i =0; i < data.size(); ++i)
  {
    vnl_vector<double > dev = data[i] - this->mean_;
    this->var_ = dot_product(dev, dev);
  }
  this->var_ /= data.size();
  


  // Compute max and min
  this->max_.set_size(num_params);
  this->max_.fill(-vnl_numeric_traits<double >::maxval);

  this->min_.set_size(num_params);
  this->min_.fill(vnl_numeric_traits<double >::maxval);

  for (unsigned i =0; i < data.size(); ++i)
  {
    for (unsigned j =0; j < num_params; ++j)
    {
      this->min_[j] = vnl_math_min(this->min_[j], data[i][j]);
      this->max_[j] = vnl_math_max(this->max_[j], data[i][j]);
    }
  }
  return true;
}




// -----------------------------------------------------------------------------
//: Extract statistics of a parameter
void dbsks_shapelet_stats::
get_stats(unsigned index, double& mean, double& var, double& min, double& max) const
{
  assert(index < this->mean_.size());
  mean = this->mean_[index];
  var = this->var_[index];
  min = this->min_[index];
  max = this->max_[index];
  return;
}


// -----------------------------------------------------------------------------
//: Set statistics of a parameter
void dbsks_shapelet_stats::
set_stats(unsigned index, double mean, double var, double min, double max)
{
  assert(index < this->mean_.size());
  this->mean_[index] = mean;
  this->var_[index] = var;
  this->min_[index] = min;
  this->max_[index] = max;
}

