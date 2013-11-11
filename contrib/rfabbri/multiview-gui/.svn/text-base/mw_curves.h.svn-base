// This is mw_curves.h
#ifndef mw_curves_h
#define mw_curves_h
//:
//\file
//\brief Class that deals with storage and querying of geno-interpolated curves
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date 04/25/2005 12:21:35 PM EDT
//

#include <vcl_vector.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <dbsol/dbsol_gshock_curve_2d_sptr.h>
#include <dbsol/algo/dbsol_geno_shock.h>


//: This is kinda specific; maybe generalize in the future
// The main objective is to deal with efficient pixel-curve querying
// 
// \section ACCESSORS provide direct but read-only access; useful for safe
// programming; but user may also use the data directly and eventually call
// check_consistency
class mw_curves {
public:
   //: curve having i-th pixel
   // i.e. pixel id to curve id

   //: geno curves
   // index into this vector is the curve id
   mw_curves(vcl_vector< vcl_vector<vsol_point_2d_sptr> > *curvepts);
   ~mw_curves();
   
   //: set_points
   // set the vector of points
   
   //: set_geno_curves

   //: set_curveid
   // sets the curveid and checks for consistency between points and curves


   const vcl_vector<vsol_point_2d_sptr> & pt() const {return pt_;}
   // returns ith point
   const vsol_point_2d &pt(unsigned i) const {return *pt_[i];}

   unsigned long npts() const {return pt_.size();}

   //: curveid()
   // curve id given pixel id
   const vcl_vector<double>& curveid() const {return curveid_;}
   
   // ith geno curve
   //: gcurve()
   const vcl_vector<dbsol_gshock_curve_2d_sptr> & gcurve() const {return gcurve_;}
   const dbsol_gshock_curve_2d_sptr gcurve(unsigned i) const {return gcurve_[i];}

   void compute_geno();

   void nearest_point_to(double x, double y,  unsigned long *np, double *min_distance);


   //: point_index()
   // returns index of point by searching though the points vector

   //: sort()
   // sorts the point vector wrt. some criterion and update the pixelid vector
   // consistently

   //: output description to stream
   friend
   vcl_ostream&  operator<<(vcl_ostream& s, const mw_curves &c);


   bool check_consistency() const;

   //: list of all pixels of all curves; 
   // index into this vector is the pixel id
   vcl_vector<vsol_point_2d_sptr> pt_;

   //: curve having i-th pixel
   // i.e. pixel id to curve id
   vcl_vector<double> curveid_;

   //: geno curves
   // index into this vector is the curve id
   vcl_vector<dbsol_gshock_curve_2d_sptr> gcurve_;

   //: vector of cached pixels for each curve 
   vcl_vector< vcl_vector<vsol_point_2d_sptr> > &curvepts_;
};


#endif // mw_curves_h
