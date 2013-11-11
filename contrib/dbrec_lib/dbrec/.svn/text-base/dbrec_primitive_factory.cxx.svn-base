//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 04/23/09
//
//

#include "dbrec_type_id_factory.h"
#include "dbrec_primitive_factory.h"
#include "dbrec_part.h"
#include "dbrec_image_compositor.h"
#include "dbrec_compositor_sptr.h"

dbrec_part_sptr dbrec_primitive_factory::get_primitive(unsigned type) {
  vcl_map<unsigned, dbrec_part_sptr>::iterator it = flyweights_.find(type);
  if (it != flyweights_.end())
    return (it->second);
  else 
    return 0;
}
void dbrec_primitive_factory::add_primitive(dbrec_part_sptr p)
{ 
  flyweights_[p->type()] = p; 
}

//: create a hiearchy where each class composition node has all the primitives as children (random classifier)
dbrec_hierarchy_sptr dbrec_primitive_factory::construct_random_classifier(int nclasses)
{
  dbrec_hierarchy_sptr h = new dbrec_hierarchy();
  for (int i = 0; i < nclasses; i++) {
    dbrec_compositor_sptr compositor = new dbrec_or_compositor();
    vcl_vector<dbrec_part_sptr> composing_parts;
    for (part_const_iterator it = parts_const_begin(); it != parts_const_end(); it++) {
      composing_parts.push_back(it->second);
    }
    dbrec_composition* cp = new dbrec_composition(dbrec_type_id_factory::instance()->new_type(), composing_parts, compositor, 1.0f); // radius is irrelevant, cause this is an or composition
    h->add_root(cp);
  }
  return h;
}

vcl_ostream& dbrec_primitive_factory::print(vcl_ostream& out) const
{
  out << "------factory:\n";
  for (dbrec_primitive_factory::part_const_iterator it = this->parts_const_begin(); it != this->parts_const_end(); it++)
    (it->second)->print(out);
  out << "-------------\n";
  return out;
}

vcl_ostream & operator<<(vcl_ostream& out, const dbrec_primitive_factory& p)
{
  return p.print(out);
}

