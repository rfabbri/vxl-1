//:
// \file
// \author Ozge C Ozcanli (ozge@lems.brown.edu)
// \date 04/23/09
//
//
#include "dbrec_part_context.h"
#include "dbrec_part.h"

#include  <vil/vil_new.h>

vil_image_resource_sptr dbrec_part_context::get_posterior_map(unsigned i, unsigned ni, unsigned nj)
{
  vil_image_view<float> out_v(ni, nj);
  out_v.fill(0.0f);
  dbrec_part_ins_sptr pi = this->first();
  if (!pi)
    return vil_new_image_resource_of_view(out_v);
  if (pi->posteriors_size() <= i)
    return 0;
  while (pi) {
    unsigned ii = (unsigned)pi->pt().x();
    unsigned jj = (unsigned)pi->pt().y();
    if (ii < ni && jj < nj && ii >=0 && jj >= 0)
      out_v(ii,jj) = (float)pi->posterior(i);
    else {
      vcl_cout << "In dbrec_part_context::get_posterior_map() -- the location of the instance is out of passed image size, ni: " << ni << " nj: " << nj << vcl_endl;
    }
    pi = this->next();
  }
  return vil_new_image_resource_of_view(out_v);
}

void dbrec_rtree_context::add_part(dbrec_part_ins_sptr p)
{
  rtree_.add(p); 
}
unsigned dbrec_rtree_context::size() const
{
  return rtree_.size();
}

dbrec_part_ins_sptr dbrec_rtree_context::first() // iterator, if no parts yet return 0 
{
  if (!rtree_.size())
    return 0;
  it_ = rtree_.begin();
  return (*it_);
}

dbrec_part_ins_sptr dbrec_rtree_context::next() // iterator, if no parts yet return 0 
{
  it_++;
  if (it_ != rtree_.end()) {
    return (*it_);
  } else 
    return 0;
}
void dbrec_rtree_context::query(const vgl_box_2d<float>& box, vcl_vector<dbrec_part_ins_sptr>& out) 
{
  rtree_.get(box, out);
}

dbrec_part_context_sptr dbrec_rtree_context_factory::new_context()
{
  return new dbrec_rtree_context();
}
dbrec_part_context_sptr dbrec_rtree_context_factory::get_context(unsigned type)
{
  dbrec_part_context_sptr c;
  vcl_map<unsigned, dbrec_part_context_sptr>::iterator it = contexts_.find(type);
  if (it != contexts_.end()) {
    c = it->second;
  } 
  return c;
}

//: Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value
void vsl_b_write(vsl_b_ostream & os, dbrec_context_factory const &ph)
{
  vcl_cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream & is, dbrec_context_factory &ph)
{
  vcl_cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_read(vsl_b_istream& is, dbrec_context_factory* ph)
{
  vcl_cerr << "vsl_b_read() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

void vsl_b_write(vsl_b_ostream& os, const dbrec_context_factory* &ph)
{
  vcl_cerr << "vsl_b_write() -- Binary io, NOT IMPLEMENTED, signatures defined to use brec_part_hierarchy as a brdb_value\n";
  return;
}

