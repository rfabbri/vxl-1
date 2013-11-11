//:
// \file
// \brief Using Visitor Design Pattern to allow for different algorithms to work with the meta-structure
//        without cluttering the interface of the meta-structure parts
//        e.g. a parser may define a concrete visitor class to parse using the meta-structure
//             a visualizer may define a concrete visitor class to visualize the meta-structure
//
//
// \author Ozge C. Ozcanli (Brown)
// \date   May 01, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//


#if !defined(_dbrec_visitor_h_)
#define _dbrec_visitor_h_

#include "dbrec_part.h"
#include "dbrec_image_compositor.h"
#include "dbrec_gaussian_primitive.h"
#include <vbl/vbl_ref_count.h>

//  if a new primitive part type is added, a method needs to be defined for it in this base visitor class, 
//  however only the relevant derived concrete visitor classes need to actually implement it;
class dbrec_visitor : public vbl_ref_count {
public:

  virtual void visit_part(dbrec_part* c) {}
  virtual void visit_composition(dbrec_composition* c) {}
  virtual void visit_gaussian_primitive(dbrec_gaussian* g) {}

protected:
  
};

//: if a new type of compositor is added, a method needs to be defined for it in this base visitor class
class dbrec_compositor_visitor : public vbl_ref_count {
public:

  virtual void visit_compositor(dbrec_compositor* c, const vcl_vector<dbrec_part_sptr>& children) {}
  virtual void visit_or_compositor(dbrec_or_compositor* c, const vcl_vector<dbrec_part_sptr>& children) {}
  virtual void visit_central_compositor(dbrec_central_compositor* g, const vcl_vector<dbrec_part_sptr>& children) {}

protected:
  
};

// Binary io, NOT IMPLEMENTED, signatures defined to use dbrec_visitor as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbrec_visitor const &ph);
void vsl_b_read(vsl_b_istream & is, dbrec_visitor &ph);
void vsl_b_read(vsl_b_istream& is, dbrec_visitor* ph);
void vsl_b_write(vsl_b_ostream& os, const dbrec_visitor* &ph);

#endif  //_dbrec_visitor_h_
