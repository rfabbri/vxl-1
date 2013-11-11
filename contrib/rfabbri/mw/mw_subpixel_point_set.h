// This is mw_subpixel_point_set.h
#ifndef mw_subpixel_point_set_h
#define mw_subpixel_point_set_h
//:
//\file
//\brief class for efficient query of points within an image domain
//\author Ricardo Fabbri (rfabbri), Brown University  (rfabbri@lems.brown.edu)
//\date Wed Apr 26 22:12:55 EDT 2006
//

#include <vcl_vector.h>

#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

//: 
// The main purpose is to deal with efficient traversal and algorithms by
// representing the subpixel points in special structures (e.g. grid buckets).
//
// \remarks THIS CLASS IS UNDER DEVELOPMENT. Currently it uses straightforward
// bruteforce and no bucketing
// - check bsol_point_index_2d
//
// \todo Use of smart pointers may be too inefficient
//
// \todo Use of a quadtree structure and the "Overlap Search Query" to speedup 
// queries e.g. of points inside a given polygon.
// 
class mw_subpixel_point_set {
public:

  mw_subpixel_point_set(const vcl_vector<vsol_point_2d_sptr> &pts);

  ~mw_subpixel_point_set() {};

  //: brute force; no bucketting used and search is done in whole image
  bool nearest_point(double x, double y,  unsigned *np, double *min_distance) const;

  //: build a bucketing grid for neighborhood determination
  void build_bucketing_grid(unsigned nrows, unsigned ncols);

  //: nhood within which searches are made
  void set_nhood_radius(unsigned nrad) {nrad_ = nrad;}

  //: Nearest point within a maximum neighborhood radius bounding the search
  bool nearest_point_by_bucketing(double x, double y,  unsigned *np, double *min_distance) const;

  unsigned col(vsol_point_2d_sptr &pt) const {return (unsigned) (pt->x()/NNN);}
  unsigned row(vsol_point_2d_sptr &pt) const {return (unsigned) (pt->y()/NNN);}

  unsigned col(double x) const {return (unsigned) (x/NNN);}
  unsigned row(double y) const {return (unsigned) (y/NNN);}

  bool valid(unsigned i_col,unsigned i_row)  const { return i_col < ncols() && i_row < nrows(); }

  //: points represented by index into input point set vector.
  //cells[row][col][point_idx]
  const vcl_vector< vcl_vector< vcl_vector<unsigned> > > & cells() const { return cells_; }

  unsigned npts()  const { return pt_.size(); }
  unsigned nrows() const { return nrows_; }
  unsigned ncols() const { return ncols_; }
  unsigned nrad()  const { return nrad_; }

  bool is_bucketed() const { return cells_.size() != 0; }

protected:
  const vcl_vector<vsol_point_2d_sptr> &pt_;
  vcl_vector< vcl_vector< vcl_vector<unsigned> > > cells_;
  unsigned nrows_;     //:< size of the bucketing grid
  unsigned ncols_;     //:< size of the bucketing grid
  unsigned nrad_;      //:< radius of neighborhood for searches
  unsigned const NNN; //:< may be used in future if coarser grid is desired
};


#endif // mw_subpixel_point_set_h
