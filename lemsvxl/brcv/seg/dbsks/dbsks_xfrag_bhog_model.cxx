// This is file shp/dbsksp/dbsks_xfrag_bhog_model.cxx

//:
// \file

#include "dbsks_xfrag_bhog_model.h"
#include <dbsks/libsvm/svm.h>
#include <vbl/vbl_array_1d.h>
#include <vnl/vnl_math.h>

// =============================================================================
// dbsks_xfrag_bhog_model
// =============================================================================

// -----------------------------------------------------------------------------
//: destructor
dbsks_xfrag_bhog_model::
~dbsks_xfrag_bhog_model()
{
  if (this->libsvm_model_)
  {
    svm_destroy_model(this->libsvm_model_);
    this->libsvm_model_ = 0;
  }
}

// -----------------------------------------------------------------------------
//: Compute the score (probablity) of a BHOG feature vector
bool dbsks_xfrag_bhog_model::
compute_score(const vnl_vector<double >& feature, double& score)
{
  if (!this->libsvm_model_) return false;

  int svm_type = svm_get_svm_type(this->libsvm_model_);
  int nr_class = svm_get_nr_class(this->libsvm_model_);

  // make sure the loaded model is the correct type
  if (svm_type != C_SVC && svm_type != NU_SVC)
    return false;

  // obtain a list of labels
  vnl_vector<int > labels(nr_class, 0);
  svm_get_labels(this->libsvm_model_, labels.data_block());
  
  svm_node* x = new svm_node[feature.size()+1];
  for (unsigned i =0; i < feature.size(); ++i)
  {
    x[i].index = (i+1);
    x[i].value = feature[i];
  }
  x[feature.size()].index = -1;


  if( svm_check_probability_model( this->libsvm_model_)==0 )
  {
    int label = (int) svm_predict(this->libsvm_model_, x);
    score = (label == labels[0]) ? 1 : 0;
  }
  else
  {
    // probability estimate for each class
    vnl_vector<double > prob_estimates(nr_class);
    int label = (int) svm_predict_probability(this->libsvm_model_, 
      x, prob_estimates.data_block());
    score = prob_estimates[0];
  }
  delete[] x;
  return true;

  return false;
}



// -------------------------------------------------------------------------------
//: Compute the score (probablity) of a list of BHOG feature vector
bool dbsks_xfrag_bhog_model::
compute_score(const vcl_vector<vnl_vector<double > >& feat_list, 
              vcl_vector<double >& score_list)
{
  // preliminary checks
  if (feat_list.empty())
  {
    score_list.clear();
    return true;
  }

  // check if the svm model has been loaded
  if (!this->libsvm_model_) 
    return false;

  // make sure the loaded model is the correct type
  int svm_type = svm_get_svm_type(this->libsvm_model_);
  if (svm_type != C_SVC && svm_type != NU_SVC) 
    return false;

  // obtain a list of labels
  int nr_class = svm_get_nr_class(this->libsvm_model_);
  vnl_vector<int > labels(nr_class, 0);
  svm_get_labels(this->libsvm_model_, labels.data_block());
  

  // allocate memory for the feature vector, in LIBSVM format
  vbl_array_1d<svm_node > x;
  int feat_size = feat_list.front().size();
  x.reserve(feat_size + 1);

  // make prediction for each vector one by one
  bool is_probability_model = (svm_check_probability_model(this->libsvm_model_) != 0);
  vnl_vector<double > prob_estimates(nr_class); // probability estimate for each class
  score_list.resize(feat_list.size(), 0);
  for (unsigned k =0; k < feat_list.size(); ++k)
  {
    // reformat the feature vector in libsvm format
    x.clear();
    const vnl_vector<double >& feat = feat_list[k];
    for (int i =0; i < feat_size; ++i)
    {
      if (vnl_math::abs(feat[i]) < 1e-8) continue;
      svm_node node;
      node.index = i+1;
      node.value = feat[i];
      x.push_back(node);
    }
    svm_node terminate;
    terminate.index = -1;
    x.push_back(terminate);

    // make prediction depending on the type of libsvm model
    if( !is_probability_model )
    {
      int label = (int) svm_predict(this->libsvm_model_, x.begin());
      score_list[k] = (label == labels[0]) ? 1 : 0;
    }
    else
    {
      // probability estimate for each class
      double label = svm_predict_probability(this->libsvm_model_, 
        x.begin(), prob_estimates.begin());
      score_list[k] = prob_estimates[0];
    }
  }

  return true;
}


// -----------------------------------------------------------------------------
//: Load SVM model from file
bool dbsks_xfrag_bhog_model::
load_from_file(const vcl_string& libsvm_model_file)
{
  if (this->libsvm_model_)
  {
    svm_destroy_model(this->libsvm_model_);
    this->libsvm_model_ = 0;
  }

  this->libsvm_model_ = svm_load_model(libsvm_model_file.c_str());
  return (this->libsvm_model_) ? true : false;
}






