//:
// \file
// \brief classes to represent part contexts
//
//  Context holds the extrinsic state of parts, e.g. their locations in the images
//  They implement efficient methods to access instances of parts through location-based queries or iteratively
//
//  context uses a vgl_rtree to efficiently index the instances
//
// \author Ozge C. Ozcanli (Brown)
// \date   April 23, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//


#if !defined(_dbrec_part_context_h_)
#define _dbrec_part_context_h_

#include "dbrec_part_context_sptr.h"
#include "dbrec_part_sptr.h"
#include <vbl/vbl_ref_count.h>
#include <vsl/vsl_binary_io.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vnl/vnl_vector_fixed.h>
#include <vgl/algo/vgl_rtree.h>
#include <vgl/algo/vgl_rtree_c.h>
#include <vcl_map.h>
#include <vil/vil_image_resource.h>

//: an instance of a part at a particular location in an image
class dbrec_part_ins : public vbl_ref_count {
public:
  dbrec_part_ins(unsigned type, float x, float y) : type_(type), pt_(x, y) {}
  dbrec_part_ins(unsigned type, const vgl_point_2d<float>& pt) : type_(type), pt_(pt) {}

  virtual ~dbrec_part_ins() {}

  vgl_point_2d<float>& pt() { return pt_; }
  unsigned type() { return type_; }

  void push_posterior(double val) { posteriors_.push_back(val); }
  double posterior(int i) { return posteriors_[i]; }
  unsigned posteriors_size() { return posteriors_.size(); }

  virtual float theta() { return 0.0f; }  
  virtual vnl_vector_fixed<float, 2> dir_vector() { return vnl_vector_fixed<float, 2>(); }

protected:
  unsigned type_;
  vgl_point_2d<float> pt_;
  vcl_vector<double> posteriors_;
};

//: a specialized instance that stores the rotation of the part in the image
//  to be used for rotationally invariant gaussian extrema primitives (the part in the hierarchy won't have an angle associated with it, but its instances will have)
class dbrec_rot_inv_part_ins : public dbrec_part_ins {
public: 
  dbrec_rot_inv_part_ins(unsigned type, float x, float y, float theta, vnl_vector_fixed<float, 2>& dir_vec) : dbrec_part_ins(type, x, y), theta_(theta), dir_vec_(dir_vec) {}
  dbrec_rot_inv_part_ins(unsigned type, const vgl_point_2d<float>& pt, float theta, vnl_vector_fixed<float, 2>& dir_vec) : dbrec_part_ins(type, pt), theta_(theta), dir_vec_(dir_vec) {}
  virtual float theta() { return theta_; }
  virtual vnl_vector_fixed<float, 2> dir_vector() { return dir_vec_; }
protected:
  float theta_;
  vnl_vector_fixed<float, 2> dir_vec_; // when the instance is create its direction vector need to be computed and saved
};

//: an instance of a composition at a particular location in an image
class dbrec_composition_ins : public dbrec_part_ins {
public:
  dbrec_composition_ins(unsigned type, float x, float y, const vcl_vector<dbrec_part_ins_sptr>& children) : dbrec_part_ins(type, x, y), children_(children) {}
  dbrec_composition_ins(unsigned type, const vgl_point_2d<float>& pt, const vcl_vector<dbrec_part_ins_sptr>& children) : dbrec_part_ins(type, pt), children_(children) {}

  virtual ~dbrec_composition_ins() { children_.clear(); }

  vcl_vector<dbrec_part_ins_sptr>& children() { return children_; }
protected:
  vcl_vector<dbrec_part_ins_sptr> children_;
};

class dbrec_rot_inv_composition_ins : public dbrec_rot_inv_part_ins {
public:
  dbrec_rot_inv_composition_ins(unsigned type, float x, float y, const vcl_vector<dbrec_part_ins_sptr>& children, vnl_vector_fixed<float, 2>& dir_vec) : dbrec_rot_inv_part_ins(type, x, y, 0.0f, dir_vec), children_(children) {}
  dbrec_rot_inv_composition_ins(unsigned type, const vgl_point_2d<float>& pt, const vcl_vector<dbrec_part_ins_sptr>& children, vnl_vector_fixed<float, 2>& dir_vec) : dbrec_rot_inv_part_ins(type, pt, 0.0f, dir_vec), children_(children) {}
  virtual ~dbrec_rot_inv_composition_ins() { children_.clear(); }
  vcl_vector<dbrec_part_ins_sptr>& children() { return children_; }
protected:
  vcl_vector<dbrec_part_ins_sptr> children_;
};

//: helper class for vgl_rtree, this is class C
class rtree_dbrec_ins_box_2d
{
  // only static methods
  rtree_dbrec_ins_box_2d();
  ~rtree_dbrec_ins_box_2d();

 public:
  typedef dbrec_part_ins_sptr v_type;
  typedef vgl_box_2d<float> b_type;

  // Operations------
  static void init  (vgl_box_2d<float>& b, dbrec_part_ins_sptr const& pi)
  { b = vgl_box_2d<float>();  b.add(pi->pt()); }

  static void update(vgl_box_2d<float>& b, dbrec_part_ins_sptr const& pi)
  { b.add(pi->pt()); }

  static void update(vgl_box_2d<float>& b0, vgl_box_2d<float> const &b1)
  { b0.add(b1.min_point());  b0.add(b1.max_point()); }

  static bool meet(vgl_box_2d<float> const& b, dbrec_part_ins_sptr const& pi)
  { return b.contains(pi->pt()); }

  static bool  meet(vgl_box_2d<float> const& b0, vgl_box_2d<float> const& b1) {
    vgl_point_2d<float> b0min = b0.min_point();
    vgl_point_2d<float> b1min = b1.min_point();
    vgl_point_2d<float> b0max = b0.max_point();
    vgl_point_2d<float> b1max = b1.max_point();
    vgl_point_2d<float> max_of_mins(b0min.x() > b1min.x() ? b0min.x() : b1min.x(), b0min.y() > b1min.y() ? b0min.y() : b1min.y());
    vgl_point_2d<float> min_of_maxs(b0min.x() < b1min.x() ? b0min.x() : b1min.x(), b0min.y() < b1min.y() ? b0min.y() : b1min.y());

    return b0.contains(b1min) || b0.contains(b1max) ||
           b1.contains(b0min) || b1.contains(b0max) ||
           ( (b0.contains(max_of_mins) || b0.contains(min_of_maxs)) &&
             (b1.contains(max_of_mins) || b1.contains(min_of_maxs)) );
  }
  static float volume(vgl_box_2d<float> const& b)
  { return b.area(); }

  // point meets for a polygon, used by generic rtree probe
  static bool meets(dbrec_part_ins_sptr const& pi, vgl_polygon<float> poly)
  { return poly.contains(pi->pt()); }

  // box meets for a polygon, used by generic rtree probe
  static bool meets(vgl_box_2d<float> const& b, vgl_polygon<float> poly)
  { return vgl_intersection<float>(b, poly); }
};

// rtree definitions
typedef dbrec_part_ins_sptr V_type;  // the contained object type
typedef vgl_box_2d<float> B_type;    // the bounding object type
typedef rtree_dbrec_ins_box_2d C_type; // the helper class
typedef vgl_rtree<V_type, B_type, C_type> dbrec_rtree_type;

//: The abstract base class for the context that defines the interface
class dbrec_part_context : public vbl_ref_count {

public:
  virtual void add_part(dbrec_part_ins_sptr p) = 0;
  virtual dbrec_part_ins_sptr first() = 0;  // iterator, if no parts yet return 0 
  virtual dbrec_part_ins_sptr next() = 0;   // iterator, if reached the end return 0
  virtual void query(const vgl_box_2d<float>& box, vcl_vector<dbrec_part_ins_sptr>& out) = 0;
  virtual unsigned size() const = 0;

  void add_map(vil_image_resource_sptr m) { maps_.push_back(m); }
  vil_image_resource_sptr get_map(unsigned i) const { if (i >= maps_.size()) return 0; else return maps_[i]; }
  unsigned maps_size() const { return maps_.size(); }

  vil_image_resource_sptr get_posterior_map(unsigned i, unsigned ni, unsigned nj);
  
protected:
  //: get some space for some maps which various operators or compositions may wanna save
  vcl_vector<vil_image_resource_sptr> maps_;
};

//: In this concrete context subclass, the context is basically an rtree for fast access during location-based queries (e.g. give me all the parts around this location) 
//  as well as iterators
class dbrec_rtree_context : public dbrec_part_context {

public:
  virtual void add_part(dbrec_part_ins_sptr p);
  virtual dbrec_part_ins_sptr first();  // iterator, if no parts yet return 0 
  virtual dbrec_part_ins_sptr next();   // iterator, if reached the end return 0
  virtual void query(const vgl_box_2d<float>& box, vcl_vector<dbrec_part_ins_sptr>& out);
  virtual unsigned size() const;

protected:
  dbrec_rtree_type rtree_;
  dbrec_rtree_type::iterator it_;
  
};


//: parsing each image will require a context factory to hold the contexts for each part in the meta-structure
class dbrec_context_factory : public vbl_ref_count {
public:
  //: create and return a new context, not added to the map
  virtual dbrec_part_context_sptr new_context() = 0;
  //: check the map, if a context for this type has been added return it, otherwise return a null pointer
  virtual dbrec_part_context_sptr get_context(unsigned type) = 0;
  //: add the context to the map, if already exists, the existing one is replaced
  void add_context(unsigned type, dbrec_part_context_sptr c) { contexts_[type] = c; }

  unsigned size() { return contexts_.size(); }

protected:
  vcl_map<unsigned, dbrec_part_context_sptr> contexts_;
};

class dbrec_rtree_context_factory : public dbrec_context_factory {
public:
  virtual dbrec_part_context_sptr new_context();
  virtual dbrec_part_context_sptr get_context(unsigned type);

};

// Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_hierarchy as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbrec_context_factory const &ph);
void vsl_b_read(vsl_b_istream & is, dbrec_context_factory &ph);
void vsl_b_read(vsl_b_istream& is, dbrec_context_factory* ph);
void vsl_b_write(vsl_b_ostream& os, const dbrec_context_factory* &ph);


#endif  //_dbrec_part_context_h_
