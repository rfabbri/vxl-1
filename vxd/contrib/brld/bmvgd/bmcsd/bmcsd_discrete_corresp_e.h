// This is bmcsd_discrete_corresp_e.h
#ifndef bmcsd_discrete_corresp_e_h
#define bmcsd_discrete_corresp_e_h
//:
//\file
//\brief Data structure representing correspondence between two sets of objects
//\author Ricardo Fabbri (rfabbri), Brown University  (@gmail.com)
//\date 04/25/2005 06:41:03 PM EDT
// 
// Modifications:
// Wed Sep 30 02:12:23 EDT 2009   Ricardo Fabbri  Renamed things: s/point/object/g
//

#include <cassert>
#include <vector>
#include <list>
#include <algorithm>
#include <vsl/vsl_binary_io.h>
#include <bmcsd/bmcsd_discrete_corresp.h> // for _e version we use some of the original
#include <vcl_set.h>


// _match_attribute is from original (without _e)

//: This is to be used within the bmcsd_discrete_corresp_e class;
struct bmcsd_attributed_object_e : public bmcsd_attributed_object {
public:
  bmcsd_attributed_object_e() { }

  bmcsd_attributed_object(unsigned obj_idx, unsigned container_idx, bool isnull=false, double cost=0)
   :
    mw_match_attribute(isnull, cost),
    obj_(obj_idx),
    container_id_(container_idx)
  {
  }

  bmcsd_attributed_object(unsigned obj_idx, std::vector<std::set<int> > &supportStructures, 
                     std::vector<unsigned> &inliers, bool isnull=false, double cost=0)
   :
    mw_match_attribute(isnull, cost),
    obj_(obj_idx),
    supportingStructures_(supportStructures),
    inliers_(inliers)
  {
  }

  bmcsd_attributed_object(unsigned obj_idx, unsigned container_idx, std::vector<std::set<int> > &supportStructures, 
                     std::vector<unsigned> &inliers, bool isnull=false, double cost=0)
   :
    mw_match_attribute(isnull, cost),
    obj_(obj_idx),
    supportingStructures_(supportStructures),
    inliers_(inliers),
    container_id_(container_idx)
  {
  }

  bmcsd_attributed_object(unsigned obj_idx, unsigned container_idx, std::vector<std::set<int> > &supportStructures, 
		       std::vector<unsigned> &inliers, std::vector<unsigned> &supportCount, 
		       bool isnull=false, double cost=0, std::vector<std::vector<int> > indexChain=std::vector<std::vector<int> >())
   :
    mw_match_attribute(isnull, cost),
    obj_(obj_idx),
    supportingStructures_(supportStructures),
    inliers_(inliers),
    container_id_(container_idx),
    support_count_(supportCount),
    index_chain_(indexChain)
  {
  }

  ~bmcsd_attributed_object_e() {}
  
  // I/O ------------------------------------------------------------------

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;
  
  //: Print an ascii summary to the stream
  void print_summary(std::ostream &os) const;

  friend std::ostream&  operator<<(std::ostream& s, const bmcsd_attributed_object_e &c);

  std::string is_a() const;

public:
   //Anil: The ID for the container of obj_ (in multiview stereo, this is view ID)
   unsigned container_id_;

   //Anil: A vector of supporting structure list, implemented with multiview edge support in mind
   //TODO: Make abstract, maybe move to a subclass
   std::vector<std::set<int> > supportingStructures_;

   //Anil: A vector of supporting structure count, implemented with multiview edge support in mind
   std::vector<unsigned> support_count_;

   //Anil: A 2D vector of supporting structure IDs (per view, per sample)
   std::vector<std::vector<int> > index_chain_;

   //Anil: A vector of inlier IDs, implemented with multiview stereo in mind
   std::vector<unsigned> inliers_;
};

class bmcsd_attributed_object_e_eq : public std::unary_function<bmcsd_attributed_object_e_eq, bool> {
  unsigned p_;
public:
  explicit bmcsd_attributed_object_e_eq(const unsigned pp) : p_(pp) { }; 
  bool operator() (const bmcsd_attributed_object_e_e &e) const { return e.obj_ == p_; }
};

//: Binary save vnl_my_class to stream.
inline void vsl_b_write(vsl_b_ostream &os, const bmcsd_attributed_object_e & v)
{
  v.b_write(os);
}

//: Binary load vnl_my_class from stream.
inline void vsl_b_read(vsl_b_istream &is, bmcsd_attributed_object_e & v)
{
  v.b_read(is);
}

inline void vsl_print_summary(std::ostream &os, const bmcsd_attributed_object_e &p)
{
  p.print_summary(os);
}


//: We support many-to-one, one-to-many, and even zero-to-many and many-to-zero
// Access is directly on datastructure, since its kinda low-level
//
// The object sets are indexed in the same way as in the initialization object
// vectors.
//
// The representation for an object is an id (unsigned int). Therefore, objects
// can be anything having an id. Each object can be a single point, or a SIFT
// point, or a curve fragment. Anything goes.
//
// Enhnanced _e version is same as original, but using attributed_object_e
struct bmcsd_discrete_corresp_e {
public:
  typedef std::list< bmcsd_attributed_object_e > one_corresp_list;
  typedef std::list< bmcsd_attributed_object_e >::iterator one_corresp_list_iter;
  typedef std::list< bmcsd_attributed_object_e >::const_iterator one_corresp_list_const_iter;
  typedef std::vector < std::list< bmcsd_attributed_object_e > > corresp_data;

  bmcsd_discrete_corresp_e(unsigned num_objs0, unsigned num_objs1) 
     :
     corresp_(num_objs0+1),
     num_counterdomain_(num_objs1+1),
     cksum_(num_objs0+2*num_objs1) // arbitrary hash.
  { }
  bmcsd_discrete_corresp_e() {}
  ~bmcsd_discrete_corresp_e() {}

  void set_size(unsigned num_objs0, unsigned num_objs1) 
  { corresp_.resize(num_objs0+1); num_counterdomain_ = num_objs1+1; }

  //: Set a checksum for the objects in the domain and counter-domain. This is
  // to ensure this correspondence is for the objects it was intended for.
  void set_checksum(unsigned long long c) { cksum_ = c;}
  unsigned long checksum() const {return cksum_; }

  //: adds object, testing if is unique, and also resize corresp. list if needed
  bool add_unique(const bmcsd_attributed_object_e &e, unsigned i, 
      std::list<bmcsd_attributed_object_e>::iterator *itr);

  void threshold_by_cost(double cost);
  void threshold_by_cost_lteq(double cost);
  void sort();

  //: \param[in] maxmin : if true, keep only max cost. if false, keep only min
  // cost.
  void keep_only_extreme_cost(bool keep_max)
  {
    sort();
    for (unsigned i=0; i < n0(); ++i) {
      if (corresp_[i].empty())
        continue;
      bmcsd_attributed_object_e attr;
      if (keep_max)
        attr = corresp_[i].back();
      else
        attr = corresp_[i].front();
      corresp_[i].clear();
      corresp_[i].push_back(attr);
    }
  }

  //: Use lowe-style disambiguation, i.e.,
  // keep only the match having max #votes,
  // if this #votes is better than the ratio*#votes of second best.
  //
  // If there is no second best to compare, demand that the match be way beyond
  // lonely_threshold.
  //
  // Extremes behavior is as follows:
  //
  // With ratio == 0, the behavior is that of keeping the max cost.
  //
  // With ratio != 0 and: 
  //    - lonely_threshold == infinity, the behavior is to never include loners
  //    - lonely_threshold == 0, the behavior is to always include loners
  //
  // With ratio == infinity and lonely_threshold != infinity, the behavior is to
  // never include correspondences having a second best, no matter how bad the
  // second best is.
  //
  // This assumes there are no correspondences with 0 or Inf cost.
  void keep_only_unambiguous_max(double ratio=1.5, double lonely_threshold = 5.0);

  //: returns the total number of correspondences.
  unsigned num_corresps() const
  {
    unsigned sum = 0;
    for (unsigned i=0; i < n0(); ++i) {
      sum += corresp_[i].size();
    }
    return sum;
  }
  const one_corresp_list & operator[](unsigned i) const { assert (i < n0()); return corresp_[i]; }
  one_corresp_list & operator[](unsigned i) { assert (i < n0()); return corresp_[i]; }

  // todo: add as heap / sorted insert

  // Functions to be moved to algo ----------------------------------------

  std::list<bmcsd_attributed_object_e>::const_iterator 
  find_right_corresp_mincost(unsigned p1_idx, const bmcsd_discrete_corresp_e *gt) const;

  void 
  percentage_of_matches_above_truth(unsigned &n, unsigned &n_valid, const bmcsd_discrete_corresp_e *gt) const;

  void
  number_of_pts1_with_gt_among_any_candidates(unsigned &n_w_gt, const bmcsd_discrete_corresp_e *gt) const;

  bool is_gt_among_top5(unsigned p1_idx, const bmcsd_discrete_corresp_e *gt) const;

  //: \todo functional access

  friend std::ostream&  operator<<(std::ostream& s, const bmcsd_discrete_corresp_e &c);

  //: Equality test
  inline bool operator==(bmcsd_discrete_corresp_e const &that) const
  { return corresp_ == that.corresp_; }

  // I/O ------------------------------------------------------------------

  //: Binary save self to stream.
  void b_write(vsl_b_ostream &os) const;

  //: Binary load self from stream.
  void b_read(vsl_b_istream &is);

  //: Return IO version number;
  short version() const;

  //: Print an ascii summary to the stream
  void print_summary(std::ostream &os) const;

  //: Return a platform independent string identifying the class
  std::string is_a() const;

  //: # objects in view 1; returns (unsigned) -1  in case nothing was initialized yet
  unsigned n_objects_view_0() const {return corresp_.size() - 1; }
  unsigned n_objects_view_1() const {return n1() - 1; }
  unsigned n0() const { return corresp_.size(); }
  unsigned n1() const { return num_counterdomain_;}

  unsigned size() const {return n_objects_view_0(); }
  bool is_empty() const {return corresp_.size() == 0; }

  // returns the number of objects (in image 1) having no correspondence.
  // If the dummy object has no correspondence, it also counts.
  unsigned count_empty() const { 
    return std::count_if(corresp_.begin(), corresp_.end(), 
                        std::mem_fun_ref(&one_corresp_list::empty )); 
  }

  void compare_and_print( const bmcsd_discrete_corresp_e *gt) const;

public:
   corresp_data corresp_;
private:
   unsigned num_counterdomain_;
   unsigned long cksum_;
};

//: Binary save vnl_my_class to stream.
inline void vsl_b_write(vsl_b_ostream &os, const bmcsd_discrete_corresp_e & v)
{
  v.b_write(os);
}

//: Binary load vnl_my_class from stream.
inline void vsl_b_read(vsl_b_istream &is, bmcsd_discrete_corresp_e & v)
{
  v.b_read(is);
}

#endif // bmcsd_discrete_corresp_e_h
