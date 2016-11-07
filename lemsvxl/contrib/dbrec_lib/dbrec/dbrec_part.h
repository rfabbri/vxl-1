//:
// \file
// \brief classes to represent part hierarchies as the meta-structure of recognition grammar
//
//
// \author Ozge C. Ozcanli (Brown)
// \date   April 22, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//


#if !defined(_DBREC_PART_H)
#define _DBREC_PART_H

#include <dbrec/dbrec_part_sptr.h>
#include <vcl_vector.h>
#include <bxml/bsvg/bsvg_document.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_iostream.h>
#include <vsl/vsl_binary_io.h>
#include <vnl/vnl_vector_fixed.h>
#include <dbrec/dbrec_compositor_sptr.h>
#include <dbrec/dbrec_part_context_sptr.h>
#include <dbrec/dbrec_type_id_factory.h>

const float _visualization_radius = 10.0f;


class dbrec_visitor;
class dbrec_part : public vbl_ref_count {

public:
  dbrec_part(unsigned type) : type_(type) {}
  dbrec_part(const dbrec_part& other) { type_ = other.type_; }
  virtual ~dbrec_part() {}
  
  virtual bool equal(const dbrec_part& other) const { return type_ == other.type_; }

  virtual vcl_ostream& print(vcl_ostream& out) const { out << "\tpart type: " << type_ << "\n"; return out; }
  virtual void visualize(bsvg_document& doc, float x, float y, float vis_rad, const vcl_string& color) const;
  virtual void visualize_models(const vcl_string&suffix) const {}
  virtual vcl_string string_identifier() const { vcl_stringstream ss; ss << type_; return ss.str(); }
  
  unsigned type() const { return type_; }
  void reset_type(unsigned type) { type_ = type; }

  //: recursively compute the depth of the tree, compositions will increment the depth, part is the leaf node with depth 1
  virtual unsigned depth() const { return 1; }
  //: width of a leaf part is 1
  virtual unsigned width() const { return 1; }

  //: return the part pointer with the given type id, parents return pointers to their children, so if we reach base of recursion just return 0
  virtual dbrec_part_sptr get_part(unsigned type) const { return 0; }

  //: a recursive helper for populate_table() method of the hierarchy
  //  inefficient cause depth() is also recursive and traverses the tree more than once when it calls depth() method on parts
  virtual void populate_table(vcl_vector<vcl_pair<dbrec_part_sptr, int> >& part_table) const { return; }  // do nothing at the base

  //: allow for various visitors (encapsulated algos working with the meta-structure) visit the part
  virtual void accept(dbrec_visitor* visitor);

  virtual bool same_part(dbrec_part_sptr other) { return type_ == other->type(); }

  virtual float mass() const { return 0; }

protected:
  unsigned type_;

};

class dbrec_composition : public dbrec_part {

public:
  dbrec_composition(unsigned type, const vcl_vector<dbrec_part_sptr>& children, dbrec_compositor_sptr compositor, float radius) 
    : dbrec_part(type), children_(children), compositor_(compositor), class_prior_(-1.0f), radius_(radius) {}
  dbrec_composition(const dbrec_composition& other) 
    : dbrec_part(other.type_), children_(other.children_), compositor_(other.compositor_), class_prior_(other.class_prior_), radius_(other.radius_) {}
  virtual ~dbrec_composition() { children_.clear(); }

  vcl_vector<dbrec_part_sptr>& children() { return children_; }
  dbrec_compositor_sptr compositor() { return compositor_; }

  virtual bool equal(const dbrec_composition& other) const;
  virtual vcl_ostream& print(vcl_ostream& out) const;
  
  virtual void visualize(bsvg_document& doc, float x, float y, float vis_rad, const vcl_string& color) const;
  virtual void visualize_models(const vcl_string& suffix) const;
  
  unsigned size() const { return children_.size(); }

  //: recursively compute the depth of the tree, compositions will increment the depth, part is the leaf node with depth 1
  virtual unsigned depth() const;
  //: width of the composition is the total width of children
  virtual unsigned width() const;

  //: return the part pointer with the given type id
  virtual dbrec_part_sptr get_part(unsigned type) const;

  //: a recursive helper for populate_table() method of the hierarchy
  //  inefficient cause depth() is also recursive and traverses the tree more than once when it calls depth() method on parts
  virtual void populate_table(vcl_vector<vcl_pair<dbrec_part_sptr, int> >& part_table) const;

  void set_compositor(dbrec_compositor_sptr c) { compositor_ = c; }

  //: allow for various visitors (encapsulated algos working with the meta-structure) visit the composition
  virtual void accept(dbrec_visitor* v); 

  float class_prior() { return class_prior_; }
  void set_class_prior(float p) { class_prior_ = p; }

  float radius() { return radius_; }
  void set_radius(float r) { radius_ = r; }

  //: being the same or not for a compositional part is to have the "same" children in the "same" order
  virtual bool same_part(dbrec_part_sptr other);

  virtual float mass() const;

protected:
  vcl_vector<dbrec_part_sptr> children_;
  dbrec_compositor_sptr compositor_;
  float class_prior_;
  float radius_;
};

//: hierarchy contains a root node per class, its just a container of roots
class dbrec_hierarchy : public vbl_ref_count {
public:
  dbrec_hierarchy() : part_table_(100, vcl_pair<dbrec_part_sptr, int>(dbrec_part_sptr(0), -1) ), table_populated_(false) {}

  void add_root(dbrec_part_sptr r) { roots_.push_back(r); }
  unsigned class_cnt() { return roots_.size(); }
  
  dbrec_part_sptr first(); 
  dbrec_part_sptr next();
  //: caution, no check on the size of roots_ array 
  dbrec_part_sptr root(unsigned i) { return roots_[i]; }

  vcl_ostream& print(vcl_ostream& out) const;
  //: visualize as an svn document
  void visualize(const vcl_string& out) const;

  //: depth of the hierarchy is the depth of the deepest root node
  unsigned depth() const;
  //: width of the hierarchy is the total number of children of all root nodes
  unsigned width() const;

  //: return the part pointer with the given type id
  dbrec_part_sptr get_part(unsigned type) const;

  //: return a list of parts at the given depth
  void get_parts(int depth, vcl_vector<dbrec_part_sptr>& parts) const;

  //: return a list of all parts in the hierarchy (usually used for visualizations)
  void get_all_parts(vcl_vector<dbrec_part_sptr>& parts) const;

  //: get_parts method uses the part table for quick access to the depths of the parts, it calls the following method if parts are not cached yet
  void populate_table() const;

  //: register all the current parts to the type id factory to support creation of new unique type ids
  void register_parts(dbrec_type_id_factory* ins) const;

protected:
  vcl_vector<dbrec_part_sptr> roots_;

  vcl_vector<dbrec_part_sptr>::iterator it_;

  //: we need fast access to the parts so cache them in a direct-address hash table using their type as the key (flyweight pattern: we keep a single instance of the part, accessed via its type throughout the program, but only hierarchy acts as a factory class)
  //  the max number of parts is around 100, so reserve space for 100, but use an array so it could grow
  //mutable vcl_map<unsigned, dbrec_part_sptr> part_map_;
  // we also cache the depths of the parts for quick access later
  mutable vcl_vector<vcl_pair<dbrec_part_sptr, int> > part_table_;
  mutable bool table_populated_;

};

vcl_ostream & operator<<(vcl_ostream& out, const dbrec_part& p);
vcl_ostream & operator<<(vcl_ostream& out, const dbrec_composition& p);

// Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_hierarchy as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbrec_hierarchy const &ph);
void vsl_b_read(vsl_b_istream & is, dbrec_hierarchy &ph);
void vsl_b_read(vsl_b_istream& is, dbrec_hierarchy* ph);
void vsl_b_write(vsl_b_ostream& os, const dbrec_hierarchy* &ph);

#endif  //_DBREC_PART_H
