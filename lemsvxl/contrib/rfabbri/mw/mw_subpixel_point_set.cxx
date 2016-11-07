#include "mw_subpixel_point_set.h"
#include "mw_util.h"
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_2d.h>

//: this class is the one who unallocs the vectors of points
// TODO: support open curves; add open/closed bool vector to input
mw_subpixel_point_set::
mw_subpixel_point_set(const vcl_vector<vsol_point_2d_sptr> &pts)
   :
   pt_(pts),
   nrows_(0),
   ncols_(0),
   nrad_(10),
   NNN(1)
{
}

//: returns the index of the closest point to a given point, together with the
// distance value
bool mw_subpixel_point_set::
nearest_point(double x, double y,  unsigned *np, double *min_distance) const
{
   if (npts() == 0) {
      vcl_cerr << "Hey, no points!" << vcl_endl;
      assert(npts() > 0); //:< simply abort for now
      return false;
   }

   double mindist = d_sqr(x,y,pt_[0]->x(),pt_[0]->y());
   unsigned argmin=0;
   for (unsigned i=1; i<npts(); ++i) {
      double cdist = d_sqr(x,y,pt_[i]->x(),pt_[i]->y());
      if ( cdist < mindist )
         mindist = cdist;
   }

   *min_distance = mindist;
   *np = argmin;

   return true;
}



void mw_subpixel_point_set::
build_bucketing_grid(unsigned nr, unsigned nc)
{
  nrows_ = nr; 
  ncols_ = nc; 

  vcl_cout << "Building bucketing grid..." ;

  cells_.clear();

  //build a ncols x nrows cell grid
  cells_.resize(nrows_);
  for (unsigned ii=0;ii<nrows_;ii++) {
      cells_[ii].resize(ncols_);
  }

  for (unsigned ii=0;ii<nrows_;ii++) {
    for (unsigned jj=0; jj<ncols_; jj++)
      cells_[ii][jj].clear();
  }

  //put the points into buckets
  for (unsigned id=0; id < pt_.size(); ++id) 
  {
    //get the cell coordinates for this pint
    unsigned gx = (unsigned)(pt_[id]->x()/NNN);
    unsigned gy = (unsigned)(pt_[id]->y()/NNN);

    //if the pt is within the grid
    //assign it to the right grid point
    if (gx<ncols_ && gy<nrows_)
      cells_[gy][gx].push_back(id);
  }

  vcl_cout << "done!" << vcl_endl;
}


//: \return false if no point in nhood
bool mw_subpixel_point_set::
nearest_point_by_bucketing(double x, double y,  unsigned *np, double *min_distance) const
{
  // get bucket of x,y
  unsigned p_j = col(x);
  unsigned p_i = row(y);
  
  // loop in nhood

  double dmin = vcl_numeric_limits<double>::infinity();
  unsigned jcell_min = 0, icell_min = 0;
  unsigned kmin = 0;

  long aux = (long int)p_i - (long int)nrad_;
  unsigned i_range_min = ( 0 > aux ) ? 0 : aux;

  aux = (long int)p_i + (long int)nrad_;
  unsigned i_range_max = (nrows()-1 < aux) ? nrows()-1 : aux;

  aux = (long int)p_j - (long int)nrad_;
  unsigned j_range_min = ( 0 > aux ) ? 0 : aux;

  aux = (long int)p_j + (long int)nrad_;
  unsigned j_range_max = (ncols()-1 < aux) ? ncols()-1 : aux;

  for (unsigned icell = i_range_min; icell <= i_range_max; ++icell) {
    for (unsigned jcell = j_range_min; jcell <= j_range_max; ++jcell) {
        for (unsigned i=0; i < cells_[icell][jcell].size(); ++i) {
          const vsol_point_2d_sptr &pt = pt_[ cells_[icell][jcell][i] ];
          double dx = pt->x() - x; 
          double dy = pt->y() - y; 
          double d  = dx*dx+dy*dy;
          if (d < dmin) {
            kmin = i;
            dmin = d;
            jcell_min = jcell;
            icell_min = icell;
          }
        }
    }
  }

  *min_distance = dmin;

  if (dmin == vcl_numeric_limits<double>::infinity())
    return false;

  *np = cells_[icell_min][jcell_min][kmin];


//  vgl_point_2d<double> pt = sel_->cells()[icell_min][jcell_min][imin]->pt;
  return true;
}
