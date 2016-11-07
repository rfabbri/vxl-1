// This is//Projects/lemsvxl/src/contrib/dbrec_lib/dbrec3d/dbrec3d_part.h
#ifndef dbrec3d_part_h
#define dbrec3d_part_h

//:
// \file
// \brief Methods that are common to all classes under parts (brec3d_part concept)
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date  2/24/10
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim



class dbrec3d_part
{
  
public:
  dbrec3d_part(int id) : id_(id) {}
  
  dbrec3d_part(const dbrec3d_part& other) { id_ = other.id_; dbrec3d_concrete_part(other) }
  
  ~dbrec3d_part(){~dbrec3d_concrete_part()}
  
  virtual bool equal(const dbrec3d_part& other) const { return type_ == other.type_; }
  
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
  virtual dbrec3d_part_sptr get_part(unsigned type) const { return 0; }
  
  //: a recursive helper for populate_table() method of the hierarchy
  //  inefficient cause depth() is also recursive and traverses the tree more than once when it calls depth() method on parts
  virtual void populate_table(vcl_vector<vcl_pair<dbrec3d_part_sptr, int> >& part_table) const { return; }  // do nothing at the base
  
  //: allow for various visitors (encapsulated algos working with the meta-structure) visit the part
  template < typename Visitor >
  typename visit_return_trait< Visitor, dbrec3d_concrete_part >::type accept (Visitor& v) {
    return v.visit (static_cast< dbrec3d_concrete_part& > (*this));
  }
  
  
  virtual bool same_part(dbrec3d_part_sptr other) { return type_ == other->type(); }
  
  virtual float mass() const { return 0; }
  
protected:
  int id_;
  
};



#endif
