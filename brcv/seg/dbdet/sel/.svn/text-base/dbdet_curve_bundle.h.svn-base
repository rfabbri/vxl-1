// This is dbdet_curve_bundle.h
#ifndef dbdet_curve_bundle_h
#define dbdet_curve_bundle_h
//:
//\file
//\brief Various classes representing curve bundle representations
//\author Amir Tamrakar
//\date 09/05/06
//
//\verbatim
//  Modifications
//  Amir Tamrakar 09/05/06   Moved it from dbdet_se1.h to a new file
//
//\endverbatim

#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vcl_list.h>
#include <vcl_set.h>
#include <vcl_map.h>
#include <vcl_utility.h>

#include <vnl/vnl_math.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>

#include <dbgl/algo/dbgl_eulerspiral.h>
#include "dbdet_edgel.h"
#include "dbdet_sel_utils.h"

//: This class stores a curve bundle as a region in the parameter space 
//  of the curve model it's associated with. It also contains the 
//  methods to compute the bundles from edge measurements given expected 
//  uncertainties
class dbdet_curve_bundle
{
public:
  vgl_polygon<double> poly; ///< the region in the parameters space defining the curve bundle
  
  //: default constructor
  dbdet_curve_bundle();

  //: constructor 1" construct this curve bundle from a set of edgels
  dbdet_curve_bundle(vcl_vector<dbdet_edgel*> edgels, dbdet_edgel* ref_edgel)=0;

  //: constructor 2: construct a curve bundle from the intersection of two curve bundles
  dbdet_curve_bundle(dbdet_curve_bundle* cb1, dbdet_curve_bundle* cb2)=0;

  //: copy constructor
  dbdet_curve_bundle(const dbdet_curve_bundle& cb)=0;

  //: is this bundle valid?
  bool is_valid() { return poly.num_sheets() != 0; }

  //: Compute the centroid of this curve bundle 
  vgl_point_2d<double> get_centroid();

};

class dbdet_linear_curve_bundle
{
public:  
  //: default constructor
  dbdet_linear_curve_bundle(){};

  //: constructor 1" construct this curve bundle from a set of edgels
  dbdet_linear_curve_bundle(vcl_vector<dbdet_edgel*> edgels){};

  //: constructor 2: construct a curve bundle from the intersection of two curve bundles
  dbdet_linear_curve_bundle(dbdet_curve_bundle* cb1, dbdet_curve_bundle* cb2){};

  //: copy constructor
  dbdet_linear_curve_bundle(const dbdet_curve_bundle& cb){};

};

class dbdet_CC_curve_bundle
{
public:  
  //: default constructor
  dbdet_CC_curve_bundle(){};

  //: constructor 1" construct this curve bundle from a set of edgels
  dbdet_CC_curve_bundle(vcl_vector<dbdet_edgel*> edgels){};

  //: constructor 2: construct a curve bundle from the intersection of two curve bundles
  dbdet_CC_curve_bundle(dbdet_curve_bundle* cb1, dbdet_curve_bundle* cb2){};

  //: copy constructor
  dbdet_CC_curve_bundle(const dbdet_curve_bundle& cb){};

};

class dbdet_ES_curve_bundle
{
public:

  //: default constructor
  dbdet_ES_curve_bundle(){};

  //: constructor 1" construct this curve bundle from a set of edgels
  dbdet_ES_curve_bundle(vcl_vector<dbdet_edgel*> edgels){};

  //: constructor 2: construct a curve bundle from the intersection of two curve bundles
  dbdet_ES_curve_bundle(dbdet_curve_bundle* cb1, dbdet_curve_bundle* cb2){};

  //: copy constructor
  dbdet_ES_curve_bundle(const dbdet_curve_bundle& cb){};

  //: determine if edgel pair is legal
  bool edgel_pair_legal(dbdet_int_params &params, double tan1, double tan2);

  //: determine if an edgel pair can be formed between these edgels
  bool edgel_pair_legal1(dbdet_edgel* e1, dbdet_edgel* e2, 
                         dbdet_edgel* ref_e, vgl_polygon<double> & cb);

  //: determine if an edgel pair can be formed between these edgels
  bool edgel_pair_legal2(dbdet_edgel* e1, dbdet_edgel* e2, 
                         dbdet_edgel* ref_e, vgl_polygon<double> & cb);

  //: determine if an edgel pair can be formed between these edgels
  bool edgel_pair_legal2(dbdet_int_params &params, bool first_is_ref);

  //: compute the ES curve bundle for an edgel pair given expected errors
  vgl_polygon<double> compute_curve_bundle(dbdet_edgel* e1, dbdet_edgel* e2, dbdet_edgel* ref_e);

  //: compute the ES curve bundle for an edgel pair given intrinsic params
  vgl_polygon<double> compute_curve_bundle(dbdet_int_params &params, bool first_is_ref);

};


#endif // dbdet_curve_bundle_h
