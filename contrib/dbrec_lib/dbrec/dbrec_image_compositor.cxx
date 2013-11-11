//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 04/27/09
//
//
#include "dbrec_image_compositor.h"
#include "dbrec_image_pairwise_models.h"
#include "dbrec_image_visitors.h"

#include <vcl_algorithm.h>
#include <bxml/bsvg/bsvg_element.h>
#include <bxml/bxml_find.h>

#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_rational.h>
#include <vcl_limits.h>

//: OZGE TODO: remove this constant and write a method that returns the "receptive field" of a subtree in the hierarchy
const float temporary_width_ = 100.0f;

//: return self as a bxml_data_sptr
bxml_data_sptr dbrec_image_compositor::xml_element() const
{
  bxml_element* data = new bxml_element("image_compositor");
  data->append_text("\n ");
  return data;
}
//: read self from the bxml_data_sptr
dbrec_compositor_sptr dbrec_image_compositor::xml_parse_element(bxml_data_sptr data)
{
  dbrec_compositor_sptr comp;
  bxml_element query("image_compositor");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);
  if (!base_root)
    return comp;
  comp = new dbrec_compositor;  // maybe we should throw an exception here!
  return comp;
}

void dbrec_or_compositor::sample_locations(const vgl_point_2d<float>& loc, float scale, const vcl_vector<dbrec_part_sptr>& children, vcl_vector<vgl_point_2d<float> >& locs) const
{
  locs.clear();
  locs.push_back(loc);
  for (unsigned i = 1; i < children.size(); i++) {
    vgl_point_2d<float> pt(loc.x()+i*temporary_width_, loc.y());
    locs.push_back(pt);
  }
}
//: for the or compositor, the direction vector is undefined, just return unit vector
void dbrec_or_compositor::direction_vector(const vcl_vector<dbrec_part_sptr>& children, vnl_vector_fixed<float,2>& v) const
{
  v(0) = 0.0f;
  v(1) = 1.0f;
}
//: return self as a bxml_data_sptr
bxml_data_sptr dbrec_or_compositor::xml_element() const
{
  bxml_element* data = new bxml_element("image_or_compositor");
  data->append_text("\n ");
  return data;
}
//: read self from the bxml_data_sptr
dbrec_compositor_sptr dbrec_or_compositor::xml_parse_element(bxml_data_sptr data)
{
  dbrec_compositor_sptr comp;
  bxml_element query("image_or_compositor");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);
  if (!base_root)
    return comp;
  comp = new dbrec_or_compositor;
  return comp;
}

//: method to detect instances of a composition, or compositor just puts an instance at every child instance's location
dbrec_part_context_sptr dbrec_or_compositor::detect_instances(unsigned type_id, dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, const vcl_vector<float>& comp_priors, float radius) const
{
  dbrec_part_context_sptr out_c = cf->new_context();
  
  for (unsigned i = 0; i < children.size(); i++) {
    dbrec_part_context_sptr cont = cf->get_context(children[i]->type());
    if (!cont)
      continue;
    dbrec_part_ins_sptr pi = cont->first();
    while (pi) {
      vcl_vector<dbrec_part_ins_sptr> c_ch_ins;
      c_ch_ins.push_back(pi); // push the part as the only child

      dbrec_part_ins_sptr ci = new dbrec_composition_ins(type_id, pi->pt(), c_ch_ins);
      out_c->add_part(ci);
      for (unsigned jj = 0; jj < pi->posteriors_size(); jj++) 
        ci->push_posterior(pi->posterior(jj));

      pi = cont->next();
    }
  }

  return out_c;
}

//: return self as a bxml_data_sptr
bxml_data_sptr dbrec_central_compositor::xml_element() const
{
  bxml_element* data = new bxml_element("image_central_compositor");
  data->set_attribute("nmodels", models_.size());
  data->append_text("\n");
  for (unsigned i = 0; i < models_.size(); i++) {
    bxml_data_sptr model_d = models_[i]->xml_element();
    data->append_data(model_d);
    data->append_text("\n");
  }
  return data;
}

//: read self from the bxml_data_sptr
dbrec_compositor_sptr dbrec_central_compositor::xml_parse_element(bxml_data_sptr data)
{
  dbrec_compositor_sptr comp;
  bxml_element query("image_central_compositor");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);
  bxml_element* c_elm = dynamic_cast<bxml_element*>(base_root.ptr());
  if (!base_root || !c_elm)
    return comp;
  unsigned nm;
  c_elm->get_attribute("nmodels", nm);
  vcl_vector<dbrec_pairwise_model_sptr> class_models;

  //: create an array of parsers to hold a parser for each type of parser and try them all
  typedef dbrec_pairwise_model_sptr (*parsers)(bxml_data_sptr);
  const unsigned parser_size = 5;
  parsers parser_array[parser_size] = {NULL};
  parser_array[0] = &dbrec_pairwise_indep_gaussian_model::xml_parse_element; // put the ones that we use more to the top locations
  parser_array[1] = &dbrec_pairwise_discrete_model::xml_parse_element;
  parser_array[2] = &dbrec_pairwise_indep_uniform_model::xml_parse_element;
  parser_array[3] = &dbrec_pairwise_indep_uniform_discrete_model::xml_parse_element;
  parser_array[4] = &dbrec_pairwise_model::xml_parse_element;
  for (bxml_element::const_data_iterator it = c_elm->data_begin(); it != c_elm->data_end(); it++) {
    if ((*it)->type() != bxml_element::ELEMENT)
      continue;

    dbrec_pairwise_model_sptr model;
    for (unsigned i = 0; i < parser_size; i++) {
      model = parser_array[i](*it);
      if (model) {
        class_models.push_back(model);
        break;  // no need to try the rest of the parsers
      }
    }
  }
  if (class_models.size() != nm)
    return comp;

  comp = new dbrec_central_compositor(class_models);
  return comp;
}

//: sample locations of each child in the composition wrt given location
void dbrec_central_compositor::sample_locations(const vgl_point_2d<float>& loc, float scale, const vcl_vector<dbrec_part_sptr>& children, vcl_vector<vgl_point_2d<float> >& locs) const
{
  if (!children.size())
    return;

  locs.clear();
  locs.push_back(loc);
  for (unsigned i = 0; i < models_.size(); i++) {
    vgl_point_2d<float> second_loc;
    models_[i]->sample_location(loc, scale, children[0], second_loc);
    locs.push_back(second_loc);
  }
}

//: for the central_compositor, the direction vector is the direction vector of the central part, the first child
void dbrec_central_compositor::direction_vector(const vcl_vector<dbrec_part_sptr>& children, vnl_vector_fixed<float,2>& v) const
{
  if (!children.size())
    return;
  dbrec_get_direction_vector_visitor dv;
  children[0]->accept(&dv);
  v = dv.v_;
}

//: draw additional attributes on the composing parts, central compositor marks the central part by displaying its direction vector, and draws lines connecting center to other parts
void dbrec_central_compositor::draw_attributes(bsvg_document& doc, const vcl_vector<dbrec_part_sptr>& children, const vcl_vector<vgl_point_2d<float> >& locs) const
{
  if (!children.size() || children.size() != locs.size()) {
    vcl_cout << "Error! incompatible vector sizes in dbrec_central_compositor::draw_attributes()!\n";
    return;
  }

  vnl_vector_fixed<float,2> v;
  this->direction_vector(children, v);

  vnl_vector_fixed<float,2> v2 = 100.0f*v; // scale it up
  bsvg_line* l = new bsvg_line(locs[0].x(), locs[0].y(), locs[0].x()+v2[0], locs[0].y()+v2[1]);
  l->set_stroke_color("red");
  //l->set_stroke_width(10);
  doc.add_element(l);

  for (unsigned i = 1; i < locs.size(); i++) {
    bsvg_line* l = new bsvg_line(locs[0].x(), locs[0].y(), locs[i].x(), locs[i].y());
    l->set_stroke_color("yellow");
    //l->set_stroke_width(10);
    doc.add_element(l);
  }
}

struct adder { 
  double sum_;
  adder() : sum_(0) {}
  void operator() (double val) { sum_ += val; }
};

//: method to detect instances of a composition
dbrec_part_context_sptr dbrec_central_compositor::detect_instances(unsigned type_id, dbrec_context_factory_sptr cf, 
   const vcl_vector<dbrec_part_sptr>& children, const vcl_vector<float>& comp_priors, float radius) const
{
  dbrec_part_context_sptr out_c = cf->new_context();

  if (!children.size() || models_.size() != children.size() - 1) // there should be a model for each non-central child
    return out_c;

  dbrec_part_sptr central_part = children[0];
  if (!central_part) 
    return out_c;

  //: find central part's direction vector, it is used in the geometric calculations
  vnl_vector_fixed<float, 2> central_dir_v;
  this->direction_vector(children, central_dir_v);

  dbrec_part_context_sptr central_c = cf->get_context(central_part->type());
  if (!central_c)
    return out_c;
  vcl_vector<dbrec_part_context_sptr> children_c;
  for (unsigned i = 1; i < children.size(); i++) {
    dbrec_part_context_sptr cont = cf->get_context(children[i]->type());
    if (!cont)
      return out_c;
    children_c.push_back(cont);
  }
  //: prepare the models depending on the size of composition priors
  vcl_vector<vcl_vector<dbrec_pairwise_model_sptr> > models;
  dbrec_pairwise_model_sptr non_class_uniform_model = new dbrec_pairwise_indep_uniform_discrete_model(radius);
  vcl_vector<dbrec_pairwise_model_sptr> nc_models(models_.size(), non_class_uniform_model);
  switch(comp_priors.size()) { // it is either 2 or 4
    case 2: // use the class models in the compositor and the uniform non-class model
      { models.push_back(models_); models.push_back(nc_models); break; }
    case 4: // push 4 models for class-foreground, non-class-foreground, class-background, non-class background
      { models.push_back(models_); models.push_back(nc_models); models.push_back(models_); models.push_back(nc_models); break; }
    case 3: case 0: case 1: default: { vcl_cout << "Error! dbrec_central_compositor::detect_instances() -- size of priors vector is not valid!\n"; return out_c; }
  }

  //: now go through each instance of the central part
  dbrec_part_ins_sptr pi = central_c->first();
  
  //: make sure the models vector is the same size as the lower levels posterior size
  if (pi)
    if (pi->posteriors_size() != models.size())  // we're only parsing the primitives e.g. for training appearance models purposes, they don't have posteriors yet
      return out_c;

  while (pi) {

    if (pi->pt().x() == 740 && pi->pt().y() == 511) {
      vcl_cout << "here found 459 and 735!\n";
    }

    vcl_vector<double> temp_central_likelihoods(pi->posteriors_size());
    for (unsigned jj = 0; jj < pi->posteriors_size(); jj++) {
      temp_central_likelihoods[jj] = pi->posterior(jj)*comp_priors[jj];
    }
  
    vcl_vector<dbrec_part_ins_sptr> c_ch_ins;
    c_ch_ins.push_back(pi); // push the central part

    //: now go through each other children and their contexts
    for (unsigned i = 1; i < children.size(); i++) {
      dbrec_part_context_sptr ch_i_c = children_c[i-1];
      //: find the probe box for this instance of central part, using this child's pairwise model
      vgl_box_2d<float> probe_box = models_[i-1]->get_probe_box(pi->pt(), central_dir_v);
      vcl_vector<dbrec_part_ins_sptr> out;
      ch_i_c->query(probe_box, out);
      
      // now for each instance in the probe, compute the likelihood's for all the cases, and pick the best one
      double best_score = -100000.0;
      dbrec_part_ins_sptr best_part;
      for (unsigned k = 0; k < out.size(); k++) {
        vcl_vector<double> temp(pi->posteriors_size());
        
        for (unsigned jj = 0; jj < out[k]->posteriors_size(); jj++) {
          double dens = models[jj][i-1]->prob_density(central_dir_v, pi->pt(), out[k]->pt());
          temp[jj] = dens*out[k]->posterior(jj)*temp_central_likelihoods[jj];
        }
        
        for (unsigned jj = 1; jj < temp.size(); jj++) 
          temp[jj] = temp[0]/temp[jj];
        
        vcl_vector<double>::iterator it = temp.begin(); it++;
        double min = *vcl_min_element(it, temp.end());
        if (best_score < min) {
          best_score = min;
          best_part = out[k];
        }
      }
      
      if (best_score > 0 && best_part) {  // if we've found an instance in the probe region
        c_ch_ins.push_back(best_part);
      } else 
        break;
    }

    if (c_ch_ins.size() == children.size()) { // we've located valid instances for each children
      dbrec_part_ins_sptr ci = new dbrec_composition_ins(type_id, pi->pt(), c_ch_ins);
      out_c->add_part(ci);

      //: now compute the posteriors for this instance with a second pass
      for (unsigned i = 1; i < c_ch_ins.size(); i++) {
        dbrec_part_ins_sptr second_pi = c_ch_ins[i];
        
        for (unsigned jj = 0; jj < pi->posteriors_size(); jj++) 
          temp_central_likelihoods[jj] *= models[jj][i-1]->prob_density(central_dir_v, pi->pt(), second_pi->pt())*second_pi->posterior(jj);
      }

      /* OZGE TODO: implement the contributions from the other classes */

      // if all of them have been detected then declare existence at the central parts location
      double den = 0;
      for (unsigned jj = 0; jj < temp_central_likelihoods.size(); jj++) { den += temp_central_likelihoods[jj]; }
      for (unsigned jj = 0; jj < temp_central_likelihoods.size(); jj++) {
        ci->push_posterior(temp_central_likelihoods[jj]/den);
      }
    }

    pi = central_c->next();
  }

  return out_c;
}

//: method to train a compositional part's geometric models
bool dbrec_central_compositor::train_instances(dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float radius)
{
  if (!children.size() || models_.size() != children.size() - 1) // there should be a model for each non-central child
    return false;

  dbrec_part_sptr central_part = children[0];
  if (!central_part) 
    return false;

  //: find central part's direction vector, it is used in the geometric calculations
  vnl_vector_fixed<float, 2> central_dir_v;
  this->direction_vector(children, central_dir_v);

  dbrec_part_context_sptr central_c = cf->get_context(central_part->type());
  if (!central_c)
    return false;
  vcl_vector<dbrec_part_context_sptr> children_c;
  for (unsigned i = 1; i < children.size(); i++) {
    dbrec_part_context_sptr cont = cf->get_context(children[i]->type());
    if (!cont)
      return false;
    children_c.push_back(cont);
  }

  //: we need this model to generate a probe box which is centered around the central part and of width 2*radius
  dbrec_pairwise_model_sptr non_class_uniform_model = new dbrec_pairwise_indep_uniform_discrete_model(radius);

  //: now go through each instance of the central part
  dbrec_part_ins_sptr pi = central_c->first();
  while (pi) {
     vnl_vector_fixed<float, 2> central_loc(pi->pt().x(), pi->pt().y());
     if (central_loc[0] == 418 && central_loc[1] == 69)
       vcl_cout << "here";

    //: find the probe box around this central part
    vgl_box_2d<float> probe_box_central = non_class_uniform_model->get_probe_box(pi->pt(), central_dir_v);

    //: now go through each other children and their contexts
    for (unsigned i = 1; i < children.size(); i++) {
      dbrec_part_context_sptr ch_i_c = children_c[i-1];
      
      //: find all the parts from this children's type in the vicinity of the central part instance
      vcl_vector<dbrec_part_ins_sptr> out;
      ch_i_c->query(probe_box_central, out);
      
      // now for each instance in the probe, train the model of the compositional part for this pair 
      for (unsigned k = 0; k < out.size(); k++) { 
        vnl_vector_fixed<float, 2> second_loc(out[k]->pt().x(), out[k]->pt().y());
        this->models_[i-1]->update_models(central_dir_v, central_loc, second_loc, (float)pi->posterior(0), (float)out[k]->posterior(0));
      }
    }
    pi = central_c->next();
  }
  return true;
}

//: visualize the compositor's models
void dbrec_central_compositor::visualize(vcl_string& doc_name, const vcl_vector<dbrec_part_sptr>& children) const
{
  for (unsigned i = 0; i < models_.size(); i++) {
    vcl_stringstream ii; ii << i;
    vcl_string doc_name2 = doc_name+"_model_"+ii.str();
    models_[i]->visualize(doc_name2);
  }
}

void dbrec_pairwise_discrete_compositor::visualize(vcl_string& doc_name, const vcl_vector<dbrec_part_sptr>& children) const
{
  vcl_string filename = doc_name+ "_model_";
  model_->visualize(filename);
}

//: sample locations of each child in the composition wrt given location
void dbrec_pairwise_compositor::sample_locations(const vgl_point_2d<float>& loc, float scale, const vcl_vector<dbrec_part_sptr>& children, vcl_vector<vgl_point_2d<float> >& locs) const
{
  if (!children.size())
    return;
/*
  locs.clear();
  vgl_point_2d<float> f = model_.sample_first_loc(loc);
  locs.push_back(f);
  vgl_point_2d<float> s = model_.sample_second_loc(loc);
  locs.push_back(s);
  */
}

//: return self as a bxml_data_sptr
bxml_data_sptr dbrec_pairwise_compositor::xml_element() const
{
  bxml_element* data = new bxml_element("image_pairwise_compositor");
  data->append_text("\n");
  bxml_data_sptr model_d = model_->xml_element();
  data->append_data(model_d);
  data->append_text("\n");
  return data;
}

//: read self from the bxml_data_sptr
dbrec_compositor_sptr dbrec_pairwise_compositor::xml_parse_element(bxml_data_sptr data)
{
  dbrec_compositor_sptr comp;
  bxml_element query("image_pairwise_compositor");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);
  bxml_element* c_elm = dynamic_cast<bxml_element*>(base_root.ptr());
  if (!base_root || !c_elm)
    return comp;
  
  dbrec_pairwise_model_sptr class_model;
  
  for (bxml_element::const_data_iterator it = c_elm->data_begin(); it != c_elm->data_end(); it++) {
    if ((*it)->type() != bxml_element::ELEMENT)
      continue;

    class_model = dbrec_pairwise_rot_invariant_model::xml_parse_element(*it);
    if (class_model)
      break;
  }
  
  if (!class_model)
    return comp;

  dbrec_pairwise_rot_invariant_model_sptr m = dynamic_cast<dbrec_pairwise_rot_invariant_model*>(class_model.ptr());
  if (!m)
    return comp;
  comp = new dbrec_pairwise_compositor(m);
  return comp;
}

//: method to detect instances of a composition in a rotationally invariant way for the 2 case problem: class and non-class
dbrec_part_context_sptr dbrec_pairwise_compositor::detect_instances_rot_inv(unsigned type_id, dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float comp_class_prior, float radius)
{
  dbrec_part_context_sptr out_c = cf->new_context();

  if (children.size() != 2) 
    return out_c;

  dbrec_part_sptr part1 = children[0];
  dbrec_part_sptr part2 = children[1];
  if (!part1 || !part2) 
    return out_c;

  dbrec_part_context_sptr part1_c = cf->get_context(part1->type());
  dbrec_part_context_sptr part2_c = cf->get_context(part2->type());
  if (!part1_c || !part2_c)
    return out_c;
  
  //: prepare this model to get a probe box of size radius around a given point
  dbrec_pairwise_indep_uniform_discrete_model nc_model(radius);

  float part_mass_ratio = (part2->mass()/part1->mass() + 1.0f);  // a constant required to find center of mass for the composition instance

  //: now go through each instance of the first part
  dbrec_part_ins_sptr pi = part1_c->first();
  
  //: make sure the models vector is the same size as the lower levels posterior size
  if (pi)
    if (pi->posteriors_size() != 2)  // we're only parsing the primitives e.g. for training appearance models purposes, they don't have posteriors yet
      return out_c;

  while (pi) {

    //vcl_cout << "At part1 instance: " << pi->pt() << " class post: " << pi->posterior(0) << " non_class post: " << pi->posterior(1) << vcl_endl;

    double class_likelihood_pi = pi->posterior(0)*comp_class_prior;
    double non_class_likelihood_pi = pi->posterior(1)*(1.0f-comp_class_prior);

    vgl_box_2d<float> probe_box = nc_model.get_probe_box(pi->pt()); 
    vcl_vector<dbrec_part_ins_sptr> out;
    part2_c->query(probe_box, out);
    
    //vcl_cout << "\t found " << out.size() << " part2 instances around it!\n";
    for (unsigned k = 0; k < out.size(); k++) {
      //vcl_cout << "\t part2 instance: " << out[k]->pt() << " class post: " << out[k]->posterior(0) << " non_class post: " << out[k]->posterior(1) << vcl_endl;
      dbrec_part_ins_sptr best_part = out[k];
      double class_likelihood = model_->prob_density(pi, out[k]);
      if (class_likelihood == 0.0)
        continue;
      class_likelihood *= out[k]->posterior(0)*class_likelihood_pi; // this is either one or zero
      double non_class_likelihood = model_->prob_density_non_class();  // this is a constant value for all instances in the radius satisfying gamma criterion
      non_class_likelihood *= out[k]->posterior(1)*non_class_likelihood_pi;

    //if (best_score > 0 && best_part) { // if we've found an instance in the probe region
      //vcl_cout << "\t winner part2 instance: " << best_part->pt() << " ";

      //: compute the center as the center of mass of the two
      //: first find the vector that joins the two centers
      vnl_vector_fixed<float, 2> center1(pi->pt().x(), pi->pt().y());
      vnl_vector_fixed<float, 2> center2(best_part->pt().x(), best_part->pt().y());
      vnl_vector_fixed<float, 2> dif_vec = center2 - center1;
      float distance = dif_vec.magnitude();
      dif_vec = dif_vec.normalize();  // find the unit vector in this direction
      //: now find p and q which are the values that give how far apart the centers are from the center of mass
      float q = distance / part_mass_ratio;
      float p = distance - q;
      vnl_vector_fixed<float, 2> center = center1 + p*dif_vec;

      bool exists = false;
      if (!model_->rho_defined()) {//: if rho is undefined, there is a danger that the same part is about to be added twice so check for this
        
        vgl_point_2d<float> pr0(center[0]-2, center[1]-2), pr1(center[0]+2, center[1]+2);
        vgl_box_2d<float> probe;
        probe.add(pr0); probe.add(pr1); 
        
        vcl_vector<dbrec_part_ins_sptr> out2;
        out_c->query(probe, out2);

        for (unsigned i = 0; i < out2.size(); i++) {
          dbrec_rot_inv_composition_ins* ci_i = dynamic_cast<dbrec_rot_inv_composition_ins*>(out2[i].ptr());
          if ((ci_i->children())[0] == best_part && (ci_i->children())[1] == pi) {
            exists = true;
            break;
          }
        }
      }
      
      if (!exists) {
        //: compute the direction vector as the vector perpendicular to the vector that joins the two centers
        double rad = 90.0*vnl_math::pi/180.0;
        float c = (float)vcl_cos(rad), s = (float)vcl_sin(rad);
        vnl_matrix_fixed<float, 2, 2> rot_matrix;
        rot_matrix(0,0) = c; rot_matrix(0,1) = -s; rot_matrix(1,0) = s; rot_matrix(1,1) = c;
        vnl_vector_fixed<float, 2> dir_vec = rot_matrix*dif_vec;

        vcl_vector<dbrec_part_ins_sptr> c_ch_ins;
        c_ch_ins.push_back(pi); c_ch_ins.push_back(best_part);
        dbrec_part_ins_sptr ci = new dbrec_rot_inv_composition_ins(type_id, center[0], center[1], c_ch_ins, dir_vec);
        double den = class_likelihood + non_class_likelihood;
        ci->push_posterior(class_likelihood/den);  ci->push_posterior(non_class_likelihood/den);
        out_c->add_part(ci);
        //vcl_cout << " class post: " << best_class_likelihood/den << " ";
      }
      //vcl_cout << "\n";
    }

    pi = part1_c->next();
  }

  return out_c;

}

//: method to detect instances of a composition in a rotationally invariant way for the 4 case problem: class-foreground, non-class foreground, class background, non-class background
dbrec_part_context_sptr dbrec_pairwise_compositor::detect_instances_rot_inv(unsigned type_id, dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float comp_class_prior, vil_image_view<float>& fg_prob, float radius)
{
  dbrec_part_context_sptr out_c = cf->new_context();

  if (children.size() != 2) 
    return out_c;

  dbrec_part_sptr part1 = children[0];
  dbrec_part_sptr part2 = children[1];
  if (!part1 || !part2) 
    return out_c;

  dbrec_part_context_sptr part1_c = cf->get_context(part1->type());
  dbrec_part_context_sptr part2_c = cf->get_context(part2->type());
  if (!part1_c || !part2_c)
    return out_c;
  
  //: prepare this model to get a probe box of size radius around a given point
  dbrec_pairwise_indep_uniform_discrete_model nc_model(radius);

  float part_mass_ratio = (part2->mass()/part1->mass() + 1.0f);  // a constant required to find center of mass for the composition instance

  //: now go through each instance of the first part
  dbrec_part_ins_sptr pi = part1_c->first();
  
  //: make sure the models vector is the same size as the lower levels posterior size
  if (pi)
    if (pi->posteriors_size() != 4)  // we're only parsing the primitives e.g. for training appearance models purposes, they don't have posteriors yet
      return out_c;

  while (pi) {

    //vcl_cout << "At part1 instance: " << pi->pt() << " class post: " << pi->posterior(0) << " non_class post: " << pi->posterior(1) << vcl_endl;
    
    dbrec_measure_fg_prob_visitor mfgv(fg_prob, pi);
    part1->accept(&mfgv);
    float prior_i_f = mfgv.prob_;
    
    if ((pi->pt().x() == 281 && pi->pt().y() == 469))
      vcl_cout << "here in (281, 469) class post: " << pi->posterior(0) << " non_class post: " << pi->posterior(1) << vcl_endl;

    float comp_non_class_prior = 1.0f-comp_class_prior;

    vgl_box_2d<float> probe_box = nc_model.get_probe_box(pi->pt()); 
    vcl_vector<dbrec_part_ins_sptr> out;
    part2_c->query(probe_box, out);
    
    if ((pi->pt().x() == 523 && pi->pt().y() == 690))
      vcl_cout << "\t found " << out.size() << " part2 instances around it!\n";
    for (unsigned k = 0; k < out.size(); k++) {
      if ((pi->pt().x() == 523 && pi->pt().y() == 690))
        vcl_cout << "\t part2 instance: " << out[k]->pt() << " class post: " << out[k]->posterior(0) << " non_class post: " << out[k]->posterior(1) << vcl_endl;
      dbrec_part_ins_sptr pi_k = out[k];

      double class_likelihood = model_->prob_density(pi, pi_k);
      if (class_likelihood == 0.0)
        continue;
      
      //: compute the center as the center of mass of the two
      //: first find the vector that joins the two centers
      vnl_vector_fixed<float, 2> center1(pi->pt().x(), pi->pt().y());
      vnl_vector_fixed<float, 2> center2(pi_k->pt().x(), pi_k->pt().y());
      vnl_vector_fixed<float, 2> dif_vec = center2 - center1;
      float distance = dif_vec.magnitude();
      dif_vec = dif_vec.normalize();  // find the unit vector in this direction
      //: now find p and q which are the values that give how far apart the centers are from the center of mass
      float q = distance / part_mass_ratio;
      float p = distance - q;
      vnl_vector_fixed<float, 2> center = center1 + p*dif_vec;

      bool exists = false;
      if (!model_->rho_defined()) {//: if rho is undefined, there is a danger that the same part is about to be added twice so check for this
        
        vgl_point_2d<float> pr0(center[0]-2, center[1]-2), pr1(center[0]+2, center[1]+2);
        vgl_box_2d<float> probe;
        probe.add(pr0); probe.add(pr1); 
        
        vcl_vector<dbrec_part_ins_sptr> out2;
        out_c->query(probe, out2);

        for (unsigned i = 0; i < out2.size(); i++) {
          dbrec_rot_inv_composition_ins* ci_i = dynamic_cast<dbrec_rot_inv_composition_ins*>(out2[i].ptr());
          if ((ci_i->children())[0] == pi_k && (ci_i->children())[1] == pi) {
            exists = true;
            break;
          }
        }
      }
      
      if (!exists) {
        //: compute the direction vector as the vector perpendicular to the vector that joins the two centers
        double rad = 90.0*vnl_math::pi/180.0;
        float c = (float)vcl_cos(rad), s = (float)vcl_sin(rad);
        vnl_matrix_fixed<float, 2, 2> rot_matrix;
        rot_matrix(0,0) = c; rot_matrix(0,1) = -s; rot_matrix(1,0) = s; rot_matrix(1,1) = c;
        vnl_vector_fixed<float, 2> dir_vec = rot_matrix*dif_vec;

        vcl_vector<dbrec_part_ins_sptr> c_ch_ins;
        c_ch_ins.push_back(pi); c_ch_ins.push_back(pi_k);
        dbrec_part_ins_sptr ci = new dbrec_rot_inv_composition_ins(type_id, center[0], center[1], c_ch_ins, dir_vec);

        //: now compute 4 likelihoods
        dbrec_measure_fg_prob_visitor mfgvk(fg_prob, pi_k);
        part2->accept(&mfgvk);
        float prior_k_f = mfgvk.prob_;
        float comp_prior_f = (prior_i_f + prior_k_f)/2.0f;
        
        if ((pi->pt().x() == 463 && pi->pt().y() == 272))
          vcl_cout << "\t \t prior_i_f: " << prior_i_f << " prior_k_f: " << prior_k_f << " comp_prior_f: " << comp_prior_f << vcl_endl;

        double non_class_likelihood = model_->prob_density_non_class();  // this is a constant value for all instances in the radius satisfying gamma criterion

        if ((pi->pt().x() == 463 && pi->pt().y() == 272))
          vcl_cout << "\t \t non_class_likelihood: " << non_class_likelihood << vcl_endl;
        
        /*double class_fg_likelihood = pi->posterior(0)*pi_k->posterior(0)*comp_class_prior*comp_prior_f;
        double non_class_fg_likelihood = non_class_likelihood*pi->posterior(1)*pi_k->posterior(1)*comp_non_class_prior*comp_prior_f;
        double class_bg_likelihood = pi->posterior(2)*pi_k->posterior(2)*comp_class_prior*(1.0f-comp_prior_f);
        double non_class_bg_likelihood = non_class_likelihood*pi->posterior(3)*pi_k->posterior(3)*comp_non_class_prior*(1.0f-comp_prior_f);
        */
        double class_fg_likelihood = pi->posterior(0)*pi_k->posterior(0)*0.15f;
        double non_class_fg_likelihood = non_class_likelihood*pi->posterior(1)*pi_k->posterior(1)*0.15f;
        double class_bg_likelihood = pi->posterior(2)*pi_k->posterior(2)*0.15f;
        double non_class_bg_likelihood = non_class_likelihood*pi->posterior(3)*pi_k->posterior(3)*0.55f;
        

        double den = class_fg_likelihood + non_class_fg_likelihood + class_bg_likelihood + non_class_bg_likelihood;

        if ((pi->pt().x() == 463 && pi->pt().y() == 272))
          vcl_cout << "\t \t class_fg_likelihood: " << class_fg_likelihood << " non_class_fg_like: " << non_class_fg_likelihood << " den: " << den << vcl_endl;


        ci->push_posterior(class_fg_likelihood/den);  
        ci->push_posterior(non_class_fg_likelihood/den);
        ci->push_posterior(class_bg_likelihood/den);
        ci->push_posterior(non_class_bg_likelihood/den);
        out_c->add_part(ci);
        //vcl_cout << " class post: " << best_class_likelihood/den << " ";
      }
      //vcl_cout << "\n";
    }

    pi = part1_c->next();
  }

  return out_c;
}

void dbrec_pairwise_compositor::collect_rot_inv_stats(dbrec_context_factory_sptr cf, int posterior_id, dbrec_part_sptr p1, dbrec_part_sptr p2, float radius, const vcl_string& output_file)
{
  dbrec_part_context_sptr part1_c = cf->get_context(p1->type());
  dbrec_part_context_sptr part2_c = cf->get_context(p2->type());
  if (!part1_c || !part2_c)
    return;
  
  //: check if gamma is defined for this pair
  dbrec_has_direction_vector_visitor hdv1;
  p1->accept(&hdv1);

  dbrec_has_direction_vector_visitor hdv2;
  p2->accept(&hdv2);

  if (hdv2.has_it_ && !hdv1.has_it_)  // this is not a valid pair 
    return;

  bool rho_defined = false;
  bool gamma_defined = false;
  int gamma_range = 0;
  int rho_range = 0;
  if (hdv1.has_it_) {
    rho_defined = true;
    dbrec_get_symmetry_angle_visitor sav1;
    p1->accept(&sav1);
    rho_range = sav1.angle_;

    if (hdv2.has_it_) {
      gamma_defined = true;
      dbrec_get_symmetry_angle_visitor sav2;
      p2->accept(&sav2);
        
      gamma_range = vnl_rational::gcd(sav1.angle_, sav2.angle_);
      //: find the least common multiplier as the range of rho: lcm(a,b) = |a.b|/gcd(a,b)
      rho_range = sav1.angle_*sav2.angle_ / gamma_range;
      gamma_range = gamma_range > 360 ? 360 : gamma_range;
      rho_range = rho_range > 360 ? 360 : rho_range;
      vcl_cout << " \t found gamma_range: " << gamma_range << " rho_range: " << rho_range << vcl_endl;
    }
  }

  vcl_ofstream ofs(output_file.c_str(), vcl_ofstream::app);
  if (!ofs) {
    vcl_cout << "In dbrec_pairwise_compositor::collect_rot_inv_stats() -- problems opening file: " << output_file << vcl_endl;
    return;
  }

  //: prepare this model to get a probe box of size radius around a given point
  dbrec_pairwise_indep_uniform_discrete_model nc_model(radius);

  //: now go through each instance of the first part
  dbrec_part_ins_sptr pi = part1_c->first();
  
  //: make sure the part's class posterior have been computed
  if (pi)
    if (posterior_id >= (int)pi->posteriors_size()) {
      vcl_cout << "In dbrec_pairwise_compositor::collect_rot_inv_stats) -- posterior maps have not been computed!\n";
      throw 0;
    }

  while (pi) {

    vgl_box_2d<float> probe_box = nc_model.get_probe_box(pi->pt()); 
    vcl_vector<dbrec_part_ins_sptr> out;
    part2_c->query(probe_box, out);
    
    for (unsigned k = 0; k < out.size(); k++) {
      dbrec_part_ins_sptr pi_k = out[k];

      ofs << pi->posterior(posterior_id)*pi_k->posterior(posterior_id) << " ";

      int d = dbrec_pairwise_rot_invariant_model::measure_d(pi, pi_k);
      ofs << d << " ";
      //vcl_cout << d << " ";
      
      if (rho_defined) {
        int rho = dbrec_pairwise_rot_invariant_model::measure_rho(pi, pi_k);
        ofs << rho % rho_range << " ";
        //vcl_cout << rho << " ";
        if (gamma_defined) {
          int gamma = dbrec_pairwise_rot_invariant_model::measure_gamma(pi, pi_k);
          ofs << gamma % gamma_range << " ";
          //vcl_cout << gamma << " ";
        }
      } 
      ofs << "\n";
    }

    pi = part1_c->next();
  }

  ofs.close();
  return;
}

//: method to detect instances of a composition in a rotationally invariant way for the 2 case problem: class and non-class
dbrec_part_context_sptr dbrec_pairwise_discrete_compositor::detect_instances_rot_inv(unsigned type_id, dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float comp_class_prior, float radius)
{
  vcl_cout << "In dbrec_pairwise_discrete_compositor::detect_instances_rot_inv() - not implemented yet!\n";
  throw 0;
}

//: method to detect instances of a composition in a rotationally invariant way for the 4 case problem: class-foreground, non-class foreground, class background, non-class background
dbrec_part_context_sptr dbrec_pairwise_discrete_compositor::detect_instances_rot_inv(unsigned type_id, dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float comp_class_prior, vil_image_view<float>& fg_prob, float radius)
{
  vcl_cout << "PARSING PART: " << type_id << "!!!!!!!!!!!!!!!!!!!!!!!!!\n";
  vcl_cout.flush();
  dbrec_part_context_sptr out_c = cf->new_context();

  if (children.size() != 2) 
    return out_c;

  dbrec_part_sptr part1 = children[0];
  dbrec_part_sptr part2 = children[1];
  if (!part1 || !part2) 
    return out_c;

  dbrec_part_context_sptr part1_c = cf->get_context(part1->type());
  dbrec_part_context_sptr part2_c = cf->get_context(part2->type());
  if (!part1_c || !part2_c)
    return out_c;
  
  //: prepare this model to get a probe box of size radius around a given point
  dbrec_pairwise_indep_uniform_discrete_model nc_model(radius);

  float part_mass_ratio = (part2->mass()/part1->mass() + 1.0f);  // a constant required to find center of mass for the composition instance

  //: now go through each instance of the first part
  dbrec_part_ins_sptr pi = part1_c->first();
  
  //: make sure the models vector is the same size as the lower levels posterior size
  if (pi)
    if (pi->posteriors_size() != 4)  // we're only parsing the primitives e.g. for training appearance models purposes, they don't have posteriors yet
      return out_c;

  float comp_non_class_prior = 1.0f-comp_class_prior;
  while (pi) {

    //vcl_cout << "At part1 instance: " << pi->pt() << " class post: " << pi->posterior(0) << " non_class post: " << pi->posterior(1) << vcl_endl;
    
    dbrec_measure_fg_prob_visitor mfgv(fg_prob, pi);
    part1->accept(&mfgv);
    float prior_i_f = mfgv.prob_;
    
    //if ((pi->pt().x() == 281 && pi->pt().y() == 469))
    //  vcl_cout << "here in (281, 469) class post: " << pi->posterior(0) << " non_class post: " << pi->posterior(1) << vcl_endl;

    vgl_box_2d<float> probe_box = nc_model.get_probe_box(pi->pt()); 
    vcl_vector<dbrec_part_ins_sptr> out;
    part2_c->query(probe_box, out);
    
    //if ((pi->pt().x() == 523 && pi->pt().y() == 690))
    //  vcl_cout << "\t found " << out.size() << " part2 instances around it!\n";
    for (unsigned k = 0; k < out.size(); k++) {
      //if ((pi->pt().x() == 523 && pi->pt().y() == 690))
      //  vcl_cout << "\t part2 instance: " << out[k]->pt() << " class post: " << out[k]->posterior(0) << " non_class post: " << out[k]->posterior(1) << vcl_endl;
      dbrec_part_ins_sptr pi_k = out[k];

      double class_likelihood, non_class_likelihood;
      model_->prob_densities(pi, pi_k, class_likelihood, non_class_likelihood);
      if (class_likelihood == 0.0)
        continue;
      
      //: compute the center as the center of mass of the two
      //: first find the vector that joins the two centers
      vnl_vector_fixed<float, 2> center1(pi->pt().x(), pi->pt().y());
      vnl_vector_fixed<float, 2> center2(pi_k->pt().x(), pi_k->pt().y());
      vnl_vector_fixed<float, 2> dif_vec = center2 - center1;
      float distance = dif_vec.magnitude();
      dif_vec = dif_vec.normalize();  // find the unit vector in this direction
      //: now find p and q which are the values that give how far apart the centers are from the center of mass
      float q = distance / part_mass_ratio;
      float p = distance - q;
      vnl_vector_fixed<float, 2> center = center1 + p*dif_vec;

      bool exists = false;
      if (!model_->rho_defined()) {//: if rho is undefined, there is a danger that the same part is about to be added twice so check for this
        
        vgl_point_2d<float> pr0(center[0]-2, center[1]-2), pr1(center[0]+2, center[1]+2);
        vgl_box_2d<float> probe;
        probe.add(pr0); probe.add(pr1); 
        
        vcl_vector<dbrec_part_ins_sptr> out2;
        out_c->query(probe, out2);

        for (unsigned i = 0; i < out2.size(); i++) {
          dbrec_rot_inv_composition_ins* ci_i = dynamic_cast<dbrec_rot_inv_composition_ins*>(out2[i].ptr());
          if ((ci_i->children())[0] == pi_k && (ci_i->children())[1] == pi) {
            exists = true;
            break;
          }
        }
      }
      
      if (!exists) {
        //: compute the direction vector as the vector perpendicular to the vector that joins the two centers
        double rad = 90.0*vnl_math::pi/180.0;
        float c = (float)vcl_cos(rad), s = (float)vcl_sin(rad);
        vnl_matrix_fixed<float, 2, 2> rot_matrix;
        rot_matrix(0,0) = c; rot_matrix(0,1) = -s; rot_matrix(1,0) = s; rot_matrix(1,1) = c;
        vnl_vector_fixed<float, 2> dir_vec = rot_matrix*dif_vec;

        vcl_vector<dbrec_part_ins_sptr> c_ch_ins;
        c_ch_ins.push_back(pi); c_ch_ins.push_back(pi_k);
        dbrec_part_ins_sptr ci = new dbrec_rot_inv_composition_ins(type_id, center[0], center[1], c_ch_ins, dir_vec);

        //: now compute 4 likelihoods
        dbrec_measure_fg_prob_visitor mfgvk(fg_prob, pi_k);
        part2->accept(&mfgvk);
        float prior_k_f = mfgvk.prob_;
        float comp_prior_f = (prior_i_f + prior_k_f)/2.0f;
        
        //if ((pi->pt().x() == 463 && pi->pt().y() == 272))
        //  vcl_cout << "\t \t prior_i_f: " << prior_i_f << " prior_k_f: " << prior_k_f << " comp_prior_f: " << comp_prior_f << vcl_endl;
/*
        double class_fg_likelihood = class_likelihood*pi->posterior(0)*pi_k->posterior(0)*comp_class_prior*comp_prior_f;
        double non_class_fg_likelihood = non_class_likelihood*pi->posterior(1)*pi_k->posterior(1)*comp_non_class_prior*comp_prior_f;
        double class_bg_likelihood = class_likelihood*pi->posterior(2)*pi_k->posterior(2)*comp_class_prior*(1.0f-comp_prior_f);
        double non_class_bg_likelihood = non_class_likelihood*pi->posterior(3)*pi_k->posterior(3)*comp_non_class_prior*(1.0f-comp_prior_f);
  */        
        double class_fg_likelihood = class_likelihood*pi->posterior(0)*pi_k->posterior(0)*0.3f;
        double non_class_fg_likelihood = non_class_likelihood*pi->posterior(1)*pi_k->posterior(1)*0.05f;
        double class_bg_likelihood = class_likelihood*pi->posterior(2)*pi_k->posterior(2)*0.15f;
        double non_class_bg_likelihood = non_class_likelihood*pi->posterior(3)*pi_k->posterior(3)*0.50f;
        
        
        double den = class_fg_likelihood + non_class_fg_likelihood + class_bg_likelihood + non_class_bg_likelihood;

        //if ((pi->pt().x() == 463 && pi->pt().y() == 272))
        //  vcl_cout << "\t \t class_fg_likelihood: " << class_fg_likelihood << " non_class_fg_like: " << non_class_fg_likelihood << " den: " << den << vcl_endl;

        if (den > vcl_numeric_limits<double>::epsilon()) {
          ci->push_posterior(class_fg_likelihood/den);  
          ci->push_posterior(non_class_fg_likelihood/den);
          ci->push_posterior(class_bg_likelihood/den);
          ci->push_posterior(non_class_bg_likelihood/den);
          out_c->add_part(ci);
        }
      }
      //vcl_cout << "\n";
    }

    pi = part1_c->next();
  }

  return out_c;
}

//: method to train a compositional part's geometric models
bool dbrec_pairwise_discrete_compositor::train_instances(dbrec_context_factory_sptr cf, const vcl_vector<dbrec_part_sptr>& children, float radius)
{
  if (children.size() != 2) {
    vcl_cout << "In dbrec_pairwise_discrete_compositor::train_instances() -- wrong number of children!\n";
    throw 0;
  }
  dbrec_part_sptr p1 = children[0];
  dbrec_part_sptr p2 = children[1];
  dbrec_part_context_sptr part1_c = cf->get_context(p1->type());
  dbrec_part_context_sptr part2_c = cf->get_context(p2->type());
  if (!part1_c || !part2_c) {
    vcl_cout << "In dbrec_pairwise_discrete_compositor::train_instances() -- part contexts have not been computed!\n";
    return false;
  }
  
  //: prepare this model to get a probe box of size radius around a given point
  dbrec_pairwise_indep_uniform_discrete_model nc_model(radius);

  //: now go through each instance of the first part
  dbrec_part_ins_sptr pi = part1_c->first();
  
  if (!pi) {
    vcl_cout << "In dbrec_pairwise_discrete_compositor::train_instances() -- no instances of part: " << pi->type() << " in its context!!!!!!!!!!!!\n";
    return true;
  }
  //: make sure the part's class posterior have been computed
  unsigned int class_posterior_id = 0;
  unsigned int non_class_posterior_id = pi->posteriors_size() == 2 ? 1 : 3;
    
  while (pi) {

    vgl_box_2d<float> probe_box = nc_model.get_probe_box(pi->pt()); 
    vcl_vector<dbrec_part_ins_sptr> out;
    part2_c->query(probe_box, out);
    
    for (unsigned k = 0; k < out.size(); k++) {
      dbrec_part_ins_sptr pi_k = out[k];

      if (pi == pi_k)
        continue;

      double class_mag = pi->posterior(class_posterior_id)*pi_k->posterior(class_posterior_id);
      double non_class_mag = pi->posterior(non_class_posterior_id)*pi_k->posterior(non_class_posterior_id);
      int d = dbrec_pairwise_rot_invariant_model::measure_d(pi, pi_k);
      if (model_->rho_defined()) {
        int rho = dbrec_pairwise_rot_invariant_model::measure_rho(pi, pi_k); // measures clockwise in 0,360
        if (model_->gamma_defined()) {
          int gamma = dbrec_pairwise_rot_invariant_model::measure_gamma(pi, pi_k); // measures clockwise in 0,360
          model_->update_gamma_rho_d(gamma, rho, d, class_mag, non_class_mag);     // updates histograms based on whether its intervals are shifted or not
        } else
          model_->update_rho_d(rho, d, class_mag, non_class_mag);  // updates histogram based on whether its rho interval is shifted or not
      } else
        model_->update_d(d, class_mag, non_class_mag);
    }

    pi = part1_c->next();
  }

  return true;
}
//: return self as a bxml_data_sptr
bxml_data_sptr dbrec_pairwise_discrete_compositor::xml_element() const
{
  bxml_element* data = new bxml_element("image_pairwise_discrete_compositor");
  data->append_text("\n");
  bxml_data_sptr model_d = model_->xml_element();
  data->append_data(model_d);
  data->append_text("\n");
  return data;
}

//: read self from the bxml_data_sptr
dbrec_compositor_sptr dbrec_pairwise_discrete_compositor::xml_parse_element(bxml_data_sptr data)
{
  dbrec_compositor_sptr comp;
  bxml_element query("image_pairwise_discrete_compositor");
  bxml_data_sptr base_root = bxml_find_by_name(data, query);
  bxml_element* c_elm = dynamic_cast<bxml_element*>(base_root.ptr());
  if (!base_root || !c_elm)
    return comp;
  dbrec_pairwise_model_sptr class_model;
  for (bxml_element::const_data_iterator it = c_elm->data_begin(); it != c_elm->data_end(); it++) {
    if ((*it)->type() != bxml_element::ELEMENT)
      continue;
    class_model = dbrec_pairwise_rot_invariant_discrete_model::xml_parse_element(*it);
    if (class_model)
      break;
  }
  if (!class_model)
    return comp;
  dbrec_pairwise_rot_invariant_discrete_model_sptr m = dynamic_cast<dbrec_pairwise_rot_invariant_discrete_model*>(class_model.ptr());
  if (!m)
    return comp;
  comp = new dbrec_pairwise_discrete_compositor(m);
  return comp;
}

// generate an initial pool of compositional parts from the parts right below each class node (root) in the hieararchy
//dbrec_hierarchy_sptr populate_class_based_pairwise_discrete_compositor(dbrec_hierarchy_sptr orig_hierarchy, int radius, int gamma_interval, int d_interval, int rho_interval)
dbrec_hierarchy_sptr populate_class_based_pairwise_discrete_compositor(dbrec_hierarchy_sptr orig_hierarchy, int radius, float angle_increment, int d_interval, int rho_interval)
{
  //: use an id factory and register the parts to create unique ids for the new parts
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();
  orig_hierarchy->register_parts(ins);

  int gamma_interval = (int)angle_increment;

  dbrec_hierarchy_sptr new_h = new dbrec_hierarchy();
  for (unsigned c = 0; c < orig_hierarchy->class_cnt(); c++) {
    dbrec_part_sptr c_node = orig_hierarchy->root(c);
    dbrec_composition* c_node_comp = dynamic_cast<dbrec_composition*>(c_node.ptr());
    vcl_vector<dbrec_part_sptr> parts = c_node_comp->children();  // this is an OR composition, use all of its parts to generate new compositional nodes
    
    //: first compute the direction vectors and sym angles if any
    vcl_vector<bool> has_direction_vector(parts.size(), false);
    vcl_vector<int> sym_angles(parts.size(), -1);

    for (unsigned i = 0; i < parts.size(); i++) {
      dbrec_has_direction_vector_visitor hdv;
      parts[i]->accept(&hdv);
      has_direction_vector[i] = hdv.has_it_;
      if (has_direction_vector[i]) {
        dbrec_get_symmetry_angle_visitor sav;
        parts[i]->accept(&sav);
        sym_angles[i] = sav.angle_;
      }
    }
    
    
    vcl_vector<dbrec_part_sptr> part_pool;  
    for (unsigned i = 0; i < parts.size(); i++) {
      dbrec_part_sptr p1 = parts[i];
      
      for (unsigned j = i; j < parts.size(); j++) {
        dbrec_part_sptr p2 = parts[j];

        //: check if gamma is defined for this pair
        if (has_direction_vector[j] && !has_direction_vector[i])  // this is not a valid pair 
          continue;

        bool rho_defined = false;
        bool gamma_defined = false;
        int gamma_range = 0;
        int rho_range = 0;
        if (has_direction_vector[i]) {
          rho_defined = true;
          rho_range = sym_angles[i];

          if (has_direction_vector[j]) {
            gamma_defined = true;
        
            gamma_range = vnl_rational::gcd(sym_angles[i], sym_angles[j]);
            //: find the least common multiplier as the range of rho: lcm(a,b) = |a.b|/gcd(a,b)
            rho_range = sym_angles[i]*sym_angles[j] / gamma_range;
            gamma_range = gamma_range > 360 ? 360 : gamma_range;
            rho_range = rho_range > 360 ? 360 : rho_range;
            vcl_cout << " \t found gamma_range: " << gamma_range << " rho_range: " << rho_range << vcl_endl;
          }
        }

        vcl_vector<dbrec_part_sptr> pp; pp.push_back(p1); pp.push_back(p2);

        vcl_vector<dbrec_pairwise_rot_invariant_discrete_model_sptr> prims;
        if (gamma_defined) {
/*          for (int gamma_min = 0; gamma_min < gamma_range; gamma_min += gamma_interval) {
            dbrec_pairwise_rot_invariant_discrete_model_sptr prim = 
              new dbrec_pairwise_rot_invariant_discrete_model(gamma_min, gamma_min+gamma_interval, gamma_range, 0, rho_range, rho_range/rho_interval, 0, radius, radius/d_interval);
            prims.push_back(prim);
            prim = new dbrec_pairwise_rot_invariant_discrete_model(gamma_min, gamma_min+gamma_interval, gamma_range, -rho_interval/2, rho_range-rho_interval/2, rho_range/rho_interval, 0, radius, radius/d_interval);
            prims.push_back(prim);
          }
          */
          for (int gamma_min = -gamma_interval/2; gamma_min < gamma_range-gamma_interval/2; gamma_min += gamma_interval) {
            dbrec_pairwise_rot_invariant_discrete_model_sptr prim = 
              new dbrec_pairwise_rot_invariant_discrete_model(gamma_min, gamma_min+gamma_interval, gamma_range, 0, rho_range, rho_range/rho_interval, -1, radius, (radius+1)/d_interval);
            prims.push_back(prim);
            prim = new dbrec_pairwise_rot_invariant_discrete_model(gamma_min, gamma_min+gamma_interval, gamma_range, -rho_interval/2, rho_range-rho_interval/2, rho_range/rho_interval, -1, radius, (radius+1)/d_interval);
            prims.push_back(prim);
          }
        } else if (rho_defined) {
          dbrec_pairwise_rot_invariant_discrete_model_sptr prim = 
            new dbrec_pairwise_rot_invariant_discrete_model(0, rho_range, rho_range/rho_interval, -1, radius, (radius+1)/d_interval);
          prims.push_back(prim);
          prim = new dbrec_pairwise_rot_invariant_discrete_model(-rho_interval/2, rho_range-rho_interval/2, rho_range/rho_interval, -1, radius, (radius+1)/d_interval);
          prims.push_back(prim);
        } else {
          dbrec_pairwise_rot_invariant_discrete_model_sptr prim = 
            new dbrec_pairwise_rot_invariant_discrete_model(-1, radius, (radius+1)/d_interval);
          prims.push_back(prim);
        }
        for (unsigned k = 0; k < prims.size(); k++) {
          dbrec_compositor_sptr cc = new dbrec_pairwise_discrete_compositor(prims[k]);  
          dbrec_part_sptr composition = new dbrec_composition(ins->new_type(), pp, cc, (float)radius);
          part_pool.push_back(composition);
        }
      }
    }
    dbrec_compositor_sptr oc = new dbrec_or_compositor();
    dbrec_part_sptr class_composition = new dbrec_composition(ins->new_type(), part_pool, oc, (float)radius); // the OR node is higher in the hierarchy but has the same radius
    new_h->add_root(class_composition);
  }

  return new_h;
}



//: a method to generate a new pool of compositional parts from a given pool, the given pool needs to have been trained
// for each part in the existing pool add n new parts
dbrec_hierarchy_sptr populate_class_based_central_compositor_by_sampling(dbrec_hierarchy_sptr orig_hierarchy, unsigned n)
{
  //: use an id factory and register the parts to create unique ids for the new parts
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();
  orig_hierarchy->register_parts(ins);

  dbrec_hierarchy_sptr new_h = new dbrec_hierarchy();
  for (unsigned c = 0; c < orig_hierarchy->class_cnt(); c++) {
    dbrec_part_sptr c_node = orig_hierarchy->root(c);
    dbrec_composition* c_node_comp = dynamic_cast<dbrec_composition*>(c_node.ptr());
    vcl_vector<dbrec_part_sptr> parts = c_node_comp->children();  // this is an OR composition, use all of its parts to generate new compositional nodes
    //: generate n new parts from each of its children
    vcl_vector<dbrec_part_sptr> part_pool;  
    for (unsigned i = 0; i < parts.size(); i++) {
      dbrec_composition* part_i = dynamic_cast<dbrec_composition*>(parts[i].ptr());
      if (!part_i) {
        vcl_cout << "In populate_class_based_central_compositor_by_sampling() -- the node is not a composition!\n";
        throw 0;
      }
      dbrec_central_compositor* c_i = dynamic_cast<dbrec_central_compositor*>(part_i->compositor().ptr());
      if (!c_i) {
        vcl_cout << "In populate_class_based_central_compositor_by_sampling() -- the compositor is not a central compositor!\n";
        throw 0;
      }
      vcl_vector<dbrec_pairwise_model_sptr>& mods = c_i->models();
      vcl_vector<dbrec_part_sptr>& ch = part_i->children();
      
      vcl_vector<vcl_vector<dbrec_pairwise_model_sptr> > new_mods(n, vcl_vector<dbrec_pairwise_model_sptr>());
      for (unsigned mm = 0; mm < mods.size(); mm++) {
        dbrec_pairwise_discrete_model* m = dynamic_cast<dbrec_pairwise_discrete_model*>(mods[mm].ptr());
        if (!m) {
          vcl_cout << "In populate_class_based_central_compositor_by_sampling() -- the pairwise model is not a discrete model!\n";
          throw 0;
        }
        vcl_vector<dbrec_pairwise_model_sptr> new_ins;
        m->sample_new_instances(n, new_ins);
        for (unsigned k = 0; k < n; k++) {
          new_mods[k].push_back(new_ins[k]);
        }
      }
      for (unsigned k = 0; k < n; k++) {
        dbrec_compositor_sptr cc = new dbrec_central_compositor(new_mods[k]);
        dbrec_part_sptr composition = new dbrec_composition(ins->new_type(), ch, cc, part_i->radius());
        part_pool.push_back(composition);
      }      
    }

    dbrec_compositor_sptr oc = new dbrec_or_compositor();
    dbrec_part_sptr class_composition = new dbrec_composition(ins->new_type(), part_pool, oc, c_node_comp->radius()); // the OR node is higher in the hierarchy but has the same radius
    new_h->add_root(class_composition);
  }

  return new_h;
}

// for each part in the existing pool add n new parts with indep gaussian as model
dbrec_hierarchy_sptr populate_class_based_central_compositor_by_indep_gaussian_model(dbrec_hierarchy_sptr orig_hierarchy, unsigned n)
{
  //: use an id factory and register the parts to create unique ids for the new parts
  dbrec_type_id_factory* ins = dbrec_type_id_factory::instance();
  orig_hierarchy->register_parts(ins);

  dbrec_hierarchy_sptr new_h = new dbrec_hierarchy();
  for (unsigned c = 0; c < orig_hierarchy->class_cnt(); c++) {
    dbrec_part_sptr c_node = orig_hierarchy->root(c);
    dbrec_composition* c_node_comp = dynamic_cast<dbrec_composition*>(c_node.ptr());
    vcl_vector<dbrec_part_sptr> parts = c_node_comp->children();  // this is an OR composition, use all of its parts to generate new compositional nodes
    //: generate n new parts from each of its children
    vcl_vector<dbrec_part_sptr> part_pool;  
    for (unsigned i = 0; i < parts.size(); i++) {
      dbrec_composition* part_i = dynamic_cast<dbrec_composition*>(parts[i].ptr());
      if (!part_i) {
        vcl_cout << "In populate_class_based_central_compositor_by_indep_gaussian_model() -- the node is not a composition!\n";
        throw 0;
      }
      dbrec_central_compositor* c_i = dynamic_cast<dbrec_central_compositor*>(part_i->compositor().ptr());
      if (!c_i) {
        vcl_cout << "In populate_class_based_central_compositor_by_indep_gaussian_model() -- the compositor is not a central compositor!\n";
        throw 0;
      }
      vcl_vector<dbrec_pairwise_model_sptr>& mods = c_i->models();
      vcl_vector<dbrec_part_sptr>& ch = part_i->children();
      
      vcl_vector<vcl_vector<dbrec_pairwise_model_sptr> > new_mods(n, vcl_vector<dbrec_pairwise_model_sptr>());
      for (unsigned mm = 0; mm < mods.size(); mm++) {
        dbrec_pairwise_discrete_model* m = dynamic_cast<dbrec_pairwise_discrete_model*>(mods[mm].ptr());
        if (!m) {
          vcl_cout << "In populate_class_based_central_compositor_by_indep_gaussian_model() -- the pairwise model is not a discrete model!\n";
          throw 0;
        }
        vcl_vector<dbrec_pairwise_model_sptr> new_ins;
        m->sample_new_indep_gaussian_instances(n, new_ins);
        for (unsigned k = 0; k < n; k++) {
          new_mods[k].push_back(new_ins[k]);
        }
      }
      for (unsigned k = 0; k < n; k++) {
        dbrec_compositor_sptr cc = new dbrec_central_compositor(new_mods[k]);
        dbrec_part_sptr composition = new dbrec_composition(ins->new_type(), ch, cc, part_i->radius());
        part_pool.push_back(composition);
      }      
    }

    dbrec_compositor_sptr oc = new dbrec_or_compositor();
    dbrec_part_sptr class_composition = new dbrec_composition(ins->new_type(), part_pool, oc, c_node_comp->radius()); // the OR node is higher in the hierarchy but has the same radius
    new_h->add_root(class_composition);
  }

  return new_h;
}






