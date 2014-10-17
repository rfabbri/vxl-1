// This is file shp/dbsksp/dbsks_xfrag_geom_model.cxx

//:
// \file

#include "dbsks_xfrag_geom_model.h"

#include <dbsks/dbsks_utils.h>
#include <dbsks/dbsks_xshock_utils.h>
#include <dbsks/dbsks_biarc_sampler.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <dbsksp/dbsksp_xshock_fragment_sptr.h>

#include <mbl/mbl_data_array_wrapper.h>
#include <pdf1d/pdf1d_gaussian_builder.h>
#include <pdf1d/pdf1d_pdf.h>
#include <pdf1d/pdf1d_sampler.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_random.h>
#include <container/vcl_vector_to_vnl_vector.h>


// =============================================================================
// dbsks_xfrag_geom_model
// =============================================================================

// -----------------------------------------------------------------------------
//: Random generator
vnl_random dbsks_xfrag_geom_model::random_engine = vnl_random(12763909);



// -----------------------------------------------------------------------------
//: Default constructor
dbsks_xfrag_geom_model::
dbsks_xfrag_geom_model()
{
  this->graph_size_ = 100;
  this->nkdiff_threshold_ = 3.0;
  this->biarc_sampler_ = 0;

  this->alpha_start_sampler_ = 0;
  this->chord_sampler_ = 0;
  this->dpsi_sampler_ = 0;
  this->r_end_sampler_ = 0;
  this->phi_end_sampler_ = 0;

  // threshold for constraining start-width
  this->start_width_num_std_ = 3.0;
}



//: destructor
dbsks_xfrag_geom_model::
~dbsks_xfrag_geom_model()
{
  // Generator
  delete alpha_start_sampler_;
  alpha_start_sampler_ = 0;

  delete chord_sampler_;
  chord_sampler_ = 0;

  delete dpsi_sampler_;
  dpsi_sampler_ = 0;

  delete r_end_sampler_;
  r_end_sampler_ = 0;

  delete phi_end_sampler_;
  phi_end_sampler_ = 0;

  return;
}






// -----------------------------------------------------------------------------
//: Set parameter range
void dbsks_xfrag_geom_model::
set_param_range(double min_psi_start, double max_psi_start,
    double min_r_start, double max_r_start,
    double min_phi_start, double max_phi_start,
    double min_alpha_start, double max_alpha_start,
    double min_chord, double max_chord,
    double min_dpsi, double max_dpsi,
    double min_r_end, double max_r_end,
    double min_phi_end, double max_phi_end,
    double graph_size)
{
  double eps = 1e-5;

  // make sure the range is less than 2*Pi
  max_psi_start = vnl_math_min(max_psi_start, min_psi_start + 2*vnl_math::pi);

  // make sure radius is always positive
  min_r_start = vnl_math_max(1.0, min_r_start);
  max_r_start = vnl_math_max(1.0, max_r_start);
  max_r_start = vnl_math_max(min_r_start + eps, max_r_start);

  // make sure phi is between 0 and pi
  min_phi_start = vnl_math_max(min_phi_start, vnl_math::pi / 36);
  max_phi_start = vnl_math_min(max_phi_start, vnl_math::pi - vnl_math::pi/36);
  max_phi_start = vnl_math_max(min_phi_start + eps, max_phi_start);

  // make sure chord is always positive
  min_chord = vnl_math_max(1.0, min_chord);
  max_chord = vnl_math_max(1.0, max_chord);
  max_chord = vnl_math_max(min_chord + eps, max_chord);

  // make sure radius end is always positive
  min_r_end = vnl_math_max(1.0, min_r_end);
  max_r_end = vnl_math_max(1.0, max_r_end);
  max_r_end = vnl_math_max(min_r_end + eps, max_r_end);

  // make sure phi_end is between 0 and pi
  min_phi_end = vnl_math_max(min_phi_end, vnl_math::pi / 36);
  max_phi_end = vnl_math_min(max_phi_end, vnl_math::pi - vnl_math::pi/36);
  max_phi_end = vnl_math_max(min_phi_end + eps, max_phi_end);

  // set pdf
  dbsks_regularize_min_max_values(min_psi_start, max_psi_start);
  this->psi_start_model_.set(min_psi_start, max_psi_start);
  //
  dbsks_regularize_min_max_values(min_r_start, max_r_start);
  this->r_start_model_.set(min_r_start, max_r_start);
  //
  dbsks_regularize_min_max_values(min_phi_start, max_phi_start);
  this->phi_start_model_.set(min_phi_start, max_phi_start);
  //
  dbsks_regularize_min_max_values(min_alpha_start, max_alpha_start);
  this->alpha_start_model_.set(min_alpha_start, max_alpha_start);
  this->alpha_start_sampler_ = this->alpha_start_model_.new_sampler();
  //
  dbsks_regularize_min_max_values(min_chord, max_chord);
  this->chord_model_.set(min_chord, max_chord);
  this->chord_sampler_ = this->chord_model_.new_sampler();
  //
  dbsks_regularize_min_max_values(min_dpsi, max_dpsi);
  this->dpsi_model_.set(min_dpsi, max_dpsi);
  this->dpsi_sampler_ = this->dpsi_model_.new_sampler();
  //
  dbsks_regularize_min_max_values(min_r_end, max_r_end);
  this->r_end_model_.set(min_r_end, max_r_end);
  this->r_end_sampler_ = this->r_end_model_.new_sampler();
  //
  dbsks_regularize_min_max_values(min_phi_end, max_phi_end);
  this->phi_end_model_.set(min_phi_end, max_phi_end);
  this->phi_end_sampler_ = this->phi_end_model_.new_sampler();

  this->graph_size_ = graph_size;
  return;
}


// -----------------------------------------------------------------------------
//: Get the parameter range
void dbsks_xfrag_geom_model::
get_param_range(double& min_psi_start, double& max_psi_start,
                double& min_r_start, double& max_r_start,
                double& min_phi_start, double& max_phi_start,
                double& min_alpha_start, double& max_alpha_start,
                double& min_chord, double& max_chord,
                double& min_dpsi, double& max_dpsi,
                double& min_r_end, double& max_r_end,
                double& min_phi_end, double& max_phi_end,
                double& graph_size)
{
  //
  min_psi_start = this->psi_start_model_.lo();
  max_psi_start = this->psi_start_model_.hi();
  
  min_r_start = this->r_start_model_.lo();
  max_r_start = this->r_start_model_.hi();

  min_phi_start = this->phi_start_model_.lo(); 
  max_phi_start = this->phi_start_model_.hi();

  min_alpha_start = this->alpha_start_model_.lo();
  max_alpha_start = this->alpha_start_model_.hi();

  min_chord = this->chord_model_.lo();
  max_chord = this->chord_model_.hi();

  min_dpsi = this->dpsi_model_.lo();
  max_dpsi = this->dpsi_model_.hi();

  min_r_end = this->r_end_model_.lo();
  max_r_end = this->r_end_model_.hi();

  min_phi_end = this->phi_end_model_.lo();
  max_phi_end = this->phi_end_model_.hi();

  graph_size = this->graph_size_ ;
  return;
}




//------------------------------------------------------------------------------
//: Get range of psi start
void dbsks_xfrag_geom_model::
get_range_psi_start(double& min_psi_start, double& max_psi_start) const
{
  min_psi_start = this->psi_start_model_.lo();
  max_psi_start = this->psi_start_model_.hi();
  return;
}


//------------------------------------------------------------------------------
//: Get range of phi start
void dbsks_xfrag_geom_model::
get_range_phi_start(double& min_phi_start, double& max_phi_start) const
{
  min_phi_start = this->phi_start_model_.lo(); 
  max_phi_start = this->phi_start_model_.hi();
  return;
}




//------------------------------------------------------------------------------
//: Get range of chord length
void dbsks_xfrag_geom_model::
get_range_chord(double& min_chord, double& max_chord) const
{
  min_chord = this->chord_model_.lo();
  max_chord = this->chord_model_.hi();
}

//------------------------------------------------------------------------------
//: Get range of alpha - angle between shock chord and shock tangent
void dbsks_xfrag_geom_model::
get_range_alpha_start(double& min_alpha, double& max_alpha) const
{
  min_alpha = this->alpha_start_model_.lo();
  max_alpha = this->alpha_start_model_.hi();
}










//------------------------------------------------------------------------------
//: Get data values for a geometric attribute
// Return false if the attribute does not exist
bool dbsks_xfrag_geom_model::
get_attr_data(const vcl_string& attr_name, vcl_vector<double >& attr_values) const
{
  // clean up first
  attr_values.clear();

  vcl_map<vcl_string, vcl_vector<double > >::const_iterator iter = 
    this->attr_data_.find(attr_name);

  // return false if the specified attribute does not exist
  if (iter == this->attr_data_.end())
    return false;

  // copy to the returned variable
  attr_values = iter->second;
  return true;  
}



// -----------------------------------------------------------------------------
//: Generate samples for the ending node descriptor, hard-coded variation range
bool dbsks_xfrag_geom_model::
sample_end_given_start_using_fixed_range(const dbsksp_xshock_node_descriptor& start, 
                                         int num_samples,
                                         vcl_vector<dbsksp_xshock_node_descriptor >& xdesc_list)
{
  xdesc_list.resize(num_samples);

  // rotate the original fragment to align with the given start_node descritor
  dbsksp_xshock_fragment_sptr ref_xfrag = new dbsksp_xshock_fragment(this->ref_start_, this->ref_end_);
  dbsksp_xshock_fragment_sptr xfrag = ref_xfrag->rot_scale_trans(this->ref_start_.pt(), 
    start.psi_-this->ref_start_.psi_, 1, start.pt() - this->ref_start_.pt());

  dbsksp_xshock_node_descriptor end0 = xfrag->end();
  
  // \TODO
  // hard-code the variation for now, should change in the future
  double len = (end0.pt() - start.pt()).length();
  double delta_x = 0.15 * len;
  double delta_y = 0.15 * len;
  double delta_psi = vnl_math::pi / 9;
  double delta_phi = vnl_math::pi / 9;
  double delta_r = 0.15 * end0.radius_;

  double x0 = end0.pt().x();
  double y0 = end0.pt().y();
  double psi0 = end0.psi_;
  double phi0 = end0.phi_;
  double r0 = end0.radius_;

  //vnl_random rand_engine(10120349);
  // for now, ignore the starting node descriptor, just perturb the end descriptor
  for (int k =0; k < num_samples; ++k)
  {
    double dx = random_engine.drand32(-delta_x, delta_x);
    double dy = random_engine.drand32(-delta_y, delta_y);
    double dpsi = random_engine.drand32(-delta_psi, delta_psi);
    double dphi = random_engine.drand32(-delta_phi, delta_phi);
    double dr = random_engine.drand32(-delta_r, delta_r);
    xdesc_list[k].set(x0+dx, y0+dx, psi0+dpsi, phi0+dphi, r0+dr);
  }

  //// \debug ///////////////////////////////////////////////////////
  //// put the source descriptor to the sample set
  //xdesc_list[0].set(x0, y0, psi0, phi0, r0);
  //// ///////////////////////////////////////////////////////
  return true;
}

//// -----------------------------------------------------------------------------
////: Generate samples for the ending node descriptor, using minmax-range from model
//bool dbsks_xfrag_geom_model::
//sample_end_given_start_using_model_minmax_range(const dbsksp_xshock_node_descriptor& start, 
//                                                double graph_size, int num_samples, 
//                                                vcl_vector<dbsksp_xshock_node_descriptor >& xdesc_list)
//{
//  xdesc_list.resize(num_samples);
//
//  // ratio between current graph size and the model graph size
//  double scale_coeff = graph_size / this->graph_size_;
//
//  // range of variation
//  double mean_chord = vcl_sqrt(this->chord_model_.hi()*this->chord_model_.lo()) * scale_coeff;
//  double delta_log_chord = vcl_log(this->chord_model_.hi() / this->chord_model_.lo()) / 2;
//
//  double mean_alpha_start = (this->alpha_start_model_.hi() +this->alpha_start_model_.lo()) / 2;
//  double delta_alpha_start = (this->alpha_start_model_.hi() - this->alpha_start_model_.lo()) / 2;
//  
//  double mean_phi_end = (this->phi_end_model_.hi() +this->phi_end_model_.lo()) / 2;
//  double delta_phi_end = (this->phi_end_model_.hi()-this->phi_end_model_.lo()) / 2;
//  
//  double mean_r_end = vcl_sqrt(this->r_end_model_.hi()*this->r_end_model_.lo()) * scale_coeff;
//  double delta_log_r_end = vcl_log(this->r_end_model_.hi()/this->r_end_model_.lo()) / 2;
//
//  double mean_dpsi = (this->dpsi_model_.hi() + this->dpsi_model_.lo())/2;
//  double delta_dpsi = (this->dpsi_model_.hi() - this->dpsi_model_.lo())/2;
//
//  // for now, ignore the starting node descriptor, just perturb the end descriptor
//  double x_start = start.pt().x();
//  double y_start = start.pt().y();
//  for (int k =0; k < num_samples; ++k)
//  {
//    // x and y
//    double dlog_chord = random_engine.drand32(-delta_log_chord, delta_log_chord);
//    double chord = mean_chord * vcl_exp(dlog_chord);
//
//    double dalpha_start = random_engine.drand32(-delta_alpha_start, delta_alpha_start);
//    double alpha_start = mean_alpha_start + dalpha_start;
//
//    double x_end = x_start + vcl_cos(start.psi_ - alpha_start) * chord;
//    double y_end = y_start + vcl_sin(start.psi_ - alpha_start) * chord;
//
//    // phi
//    double dphi_end = random_engine.drand32(-delta_phi_end, delta_phi_end);
//    double phi_end = mean_phi_end + dphi_end;
//
//    // psi
//    double ddpsi = random_engine.drand32(-delta_dpsi, delta_dpsi);
//    double psi_end = start.psi_ + mean_dpsi + ddpsi;
//
//    // radius
//    double dlog_r_end = random_engine.drand32(-delta_log_r_end, delta_log_r_end);
//    double r_end = mean_r_end * vcl_exp(dlog_r_end);
//
//    xdesc_list[k].set(x_end, y_end, psi_end, phi_end, r_end);
//  }
//  return true;
//}
//

// -----------------------------------------------------------------------------
//: Generate samples for the ending node descriptor, using minmax-range from model
bool dbsks_xfrag_geom_model::
sample_end_given_start_using_model_minmax_range(const dbsksp_xshock_node_descriptor& start, 
                                                double graph_size, int num_samples, 
                                                vcl_vector<dbsksp_xshock_node_descriptor >& xdesc_list)
{
  xdesc_list.resize(num_samples);

  // ratio between current graph size and the model graph size
  double scale_coeff = graph_size / this->graph_size_;

  double x_start = start.pt().x();
  double y_start = start.pt().y();
  
  // chord
  vnl_vector<double > vec_chord(num_samples);
  this->chord_sampler_->get_samples(vec_chord);
  vec_chord *= scale_coeff;
  
  // alpha_start
  vnl_vector<double > vec_alpha_start(num_samples);
  this->alpha_start_sampler_->get_samples(vec_alpha_start);

  // phi_end
  vnl_vector<double > vec_phi_end(num_samples);
  this->phi_end_sampler_->get_samples(vec_phi_end);

  // dpsi
  vnl_vector<double > vec_dpsi(num_samples);
  this->dpsi_sampler_->get_samples(vec_dpsi);

  // r_end
  vnl_vector<double > vec_r_end(num_samples);
  this->r_end_sampler_->get_samples(vec_r_end);
  vec_r_end *= scale_coeff;


  for (int k =0; k < num_samples; ++k)
  {
    // x and y
    double chord = vec_chord[k]; //scale_coeff * this->chord_sampler_->sample();
    double alpha_start = vec_alpha_start[k]; // this->alpha_start_sampler_->sample();

    double x_end = x_start + vcl_cos(start.psi_ - alpha_start) * chord;
    double y_end = y_start + vcl_sin(start.psi_ - alpha_start) * chord;

    // phi
    double phi_end = vec_phi_end[k]; // this->phi_end_sampler_->sample();

    // psi
    double dpsi = vec_dpsi[k]; //this->dpsi_sampler_->sample();
    double psi_end = start.psi() + dpsi;

    // radius
    double r_end = vec_r_end[k]; //scale_coeff * this->r_end_sampler_->sample();

    xdesc_list[k].set(x_end, y_end, psi_end, phi_end, r_end);
  }
  return true;
}







//// -----------------------------------------------------------------------------
////: Generate samples for the ending node descriptor, using minmax-range from model
//bool dbsks_xfrag_geom_model::
//sample_legal_end_given_start_using_model_minmax_range(const dbsksp_xshock_node_descriptor& start, 
//                                                double graph_size, int num_samples, 
//                                                vcl_vector<dbsksp_xshock_node_descriptor >& xdesc_list)
//{
//  //
//  double length_padding_ratio = 1; //2;
//  double angle_padding_ratio = 1; //1.5;
//
//  xdesc_list.clear();
//
//  // ratio between current graph size and the model graph size
//  double scale_coeff = graph_size / this->graph_size_;
//
//  // range of variation
//  double mean_chord = vcl_sqrt(this->chord_model_.hi()*this->chord_model_.lo()) * scale_coeff;
//  double delta_log_chord = length_padding_ratio * vcl_log(this->chord_model_.hi() / this->chord_model_.lo()) / 2;
//
//  double mean_alpha_start = (this->alpha_start_model_.hi() +this->alpha_start_model_.lo()) / 2;
//  double delta_alpha_start = angle_padding_ratio * (this->alpha_start_model_.hi()-this->alpha_start_model_.lo()) / 2;
//  
//  double mean_phi_end = (this->phi_end_model_.hi() +this->phi_end_model_.lo()) / 2;
//  double delta_phi_end = angle_padding_ratio * (this->phi_end_model_.hi()-this->phi_end_model_.lo()) / 2;
//  
//  double mean_r_end = vcl_sqrt(this->r_end_model_.hi()*this->r_end_model_.lo()) * scale_coeff;
//  double delta_log_r_end = length_padding_ratio * vcl_log(this->r_end_model_.hi()/this->r_end_model_.lo()) / 2;
//
//  double mean_dpsi = (this->dpsi_model_.hi() + this->dpsi_model_.lo())/2;
//  double delta_dpsi = angle_padding_ratio * (this->dpsi_model_.hi() - this->dpsi_model_.lo())/2;
//
//  // for now, ignore the starting node descriptor, just perturb the end descriptor
//  // we will generate up to 10*num_samples and take the first num_samples that form legal
//  // xshock fragments
//  double x_start = start.pt().x();
//  double y_start = start.pt().y();
//  int num_samples_to_try = 10*num_samples;
//  int count_legal_samples = 0;
//  dbsksp_xshock_fragment_sptr test_xfrag = new dbsksp_xshock_fragment(start, start);
//  for (int k =0; k < num_samples_to_try && count_legal_samples < num_samples; ++k)
//  {
//    // x and y
//    double dlog_chord = random_engine.drand32(-delta_log_chord, delta_log_chord);
//    double chord = mean_chord * vcl_exp(dlog_chord);
//
//    double dalpha_start = random_engine.drand32(-delta_alpha_start, delta_alpha_start);
//    double alpha_start = mean_alpha_start + dalpha_start;
//
//    double x_end = x_start + vcl_cos(start.psi_ - alpha_start) * chord;
//    double y_end = y_start + vcl_sin(start.psi_ - alpha_start) * chord;
//
//    // phi
//    double dphi_end = random_engine.drand32(-delta_phi_end, delta_phi_end);
//    double phi_end = mean_phi_end + dphi_end;
//
//    // psi
//    double ddpsi = random_engine.drand32(-delta_dpsi, delta_dpsi);
//    double psi_end = start.psi_ + mean_dpsi + ddpsi;
//
//    // radius
//    double dlog_r_end = random_engine.drand32(-delta_log_r_end, delta_log_r_end);
//    double r_end = mean_r_end * vcl_exp(dlog_r_end);
//
//    // only take this configuration if it forms a legal fragment
//    dbsksp_xshock_node_descriptor end(x_end, y_end, psi_end, phi_end, r_end);
//    test_xfrag->set_end(end);
//
//    // check constraint
//    if (!this->check_constraints(*test_xfrag))
//      continue;
//
//    // check legality
//    if (!test_xfrag->is_legal())
//      continue;
//    
//    xdesc_list.push_back(end);
//    ++count_legal_samples;
//  }
//  return true;
//}




// -----------------------------------------------------------------------------
//: Generate samples for the ending node descriptor, using minmax-range from model
bool dbsks_xfrag_geom_model::
sample_legal_end_given_start_using_model_minmax_range(const dbsksp_xshock_node_descriptor& start, 
                                                double graph_size, int num_samples, 
                                                vcl_vector<dbsksp_xshock_node_descriptor >& xdesc_list)
{
  xdesc_list.clear();

  // ratio between current graph size and the model graph size
  double scale_coeff = graph_size / this->graph_size_;

  // we will generate up to 10*num_samples and take the first num_samples that form legal xshock fragments
  
  double x_start = start.pt().x();
  double y_start = start.pt().y();
  
  int num_samples_to_try = 100*num_samples;
  int count_legal_samples = 0;
  
  dbsksp_xshock_fragment test_xfrag(start, start);
  for (int k =0; k < num_samples_to_try && count_legal_samples < num_samples; ++k)
  {
    // x and y
    double chord = scale_coeff * this->chord_sampler_->sample();
    double alpha_start = this->alpha_start_sampler_->sample();

    double x_end = x_start + vcl_cos(start.psi_ - alpha_start) * chord;
    double y_end = y_start + vcl_sin(start.psi_ - alpha_start) * chord;

    // phi
    double phi_end = this->phi_end_sampler_->sample();

    // psi
    double dpsi = this->dpsi_sampler_->sample();
    double psi_end = start.psi() + dpsi;

    // radius
    double r_end = scale_coeff * this->r_end_sampler_->sample();

    // only take this configuration if it forms a legal fragment
    dbsksp_xshock_node_descriptor end(x_end, y_end, psi_end, phi_end, r_end);
    test_xfrag.set_end(end);

    // check constraint
    if (!this->check_constraints(test_xfrag))
      continue;

    // check legality
    if (!test_xfrag.is_legal())
      continue;
    
    xdesc_list.push_back(end);
    ++count_legal_samples;
  }
  if(xdesc_list.size() ==0)
  {
	vcl_cout << "Fail in sampling any legal end" << vcl_endl;
	return false;
  }
  return true;
}


// -----------------------------------------------------------------------------
//: Generate samples for the ending node descriptor, using minmax-range from model
bool dbsks_xfrag_geom_model::
sample_new_legal_end_given_start_using_model_minmax_range(const dbsksp_xshock_node_descriptor& start, 
                                                double graph_size, int num_samples, 
                                                vcl_vector<dbsksp_xshock_node_descriptor >& xdesc_list)
{
  xdesc_list.clear();

  // ratio between current graph size and the model graph size
  double scale_coeff = graph_size / this->graph_size_;

  // we will generate up to 10*num_samples and take the first num_samples that form legal xshock fragments
  
  double x_start = start.pt().x();
  double y_start = start.pt().y();
  
  int num_samples_to_try = 100*num_samples;
  int count_legal_samples = 0;
  
  dbsksp_xshock_fragment test_xfrag(start, start);
  for (int k =0; k < num_samples_to_try && count_legal_samples < num_samples; ++k)
  {
    // x and y
    double chord = scale_coeff * this->chord_sampler_->sample();
    double alpha_start = this->alpha_start_sampler_->sample();

    double x_end = x_start + vcl_cos(start.psi_ - alpha_start) * chord;
    double y_end = y_start + vcl_sin(start.psi_ - alpha_start) * chord;

    // phi
    double phi_end = this->phi_end_sampler_->sample();

    // psi
    double dpsi = this->dpsi_sampler_->sample();
    double psi_end = start.psi() + dpsi;

    // radius
    double r_end = scale_coeff * this->r_end_sampler_->sample();

    // only take this configuration if it forms a legal fragment
    dbsksp_xshock_node_descriptor end(x_end, y_end, psi_end, phi_end, r_end);
    test_xfrag.set_end(end);

    // check constraint
    if (!this->check_constraints(test_xfrag))
      continue;

    // check legality
    if (!test_xfrag.is_legal_new())
      continue;
    
    xdesc_list.push_back(end);
    ++count_legal_samples;
  }
  if(xdesc_list.size() ==0)
  {
	vcl_cout << "Fail in sampling any legal end" << vcl_endl;
	return false;
  }
  return true;
}










// -----------------------------------------------------------------------------
//: Uniform sampling of the starting location of the fragment given the allowed
// deviation from the initial location
bool dbsks_xfrag_geom_model::
sample_start_position(int num_samples, double delta_x, double delta_y,
                      vcl_vector<double >& x, vcl_vector<double >& y)
{
  x.resize(num_samples);
  y.resize(num_samples);

  double x0 = this->ref_start_.pt().x();
  double y0 = this->ref_start_.pt().y();

  // for now, ignore the starting node descriptor, just perturb the end descriptor
  for (int k =0; k < num_samples; ++k)
  {
    x[k] = x0 + random_engine.drand32(-delta_x, delta_x);
    y[k] = y0 + random_engine.drand32(-delta_y, delta_y);
  }
  return true;
}



//: Generate samples of the position of the starting node which form a grid surrounding
// its current position
bool dbsks_xfrag_geom_model::
sample_start_position(double step_x, int num_x_backward, int num_x_forward,
                      double step_y, int num_y_backward, int num_y_forward, 
                      vcl_vector<double >& xs, vcl_vector<double >& ys)
{
  int num_pts = (num_x_backward+num_x_forward+1) * (num_y_backward+num_y_forward+1);
  xs.reserve(num_pts);
  ys.reserve(num_pts);


  double x0 = this->ref_start_.pt().x();
  double y0 = this->ref_start_.pt().y();

  // generate the point grid around the reference point (x0, y0);
  for (int i_x = -num_x_backward; i_x <= num_x_forward; ++i_x)
  {
    double x = x0 + i_x * step_x;
    for (int i_y = -num_y_backward; i_y <= num_y_forward; ++i_y)
    {
      double y = y0 + i_y * step_y;
      xs.push_back(x);
      ys.push_back(y);
    }
  }
  
  return true;
}





// -----------------------------------------------------------------------------
//: Uniform sampling of the intrinsic properties of the start descriptor, given the allowed range
bool dbsks_xfrag_geom_model::
sample_start_idesc(int num_samples, double delta_psi, double delta_phi, double delta_log2r,
                   vcl_vector<double >& psi, vcl_vector<double >& phi, vcl_vector<double >& r)
{
  psi.resize(num_samples);
  phi.resize(num_samples);
  r.resize(num_samples);

  double psi0 = this->ref_start_.psi_;
  double phi0 = this->ref_start_.phi_;
  double r0 = this->ref_start_.radius_;

  // for now, ignore the starting node descriptor, just perturb the end descriptor
  for (int k =0; k < num_samples; ++k)
  {
    psi[k] = psi0 + random_engine.drand32(-delta_psi, delta_psi);
    phi[k] = phi0 + random_engine.drand32(-delta_phi, delta_phi);
    r[k] = r0 * vcl_exp(random_engine.drand32(-delta_log2r, delta_log2r) * vnl_math::ln2);
  }
  return true;
}




//------------------------------------------------------------------------------
//: Build model from computed attribute data
bool dbsks_xfrag_geom_model::
build_from_attr_data()
{
  // \experiment different methods to construct grid
  enum compute_method
  {
    MINMAX = 0,
    GAUSSIAN_3STD = 1,
  };
//  compute_method method = GAUSSIAN_3STD;
  compute_method method = GAUSSIAN_3STD;

  vcl_vector<double > vcl_psi_start;
  vcl_vector<double > vcl_r_start;
  vcl_vector<double > vcl_phi_start;

  vcl_vector<double > vcl_alpha_start;
  vcl_vector<double > vcl_chord;
  vcl_vector<double > vcl_dpsi;

  vcl_vector<double > vcl_phi_end;
  vcl_vector<double > vcl_r_end;

  // Retrieve attribute data
  if (!this->get_attr_data("list_psi_start", vcl_psi_start) ||
    !this->get_attr_data("list_r_start", vcl_r_start) ||
    !this->get_attr_data("list_phi_start", vcl_phi_start) ||
    !this->get_attr_data("list_alpha_start", vcl_alpha_start) ||
    !this->get_attr_data("list_chord", vcl_chord) ||
    !this->get_attr_data("list_dpsi", vcl_dpsi) ||
    !this->get_attr_data("list_phi_end", vcl_phi_end) ||
    !this->get_attr_data("list_r_end", vcl_r_end))
  {
    return false;
  }

  if (method == MINMAX)
  {
    // convert to vnl vector ease / laziness of computation
    vnl_vector<double > psi_start = vcl_vector_to_vnl_vector(vcl_psi_start);
    vnl_vector<double > r_start = vcl_vector_to_vnl_vector(vcl_r_start);
    vnl_vector<double > phi_start = vcl_vector_to_vnl_vector(vcl_phi_start);

    vnl_vector<double > alpha_start = vcl_vector_to_vnl_vector(vcl_alpha_start);
    vnl_vector<double > chord = vcl_vector_to_vnl_vector(vcl_chord);
    vnl_vector<double > dpsi = vcl_vector_to_vnl_vector(vcl_dpsi);

    vnl_vector<double > phi_end = vcl_vector_to_vnl_vector(vcl_phi_end);
    vnl_vector<double > r_end = vcl_vector_to_vnl_vector(vcl_r_end);

    //>> compute min and max of psi start, noting the circular property of psi
    double min_psi_start, max_psi_start;
    dbsks_compute_angle_minmax(psi_start, min_psi_start, max_psi_start);

    this->set_param_range(min_psi_start, max_psi_start, 
      r_start.min_value(),  r_start.max_value(), 
      phi_start.min_value(), phi_start.max_value(), 
      alpha_start.min_value(), alpha_start.max_value(), 
      chord.min_value(), chord.max_value(), 
      dpsi.min_value(), dpsi.max_value(), 
      r_end.min_value(), r_end.max_value(), 
      phi_end.min_value(), phi_end.max_value(),
      this->graph_size_for_attr_data());
    return true;
  }
  else if (method == GAUSSIAN_3STD)
  {
    int num_std = 3;

    // build a new one
    pdf1d_gaussian_builder builder;
    pdf1d_pdf* model = builder.new_model();


    // psi_start - min and max
    {
      double min, max;
      vnl_vector<double > psi_start = vcl_vector_to_vnl_vector(vcl_psi_start);
      psi_start = dbsks_compute_angle_minmax(psi_start, min, max);      
      builder.build_from_array(*model, psi_start.data_block(), psi_start.size());
    }
    double min_psi_start = model->mean() - num_std * vcl_sqrt(model->variance());
    double max_psi_start = model->mean() + num_std * vcl_sqrt(model->variance());


    // r_start
    {
      vnl_vector<double > r_start = vcl_vector_to_vnl_vector(vcl_r_start);
      builder.build_from_array(*model, r_start.data_block(), r_start.size());
    }
    double min_r_start = model->mean() - num_std * vcl_sqrt(model->variance());
    double max_r_start = model->mean() + num_std * vcl_sqrt(model->variance());

    // phi_start
    {
      vnl_vector<double > phi_start = vcl_vector_to_vnl_vector(vcl_phi_start);
      builder.build_from_array(*model, phi_start.data_block(), phi_start.size());
    }
    double min_phi_start = model->mean() - num_std * vcl_sqrt(model->variance());
    double max_phi_start = model->mean() + num_std * vcl_sqrt(model->variance());

    // make sure phi is > 0 and less then pi
    min_phi_start = vnl_math_max(min_phi_start, vnl_math::pi / 36);
    max_phi_start = vnl_math_min(max_phi_start, vnl_math::pi - vnl_math::pi/36);
    min_phi_start = vnl_math_min(min_phi_start, max_phi_start);


    // Generator
    // alpha_start
    {
      vnl_vector<double > alpha_start = vcl_vector_to_vnl_vector(vcl_alpha_start);
      builder.build_from_array(*model, alpha_start.data_block(), alpha_start.size());
    }
    double min_alpha_start = model->mean() - num_std * vcl_sqrt(model->variance());
    double max_alpha_start = model->mean() + num_std * vcl_sqrt(model->variance());
    

    // chord
    {
      vnl_vector<double > chord = vcl_vector_to_vnl_vector(vcl_chord);
      builder.build_from_array(*model, chord.data_block(), chord.size());
    }
    double min_chord = model->mean() - num_std * vcl_sqrt(model->variance());
    double max_chord = model->mean() + num_std * vcl_sqrt(model->variance());

    // dpsi
    {
      vnl_vector<double > dpsi = vcl_vector_to_vnl_vector(vcl_dpsi);
      builder.build_from_array(*model, dpsi.data_block(), dpsi.size());
    }
    double min_dpsi = model->mean() - num_std * vcl_sqrt(model->variance());
    double max_dpsi = model->mean() + num_std * vcl_sqrt(model->variance());

    // phi_end
    {
      vnl_vector<double > phi_end = vcl_vector_to_vnl_vector(vcl_phi_end);
      builder.build_from_array(*model, phi_end.data_block(), phi_end.size());
    }
    double min_phi_end = model->mean() - num_std * vcl_sqrt(model->variance());
    double max_phi_end = model->mean() + num_std * vcl_sqrt(model->variance());

    // make sure phi is > 0 and less then pi
    min_phi_end = vnl_math_max(min_phi_end, vnl_math::pi / 36);
    max_phi_end = vnl_math_min(max_phi_end, vnl_math::pi - vnl_math::pi/36);
    min_phi_end = vnl_math_min(min_phi_end, max_phi_end);

        
    // r_end
    {
      vnl_vector<double > r_end = vcl_vector_to_vnl_vector(vcl_r_end);
      builder.build_from_array(*model, r_end.data_block(), r_end.size());
    }
    double min_r_end = model->mean() - num_std * vcl_sqrt(model->variance());
    double max_r_end = model->mean() + num_std * vcl_sqrt(model->variance());

    // set  min-max params
    this->set_param_range(min_psi_start, max_psi_start, 
      min_r_start,  max_r_start, 
      min_phi_start, max_phi_start, 
      min_alpha_start, max_alpha_start, 
      min_chord, max_chord, 
      min_dpsi, max_dpsi, 
      min_r_end, max_r_end, 
      min_phi_end, max_phi_end,
      this->graph_size_for_attr_data());

    // deallocate model
    delete model;
    
    return true;
  }
  else
  {
    return false;
  }
}




  
//------------------------------------------------------------------------------
//: Compute geometric constraints based on collected exemplar data
void dbsks_xfrag_geom_model::
compute_constraints_from_attr_data()
{
  this->compute_nkdiff_constraint_from_attr_data();
  this->compute_start_width_constraint_from_attr_data();
  return;
}

//------------------------------------------------------------------------------
//: Compute normalized curvature difference constraint from exemplar data
void dbsks_xfrag_geom_model::
compute_nkdiff_constraint_from_attr_data()
{
  // Compute (Gaussian) distribution of normalized curvature difference
  // for each boundary biarc
  vcl_vector<double > list_curvature_diff[2];
  this->get_attr_data("list_left_curvature_diff", list_curvature_diff[0]);
  this->get_attr_data("list_right_curvature_diff", list_curvature_diff[1]);

  vcl_vector<double > list_chord_length[2];
  this->get_attr_data("list_left_chord_length", list_chord_length[0]);
  this->get_attr_data("list_right_chord_length", list_chord_length[1]);

  assert (list_curvature_diff[0].size() == list_curvature_diff[1].size());
  assert (list_chord_length[0].size() == list_chord_length[1].size());
  assert (list_curvature_diff[0].size() == list_chord_length[0].size());

  unsigned num_samples = list_curvature_diff[0].size();
  vcl_vector<double > list_norm_curvature_diff[2];

  // construct model for each side
  for (int side = 0; side < 2; ++side)
  {
    // compute normalized curvature difference samples = length * kdiff
    vcl_vector<double >& x = list_norm_curvature_diff[side];
    vcl_vector<double >& kdiff = list_curvature_diff[side];
    vcl_vector<double >& len = list_chord_length[side];
    pdf1d_pdf& model = this->nkdiff_model_[side];

    x.resize(num_samples);
    for (unsigned i =0; i < x.size(); ++i)
    {
      x[i] = kdiff[i] *  len[i];
    }
   
    //> Build a statistical model
    pdf1d_gaussian_builder builder;
    vnl_vector<double > vnl_x = vcl_vector_to_vnl_vector(x);
    builder.build_from_array(model, vnl_x.data_block(), vnl_x.size());
  }
  return;
}








//------------------------------------------------------------------------------
//: Compute start-width constraint
void dbsks_xfrag_geom_model::
compute_start_width_constraint_from_attr_data()
{
  // Compute (Gaussian) distribution of normalized curvature difference
  // for each boundary biarc
  vcl_vector<double > list_start_width;
  this->get_attr_data("list_start_width", list_start_width);

  // build a new one
  pdf1d_gaussian_builder builder;
  vnl_vector<double > x = vcl_vector_to_vnl_vector(list_start_width);
  builder.build_from_array(this->start_width_model_, x.data_block(), x.size());
  
  return;
}


//------------------------------------------------------------------------------
//: Check "normalized curvature difference" constraint using cache values in biarc
bool dbsks_xfrag_geom_model::
check_nkdiff_constraint_no_biarc_sampler(const dbsksp_xshock_fragment& xfrag) const
{
  // check against gaussian model of kndiff
  for (int i =0; i < 2; ++i)
  {
    dbsksp_xshock_fragment::bnd_side side = dbsksp_xshock_fragment::bnd_side(i);

    dbgl_biarc biarc = xfrag.bnd_as_biarc(side);
    double kdiff = biarc.k1() - biarc.k2();
    double chord = (biarc.end() - biarc.start()).length();
    double nkdiff = kdiff * chord;
   
    // compare using pre-computed distribution
    if (!this->pass_norm_curvature_diff_constraint(side, nkdiff))
      return false;
  }
  return true;    
}







//------------------------------------------------------------------------------
//: Check constraint on start width (euclidean distance between two boundary points)
bool dbsks_xfrag_geom_model::
check_start_width_constraint(const dbsksp_xshock_node_descriptor& start, double graph_size) const
{
  double width = start.radius()*vcl_sin(start.phi()) * (this->graph_size_ / graph_size);
  double num_sd = vnl_math_abs(width-this->start_width_model_.mean())/this->start_width_model_.sd();
  return (num_sd <= this->start_width_num_std_);
}


//------------------------------------------------------------------------------
//: Check whether a fragment satisfy the geometric constaints using biarc sampler
bool dbsks_xfrag_geom_model::
check_nkdiff_constraint_using_biarc_sampler_cache(const dbsksp_xshock_fragment& xfrag) const
{
  // check against gaussian model of kndiff
  double nkdiff_left;
  bool check_left = this->biarc_sampler()->compute_nkdiff_using_cache(
    xfrag.start().bnd_pt_left(), xfrag.start().bnd_tangent_left(), 
    xfrag.end().bnd_pt_left(), xfrag.end().bnd_tangent_left(), nkdiff_left);

  if (!check_left || !this->pass_norm_curvature_diff_constraint(dbsksp_xshock_fragment::LEFT, nkdiff_left))
    return false;

  double nkdiff_right;
  bool check_right = this->biarc_sampler()->compute_nkdiff_using_cache(
    xfrag.start().bnd_pt_right(), xfrag.start().bnd_tangent_right(),
    xfrag.end().bnd_pt_right(), xfrag.end().bnd_tangent_right(), nkdiff_right);

  if (!check_right || !this->pass_norm_curvature_diff_constraint(dbsksp_xshock_fragment::RIGHT, nkdiff_right))
    return false;

  return true;    
}


//------------------------------------------------------------------------------
//: Check constraint on normalized curvature difference
bool dbsks_xfrag_geom_model::
pass_norm_curvature_diff_constraint(dbsksp_xshock_fragment::bnd_side side,
                                    double norm_curvature_diff) const
{
  // compare against mean and standard deviation
  double mu = this->nkdiff_model_[side].mean();
  double sigma = this->nkdiff_model_[side].sd();
  double num_std = vnl_math_abs(norm_curvature_diff - mu) / sigma;
  return (num_std <= this->nkdiff_threshold_);
}

