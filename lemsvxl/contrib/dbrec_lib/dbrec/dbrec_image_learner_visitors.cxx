//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 04/23/09
//
//
#include "dbrec_image_learner_visitors.h"
#include "dbrec_part_context.h"
#include "dbrec_image_models.h"
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_save.h>
#include <bsta/bsta_histogram.h>
#include <bsta/vis/bsta_svg_tools.h>
#include <bsta/algo/bsta_fit_weibull.h>

//: fits a weibull model to the current parameters
dbrec_gaussian_appearance_model_sptr dbrec_gaussian_weibull_model_learner::fit_weibull(double& k, double& lambda)
{
  if (p_sum_ > 0.0001) {  // if different from zero
    double mean = x_sum_/p_sum_; // estimate of mean
    double total_var = xsq_sum_/p_sum_; //estimate of total variance
    double var = total_var - mean*mean;
    double std_dev = vcl_sqrt(var);
    vcl_cout << "mean = " << mean << "  std_dev = " << std_dev << '\n';

    bsta_weibull_cost_function wcf(mean, std_dev);
    bsta_fit_weibull<double> fw(&wcf);
    k = 1.001;
    fw.init(k);
    fw.solve(k);
    
    vcl_cout << "Weibull k fit with residual " << fw.residual() << '\n';
    lambda = fw.lambda(k);
    
  } else {  // make it really peaked around zero if the total response was really close to zero
    vcl_cout << "p_sum = " << p_sum_ << "  x_sum = " << x_sum_ << '\n';
    vcl_cout << "Total response was very close to zero so no fitting is done, output a really peaked distribution around strength = 0.00001!\n";
    k = 1.001;
    lambda = 0.00001;
  }
  
  vcl_cout << "k = " << k << "  lambda = " << lambda << '\n';  
  dbrec_gaussian_appearance_model_sptr m = new dbrec_gaussian_weibull_appearance_model(float(lambda), float(k));
  return m;
}

//: creates a histogram from the model and writes as an svg doc
void dbrec_gaussian_weibull_model_learner::visualize_model(dbrec_gaussian_appearance_model_sptr m, const vcl_string& name, float width, float height, float margin, int font_size)
{
  //: first create a histogram
  bsta_histogram<float> h(-7.0f, 1.0f, 32);
  for (unsigned a = 0; a < h.nbins(); a++) {
    float min_val = a*h.delta() + h.min();
    float max_val = (a+1)*h.delta() + h.min();
    float prob = m->probability(vcl_pow(10.0f, min_val), vcl_pow(10.0f, max_val), 0, 0);
    h.upcount(max_val, prob);
  }
  write_svg<float>(h, name, width, height, margin, font_size);
}

//: writes the histogram as an svg doc
void dbrec_gaussian_weibull_model_learner::visualize_hist(const vcl_string& name, float width, float height, float margin, int font_size)
{
  write_svg<float>(h_, name, width, height, margin, font_size);
}

//: extracts stats from a context factory for the gaussian primitive parts of the hierarchy
void dbrec_gaussian_weibull_model_learner_visitor::visit_composition(dbrec_composition* c) 
{
  //: just visit the children
  vcl_vector<dbrec_part_sptr>& ch = c->children();
  for (unsigned i = 0; i < ch.size(); i++) {
    ch[i]->accept(this);
  }
}
void dbrec_gaussian_weibull_model_learner_visitor::visit_gaussian_primitive(dbrec_gaussian* g) 
{
  if (!data_set_) {
    vcl_cout << "In dbrec_gaussian_weibull_model_learner_visitor::visit_gaussian_primitive() -- images are not set! cannot collect stats for: " << g->type() << "\n";
    return;
  }
  dbrec_part_context_sptr pc = cf_->get_context(g->type());
  if (!pc || pc->maps_size() < 2) {
    vcl_cout << "In dbrec_gaussian_weibull_model_learner_visitor::visit_gaussian_primitive() -- the context for type: " << g->type() << " has not been created!\n";
    return;
  } 

  dbrec_gaussian_weibull_model_learner_sptr l1, l2;
  map_type::iterator it = learners_.find(g->type());
  if (it == learners_.end()) {
    l1 = new dbrec_gaussian_weibull_model_learner();
    l2 = new dbrec_gaussian_weibull_model_learner();
    learners_[g->type()] = vcl_pair<dbrec_gaussian_weibull_model_learner_sptr, dbrec_gaussian_weibull_model_learner_sptr>(l1, l2);
  } else {
    l1 = it->second.first;
    l2 = it->second.second;
  }

  //: update the stats
  vil_image_resource_sptr res_sptr = pc->get_map(0);
  vil_image_view<float> res(res_sptr->get_view());
  unsigned ni = res.ni(); unsigned nj = res.nj();
  if (ni != prob_img_.ni() || nj != prob_img_.nj() || ni != mask_img_.ni() || nj != mask_img_.nj()) {
    vcl_cout << "In dbrec_gaussian_weibull_model_learner_visitor::visit_gaussian_primitive() -- image sizes are not compatible!\n";
    return;
  }

  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (mask_img_(i,j)) {
        float op_res  = res(i,j);
        if (op_res < 1.0e-3f)
          continue;

        float prob_class = g->fg_prob_operator(prob_img_, i,j);
        float prob_non_class = 1.0f - prob_class;

        l1->update(op_res, prob_class);
        l2->update(op_res, prob_non_class);
      }
    }
    
}

void dbrec_rot_inv_gaussian_weibull_model_learner_visitor::visit_gaussian_primitive(dbrec_gaussian* g)
{
  if (!data_set_) {
    vcl_cout << "In dbrec_rot_inv_gaussian_weibull_model_learner_visitor::visit_gaussian_primitive() -- images are not set! cannot collect stats for: " << g->type() << "\n";
    return;
  }
  dbrec_part_context_sptr pc = cf_->get_context(g->type());
  if (!pc || pc->maps_size() < 3) {
    vcl_cout << "In dbrec_rot_inv_gaussian_weibull_model_learner_visitor::visit_gaussian_primitive() -- the context for type: " << g->type() << " has not been created!\n";
    return;
  } 

  dbrec_gaussian_weibull_model_learner_sptr l1, l2;
  map_type::iterator it = learners_.find(g->type());
  if (it == learners_.end()) {
    l1 = new dbrec_gaussian_weibull_model_learner();
    l2 = new dbrec_gaussian_weibull_model_learner();
    learners_[g->type()] = vcl_pair<dbrec_gaussian_weibull_model_learner_sptr, dbrec_gaussian_weibull_model_learner_sptr>(l1, l2);
  } else {
    l1 = it->second.first;
    l2 = it->second.second;
  }

  //: update the stats
  vil_image_resource_sptr res_sptr = pc->get_map(0);
  vil_image_view<float> res(res_sptr->get_view());
  vil_image_resource_sptr res_angle_sptr = pc->get_map(2);
  vil_image_view<float> res_angle(res_angle_sptr->get_view());
  unsigned ni = res.ni(); unsigned nj = res.nj();
  if (ni != prob_img_.ni() || nj != prob_img_.nj() || ni != mask_img_.ni() || nj != mask_img_.nj()) {
    vcl_cout << "In dbrec_gaussian_weibull_model_learner_visitor::visit_gaussian_primitive() -- image sizes are not compatible!\n";
    return;
  }

  for (unsigned j = 0; j<nj; ++j)
    for (unsigned i = 0; i<ni; ++i)
    {
      if (mask_img_(i,j)) {
        float op_res  = res(i,j);
        if (op_res < 1.0e-3f)
          continue;

        float angle = res_angle(i,j);
        float prob_class = g->fg_prob_operator_rotational(prob_img_, i, j, angle);
        float prob_non_class = 1.0f - prob_class;

        l1->update(op_res, prob_class);
        l2->update(op_res, prob_non_class);
      }
    }
}
//: same as dbrec_gaussian_weibull_model_learner_visitor::print_current_models(), but it prints the model parameters for each orientation
void dbrec_rot_inv_gaussian_weibull_model_learner_visitor::print_current_models_at_each_orientation(const vcl_string& prefix, float angle_inc)
{
  for (map_type::iterator it = learners_.begin(); it != learners_.end(); it++) {
    dbrec_gaussian_weibull_model_learner_sptr l1 = it->second.first;
    dbrec_gaussian_weibull_model_learner_sptr l2 = it->second.second;
    dbrec_part_sptr p = h_->get_part(it->first);
    vcl_string str_id = p->string_identifier();
    vcl_string name = prefix + str_id + "_class_model.svg";
    vcl_cout << "------------ " << str_id << " fitting class model: -----\n";
    double k1, lambda1;
    dbrec_gaussian_appearance_model_sptr m = l1->fit_weibull(k1, lambda1);
    vcl_cout << "---------------------------------------------\n";
    dbrec_gaussian_weibull_model_learner::visualize_model(m,name);
    name = prefix + str_id + "_non_class_model.svg";
    vcl_cout << "------------ " << str_id << " fitting non-class model: -----\n";
    double k2, lambda2;
    dbrec_gaussian_appearance_model_sptr m2 = l2->fit_weibull(k2, lambda2);
    vcl_cout << "---------------------------------------------\n";
    dbrec_gaussian_weibull_model_learner::visualize_model(m2,name);
    //: also write the parameters to output text file
    vcl_string text_file = prefix + str_id + "_fg_params_dbrec.txt";
    vcl_ofstream of(text_file.c_str());
    of << k1 << " " << lambda1 << vcl_endl;
    of << k2 << " " << lambda2 << vcl_endl;
    of.close();

    dbrec_gaussian* pg = dynamic_cast<dbrec_gaussian*>(p.ptr());
    for (float angle = 0.0f; angle < 180.0f; angle += angle_inc) {
      dbrec_gaussian g_a(0, pg->lambda0_, pg->lambda1_, angle, pg->bright_);
      vcl_string text_file = prefix + g_a.string_identifier() + "_fg_params_dbrec.txt";
      vcl_ofstream of_a(text_file.c_str());
      of_a << k1 << " " << lambda1 << vcl_endl;
      of_a << k2 << " " << lambda2 << vcl_endl;
      of_a.close();
    }
  }
}

void dbrec_gaussian_weibull_model_learner_visitor::print_current_histograms(const vcl_string& prefix)
{
  for (map_type::iterator it = learners_.begin(); it != learners_.end(); it++) {
    dbrec_gaussian_weibull_model_learner_sptr l1 = it->second.first;
    dbrec_gaussian_weibull_model_learner_sptr l2 = it->second.second;
    dbrec_part_sptr p = h_->get_part(it->first);
    vcl_string str_id = p->string_identifier();
    vcl_string name = prefix + "_" + str_id + "_class.svg";
    l1->visualize_hist(name);
    name = prefix + "_" + str_id + "_non_class.svg";
    l2->visualize_hist(name);
  }
}
void dbrec_gaussian_weibull_model_learner_visitor::print_current_models(const vcl_string& prefix)
{
  for (map_type::iterator it = learners_.begin(); it != learners_.end(); it++) {
    dbrec_gaussian_weibull_model_learner_sptr l1 = it->second.first;
    dbrec_gaussian_weibull_model_learner_sptr l2 = it->second.second;
    dbrec_part_sptr p = h_->get_part(it->first);
    vcl_string str_id = p->string_identifier();
    vcl_string name = prefix + str_id + "_class_model.svg";
    vcl_cout << "------------ " << str_id << " fitting class model: -----\n";
    double k1, lambda1;
    dbrec_gaussian_appearance_model_sptr m = l1->fit_weibull(k1, lambda1);
    vcl_cout << "---------------------------------------------\n";
    dbrec_gaussian_weibull_model_learner::visualize_model(m,name);
    name = prefix + str_id + "_non_class_model.svg";
    vcl_cout << "------------ " << str_id << " fitting non-class model: -----\n";
    double k2, lambda2;
    dbrec_gaussian_appearance_model_sptr m2 = l2->fit_weibull(k2, lambda2);
    vcl_cout << "---------------------------------------------\n";
    dbrec_gaussian_weibull_model_learner::visualize_model(m2,name);
    //: also write the parameters to output text file
    vcl_string text_file = prefix + str_id + "_fg_params_dbrec.txt";
    vcl_ofstream of(text_file.c_str());
    of << k1 << " " << lambda1 << vcl_endl;
    of << k2 << " " << lambda2 << vcl_endl;
  }
}

//: finds mu and sigma images for each primitive part and writes them to model folder, does nothing for the compositions, just passes the composition to children
void dbrec_image_construct_bg_model_visitor::visit_composition(dbrec_composition* c)
{
  //: just visit the children
  vcl_vector<dbrec_part_sptr>& ch = c->children();
  for (unsigned i = 0; i < ch.size(); i++) {
    ch[i]->accept(this);
  }
}
void dbrec_image_construct_bg_model_visitor::visit_gaussian_primitive(dbrec_gaussian* g)
{
  //: first compute the bg mu and sigma images for this primitive
  vil_image_view<float> mu_img(mean_img_.ni(), mean_img_.nj());
  vil_image_view<float> sigma_img(mean_img_.ni(), mean_img_.nj());
  g->construct_bg_response_model_gauss(mean_img_, std_dev_img_, mu_img, sigma_img);
  
  //: write the images out to the folder
  vcl_string name = model_folder_+g->string_identifier()+"_bg_mu_img_dbrec.tiff";
  bool result = vil_save(mu_img, name.c_str());
  if ( !result ) {
    vcl_cerr << "Failed to save image to " << name << vcl_endl;
    return;
  }

  name = model_folder_+g->string_identifier()+"_bg_sigma_img_dbrec.tiff";
  result = vil_save(sigma_img,name.c_str());
  if ( !result ) {
    vcl_cerr << "Failed to save image to " << name << vcl_endl;
    return;
  }
}

void dbrec_image_construct_bg_model_rot_inv_visitor::visit_gaussian_primitive(dbrec_gaussian* g) 
{
  vil_image_view<float> mu_img(mean_img_.ni(), mean_img_.nj());
  vil_image_view<float> sigma_img(mean_img_.ni(), mean_img_.nj());

  if (g->lambda0_ == g->lambda1_) {
    if (g->theta_ != 0.0) {
      vcl_cout << "ERROR: In dbrec_image_construct_bg_model_rot_inv_visitor::visit_gaussian_primitive() -- the primitive is isotropic but its angle is not set to be zero!!\n";
      throw 0;
    }
    dbrec_image_construct_bg_model_visitor::visit_gaussian_primitive(g);
    return;
  }
  
  for (float angle = 0.0f; angle < 180.0f; angle += angle_inc_) {
    mu_img.fill(0.0f); sigma_img.fill(0.0f);
    dbrec_gaussian g_a(0, g->lambda0_, g->lambda1_, angle, g->bright_);
    g_a.construct_bg_response_model_gauss(mean_img_, std_dev_img_, mu_img, sigma_img);
    //: write the images out to the folder
    vcl_string name = model_folder_+g_a.string_identifier()+"_bg_mu_img_dbrec.tiff";
    bool result = vil_save(mu_img, name.c_str());
    if ( !result ) {
      vcl_cerr << "Failed to save image to " << name << vcl_endl;
      return;
    }

    name = model_folder_+g_a.string_identifier()+"_bg_sigma_img_dbrec.tiff";
    result = vil_save(sigma_img,name.c_str());
    if ( !result ) {
      vcl_cerr << "Failed to save image to " << name << vcl_endl;
      return;
    }
  }
}



  
