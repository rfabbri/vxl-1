//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 05/01/09
//
//

#include "dbrec_image_visitors.h"
#include "dbrec_part_context.h"
#include <bxml/bxml_write.h>
#include <bxml/bxml_read.h>
#include <bxml/bxml_find.h>
#include <bxml/bsvg/bsvg_element.h>
#include <vil/vil_image_view.h>
#include <vil/vil_new.h>
#include <vil/vil_load.h>
#include <vil/vil_math.h>
#include <brip/brip_vil_float_ops.h>
#include "dbrec_image_models.h"
#include "dbrec_image_pairwise_models.h"
#include <vul/vul_file.h>
#include <vcl_limits.h>
#include <vnl/vnl_rational.h>
#include <vnl/vnl_vector_fixed.h>

dbrec_write_xml_visitor::dbrec_write_xml_visitor()
{
  bxml_element * struct_data = new bxml_element("structure");
  struct_data->append_text("\n");
  structural_data_ = struct_data;
  bxml_element * p_data = new bxml_element("parts");
  p_data->append_text("\n");
  part_data_ = p_data;
}
void dbrec_write_xml_visitor::write_doc(vcl_string& name) {
  bxml_document doc;
  bxml_element * root = new bxml_element("hierarchy");
  doc.set_root_element(root);
  root->append_text("\n");
  root->append_data(structural_data_);
  root->append_text("\n");
  root->append_data(part_data_);
  root->append_text("\n");
  vcl_ofstream os(name.c_str());
  bxml_write(os,doc);
  os.close();
}
void dbrec_write_xml_visitor::visit_composition(dbrec_composition* c)
{
  //: check if the part has already been written
  vcl_set<unsigned>::iterator it = part_set_.find(c->type());
  if (it == part_set_.end()) {
    //: just dump the part as a new data node
    bxml_element * data = new bxml_element("composition");
    data->append_text("\n");
    data->set_attribute("type", c->type());
    data->set_attribute("radius", c->radius());
    data->set_attribute("class_prior", c->class_prior());
    bxml_data_sptr comp_d = c->compositor()->xml_element();
    data->append_data(comp_d);
    data->append_text("\n");

    bxml_element* part_data_ptr = dynamic_cast<bxml_element*>(part_data_.ptr());
    part_data_ptr->append_data(data);
    part_data_ptr->append_text("\n");
    
    //: just dump the structure as a new data node
    bxml_element * s_data = new bxml_element("composition");
    s_data->append_text("\n");
    s_data->set_attribute("type", c->type());
    s_data->set_attribute("nchildren", c->children().size());
    vcl_stringstream ss;
    for (unsigned i = 0; i < c->children().size(); i++) 
      ss << c->children()[i]->type() << " ";
    s_data->append_text(ss.str());
    s_data->append_text("\n");
    bxml_element* str_data_ptr = dynamic_cast<bxml_element*>(structural_data_.ptr());
    str_data_ptr->append_data(s_data);
    str_data_ptr->append_text("\n");
    for (unsigned i = 0; i < c->children().size(); i++)
      c->children()[i]->accept(this);
    part_set_.insert(c->type());
  }
}

void dbrec_write_xml_visitor::visit_gaussian_primitive(dbrec_gaussian* g)
{
  //: check if the part has already been written
  vcl_set<unsigned>::iterator it = part_set_.find(g->type());
  if (it == part_set_.end()) {
    //: just dump the part as a new data node
    bxml_element * data = new bxml_element("gaussian_primitive");
    data->append_text("\n");
    data->set_attribute("type", g->type());
    data->set_attribute("lambda0", g->lambda0_);
    data->set_attribute("lambda1", g->lambda1_);
    data->set_attribute("theta", g->theta_);
    if (g->bright_) data->set_attribute("bright", 1);
    else data->set_attribute("bright", 0);
    if (g->fast_) data->set_attribute("fast", 1);
    else data->set_attribute("fast", 0);
    data->set_attribute("cutoff_percentage", g->cutoff_percentage_);
    
    bxml_element* part_data_ptr = dynamic_cast<bxml_element*>(part_data_.ptr());
    part_data_ptr->append_data(data);
    part_data_ptr->append_text("\n");
    part_set_.insert(g->type());
  }
}
bool dbrec_parse_hierarchy_xml::parse(const vcl_string& name)
{
  vcl_ifstream is(name.c_str());
  if (!is) return false;
  bxml_document doc = bxml_read(is);
  bxml_element query("hierarchy");
  bxml_data_sptr hierarchy_root = bxml_find_by_name(doc.root_element(), query);
  if (!hierarchy_root) {
    vcl_cout << "dbrec_parse_hierarchy_xml::parse() - could not find the main node with name hierarchy!\n";
    return false;
  }
  bxml_element query2("structure");
  bxml_data_sptr structural_data = bxml_find_by_name(hierarchy_root, query2);
  bxml_element* str_elm = dynamic_cast<bxml_element*>(structural_data.ptr());
  if (!structural_data || !str_elm) {
    vcl_cout << "dbrec_parse_hierarchy_xml::parse() - could not find the main node with name structure!\n";
    return false;
  }
  bxml_element query3("parts");
  bxml_data_sptr parts_data = bxml_find_by_name(hierarchy_root, query3);
  bxml_element* parts_elm = dynamic_cast<bxml_element*>(parts_data.ptr());
  if (!parts_data || !parts_elm) {
    vcl_cout << "dbrec_parse_hierarchy_xml::parse() - could not find the main node with name parts!\n";
    return false;
  }
  //: parse each part first and create part_id, part ptr map
  vcl_map<unsigned, dbrec_part_sptr> part_map;

  //: add the parsers for each type into the vector
  typedef dbrec_part_sptr (*parsers)(bxml_data_sptr);
  const unsigned parser_size = 2;
  parsers parser_array[parser_size] = {NULL};
  parser_array[0] = &parse_composition;
  parser_array[1] = &parse_gaussian_primitive;
  for (bxml_element::const_data_iterator d_it = parts_elm->data_begin(); d_it != parts_elm->data_end(); d_it++) {
    if ((*d_it)->type() == bxml_data::ELEMENT) {
      //: try to parse it with each part type
      for (unsigned i = 0; i < parser_size; i++) {
        dbrec_part_sptr part = parser_array[i](*d_it);
        if (part) {
          part_map_[part->type()] = vcl_pair<dbrec_part_sptr, bool>(part, false);
          break; // get out of the parser loop
        } 
      }
    }
  }

  //: parse the structure and build the hierarchy
  for (bxml_element::const_data_iterator s_it = str_elm->data_begin(); s_it != str_elm->data_end(); s_it++) {
    if ((*s_it)->type() == bxml_data::ELEMENT) {
      //: only the compositional nodes are in the structural section
      if (!parse_composition_structure(*s_it, part_map_))
        return false;
    }
  }
  return true;  
}
dbrec_part_sptr dbrec_parse_hierarchy_xml::parse_gaussian_primitive(bxml_data_sptr d)
{
  dbrec_part_sptr out_p;
  bxml_element query("gaussian_primitive");
  bxml_data_sptr root = bxml_find_by_name(d, query);

  if (!root || root->type() != bxml_data::ELEMENT) {
    return out_p;
  }
  bxml_element* gp_root = dynamic_cast<bxml_element*>(root.ptr());
  unsigned type; float lambda0, lambda1, theta, cutoff; unsigned bool_val; bool bright;
  gp_root->get_attribute("type", type);
  gp_root->get_attribute("lambda0", lambda0);
  gp_root->get_attribute("lambda1", lambda1);
  gp_root->get_attribute("theta", theta);
  gp_root->get_attribute("bright", bool_val);
  bool_val == 0 ? bright = false : bright = true;

  dbrec_gaussian* gp = new dbrec_gaussian(type, lambda0, lambda1, theta, bright);

  gp_root->get_attribute("fast", bool_val);
  bool_val == 0 ? gp->set_extraction_method_to_normal() : gp->set_extraction_method_to_fast();
  gp_root->get_attribute("cutoff_percentage", cutoff);
  gp->set_cutoff_percentage(cutoff);
  out_p = gp;
  return out_p;
}
dbrec_part_sptr dbrec_parse_hierarchy_xml::parse_composition(bxml_data_sptr d)
{
  dbrec_part_sptr out_p;
  bxml_element query("composition");
  bxml_data_sptr root = bxml_find_by_name(d, query);
  if (!root || root->type() != bxml_data::ELEMENT) {
    return out_p;
  }
  bxml_element* gc_root = dynamic_cast<bxml_element*>(root.ptr());
  unsigned type; float radius, class_prior;
  gc_root->get_attribute("type", type);
  gc_root->get_attribute("radius", radius);
  gc_root->get_attribute("class_prior", class_prior);
  
  dbrec_compositor_sptr compositor;
  for (bxml_element::const_data_iterator it = gc_root->data_begin(); it != gc_root->data_end(); it++) {
    if ((*it)->type() != bxml_element::ELEMENT)
      continue;
    //: try each possible compositor and put the one whichever succeeds, add new compositors to this list
    typedef dbrec_compositor_sptr (*comp_parsers)(bxml_data_sptr);
    const unsigned parser_size = 6;
    comp_parsers parser_array[parser_size] = {NULL};
    parser_array[0] = &dbrec_compositor::xml_parse_element;
    parser_array[1] = &dbrec_or_compositor::xml_parse_element;
    parser_array[2] = &dbrec_image_compositor::xml_parse_element;
    parser_array[3] = &dbrec_central_compositor::xml_parse_element;
    parser_array[4] = &dbrec_pairwise_compositor::xml_parse_element;
    parser_array[5] = &dbrec_pairwise_discrete_compositor::xml_parse_element;
    for (unsigned i = 0; i < parser_size; i++) {
      compositor = parser_array[i](*it);
      if (!compositor)
        continue;
      break;
    }
    break;
  }
  
  vcl_vector<dbrec_part_sptr> children;  // empty for now

  dbrec_composition* cp = new dbrec_composition(type, children, compositor, radius);
  cp->set_class_prior(class_prior);

  out_p = cp;
  return out_p;
}
bool dbrec_parse_hierarchy_xml::parse_composition_structure(bxml_data_sptr d, vcl_map<unsigned, vcl_pair<dbrec_part_sptr, bool> >& part_map)
{
  bxml_element query("composition");
  bxml_data_sptr root = bxml_find_by_name(d, query);
  if (!root || root->type() != bxml_data::ELEMENT) {
    return false;
  }
  bxml_element* c_root = dynamic_cast<bxml_element*>(root.ptr());
  unsigned type, nchildren;
  c_root->get_attribute("type", type);
  c_root->get_attribute("nchildren", nchildren);
  //: retrieve the part
  vcl_map<unsigned, vcl_pair<dbrec_part_sptr, bool> >::iterator it = part_map.find(type);
  if (it == part_map.end())
    return false;
  dbrec_part_sptr p = it->second.first;
  dbrec_composition* cp = dynamic_cast<dbrec_composition*>(p.ptr());
  if (!cp)
    return false;
  if (!nchildren)
    return true;  // no need to parse the rest
  //: read out the children
  for (bxml_element::const_data_iterator s_it = c_root->data_begin(); s_it != c_root->data_end(); s_it++) {
    if ((*s_it)->type() == bxml_data::TEXT) {
      bxml_text* t = dynamic_cast<bxml_text*>((*s_it).ptr());
      vcl_stringstream text_d(t->data()); vcl_string buf;
      vcl_vector<vcl_string> tokens;
      while (text_d >> buf) {
        tokens.push_back(buf);
      }
      if (tokens.size() != nchildren)
        continue;
      for (unsigned i = 0; i < nchildren; i++) {
        vcl_stringstream ss2(tokens[i]); unsigned c_type;
        ss2 >> c_type;
        it = part_map.find(c_type);
        if (it == part_map.end())
          return false;
        cp->children().push_back(it->second.first);
        it->second.second = true;  // this part has been a child of someone at least once
      }
      break;
    }
  }
  
  return true;
}
dbrec_hierarchy_sptr dbrec_parse_hierarchy_xml::get_hierarchy()
{
  dbrec_hierarchy_sptr h = new dbrec_hierarchy;
  //: check the part map and find the parts which have never been in the child list of any node, those are the root nodes of the hierarchy
  for (vcl_map<unsigned, vcl_pair<dbrec_part_sptr, bool> >::const_iterator it = part_map_.begin(); it != part_map_.end(); it++) {
    if (it->second.second)
      continue;
    dbrec_part_sptr p = it->second.first;
    dbrec_composition* cp = dynamic_cast<dbrec_composition*>(p.ptr());
    if (!cp) continue;
    h->add_root(p);
  }
  return h;
}

void dbrec_sample_2d_location_visitor::visit_or_compositor(dbrec_or_compositor* c, const vcl_vector<dbrec_part_sptr>& children)
{
  children_locs_.clear();
  children_locs_.push_back(composition_loc_);
  for (unsigned i = 1; i < children.size(); i++) {
    vgl_point_2d<float> pt(composition_loc_.x()+i*space_, composition_loc_.y());
    children_locs_.push_back(pt);
  }

}
void dbrec_sample_2d_location_visitor::visit_central_compositor(dbrec_central_compositor* g, const vcl_vector<dbrec_part_sptr>& children)
{
  children_locs_.clear();
  children_locs_.push_back(composition_loc_);
  vcl_vector<dbrec_pairwise_model_sptr>& mods = g->models();
  for (unsigned i = 0; i < mods.size(); i++) {
    vgl_point_2d<float> second_loc;
    mods[i]->sample_location(composition_loc_, scale_, children[0], second_loc);
    children_locs_.push_back(second_loc);
  }
}


void dbrec_draw_bsvg_visitor::visit_composition(dbrec_composition* c)
{
  vcl_vector<vgl_point_2d<float> > locs;
  dbrec_image_compositor* c_img_comp = dynamic_cast<dbrec_image_compositor*>(c->compositor().ptr());
  if (!c_img_comp) {
    vcl_cout << "ERROR: dbrec_composition::draw() - compositor is not an image image compositor!\n";
    return;
  }
  float rad_in_image = c->radius();
  vcl_stringstream ss; ss << "radius in image: " << rad_in_image << " pixels.";
  bsvg_text* tmm = new bsvg_text(ss.str());
  tmm->set_location(15.0f, 15.0f);
  doc_.add_element(tmm);

  scale_ = width_/(2.0f*rad_in_image);
  //c->compositor()->sample_locations(current_loc_, c->children(), locs);
  c_img_comp->sample_locations(current_loc_, scale_, c->children(), locs);
  if (locs.size() != c->children().size()) {
    vcl_cout << "ERROR: dbrec_composition::draw() - size of location vector is different from size of children vector!\n";
    return;
  }
  vcl_vector<dbrec_part_sptr>& cv = c->children();
  for (unsigned i = 0; i < cv.size(); i++) {
    current_loc_ = locs[i];
    cv[i]->accept(this);
  }
  //c->compositor()->draw_attributes(doc_, cv, locs);
  c_img_comp->draw_attributes(doc_, cv, locs);
}

void dbrec_draw_bsvg_visitor::visit_gaussian_primitive(dbrec_gaussian* g)
{
  float rx = 0.0f, ry = 0.0f;
  rx = g->lambda0_*scale_;
  ry = g->lambda1_*scale_;
  /*if (g->lambda0_ < g->lambda1_) {
    rx = vis_rad_; //lambda0_;
    ry = (vis_rad_/g->lambda0_) * g->lambda1_;
  } else {
    ry = vis_rad_;
    rx = (vis_rad_/g->lambda1_) * g->lambda0_;
  }*/
  bsvg_ellipse* ell = new bsvg_ellipse(rx, ry); 
  ell->set_location(current_loc_.x(), current_loc_.y());
  ell->set_rotation(g->theta_);
  ell->set_fill_color(color_);
  ell->set_stroke_color("yellow");
  doc_.add_element(ell);

  vcl_stringstream ss; ss << g->type(); 
  bsvg_text* t = new bsvg_text(ss.str());
  t->set_location(current_loc_.x()+2*rx, current_loc_.y()+2*ry);
  t->set_fill_color(color_);
  doc_.add_element(t);
}

void dbrec_draw_bsvg_visitor::write(const vcl_string& file)
{
  bxml_write(file, doc_);
}

void dbrec_draw_class(dbrec_hierarchy_sptr h, const vcl_string& out, unsigned class_id, float visualization_radius, const vcl_string& color)
{
  if (h->class_cnt() <= class_id)
    return;

  //: for now determine the width and use it as a measure to find document height and width, actually we should be using "activation radius"
  unsigned w = h->width();
  float width = visualization_radius * 3.0f * w;
  dbrec_draw_bsvg_visitor v(width, width, width/2.0f, width/2.0f, visualization_radius, color);
  h->root(class_id)->accept(&v);
  v.write(out);
}

void dbrec_draw_composition(dbrec_hierarchy_sptr h, const vcl_string& out, unsigned composition_type_id, float visualization_radius, const vcl_string& color)
{
  //h->print(vcl_cout);
  dbrec_part_sptr c = h->get_part(composition_type_id);
  if (!c) {
    vcl_cout << "In dbrec_draw_composition() -- cannot find the composition with type id: " << composition_type_id << vcl_endl;
    return;
  }

  //: for now determine the width and use it as a measure to find document height and width, actually we should be using "activation radius"
  unsigned w = c->width();
  float width = visualization_radius * 4.0f * w;
  dbrec_draw_bsvg_visitor v(width, width, width/2.0f, width/2.0f, visualization_radius, color);
  c->accept(&v);
  v.write(out);
}

void dbrec_get_direction_vector_visitor::visit_composition(dbrec_composition* c) 
{
  vcl_vector<dbrec_part_sptr>& ch = c->children();
  //: return the direction vector using the compositor
  dbrec_image_compositor* compositor = dynamic_cast<dbrec_image_compositor*>(c->compositor().ptr());
  compositor->direction_vector(ch, v_);
}

void dbrec_get_direction_vector_visitor::visit_gaussian_primitive(dbrec_gaussian* g)
{
  g->get_direction_vector(v_);
}

//////////////////////////////////////////
//: RECEPTIVE FIELD MARKER ON 2D IMAGES

void dbrec_mark_receptive_fields_visitor::visit_composition(dbrec_composition* c) 
{
  //: first check if the context has any parts and if their type is consistent with c
  dbrec_part_ins_sptr pi = c_->first();
  if (!pi) {
    vcl_cout << "In dbrec_mark_receptive_fields_visitor::visit_composition() -- no part instances in the context, cannot mark receptive fields!\n";
    return;
  }
  if (c->type() != pi->type()) 
    return;
  do { //: mark all the individual instances in the context for this part one by one 
    //: grep the center value and mark the whole receptive field of the instance with that value
    int i = (int)pi->pt().x();
    int j = (int)pi->pt().y();
    float val = center_map_(i, j);
    dbrec_mark_receptive_field_visitor mrfv(pi, val, out_rec_field_map_);
    c->accept(&mrfv);
    pi = c_->next();
  } while (pi);
}
//: we don't need the context of the gaussian primitive while visiting it, as we already have all the information in center_map
void dbrec_mark_receptive_fields_visitor::visit_gaussian_primitive(dbrec_gaussian* g)
{
  //: first check if the context has any parts and if their type is consistent with c
  dbrec_part_ins_sptr pi = c_->first();
  if (!pi) {
    vcl_cout << "In dbrec_mark_receptive_fields_visitor::visit_composition() -- no part instances in the context, cannot mark receptive fields!\n";
    return;
  }
  if (g->type() != pi->type()) 
    return;
  do { //: mark all the individual instances in the context for this part one by one 
    //: grep the center value and mark the whole receptive field of the instance with that value
    int i = (int)pi->pt().x();
    int j = (int)pi->pt().y();
    float val = center_map_(i, j);
    dbrec_mark_receptive_field_visitor mrfv(pi, val, out_rec_field_map_);
    g->accept(&mrfv);
    pi = c_->next();
  } while (pi);
}
//: create an image with 3 planes, where the red channel is the original image, and receptive field is in blue channel so the centers are cyan color
void dbrec_mark_receptive_fields_visitor::get_colored_img(const vil_image_view<vxl_byte>& img, vil_image_view<vxl_byte>& out)
{
  unsigned ni = img.ni(); unsigned nj = img.nj();
  if (ni != out.ni() || nj != out.nj() || ni != out_rec_field_map_.ni() || nj != out_rec_field_map_.nj() || out.nplanes() != 3) {
    vcl_cout << "In dbrec_mark_receptive_field_visitor::get_colored_img() -- image sizes are incompatible or input images does not have 3 planes!\n";
    return;
  }
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++) {
      out(i,j,0) = img(i,j);
      out(i,j,1) = (vxl_byte)(center_map_(i,j)*255);
      out(i,j,2) = (vxl_byte)(out_rec_field_map_(i,j)*255);
    }
}

//: a static method to create the colored image given the center map and the receptive field map
void dbrec_mark_receptive_fields_visitor::get_colored_img(const vil_image_view<vxl_byte>& img, const vil_image_view<float>& center_map, const vil_image_view<float>& rec_fields_map, vil_image_view<vxl_byte>& out)
{
  unsigned ni = img.ni(); unsigned nj = img.nj();
  if (ni != out.ni() || nj != out.nj() || 
      ni != rec_fields_map.ni() || nj != rec_fields_map.nj() || out.nplanes() != 3 ||
      ni != center_map.ni() || nj != center_map.nj()) {
    vcl_cout << "In dbrec_mark_receptive_field_visitor::get_colored_img() -- image sizes are incompatible or input images does not have 3 planes!\n";
    return;
  }
  for (unsigned i = 0; i < ni; i++)
    for (unsigned j = 0; j < nj; j++) {
      out(i,j,0) = img(i,j);
      out(i,j,1) = (vxl_byte)(center_map(i,j)*255);
      out(i,j,2) = (vxl_byte)(rec_fields_map(i,j)*255);
    }
}


void dbrec_mark_receptive_field_visitor::visit_composition(dbrec_composition* c) {
  if (c->type() != pi_->type())
    return;
  vcl_vector<dbrec_part_ins_sptr> ch_ins;
  //: we know pi_ is of type dbrec_composition_ins
  dbrec_composition_ins* cins = dynamic_cast<dbrec_composition_ins*>(pi_.ptr());
  if (!cins) {
    dbrec_rot_inv_composition_ins* cins_ri = dynamic_cast<dbrec_rot_inv_composition_ins*>(pi_.ptr());
    ch_ins = cins_ri->children();
  } else
    ch_ins = cins->children();

  vcl_vector<dbrec_part_sptr>& ch = c->children();
  //: determine which child/children needs to be recursively visited using the type ids of compositions children
  for (unsigned i = 0; i < ch_ins.size(); i++) {
    //: find the corresponding part in the child vector
    for (unsigned j = 0; j < ch.size(); j++) {
      if (ch[j]->type() == ch_ins[i]->type()) {
        dbrec_mark_receptive_field_visitor mrfv(ch_ins[i], val_, map_);
        ch[j]->accept(&mrfv);
      }
    }
  }
}
void dbrec_mark_receptive_field_visitor::visit_gaussian_primitive(dbrec_gaussian* g) {
  if (g->type() != pi_->type())
    return;

  //: check if this is a rotationally invariant instance
  dbrec_rot_inv_part_ins* rot_pi = dynamic_cast<dbrec_rot_inv_part_ins*>(pi_.ptr());
  
  vbl_array_2d<bool> mask;
  if (!rot_pi) {
    //mask = g->mask_;
    vbl_array_2d<float> kernel;
    brip_vil_float_ops::extrema_kernel_mask(g->lambda0_, g->lambda1_, g->theta_, kernel, mask, g->cutoff_percentage_);
  } else {
    //g->get_mask(mask, rot_pi->theta());
    vbl_array_2d<float> kernel;
    brip_vil_float_ops::extrema_kernel_mask(g->lambda0_, g->lambda1_, rot_pi->theta(), kernel, mask, g->cutoff_percentage_);
  }
  unsigned nrows = mask.rows();
  unsigned ncols = mask.cols();

  int i = (int)pi_->pt().x();
  int j = (int)pi_->pt().y();

  int js = (int)vcl_floor(j - (float)nrows/2.0f + 0.5f);
  int is = (int)vcl_floor(i - (float)ncols/2.0f + 0.5f);
  int je = (int)vcl_floor(j + (float)nrows/2.0f + 0.5f);
  int ie = (int)vcl_floor(i + (float)ncols/2.0f + 0.5f);

  int ni = (int)map_.ni();
  int nj = (int)map_.nj();
  if (val_ > 0.0f) {
    for (int i = is; i < ie; i++)
      for (int j = js; j < je; j++) {
        int mask_i = i - is;
        int mask_j = j - js;
        if (mask[mask_j][mask_i] && i >= 0 && j >= 0 && i < ni && j < nj) {
          /*if ((img(i, j) + val) > 1.0f)
            img(i, j) = 1.0f;
          else
            img(i, j) += val;*/
          if (map_(i,j) < val_)
            map_(i,j) = val_;
        }
      }
  }
}

void dbrec_has_direction_vector_visitor::visit_gaussian_primitive(dbrec_gaussian* g) {
  if (g->lambda0_ == g->lambda1_) has_it_ = false;
  else has_it_ = true;
}

void dbrec_get_symmetry_angle_visitor::visit_gaussian_primitive(dbrec_gaussian* g)
{
  if (g->lambda0_ == g->lambda1_) angle_ = 0;  // circles have full rot symmetry
  else angle_ = 180;       // ellipses have 180 degree rot symmetry
}
//: this method is supposed to decompose the parts into primitives and depending on the configuration decide whether a symmetry forms and what is the symmetry angle
//  but in our applications we did not need this feature yet!
void dbrec_get_symmetry_angle_visitor::visit_composition(dbrec_composition* c) 
{
  vcl_cout << "In dbrec_get_symmetry_angle_visitor::visit_composition() -- WARNING: returns 360 for all compositions!\n";
  angle_ = 360;
}

void dbrec_sample_and_draw_part_visitor::visit_composition(dbrec_composition* c) 
{
  dbrec_pairwise_compositor* pc = dynamic_cast<dbrec_pairwise_compositor*>(c->compositor().ptr());
  if (!pc) {
    dbrec_or_compositor* oc = dynamic_cast<dbrec_or_compositor*>(c->compositor().ptr());
    if (!oc) {
      vcl_cout << "dbrec_sample_and_draw_part_visitor is not designed to work with compositors other then the type: dbrec_pairwise_compositor or dbrec_or_compositor!\n";
      throw 0;
    }
    //: pick one of the children randomly and sample from it
    unsigned int ch_id = rng_.lrand32(0, c->children().size()-1);
    dbrec_part_sptr child = c->children()[ch_id];
    child->accept(this);
    return;
  }
  //: now current i,j is the center of mass for the composition. so find centers of part1 and part2
  dbrec_part_sptr p1 = c->children()[0];  // rho is always measured wrt part 1 and in clockwise direction
  dbrec_part_sptr p2 = c->children()[1];
  float m1 = p1->mass(); float m2 = p2->mass();

  //: sample a d and find p and q depending on masses
  int d = pc->model_->sample_d(rng_);
  float q = d/((m2/m1)+1.0f); float p = d-q;  // these are the lenghts of displacement from the center towards each part
  //vcl_cout << " sampled d: " << d << " q: " << q << " p: " << p << vcl_endl;

  vnl_vector_fixed<float,2> axis_vec;

  //: get the direction vector of the first part
  dbrec_has_direction_vector_visitor hdvv1; p1->accept(&hdvv1);
  if (hdvv1.has_it_) {
    vnl_vector_fixed<float,2> v;
    dbrec_get_direction_vector_visitor dv;
    p1->accept(&dv);  v = dv.v_;

    //: sample a rho
    int rho = pc->model_->sample_rho(rng_);
    //: find the total rotation around the direction vector
    int total_rot = rho+rot_angle_;
    
    //: rotate anti-clockwise
    double theta_rad = total_rot*vnl_math::pi/180.0;
    float c = (float)vcl_cos(theta_rad);
    float s = (float)vcl_sin(theta_rad);
    vnl_matrix_fixed<float, 2, 2> rot_matrix;
    rot_matrix(0,0) = c; rot_matrix(0,1) = -s; rot_matrix(1,0) = s; rot_matrix(1,1) = c;
    axis_vec = rot_matrix*v;
  } else {  // only rotate the x-axis unit vector by rotation angle
    vnl_vector_fixed<float,2> v; v(0) = 1.0f; v(1) = 0.0f;
    double theta_rad = rot_angle_*vnl_math::pi/180.0;
    float c = (float)vcl_cos(theta_rad);
    float s = (float)vcl_sin(theta_rad);
    vnl_matrix_fixed<float, 2, 2> rot_matrix;
    rot_matrix(0,0) = c; rot_matrix(0,1) = -s; rot_matrix(1,0) = s; rot_matrix(1,1) = c;
    axis_vec = rot_matrix*v;
  }
  //: now find the new centers using axis vector and the sampled translation in the image (i,j)
  int t_i = i_; int t_j = j_;  int rot_angle = rot_angle_;  // save the parameters
  i_ = (int)vcl_floor(-p*axis_vec(0)+0.5f) + t_i;
  j_ = (int)vcl_floor(-p*axis_vec(1)+0.5f) + t_j;
  p1->accept(this);

  //: the second part might be needed to be rotated by the sample alpha, so adjust its rotation angle
  rot_angle_ = rot_angle;
  //: we need the symmetry angles of the two parts
  dbrec_has_direction_vector_visitor hdvv2; p2->accept(&hdvv2);
  if (hdvv1.has_it_ && hdvv2.has_it_) {  // gamma is only defined when both parts have a direction vector
    int gamma = pc->model_->sample_gamma(rng_);   
    rot_angle_ = rot_angle + gamma;
  }
  i_ = (int)vcl_floor(q*axis_vec(0)+0.5f) + t_i;
  j_ = (int)vcl_floor(q*axis_vec(1)+0.5f) + t_j;
  p2->accept(this);
  
}
void dbrec_sample_and_draw_part_visitor::visit_gaussian_primitive(dbrec_gaussian* g)
{
  vbl_array_2d<bool> mask;
  vbl_array_2d<float> kernel;
  float new_theta = g->theta_+rot_angle_;  // rotation is just summation with the absolute angle (means clockwise rotation of the extrema operator)
  new_theta = brip_vil_float_ops::extrema_revert_angle(new_theta);
  //: alyways set the cutoff to 0.5f, extrema operators kernel contains positive coefficients at the center and negatives at the surrounding, 
  //  prepare image smaller than the kernel (such that the image coincides with the positive coefficients.)
  brip_vil_float_ops::extrema_kernel_mask(g->lambda0_, g->lambda1_, new_theta, kernel, mask, 0.5f);
  unsigned nrows = mask.rows();
  unsigned ncols = mask.cols();
  int js = (int)vcl_floor(j_ - (float)nrows/2.0f + 0.5f);
  int is = (int)vcl_floor(i_ - (float)ncols/2.0f + 0.5f);
  int je = (int)vcl_floor(j_ + (float)nrows/2.0f + 0.5f);
  int ie = (int)vcl_floor(i_ + (float)ncols/2.0f + 0.5f);
  int ni = (int)image_.ni();
  int nj = (int)image_.nj();
  vxl_byte val = (vxl_byte)(strength_*255);  // strength is a value in [0,1]
  val = g->bright_ ? val : 255-val;  // a very strong dark response will be closer to zero
  for (int i = is; i < ie; i++)
    for (int j = js; j < je; j++) {
      int mask_i = i - is;
      int mask_j = j - js;
      if (mask[mask_j][mask_i] && i >= 0 && j >= 0 && i < ni && j < nj) {
        image_(i,j) = val;
      }
    }
}

void dbrec_measure_fg_prob_visitor::visit_composition(dbrec_composition* c) {
  //: now current i,j is the center of mass for the composition. so find centers of part1 and part2
  dbrec_part_sptr p1 = c->children()[0];  // rho is always measured wrt part 1 and in clockwise direction
  dbrec_part_sptr p2 = c->children()[1];
  
  dbrec_rot_inv_composition_ins* rpi = dynamic_cast<dbrec_rot_inv_composition_ins*>(pi_.ptr());
  if (!rpi) {
    vcl_cout << " In dbrec_measure_fg_prob_visitor::visit_composition() - wrong type of instance pointer!\n";
    throw 0;
  }
  dbrec_part_ins_sptr p1i = rpi->children()[0];
  dbrec_part_ins_sptr p2i = rpi->children()[1];

  float sum = 0.0f;
  pi_ = p1i;
  p1->accept(this);
  sum += prob_;

  pi_ = p2i;
  p2->accept(this);
  sum += prob_;

  prob_ = sum / 2.0f;
}

void dbrec_measure_fg_prob_visitor::visit_gaussian_primitive(dbrec_gaussian* g) {
  vbl_array_2d<bool> mask;
  dbrec_rot_inv_part_ins* rpi = dynamic_cast<dbrec_rot_inv_part_ins*>(pi_.ptr());
  g->get_mask(mask, rpi->theta());
  unsigned nrows = mask.rows();
  unsigned ncols = mask.cols();
  unsigned ii = (unsigned)rpi->pt().x();
  unsigned jj = (unsigned)rpi->pt().y();
  int js = (int)vcl_floor(jj - (float)nrows/2.0f + 0.5f);
  int is = (int)vcl_floor(ii - (float)ncols/2.0f + 0.5f);
  int je = (int)vcl_floor(jj + (float)nrows/2.0f + 0.5f);
  int ie = (int)vcl_floor(ii + (float)ncols/2.0f + 0.5f);
  int ni = (int)fg_prob_.ni();
  int nj = (int)fg_prob_.nj();
  float sum = 0.0f;
  int cnt = 0;
  for (int i = is; i < ie; i++)
    for (int j = js; j < je; j++) {
      int mask_i = i - is;
      int mask_j = j - js;
      if (mask[mask_j][mask_i] && i >= 0 && j >= 0 && i < ni && j < nj) {
        cnt++;
        sum += fg_prob_(i,j);
      }
    }
  prob_ = sum/cnt;
}

/*  int i_;
  int j_;
  float rot_angle_; // in degrees
  vil_image_view<float> fg_prob_;
*/


///////////////////////////////////////////////
//: helper to set the appearance models of gaussian primitives
void dbrec_parse_image_visitor_helper::visit_composition(dbrec_composition* c)
{
  vcl_vector<dbrec_part_sptr>& ch = c->children();
  for (unsigned i = 0; i < ch.size(); i++) {
    ch[i]->accept(this);
  }
}
void dbrec_parse_image_visitor_helper::visit_gaussian_primitive(dbrec_gaussian* g)
{
  //: if no model files are found don't do anything, the models are not available!
  vcl_string str_id = g->string_identifier();
  //vcl_string text_file = fg_app_model_path_ + str_id + "_fg_params_dbrec.txt";
  vcl_string text_file = fg_app_model_path_ + str_id + file_fg_suffix;
  
  if (!vul_file::exists(text_file)) {
    vcl_cout << "WARNING: dbrec_parse_image_visitor_helper::visit_gaussian_primitive() -- app models in: " << text_file << " cannot be found!\n";
    return;
  }

  vcl_ifstream ifs(text_file.c_str());
  double k1, lambda1, k2, lambda2;
  ifs >> k1; ifs >> lambda1;
  ifs >> k2; ifs >> lambda2;
  dbrec_gaussian_appearance_model_sptr m_class = new dbrec_gaussian_weibull_appearance_model(float(lambda1), float(k1));
  dbrec_gaussian_appearance_model_sptr m_non_class = new dbrec_gaussian_weibull_appearance_model(float(lambda2), float(k2));
  vcl_vector<dbrec_gaussian_appearance_model_sptr> models;
  models.push_back(m_class); models.push_back(m_non_class);
  
  g->set_models(models);
}
///////////////////////////////////////////////
float dbrec_parse_image_visitor::marginal_value(const vcl_vector<dbrec_gaussian_appearance_model_sptr>& app_models, const vcl_vector<dbrec_prior_model_sptr>& priors, float res, unsigned i, unsigned j)
{
  float sum = 0.0f;
  for (unsigned ii = 0; ii < app_models.size(); ii++) {
    float val = app_models[ii]->prob_density(res, i, j)*priors[ii]->prior(i, j);
    sum += val;
  }
  return sum;
}

void dbrec_parse_image_visitor::initialize(const vcl_string& model_path, float class_prior) {
  //: use the helper visitor to initialize appearance models of the gaussians
  dbrec_parse_image_visitor_helper hv(model_path);
  for (unsigned i = 0; i < h_->class_cnt(); i++) {
    h_->root(i)->accept(&hv);
  }

  //: this parser uses 2 models for the gaussian primitives, so it needs 2 priors
  //: set up the prior models to be used for parsing
  dbrec_prior_model_sptr cpm = new dbrec_constant_prior_model(class_prior);
  dbrec_prior_model_sptr ncpm = new dbrec_constant_prior_model(1.0f-class_prior);
  priors_.push_back(cpm);
  priors_.push_back(ncpm);
}

dbrec_parse_image_visitor::dbrec_parse_image_visitor(dbrec_hierarchy_sptr h, 
                                                     vil_image_resource_sptr img, 
                                                     float class_prior, const vcl_vector<float>& comp_priors,
                                                     const vcl_string& model_path) : h_(h), img_(img), fg_model_path_(model_path), composition_priors_(comp_priors)
{
  //: use rtree contexts
  cf_ = new dbrec_rtree_context_factory;
  initialize(model_path, class_prior);
}
//: if a context factory is available for the image for some parts of the hierarchy, use it to avoid re-extraction
dbrec_parse_image_visitor::dbrec_parse_image_visitor(dbrec_hierarchy_sptr h, 
                                                     dbrec_context_factory_sptr cf, 
                                                     vil_image_resource_sptr img, 
                                                     float class_prior, const vcl_vector<float>& comp_priors, 
                                                     const vcl_string& model_path) : h_(h), img_(img), fg_model_path_(model_path), composition_priors_(comp_priors), cf_(cf)
{
  initialize(model_path, class_prior);
}

void dbrec_parse_image_visitor::visit_gaussian_primitive(dbrec_gaussian* g)
{
  dbrec_part_context_sptr c = cf_->get_context(g->type());
  if (!c) { // if not populated yet
    //: first make sure that this g can do the parsing and has the models necessary
    if (g->models_.size() < 2) {
      vcl_cout << "In dbrec_parse_image_visitor::visit_gaussian_primitive() -- part: " << g->type() << " appearance models have not been set, only finding the strength map!\n";
    } //else {
      //vcl_cout << "In dbrec_parse_image_visitor::visit_gaussian_primitive() -- part: " << g->type() << " appearance models have been set, finding the strength map and the posteriors!\n";
    //}
    c = cf_->new_context();
    cf_->add_context(g->type(), c);
    vil_image_view<float> fimg = img_->get_view();
    vil_image_view<float> extr;
    if (g->fast()) 
      extr = brip_vil_float_ops::fast_extrema(fimg, g->lambda0_, g->lambda1_, g->theta_, g->bright_, true);
    else
      extr = brip_vil_float_ops::extrema(fimg, g->lambda0_, g->lambda1_, g->theta_, g->bright_, true);

    if (extr.nplanes() < 2) {
      vcl_cout << "In dbrec_parse_image_visitor::visit_gaussian_primitive() -- Error in extracting gaussian primitive from image!\n";
      return;
    }

    unsigned ni = fimg.ni();
    unsigned nj = fimg.nj();

    vil_image_view<float> res(ni, nj), mask(ni, nj);
    for (unsigned j = 0; j<nj; ++j) {
      for (unsigned i = 0; i<ni; ++i)
      {
        res(i,j) = extr(i,j,0);
        mask(i,j) = extr(i,j,1);
      }
    }

    //: add the strength map to be utilized by various context operations
    c->add_map(vil_new_image_resource_of_view(res));
    c->add_map(vil_new_image_resource_of_view(mask));

    for (unsigned j = 0; j<nj; ++j) {
      for (unsigned i = 0; i<ni; ++i)
      {
        if (res(i,j) > 1.0e-3f) {
          if (i == 796 && j == 545)
            vcl_cout << "here!\n";

          float str = res(i,j);
          float marginal = marginal_value(g->models_, priors_, res(i,j), i, j);
          if (marginal < vcl_numeric_limits<float>::epsilon())
            continue;
          dbrec_part_ins_sptr dp = new dbrec_part_ins(g->type(), (float)i, (float)j);
          c->add_part(dp);
          for (unsigned ii = 0; ii < g->models_.size(); ii++) {
            float prob = (g->models_)[ii]->prob_density(res(i,j), i, j);
            float prior = priors_[ii]->prior(i, j);
            float likelihood = (g->models_)[ii]->prob_density(res(i,j), i, j)*priors_[ii]->prior(i,j);
            dp->push_posterior(likelihood/marginal);
          }
        }
      } 
    }
  }
}
void dbrec_parse_image_visitor::visit_composition(dbrec_composition* c)
{
  dbrec_part_context_sptr pc = cf_->get_context(c->type());
  if (!pc) {
    //: first parse the children
    vcl_vector<dbrec_part_sptr>& ch = c->children();
    for (unsigned i = 0; i < ch.size(); i++) {
      ch[i]->accept(this);
    }

    //vcl_cout << "In dbrec_parse_image_visitor::visit_composition) -- part: " << c->type() << " finding the posteriors!\n";

    //: now, given this
    dbrec_image_compositor* compositor = dynamic_cast<dbrec_image_compositor*>(c->compositor().ptr());
    if (!compositor) {
      vcl_cout << "In dbrec_parse_image_visitor::visit_composition() -- cannot cast compositor* to image_compositor* !\n";
      throw 0;
    }
    assert(composition_priors_.size() == 2); 
    
    dbrec_part_context_sptr c_context = compositor->detect_instances(c->type(), cf_, c->children(), composition_priors_, c->radius());
    if (!c_context) {
      vcl_cout << "In dbrec_parse_image_visitor::visit_composition() -- cannot detect instances of composition with type: " << c->type() << "!\n";
      throw 0;
    }
    cf_->add_context(c->type(), c_context);
  }
}

/*
void dbrec_parse_image_with_fg_map_visitor::initialize_priors(const vil_image_view<float>& fg_prob, float class_prior)
{
  priors_.clear();
  //: this parser uses 4 models for the gaussian primitives, so it needs 4 priors
  //: set up the prior models to be used for parsing
  dbrec_prior_model_sptr cpm = new dbrec_constant_prior_model(class_prior);
  dbrec_prior_model_sptr ncpm = new dbrec_constant_prior_model(1.0f-class_prior);
  dbrec_prior_model_sptr fgpm = new dbrec_loc_prior_model(fg_prob);
  vil_image_view<float> bg_prob(fg_prob.ni(), fg_prob.nj());
  vil_image_view<float> tmp(fg_prob.ni(), fg_prob.nj()); tmp.fill(1.0f);
  vil_math_image_difference(tmp, fg_prob, bg_prob);
  dbrec_prior_model_sptr bgpm = new dbrec_loc_prior_model(bg_prob);
  
  //: we want to model class-foreground, non-class-foreground, class-background, non-class background appearance 
  //  it is important that prior models vector contain the prior corresponding to the order in the models vector of gaussian primitives
  priors_.push_back(new dbrec_indep_prior_model(cpm, fgpm));
  priors_.push_back(new dbrec_indep_prior_model(ncpm, fgpm));
  priors_.push_back(new dbrec_indep_prior_model(cpm, bgpm));
  priors_.push_back(new dbrec_indep_prior_model(ncpm, bgpm));
}
*/
dbrec_parse_image_with_fg_map_visitor::dbrec_parse_image_with_fg_map_visitor(dbrec_hierarchy_sptr h, 
                                                                             vil_image_resource_sptr img, 
                                                                             const vil_image_view<float>& fg_prob, 
                                                                             float class_prior,
                                                                             const vcl_vector<float>& comp_priors,
                                                                             const vcl_string& fg_model_path, const vcl_string& bg_model_path) 
                                                                             : dbrec_parse_image_visitor(h, img, class_prior, comp_priors, fg_model_path), fg_prob_(fg_prob), class_prior_(class_prior), bg_model_path_(bg_model_path)
{
  //initialize_priors(fg_prob, class_prior);
}
//: if a context factory is available for the image for some parts of the hierarchy, use it to avoid re-extraction
dbrec_parse_image_with_fg_map_visitor::dbrec_parse_image_with_fg_map_visitor(dbrec_hierarchy_sptr h, 
                                                                             dbrec_context_factory_sptr cf, 
                                                                             vil_image_resource_sptr img, 
                                                                             const vil_image_view<float>& fg_prob,
                                                                             float class_prior, 
                                                                             const vcl_vector<float>& comp_priors, 
                                                                             const vcl_string& fg_model_path, const vcl_string& bg_model_path)
                                                                             : dbrec_parse_image_visitor(h, cf, img, class_prior, comp_priors, fg_model_path), fg_prob_(fg_prob), class_prior_(class_prior), bg_model_path_(bg_model_path)
{
  //initialize_priors(fg_prob, class_prior);
}

void dbrec_parse_image_with_fg_map_visitor::visit_gaussian_primitive(dbrec_gaussian* g)
{
  //: prepare the priors wrt to this gaussian primitive
  priors_.clear();
  //: this parser uses 4 models for the gaussian primitives, so it needs 4 priors
  //: set up the prior models to be used for parsing
  dbrec_prior_model_sptr cpm = new dbrec_constant_prior_model(class_prior_);
  dbrec_prior_model_sptr ncpm = new dbrec_constant_prior_model(1.0f-class_prior_);

  //: now dilate the fg_map with the mask of this gaussian
  vil_image_view<float> fg_prob_out(fg_prob_.ni(), fg_prob_.nj());
  fg_prob_out.fill(0.0f);
  g->fg_map_dilate(fg_prob_, fg_prob_out);

  dbrec_prior_model_sptr fgpm = new dbrec_loc_prior_model(fg_prob_out);
  vil_image_view<float> bg_prob(fg_prob_.ni(), fg_prob_.nj());
  vil_image_view<float> tmp(fg_prob_.ni(), fg_prob_.nj()); tmp.fill(1.0f);
  vil_math_image_difference(tmp, fg_prob_out, bg_prob);
  dbrec_prior_model_sptr bgpm = new dbrec_loc_prior_model(bg_prob);
  
  //: we want to model class-foreground, non-class-foreground, class-background, non-class background appearance 
  //  it is important that prior models vector contain the prior corresponding to the order in the models vector of gaussian primitives
  priors_.push_back(new dbrec_indep_prior_model(cpm, fgpm));
  priors_.push_back(new dbrec_indep_prior_model(ncpm, fgpm));
  priors_.push_back(new dbrec_indep_prior_model(cpm, bgpm));
  priors_.push_back(new dbrec_indep_prior_model(ncpm, bgpm));

  //: first check if the bg mu and sigma images have been computed for this primitive
  vcl_string str_id = g->string_identifier();
  vcl_string mu_file = bg_model_path_ + str_id + file_bg_mu_suffix;
  vcl_string sigma_file = bg_model_path_ + str_id + file_bg_sigma_suffix;
  if (!vul_file::exists(mu_file) || !vul_file::exists(sigma_file)) {  // error
    vcl_cout << "ERROR: In dbrec_parse_image_with_fg_map_visitor::visit_gaussian_primitive() -- bg mu and sigma images:\n" << mu_file << vcl_endl;
    vcl_cout << "and:\n" << sigma_file << vcl_endl;
    vcl_cout << "have not been computed and saved in:\n " << bg_model_path_ << "!\n";
    return;
  }
  vil_image_view<float> mu_img = vil_load(mu_file.c_str());
  vil_image_view<float> sigma_img = vil_load(sigma_file.c_str());
  dbrec_gaussian_appearance_model_sptr bgam = new dbrec_gaussian_bg_appearance_model(mu_img, sigma_img);

  //: push the background appearance model temporarily to compute 4 posteriors
  g->models_.push_back(bgam);
  g->models_.push_back(bgam);
  dbrec_parse_image_visitor::visit_gaussian_primitive(g);
  //: remove the background appearance model
  g->models_.pop_back();
  g->models_.pop_back();
}
void dbrec_parse_image_with_fg_map_visitor::visit_composition(dbrec_composition* c)
{
  dbrec_part_context_sptr pc = cf_->get_context(c->type());
  if (!pc) {
    //: first parse the children
    vcl_vector<dbrec_part_sptr>& ch = c->children();
    for (unsigned i = 0; i < ch.size(); i++) {
      ch[i]->accept(this);
    }

    vcl_cout << "In dbrec_parse_image_with_fg_map_visitor::visit_composition) -- part: " << c->type() << " finding the posteriors!\n";

    //: now, given this
    dbrec_image_compositor* compositor = dynamic_cast<dbrec_image_compositor*>(c->compositor().ptr());
    if (!compositor) {
      vcl_cout << "In dbrec_parse_image_with_fg_map_visitor::visit_composition() -- cannot cast compositor* to image_compositor* !\n";
      throw 0;
    }
    
    //: prepare the composition priors vector, 
    // assuming this visitor always uses the passed in fixed values for the priors
    // Ozge TODO: fix this so that it is possible to also use the prior saved in c, the compositional in the hierarchy (e.g. during training)
    assert(composition_priors_.size() == 4);  
    
    dbrec_part_context_sptr c_context = compositor->detect_instances(c->type(), cf_, c->children(), composition_priors_, c->radius());
    if (!c_context) {
      vcl_cout << "In dbrec_parse_image_with_fg_map_visitor::visit_composition() -- cannot detect instances of composition with type: " << c->type() << "!\n";
      throw 0;
    }
    cf_->add_context(c->type(), c_context);
  
  }
}

//: run a non-maximum supression across detection maps of the same gaussian with a range of absolute orientations
//  if the gaussian is circular then no need to do that
void dbrec_parse_image_rot_inv_visitor::visit_gaussian_primitive(dbrec_gaussian* g) {
  dbrec_part_context_sptr c = cf_->get_context(g->type());
  if (!c) { // if not populated yet
    //: first make sure that this g can do the parsing and has the models necessary
    if (g->models_.size() < 2) {
      vcl_cout << "In dbrec_parse_image_visitor::visit_gaussian_primitive() -- part: " << g->type() << " appearance models have not been set, only finding the strength map!\n";
    } //else {
      //vcl_cout << "In dbrec_parse_image_visitor::visit_gaussian_primitive() -- part: " << g->type() << " appearance models have been set, finding the strength map and the posteriors!\n";
    //}
    c = cf_->new_context();
    cf_->add_context(g->type(), c);
    vil_image_view<float> fimg = img_->get_view();
    vil_image_view<float> extr = brip_vil_float_ops::extrema_rotational(fimg, g->lambda0_, g->lambda1_, theta_inc_, g->bright_);

    unsigned ni = fimg.ni();
    unsigned nj = fimg.nj();

    vil_image_view<float> res(ni, nj), res_angle(ni, nj), res_mask(ni, nj);
    for (unsigned j = 0; j<nj; ++j) {
      for (unsigned i = 0; i<ni; ++i)
      {
        res(i,j) = extr(i,j,0);
        res_angle(i,j) = extr(i,j,1);
        res_mask(i,j) = extr(i,j,2);
      }
    }

    //: add the strength map to be utilized by various context operations
    c->add_map(vil_new_image_resource_of_view(res));
    c->add_map(vil_new_image_resource_of_view(res_mask));
    c->add_map(vil_new_image_resource_of_view(res_angle));

    for (unsigned j = 0; j<nj; ++j) {
      for (unsigned i = 0; i<ni; ++i)
      {
        if (!valid_region_mask_(i,j))
          continue;

        if (res(i,j) > 1.0e-3f) {
          float str = res(i,j);
          float marginal = dbrec_parse_image_visitor::marginal_value(g->models_, priors_, res(i,j), i, j);
          if (marginal < vcl_numeric_limits<float>::epsilon())
            continue;
          vnl_vector_fixed<float, 2> dir_vec;
          g->get_direction_vector(res_angle(i,j), dir_vec);
          dbrec_part_ins_sptr dp = new dbrec_rot_inv_part_ins(g->type(), (float)i, (float)j, res_angle(i,j), dir_vec);
          c->add_part(dp);
          for (unsigned ii = 0; ii < g->models_.size(); ii++) {
            float prob = (g->models_)[ii]->prob_density(res(i,j), i, j);
            float prior = priors_[ii]->prior(i, j);
            float likelihood = (g->models_)[ii]->prob_density(res(i,j), i, j)*priors_[ii]->prior(i,j);
            dp->push_posterior(likelihood/marginal);
          }
        }
      } 
    }
  }
}

void dbrec_parse_image_rot_inv_visitor::visit_composition(dbrec_composition* c)
{

  dbrec_part_context_sptr pc = cf_->get_context(c->type());
  if (!pc) {
    //: first parse the children
    vcl_vector<dbrec_part_sptr>& ch = c->children();
    for (unsigned i = 0; i < ch.size(); i++) {
      ch[i]->accept(this);
    }
    
    dbrec_pairwise_compositor* compositor = dynamic_cast<dbrec_pairwise_compositor*>(c->compositor().ptr());
    if (!compositor) {
      dbrec_or_compositor* or_comp = dynamic_cast<dbrec_or_compositor*>(c->compositor().ptr());
      if (!or_comp) {
        vcl_cout << "In dbrec_parse_image_rot_inv_visitor::visit_composition() -- cannot cast compositor* to either pairwise_compositor* nor or_compositor* !\n";
        throw 0;
      }
      dbrec_part_context_sptr or_context = or_comp->detect_instances(c->type(), cf_, c->children(), vcl_vector<float>(), c->radius());
      cf_->add_context(c->type(), or_context);
      return;
    }
    if (c->class_prior() < 0.0) {
      vcl_cout << "In dbrec_parse_image_rot_inv_visitor::visit_composition() -- cannot parse this composition as its prior is not set!\n";
      throw 0;
    }
    dbrec_part_context_sptr c_context = compositor->detect_instances_rot_inv(c->type(), cf_, c->children(), c->class_prior(), c->radius());
    if (!c_context) {
      vcl_cout << "In dbrec_parse_image_rot_inv_visitor::visit_composition() -- cannot detect instances of composition with type: " << c->type() << "!\n";
      throw 0;
    }
    cf_->add_context(c->type(), c_context);
  }
}

void dbrec_parse_image_rot_inv_with_fg_map_visitor::initialize(const vcl_string& fg_model_path)
{
  dbrec_parse_image_visitor_helper hv(fg_model_path);
  for (unsigned i = 0; i < h_->class_cnt(); i++) {
    h_->root(i)->accept(&hv);
  }
}
dbrec_parse_image_rot_inv_with_fg_map_visitor::dbrec_parse_image_rot_inv_with_fg_map_visitor(dbrec_hierarchy_sptr h, vil_image_resource_sptr img, 
    const vil_image_view<bool>& valid_region_mask, const vil_image_view<float>& fg_prob, float class_prior, float theta_inc, const vcl_string& fg_model_path, const vcl_string& bg_model_path) : 
    h_(h), img_(img), fg_model_path_(fg_model_path), bg_model_path_(bg_model_path), fg_prob_(fg_prob), valid_region_mask_(valid_region_mask), class_prior_(class_prior), theta_inc_(theta_inc)
{
  cf_ = new dbrec_rtree_context_factory;
  initialize(fg_model_path);
}
  
dbrec_parse_image_rot_inv_with_fg_map_visitor::dbrec_parse_image_rot_inv_with_fg_map_visitor(dbrec_hierarchy_sptr h, dbrec_context_factory_sptr cf, vil_image_resource_sptr img, const vil_image_view<bool>& valid_region_mask,
    const vil_image_view<float>& fg_prob, float class_prior, float theta_inc, const vcl_string& fg_model_path, const vcl_string& bg_model_path) : 
    h_(h), cf_(cf), img_(img), fg_model_path_(fg_model_path), bg_model_path_(bg_model_path), fg_prob_(fg_prob), valid_region_mask_(valid_region_mask), class_prior_(class_prior), theta_inc_(theta_inc)
{
  initialize(fg_model_path);
}

dbrec_gaussian_appearance_model_sptr dbrec_parse_image_rot_inv_with_fg_map_visitor::get_map(
      vcl_map<float, dbrec_gaussian_appearance_model_sptr>& bg_model_map, float angle, dbrec_gaussian* g) 
{
  dbrec_gaussian_appearance_model_sptr map;
  vcl_map<float, dbrec_gaussian_appearance_model_sptr>::iterator it = bg_model_map.find(angle);
  if (it == bg_model_map.end()) {
    //: first check if the bg mu and sigma images have been computed for this primitive
    vcl_string str_id = g->string_identifier(angle);
    vcl_string mu_file = bg_model_path_ + str_id + file_bg_mu_suffix;
    vcl_string sigma_file = bg_model_path_ + str_id + file_bg_sigma_suffix;
    if (!vul_file::exists(mu_file) || !vul_file::exists(sigma_file)) {  // error
      vcl_cout << "ERROR: In dbrec_parse_image_with_fg_map_visitor::visit_gaussian_primitive() -- bg mu and sigma images:\n" << mu_file << vcl_endl;
      vcl_cout << "and:\n" << sigma_file << vcl_endl;
      vcl_cout << "have not been computed and saved in:\n " << bg_model_path_ << "!\n";
      throw 0;
    }
    vil_image_view<float> mu_img = vil_load(mu_file.c_str());
    vil_image_view<float> sigma_img = vil_load(sigma_file.c_str());
    map = new dbrec_gaussian_bg_appearance_model(mu_img, sigma_img);
    bg_model_map[angle] = map;
  } else 
    map = it->second;
  return map;
}

void dbrec_parse_image_rot_inv_with_fg_map_visitor::visit_gaussian_primitive(dbrec_gaussian* g) 
{
  dbrec_part_context_sptr c = cf_->get_context(g->type());
  if (!c) { // if not populated yet
    //: first make sure that this g can do the parsing and has the models necessary
    if (g->models_.size() < 2) {
      vcl_cout << "In dbrec_parse_image_visitor::visit_gaussian_primitive() -- part: " << g->type() << " appearance models have not been set, only finding the strength map!\n";
    } //else {
      //vcl_cout << "In dbrec_parse_image_visitor::visit_gaussian_primitive() -- part: " << g->type() << " appearance models have been set, finding the strength map and the posteriors!\n";
    //}
    c = cf_->new_context();
    cf_->add_context(g->type(), c);
    vil_image_view<float> fimg = img_->get_view();
    vil_image_view<float> extr = brip_vil_float_ops::extrema_rotational(fimg, g->lambda0_, g->lambda1_, theta_inc_, g->bright_);

    unsigned ni = fimg.ni();
    unsigned nj = fimg.nj();

    vil_image_view<float> res(ni, nj), res_angle(ni, nj), res_mask(ni, nj);
    for (unsigned j = 0; j<nj; ++j) {
      for (unsigned i = 0; i<ni; ++i)
      {
        res(i,j) = extr(i,j,0);
        res_angle(i,j) = extr(i,j,1);
        res_mask(i,j) = extr(i,j,2);
      }
    }

    //: add the strength map to be utilized by various context operations
    c->add_map(vil_new_image_resource_of_view(res));
    c->add_map(vil_new_image_resource_of_view(res_mask));
    c->add_map(vil_new_image_resource_of_view(res_angle));

    vcl_map<float, dbrec_gaussian_appearance_model_sptr> bg_model_map;
    float non_class_prior = 1.0f - class_prior_;
    for (unsigned j = 0; j<nj; ++j) {
      for (unsigned i = 0; i<ni; ++i)
      {
        if (!valid_region_mask_(i,j))
          continue;

        if (res(i,j) > 1.0e-3f) {
          float str = res(i,j);
          float angle = res_angle(i,j);
          
          //: calculate 4 likelihoods given this response: class-foreground, nonclass-foreground, class-background, nonclass background
          //: calculate foreground prior
          float prior_f = g->fg_prob_operator_rotational(fg_prob_, i, j, angle);
          float prior_b = 1-prior_f;

          //: calculate class likelihood given response
          float class_likelihood = (g->models_)[0]->prob_density(str, i, j);
          float non_class_likelihood = (g->models_)[1]->prob_density(str, i, j);

          //: calculate background likelihood given response
          dbrec_gaussian_appearance_model_sptr bgam = get_map(bg_model_map, angle, g);
          float bg_likelihood = bgam->prob_density(str, i, j);

          float cfl = class_likelihood*class_prior_*prior_f;
          float ncfl = non_class_likelihood*non_class_prior*prior_f;
          float cbgl = bg_likelihood*class_prior_*prior_b;
          float ncbgl = bg_likelihood*non_class_prior*prior_b;

/*          
          float cfl = class_likelihood*prior_f;
          float ncfl = non_class_likelihood*prior_f;
          float cbgl = bg_likelihood*prior_b;
          float ncbgl = bg_likelihood*prior_b;
*/
          float marginal = cfl + ncfl + cbgl + ncbgl;

          if (marginal < vcl_numeric_limits<float>::epsilon())
            continue;

          vnl_vector_fixed<float, 2> dir_vec;
          g->get_direction_vector(angle, dir_vec);
          dbrec_part_ins_sptr dp = new dbrec_rot_inv_part_ins(g->type(), (float)i, (float)j, angle, dir_vec);
          c->add_part(dp);

          dp->push_posterior(cfl/marginal);
          dp->push_posterior(ncfl/marginal);
          dp->push_posterior(cbgl/marginal);
          dp->push_posterior(ncbgl/marginal);
        }
      } 
    }
  }
}

void dbrec_parse_image_rot_inv_with_fg_map_visitor::visit_composition(dbrec_composition* c)
{

  dbrec_part_context_sptr pc = cf_->get_context(c->type());
  if (!pc) {
    //: first parse the children
    vcl_vector<dbrec_part_sptr>& ch = c->children();
    for (unsigned i = 0; i < ch.size(); i++) {
      ch[i]->accept(this);
    }

    dbrec_part_context_sptr c_context;
    dbrec_or_compositor* or_comp = dynamic_cast<dbrec_or_compositor*>(c->compositor().ptr());
    if (or_comp) {
      c_context = or_comp->detect_instances(c->type(), cf_, c->children(), vcl_vector<float>(), c->radius());
    } else {
      if (c->class_prior() < 0.0) {
        vcl_cout << "In dbrec_parse_image_rot_inv_visitor::visit_composition() -- cannot parse this composition as its prior is not set!\n";
        throw 0;
      }

      dbrec_pairwise_compositor* compositor = dynamic_cast<dbrec_pairwise_compositor*>(c->compositor().ptr());
      if (compositor) {
        c_context = compositor->detect_instances_rot_inv(c->type(), cf_, c->children(), c->class_prior(), fg_prob_, c->radius());
      } else {
        dbrec_pairwise_discrete_compositor* d_comp = dynamic_cast<dbrec_pairwise_discrete_compositor*>(c->compositor().ptr());
        if (d_comp) {
          c_context = d_comp->detect_instances_rot_inv(c->type(), cf_, c->children(), c->class_prior(), fg_prob_, c->radius());
          //c_context = d_comp->detect_instances_rot_inv(c->type(), cf_, c->children(), 0.3f, fg_prob_, c->radius());
        } else {
          vcl_cout << "In dbrec_parse_image_rot_inv_visitor::visit_composition() -- cannot detect instances of composition with type: " << c->type() << "!\n";
          throw 0;
        }
      }
    }

    cf_->add_context(c->type(), c_context);
  }
}


void dbrec_train_compositional_parts_visitor::visit_composition(dbrec_composition* c)
{
  //: first train the children
  vcl_vector<dbrec_part_sptr>& ch = c->children();
  for (unsigned i = 0; i < ch.size(); i++) {
    ch[i]->accept(this);
  }

  //: now, train this part if at the correct depth
  if (depth_ != c->depth())
    return;

  dbrec_image_compositor* compositor = dynamic_cast<dbrec_image_compositor*>(c->compositor().ptr());
  if (!compositor) {
    vcl_cout << "In dbrec_parse_image_visitor::visit_composition() -- cannot cast compositor* to image_compositor* !\n";
    throw 0;
  }
    
  if (!compositor->train_instances(cf_, c->children(), c->radius())) {
    vcl_cout << "In dbrec_train_compositional_parts_visitor::visit_composition() -- cannot train composition with type: " << c->type() << "!\n";
    throw 0;
  }
}
void dbrec_train_compositional_parts_visitor::visit_gaussian_primitive(dbrec_gaussian* g)
{
  return;  // does nothing for the primitives
}

void dbrec_train_rot_inv_compositional_parts_visitor::visit_composition(dbrec_composition* c)
{
  //: first train the children
  vcl_vector<dbrec_part_sptr>& ch = c->children();
  for (unsigned i = 0; i < ch.size(); i++) {
    ch[i]->accept(this);
  }

  //: now, train this part if at the correct depth
  if (depth_ != c->depth())
    return;

  dbrec_pairwise_discrete_compositor* compositor = dynamic_cast<dbrec_pairwise_discrete_compositor*>(c->compositor().ptr());
  if (!compositor) {
    vcl_cout << "In dbrec_train_rot_inv_compositional_parts_visitor::visit_composition() -- cannot cast compositor* to dbrec_pairwise_discrete_compositor* !\n";
    throw 0;
  }  
    
  if (!compositor->train_instances(cf_, c->children(), c->radius())) {
    vcl_cout << "In dbrec_train_rot_inv_compositional_parts_visitor::visit_composition() -- cannot train composition with type: " << c->type() << "!\n";
    throw 0;
  }
}
void dbrec_train_rot_inv_compositional_parts_visitor::visit_gaussian_primitive(dbrec_gaussian *g)
{
  return;  // does nothing for the primitives
}

