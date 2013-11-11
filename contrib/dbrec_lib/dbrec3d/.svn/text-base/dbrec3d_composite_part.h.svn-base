// This is//Projects/lemsvxl/src/contrib/dbrec_lib/dbrec3d/brec3d_composite_part.h
#ifndef brec3d_composite_part_h
#define brec3d_composite_part_h

//:
// \file
// \brief A class that implements the dbrec3d_part concept for composite parts (i.e non-leafs). 
// \author Isabel Restrepo mir@lems.brown.edu
// \date  2/24/10
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include "dbrec3d_part.h"
#include "dbrec3d_visitor.h"
#include <bxml/bxml_document.h>
#include <bxml/bxml_find.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>

//: Forward declaration of XML read
dbrec3d_part_sptr dbrec3d_composite_part_parse_xml_element(bxml_data_sptr d);



//: A class that implements the dbrec3d_part concept for composite parts (i.e non-leafs)
//  This class is templated on a compositor class, who knows how to put the parts together and detec instances of it
template <class T_compositor>
class dbrec3d_composite_part : public dbrec3d_part
{
  
public:
  
  typedef T_compositor comp_type;
  typedef typename T_compositor::model_type model_type;
  
  friend class dbrec3d_parts_manager;
  
private:
  
  //: Default contructor
  dbrec3d_composite_part() : dbrec3d_part(-1, vnl_float_3(0.0f, 0.0f, 0.0f), 0.0f, vnl_float_3(0.0f, 0.0f, 0.0f),vnl_float_3(0.0f, 0.0f, 0.0f), false, false),
  children_(NULL), class_prior_(-1.0f) {}
  
  
  //: Constructor from: compositor part id, geometry, children, neighborhood radious
  dbrec3d_composite_part(const T_compositor &compositor, int type_id, vnl_float_3 axis, vnl_float_3 aux_axis, float angle, vnl_float_3 scale, vnl_float_3 radius, bool symmetric, bool rot_invar, const vcl_vector<int>& children ) 
  : dbrec3d_part(type_id, axis, aux_axis, angle, scale, radius, symmetric, rot_invar),compositor_(compositor), children_(children), class_prior_(-1.0f) {}
  
  //: Copy constructor
  dbrec3d_composite_part(const dbrec3d_composite_part& other) 
  : dbrec3d_part(other.type_id_, other.axis_,other.aux_axis_, other.angle_, other.scale_, other.radius_, other.symmetric_, other.rot_invar_),
  children_(other.children_),class_prior_(other.class_prior_),compositor_(compositor) {}
  
    
public:
  
  //: Destructor
  ~dbrec3d_composite_part() { children_.clear(); }
  
  T_compositor& compositor() { return compositor_; }
  
  model_type& model() { return compositor_.model(); } 
    
  virtual dbrec3d_part_types part_type() { return COMPOSITE; }

  //void set_type_id(float type_id) { reset_type_id(type_id); }
 
  //: Return vector of children
  vcl_vector<int>& children_id() { return children_; }
  
    //: Return number of children
  unsigned num_children() const { return children_.size(); }
  
  //: Allow for various visitors (encapsulated algos working with the meta-structure) visit the composition
  //virtual void accept(dbrec_visitor* v); 
  
  //: Return the class prior probability
  float class_prior() { return class_prior_; }
  
  //: Set the class prior probability
  void set_class_prior(float p) { class_prior_ = p; }
  
  //: Get the raius of receptive field
  float radius() { return radius_; }
  
  //: Set radious of receptive field
  void set_radius(float r) { radius_ = r; }
  
  //: XML write
  virtual bxml_data_sptr xml_element() const;

  virtual void accept(dbrec3d_visitor* v) { v->visit(this); }

  //: XML read
  static dbrec3d_part_sptr parse_xml_element(bxml_data_sptr d);
  
  //: Return the maximum values fo a porterior probability density - used for visualization normalization
  virtual inline double max_posterior() { return compositor_.model().max_posterior();}
    
protected:
  
  T_compositor compositor_;
  
  //:ID's of children. They can be either primitive parts or composite parts
  vcl_vector<int> children_;
  
  //: Prior distribution of this composition
  float class_prior_;
  
  //: Radious of receptive field
  //float radius_;
  
  //: One of the prominant relation between line segments
  //dbrec3d_composition_relation relation_;
};


//
////: A composite part that represents a bin or directions
//template <class T_compositor>
//class dbrec3d_discrete_composite_part : public dbrec3d_composite_part<T_compositor>
//{
//  //: Defaul contructor
//  dbrec3d_discrete_composite_part() : dbrec3d_composite_part<T_compositor>(), max_azimuthal_(0.0f), min_azimuthal_(0.0f), max_polar_(0.0f),min_polar_(0.0f) {}
//  
//  
//  //: Constructor from: part id, geometry, children, neighborhood radious and bin ranges
//  dbrec3d_discrete_composite_part(const T_compositor &compositor, int type_id, vnl_float_3 axis, float angle, vnl_float_3 scale, bool symmetric, bool rot_invar, 
//                                  const vcl_vector<dbrec3d_part_sptr>& children, float radius,
//                                  float max_azimuthal, float min_azimuthal, float max_polar, float min_polar) 
//  : dbrec3d_composite_part<T_compositor>(compositor, type_id, axis, angle, scale, symmetric, rot_invar,children,-1.0f, radius),
//  max_azimuthal_(max_azimuthal), min_azimuthal_(min_azimuthal), max_polar_(max_polar), min_polar_(min_polar) {}
//  
//  //: Copy constructor
//  dbrec3d_discrete_composite_part(const dbrec3d_discrete_composite_part& other) 
//  : dbrec3d_composite_part<T_compositor>(other.compositor_, other.type_id_, other.axis_, other.angle_, other.scale_, other.symmetric_, other.rot_invar_, other.children_,other.class_prior_, other.radius_),
//  max_azimuthal_(other.max_azimuthal_), min_azimuthal_(other.min_azimuthal_), max_polar_(other.max_polar_), min_polar_(other.min_polar_){}
//  
//  //: XML read
//  static dbrec3d_part_sptr parse_xml_element(bxml_data_sptr d) {return NULL;}
//  
//  //:XML write
//  //virtual bxml_data_sptr xml_element() const;
//  
//protected:
//  // Bin ranges
//  float max_azimuthal_;
//  float min_azimuthal_;
//  float max_polar_;
//  float min_polar_;
//};

template <class T_compositor>
bxml_data_sptr dbrec3d_composite_part<T_compositor>::xml_element() const
{
  bxml_element* data = new bxml_element("dbrec3d_composite_part");
  data->append_text("\n");
  data->set_attribute("type_id", this->type_id_);
  data->set_attribute("class_prior", this->class_prior_);
  data->set_attribute("nchildren", children_.size());
  vcl_stringstream ss;
  for (unsigned i = 0; i < children_.size(); i++) 
    ss << children_[i]<< " ";
  data->append_text(ss.str());
  data->append_text("\n");
  bxml_data_sptr compositor_data = this->compositor_.xml_element();
  data->append_data(compositor_data);
  data->append_text("\n");
  return data;
}

//: XML read
template <class T_compositor>
dbrec3d_part_sptr dbrec3d_composite_part<T_compositor>::parse_xml_element(bxml_data_sptr d)
{
  bxml_element query("dbrec3d_composite_part");
  bxml_data_sptr root = bxml_find_by_name(d, query);
  if (!root || root->type() != bxml_data::ELEMENT) {
    return 0;
  }
  bxml_element* root_elm = dynamic_cast<bxml_element*>(root.ptr());
  
  //get the variables
  int type_id = -1; 
  float class_prior = -1.0;
  unsigned nchildren = 0;
  root_elm->get_attribute("type_id", type_id);
  root_elm->get_attribute("class_prior", class_prior);
  root_elm->get_attribute("nchildren",nchildren);
  
  //: read out the children
  vcl_vector<int> children;
  for (bxml_element::const_data_iterator s_it = root_elm->data_begin(); s_it != root_elm->data_end(); s_it++) {
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
        vcl_stringstream ss2(tokens[i]); int c_type_id;
        ss2 >> c_type_id;
        children.push_back(c_type_id);
      }
      break;
    }
  }
  
  
  T_compositor comp;
  comp=*(comp.parse_xml_element(root_elm));
  vnl_float_3 axis, aux_axis, scale, radius;
  bool symmetric=true, rot_invar=true;
  float angle=0,max_azimuthal=0,min_azimuthal=0,max_polar=0,min_polar=0;

  //create part
  return new dbrec3d_composite_part(comp, type_id, axis, aux_axis, angle, scale, radius, symmetric, rot_invar, children ); 

  
#if 0
  int part_id = PARTS_MANAGER->new_composite_part<T_compositor>(comp,axis,aux_axis,angle,scale,
                                               symmetric,rot_invar,children,radius,
                                               max_azimuthal,min_azimuthal,max_polar,min_polar); 
  dbrec3d_part_sptr part = PARTS_MANAGER->get_part(part_id);
  part->reset_type_id(type_id);
  dbrec3d_composite_part<T_compositor>* c = static_cast<dbrec3d_composite_part<T_compositor>*>(part.as_pointer());
  c->set_class_prior(class_prior);
#endif
  //return part;
}

//vcl_ostream & operator<<(vcl_ostream& out, const dbrec3d_composition& p);



#endif
