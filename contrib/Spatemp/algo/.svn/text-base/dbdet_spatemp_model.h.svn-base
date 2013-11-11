// This is dbdet_spatemp_model.h
#ifndef dbdet_spatemp_model_h
#define dbdet_spatemp_model_h
//:
//\file
//\brief Various curve mdoels used for linking using gemoetric consistency
//\author Amir Tamrakar
//\date 09/05/06
//
//\verbatim
//  Modifications
//
//  Ozge Can Ozcanli Jan 15, 2007   Added copy constructor
//
//\endverbatim

#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vcl_deque.h>
#include <vnl/vnl_math.h>

#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include "dbdet_spatemp_edgel.h"

//: The base class for curve models used by the linking algorithm.
class dbdet_spatemp_model
{
public:
  enum spatemp_type { RIGID };
  spatemp_type type;

  //: default constructor
  dbdet_spatemp_model(){}

  //: copy constructor
  dbdet_spatemp_model(const dbdet_spatemp_model& other) { type = other.type; }

  //: destructor
  virtual ~dbdet_spatemp_model(){}

  //: construct and return a curve model of the same type by intersecting with another curve bundle
  virtual dbdet_spatemp_model* intersect(dbdet_spatemp_model* cm)=0;

  //: is the bundle valid?
  virtual bool bundle_is_valid()=0;

  //: Compute the best fit curve from the curve bundle 
  virtual void compute_best_fit(vcl_deque<dbdet_edgel*> &edgel_chain)=0; 

  //: function to check if the curve fit is reasonable
  virtual bool curve_fit_is_reasonable(vcl_deque<dbdet_edgel*> &edgel_chain, dbdet_edgel* ref_e, double dpos)=0;

  //: report accuracy of measurement
  virtual void report_accuracy(double *estimates, double *min_estimates, double *max_estimates)=0;

  //: print info
  virtual void print_info()=0;

};

class dbdet_rigid_spatemp_model : public dbdet_spatemp_model 
{
public:

};

//: The linear curve model class

