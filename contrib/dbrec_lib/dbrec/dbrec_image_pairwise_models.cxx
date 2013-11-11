//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 04/27/09
//
//
#include "dbrec_image_pairwise_models.h"
#include "dbrec_part.h"
#include "dbrec_image_visitors.h"
#include "dbrec_image_compositor.h"

#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_cross_product_matrix.h>
#include <vnl/vnl_double_3.h>
#include <bsta/algo/bsta_gaussian_updater.h>
#include <bxml/bxml_find.h>
#include <bsta/vis/bsta_svg_tools.h>



//: Concrete classes for pairwise modeling


void sample_location_helper(const vgl_point_2d<float>& loc, dbrec_part_sptr first_part, vgl_point_2d<float>& second_loc, float dist, float angle)
{
  dbrec_get_direction_vector_visitor dv;
  first_part->accept(&dv);
  vnl_vector_fixed<float, 2> v = dv.v_;
  
  //: define a rotation about z axis (in the image plane)
  vnl_quaternion<float> q(0.0f, 0.0f, angle);
  vnl_vector_fixed<float,3> v3d(v[0], v[1], 0.0f);
  vnl_vector_fixed<float,3> out = q.rotate(v3d);
  vnl_vector_fixed<float,3> out_dist = out*dist;
  second_loc = vgl_point_2d<float>(loc.x() + out_dist[0], loc.y() + out_dist[1]);
}

//: independent gaussian models for distance and angle
void dbrec_pairwise_indep_gaussian_model::sample_location(const vgl_point_2d<float>& loc, float scale, dbrec_part_sptr first_part, vgl_point_2d<float>& second_loc) const
{
  float dist = (float)dist_model_.sample(dbrec_type_id_factory::instance()->rng());
  float angle = (float)angle_model_.sample(dbrec_type_id_factory::instance()->rng());
  vcl_cout << "indep gaussian sampled distance: " << dist << " (scaled dist: " << dist*scale << ") angle: " << angle << " (" << angle/vnl_math::pi*180.0f << ")\n";
  sample_location_helper(loc, first_part, second_loc, dist*scale, angle);
}

void get_distance_and_angle(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vnl_vector_fixed<float,2>& central_part_loc, const vnl_vector_fixed<float,2>& second_part_loc, float& angle, float& dist)
{
  //: first find difference to center vector
  vnl_vector_fixed<float,2> dif_to_center = second_part_loc - central_part_loc; 

  dist = (float)dif_to_center.magnitude();
  vnl_vector_fixed<float, 2> v1_hat = dif_to_center.normalize();
  angle = (float)vcl_acos(dot_product(central_part_direction_vector, v1_hat));

  //: if angle is ~ 180 degrees return a positive angle, otherwise negate the angle
  if (!(vcl_abs(angle-vnl_math::pi) < 0.17) && !(angle < 0.17))  // allow for a 10 degree interval around 180 degrees and 0 degree
  {
    //: now we want this angle positive or negative, depending on which side of v does v1 lie
    vnl_double_3 v_3(central_part_direction_vector[0], central_part_direction_vector[1], 0.0);
    vnl_double_3 v1_hat_3(v1_hat[0], v1_hat[1], 0.0);
    vnl_double_3x3 V = vnl_cross_product_matrix(v_3);
    vnl_double_3 v_v1_hat = V*v1_hat_3;
    if (v_v1_hat[2] < 0)
      angle = -angle;  // negate the angle
  } 
}
void get_distance_and_angle(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vgl_point_2d<float>& central_part_loc, const vgl_point_2d<float>& second_part_loc, float& angle, float& dist)
{
  vnl_vector_fixed<float,2> c_p_loc(central_part_loc.x(), central_part_loc.y());
  vnl_vector_fixed<float,2> s_p_loc(second_part_loc.x(), second_part_loc.y());
  get_distance_and_angle(central_part_direction_vector, c_p_loc, s_p_loc, angle, dist);   
}


void dbrec_pairwise_indep_gaussian_model::update_models(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vnl_vector_fixed<float,2>& central_part_loc, const vnl_vector_fixed<float,2>& second_part_loc, float central_part_mag, float second_part_mag)
{
  float angle, dist;
  get_distance_and_angle(central_part_direction_vector, central_part_loc, second_part_loc, angle, dist);

  //: update the models
  bsta_update_gaussian(dist_model_, 1.0f, dist, min_stad_dev_dist_*min_stad_dev_dist_);
  float min_stad_dev = (float)(vnl_math::pi*(min_stad_dev_angle_/180.0f));  // min stad_dev = 10 degrees
  bsta_update_gaussian(angle_model_, 1.0f, angle, min_stad_dev*min_stad_dev);
}

//: create a probe box using the model given the first part's location
vgl_box_2d<float> dbrec_pairwise_indep_gaussian_model::get_probe_box(const vgl_point_2d<float>& loc, const vnl_vector_fixed<float, 2>& v) const
{
  float cx = loc.x(); float cy = loc.y();

  //: define a rotation about z axis (in the image plane)
  double mean_angle = angle_model_.mean(); //this->mean_angle();
  double var_angle = angle_model_.var(); // this->var_angle();
  vnl_quaternion<float> q(0.0f, 0.0f, float(mean_angle-var_angle));

  double mean_dist = dist_model_.mean(); // this->mean_dist();
  double var_dist = dist_model_.var(); 
  vnl_vector_fixed<float,3> v3d(v[0], v[1], 0.0f);
  vnl_vector_fixed<float,3> out = q.rotate(v3d);
  vnl_vector_fixed<float,3> out_dist = out*float(mean_dist);

  float mx = cx + out_dist[0];
  float my = cy + out_dist[1];
  float rad = (float)vcl_ceil(vcl_sqrt(var_dist)+3);
  float si = mx - rad;
  float upper_i = mx + rad;
  float sj = my - rad;
  float upper_j = my + rad;

  vgl_point_2d<float> pr0(si, sj), pr1(si, upper_j), pr2(upper_i, upper_j), pr3(upper_i, sj);
  vgl_box_2d<float> probe;
  probe.add(pr0); probe.add(pr1); probe.add(pr2); probe.add(pr3);

  //: create these boxes for each var_angle() and take union of all boxes
  vnl_quaternion<float> q2(0.0f, 0.0f, float(mean_angle+var_angle));
  vnl_vector_fixed<float,3> out2 = q2.rotate(v3d);
  vnl_vector_fixed<float,3> out_dist2 = out2*float(mean_dist);

  mx = cx + out_dist2[0];
  my = cy + out_dist2[1];
  si = mx - rad;
  upper_i = mx + rad;
  sj = my - rad;
  upper_j = my + rad;
  pr0.set(si, sj); pr1.set(si, upper_j); pr2.set(upper_i, upper_j); pr3.set(upper_i, sj);
  probe.add(pr0); probe.add(pr1); probe.add(pr2); probe.add(pr3);

  return probe;
}

double dbrec_pairwise_indep_gaussian_model::prob_density(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vgl_point_2d<float>& central_part_loc, const vgl_point_2d<float>& second_part_loc) const
{
  float angle, dist;
  get_distance_and_angle(central_part_direction_vector, central_part_loc, second_part_loc, angle, dist);   
  
  //: return non-normalized value
  //return float(loc_model_.var()*2*vnl_math::pi*loc_model_.prob_density(pt));
  double d_dens = dist_model_.prob_density(dist);
  double d_var = dist_model_.var();
  d_dens = d_var*2.0f*vnl_math::pi*d_dens;

  double a_dens = angle_model_.prob_density(angle);
  double a_var = angle_model_.var();
  a_dens = a_var*2.0f*(vnl_math::pi)*a_dens;

  //return weight_*d_dens*a_dens;
  return d_dens*a_dens;  // not using weighted density (weights were given by mean-shit mode finding for 1D angle and distance distributions separately)
}

//: return self as a bxml_data_sptr
bxml_data_sptr dbrec_pairwise_indep_gaussian_model::xml_element() const
{
  bxml_element* data = new bxml_element("pairwise_indep_gaussian_model");
  data->set_attribute("min_stad_dev_dist", min_stad_dev_dist_);
  data->set_attribute("min_stad_dev_angle", min_stad_dev_angle_);
  data->set_attribute("dist_mean", dist_model_.mean());
  data->set_attribute("dist_var", dist_model_.var());
  data->set_attribute("angle_mean", angle_model_.mean());
  data->set_attribute("angle_var", angle_model_.var());
  data->append_text("\n");
  return data;
}
//: read self from the bxml_data_sptr
dbrec_pairwise_model_sptr dbrec_pairwise_indep_gaussian_model::xml_parse_element(bxml_data_sptr data)
{
  dbrec_pairwise_model_sptr comp;
  bxml_element query("pairwise_indep_gaussian_model");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);
  if (!base_root)
    return comp;
  bxml_element* r_elm = dynamic_cast<bxml_element*>(base_root.ptr());
  float min_stad_dev_dist, min_stad_dev_angle, mean, var;
  bsta_gaussian_sphere<double, 1> dist_model, angle_model;
  
  r_elm->get_attribute("min_stad_dev_dist", min_stad_dev_dist);
  r_elm->get_attribute("min_stad_dev_angle", min_stad_dev_angle);

  r_elm->get_attribute("dist_mean", mean);
  r_elm->get_attribute("dist_var", var);
  dist_model.set_mean(mean); dist_model.set_var(var);

  r_elm->get_attribute("angle_mean", mean);
  r_elm->get_attribute("angle_var", var);
  angle_model.set_mean(mean); angle_model.set_var(var);
  
  comp = new dbrec_pairwise_indep_gaussian_model(dist_model, angle_model, min_stad_dev_dist, min_stad_dev_angle);
  return comp;
}


//: independent uniform models for distance and angle
void dbrec_pairwise_indep_uniform_model::sample_location(const vgl_point_2d<float>& loc, float scale, dbrec_part_sptr first_part, vgl_point_2d<float>& second_loc) const
{
  //: sample a distance from uniform distribution
  float dist = min_d_ + float(dbrec_type_id_factory::instance()->random())*(max_d_ - min_d_);
  
  //: sample a distance from uniform distribution
  float angle = min_angle_ + float(dbrec_type_id_factory::instance()->random())*(max_angle_ - min_angle_);
  vcl_cout << "indep uniform sampled distance: " << dist << " angle: " << angle << " (" << angle/vnl_math::pi*180.0f << ")\n";
  sample_location_helper(loc, first_part, second_loc, dist*scale, angle);
}

double dbrec_pairwise_indep_uniform_model::prob_density(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vgl_point_2d<float>& central_part_loc, const vgl_point_2d<float>& second_part_loc) const
{
  vcl_cout << "In dbrec_pairwise_indep_uniform_model::prob_density() -- not implemented for this model!\n";
  throw 0;
}
//: return self as a bxml_data_sptr
bxml_data_sptr dbrec_pairwise_indep_uniform_model::xml_element() const
{
  bxml_element* data = new bxml_element("pairwise_indep_uniform_model");
  data->set_attribute("min_d", min_d_);
  data->set_attribute("max_d", max_d_);
  data->set_attribute("min_angle", min_angle_);
  data->set_attribute("max_angle", max_angle_);
  data->append_text("\n");
  return data;
}
//: read self from the bxml_data_sptr
dbrec_pairwise_model_sptr dbrec_pairwise_indep_uniform_model::xml_parse_element(bxml_data_sptr data)
{
  dbrec_pairwise_model_sptr comp;
  bxml_element query("pairwise_indep_uniform_model");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);
  if (!base_root)
    return comp;
  bxml_element* r_elm = dynamic_cast<bxml_element*>(base_root.ptr());
  float min_d, max_d, min_angle, max_angle;
  r_elm->get_attribute("min_d", min_d);
  r_elm->get_attribute("max_d", max_d);
  r_elm->get_attribute("min_angle", min_angle);
  r_elm->get_attribute("max_angle", max_angle);
  
  comp = new dbrec_pairwise_indep_uniform_model(min_d, max_d, min_angle, max_angle);
  return comp;
}

//: return a box with 2*radius width centered at the central_part_loc
vgl_box_2d<float> dbrec_pairwise_indep_uniform_discrete_model::get_probe_box(const vgl_point_2d<float>& central_part_loc, const vnl_vector_fixed<float, 2>& central_part_direction_vector) const
{
  return this->get_probe_box(central_part_loc);
}
vgl_box_2d<float> dbrec_pairwise_indep_uniform_discrete_model::get_probe_box(const vgl_point_2d<float>& central_part_loc) const
{
  float cx = central_part_loc.x(); float cy = central_part_loc.y();
  float mx = cx + radius_;
  float my = cy + radius_;
  float minx = cx-radius_;
  float miny = cy-radius_;
  vgl_point_2d<float> pr0(mx, my), pr1(minx, miny);
  vgl_box_2d<float> probe;
  probe.add(pr0); probe.add(pr1); 
  return probe;
}

//: create a probe box using the model given the first part's location
vgl_box_2d<float> dbrec_pairwise_indep_uniform_model::get_probe_box(const vgl_point_2d<float>& loc, const vnl_vector_fixed<float, 2>& v) const
{
  vcl_cout << "In dbrec_pairwise_indep_uniform_model::get_probe_box() -- not implemented for this model!\n";
  throw 0;
}
//: create a probe box using the model given the first part's location
vgl_box_2d<float> dbrec_pairwise_2d_gaussian_model::get_probe_box(const vgl_point_2d<float>& loc, const vnl_vector_fixed<float, 2>& v) const
{
  vcl_cout << "In dbrec_pairwise_gaussian_model::get_probe_box() -- not implemented for this model!\n";
  throw 0;
}

//: return self as a bxml_data_sptr
bxml_data_sptr dbrec_pairwise_indep_uniform_discrete_model::xml_element() const
{
  bxml_element* data = new bxml_element("pairwise_indep_uniform_discrete_model");
  data->set_attribute("radius", radius_);
  data->append_text("\n");
  return data;
}
//: read self from the bxml_data_sptr
dbrec_pairwise_model_sptr dbrec_pairwise_indep_uniform_discrete_model::xml_parse_element(bxml_data_sptr data)
{
  dbrec_pairwise_model_sptr comp;
  bxml_element query("pairwise_indep_uniform_discrete_model");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);
  if (!base_root)
    return comp;
  bxml_element* r_elm = dynamic_cast<bxml_element*>(base_root.ptr());
  float rad;
  r_elm->get_attribute("radius", rad);
  comp = new dbrec_pairwise_indep_uniform_discrete_model(rad);
  return comp;
}

// implement discrete 2D prob dist methods

//: sample location of the second wrt first's given location
void dbrec_pairwise_discrete_model::sample_location(const vgl_point_2d<float>& loc, float scale, dbrec_part_sptr first_part, vgl_point_2d<float>& second_loc) const
{
  //: use the bsta sampler to sample from a 2D discrete prob distribution given as a joint histogram
  vcl_vector<vcl_pair<float, float> > out;
  //bool done = bsta_sampler<unsigned>::sample(hist_, 1, out);
  bool done = bsta_sampler<unsigned>::sample_in_likelihood_order(hist_, 1, out);
  if (!done || !out.size()) {
    vcl_cout << "In dbrec_pairwise_discrete_model::sample_location() -- sampling problem!\n";
    throw 0;
  }

  float angle = out[0].first;
  float dist = out[0].second;
  vcl_cout << "pairwise discrete model sampled distance: " << dist << " (scaled dist: " << dist*scale << ") angle: " << angle << " (" << angle/vnl_math::pi*180.0f << ")\n";
  sample_location_helper(loc, first_part, second_loc, dist*scale, angle);
}

//: create a probe box using the model given the first part's location
//  with this model, we don't have a well defined mean angle and distance, the objects could be anywhere around the central part
//  in a certain radius, so we'll create a box large enough to contain max-radius search region
//  performance-wise this will be worse since it will return a lot of hits for a given query box but there is no escape from it
vgl_box_2d<float> dbrec_pairwise_discrete_model::get_probe_box(const vgl_point_2d<float>& central_part_loc, const vnl_vector_fixed<float, 2>& central_part_direction_vector) const
{
  float dist_range = hist_.range_b();
  float cx = central_part_loc.x(); float cy = central_part_loc.y();

  float mx = cx + dist_range;
  float my = cy + dist_range;
  float minx = cx-dist_range;
  float miny = cy-dist_range;
  
  vgl_point_2d<float> pr0(mx, my), pr1(minx, miny);
  vgl_box_2d<float> probe;
  probe.add(pr0); probe.add(pr1); 
  return probe;
}

double dbrec_pairwise_discrete_model::prob_density(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vgl_point_2d<float>& central_part_loc, const vgl_point_2d<float>& second_part_loc) const
{
  float angle, dist;
  get_distance_and_angle(central_part_direction_vector, central_part_loc, second_part_loc, angle, dist);   
  //: return the discrete prob value given by the histogram
  return hist_.p(angle, dist);
}

void dbrec_pairwise_discrete_model::update_models(const vnl_vector_fixed<float, 2>& central_part_direction_vector, 
    const vnl_vector_fixed<float,2>& central_part_loc, const vnl_vector_fixed<float,2>& second_part_loc, float central_part_mag, float second_part_mag)
{
  float angle, dist;
  get_distance_and_angle(central_part_direction_vector, central_part_loc, second_part_loc, angle, dist);

  //: update the histogram
  float weight = central_part_mag*second_part_mag;
  hist_.upcount(angle, weight, dist, weight);
}

//: return self as a bxml_data_sptr
bxml_data_sptr dbrec_pairwise_discrete_model::xml_element() const
{
  bxml_element* data = new bxml_element("pairwise_discrete_model");
  //unsigned int nbins_a, float min_dist, float max_dist, unsigned int nbins_dist
  data->set_attribute("nbins_a", hist_.nbins_a());
  data->set_attribute("min_d", hist_.min_b());
  data->set_attribute("max_d", hist_.max_b());
  data->set_attribute("nbins_d", hist_.nbins_b());
  data->append_text("\n");

  //: dump the data of the histogram as a different data element
  bxml_element* hist_data = new bxml_element("hist_data");
  hist_data->append_text("\n");
  vcl_stringstream ss;
  vbl_array_2d<float> count_arr = hist_.counts();
  for (unsigned na = 0; na < hist_.nbins_a(); na++) {
    for (unsigned nb = 0; nb < hist_.nbins_b(); nb++) {
      ss << count_arr(na, nb) << " ";
    }
    ss << "\n";
  }
  hist_data->append_text(ss.str());
  data->append_data(hist_data);
  data->append_text("\n");
  return data;
}
//: read self from the bxml_data_sptr
dbrec_pairwise_model_sptr dbrec_pairwise_discrete_model::xml_parse_element(bxml_data_sptr data)
{
  dbrec_pairwise_model_sptr comp;
  bxml_element query("pairwise_discrete_model");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);
  if (!base_root)
    return comp;
  bxml_element* r_elm = dynamic_cast<bxml_element*>(base_root.ptr());
  unsigned nbins_a, nbins_d; float min_d, max_d;
  r_elm->get_attribute("nbins_a", nbins_a);
  r_elm->get_attribute("min_d", min_d);
  r_elm->get_attribute("max_d", max_d);
  r_elm->get_attribute("nbins_d", nbins_d);

  bsta_joint_histogram<float> hist((float)(-vnl_math::pi), (float)(vnl_math::pi), nbins_a, min_d, max_d, nbins_d);
  
  bxml_element hquery("hist_data");
  bxml_data_sptr hist_root = bxml_find_by_name(base_root, hquery);
  if (!hist_root)
    return comp;

  bxml_element* hist_data = dynamic_cast<bxml_element*>(hist_root.ptr());
  for (bxml_element::const_data_iterator it = hist_data->data_begin(); it != hist_data->data_end(); it++) {
    if ((*it)->type() != bxml_element::TEXT)
      continue;
    bxml_text* ht = dynamic_cast<bxml_text*>((*it).ptr());
    vcl_stringstream ss(ht->data());
    for (unsigned na = 0; na < nbins_a; na++) {
      for (unsigned nd = 0; nd < nbins_d; nd++) {
        float val; ss >> val;
        hist.set_count(na, nd, val); 
      }
    }
  }
  
  comp = new dbrec_pairwise_discrete_model(hist);
  return comp;
}

//: visualize the model 
void dbrec_pairwise_discrete_model::visualize(vcl_string& doc_name) const
{
  vcl_string doc_name2 = doc_name + ".vrml";
  vcl_ofstream os(doc_name2.c_str());
  hist_.print_to_vrml(os);
  os.close();

  vcl_string doc_name3 = doc_name + ".svg";
  write_svg_angle_distance(hist_, doc_name3); 
}

void dbrec_pairwise_rot_invariant_discrete_model::visualize(vcl_string& doc_name) const
{
  vcl_stringstream ns;
  if (gamma_defined_) 
    ns << "gmin_" << gamma_min_ << "_gmax_" << gamma_max_ << "_gr_" << gamma_range_;
  if (rho_defined_)
    ns << "_rmin_" << (int)class_hist_.min_a() << "_rmax_" << (int)class_hist_.max_a() << "_r_int_" << (int)class_hist_.delta_a();
  ns << "_dmin_" << (int)class_hist_.min_b() << "_dmax_" << (int)class_hist_.max_b() << "_d_int_" << (int)class_hist_.delta_b();
  
  vcl_stringstream ent_ns; ent_ns << "_ent_" << class_hist_.entropy() << "_vol_" << class_hist_.volume();
  vcl_stringstream ent_ns_nc; ent_ns_nc << "_ent_" << non_class_hist_.entropy() << "_vol_" << non_class_hist_.volume();

  vcl_string doc_name2 = doc_name + "_"+ns.str()+ent_ns.str()+"_class_hist.vrml";
  vcl_ofstream os(doc_name2.c_str());
  class_hist_.print_to_vrml(os);
  os.close();
  vcl_string doc_name3 = doc_name + "_"+ns.str()+ent_ns_nc.str()+"_non_class_hist.vrml";
  vcl_ofstream os3(doc_name3.c_str());
  non_class_hist_.print_to_vrml(os3);
  os3.close();
}

//: create new instances of self by sampling from the histogram
bool dbrec_pairwise_discrete_model::sample_new_instances(unsigned n, vcl_vector<dbrec_pairwise_model_sptr>& new_ins)
{
  vcl_vector<vcl_pair<unsigned, unsigned> > out_indices;
  if (!bsta_sampler<unsigned>::sample_in_likelihood_order(hist_, n, out_indices))
    return false;
 
  for (unsigned i = 0; i < out_indices.size(); i++) {
    bsta_joint_histogram<float> hist(hist_.min_a(), hist_.max_a(), hist_.nbins_a(), hist_.min_b(), hist_.max_b(), hist_.nbins_b()); 
    hist.set_count(out_indices[i].first, out_indices[i].second, hist_.get_count(out_indices[i].first, out_indices[i].second));
    dbrec_pairwise_model_sptr m = new dbrec_pairwise_discrete_model(hist);
    new_ins.push_back(m);
  }
  return true;
}

//: create new instances of indep gaussian models by sampling from the histogram
bool dbrec_pairwise_discrete_model::sample_new_indep_gaussian_instances(unsigned n, vcl_vector<dbrec_pairwise_model_sptr>& new_ins)
{
  vcl_vector<vcl_pair<float, float> > out_values;
  if (!bsta_sampler<unsigned>::sample_in_likelihood_order(hist_, n, out_values))
    return false;
 
  float delta_a = hist_.delta_a(); delta_a /= 2.0f;
  float delta_d = hist_.delta_b(); delta_d /= 2.0f;
  for (unsigned i = 0; i < out_values.size(); i++) {
    bsta_gaussian_sphere<double, 1> dist_model, angle_model;  
    float a = out_values[i].first - delta_a; // this is the center value of the bin
    float d = out_values[i].second - delta_d; 
    angle_model.set_mean(a); angle_model.set_var(delta_a*delta_a);
    dist_model.set_mean(d); dist_model.set_var(delta_d*delta_d);
    dbrec_pairwise_model_sptr m = new dbrec_pairwise_indep_gaussian_model(dist_model, angle_model, 1.0f, 10.0f);
    new_ins.push_back(m);
  }
  return true;
}

dbrec_pairwise_model_sptr dbrec_pairwise_indep_gaussian_model_factory::generate_model() const { 
  return new dbrec_pairwise_indep_gaussian_model(min_stad_dev_dist_, min_stad_dev_angle_); 
}
dbrec_pairwise_model_sptr dbrec_pairwise_discrete_model_factory::generate_model() const { 
  return new dbrec_pairwise_discrete_model(nbins_a_, min_dist_, max_dist_, nbins_dist_); 
}

int dbrec_pairwise_rot_invariant_model::sample_d(vnl_random& rng) const {
  return rng.lrand32(d_min_, d_max_-1);  // interval is [d_min,d_max) so sample from [d_min, d_max-1]
}
int dbrec_pairwise_rot_invariant_model::sample_rho(vnl_random& rng) const {
  if (rho_min_ > rho_max_) {  // the interval wraps around
    if (rng.drand32() < 0.5)
      return rng.lrand32(rho_min_, rho_range_-1);
    else 
      return rng.lrand32(0, rho_max_-1);
  } else
    return rng.lrand32(rho_min_, rho_max_-1);  // interval is [rho_min,rho_max) so sample from [rho_min, rho_max-1]
}
int dbrec_pairwise_rot_invariant_model::sample_gamma(vnl_random& rng) const {
  if (gamma_min_ > gamma_max_) {  // the interval wraps around
    if (rng.drand32() < 0.5)
      return rng.lrand32(gamma_min_, gamma_range_-1);
    else
      return rng.lrand32(0, gamma_max_-1);
  } else 
    return rng.lrand32(gamma_min_, gamma_max_-1); // interval is [gamma_min,gamma_max) so sample from [gamma_min, gamma_max-1]
}

double measure_angle_clockwise(vnl_vector_fixed<float, 2>& dir1, vnl_vector_fixed<float, 2>& dir2) {
  //: first find angle using the dot product
  double angle_rad = vcl_acos(dot_product(dir1, dir2));
  //: if cross_product is along positive z axis, then use the angle as is otherwise use (2*pi - angle)
  vnl_double_3 v_3(dir1[0], dir1[1], 0.0);
  vnl_double_3 v1_hat_3(dir2[0], dir2[1], 0.0);
  vnl_double_3x3 V = vnl_cross_product_matrix(v_3);
  vnl_double_3 v_v1_hat = V*v1_hat_3;
  if (v_v1_hat[2] < 0)
    angle_rad = 2*vnl_math::pi-angle_rad;  
  return angle_rad;
}

int dbrec_pairwise_rot_invariant_model::measure_gamma(dbrec_part_ins_sptr p1_ins, dbrec_part_ins_sptr p2_ins) {
  vnl_vector_fixed<float, 2> dir1 = p1_ins->dir_vector();
  vnl_vector_fixed<float, 2> dir2 = p2_ins->dir_vector();
  double angle_rad = measure_angle_clockwise(dir1, dir2);
  int angle_degree = (int)((angle_rad/vnl_math::pi)*180);
  return angle_degree;
}
int dbrec_pairwise_rot_invariant_model::measure_rho(dbrec_part_ins_sptr p1_ins, dbrec_part_ins_sptr p2_ins) {
  vnl_vector_fixed<float, 2> p1_c(p1_ins->pt().x(), p1_ins->pt().y());
  vnl_vector_fixed<float, 2> p2_c(p2_ins->pt().x(), p2_ins->pt().y());
  vnl_vector_fixed<float, 2> dir1 = p1_ins->dir_vector();
  vnl_vector_fixed<float, 2> dif_vec = p2_c - p1_c;
  dif_vec = dif_vec.normalize();  // find the unit vector in this direction
  double angle_rad = measure_angle_clockwise(dir1, dif_vec);
  int angle_degree = (int)((angle_rad/vnl_math::pi)*180);
  return angle_degree;
}
int dbrec_pairwise_rot_invariant_model::measure_d(dbrec_part_ins_sptr p1_ins, dbrec_part_ins_sptr p2_ins) {
  vnl_vector_fixed<float, 2> p1_c(p1_ins->pt().x(), p1_ins->pt().y());
  vnl_vector_fixed<float, 2> p2_c(p2_ins->pt().x(), p2_ins->pt().y());
  float d = (p1_c-p2_c).magnitude();
  int d_i = (int)vcl_floor(d+0.5f);
  return d_i;
}

double dbrec_pairwise_rot_invariant_model::prob_density(dbrec_part_ins_sptr p1_ins, dbrec_part_ins_sptr p2_ins) const
{
  double gamma_prob = 1.0;
  if (gamma_range_ > 0) { //: if gamma is defined for these two parts, compute it
    int angle_degree = measure_gamma(p1_ins, p2_ins);
    int gamma = angle_degree % gamma_range_;  // gamma_range_ is the greatest common divisor of symmetry angles of the two parts, and saved here at the compositor after training
    //vcl_cout << "\t \t gamma: " << gamma << " ";
    gamma_prob = prob_density_gamma(gamma);
  }
  double rho_prob = 1.0;
  if (rho_range_ > 0) { // if rho is defined for these two parts, compute it
    int angle_degree = measure_rho(p1_ins, p2_ins);
    int rho = angle_degree % rho_range_;  // rho_range_ is the lowest common multiplier of symmetry angles of the two parts, and saved here at the compositor after training
    //vcl_cout << " rho: " << rho << " ";
    rho_prob = prob_density_rho(rho);
  }
  int d_i = measure_d(p1_ins, p2_ins);
  //vcl_cout << " d: " << d_i << "\n";
  double d_prob = prob_density_d(d_i);
  return gamma_prob*rho_prob*d_prob;
}
double dbrec_pairwise_rot_invariant_model::prob_density_gamma(int gamma) const
{
  if (gamma_min_ > gamma_max_) {  // the interval wraps around
    if (gamma >= gamma_min_ && gamma < gamma_range_) return 1.0;
    if (gamma >= 0 && gamma < gamma_max_) return 1.0;
  } else 
    if (gamma >= gamma_min_ && gamma < gamma_max_) return 1.0;
  return 0.0;
}
double dbrec_pairwise_rot_invariant_model::prob_density_rho(int rho) const
{
  if (rho_min_ > rho_max_) {  // the interval wraps around
    if (rho >= rho_min_ && rho < rho_range_) return 1.0;
    if (rho >= 0 && rho < rho_max_) return 1.0;
  } else 
    if (rho >= rho_min_ && rho < rho_max_) return 1.0;
  return 0.0;
}
//: assume that there is 1 pixel noise in distance calculation so always enlarge the range by 1 pixel from both ends
double dbrec_pairwise_rot_invariant_model::prob_density_d(int d) const
{
  if (d >= d_min_-1 && d < d_max_+1) return 1.0;
  return 0.0;
}

//: return self as a bxml_data_sptr
bxml_data_sptr dbrec_pairwise_rot_invariant_model::xml_element() const
{
  bxml_element* data = new bxml_element("dbrec_pairwise_rot_invariant_model");
  data->set_attribute("d_min", d_min_);
  data->set_attribute("d_max", d_max_);
  data->set_attribute("rho_min", rho_min_);
  data->set_attribute("rho_max", rho_max_);
  data->set_attribute("rho_range", rho_range_);
  data->set_attribute("gamma_min", gamma_min_);
  data->set_attribute("gamma_max", gamma_max_);
  data->set_attribute("gamma_range", gamma_range_);
  data->set_attribute("non_class_prob", non_class_prob_);
  data->append_text("\n");
  return data;
}
//: read self from the bxml_data_sptr
dbrec_pairwise_model_sptr dbrec_pairwise_rot_invariant_model::xml_parse_element(bxml_data_sptr data)
{
  dbrec_pairwise_model_sptr comp;
  bxml_element query("dbrec_pairwise_rot_invariant_model");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);
  if (!base_root)
    return comp;
  bxml_element* r_elm = dynamic_cast<bxml_element*>(base_root.ptr());
  int d_min, d_max, rho_min, rho_max, rho_range, gamma_min, gamma_max, gamma_range;
  double non_class_prob;
  r_elm->get_attribute("d_min", d_min);
  r_elm->get_attribute("d_max", d_max);
  r_elm->get_attribute("rho_min", rho_min);
  r_elm->get_attribute("rho_max", rho_max);
  r_elm->get_attribute("rho_range", rho_range);
  r_elm->get_attribute("gamma_min", gamma_min);
  r_elm->get_attribute("gamma_max", gamma_max);
  r_elm->get_attribute("gamma_range", gamma_range);
  r_elm->get_attribute("non_class_prob", non_class_prob);  
  comp = new dbrec_pairwise_rot_invariant_model(d_min, d_max, rho_min, rho_max, rho_range, gamma_min, gamma_max, gamma_range, non_class_prob);
  return comp;
}

//: return self as a bxml_data_sptr
bxml_data_sptr dbrec_pairwise_rot_invariant_discrete_model::xml_element() const
{
  bxml_element* data = new bxml_element("dbrec_pairwise_rot_invariant_discrete_model");
  data->set_attribute("gamma_min", gamma_min_);
  data->set_attribute("gamma_max", gamma_max_);
  data->set_attribute("gamma_range", gamma_range_);
  if (gamma_defined_) data->set_attribute("gamma_defined", 1);
  else data->set_attribute("gamma_defined", 0);
  if (rho_defined_) data->set_attribute("rho_defined", 1);
  else data->set_attribute("rho_defined", 0);

  data->append_text("\n");

  //: dump the data of the class histogram as a different data element
  bxml_element* hist_data = new bxml_element("class_hist_data");
  hist_data->set_attribute("min_rho", class_hist_.min_a());
  hist_data->set_attribute("max_rho", class_hist_.max_a());
  hist_data->set_attribute("nbins_rho", class_hist_.nbins_a());
  hist_data->set_attribute("min_d", class_hist_.min_b());
  hist_data->set_attribute("max_d", class_hist_.max_b());
  hist_data->set_attribute("nbins_d", class_hist_.nbins_b());
  hist_data->append_text("\n");
  vcl_stringstream ss;
  vbl_array_2d<float> count_arr = class_hist_.counts();
  for (unsigned na = 0; na < class_hist_.nbins_a(); na++) {
    for (unsigned nb = 0; nb < class_hist_.nbins_b(); nb++) {
      ss << count_arr(na, nb) << " ";
    }
    ss << "\n";
  }
  hist_data->append_text(ss.str());
  data->append_data(hist_data);
  data->append_text("\n");

  //: dump the data of the class histogram as a different data element
  bxml_element* hist_data2 = new bxml_element("non_class_hist_data");
  hist_data2->append_text("\n");
  vcl_stringstream ss2;
  vbl_array_2d<float> count_arr2 = non_class_hist_.counts();
  for (unsigned na = 0; na < non_class_hist_.nbins_a(); na++) {
    for (unsigned nb = 0; nb < non_class_hist_.nbins_b(); nb++) {
      ss2 << count_arr2(na, nb) << " ";
    }
    ss2 << "\n";
  }
  hist_data2->append_text(ss2.str());
  data->append_data(hist_data2);
  data->append_text("\n");

  return data;
}

//: read self from the bxml_data_sptr
dbrec_pairwise_model_sptr dbrec_pairwise_rot_invariant_discrete_model::xml_parse_element(bxml_data_sptr data)
{
  dbrec_pairwise_model_sptr comp;
  bxml_element query("dbrec_pairwise_rot_invariant_discrete_model");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);
  if (!base_root)
    return comp;
  bxml_element* r_elm = dynamic_cast<bxml_element*>(base_root.ptr());
  int gamma_min, gamma_max, gamma_range, val; bool gamma_defined, rho_defined;
  r_elm->get_attribute("gamma_min", gamma_min);
  r_elm->get_attribute("gamma_max", gamma_max);
  r_elm->get_attribute("gamma_range", gamma_range);
  r_elm->get_attribute("gamma_defined", val);
  gamma_defined = val == 1 ? true : false;
  r_elm->get_attribute("rho_defined", val);
  rho_defined = val == 1 ? true : false;
  
  bxml_element hquery("class_hist_data");
  bxml_data_sptr hist_root = bxml_find_by_name(base_root, hquery);
  if (!hist_root)
    return comp;
  bxml_element* hist_data = dynamic_cast<bxml_element*>(hist_root.ptr());
  float min_rho, max_rho, min_d, max_d; int nbins_rho, nbins_d;
  hist_data->get_attribute("min_rho", min_rho);
  hist_data->get_attribute("max_rho", max_rho);
  hist_data->get_attribute("nbins_rho", nbins_rho);
  hist_data->get_attribute("min_d", min_d);
  hist_data->get_attribute("max_d", max_d);
  hist_data->get_attribute("nbins_d", nbins_d);
  bsta_joint_histogram<float> class_hist(min_rho, max_rho, nbins_rho, min_d, max_d, nbins_d);
  for (bxml_element::const_data_iterator it = hist_data->data_begin(); it != hist_data->data_end(); it++) {
    if ((*it)->type() != bxml_element::TEXT)
      continue;
    bxml_text* ht = dynamic_cast<bxml_text*>((*it).ptr());
    vcl_stringstream ss(ht->data());
    for (int na = 0; na < nbins_rho; na++) {
      for (int nd = 0; nd < nbins_d; nd++) {
        float val; ss >> val;
        class_hist.set_count(na, nd, val); 
      }
    }
  }
  bxml_element hquery2("non_class_hist_data");
  bxml_data_sptr hist_root2 = bxml_find_by_name(base_root, hquery2);
  if (!hist_root2)
    return comp;
  bxml_element* hist_data2 = dynamic_cast<bxml_element*>(hist_root2.ptr());
  bsta_joint_histogram<float> non_class_hist(min_rho, max_rho, nbins_rho, min_d, max_d, nbins_d);
  for (bxml_element::const_data_iterator it = hist_data2->data_begin(); it != hist_data2->data_end(); it++) {
    if ((*it)->type() != bxml_element::TEXT)
      continue;
    bxml_text* ht = dynamic_cast<bxml_text*>((*it).ptr());
    vcl_stringstream ss(ht->data());
    for (int na = 0; na < nbins_rho; na++) {
      for (int nd = 0; nd < nbins_d; nd++) {
        float val; ss >> val;
        non_class_hist.set_count(na, nd, val); 
      }
    }
  }
  
  comp = new dbrec_pairwise_rot_invariant_discrete_model(gamma_min, gamma_max, gamma_range, gamma_defined, rho_defined, class_hist, non_class_hist);
  return comp;
}

void dbrec_pairwise_rot_invariant_discrete_model::update_d(int d, double class_magnitude, double non_class_magnitude) // case when rho & gamma are undefined
{
  class_hist_.upcount(1.0f, (float)class_magnitude, (float)d, 0.0f);
  non_class_hist_.upcount(1.0f, (float)non_class_magnitude, (float)d, 0.0f);
}
void dbrec_pairwise_rot_invariant_discrete_model::update_rho_d(int rho, int d, double class_magnitude, double non_class_magnitude)
{
  int rho_range = (int)class_hist_.range_a();
  int r = rho % rho_range;
  //: the histogram might be starting from a negative value to represent shifted intervals
  if (class_hist_.min_a() < 0) {
    if (r > class_hist_.max_a()) {
      class_hist_.upcount(float(r-rho_range), (float)class_magnitude, (float)d, 0.0f);
      non_class_hist_.upcount(float(r-rho_range), (float)non_class_magnitude, (float)d, 0.0f);
    } else {
      class_hist_.upcount(float(r), (float)class_magnitude, (float)d, 0.0f);
      non_class_hist_.upcount(float(r), (float)non_class_magnitude, (float)d, 0.0f);
    }
  } else {
    class_hist_.upcount(float(r), (float)class_magnitude, (float)d, 0.0f);
    non_class_hist_.upcount(float(r), (float)non_class_magnitude, (float)d, 0.0f);
  }
}
void dbrec_pairwise_rot_invariant_discrete_model::update_gamma_rho_d(int gamma, int rho, int d, double class_magnitude, double non_class_magnitude)
{
  //: the measured gamma is the clockwise value in degrees in range 0, 360
  int g = gamma % gamma_range_;
  if (gamma_min_ < 0) { 
    if (g >= gamma_range_ + gamma_min_ && g <= gamma_range_) {
      this->update_rho_d(rho, d, class_magnitude, non_class_magnitude);
    } else if (g >= 0 && g <= gamma_max_) {
      this->update_rho_d(rho, d, class_magnitude, non_class_magnitude);
    }
  } else {
    if (g >= gamma_min_ && g <= gamma_max_) {
      this->update_rho_d(rho, d, class_magnitude, non_class_magnitude);
    }
  }
}

float dbrec_pairwise_rot_invariant_discrete_model::class_entropy() {
  return class_hist_.entropy(); }
float dbrec_pairwise_rot_invariant_discrete_model::non_class_entropy() {
  return non_class_hist_.entropy(); }
float dbrec_pairwise_rot_invariant_discrete_model::class_volume() {
  return class_hist_.volume(); }
float dbrec_pairwise_rot_invariant_discrete_model::non_class_volume() {
  return non_class_hist_.volume(); }

void dbrec_pairwise_rot_invariant_discrete_model::prob_densities(dbrec_part_ins_sptr p1_ins, dbrec_part_ins_sptr p2_ins, double& class_density, double& non_class_density) const
{
  bool gamma_ok = false;
  if (gamma_defined_) { //: if gamma is defined for these two parts, compute it
    int angle_degree = dbrec_pairwise_rot_invariant_model::measure_gamma(p1_ins, p2_ins);
    int gamma = angle_degree % gamma_range_;  // gamma_range_ is the greatest common divisor of symmetry angles of the two parts, and saved here at the compositor after training
    //vcl_cout << "\t \t gamma: " << gamma << " ";
    if (gamma_min_ < 0) {  // gamma range might be [-45,45)
      if ((gamma >= 0 && gamma <= gamma_max_) ||
          (gamma >= gamma_range_+gamma_min_ && gamma <= gamma_range_))
          gamma_ok = true;
    } else {               // or it might be [90, 135)
      if (gamma >= gamma_min_ && gamma <= gamma_max_)
        gamma_ok = true;
    }
  } else 
    gamma_ok = true;

  if (!gamma_ok) {
    class_density = 0.0;
    non_class_density = 0.0;
    return;
  }

  int d_i = dbrec_pairwise_rot_invariant_model::measure_d(p1_ins, p2_ins);

  if (rho_defined_) { // if rho is defined for these two parts, compute it
    int angle_degree = dbrec_pairwise_rot_invariant_model::measure_rho(p1_ins, p2_ins);
    //: calculate rho range from the joint histogram
    int rho_range = (int)class_hist_.range_a();
    int rho = angle_degree % rho_range;
    if (class_hist_.min_a() < 0) {  // convert rho to neg value if necessary
      if (rho > class_hist_.max_a())
        class_density = class_hist_.p(float(rho-rho_range), (float)d_i);
      else 
        class_density = class_hist_.p((float)rho, (float)d_i);
    } else {
      class_density = class_hist_.p((float)rho, (float)d_i);
    }
  } else {
    class_density = class_hist_.p(1.0f, (float)d_i);
    //non_class_density = non_class_hist_.p(1.0f, (float)d_i);
  }
  //: define a constant non_class_density
  non_class_density = 1.0f/(class_hist_.nbins_a()*class_hist_.nbins_b());
  return;
}



