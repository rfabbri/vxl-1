// This is mw_point_matcher.h
#ifndef mw_discrete_corresp_algo_h
#define mw_discrete_corresp_algo_h
//:
//\file
//\brief Code to deal with discrete corresp. structures.
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date Mon Nov 6  2006
//
#include <vcl_algorithm.h>
#include <mw/mw_discrete_corresp.h>
#include <mw/mw_discrete_corresp_n.h>
#include <mw/mw_util.h>

class mw_curve_stereo;

class mw_discrete_corresp_algo {
  public:

  static void 
  print_difference(const mw_discrete_corresp_n &c1, const mw_discrete_corresp_n &c2);

  //: Compose two correspondences:
  //
  // \p a maps a0 to a1
  // \p b maps b0 to a0
  // \p ab maps b0 to a1
  static void compose(const mw_discrete_corresp &a, const mw_discrete_corresp &b, 
      mw_discrete_corresp *ab_ptr);

  //: Forms corresp indicating which element from the vcl_vector \p from is in
  // to.
  template <typename Tptr> static void 
  match_equal(const vcl_vector<Tptr> &from, const vcl_vector<Tptr> &to,
      mw_discrete_corresp *c_ptr)
  {
    mw_discrete_corresp &c = *c_ptr;

    c.set_size(from.size(), to.size());
    for (unsigned i=0; i < from.size(); ++i) {
      bool found = false;
      unsigned k=0;
      for (; k < to.size(); ++k) {
        if (*to[k] == *from[i]) {
          found = true;
          break;
        }
      }
      if (found)
        c[i].push_back(mw_attributed_object(k));
    }
  }

  //: Forms corresp indicating which element from the vcl_vector \p from is a
  // subcurve of some curve in \p to.
  template <typename Tptr> static void 
  match_subcurves(const vcl_vector<Tptr> &from, const vcl_vector<Tptr> &to,
      mw_discrete_corresp *c_ptr);

  //: Given:
  //
  //  acorr between a0 and a1
  //
  //  and given b0 having some curves in common with a0
  //  and given b1 having some curves in common with a1
  //
  //  extend acorr to a correspondence bcorr between b0 and b1,
  //  defined by acorr restricted to the common elements.
  template <typename Tptr> static void 
  extend(
      const vcl_vector<Tptr> &a0, const vcl_vector<Tptr> &a1,
      const vcl_vector<Tptr> &b0, const vcl_vector<Tptr> &b1,
      const mw_discrete_corresp &acorr,
      mw_discrete_corresp *bcorr_ptr)
  {
    mw_discrete_corresp &bcorr = *bcorr_ptr;

    mw_discrete_corresp equal_ba0;
    match_equal(b0, a0, &equal_ba0);

    mw_discrete_corresp equal_ab1;
    match_equal(a1, b1, &equal_ab1);

    mw_discrete_corresp a_comp_equal_ba0;
    compose(acorr, equal_ba0, &a_comp_equal_ba0);
    compose(equal_ab1, a_comp_equal_ba0, &bcorr);
    bcorr.set_checksum(compute_checksum(b0, b1));
  }

  //: Given:
  //
  //  acorr between a0 and a1
  //
  //  and given b0 having some curves as subcurves of some curves in a0
  //  and given b1 having some curves as subcurves of some curves in a1
  //
  //  extend acorr to a correspondence bcorr between b0 and b1,
  //  defined by acorr restricted to the common elements.
  template <typename Tptr> static void 
  extend_to_subcurves(
      const vcl_vector<Tptr> &a0, const vcl_vector<Tptr> &a1,
      const vcl_vector<Tptr> &b0, const vcl_vector<Tptr> &b1,
      const mw_discrete_corresp &acorr,
      mw_discrete_corresp *bcorr_ptr)
  {
    mw_discrete_corresp &bcorr = *bcorr_ptr;

    mw_discrete_corresp equal_ba0;
    match_subcurves(b0, a0, &equal_ba0);

    mw_discrete_corresp equal_ab1;
    match_subcurves(a1, b1, &equal_ab1);

    mw_discrete_corresp a_comp_equal_ba0;
    compose(acorr, equal_ba0, &a_comp_equal_ba0);
    compose(equal_ab1, a_comp_equal_ba0, &bcorr);
    bcorr.set_checksum(compute_checksum(b0, b1));
  }

  static unsigned long compute_checksum(const mw_curve_stereo &s);
  static unsigned long compute_checksum(
      const vcl_vector<vsol_polyline_2d_sptr> &pts0, 
      const vcl_vector<vsol_polyline_2d_sptr> &pts1);
};

#define MW_DISCRETE_CORRESP_ALGO_INSTANTIATE(T) extern "please include mw/mw_discrete_corresp_algo.txx first"

#endif // mw_discrete_corresp_algo_h
