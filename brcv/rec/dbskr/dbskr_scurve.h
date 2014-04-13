//---------------------------------------------------------------------
// This is brcv/rec/dbskr/dbskr_scurve.h
//:
// \file
// \brief Shock curve class which includes 2 reconstructed boundaries from
//        a given path on the shock graph
//        Currently it simply includes discrete point samples on the boundaries
//        in the future these two boundary curves can be dbsol_interp_curve2d
//        with parametric intervals
//
// \author
//  O.C. Ozcanli - September 20, 2005
//
// \verbatim
//
//  Modifications
//    Amir Tamrakar May10 2006    added a new splice cost method that uses dpmatch 
//                                instead of the simplified cost function
//    Amir Tamrakar 06/10/06      I've changed the representation of the scurve to use
//                                phi instead of alpha. This removes the need for shock_dir
//                                as well. I also removed various hacks.
//
//    Ozge C Ozcanli May 17, 07   I've made Amir's previous changes that effectively combines width and orieantation costs
//                                in deformation cost into new methods so that edit distance can have two versions, and 
//                                the original version can be kept while an inheritad class as dbskr_tree_edit_combined 
//                                is can be used as the new version
//
//    Ozge C Ozcanli May 23, 07   added parameters construct_circular_ends and dpmatch_combined into the splice_cost method
//                                Splice costs should be computed according to the preferences in the editing algorithm
//
//    Amir Tamrakar Oct 31, 07    Updated the copy constructor to allow for dense resampling which was necessary
//                                to generate densely subsampled version of scurves for localized shock  matching
//
// \endverbatim
//
//-------------------------------------------------------------------------

#ifndef _dbskr_scurve_h
#define _dbskr_scurve_h

#include <vcl_string.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vcl_cmath.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include "dbskr_scurve_sptr.h"

#define EPS 1E-2

double fixAngleMPiPi_new(double a);
//: Does a1-a2
double angleDiff_new(double a1, double  a2);

//: Shock Curve class
class dbskr_scurve : public vbl_ref_count 
{ 
public:
  
  //: default constructor
  dbskr_scurve() : interpolate_ds_(1.0), subsample_ds_(5.0), num_points_(0) , 
        virtual_length_(0.0) {}

  //: contructor with data
  dbskr_scurve(int num_points,
               vcl_vector<vgl_point_2d<double> > &sh_pt, 
               vcl_vector<double> &time,
               vcl_vector<double> &theta,
               vcl_vector<double> &phi, 
               bool binterpolate=true, double interpolate_ds=1.0, 
               bool bsub_sample=true, double subsample_ds=1.0);

  //: Interpolate/copy constructor
  //  Constructs and interpolated copy of an scurve
  dbskr_scurve(dbskr_scurve& old, 
               vcl_vector<int> &lmap,
               double interpolate_ds=1.0);

  //: Destructor
  ~dbskr_scurve();
  
  //: Assignment operator
  dbskr_scurve& operator=(const dbskr_scurve &rhs);

  // set area factor
  void set_area_factor(double area_factor){area_factor_=area_factor;}

  // get area factor
  double get_area_factor(){return area_factor_;}

  // set outside shock radius
  void set_bdry_plus_outside_shock_radius(vcl_vector<double>& radius)
  {
      bdry_plus_outer_shock_radius_=radius; ///< outer shock radius
  }

  // set outside shock radius
  void set_bdry_minus_outside_shock_radius(vcl_vector<double>& radius)
  {
      bdry_minus_outer_shock_radius_=radius; ///< outer shock radius
  }

  // get outside shock radius
  vcl_vector<double> get_bdry_plus_outside_shock_radius()
  {
      return bdry_plus_outer_shock_radius_;
  }

  // get outside shock radius
  vcl_vector<double>  get_bdry_minus_outside_shock_radius()
  {
      return bdry_minus_outer_shock_radius_;
  }


protected:
  //: This function takes a sampled shock curve and interpolates it.
  void interpolate( int num_points,
                    vcl_vector<vgl_point_2d<double> > &sh_pt, 
                    vcl_vector<double> &time,
                    vcl_vector<double> &theta,
                    vcl_vector<double> &phi,
                    vcl_vector<int> &lmap);

  //: This function subsamples the shock curve to evenly distribute them
  // and also to speed up shock curve matching.
  void subsample();

  void reconstruct_boundary();
  void compute_arclengths();
  void compute_areas();

public:
  // return the data points
  int num_points() { return num_points_; };

  vgl_point_2d<double> sh_pt(int index) { return sh_pt_[index]; }
  double time(int index) { return time_[index]; }
  double theta(int index) { return theta_[index]; }
  double phi(int index) { return phi_[index]; }

  vgl_point_2d<double> bdry_plus_pt(int index) { return bdry_plus_[index]; }
  vgl_point_2d<double> bdry_minus_pt(int index) { return bdry_minus_[index]; }

  vcl_vector< vgl_point_2d<double> >& bdry_plus() { return bdry_plus_; }
  vcl_vector< vgl_point_2d<double> >& bdry_minus() { return bdry_minus_; }

  double sh_pt_x(int index) { return sh_pt_[index].x(); };
  double sh_pt_y(int index) { return sh_pt_[index].y(); };

  double arclength(int index){ return arclength_[index];};

  double boundary_plus_x(int index) { return bdry_plus_[index].x(); };
  double boundary_plus_y(int index) { return bdry_plus_[index].y(); };
  double boundary_minus_x(int index) { return bdry_minus_[index].x(); };
  double boundary_minus_y(int index) { return bdry_minus_[index].y(); };

  double boundary_plus_arclength(int index) { return bdry_plus_arclength_[index]; };
  double boundary_minus_arclength(int index) { return bdry_minus_arclength_[index]; };
  double boundary_plus_angle(int index) { return bdry_plus_angle_[index]; };
  double boundary_minus_angle(int index) { return bdry_minus_angle_[index]; };

  double boundary_plus_length() {return bdry_plus_length_; };
  double boundary_minus_length() {return bdry_minus_length_; };

  //Debug functions
  void writeData(vcl_string fname);

  //: return an extrinsic point corresponding to the intrinsic 
  //  fragment coordinates(s(i),t) { i.e., (s(i),t)->(x,y) }
  //  radius>0 for plus side and <0 for minus side
  vgl_point_2d<double> fragment_pt(int index, double radius);

  //: return the radius at an interpolated point
  double interp_radius(int i1, int i2, int N, int n);

  //: return an extrinsic point corresponding to the intrinsic 
  //  fragment coordinates(s,t) { i.e., (s,t)->(x,y) }
  //  [radius>0 for plus side and <0 for minus side]
  //  Note: the shock edge is linearly interpolated to n/N of the chosen interval
  vgl_point_2d<double> fragment_pt(int index1, int index2, int N, int n, double radius);

  //: return the radius at an interpolated point defined by the continuous index
  double interp_radius(double index);

  //: return an extrinsic point corresponding to the intrinsic 
  //  fragment coordinates(s(i),t) { i.e., (s(i),t)->(x,y) }
  //  radius>0 for plus side and <0 for minus side defined in terms
  //  of a continuous index
  vgl_point_2d<double> fragment_pt(double index, double radius);

  // ---------------------------------------
  // Functions to compute costs for dpmatch
  //----------------------------------------

  void stretch_cost(int i, int ip, vcl_vector<double> &a);
  void bend_cost(int i, int ip, vcl_vector<double> &a);
  void area_cost(int i, int ip, double& dA);
  void outer_shock_cost(int i,int ip,vcl_vector<double> &a);

  void stretch_cost_combined(int i, int ip, vcl_vector<double> &a);
  void bend_cost_combined(int i, int ip, vcl_vector<double> &a);

  //: ozge put these functions here 
  //  if this shock curve ever becomes a "link" (not a branch) in the tree structure built on top
  //  of a shock graph, then that tree computes and caches the costs of contracting and splicing
  //  this link by calling these functions
  double contract_cost();
  double splice_cost(double R_const, bool new_def, bool construct_circular_ends, bool dpmatch_combined, bool leaf_dart);
  double splice_cost_new(double R_const, bool construct_circular_ends, bool dpmatch_combined);
  
  // construct the shock curves involved in splicing operations
  dbskr_scurve_sptr get_original_scurve(bool construct_circular_ends);
  dbskr_scurve_sptr get_replacement_scurve(int num_pts);
  void set_euler_spiral_completion_length();

  //: for visualization purposes
  void get_polys(vcl_vector<vsol_polygon_2d_sptr>& polys);

protected:

  double interpolate_ds_; ///< parameter for interpolating
  double subsample_ds_;   ///< parameter for subsampling

  //----------------------------------------
  // Shock curve Data
  //----------------------------------------

  int num_points_;                          ///< number of data points along the shock curve
  
  // Data along the shock branch.
  vcl_vector<vgl_point_2d<double> > sh_pt_; ///< point along the shock edge
  vcl_vector<double> arclength_;            ///< arclength along shock branch
  vcl_vector<double> time_;                 ///< radius of the maximal circle
  vcl_vector<double> theta_;                ///< tangent vector to the shock curve
  vcl_vector<double> phi_;                  ///< Angle between the tangent to the shock and the 
                                            //   vector from shock to the bndry point.
  // Data for boundaries
  // +
  vcl_vector< vgl_point_2d<double> > bdry_plus_; ///< corresponding bndry pt on the + side 
  double bdry_plus_length_;                      ///< total length of the + bndry curve
  vcl_vector<double> bdry_plus_arclength_;       ///< arclength along the + bndry curve
  vcl_vector<double> bdry_plus_angle_;           ///< tangent to the + bndry curve
  vcl_vector<double> bdry_plus_outer_shock_radius_; ///< outer shock radius

  // -
  vcl_vector< vgl_point_2d<double> > bdry_minus_; ///< corresponding bndry pt on the - side 
  double bdry_minus_length_;                      ///< total length of the - bndry curve
  vcl_vector<double> bdry_minus_arclength_;       ///< arclength along the - bndry curve
  vcl_vector<double> bdry_minus_angle_;           ///< tangent to the - bndry curve
  vcl_vector<double> bdry_minus_outer_shock_radius_; ///< outer shock radius

  vcl_vector<double> area_; // area of intervals as a function of length
  double total_area_; // total area of this scurve shock branch

  double virtual_length_; // If set use this

  double area_factor_;
};

#endif  // _dbskr_scurve_h
