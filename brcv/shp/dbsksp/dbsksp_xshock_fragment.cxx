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
#include <vgl/vgl_distance.h>


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

//------------------------------------------------------------------------------
// Added by Yuliang, Oct, 2014
//: Return true if this fragment is legal, i.e. no self-intersection
bool dbsksp_xshock_fragment::
is_legal_new() const
{
  // 
  bool is_legal = true;
  is_legal &= !(this->bnd_has_swallowtail_left());
  is_legal &= !(this->bnd_has_swallowtail_right());


  // 1 bi-arc model fragment can me decomposed into three singular-arc model fragments, check their legality in order
  dbgl_biarc L_bnd = bnd_left_as_biarc();
  dbgl_biarc R_bnd = bnd_right_as_biarc();

  vgl_point_2d< double > start_pt_L = L_bnd.start();
  vgl_point_2d< double > start_pt_R = R_bnd.start();
  double start_angle_L = L_bnd.start_angle(); // (0, 2pi)
  double start_angle_R = R_bnd.start_angle(); // (0, 2pi)
  double L_L = L_bnd.len1();
  double L_R = R_bnd.len1();
  vgl_point_2d< double > center_L = L_bnd.center1();
  vgl_point_2d< double > center_R = R_bnd.center1();
  double r_L = L_bnd.r1();
  double r_R = R_bnd.r1();
  vgl_point_2d<double > start_shock_pt = this->start_shock_pt();


  vgl_point_2d< double > end_pt_L; double end_angle_L, actual_L_L;
  vgl_point_2d< double > end_pt_R; double end_angle_R, actual_L_R;
  vgl_point_2d< double > end_shock_pt;

  vcl_cout << "\n current bi-arcs: \n";
  vcl_cout << "L: r1 " << L_bnd.r1() << " l1 " << L_bnd.len1() << " r2 " << L_bnd.r2() << " l2 " << L_bnd.len2() << vcl_endl;
  vcl_cout << "R: r1 " << R_bnd.r1() << " l1 " << R_bnd.len1() << " r2 " << R_bnd.r2() << " l2 " << R_bnd.len2() << vcl_endl;

  // check the first singular-arc fragment
  is_legal &= is_legal_singular_arc_frag(start_shock_pt, start_pt_L, start_angle_L, L_L, center_L, r_L,
										 start_pt_R, start_angle_R, L_R, center_R, r_R,
										 end_pt_L, end_angle_L, actual_L_L,
										 end_pt_R, end_angle_R, actual_L_R, end_shock_pt);

  if(!is_legal)
	  return is_legal;

  vcl_cout << "------------ pass the 1st arc frag check ---------------\n";

  //vcl_cout << " actual_L_L " << actual_L_L << " L_L " << L_L << " actual_L_R " << actual_L_R << " L_R " << L_R << vcl_endl;
  if(actual_L_L < L_L && actual_L_R == L_R) // case right bnd end before left bnd
  {
      // update left arc
	  start_pt_L = end_pt_L;
	  start_angle_L = end_angle_L;
	  // continue the incomplete length
 	  L_L = L_L - actual_L_L;
	  // center_L and r_L keep the same      

	  // update right arc
	  start_pt_R = end_pt_R; // should be equal to R_bnd.mid_pt()
	  start_angle_R = end_angle_R;
	  // right arc is new
 	  L_R = R_bnd.len2();
	  center_R = R_bnd.center2();
	  r_R = R_bnd.r2();
  }
  else if (actual_L_L == L_L && actual_L_R < L_R) // case left bnd end before right bnd
  {
      // update left arc
	  start_pt_L = end_pt_L; // should be equal to L_bnd.mid_pt()
	  start_angle_L = end_angle_L;
	  // left arc is new
	  L_L = L_bnd.len2();
	  center_L = L_bnd.center2();
	  r_L = L_bnd.r2();

	  // update right arc
	  start_pt_R = end_pt_R; // should be equal to R_bnd.mid_pt()
	  start_angle_R = end_angle_R;
	  // continue the incomplete length
	  L_R = L_R - actual_L_R;
	  // keep center_R and r_R 	  	  
  }
  else if (actual_L_L == L_L && actual_L_R == L_R) // very rare case that left bun and right bud end together
  {
      // update left arc
	  start_pt_L = end_pt_L; // should be equal to L_bnd.mid_pt()
	  start_angle_L = end_angle_L;
	  // left arc is new
	  L_L = L_bnd.len2();
	  center_L = L_bnd.center2();
	  r_L = L_bnd.r2();	  

	  // update right arc
	  start_pt_R = end_pt_R; // should be equal to R_bnd.mid_pt()
	  start_angle_R = end_angle_R;
	  // right arc is new
 	  L_R = R_bnd.len2();
	  center_R = R_bnd.center2();
	  r_R = R_bnd.r2();
		
  }

  //update start_shock_pt
  start_shock_pt = end_shock_pt;


  // Check the second sigular-arc fragment
  is_legal &= is_legal_singular_arc_frag(start_shock_pt, start_pt_L, start_angle_L, L_L, center_L, r_L,
										 start_pt_R, start_angle_R, L_R, center_R, r_R,
										 end_pt_L, end_angle_L, actual_L_L,
										 end_pt_R, end_angle_R, actual_L_R, end_shock_pt); 
  if(!is_legal)
	  return is_legal;

  vcl_cout << "------------ pass the 2nd arc frag check ---------------\n";
  //vcl_cout << " actual_L_L " << actual_L_L << " L_L " << L_L << " actual_L_R " << actual_L_R << " L_R " << L_R << vcl_endl;
  if(actual_L_L < L_L && actual_L_R == L_R) // case right bnd end before left bnd
  {
      // update left arc
	  start_pt_L = end_pt_L;
	  start_angle_L = end_angle_L;
	  // continue the incomplete length
 	  L_L = L_L - actual_L_L;
	  // center_L and r_L keep the same      

	  // update right arc
	  start_pt_R = end_pt_R; // should be equal to R_bnd.mid_pt()
	  start_angle_R = end_angle_R;
	  // right arc is new
 	  L_R = R_bnd.len2();
	  center_R = R_bnd.center2();
	  r_R = R_bnd.r2();
  }
  else if (actual_L_L == L_L && actual_L_R < L_R) // case left bnd end before right bnd
  {
      // update left arc
	  start_pt_L = end_pt_L; // should be equal to L_bnd.mid_pt()
	  start_angle_L = end_angle_L;
	  // left arc is new
	  L_L = L_bnd.len2();
	  center_L = L_bnd.center2();
	  r_L = L_bnd.r2();

	  // update right arc
	  start_pt_R = end_pt_R; // should be equal to R_bnd.mid_pt()
	  start_angle_R = end_angle_R;
	  // continue the incompete length
	  L_R = L_R - actual_L_R;
	  // keep center_R and r_R 	  	  
  }
  else if (actual_L_L == L_L && actual_L_R == L_R)  // case when mid point meet and decomposite end in two singular-arc fragments
  {
	  return is_legal;	
  }

  //update start_shock_pt
  start_shock_pt = end_shock_pt;

  // Check the third sigular-arc fragment
  is_legal &= is_legal_singular_arc_frag(start_shock_pt, start_pt_L, start_angle_L, L_L, center_L, r_L,
										 start_pt_R, start_angle_R, L_R, center_R, r_R,
										 end_pt_L, end_angle_L, actual_L_L,
										 end_pt_R, end_angle_R, actual_L_R, end_shock_pt); 

  if(!is_legal)
  	  return false;

  vcl_cout << "------------ pass the 3rd arc frag check ---------------\n";
  //vcl_cout << " actual_L_L " << actual_L_L << " L_L " << L_L << " actual_L_R " << actual_L_R << " L_R " << L_R << vcl_endl;
  return true;

}

// check the legality of singular arc model fragment, no self-intersection. return the actual left and right end point in order to decomposite bi-arc fragment
bool dbsksp_xshock_fragment::
is_legal_singular_arc_frag(vgl_point_2d< double > start_shock_pt, 
						   vgl_point_2d< double > start_pt_L, double start_angle_L, double L_L, vgl_point_2d< double > center_L, double r_L,
						   vgl_point_2d< double > start_pt_R, double start_angle_R, double L_R, vgl_point_2d< double > center_R, double r_R,
						   vgl_point_2d< double > &end_pt_L, double &end_angle_L, double &actual_L_L,
						   vgl_point_2d< double > &end_pt_R, double &end_angle_R, double &actual_L_R, vgl_point_2d< double > &end_shock_pt) const
{

	double L = vgl_distance(center_L, center_R);
	double d1 = vgl_distance(center_L, start_shock_pt);
	double d2 = vgl_distance(center_R, start_shock_pt);

	// project to cannonical space with center_L at (-0.5L,0), center_R at (0.5L,0)
	double theta_c = vcl_atan2(center_R.y()-center_L.y(), center_R.x()-center_L.x()); //  (-pi, pi)
	//convert to (0, 2pi)
	if(theta_c<0)
		theta_c += 2*vnl_math::pi;
	double delta_theta = 2*vnl_math::pi - theta_c;  //  (0,2pi)
	double x_orig = (center_R.x()+center_L.x())/2;
	double y_orig = (center_R.y()+center_L.y())/2;

	// rotation and translation
	//  |x'|       | cos(delta_theta)  -sin(delta_theta) |   |x|        | x_orig|
	//  |  |  =	   |	                                 |   | |    -   |       |
	//	|y'|   	   | sin(delta_theta)  cos(delta_theta)  |   |y|        | y_orig|

	// start shock point in cannonical coordinates
	double X_s = vcl_cos(delta_theta)*(start_shock_pt.x()- x_orig) - vcl_sin(delta_theta)*(start_shock_pt.y()- y_orig);
	double Y_s = vcl_sin(delta_theta)*(start_shock_pt.x()- x_orig) + vcl_cos(delta_theta)*(start_shock_pt.y()- y_orig);
	// start left point in cannonical coordinates
	double x_L0 = vcl_cos(delta_theta)*(start_pt_L.x()- x_orig) - vcl_sin(delta_theta)*(start_pt_L.y()- y_orig);
	double y_L0 = vcl_sin(delta_theta)*(start_pt_L.x()- x_orig) + vcl_cos(delta_theta)*(start_pt_L.y()- y_orig);
	double tau_L0 = start_angle_L + delta_theta; // can >= 2pi
	if(tau_L0 >= vnl_math::pi*2)
		tau_L0 -= vnl_math::pi*2; // (0, 2pi)
	double theta_L0 = atan2(y_L0, x_L0 + L/2); // (-pi, pi)
	if(theta_L0<0)
		theta_L0 += vnl_math::pi*2; // (0, 2pi)

	// start right point in cannonical coordinates
	double x_R0 = vcl_cos(delta_theta)*(start_pt_R.x()-x_orig) - vcl_sin(delta_theta)*(start_pt_R.y()-y_orig);
	double y_R0 = vcl_sin(delta_theta)*(start_pt_R.x()-x_orig) + vcl_cos(delta_theta)*(start_pt_R.y()-y_orig);
	double tau_R0 = start_angle_R + delta_theta; // can >= 2pi
	if(tau_R0 >= vnl_math::pi*2)
		tau_R0 -= vnl_math::pi*2; // (0, 2pi)
	double theta_R0 = atan2(y_R0, x_R0 - L/2); // (-pi, pi)
	if(theta_R0<0)
		theta_R0 += vnl_math::pi*2; // (0, 2pi)

	// compute the direction of arc 
	double sign_L = vcl_cos(theta_L0)*vcl_sin(tau_L0) - sin(theta_L0)* cos(tau_L0); // should be +1, -1
	double sign_R = vcl_cos(theta_R0)*vcl_sin(tau_R0) - sin(theta_R0)* cos(tau_R0); // should be +1, -1

	
	//vcl_cout << "\n sign_L " << sign_L << " sign_R " << sign_R << " theta_L0 " << theta_L0 << " tau_L0 " << tau_L0 << " theta_R0 " << theta_R0 << " tau_R0 " << tau_R0 << "\n\n";

	// compute the end angle in cannonical space
	double theta_L = theta_L0 + sign_L*L_L/r_L;	
	double theta_R = theta_R0 + sign_R*L_R/r_R;

	// variances to be computed in canonical space and transform to orignal space later.
	double X_I,Y_I, x_L1, y_L1, x_R1, y_R1, theta_L_actual, theta_R_actual;

	//vcl_cout << " L = " << L << " r_L = " << r_L << " r_R = " << r_R << " d1 = " << d1 << " d2 = " << d2  << vcl_endl;
	//vcl_cout << " d1+d2 = " << d1+d2 << " r_L+r_R = " << r_L + r_R << " d1-d2 = " << d1-d2  << " r_L-r_R = " << r_L - r_R  << vcl_endl;

	if(L < vcl_abs(r_L-r_R) && vcl_abs((d1+d2) - (r_L+r_R)) <0.001)
	{
		vcl_cout << " -------start case 1 check------- \n";
		double a = (r_L+r_R)/2;
		double b = vcl_sqrt(a*a - (L/2)*(L/2));

		double A = b*b + a*a*vcl_tan(theta_L)*vcl_tan(theta_L);
		double B = a*a*vcl_tan(theta_L)*vcl_tan(theta_L)*L;
		double C = a*a*vcl_tan(theta_L)*vcl_tan(theta_L)*L*L/4 - a*a*b*b;

		X_I =( -B + vcl_sqrt(B*B-4*A*C))/2/A;

		if((X_I + L/2)*vcl_cos(theta_L) < 0)
			X_I =( -B - vcl_sqrt(B*B-4*A*C))/2/A;

		Y_I = vcl_tan(theta_L) * (X_I + L/2);

		theta_R_actual = atan2(Y_I, X_I-L/2);  // (-pi, pi)
		if(theta_R_actual < 0)
			theta_R_actual += 2*vnl_math::pi;  // (0, 2pi)

		actual_L_R = (theta_R_actual - theta_R0) * r_R / sign_R;
		if(actual_L_R<0)
			actual_L_R += 2*vnl_math::pi*r_R;

		if(actual_L_R <= L_R) // case that left arc ends early
		{
			//assign end
			actual_L_L = L_L;
			x_L1 = -L/2 + r_L*vcl_cos(theta_L);
			y_L1 = r_L*vcl_sin(theta_L);
			theta_L_actual = theta_L;

			x_R1 = L/2 + r_R*vcl_cos(theta_R_actual);
			y_R1 = r_R*vcl_sin(theta_R_actual);
		}
		else // case that right arc ends early
		{
			// resolve everything
			A = b*b + a*a*vcl_tan(theta_R)*vcl_tan(theta_R);
			B = -a*a*vcl_tan(theta_R)*vcl_tan(theta_R)*L;
			C = a*a*vcl_tan(theta_R)*vcl_tan(theta_R)*L*L/4 - a*a*b*b;

			X_I =( -B + vcl_sqrt(B*B-4*A*C))/2/A;

			if((X_I - L/2)*vcl_cos(theta_R) < 0)
				X_I =( -B - vcl_sqrt(B*B-4*A*C))/2/A;

			Y_I = vcl_tan(theta_R) * (X_I - L/2);

			theta_L_actual = atan2(Y_I, X_I + L/2); // (-pi, pi)
			if(theta_L_actual < 0)
				theta_L_actual += 2*vnl_math::pi;	// (0, 2pi)

			actual_L_L = (theta_L_actual - theta_L0) * r_L / sign_L;
			if(actual_L_L<0)
				actual_L_L += 2*vnl_math::pi*r_L;
			// assign end
			x_L1 = -L/2 + r_L*vcl_cos(theta_L_actual);
			y_L1 = r_L*vcl_sin(theta_L_actual);

			actual_L_R = L_R;
			x_R1 = L/2 + r_R*vcl_cos(theta_R_actual);
			y_R1 = r_R*vcl_sin(theta_R_actual);
			theta_R_actual = theta_R;
		}
		vcl_cout << " -------pass case 1 check------- \n";
	}
	else if(L > vcl_abs(r_L + r_R) && vcl_abs((d1-d2) - (r_L-r_R)) <0.001 )
	{
		vcl_cout << " -------start case 2 check------- \n";
double a = (r_L-r_R)/2;
		double b = vcl_sqrt((L/2)*(L/2)-a*a);

		double A = b*b - a*a*vcl_tan(theta_L)*vcl_tan(theta_L);
		double B = -a*a*vcl_tan(theta_L)*vcl_tan(theta_L)*L;
		double C = -a*a*vcl_tan(theta_L)*vcl_tan(theta_L)*L*L/4 - a*a*b*b;

		// legality check for case 2
		// intersect can exist
		if(B*B-4*A*C <=0)
		{
			vcl_cout << " -------fail case 2 check------- \n";
			return false;
		}

		X_I =( -B + vcl_sqrt(B*B-4*A*C))/2/A;

		if((X_I + L/2)*vcl_cos(theta_L) < 0)
			X_I =( -B - vcl_sqrt(B*B-4*A*C))/2/A;

		Y_I = vcl_tan(theta_L) * (X_I + L/2);

		theta_R_actual = atan2(Y_I, X_I-L/2); // (-pi, pi)
		if(theta_R_actual < 0)
			theta_R_actual += 2*vnl_math::pi; // (0, 2pi)

		actual_L_R = (theta_R_actual - theta_R0) * r_R / sign_R;
		if(actual_L_R<0)
			actual_L_R += 2*vnl_math::pi*r_R;

		if(actual_L_R <= L_R) // case that left arc ends early
		{
			//assign end
			actual_L_L = L_L;
			x_L1 = -L/2 + r_L*vcl_cos(theta_L);
			y_L1 = r_L*vcl_sin(theta_L);
			theta_L_actual = theta_L;

			x_R1 = L/2 + r_R*vcl_cos(theta_R_actual);
			y_R1 = r_R*vcl_sin(theta_R_actual);
		}
		else // case that right arc ends early
		{
			// resolve everything
			A = b*b - a*a*vcl_tan(theta_R)*vcl_tan(theta_R);
			B = a*a*vcl_tan(theta_R)*vcl_tan(theta_R)*L;
			C = -a*a*vcl_tan(theta_R)*vcl_tan(theta_R)*L*L/4 - a*a*b*b;

			// legality check for case 2
			// intersect can exist
			if(B*B-4*A*C <=0)
			{
				vcl_cout << " -------fail case 2 check------- \n";
				return false;
			}
			
			X_I =( -B + vcl_sqrt(B*B-4*A*C))/2/A;

			if((X_I - L/2)*vcl_cos(theta_R) < 0)
				X_I =( -B - vcl_sqrt(B*B-4*A*C))/2/A;

			Y_I = vcl_tan(theta_R) * (X_I - L/2);

			theta_L_actual = atan2(Y_I, X_I + L/2); // (-pi, pi)
			if(theta_L_actual < 0)
				theta_L_actual += 2*vnl_math::pi;	// (0, 2pi)

			actual_L_L = (theta_L_actual - theta_L0) * r_L / sign_L;
			if(actual_L_L<0)
				actual_L_L += 2*vnl_math::pi*r_L;
			// assign end
			x_L1 = -L/2 + r_L*vcl_cos(theta_L_actual);
			y_L1 = r_L*vcl_sin(theta_L_actual);

			actual_L_R = L_R;
			x_R1 = L/2 + r_R*vcl_cos(theta_R_actual);
			y_R1 = r_R*vcl_sin(theta_R_actual);
			theta_R_actual = theta_R;
		}
		// legality check for case 2

		vcl_cout << " -------pass case 2 check------- \n";

		return true;

	}
	else if(L >= vcl_abs(r_L-r_R) && L <= vcl_abs(r_L + r_R) && vcl_abs((d1-d2) - (r_L-r_R)) <0.001 )		
	{
		//// Need to confirm the derivation
		vcl_cout << " -------start case 3 check------- \n";
		double a = (r_L-r_R)/2;
		double b = vcl_sqrt((L/2)*(L/2)-a*a);

		double A = b*b - a*a*vcl_tan(theta_L)*vcl_tan(theta_L);
		double B = -a*a*vcl_tan(theta_L)*vcl_tan(theta_L)*L;
		double C = -a*a*vcl_tan(theta_L)*vcl_tan(theta_L)*L*L/4 - a*a*b*b;

		if(B*B-4*A*C <=0)
		{
			vcl_cout << " -------fail case 2 check------- \n";
			return false;
		}
		X_I =( -B + vcl_sqrt(B*B-4*A*C))/2/A;

		if((X_I + L/2)*vcl_cos(theta_L) < 0)
			X_I =( -B - vcl_sqrt(B*B-4*A*C))/2/A;

		Y_I = vcl_tan(theta_L) * (X_I + L/2);

		theta_R_actual = atan2(Y_I, X_I-L/2); // (-pi, pi)
		if(theta_R_actual < 0)
			theta_R_actual += 2*vnl_math::pi; // (0, 2pi)

		actual_L_R = (theta_R_actual - theta_R0) * r_R / sign_R;
		if(actual_L_R<0)
			actual_L_R += 2*vnl_math::pi*r_R;

		if(actual_L_R <= L_R) // case that left arc ends early
		{
			//assign end
			actual_L_L = L_L;
			x_L1 = -L/2 + r_L*vcl_cos(theta_L);
			y_L1 = r_L*vcl_sin(theta_L);
			theta_L_actual = theta_L;

			x_R1 = L/2 + r_R*vcl_cos(theta_R_actual);
			y_R1 = r_R*vcl_sin(theta_R_actual);
		}
		else // case that right arc ends early
		{
			// resolve everything
			A = b*b - a*a*vcl_tan(theta_R)*vcl_tan(theta_R);
			B = a*a*vcl_tan(theta_R)*vcl_tan(theta_R)*L;
			C = -a*a*vcl_tan(theta_R)*vcl_tan(theta_R)*L*L/4 - a*a*b*b;

			if(B*B-4*A*C <=0)
			{
				vcl_cout << " -------fail case 2 check------- \n";
				return false;
			}
			X_I =( -B + vcl_sqrt(B*B-4*A*C))/2/A;

			if((X_I - L/2)*vcl_cos(theta_R) < 0)
				X_I =( -B - vcl_sqrt(B*B-4*A*C))/2/A;

			Y_I = vcl_tan(theta_R) * (X_I - L/2);

			theta_L_actual = atan2(Y_I, X_I + L/2); // (-pi, pi)
			if(theta_L_actual < 0)
				theta_L_actual += 2*vnl_math::pi;	// (0, 2pi)

			actual_L_L = (theta_L_actual - theta_L0) * r_L / sign_L;
			if(actual_L_L<0)
				actual_L_L += 2*vnl_math::pi*r_L;
			// assign end
			x_L1 = -L/2 + r_L*vcl_cos(theta_L_actual);
			y_L1 = r_L*vcl_sin(theta_L_actual);

			actual_L_R = L_R;
			x_R1 = L/2 + r_R*vcl_cos(theta_R_actual);
			y_R1 = r_R*vcl_sin(theta_R_actual);
			theta_R_actual = theta_R;
		}
		// legality check for case 3
		double x_e = (r_L*r_L - r_R*r_R)/2/L;
		double y_e_pos = vcl_sqrt((r_L + x_e + L/2)*(r_L - x_e - L/2));
		double y_e_neg = -vcl_sqrt((r_L + x_e + L/2)*(r_L - x_e - L/2));

		if( Y_s < y_e_pos && Y_s > y_e_neg && Y_I < y_e_pos && Y_I > y_e_neg )
			vcl_cout << " -------pass case 3 check------- \n";
		else if( Y_s > y_e_pos && Y_I > y_e_pos )
			vcl_cout << " -------pass case 3 check------- \n";
		else if( Y_s < y_e_neg && Y_I < y_e_neg )
			vcl_cout << " -------pass case 3 check------- \n";
		else
		{
			vcl_cout << " -------fail case 3 check------- \n";
			return false;
		}
	}
	else if(L >= vcl_abs(r_L-r_R) && L <= vcl_abs(r_L + r_R) && vcl_abs((d1+d2) - (r_L+r_R)) <0.001)
	{	
 		vcl_cout << " -------start case 4 check------- \n";
		double a = (r_L+r_R)/2;
		double b = vcl_sqrt(a*a - (L/2)*(L/2));

		double A = b*b + a*a*vcl_tan(theta_L)*vcl_tan(theta_L);
		double B = a*a*vcl_tan(theta_L)*vcl_tan(theta_L)*L;
		double C = a*a*vcl_tan(theta_L)*vcl_tan(theta_L)*L*L/4 - a*a*b*b;

		X_I =( -B + vcl_sqrt(B*B-4*A*C))/2/A;

		if((X_I + L/2)*vcl_cos(theta_L) < 0)
			X_I =( -B - vcl_sqrt(B*B-4*A*C))/2/A;

		Y_I = vcl_tan(theta_L) * (X_I + L/2);

		theta_R_actual = atan2(Y_I, X_I-L/2); // (-pi, pi)
		if(theta_R_actual < 0)
			theta_R_actual += 2*vnl_math::pi; // (0, 2pi)

		actual_L_R = (theta_R_actual - theta_R0) * r_R / sign_R;
		if(actual_L_R<0)
			actual_L_R += 2*vnl_math::pi*r_R;

		if(actual_L_R <= L_R) // case that left arc ends early
		{
			//assign end
			actual_L_L = L_L;
			x_L1 = -L/2 + r_L*vcl_cos(theta_L);
			y_L1 = r_L*vcl_sin(theta_L);
			theta_L_actual = theta_L;

			x_R1 = L/2 + r_R*vcl_cos(theta_R_actual);
			y_R1 = r_R*vcl_sin(theta_R_actual);
		}
		else // case that right arc ends early
		{
			// resolve everything
			A = b*b + a*a*vcl_tan(theta_R)*vcl_tan(theta_R);
			B = -a*a*vcl_tan(theta_R)*vcl_tan(theta_R)*L;
			C = a*a*vcl_tan(theta_R)*vcl_tan(theta_R)*L*L/4 - a*a*b*b;

			X_I =( -B + vcl_sqrt(B*B-4*A*C))/2/A;

			if((X_I - L/2)*vcl_cos(theta_R) < 0)
				X_I =( -B - vcl_sqrt(B*B-4*A*C))/2/A;

			Y_I = vcl_tan(theta_R) * (X_I - L/2);

			theta_L_actual = atan2(Y_I, X_I + L/2); // (-pi, pi)
			if(theta_L_actual < 0)
				theta_L_actual += 2*vnl_math::pi;	// (0, 2pi)

			actual_L_L = (theta_L_actual - theta_L0) * r_L / sign_L;
			if(actual_L_L<0)
				actual_L_L += 2*vnl_math::pi*r_L;
			// assign end
			x_L1 = -L/2 + r_L*vcl_cos(theta_L_actual);
			y_L1 = r_L*vcl_sin(theta_L_actual);

			actual_L_R = L_R;
			x_R1 = L/2 + r_R*vcl_cos(theta_R_actual);
			y_R1 = r_R*vcl_sin(theta_R_actual);
			theta_R_actual = theta_R;
		}
		// legality check for case 4
		double x_e = (r_L*r_L - r_R*r_R)/2/L;

		if(x_L0 < x_e && x_R0 < x_e &&  x_L1 < x_e && x_R1 < x_e)
			vcl_cout << " -------pass case 4 check------- \n";
		else if(x_L0 > x_e && x_R0 > x_e &&  x_L1 > x_e && x_R1 > x_e)
			vcl_cout << " -------pass case 4 check------- \n";
		else
		{
			vcl_cout << " -------fail case 4 check------- \n";
			return false;			
		}			
	}
	else
	{
		vcl_cout<<" -------current singular arc frag do not belong to any legal cases------- \n";
		return false;	
	}
	// transform back to the original coodinates
	// rotation and translation
	//  |x|       | cos(delta_theta)  sin(delta_theta) |   |x' + x_orig|
	//  | |  =	   |	                                |   |           |
	//	|y|   	   | -sin(delta_theta) cos(delta_theta) |   |y' + y_orig|

	//vcl_cout << " X_s " << X_s << " Y_s " << Y_s << " X_L0 " << x_L0 << " Y_L0 " << y_L0 << " X_R0 " << x_R0 << " Y_R0 " << y_R0 <<"\n";
	//vcl_cout << " X_I " << X_I << " Y_I " << Y_I << " X_L1 " << x_L1 << " Y_L1 " << y_L1 << " X_R1 " << x_R1 << " Y_R1 " << y_R1 <<"\n";

	double X_I_orig = vcl_cos(delta_theta)*X_I + vcl_sin(delta_theta)*Y_I + x_orig;
	double Y_I_orig = -vcl_sin(delta_theta)*X_I + vcl_cos(delta_theta)*Y_I + y_orig;
	double x_L1_orig = vcl_cos(delta_theta)*x_L1 + vcl_sin(delta_theta)*y_L1 + x_orig;
	double y_L1_orig = -vcl_sin(delta_theta)*x_L1 + vcl_cos(delta_theta)*y_L1 + y_orig;
	double x_R1_orig = vcl_cos(delta_theta)*x_R1 + vcl_sin(delta_theta)*y_R1 + x_orig;
	double y_R1_orig = -vcl_sin(delta_theta)*x_R1 + vcl_cos(delta_theta)*y_R1 + y_orig;

	end_pt_L.set(x_L1_orig, y_L1_orig);
	end_pt_R.set(x_R1_orig, y_R1_orig);
	end_shock_pt.set(X_I_orig, Y_I_orig);

	// compute end_angle from start angle directly in original coordinates
	end_angle_L = start_angle_L +  (theta_L_actual - theta_L0);
	end_angle_R = start_angle_R +  (theta_R_actual - theta_R0);

	if(end_angle_L < 0)
		end_angle_L += vnl_math::pi*2;
	else if(end_angle_L >= vnl_math::pi*2)
		end_angle_L -= vnl_math::pi*2;

	if(end_angle_R < 0)
		end_angle_R += vnl_math::pi*2;
	else if(end_angle_R >= vnl_math::pi*2)
		end_angle_R -= vnl_math::pi*2;

	return true;
}

