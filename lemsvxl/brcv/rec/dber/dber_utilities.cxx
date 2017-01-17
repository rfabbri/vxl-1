#include <dber/dber_utilities.h>
#include <dber/dber_edgel_similarity.h>
#include <vcl_cstdlib.h>
#include <vcl_iostream.h>

#include <vul/vul_timer.h>

#include <vnl/algo/vnl_amoeba.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vgl/vgl_distance.h>

#include <vgl/algo/vgl_norm_trans_2d.h>
#include <vgl/algo/vgl_line_2d_regression.h>


//:  scale all edgels in lines2 so that widhts are equal
void dber_utilities::scale_lines(vcl_vector<vsol_line_2d_sptr>& l, double scale_factor) {
    
  for (unsigned i = 0; i<l.size(); i++) {
    vsol_point_2d_sptr p0 = l[i]->p0();
    vsol_point_2d_sptr p1 = l[i]->p1();

    double x = p0->x();
    double y = p0->y();
    p0->set_x( x*scale_factor );
    p0->set_y( y*scale_factor );

    x = p1->x();
    y = p1->y();
    p1->set_x( x*scale_factor );
    p1->set_y( y*scale_factor );
  }
}

void dber_utilities::translate_lines(vcl_vector<vsol_line_2d_sptr>& l, double x, double y) 
{
  vgl_vector_2d<double> trans(x, y);
  for (unsigned i = 0; i<l.size(); i++) {
    vsol_line_2d_sptr ll = l[i];
    ll->p0()->add_vector(trans);
    ll->p1()->add_vector(trans);
  }
}

// multiply centers by length, and find weighted average
vsol_point_2d_sptr dber_utilities::center_of_gravity(vcl_vector<vsol_line_2d_sptr>& l)
{
  double xc=0, yc=0;
  double tlength = 0;
  for (unsigned i = 0; i<l.size(); i++) {
    vsol_point_2d_sptr mid = l[i]->middle();
    double len = l[i]->length();
    xc += mid->x()*len;
    yc += mid->y()*len;
    tlength += len;
  }
  xc /= tlength;
  yc /= tlength;

  vsol_point_2d_sptr gc = new vsol_point_2d(xc, yc);
  return gc;
}

vsol_box_2d_sptr dber_utilities::get_box(vcl_vector<vsol_line_2d_sptr>& l) {
  
  vsol_box_2d_sptr box = new vsol_box_2d();
  for (unsigned i = 0 ; i < l.size() ; i++ ) 
  {
    vsol_point_2d_sptr p = l[i]->middle();
    box->add_point(p->x(), p->y());
  }
  return box;
}

//: translate lines2 so that translation is optimal
// randomly choose translations within a neigborhood of centers and maximize support
void dber_utilities::optimize_translation(vcl_vector<vsol_line_2d_sptr>& l1, vcl_vector<vsol_line_2d_sptr>& l2, double sigma_square) 
{
  dber_cost_func c(2, l1, l2);
  c.set_sigma_square(sigma_square);
  //construct the Levenberg Marquardt minimizer
  vnl_amoeba minimizer(c);
  minimizer.set_relative_diameter(0.1f);
  minimizer.set_x_tolerance(0.05);
  minimizer.set_max_iterations(150);

  vnl_vector<double> x(2, 0);
  vcl_cout << "Start cost " << c.f(x) << '\n';
  minimizer.minimize(x);
  vcl_cout << "End cost " << c.f(x) << "trans x: " << x[0] << " trans y: " << x[1] << vcl_endl;
  
  // translate lines2
  dber_utilities::translate_lines(l2, x[0], x[1]);
}

vgl_line_2d<double> dber_utilities::find_dominant_dir(vsol_polygon_2d_sptr poly, double center_x, double center_y) {
  vgl_line_2d_regression<double> reg;
  
  // add points of the polygon
  for (unsigned int i = 0; i<poly->size(); i++) {
    vsol_point_2d_sptr p = poly->vertex(i);
    reg.increment_partial_sums(p->x(), p->y());
  }

  reg.fit_constrained(center_x, center_y);
  vcl_cout << "error in fitting: " << reg.get_rms_error() << vcl_endl;
  return reg.get_line();
}

//: rotate the lines about the given center point
void dber_utilities::rotate_lines(vcl_vector<vsol_line_2d_sptr>& l, vgl_h_matrix_2d<double> H, double xc, double yc) 
{
  for (unsigned i = 0; i<l.size(); i++) {
    vsol_point_2d_sptr p0 = l[i]->p0();
    vsol_point_2d_sptr p1 = l[i]->p1();

    vgl_homg_point_2d<double> hp(p0->x()-xc, p0->y()-yc);
    vgl_homg_point_2d<double> thp;
    thp = H(hp);
    vgl_point_2d<double> tep(thp);
    l[i]->set_p0(new vsol_point_2d(tep));

    vgl_homg_point_2d<double> hp2(p1->x()-xc, p1->y()-yc);
    thp = H(hp2);
    vgl_point_2d<double> tep2(thp);
    l[i]->set_p1(new vsol_point_2d(tep2));
  }
}

dber_cost_func::dber_cost_func(unsigned number_of_parameters, vcl_vector<vsol_line_2d_sptr>& l1, vcl_vector<vsol_line_2d_sptr>& l2):
  vnl_cost_function(number_of_parameters), lines1_(l1), lines2_(l2),
  no_params_(number_of_parameters), sigma_square_(1.0f)
{
  // max possible measure depends on number of edgels
  // if they all line up perfectly, since their lengths are 1
  // we get a 1 for each edgel pair, plus the number of support from edgels
  // in the vicinity, assume a constant say 10 neighbors at max
  maxval_ = (lines1_.size() > lines2_.size() ? lines1_.size() : lines2_.size())*10;
  vcl_cout << " maxval_: " << maxval_ << vcl_endl;
}

// the cost function for optimization. In this application,
// the vector x corresponds to the parameters of the allowable transformation
// between the two observations, e.g. tx and ty. The cost 
// is based on the total support between edgels.  
double 
dber_cost_func::f(vnl_vector<double> const& x)
{ 
  // translate lines2
  vcl_vector<vsol_line_2d_sptr> new_l2(lines2_);
  dber_utilities::translate_lines(new_l2, x[0], x[1]);

  //double c = maxval_-dber_utilities::measure_support(lines1_, new_l2, sigma_square_);
  double c = dber_edgel_similarity::measure_support(lines1_, new_l2, sigma_square_);
  return c;
}
