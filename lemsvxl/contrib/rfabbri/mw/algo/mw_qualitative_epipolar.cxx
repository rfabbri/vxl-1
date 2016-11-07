#include "mw_qualitative_epipolar.h"

#include <vcl_iomanip.h>
#include <mw/mw_util.h>
#include <vsol/vsol_box_2d.h>
#include <vgl/vgl_distance.h>
#include <vcl_algorithm.h>
#include <vgl/vgl_clip.h>
#include <vgl/io/vgl_io_polygon.h>


mw_qualitative_epipolar::
mw_qualitative_epipolar(
      const vcl_vector<vsol_point_2d_sptr> &p0,
      const vcl_vector<vsol_point_2d_sptr> &p1,
      const vgl_box_2d<double> &bbox)
: p0_(p0),p1_(p1),box_(bbox), have_computed_(false)
{
  assert(p0.size() == p1.size());
  assert(p0.size() > 2);
  npts_ = p0.size();

  angles0_.resize(npts_);
  angles1_.resize(npts_);
  outwards0_.resize(npts_);
  outwards1_.resize(npts_);

  // build sectors around every point and cache them
  for (unsigned i=0; i < npts_; ++i) {
    build_sectors(p0_, i, angles0_[i], outwards0_[i]);
    build_sectors(p1_, i, angles1_[i], outwards1_[i]);
    assert(angles0_[i].size() == angles1_[i].size() && outwards0_[i].size() == outwards1_[i].size() && outwards0_[i].size() == angles0_[i].size() && angles0_[i].size() == npts_-1);
  }
}

//: Compute regions within box_ where the epipole might be, given only the "set
// partition constraint" that epipolar geometry must satisfy
bool mw_qualitative_epipolar::
compute_epipolar_region()
{
  have_computed_ = true;
  vgl_polygon<double> isec0(1);
  isec0.push_back(box_.min_x(),box_.min_y());
  isec0.push_back(box_.max_x(),box_.min_y());
  isec0.push_back(box_.max_x(),box_.max_y());
  isec0.push_back(box_.min_x(),box_.max_y());

  vgl_polygon<double> isec1(1);
  isec1.push_back(box_.min_x(),box_.min_y());
  isec1.push_back(box_.max_x(),box_.min_y());
  isec1.push_back(box_.max_x(),box_.max_y());
  isec1.push_back(box_.min_x(),box_.max_y());

  polys0_.clear();
  polys1_.clear();

  unsigned selected = (unsigned)-1;
  vcl_vector<unsigned> selected_sectors(npts_,(unsigned)-1);

  return try_combination(selected, selected_sectors, isec0, isec1);
}

//: The core recursive method to search for consistent intersecting sectors 
// \return false if errors in computation have occurred, e.g. degeneracies
bool mw_qualitative_epipolar::
try_combination(
    unsigned selected, 
    vcl_vector<unsigned> &selected_sectors, 
    const vgl_polygon<double> &isec_old0,
    const vgl_polygon<double> &isec_old1
    )
{
    vgl_polygon<double> isec0;
    vgl_polygon<double> isec1;

    unsigned i0 = selected + 1;
    assert(i0 < angles1_.size());

    // for all sectors around p0_[i0]
    for (unsigned s0 = 0; s0 < angles1_[i0].size(); ++s0) {
      unsigned s1;
      if (!valid_sector(i0,s0,s1))
        continue;

      int retval0,retval1;
      isec0 = intersect_sectors(isec_old0,i0,s0,p0_,angles0_[i0],outwards0_[i0],box_,&retval0);
      isec1 = intersect_sectors(isec_old1,i0,s1,p1_,angles1_[i0],outwards1_[i0],box_,&retval1);
      if (retval0 == 0 || retval1 == 0) {
        return false;
//        vcl_cout << "Degeneracy encountered -- some valid epipolar regions might be ignored\n";
//        continue;
      }

      if (isec0.num_sheets() == 0 || isec1.num_sheets() == 0)
        continue;

          /*
      if (mw_util::near_zero(vgl_area<double>(isec0),1e-7)) {
        if (mw_util::near_zero(vgl_area<double>(isec1),1e-7)) {
//          vcl_cout << "One combination has failed\n";
//          vcl_cout << "  Points: 0..." << i0;
//          vcl_cout << "  Sectors: " ;
//          for (unsigned kk=0; kk <= selected; ++kk) {
//            vcl_cout << selected_sectors[kk] << " ";
//          }
//          vcl_cout << s0;
//          vcl_cout << vcl_endl;
        } else {
          vcl_cout << "\n\nIsec 0 nearly empty but in image[1] it is non-empty!\n";
          vcl_cout << "\tarea(isec0): " << vgl_area<double>(isec0) << vcl_endl 
                   << "\t" << isec0 << vcl_endl;
          vcl_cout << "\tarea(isec1): " <<vgl_area<double>(isec1) << vcl_endl 
                   << "\t" << isec1 << vcl_endl << vcl_endl;
        }
        continue;
      } else {
        if (mw_util::near_zero(vgl_area<double>(isec1),1e-7)) {
          vcl_cout << "\n\nIsec 1 nearly empty  but in image[0] it is non-empty!\n";
          vcl_cout << "\tarea(isec0): " << vgl_area<double>(isec0) << vcl_endl 
                   << "\t" << isec0 << vcl_endl;
          vcl_cout << "\tarea(isec1): " << vgl_area<double>(isec1) << vcl_endl 
                   << "\t" << isec1 << vcl_endl << vcl_endl;
          assert(!mw_util::near_zero(vgl_area<double>(isec1),1e-7));
        }
      }
      */

      selected = i0;
      selected_sectors[selected] = s0;
      if (selected == p0_.size()-1) {
        polys0_.push_back(isec0);
        polys1_.push_back(isec1);
//        vcl_cout << "Found solution!\n";
//        vcl_cout << "  Points: 0..." << i0;
//        vcl_cout << "  Sectors: " ;
//        for (unsigned kk=0; kk <= i0; ++kk) {
//          vcl_cout << selected_sectors[kk] << " ";
//        }
//        vcl_cout << vcl_endl;
      } else {
        bool final_retval = try_combination(selected,selected_sectors,isec0,isec1);
        if (!final_retval)
          return false;
      }
      selected--;
    }
    return true;
}

//:
// Input: 
//    - p0, p1 point sets
//    - sectors for both point sets
//    - point index i
//    - index k0 of a sector around p0[i]; this index is relative to angles vcl_vector
//
// Output:
//    - returns if sector k0 of point p0[i] is valid or not
//    - what sector k1 of point p1[i] corresponds to sector k0 of p0[i]
bool mw_qualitative_epipolar::
valid_sector(unsigned i, unsigned k0, unsigned &k1) const
{

  // get sector mid angle, and direction if needed

  unsigned n_sectors = angles0_[i].size();

  double theta_start,theta_end;
  get_sector_bounds(angles0_[i],k0,theta_start,theta_end);
  double theta_k0 = (theta_end + theta_start)*0.5;

  
  vcl_set<unsigned> l0, r0;
  mw_qualitative_epipolar::partition(p0_,i,theta_k0,true,l0,r0);

  //: TODO perhaps we can improve the performance of this. This function is currently O(n^2) 
  for (k1=0; k1 < n_sectors; ++k1) {
    get_sector_bounds(angles1_[i],k1,theta_start,theta_end);
    double theta_k1 = (theta_end + theta_start)*0.5;
    vcl_set<unsigned> l1, r1;
    mw_qualitative_epipolar::partition(p1_,i,theta_k1,true,l1,r1);

    // We don't really have to check both sides; one is the complement of the other
//        if ((l0 == l1 && r0 == r1) || (l0 == r1 && r0 == l1))
    //
    if (l0 == l1 || l0 == r1)
      return true;
  }

  return false;
}

void mw_qualitative_epipolar::
get_sector_bounds(
    const vcl_vector<double> &angle, 
    unsigned is, 
    double &theta_start,
    double &theta_end
    )
{
  unsigned last_idx = angle.size()-1;

  if (is < last_idx) {
    theta_start = angle[is];
    theta_end = angle[is+1];
  } else {
    if (is == last_idx) {
      theta_start = angle[is];
      theta_end = angle[0] + vnl_math::pi;
//      vcl_cout << "   Angle 0: " << angle[0]*180.0/vnl_math::pi << vcl_endl;
//      vcl_cout << "   Angle last: " << angle[is]*180.0/vnl_math::pi << vcl_endl;
//      vcl_cout << "   Angle start: " << theta_start << vcl_endl;
//      vcl_cout << "   Angle end: " << theta_end << vcl_endl;
    } else {
      abort();
    }
  }
}

//: intersect polygon poly_a with sector is (indexed into angles_) around p[ip]
vgl_polygon<double> mw_qualitative_epipolar::
intersect_sectors(
    const vgl_polygon<double> &poly_a, 
    unsigned ip, 
    unsigned is, 
    const vcl_vector<vsol_point_2d_sptr> & p, 
    const vcl_vector<double> &angle, 
    const vcl_vector <bool>  &outward,
    const vgl_box_2d<double> &bbox,
    int *p_retval
    )
{
  vgl_polygon<double> poly_b(1);

  // form sector with maxradius defined from bounding box. Sector may be larger than bbox, but that
  // doesn't matter 

  poly_b = form_sector_polygon(ip,is,p,angle,outward,bbox);

  vsl_b_ofstream bfs_out("polygon_bug_test.bvl.tmp");

  assert((!bfs_out) == false);

  vsl_b_write(bfs_out, poly_a);
  vsl_b_write(bfs_out, poly_b);
  bfs_out.close();

//  vcl_cout << vcl_setprecision(20) << vcl_scientific;
//  vcl_cout << "Poly A:\n" << poly_a << vcl_endl;
//  vcl_cout << "Poly B:\n" << poly_b << vcl_endl;

  int retval;
  vgl_polygon<double> result = vgl_clip(poly_a,poly_b, vgl_clip_type_intersect,&retval);
  *p_retval = retval;
  return result;
}

vgl_polygon<double> mw_qualitative_epipolar::
form_sector_polygon(
    unsigned ip, 
    unsigned is, 
    const vcl_vector<vsol_point_2d_sptr> & p, 
    const vcl_vector<double> &angle, 
    const vcl_vector <bool>  &/*outward*/,
    const vgl_box_2d<double> &bbox
    )
{
  mw_vector_2d c(0.5*(bbox.max_x() + bbox.min_x()), 0.5*(bbox.max_y() + bbox.min_y()));

  mw_vector_2d r1(0.5*(bbox.max_x() - bbox.min_x()), 0.5*(bbox.max_y() - bbox.min_y()));
  mw_vector_2d p_c(p[ip]->x(),p[ip]->y());

  //: the 100.0 bellow is whatever quantity > 0
  double rho = (c - p_c).two_norm() +  20*r1.two_norm() + 100.0;

  // The angles

  double theta_start;
  double theta_end;
  get_sector_bounds(angle, is, theta_start,theta_end);

  mw_vector_2d t_start (vcl_cos(theta_start),vcl_sin(theta_start));
  mw_vector_2d t_end (vcl_cos(theta_end),vcl_sin(theta_end));

  mw_vector_2d A = p_c + rho*t_start;
  mw_vector_2d C = p_c - rho*t_start;
  mw_vector_2d B = p_c + rho*t_end;
  mw_vector_2d D = p_c - rho*t_end;

  // we now form the polygon

  vgl_polygon<double> sec(1);

  sec.push_back(p_c[0],p_c[1]);
  sec.push_back(A[0],A[1]);
  sec.push_back(B[0],B[1]);
  sec.push_back(p_c[0],p_c[1]);
  sec.push_back(C[0],C[1]);
  sec.push_back(D[0],D[1]);

  return sec;
}

//: Given a list of points, build the sectors around the i-th point such that no other point is
// within the sectors. 
//
// \param[out] angle: angles around p indicating the lines of the sectors. This vcl_vector is
// ordered counter-clockwise. The angles range in [0,pi) and denote infinite lines around point p[i]
//
// \param[out] outward[i]: true if angle[i] represents line direction from p[i] to
// p[i]+(vcl_cos(angle[i]),vcl_sin(angle[i])), ie, the actual data point is along such direction;
// false if angle[i] represents line direction from p[i] to
// p[i]-(vcl_cos(angle[i]),vcl_sin(angle[i])).
//
void mw_qualitative_epipolar::
build_sectors(const vcl_vector<vsol_point_2d_sptr> &p, 
              unsigned i,
              vcl_vector <double> &angle,
              vcl_vector <bool> &outward)
{
  angle.reserve(p.size()-1);
  outward.resize(p.size()-1);
  for (unsigned k=0; k < p.size(); ++k) {
    if (k != i)  {
      angle.push_back(atan2(p[k]->y() - p[i]->y(),p[k]->x() - p[i]->x()));

      if (angle.back() < 0) {
        angle.back() += vnl_math::pi;
        outward[angle.size()-1] = false;
      } else {
        outward[angle.size()-1] = true;
      }

      // we might have to clip to bounds here

      if (angle.back() >= vnl_math::pi) {
        assert(vcl_fabs(angle.back()-vnl_math::pi) < 1e-10);
        angle.back() = 0;
        outward[angle.size()-1] = false;
      } else
        assert(angle.back() >= 0);
    }
  }

  // Sort
  vcl_vector<vcl_pair<double,bool> > v;
  v.resize(angle.size());

  for (unsigned k=0; k < angle.size(); ++k) {
    v[k].first = angle[k];
    v[k].second = outward[k];
  }
  
  vcl_sort(v.begin(), v.end());

  for (unsigned k=0; k < angle.size(); ++k) {
    angle[k] = v[k].first;
    outward[k] = v[k].second;
  }
}

//: Partition a given point set, given a line around a central point p[i_p].
//
void mw_qualitative_epipolar::
partition(const vcl_vector<vsol_point_2d_sptr> &p,
          unsigned p_i,
          double angle,
          bool outward,
          vcl_set <unsigned> &left,
          vcl_set <unsigned> &right
          )
{
  // for each point, test if its to the left or to the right
  
  mw_vector_2d v_c(vcl_cos(angle),vcl_sin(angle));

  if (!outward) {
    v_c = -v_c;
  }

  // rotate 90 deg
  mw_vector_2d v_c_normal(-v_c[1],v_c[0]);

  for (unsigned i=0; i < p.size(); ++i) {
    if (i != p_i) {
      mw_vector_2d v_i(p[i]->x() - p[p_i]->x(), p[i]->y() - p[p_i]->y());
      if (dot_product(v_i,v_c_normal) > 0) {
        left.insert(i);
      } else {
        right.insert(i);
      }
    }
  }
}

void mw_qualitative_epipolar::
print_polygons() const
{
  vcl_list< vgl_polygon<double> > :: const_iterator itr0=polys0_.begin();
  vcl_list< vgl_polygon<double> > :: const_iterator itr1=polys1_.begin();
  for (; itr0 != polys0_.end(); ++itr0,++itr1) {
    vcl_cout << "Poly View 0: " << *itr0 << vcl_endl 
             << "Poly View 1: " << *itr1 << vcl_endl << vcl_endl;
  }
}
