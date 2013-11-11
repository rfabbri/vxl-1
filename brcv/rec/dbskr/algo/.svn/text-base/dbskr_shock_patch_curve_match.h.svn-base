//---------------------------------------------------------------------
// This is brcv/rec/dbskr/dbskr_shock_patch_curve_match.h
//:
// \file
// \brief Class to hold shock patch curve matches, Given two images, extract its patches
//                                                 and match two patch sets
//                                                 This class holds all the corresponding elastic curve
//                                                 matches in the second set sorted wrt 
//                                                 some criteria, e.g. best top two matches in the curve sets
//            
//
// \author
//  O.C. Ozcanli - March 13, 2007
//
// \verbatim
//
//  Modifications
//
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef _dbskr_shock_patch_curve_match_h
#define _dbskr_shock_patch_curve_match_h

#include <vbl/vbl_ref_count.h>
#include <vcl_map.h>
#include <vcl_utility.h>

#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vgl/vgl_polygon.h>

#include <vil/vil_image_view.h>

#include <dbskr/dbskr_shock_patch.h>
#include <dbcvr/dbcvr_cv_cor_sptr.h>

#include "dbskr_shock_patch_curve_match_sptr.h"
#include <dbskr/algo/dbskr_shock_patch_match.h>

typedef vcl_map<int, vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >* > patch_curve_cor_map_type;
typedef vcl_map<int, vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >* >::iterator patch_curve_cor_map_iterator;

//: Shock Curve class
class dbskr_shock_patch_curve_match : public dbskr_shock_patch_match /*public vbl_ref_count */
{ 
public:
  
  //: default constructor
  dbskr_shock_patch_curve_match() {}

  //: Interpolate/copy constructor
  //  Constructs and interpolated copy of an scurve
  //dbskr_shock_patch_curve_match(dbskr_shock_patch_curve_match const& old);

  //: Destructor
  ~dbskr_shock_patch_curve_match() { clear(); }
  
  //: Assignment operator
  //dbskr_shock_patch_curve_match& operator=(const dbskr_shock_patch_curve_match &rhs);

  void clear();

  patch_curve_cor_map_type& get_map() { return patch_curve_cor_map_; }
  /*vcl_map<int, dbskr_shock_patch_sptr>& get_id_map1() { return map1_; } 
  vcl_map<int, dbskr_shock_patch_sptr>& get_id_map2() { return map2_; } 
  void set_id_map1(vcl_map<int, dbskr_shock_patch_sptr>& map) { map1_ = map; } 
  void set_id_map2(vcl_map<int, dbskr_shock_patch_sptr>& map) { map2_ = map; } 

  void set_patch_set2(vcl_vector<dbskr_shock_patch_sptr>& patch_set) { patch_set2_ = patch_set; }
  vcl_vector<dbskr_shock_patch_sptr>& get_patch_set2(void) { return patch_set2_; }
*/
  //: sort again wrt norm costs
  void resort_wrt_costs();

  //: get the best match of the patch with this id
 // vcl_pair<int, vcl_vector<float> >& get_best_match(int patch_id) { return (*patch_cor_map_[patch_id])[0]; }

  //: get the top n best match of the patch with this id
  //vcl_vector< vcl_pair<int, dbskr_sm_cor_sptr> >* get_best_n_match(int patch_id, int n); 

  //: construct the match for the second image, does not sort wrt any type of cost
  dbskr_shock_patch_curve_match_sptr construct_curve_match_just_cost();  

  //: match correspondences are kept for only top n patch real contours for each image
  void set_n(int n) { n_ = n; }

  //-----------------------
  //:  BINARY I/O METHODS |
  //-----------------------

  //: Serial I/O format version
  virtual unsigned version() const {return 1;}

  //: Return a platform independent string identifying the class
  virtual vcl_string is_a() const {return "dbskr_shock_patch_curve_match";}

  //: determine if this is the given class
  virtual bool is_class(vcl_string const& cls) const
   { return cls==is_a();}
  
  //: Binary save self to stream.
  virtual void b_write(vsl_b_ostream &os) const ;

  //: Binary load self from stream.
  virtual void b_read(vsl_b_istream &is);
  
public:
  
  //----------------------------------------
  // Shock Patch Match Data
  //----------------------------------------

  //: patch correspondence map, the vector in this map is always sorted wrt some criteria
  patch_curve_cor_map_type patch_curve_cor_map_;

  //: keep the id to sptr map if available
  //vcl_map<int, dbskr_shock_patch_sptr> map1_;
  //vcl_map<int, dbskr_shock_patch_sptr> map2_;

  //vcl_vector<dbskr_shock_patch_sptr> patch_set2_;

  //: size of dbcvr_cv_cor vectors in an id, vector pair
  int n_;  // use top n best matches of the real curve correspondences of two patches

  //: required for visualization
  //dbskr_shock_patch_sptr left_, right_;
  vcl_vector<dbcvr_cv_cor_sptr> curve_cor_;
  vcl_vector<vcl_pair<int, vcl_vector<dbcvr_cv_cor_sptr> > >* left_curve_v_;
  //int current_left_v_id_;

};

#endif  // _dbskr_shock_patch_curve_match_h
