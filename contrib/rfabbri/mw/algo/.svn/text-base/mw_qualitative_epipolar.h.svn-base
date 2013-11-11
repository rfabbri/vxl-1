// This is mw_point_matcher.h
#ifndef mw_qualitative_epipolar_h
#define mw_qualitative_epipolar_h
//:
//\file
//\brief Code to limit epipolar geometry from point correspondences
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date Thu Nov  2 23:55:07 EST 2006
//
#include <vcl_vector.h>

#include <dbecl/dbecl_epiband.h>
#include <mw/mw_discrete_corresp_n.h>
#include <vsol/vsol_box_2d_sptr.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vcl_list.h>

//: This class computes feasible regions in two image domains where two epipoles
// can lie, given corresponding points and no information about the cameras or
// the epipolar geometry. These feasible regions are build using a constraint on
// the ordering of corresponding epipolar lines.
//
class mw_qualitative_epipolar {
  public:
  
  mw_qualitative_epipolar(
      const vcl_vector<vsol_point_2d_sptr> &p0,
      const vcl_vector<vsol_point_2d_sptr> &p1,
      const vgl_box_2d<double> &bbox);

  bool compute_epipolar_region();

  // Getters and Setters

  const vcl_list <vgl_polygon<double> > &polys0() const
  { assert(have_computed_); return polys0_; }

  const vcl_list <vgl_polygon<double> > &polys1() const
  { assert(have_computed_); return polys1_; }

  void print_polygons() const;

  // Static functions 
  static void 
  build_sectors(const vcl_vector<vsol_point_2d_sptr> &p, 
              unsigned i,
              vcl_vector <double> &angle,
              vcl_vector <bool> &outward);

  static void 
  partition(const vcl_vector<vsol_point_2d_sptr> &p,
            unsigned p_i,
            double angle,
            bool outward,
            vcl_set <unsigned> &left,
            vcl_set <unsigned> &right);

  static void 
  get_sector_bounds(
      const vcl_vector<double> &angle, 
      unsigned is, 
      double &theta_start,
      double &theta_end
      );

 static vgl_polygon<double> 
  intersect_sectors(
      const vgl_polygon<double> &poly_a, 
      unsigned ip, 
      unsigned is, 
      const vcl_vector<vsol_point_2d_sptr> & p, 
      const vcl_vector<double> &angle, 
      const vcl_vector <bool>  &outward,
      const vgl_box_2d<double> &bbox,
      int *p_retval
      );

  static vgl_polygon<double> 
  form_sector_polygon(
      unsigned ip, 
      unsigned is, 
      const vcl_vector<vsol_point_2d_sptr> & p, 
      const vcl_vector<double> &angle, 
      const vcl_vector <bool>  &outward,
      const vgl_box_2d<double> &bbox
      );

  bool valid_sector(unsigned i, unsigned k0, unsigned &k1) const;

private:
  const vcl_vector<vsol_point_2d_sptr> &p0_;
  const vcl_vector<vsol_point_2d_sptr> &p1_;
  const vgl_box_2d<double> &box_;
  bool have_computed_;
  unsigned npts_;

  vcl_vector< vcl_vector<double> > angles0_;
  vcl_vector< vcl_vector<double> > angles1_;
  vcl_vector< vcl_vector<bool> > outwards0_;
  vcl_vector< vcl_vector<bool> > outwards1_;

  //: Lists of corresponding polygons
  vcl_list <vgl_polygon<double> > polys0_;
  vcl_list <vgl_polygon<double> > polys1_;

  bool
  try_combination(
      unsigned selected, 
      vcl_vector<unsigned> &selected_sectors, 
      const vgl_polygon<double> &isec_old0,
      const vgl_polygon<double> &isec_old1
      );
};


#endif // mw_qualitative_epipolar_h
