// This is file shp/dbsksp/dbsksp_xshock_fragment.cxx

//:
// \file

#include "dbsksp_xshock_fragment.h"



#include <vnl/vnl_math.h>
#include <dbgl/algo/dbgl_biarc.h>
#include <dbgl/algo/dbgl_circ_arc.h>
#include <vgl/vgl_area.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_lineseg_test.h>


// =============================================================================
// dbsksp_xshock_fragment
// =============================================================================


// -----------------------------------------------------------------------------
//: Interpolate the left boundary as a biarc
dbgl_biarc dbsksp_xshock_fragment::
bnd_left_as_biarc() const
{
  return dbgl_biarc(this->start_.bnd_pt_left(), this->start_.bnd_tangent_left(), 
    this->end_.bnd_pt_left(), this->end_.bnd_tangent_left());
}


// -----------------------------------------------------------------------------
//: Interploate the right boundary as a biarc
dbgl_biarc dbsksp_xshock_fragment::
bnd_right_as_biarc() const
{
  return dbgl_biarc(this->start_.bnd_pt_right(), this->start_.bnd_tangent_right(), 
    this->end_.bnd_pt_right(), this->end_.bnd_tangent_right());
}





//------------------------------------------------------------------------------
//: Compute area of the shape fragment
double dbsksp_xshock_fragment::
area() const
{
  return this->area_approx_w_polygon(50);
}



//------------------------------------------------------------------------------
//: Return true if this fragment is legal, i.e. so self-intersection
bool dbsksp_xshock_fragment::
is_legal() const
{
  // 
  bool is_legal = true;
  is_legal &= !(this->bnd_has_swallowtail_left());
  is_legal &= !(this->bnd_has_swallowtail_right());

  //// left boundary needs to be a "short biarc"
  //vgl_vector_2d<double > v_left = this->end_.bnd_pt_left() - this->start_.bnd_pt_left();
  //is_legal &= inner_product(v_left, this->start_.bnd_tangent_left()) >= 0;
  //is_legal &= inner_product(v_left, this->end_.bnd_tangent_left()) >= 0;

  //vgl_vector_2d<double > v_right = this->end_.bnd_pt_right() - this->start_.bnd_pt_right();
  //is_legal &= inner_product(v_right, this->start_.bnd_tangent_right()) >= 0;
  //is_legal &= inner_product(v_right, this->end_.bnd_tangent_right()) >= 0;

  return is_legal;

}


//------------------------------------------------------------------------------
//: Return true if the left boundary has swallowtail
bool dbsksp_xshock_fragment::
bnd_has_swallowtail_left() const
{
  // swallowtail occurs when the two contact shocks at two ends of the fragment intersect
  vgl_line_segment_2d<double > start_contact(this->start_.pt(), this->start_.bnd_pt_left());
  vgl_line_segment_2d<double > end_contact(this->end_.pt(), this->end_.bnd_pt_left());
  return vgl_lineseg_test_lineseg(start_contact, end_contact);
}


//------------------------------------------------------------------------------
//: Return true if the left boundary has swallowtail
bool dbsksp_xshock_fragment::
bnd_has_swallowtail_right() const
{
  // swallowtail occurs when the two contact shocks at two ends of the fragment intersect
  vgl_line_segment_2d<double > start_contact(this->start_.pt(), this->start_.bnd_pt_right());
  vgl_line_segment_2d<double > end_contact(this->end_.pt(), this->end_.bnd_pt_right());
  return vgl_lineseg_test_lineseg(start_contact, end_contact);
}


//------------------------------------------------------------------------------
//: compute fragment area by approximating it with a polygon
double dbsksp_xshock_fragment::
area_approx_w_polygon(int num_pts) const
{
  int num_pts_per_side = vnl_math_max((num_pts - 2)/2, 2);
  vcl_vector<vgl_point_2d<double > > pts;


  // left boundary
  dbgl_biarc left_biarc;
  if (left_biarc.compute_biarc_params(this->start_.bnd_pt_left(), this->start_.bnd_tangent_left(),
    this->end_.bnd_pt_left(), this->end_.bnd_tangent_left()) && left_biarc.is_consistent())
  {
    double len = left_biarc.len();
    for (int i=0; i< num_pts_per_side; ++i)
    {
      double s = (len*i) / (num_pts_per_side-1);
      vgl_point_2d<double > pt = left_biarc.point_at(s);
      pts.push_back(pt);
    }
  }
  else
  {
    return -1;
  }

  // end shock-point
  pts.push_back(this->end_.pt());

  // right boundary
  dbgl_biarc right_biarc;
  if (right_biarc.compute_biarc_params(this->start_.bnd_pt_right(), this->start_.bnd_tangent_right(),
    this->end_.bnd_pt_right(), this->end_.bnd_tangent_right()))
  {
    double len = right_biarc.len();
    for (int i= num_pts_per_side-1; i >= 0; --i)
    {
      double s = (len*i) / (num_pts_per_side-1);
      vgl_point_2d<double > pt = right_biarc.point_at(s);
      pts.push_back(pt);
    }
  }
  else
  {
    return -1;
  }
  pts.push_back(this->start_.pt());

  // compute area of a polygon
  return vgl_area(vgl_polygon<double >(pts));
}




//------------------------------------------------------------------------------
//: Translate the fragment
void dbsksp_xshock_fragment::
translate(const vgl_vector_2d<double >& v)
{
  this->start_.set_pt(this->start_.pt() + v);;
  this->end_.set_pt(this->end_.pt() + v);
  return;
}




//------------------------------------------------------------------------------
//: Rotate the fragment
void dbsksp_xshock_fragment::
rotate(const vgl_point_2d<double >& center, double angle_in_radian)
{
  // vector from center to start node
  vgl_point_2d<double > new_start = center + rotated(this->start_.pt() - center, angle_in_radian);
  vgl_point_2d<double > new_end   = center + rotated(this->end_.pt()   - center, angle_in_radian);

  this->start_.set_pt(new_start);
  this->end_.set_pt(new_end);

  this->start_.set_shock_tangent(this->start_.psi() + angle_in_radian);
  this->end_.set_shock_tangent(this->end_.psi()   + angle_in_radian);

  return;
  
}




//------------------------------------------------------------------------------
//: Compute bounding box of the fragment
vgl_box_2d<double > dbsksp_xshock_fragment::
compute_bounding_box() const
{
  // This computation is just an approximation
  // Sample 9 points on each boundary contour and compute bounding box for all of them
  int num_segs = 8;
  vgl_box_2d<double > bbox;
  dbgl_biarc bnd_left = this->bnd_left_as_biarc();
  double len_left = bnd_left.len();
  for (int i =0; i <= num_segs; ++i)
  {
    double s = i*len_left / num_segs;
    bbox.add(bnd_left.point_at(s));
  }

  dbgl_biarc bnd_right = this->bnd_right_as_biarc();
  double len_right = bnd_right.len();
  for (int i =0; i <= num_segs; ++i)
  {
    double s = i*len_right / num_segs;
    bbox.add(bnd_right.point_at(s));
  }
  
  return bbox;
}

// -----------------------------------------------------------------------------
//: Rotate, scale, then translate the fragment
dbsksp_xshock_fragment_sptr dbsksp_xshock_fragment::
rot_scale_trans(const vgl_point_2d<double >& rot_center, 
                double rot_angle_in_radian, 
                double scale_up_factor, 
                const vgl_vector_2d<double >& translate_vec)
{
  dbsksp_xshock_node_descriptor start = this->start();
  dbsksp_xshock_node_descriptor end = this->end();

  // rotation & scaling
  start.pt_ = rot_center + scale_up_factor * rotated(start.pt_-rot_center, rot_angle_in_radian);
  start.psi_ += rot_angle_in_radian;
  start.radius_ *= scale_up_factor;

  end.pt_ = rot_center + scale_up_factor * rotated(end.pt_-rot_center, rot_angle_in_radian);
  end.psi_ += rot_angle_in_radian;
  end.radius_ *= scale_up_factor;

  // translation
  start.pt_ += translate_vec;
  end.pt_ += translate_vec;

  return new dbsksp_xshock_fragment(start, end);
}


