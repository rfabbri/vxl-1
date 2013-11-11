//---------------------------------------------------------------------
// This is brcv/rec/dbru/algo/dbru_rcor_generator.h
//:
// \file
// \brief Class that contains methods to find correspondence between the pixels of polygonal input regions,
//        based on given correspondence of samples on the contour.
//
// \author
//  O.C. Ozcanli - Nov 28, 2005
//
// \verbatim
//   
// \endverbatim
//
//-------------------------------------------------------------------------


// If a pixel is within the polygonal region, it will be assumed to be valid and
// a correspondence will be found. The users of this class should assure that
// these pixels are in valid regions of the images they are imposed upon.
// (i.e. they should put checks like x <= w && y <= height, etc. for pixel (x, y) of regions)

#ifndef _dbru_rcor_generator_h
#define _dbru_rcor_generator_h

#include <vcl_vector.h>
#include <vcl_utility.h>
#include <assert.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_norm_trans_2d.h>

#include <dbsol/dbsol_interp_curve_2d_sptr.h>
#include <dbru/dbru_rcor_sptr.h>

#include <dbcvr/dbcvr_cv_cor_sptr.h>
#include <dbskr/dbskr_sm_cor_sptr.h>
#include <dbskr/dbskr_scurve_sptr.h>

#include <mbl/mbl_thin_plate_spline_2d.h>

//: This class supplies methods to compute 
//  region pixel correspondences from curve alignment and shock alignment.
//  dbru_rcor class instance should be constructed and its smart pointer
//  should be passed to each method
//  so that correspondence data structures are filled in by this generator
class dbru_rcor_generator
{
protected:
  //: Constructor 
  dbru_rcor_generator() {}

public:
  //: Destructor
  virtual ~dbru_rcor_generator() {};

  //-------------------------------------------------------------------
  // Methods to compute region correspondence
  //-------------------------------------------------------------------

  
  //: find region correspondence based on intersections of lines
  static bool find_correspondence_line(dbru_rcor_sptr cor,
                           unsigned int i_increment = 1);

  //: find region correspondence based on intersections of lines
  static bool find_correspondence_line(dbru_rcor_sptr cor, dbcvr_cv_cor_sptr sil_cor, 
                                 unsigned int i_increment = 1);

  //: compute line intersections between line segments (pt1->pt2) and (pt3->pt4)
  static bool find_intersection(double x1, double y1, double x2, double y2,
                         double x3, double y3, double x4, double y4,
                         vgl_point_2d<int> *out_pt, vgl_point_2d<float> *out_pt_float = 0);

  //: this method uses the same algorithm in find_correspondence_line, 
  //  however it uses arclength to select quads
  static bool find_correspondence_line2(dbru_rcor_sptr cor, 
                                        double ratio = 0.1);
  static bool find_correspondence_line2(dbru_rcor_sptr cor, dbcvr_cv_cor_sptr sil_cor, 
                                        double ratio = 0.1);

  //: this method uses the same algorithm in find_correspondence_line, 
  //  however it distributes increment on the selection of both starting and ending points of 
  //  both line segment
  static bool find_correspondence_line3(dbru_rcor_sptr cor, 
                                        int increment = 1);
  static bool find_correspondence_line3(dbru_rcor_sptr cor, dbcvr_cv_cor_sptr sil_cor, 
                                        int increment = 1);

  //: this method goes from pixels to the contour via pairs of line segments
  // that intersect on that pixel, 
  // it rotates the two line segments by delta_theta each time to get a new quad
  // delta_theta is determined by the total number of votes 
  static bool find_correspondence_line4(dbru_rcor_sptr cor, 
                                        int total_vote);
  static bool find_correspondence_line4(dbru_rcor_sptr cor, dbcvr_cv_cor_sptr sil_cor, 
                                        int total_vote);

  //: find region correspondence based on distance transform 
  static bool find_correspondence_dt(dbru_rcor_sptr cor,
                              dbsol_interp_curve_2d_sptr curve1,
                              dbsol_interp_curve_2d_sptr curve2,
                              float scale);

  //: find region correspondence based on distance transform 
  static bool find_correspondence_dt(dbru_rcor_sptr cor, dbcvr_cv_cor_sptr sil_cor,
                                     dbsol_interp_curve_2d_sptr curve1,
                                     dbsol_interp_curve_2d_sptr curve2,
                                     float scale);

  
  static bool find_correspondence_dt2(dbru_rcor_sptr cor, dbcvr_cv_cor_sptr sil_cor,
                                      dbsol_interp_curve_2d_sptr curve1,
                                      dbsol_interp_curve_2d_sptr curve2,
                                      float scale, float ratio);

  //: find region correspondence based on distance transform 
  //  VERSION 2
  //  This version is symetric and prevents overwrite problem of previous version
  //  via arclength test
  //  Additional steps:
  //  1) compute dt of second region
  //  2) given p in region1, find (s,d) on curve1 (s is arclength of nearest point on contour and its distance d), 
  //     get corresponding s2 = alpha(s) on curve2
  //     get p2 on region2 which is point_at(s2)+Normal_at(s2)  (go along normal direction into region2 from contour point)
  //     get (s22, d22) from distance transform of region 2 for p2
  //     test if s22 ~ s2 (i.e. |s22-s2|< epsilon, where epsilon = ratio*length of curve2)
  //     if not then discard this correspondence
  //  This test also prevents tangent problem to some extent
  //  tangent problem: tangent direction hence normal direction is not
  //  defined uniquely at exact sample points of the polygon
  //  3) Also find correspondences going from region2 into region1 and add to the correspondence set
  static bool find_correspondence_dt2(dbru_rcor_sptr cor,
                                      dbsol_interp_curve_2d_sptr curve1,
                                      dbsol_interp_curve_2d_sptr curve2,
                                      float scale, float ratio);

  //: return the distance to the nearest polygon edge, together with arclength 
  //  of that nearest point on the polygon
  static vcl_pair<double, double> dt(vgl_polygon<double> const& poly, double x, double y);

  //: find correspondence of image regions based on correspondence of shock branches
  static bool find_correspondence_shock(dbru_rcor_sptr cor);

  //: find correspondence of image regions based on correspondence of shock branches
  static bool find_correspondence_shock(dbru_rcor_sptr cor, dbskr_sm_cor_sptr sm_cor);

  //: find correspondence of image regions based on LINEAR corrospondence of shock curves
  static bool find_correspondence_shock(dbru_rcor_sptr cor, dbskr_scurve_sptr sc1, dbskr_scurve_sptr sc2);

  static void add_to_map(dbru_rcor_sptr cor,
                  vgl_point_2d<double> r1_pt_p, vgl_point_2d<double> r2_pt_p, 
                  vgl_point_2d<double> r1_pt_m, vgl_point_2d<double> r2_pt_m);

  //: find region correspondence based on TPS
  static bool find_correspondence_tps(dbru_rcor_sptr cor, 
                                      vcl_vector< vgl_homg_point_2d<double> >& hpts1, 
                                      vcl_vector< vgl_homg_point_2d<double> >& hpts2, 
                                      mbl_thin_plate_spline_2d& ftps, 
                                      vgl_norm_trans_2d<double>& trans1, 
                                      vgl_norm_trans_2d<double>& trans2,
                                      bool compute_energy = false);

  //: find region correspondence based on TPS
  static bool find_correspondence_tps(dbru_rcor_sptr cor, 
                                      mbl_thin_plate_spline_2d& ftps, 
                                      vgl_norm_trans_2d<double>& trans1, 
                                      vgl_norm_trans_2d<double>& trans2);

  //: find region correspondence getting correspondence from elastic curve matching
  static bool find_correspondence_tps_curve(dbru_rcor_sptr cor, 
                                            mbl_thin_plate_spline_2d& tps, 
                                            vgl_norm_trans_2d<double>& trans1, 
                                            vgl_norm_trans_2d<double>& trans2,
                                            int increment = 1,
                                            bool compute_energy = false); 

};

#endif // _dbru_rcor_generator_h
