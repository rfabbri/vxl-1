#include "mw_curves.h"
#include "mw_util.h"
#include <vgl/vgl_distance.h>
#include <vgl/vgl_point_2d.h>





//: this class is the one who unallocs the vectors of points
// TODO: support open curves; add open/closed bool vector to input
mw_curves::
mw_curves(vcl_vector< vcl_vector<vsol_point_2d_sptr> > *curvepts)
   :
   curvepts_(*curvepts),
   gcurve_(curvepts->size(),0)
{
   // count number of points
   unsigned long npts = 0;
   for (unsigned i=0; i<curvepts_.size(); ++i) {
      npts += curvepts_[i].size();
   }

   pt_.reserve(npts);
   curveid_.reserve(npts);
   for (unsigned i=0; i<curvepts_.size(); ++i) {
      for (unsigned j=0; j<curvepts_[i].size(); ++j) {
         curveid_.push_back(i);
         pt_.push_back(curvepts_[i][j]);
      }
   }
   assert(check_consistency());

}

mw_curves::
~mw_curves()
{
   delete &curvepts_;
}

bool mw_curves::
check_consistency() const
{
   bool consistent;

   consistent = (pt_.size() == curveid_.size());

   return consistent;
}

//: todo: check_consistency_thorough

//: output to stream
vcl_ostream&  operator<<(vcl_ostream& s, const mw_curves &c)
{
   // dump everything!
   s << "Pt\n" << "size: " << c.pt_.size() << vcl_endl;
   for (unsigned long i=0; i < ((c.pt_.size() < 4)?c.pt_.size():4); ++i) {
//   for (unsigned long i=0; i < c.pt_.size(); ++i) {
      s << i << ":\t" << *(c.pt_[i]);
   }
   vcl_cout << "  last: " << *(c.pt_[c.pt_.size()-1]);

   s << "Curveid\n" << "size: " << c.curveid_.size() << vcl_endl;
   for (unsigned long i=0; i < ((c.curveid_.size() < 4)?c.curveid_.size():4); ++i) {
      s << i << ":\t" << c.curveid_[i] << vcl_endl;
   }

   s << "gcurve_\n" << "ncurves: " << c.gcurve_.size() << vcl_endl;

   return s;
}

void mw_curves::
compute_geno()
{
   for (unsigned i=0; i<curvepts_.size(); ++i) {
      // GENO INTERP.
      gcurve_[i] = new dbsol_gshock_curve_2d;
      dbsol_geno::interpolate(gcurve_[i].ptr(), curvepts_[i], false); // open curve for now
      // no shock placing for now; but we may place them whenever we want
   }

   assert(check_consistency());
}

//: returns the index of the closest point to a given point, together with the
// distance value
void mw_curves::
nearest_point_to(double x, double y,  unsigned long *np, double *min_distance)
{
   if (npts() == 0) {
      vcl_cerr << "Hey, no points!" << vcl_endl;
      return;
   }

   double mindist = d_sqr(x,y,pt_[0]->x(),pt_[0]->y());
   unsigned long argmin=0;
   for (unsigned long i=1; i<npts(); ++i) {
      double cdist = d_sqr(x,y,pt_[i]->x(),pt_[i]->y());
      if ( cdist < mindist )
         mindist = cdist;
   }

   *min_distance = mindist;
   *np = argmin;
}
