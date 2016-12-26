//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 04/23/09
//
//
#include "dbrec_part.h"
#include <bxml/bsvg/bsvg_element.h>
#include <bxml/bxml_write.h>
#include <vcl_cmath.h>
#include "dbrec_compositor.h"
#include "dbrec_visitor.h"

bool dbrec_composition::equal(const dbrec_composition& other) const
{
  if (dbrec_part::equal(other) && children_.size() == other.children_.size()) {
    
    for (unsigned i = 0; i < children_.size(); i++) {
      if (!(this->children_[i]->equal(*other.children_[i])))
        return false;
    }

    return true;
  } else
    return false;
}

vcl_ostream& dbrec_composition::print(vcl_ostream& out) const
{
  out << "\t composition:\n";
  dbrec_part::print(out);
  out << "\t-----parts:\n";
  for (unsigned i = 0; i < children_.size(); i++) {
    children_[i]->print(out);
  }
  out << "\t-----------\n";
  return out;
}

//: recursively compute the depth of the tree, compositions will increment the depth, part is the leaf node with depth 1
unsigned dbrec_composition::depth() const
{
  unsigned deepest = 0;
  for (unsigned i = 0; i < children_.size(); i++) {
    unsigned depth_i = children_[i]->depth();
    if (deepest < depth_i)
      deepest = depth_i;
  }
  return deepest+1;
}
//: width of the composition is the total width of children
unsigned dbrec_composition::width() const
{
  unsigned w = 0;
  for (unsigned i = 0; i < children_.size(); i++) {
    w += children_[i]->width();
  }
  return w;
}

//: return the part pointer with the given type id
dbrec_part_sptr dbrec_composition::get_part(unsigned type) const
{
  dbrec_part_sptr p;
  for (unsigned i = 0; i < children_.size(); i++) {
    if (children_[i]->type() == type)
      return children_[i];
    p = children_[i]->get_part(type);
    if (!p)
      continue;
    else
      break;
  }
  return p;
}

//: a recursive helper for populate_table() method of the hierarchy
//  inefficient cause depth() is also recursive and traverses the tree more than once when it calls depth() method on parts
void dbrec_composition::populate_table(vcl_vector<vcl_pair<dbrec_part_sptr, int> >& part_table) const
{
  for (unsigned i = 0; i < children_.size(); i++) {
    int type = children_[i]->type();
    if ((int)part_table.size() < type) {
      part_table.resize(type + 100, vcl_pair<dbrec_part_sptr, int>(dbrec_part_sptr(0), -1)); // increase capacity for another hundred items after this one
    }
    if (!part_table[type].first) {
      part_table[type].first = children_[i];
      part_table[type].second = children_[i]->depth();
    }
    children_[i]->populate_table(part_table);
  }
}

//: being the same or not for a compositional part is to have the "same" children in the "same" order and have the "same" radius
bool dbrec_composition::same_part(dbrec_part_sptr other)
{
  dbrec_composition* other_comp = dynamic_cast<dbrec_composition*>(other.ptr());
  if (!other_comp)
    return false;
  if (radius_ != other_comp->radius())
    return false;
  vcl_vector<dbrec_part_sptr>& other_children = other_comp->children();
  if (other_children.size() != children_.size())
    return false;
  for (unsigned i = 0; i < children_.size(); i++) {
    if (!children_[i]->same_part(other_children[i]))
      return false;
  }
  return true;
}

float dbrec_composition::mass() const
{
  float sum = 0.0f;
  for (unsigned i = 0; i < children_.size(); i++) {
    sum += children_[i]->mass();
  }
  return sum;
}

dbrec_part_sptr dbrec_hierarchy::first()
{ 
  if (!roots_.size()) 
    return 0; 
  it_ = roots_.begin();
  return (*it_); 
}
dbrec_part_sptr dbrec_hierarchy::next()
{ 
  it_++; 
  if (it_ == roots_.end()) 
    return 0; 
  else return (*it_); 
}

vcl_ostream& dbrec_hierarchy::print(vcl_ostream& out) const
{
  out << "\t ---hierarchy:\n";
  for (vcl_vector<dbrec_part_sptr>::const_iterator it = roots_.begin(); it != roots_.end(); it++) {
    (*it)->print(out);
  }
  out << "\t end hierarchy ---\n";
  return out;
}
//: depth of the hierarchy is the depth of the deepest root node
unsigned dbrec_hierarchy::depth() const
{
  unsigned depth = 0;
  for (vcl_vector<dbrec_part_sptr>::const_iterator it = roots_.begin(); it != roots_.end(); it++) {
    unsigned d = (*it)->depth();
    if (depth < d)
      depth = d;
  }
  return depth;
}
//: width of the hierarchy is the total number of children of all root nodes
unsigned dbrec_hierarchy::width() const
{
  unsigned width = 0;
  for (vcl_vector<dbrec_part_sptr>::const_iterator it = roots_.begin(); it != roots_.end(); it++) {
    width += (*it)->width();
  }
  return width;
}
//: return the part pointer with the given type id
dbrec_part_sptr dbrec_hierarchy::get_part(unsigned type) const
{
  dbrec_part_sptr p = (type < part_table_.size()) ? part_table_[type].first : 0;
  //vcl_map<unsigned, dbrec_part_sptr>::iterator p_it = part_map_.find(type);
  //if (p_it != part_map_.end()) {
  //  p = p_it->second;
  //} else {
  if (!p) {
    for (vcl_vector<dbrec_part_sptr>::const_iterator it = roots_.begin(); it != roots_.end(); it++) {
      if ((*it)->type() == type)
        return (*it);
      p = (*it)->get_part(type);
      if (!p)
        continue;
      else {
        //part_map_[type] = p;
        if (type < part_table_.size()) {
          part_table_[type].first = p;
          part_table_[type].second = p->depth();
        } else {
          part_table_.resize(type + 100, vcl_pair<dbrec_part_sptr, int>(dbrec_part_sptr(0), -1)); // increase capacity for another hundred items after this one
          part_table_[type].first = p;
          part_table_[type].second = p->depth();
        }
        break;
      }
    }
  }  
  return p;
}

//: get_parts method uses the part table for quick access to the depths of the parts, it calls the following method if parts are not cached yet
//  inefficient cause depth() is also recursive and traverses the tree more than once when it calls depth() method on parts
void dbrec_hierarchy::populate_table() const
{
  for (unsigned i = 0; i < roots_.size(); i++) {
    int type = roots_[i]->type();
    if ((int)part_table_.size() < type) {
      part_table_.resize(type + 100, vcl_pair<dbrec_part_sptr, int>(dbrec_part_sptr(0), -1)); // increase capacity for another hundred items after this one
    }
    if (!part_table_[type].first) {
      part_table_[type].first = roots_[i];
      part_table_[type].second = roots_[i]->depth();
    }
    roots_[i]->populate_table(part_table_);
  }
  table_populated_ = true;
}

//: return a list of parts at the given depth
void dbrec_hierarchy::get_parts(int depth, vcl_vector<dbrec_part_sptr>& parts) const
{
  if (!table_populated_) {
    this->populate_table();
  }

  for (unsigned i = 0; i < part_table_.size(); i++) {
    if (part_table_[i].first && part_table_[i].second == depth)
      parts.push_back(part_table_[i].first);
  }
}
//: return a list of all parts in the hierarchy (usually used for visualizations)
void dbrec_hierarchy::get_all_parts(vcl_vector<dbrec_part_sptr>& parts) const
{
  if (!table_populated_) {
    this->populate_table();
  }
  for (unsigned i = 0; i < part_table_.size(); i++) {
    if (part_table_[i].first)
      parts.push_back(part_table_[i].first);
  }
}

//: register all the current parts to the type id factory to support creation of new unique type ids
void dbrec_hierarchy::register_parts(dbrec_type_id_factory* ins) const
{
  if (!table_populated_) {
    this->populate_table();
  }
  for (unsigned i = 0; i < part_table_.size(); i++) {
    if (part_table_[i].first)
      ins->register_type(part_table_[i].first->type());
  }
}

void dbrec_part::visualize(bsvg_document& doc, float x, float y, float vis_rad, const vcl_string& color) const
{
  bsvg_ellipse* circle = new bsvg_ellipse(vis_rad, vis_rad); // it's gonna be a circle since rx = ry
  circle->set_location(x, y);
  circle->set_fill_color(color);
  circle->set_stroke_color("black");
  doc.add_element(circle);

  vcl_stringstream ss; ss << type_; 
  bsvg_text* t = new bsvg_text(ss.str());
  t->set_location(x, y+3*vis_rad);
  t->set_fill_color(color);
  doc.add_element(t);
}

void dbrec_composition::visualize(bsvg_document& doc, float x, float y, float vis_rad, const vcl_string& color) const
{
  dbrec_part::visualize(doc, x, y, vis_rad, color);
  float h = 3*vis_rad;

  int w = this->width();
  float wid = w*4*vis_rad;
  float current = x-wid/2.0f;
  for (unsigned i = 0; i < children_.size(); i++) {
    int w_i = children_[i]->width();
    float wid_i = w_i*4*vis_rad;
    current += wid_i/2;
    children_[i]->visualize(doc, current, y+h, vis_rad, "blue");
    //draw a line between the root and the child
    bsvg_line* line = new bsvg_line(x, y, current, y+h);
    line->set_stroke_color(color);
    doc.add_element(line);
    current += wid_i/2;
  }
}
void dbrec_composition::visualize_models(const vcl_string& suffix) const
{
  vcl_stringstream ids; ids << this->type(); 
  vcl_string part_suffix = suffix + "_comp_" + ids.str();
  dbrec_image_compositor* comp = dynamic_cast<dbrec_image_compositor*>(compositor_.ptr());
  comp->visualize(part_suffix, children_);
  for (unsigned i = 0; i < children_.size(); i++)
    children_[i]->visualize_models(suffix);
}

//: allow for various visitors (encapsulated algos working with the meta-structure) visit the part
void dbrec_part::accept(dbrec_visitor* v)
{
  v->visit_part(this);
}

//: allow for various visitors (encapsulated algos working with the meta-structure) visit the composition
void dbrec_composition::accept(dbrec_visitor* v) 
{ 
  v->visit_composition(this); 
}

//: visualize as an svn document
void dbrec_hierarchy::visualize(const vcl_string& out) const
{
  //: find the depth of the hierarchy to determine a good height
  unsigned d = this->depth();
  unsigned w = this->width();
  float height = _visualization_radius * 4.0f * d;
  float width = _visualization_radius * 4.0f * w;

  bsvg_document doc(width, height);
  doc.add_description("Hierarchy");
  
  float current = 0;
  for (vcl_vector<dbrec_part_sptr>::const_iterator it = roots_.begin(); it != roots_.end(); it++) {
    unsigned w = (*it)->width();
    float rw = w*4.0f*_visualization_radius;
    current += rw/2.0f;
    (*it)->visualize(doc, current, _visualization_radius, _visualization_radius, "yellow");
    current += rw/2.0f;
  }

  bxml_write(out, doc);
}

vcl_ostream & operator<<(vcl_ostream& out, const dbrec_part& p)
{
  return p.print(out);
}
vcl_ostream & operator<<(vcl_ostream& out, const dbrec_composition& p)
{
  return p.print(out);
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_hierarchy as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbrec_hierarchy const &ph)
{
  vcl_cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & is, dbrec_hierarchy &ph)
{
  vcl_cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, dbrec_hierarchy* ph)
{
  delete ph;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr)
  {
    ph = new dbrec_hierarchy();
    vsl_b_read(is, *ph);
  }
  else
    ph = 0;
}

void vsl_b_write(vsl_b_ostream& os, const dbrec_hierarchy* &ph)
{
  if (ph==0)
  {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else
  {
    vsl_b_write(os,true); // Indicate non-null pointer stored
    vsl_b_write(os,*ph);
  }
}




