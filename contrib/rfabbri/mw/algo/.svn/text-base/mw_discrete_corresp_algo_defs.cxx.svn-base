#include "mw_discrete_corresp_algo.h"
#include "mw_discrete_corresp_algo.txx"

#include <mw/mw_util.h>
#include <mw/algo/mw_curve_stereo.h>
#include <vsol/vsol_polyline_2d_sptr.h>

//: ***beta version***
void mw_discrete_corresp_algo::
print_difference(const mw_discrete_corresp_n &c1, const mw_discrete_corresp_n &c2)
{

  vcl_cout << "------ Comparing ------\n";
  if (c1.n() != c2.n()) {
    vcl_cout << "Different dimensions.\n";
    return;
  }
  
  vbl_sparse_array_base<mw_match_attribute,mw_ntuplet>::const_iterator r;
  for (r = c1.l_.begin(); r != c1.l_.end(); ++r){
    if (!c2.l_.fullp(r->first)) {
      vcl_cout << "C1 contains: " << (r->first)<< " but C2 doesn't\n";
    }
  }

  for (r = c2.l_.begin(); r != c2.l_.end(); ++r){
    if (!c1.l_.fullp(r->first)) {
      vcl_cout << "C2 contains: " << r->first << " but C1 doesn't\n";
    }
  }
}

void mw_discrete_corresp_algo::
compose(const mw_discrete_corresp &a, const mw_discrete_corresp &b, 
    mw_discrete_corresp *ab_ptr)
{
  mw_discrete_corresp &ab = *ab_ptr;
  ab.set_size(b.n_objects_view_0(), a.n_objects_view_1());

  assert(b.n_objects_view_1() == a.n_objects_view_0());

  for (unsigned i=0; i < b.n0(); ++i) {
    // For each (i,k) given in b[i], traverse a[k] and insert.
    for (mw_discrete_corresp::one_corresp_list_const_iter bitr = b[i].begin();
        bitr != b[i].end(); ++bitr) {

      unsigned k = bitr->id();
      ab[i].insert(ab[i].end(), a[k].begin(), a[k].end());
    }
  }
}

unsigned long 
mw_discrete_corresp_algo::
compute_checksum(const mw_curve_stereo &s)
{
  unsigned long cksum=0;
  double len=0;
  for (unsigned i=0; i < s.num_curves(s.v0()); ++i) {
    cksum += static_cast<unsigned long>(s.curves(s.v0(), i)->size());
    len += s.curves(s.v0(), i)->length();
  }
  for (unsigned i=0; i < s.num_curves(s.v1()); ++i) {
    cksum += static_cast<unsigned long>(s.curves(s.v1(), i)->size());
    len += s.curves(s.v1(), i)->length();
  }

  cksum += 100001*s.num_curves(s.v0());
  cksum += 130217*s.num_curves(s.v1());
  cksum += 37*static_cast<unsigned long>(len);
  return cksum;
}

unsigned long 
mw_discrete_corresp_algo::
compute_checksum(
    const vcl_vector<vsol_polyline_2d_sptr> &pts0, 
    const vcl_vector<vsol_polyline_2d_sptr> &pts1)
{
  unsigned long cksum=0;
  double len=0;

  for (unsigned i=0; i < pts0.size(); ++i) {
    cksum += static_cast<unsigned long>(pts0[i]->size());
    len += pts0[i]->length();
  }

  for (unsigned i=0; i < pts1.size(); ++i) {
    cksum += static_cast<unsigned long>(pts1[i]->size());
    len += pts1[i]->length();
  }

  cksum += 100001*pts0.size();
  cksum += 130217*pts1.size();
  cksum += 37*static_cast<unsigned long>(len);
  return cksum;
}

MW_DISCRETE_CORRESP_ALGO_INSTANTIATE(vsol_polyline_2d_sptr);
