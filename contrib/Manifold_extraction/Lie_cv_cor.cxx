#include "Lie_cv_cor.h"
#include "Lie_cv_cor_sptr.h"
#include "Lie_contour_match.h"
#include <vcl_cmath.h>
#include <vcl_cstdio.h>
#include <vgl/algo/vgl_fit_lines_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vnl/vnl_math.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h> 
#include <vcl_iostream.h>
#include <vsol/vsol_point_2d.h>
#include <dbsol/dbsol_interp_curve_2d.h>

#define TOL                   (1.0e-1)
#define IS_ALMOST_ZERO(X)     (vcl_abs(X) < TOL)
#define IS_ALMOST_EQUAL(X,Y)  (vcl_abs(X-Y) < TOL)

Lie_cv_cor::Lie_cv_cor() : final_cost_(0), final_norm_cost_(0), length1_(-1), length2_(-1)
{
}

Lie_cv_cor::Lie_cv_cor(const dbsol_interp_curve_2d_sptr c1, 
                           const dbsol_interp_curve_2d_sptr c2, 
                           vcl_vector< vcl_pair <int,int> >& map)
                            : final_cost_(0), final_norm_cost_(0) {
                               
  // save pointers to the original polygons also
  // (might be line fitted versions of input polygons)
  vcl_vector<vsol_point_2d_sptr> tmp;
  for (int i = 0; i<c1->size(); i++)
    tmp.push_back(c1->point_at_sample(i));
  poly1_ = new vsol_polygon_2d(tmp);
  tmp.clear();
  
  
  for (int i = 0; i<c2->size(); i++)
    tmp.push_back(c2->point_at_sample(i));

  poly2_ = new vsol_polygon_2d(tmp);

unsigned int n1 = c1->size();
unsigned int n2 = c2->size();

  unsigned int idx_c1,idx_c2;
  // keep the arclengths on each interval in increasing order
  // curve1's arclengths are not necessarily starting from 0 and may be larger than length1_
  // because it might be rotated to match curve2
  for( unsigned int i = 0 ; i < map.size() ; i++ ) {

       unsigned int idx_1 = map[i].second;
      unsigned int idx_2 = c2->size();
      idx_c2 = idx_1%idx_2;

    vsol_point_2d_sptr pt = c2->point_at_sample(idx_c2);
    pts2_.push_back(pt->get_p());
    arclengths2_.push_back(c2->length_at(idx_c2)); 
  }

  for( unsigned int i = 0 ; i < map.size() ; i++ ) {

      unsigned int idx_1 = map[i].first;
      unsigned int idx_2 = c1->size();
      idx_c1 = idx_1%idx_2;

    vsol_point_2d_sptr pt = c1->point_at_sample(idx_c1);
    pts1_.push_back(pt->get_p());
    arclengths1_.push_back(c1->length_at(idx_c1)); // may not start from 0
  }
  
  // find total length of curve 2
  vsol_point_2d_sptr p2_last = c2->point_at_sample(c2->size()-1);
  vsol_point_2d_sptr p2_first = c2->point_at_sample(0);
  length2_ = c2->length_at(c2->size()-1) + vnl_math_hypot(p2_last->x()-p2_first->x(),
                                                          p2_last->y()-p2_first->y());

  // add the correspondence of the final interval 
  arclengths2_.push_back(length2_);  

  // find total length of curve 1
  //: ozge changed the following on Nov 17, 2006 
  //length1_ = c1->arcLength(n1);
  length1_ = c1->length();
  if (arclengths1_[0] != 0)
    arclengths1_.push_back(2*length1_);
  else 
    arclengths1_.push_back(length1_);
  

}


//: given s1 on curve1 return alpha(s1) on curve2
double Lie_cv_cor::get_arclength_on_curve2(double s1) {
  
  if (IS_ALMOST_ZERO(s1))
      s1 = 0.0;
   else if (IS_ALMOST_EQUAL(s1,length1_))
      s1 = length1_;
   else {
     //assert (s1 >= 0 && s1 <= length1_);
     if (s1 < 0 || s1 > length1_)
      return -1; // this should be caught by calling functions
   }
  
  if (s1 < arclengths1_[0])
    s1 = s1 + length1_;

  // binary search for s1 in vector of arclengths
  const vcl_vector<double>::const_iterator
      p = vcl_lower_bound(arclengths1_.begin(), arclengths1_.end(), s1);

  if (p == arclengths1_.begin())
    return arclengths2_[0];

  int loc = p - arclengths1_.begin();

  // find ratio on curve1
  double dif = s1-arclengths1_[loc-1];
  double interval_dif = arclengths1_[loc]-arclengths1_[loc-1];
  double interval_dif2 = arclengths2_[loc]-arclengths2_[loc-1];
  return arclengths2_[loc-1]+(dif/interval_dif)*interval_dif2;
}
//: given s2 on curve2 return alpha_inverse(s2) on curve1
double Lie_cv_cor::get_arclength_on_curve1(double s2) {
  if (IS_ALMOST_ZERO(s2))
      s2 = 0.0;
   else if (IS_ALMOST_EQUAL(s2,length2_))
      s2 = length2_;
   else {
      //assert (s2 >= 0 && s2 <= length2_);
     if (s2 < 0 || s2 > length2_)
      return -1; // this should be caught by calling functions
   }
  
  // binary search for s2 in vector of arclengths
  const vcl_vector<double>::const_iterator
      p = vcl_lower_bound(arclengths2_.begin(), arclengths2_.end(), s2);

  if (p == arclengths2_.begin())
    return vcl_fmod(arclengths1_[0], length1_);

  int loc = p - arclengths2_.begin();

  // find ratio on curve2
  double dif = s2-arclengths2_[loc-1];
  double interval_dif = arclengths2_[loc]-arclengths2_[loc-1];
  double interval_dif2 = arclengths1_[loc]-arclengths1_[loc-1];
  return vcl_fmod(arclengths1_[loc-1]+(dif/interval_dif)*interval_dif2, length1_);
}

//: write points to a file
bool Lie_cv_cor::write_correspondence(vcl_string file_name, int increment) 
{
  vcl_ofstream of(file_name.c_str());
  if (!of) {
    vcl_cout << "Unable to open output file " <<file_name << " for write " << vcl_endl;
    return false;
  }

  unsigned int cnt1 = 0, cnt2 = 0;
  for (unsigned i = 0; i<pts1_.size(); i+=increment)
    cnt1++;

  for (unsigned i = 0; i<pts2_.size(); i+=increment)
    cnt2++;

  of << cnt1 << vcl_endl;
  for (unsigned i = 0; i<pts1_.size(); i+=increment) 
    of << pts1_[i].x() << " " << pts1_[i].y() << "\n";
  
  of << cnt2 << vcl_endl;
  for (unsigned i = 0; i<pts2_.size(); i+=increment) 
    of << pts2_[i].x() << " " << pts2_[i].y() << "\n";

  of.close();
  return true;
}

//: Binary save self to stream.
void Lie_cv_cor::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, length1_);
  vsl_b_write(os, length2_);
  vsl_b_write(os, final_cost_);
  vsl_b_write(os, final_norm_cost_);

  if (poly1_) {
    vsl_b_write(os, true);
    poly1_->b_write(os);
  } else 
    vsl_b_write(os, false);

  if (poly2_) {
    vsl_b_write(os, true);
    poly2_->b_write(os);
  } else 
    vsl_b_write(os, false);

  vsl_b_write(os, arclengths1_.size());
  for (unsigned i = 0; i < arclengths1_.size(); i++) 
    vsl_b_write(os, arclengths1_[i]);
  
  vsl_b_write(os, arclengths2_.size());
  for (unsigned i = 0; i < arclengths2_.size(); i++) 
    vsl_b_write(os, arclengths2_[i]);
  
  vsl_b_write(os, pts1_.size());
  for (unsigned i = 0; i < pts1_.size(); i++) {
    vsl_b_write(os, pts1_[i].x());
    vsl_b_write(os, pts1_[i].y());
  }
  
  vsl_b_write(os, pts2_.size());
  for (unsigned i = 0; i < pts2_.size(); i++) {
    vsl_b_write(os, pts2_[i].x());
    vsl_b_write(os, pts2_[i].y());
  }
  
  return;
}

//: Binary load self from stream.
void Lie_cv_cor::b_read(vsl_b_istream &is)
{
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
    {
    case 1:
      {
       vsl_b_read(is, length1_);
       vsl_b_read(is, length2_);
       vsl_b_read(is, final_cost_);
       vsl_b_read(is, final_norm_cost_);
       
        bool poly_available;
        vsl_b_read(is, poly_available);
        if (poly_available) {
          poly1_ = new vsol_polygon_2d();
          poly1_->b_read(is);   
        } else 
          poly1_ = 0;

        vsl_b_read(is, poly_available);
        if (poly_available) {
          poly2_ = new vsol_polygon_2d();
          poly2_->b_read(is);   
        } else 
          poly2_ = 0;

       unsigned cnt;
       vsl_b_read(is, cnt);
       for (unsigned i = 0; i < cnt; i++) {
        double len;
        vsl_b_read(is, len);
        arclengths1_.push_back(len);
       }

       vsl_b_read(is, cnt);
       for (unsigned i = 0; i < cnt; i++) {
        double len;
        vsl_b_read(is, len);
        arclengths2_.push_back(len);
       }

       vsl_b_read(is, cnt);
       for (unsigned i = 0; i < cnt; i++) {
         double x, y;
         vsl_b_read(is, x); vsl_b_read(is, y);
         pts1_.push_back(vgl_point_2d<double> (x, y));
       }
         
       vsl_b_read(is, cnt);
       for (unsigned i = 0; i < cnt; i++) {
         double x, y;
         vsl_b_read(is, x); vsl_b_read(is, y);
         pts2_.push_back(vgl_point_2d<double> (x, y));
       }
      
       break;
      }
  }
}

