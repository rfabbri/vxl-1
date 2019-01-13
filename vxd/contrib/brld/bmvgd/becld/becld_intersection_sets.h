// This is becld_intersection_sets.h
#ifndef becld_intersection_sets_h
#define becld_intersection_sets_h
//:
//\file
//\brief Intersection sets of epipolar pencils
//\author Ricardo Fabbri (rfabbri), Brown University  (@gmail.com)
//\date 11/21/2005 11:05:03 PM EST
//

#include <vector>
#include <list>

#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_line_2d.h>
#include <vgl/vgl_distance.h>

#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_point_2d.h>

//:
// becld_intersection_sets is;
//
// is[i][j] = intersection neighborhood at curve i with epipolar j
//
// \todo
//  - later: function enlarge_intersection_sets (given an epipolar line and its
//  number within the ordering)
class becld_intersection_sets {
  public:

  becld_intersection_sets();

  void compute(
      const std::vector<vsol_polyline_2d_sptr> &c, //:< list of curves
      const std::vector<vgl_homg_line_2d<double> > &ep //:< list of epipolars
      );

  ~becld_intersection_sets() {}

  void nhoods_intercepting_epipolar(
      unsigned ep_i, std::vector< std::list<std::vector<unsigned> > > &Lst) const;

  void points_intercepting_epipolar(
      unsigned ep_i, std::vector<std::list<unsigned> > &pts_idx) const;

  //: returns all points of all intersections. Some pts may be repeated due to nhood size.
  // Point coordinates are given by {curve index, point index within curve}
  // pt_idx[i] = vector of all intersection points of curve i
  void all_points(std::vector<std::vector<unsigned> > &pts_idx) const;

  //: same as all_points but remove dups
  //  void all_unique_points(std::vector<unsigned> &pts_idx) const;

  //: return total number of intersection points (note some points may be repeated, due to nhood
  // size)
  //
  //  void count_points() const;
  //  void count_unique_points() const;

  //: returns net number of intersection pts of curve i
  unsigned long npoints(unsigned i) const; 

  void print() const;

  unsigned ncurves() const { return L_.size(); }
  unsigned n_intersecting_curves() const;

  static inline bool
  mw_line_polyline_intersection_1(
      const vgl_homg_line_2d<double> *l, 
      const vsol_polyline_2d_sptr crv, 
      std::vector<bool> &indices, 
      double maxdist=1.4);

  public:
    //: connected ordered set of points; a neighborhood around ideal intersection
    typedef struct intersection_nhood_ {
      unsigned ep_number; //:< pointer of epipolar line intersecting this hypothesis
      std::vector<unsigned> index; //:< index[i] == index on the curve of the point
      // we also cache the points (take this off if too heavy)
      // std::vector<vgl_point_2d<unsigned>> pts;
    } intersection_nhood_;

    //: list of intersections for a curve
    typedef struct curve_intersections_list_{
      std::list<intersection_nhood_> intercepts;
    } curve_intersections_list_;

    void print(const curve_intersections_list_ & c) const;

    //: L_[i] == list of intersections of ith curve
    // L_[i].index
    // L_[i].index
    std::vector<curve_intersections_list_> L_;
};


//--------------------------------------------------------------------------------

//: Computes intersecting interval of a line with a polyline whose vertices are
// lying on the grid.
//
// \param[out] indices: indices[i] = true for the points i (in crv) that are sufficiently
// close to l to be considered a "fuzzy" intersection of l and crv.
inline bool
becld_intersection_sets::
mw_line_polyline_intersection_1(
    const vgl_homg_line_2d<double> *l, 
    const vsol_polyline_2d_sptr crv, 
    std::vector<bool> &indices, 
    double maxdist)
{
// TODO
//  - refine the criterion
//    - currently, an intersection is simply a point whose distance to the line
//    is less than sqrt(2).

  indices.clear();
  indices.resize(crv->size(),false);
  bool has_intersection=false;
  for (unsigned i=0; i<crv->size(); ++i) {
    const vsol_point_2d_sptr pt = crv->vertex(i);
    vgl_homg_point_2d<double> homg_pt(pt->x(),pt->y());
    if (vgl_distance(*l,homg_pt) <= maxdist) {
      indices[i]=true;
      has_intersection=true;
    }
  }

  return has_intersection;
}


#endif // becld_intersection_sets_h
