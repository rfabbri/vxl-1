//: Aug 19, 2005 MingChing Chang
//  Defines the Interval Section for Goedesic DT Wavefront Propagation Algorithm

#ifndef gdt_interval_section_h_
#define gdt_interval_section_h_

#include <vcl_cmath.h>
#include <vcl_utility.h>
#include <vcl_map.h>

#include <gdt/gdt_interval.h>

typedef enum {
  BOGUS_INTERSECT_RESULT = -1,
  INTERSECT_RESULT_0 = 0,
  INTERSECT_RESULT_1 = 1,
  INTERSECT_RESULT_2 = 2,
  INTERSECT_RESULT_3 = 3,
  INTERSECT_RESULT_4 = 4,
} INTERSECT_RESULT;

//: The set of interval section
class gdt_interval_section 
{
protected:
  //: a map of intervals sorted by its stau as key. 
  //  Note that the intervals can NOT overlap.
  vcl_map<double, gdt_ibase*> I_map_;

  //: the extent of the set is [0, len]
  //  should equal to the length of the underlying mesh edge.
  double    len_;

public:
  //: ====== Constructor/Destructor ======
  gdt_interval_section () {
  }
  gdt_interval_section (double len) {
    len_ = len;
  }

  ~gdt_interval_section () {
    clear_I_map ();
  }

  void clear_I_map ();

  //: ====== Data access functions ======
  vcl_map<double, gdt_ibase*>* I_map() {
    return &I_map_;
  }
  unsigned int size() {
    return I_map_.size();
  }
  double len() const {
    return len_;
  }
  void set_len (double l) {
    len_ = l;
  }

  //: ====== Query functions ======  
  gdt_ibase* _find_interval (const double stau) {
    vcl_map<double, gdt_ibase*>::iterator it = I_map_.find (stau);
    if (it == I_map_.end())
      return NULL;
    ///gdt_ibase* I = (*it).second;
    return (*it).second;
  }

  bool _is_I_overlap (const gdt_ibase* input_I);
  bool _is_I_overlap2 (const gdt_ibase* input_I);

  bool is_a_coverage ();

  //: if all correct, return true
  bool assert_coverage_no_gap_overlap ();

  void debug_print ();

  //: ====== Modification Functions ======

  void _add_interval (gdt_ibase* I) {
    assert (!_eqT (I->stau(), I->etau()));
    assert (_find_interval(I->stau()) == NULL);
    I_map_.insert (vcl_pair<double, gdt_ibase*>(I->stau(), I));
  }

  //: just remove it from the map, not deleting it
  void _remove_interval (gdt_ibase* I) {
    I_map_.erase (I->stau());
  }

  //: remove all intervals whose stau and etau < input_stau
  //  if the interval I < input_tau, delete it directly.
  //  else, input_tau is between its (stau, etau), trim it.
  void delete_tau_less_than (double input_tau);

  //: remove all intervals whose etau (stau) > input_etau
  //  if the interval I > input_tau, delete it directly.
  //  else, input_tau is between its (stau, etau), trim it.
  void delete_tau_greater_than (double input_tau);

  //: do a brute-force numerical check and add
  void add_interval_numfix (gdt_ibase* I);

  //: ====== Intersection Functions ======

  void fill_dummy_intervals ();

  void clone_add_partial (gdt_ibase* I, double itau_min, double itau_max) {
    assert (itau_min+GDT_TAU_EPSILON_DIV < itau_max);

    gdt_ibase* new_I=NULL;
    switch (I->type()) {
    case ITYPE_PSRC:
    case ITYPE_DEGE:
      new_I = new gdt_interval ((gdt_interval*) I);
    break;
    default:
      break;
    }

    new_I->_set_stau (itau_min);
    new_I->_set_etau (itau_max);
    I_map_.insert (vcl_pair<double, gdt_ibase*>(new_I->stau(), new_I));
  }

  void clone_intersect_overlap (gdt_interval* I1, gdt_interval* I2, 
                                double itau_min, double itau_max,
                                bool& b_I1_min, bool& b_I2_min);
  
  void merge_same_intervals ();
  void fix_boundary_intervals (gdt_interval_section* IS1, 
                               gdt_interval_section* IS2);
};

//: move intervals between sesstions.
inline void move_intervals (gdt_interval_section* from_IS, 
                            gdt_interval_section* dest_IS)
{
  assert (dest_IS->I_map()->size()==0);
  vcl_map<double, gdt_ibase*>::iterator it = from_IS->I_map()->begin();
  while (it != from_IS->I_map()->end()) {
    gdt_interval* I = (gdt_interval*) (*it).second;

    #if GDT_ALGO_F
    assert (I->b_attach_to_edge() == false);
    #endif

    from_IS->I_map()->erase (it);
    dest_IS->I_map()->insert (vcl_pair<double, gdt_ibase*>(I->stau(), I));
    it = from_IS->I_map()->begin();
  }
}

inline void move_intervals_numfix (gdt_interval_section* from_IS, 
                                   gdt_interval_section* dest_IS)
{
  vcl_map<double, gdt_ibase*>::iterator it = from_IS->I_map()->begin();
  while (it != from_IS->I_map()->end()) {
    gdt_interval* I = (gdt_interval*) (*it).second;

    #if GDT_ALGO_F
    assert (I->b_attach_to_edge() == false);
    #endif

    from_IS->I_map()->erase (it);
    dest_IS->add_interval_numfix (I);
    it = from_IS->I_map()->begin();
  }
}

inline void clone_intervals (gdt_interval_section* from_IS, 
                             gdt_interval_section* dest_IS)
{
  assert (dest_IS->I_map()->size() == 0);
  assert (from_IS->len() == dest_IS->len());

  vcl_map<double, gdt_ibase*>::iterator it = from_IS->I_map()->begin();
  for (; it != from_IS->I_map()->end(); it++) {
    gdt_ibase* I = (*it).second;

    gdt_ibase* I_new=NULL;
    switch (I->type()) {
    case ITYPE_PSRC:
    case ITYPE_DEGE:
      I_new = new gdt_interval ((gdt_interval*) I);
    break;
    default:
      break;
    }

    dest_IS->I_map()->insert (vcl_pair<double, gdt_ibase*>(I_new->stau(), I_new));
  }
}

//  return 1: only the first one contribs to the min (no merge)
//  return 2: only the second one contribs to the min (no merge)
//  return 3: both contrib to the min (no merge)
//  return 4: both contrib to the min (merge required)
//  return 0: the two are the same hyperbola (degenerate)
INTERSECT_RESULT _intersect_IS (gdt_interval_section* IS1,
                                gdt_interval_section* IS2,
                                gdt_interval_section* result_IS);

//: intersect an interval to an interval_section.
//  Here we are trying to get the minumum of two hyperbola sections min(h1, h2)
//  The result is an interval_section, result_IS.
//  return 1: only the first one contribs to the min (no merge)
//  return 2: only the second one contribs to the min (no merge)
//  return 3: both contrib to the min (no merge)
//  return 4: both contrib to the min (merge required)
//  return 0: the two are the same hyperbola (degenerate)
INTERSECT_RESULT _intersect_I_IS (gdt_interval* I, 
                                  gdt_interval_section* input_IS2,
                                  gdt_interval_section* result_IS);

//: Use _intersect_I_IS() with input_IS2 = result_IS.
INTERSECT_RESULT intersect_I_with_IS (gdt_interval* I, gdt_interval_section* IS);

//: intersect two interval_sections, input_IS1, input_IS2.
//  Here we are trying to get the minumum of two hyperbola sections min(h1, h2)
//  The result is an interval_section, result_IS.
//  return 1: only the first one contribs to the min (no merge)
//  return 2: only the second one contribs to the min (no merge)
//  return 3: both contrib to the min (no merge)
//  return 4: both contrib to the min (merge required)
//  return 0: the two are the same hyperbola (degenerate)
INTERSECT_RESULT intersect_interval_sections (gdt_interval_section* input_IS1,
                                              gdt_interval_section* input_IS2,
                                              gdt_interval_section* result_IS);

#endif



