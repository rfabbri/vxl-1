#include <dber/dber_match.h>
#include <dber/dber_edgel_similarity.h>
#include <dber/dber_utilities.h>

#include <vcl_iostream.h>
#include <vcl_algorithm.h>
#include <vul/vul_timer.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_hungarian_algorithm.h>

#include <vnl/algo/vnl_amoeba.h>
#include <vil/vil_image_resource.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_box_2d.h>
#include <vgl/vgl_distance.h>


#include <dbru/dbru_rcor.h>
#include <dbru/dbru_rcor_sptr.h>
#include <dbru/algo/dbru_rcor_generator.h>
#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_matcher.h>

#include <mbl/mbl_thin_plate_spline_2d.h>
#include <vgl/algo/vgl_norm_trans_2d.h>
#include <vgl/algo/vgl_line_2d_regression.h>

dber_match::dber_match() : sigma_square_(1.0f), scale_factor_(1.0f), radius_(1)
{
}

//: match the current edgel-image pairs, uses mutual information based edgel similaity and hungarian matching
//  width_radius: the radius of neighborhood for each edgel to eliminate distant edgels as nonsimilar without consideration
//                should be determined by checking the width of the edgel set and some ratio of it should be used as width_radius
double dber_match::match(vcl_vector<vil_image_view<vxl_byte> >&set1, vcl_vector<vil_image_view<vxl_byte> >&set2, float smoothing_sigma, float width_radius, float radius)
{
  int s1 = lines1_.size();
  int s2 = lines2_.size();
  if (!s1 || !s2)
    return -1;

  dber_edgel_similarity sim;
  sim.prepare_images(set1, set2, smoothing_sigma);

  // create the cost matrix, the lower the cost, the more likely to match
  int greater = s1 > s2 ? s1 : s2;
  vnl_matrix<double> cost_matrix(greater, greater, double(10000.0f));

  for (int i = 0; i<s1; i++)
    for (int j = 0; j<s2; j++) {
      cost_matrix[i][j] = sim.compute_cost(lines1_[i], lines2_[j], width_radius, radius);
    }

  vul_timer t;
  t.mark();
  assign_ = vnl_hungarian_algorithm<double>( cost_matrix );
  //if (assign_.size() != size)
    //vcl_cout << "Problems in hungarian matching! time: " << t.real()/1000 << " seconds\n";
  //else
  vcl_cout << "assign size: " << assign_.size() << vcl_endl;
    vcl_cout << "Hungarian matching succesfull! time: " << t.real()/1000 << " seconds\n";

  double total_cost = 0;
  for (unsigned int i = 0; i < assign_.size(); i++) {
    if (assign_[i] > 0)
      total_cost = cost_matrix[i][assign_[i]];
  }
  return total_cost;

}

// for sorting
static bool cost_compare(const vcl_pair<double, vcl_pair<unsigned, unsigned> >& l1,
                         const vcl_pair<double, vcl_pair<unsigned, unsigned> >& l2)
{
  return l1.first > l2.first;
}

void dber_match::set_lines1(const vcl_vector<vsol_line_2d_sptr>& l) {
  for (unsigned i = 0; i< l.size(); i++)
    lines1_.push_back(new vsol_line_2d(*l[i]));
}

void dber_match::set_lines2(const vcl_vector<vsol_line_2d_sptr>& l) {
  for (unsigned i = 0; i< l.size(); i++)
    lines2_.push_back(new vsol_line_2d(*l[i]));
}

void dber_match::clear_assignment() {
  assign_.clear();
}

void dber_match::clear_lines2() {
  lines2_.clear();
}

void dber_match::clear_lines1() {
  lines1_.clear();
}

//: match the current edgel-image pairs, use greedy matching algorithm
double dber_match::match_greedy(double threshold) {

  int s1 = lines1_.size();
  int s2 = lines2_.size();
  if (!s1 || !s2)
    return -1;

  // first scale and translate a copy of lines2 and use that
  // get the assignment based on this scaled and translated lines2
  // but then use original points to get the TPS
  vcl_vector<vsol_line_2d_sptr> lines2;
  for (unsigned i = 0; i< lines2_.size(); i++)
    lines2.push_back(new vsol_line_2d(*lines2_[i]));

  vsol_point_2d_sptr gc1 = dber_utilities::center_of_gravity(lines1_);
  vsol_point_2d_sptr gc2 = dber_utilities::center_of_gravity(lines2);
  dber_utilities::translate_lines(lines2, gc1->x()-gc2->x(), gc1->y()-gc2->y());

  //vgl_line_2d<double> dir1 = dber_match::find_dominant_dir(poly1_, gc1->x(), gc1->y());
  //vgl_line_2d<double> dir2 = dber_match::find_dominant_dir(poly2_, gc2->x(), gc2->y());

  // first rotate, and then scale
  //vgl_h_matrix_2d<double> H;  H.set_identity(); H.set_rotation(-dir2.slope_radians()+dir1.slope_radians());
  //dber_match::rotate_lines(lines2, H, gc1->x(), gc1->y());
  vsol_box_2d_sptr box1 = dber_utilities::get_box(lines1_);
  vsol_box_2d_sptr box2 = dber_utilities::get_box(lines2);
  scale_factor_ = box1->width()/box2->width();
  dber_utilities::scale_lines(lines2, scale_factor_);
  
  vul_timer t;
  t.mark();
  vcl_vector<vcl_pair<double, vcl_pair<unsigned, unsigned> > > cost_matrix; 
  int eliminated = 0, cnt = 0;
  for (int i = 0; i<s1; i++)
    for (int j = 0; j<s2; j++) {
      cnt++;
      //double mi = compute_cost(lines1_[i], lines2_[j]);  // a good threshold might be zero, since negative mutual info, means very unlikely match
      // take radius to be sigma_square
      double max;
      double mi = dber_edgel_similarity::compute_current_cost(lines1_[i], lines2[j], radius_, max);
      // dont even consider if the matching cost is too high
      if (mi < threshold) { 
        eliminated++;
        continue;
      }
      vcl_pair<unsigned, unsigned> p(i,j);
      vcl_pair<double, vcl_pair<unsigned, unsigned> > pp(mi, p);
      cost_matrix.push_back(pp);
    }
  // sort the costs
  sort(cost_matrix.begin(), cost_matrix.end(), cost_compare);

  // initialize assignments as null
  assign_ = vcl_vector<unsigned> (s1, s1 + s2);
  
  vcl_vector<bool> j_assigned(s2, false);
  vcl_vector<double> costs(s1, 10000.0f);
  // read the best assignments from sorted vector for each row
  vcl_vector<vcl_pair<double, vcl_pair<unsigned, unsigned> > >::iterator iter;
  for (iter = cost_matrix.begin(); iter != cost_matrix.end(); iter++) {
    unsigned i = (iter->second).first;
    unsigned j = (iter->second).second;
    //vcl_cout << "cost: " << iter->first << " i: " << i << " j: " << j << vcl_endl;
    if (int(assign_[i]) > s2 && !j_assigned[j]) {
      assign_[i] = j;
      j_assigned[j] = true;
      costs[i] = iter->first;
    }
  }
  vcl_cout << "total pairs: " << cnt << " eliminated: " << eliminated << " of them\n";
  return 0;
}

//: use the current assignment to generate a dense correspondence
bool dber_match::find_tps(bool pure_affine) {

  vcl_vector< vgl_homg_point_2d<double> > hpts1, hpts2;
  for (unsigned i = 0; i<assign_.size(); i++) {
    if (assign_[i] > lines2_.size()) 
      continue;
    vsol_point_2d_sptr mid1 = lines1_[i]->middle();
    vsol_point_2d_sptr mid2 = lines2_[assign_[i]]->middle();
    hpts1.push_back(vgl_homg_point_2d<double> (mid1->x(), mid1->y()));
    hpts2.push_back(vgl_homg_point_2d<double> (mid2->x(), mid2->y()));
  }
  vcl_cout << "# of assignments: " << assign_.size() << " using: " << hpts1.size() << " of them.\n";
  if (hpts1.size() == 0)
    return false;

  trans1_.compute_from_points(hpts1);
  trans2_.compute_from_points(hpts2);

  tps_.set_pure_affine(pure_affine);

  vcl_vector<vgl_point_2d<double> > tpts1, tpts2;
  for (unsigned int i = 0; i<hpts1.size(); i++) {
    vgl_homg_point_2d<double> tpt = trans1_(hpts1[i]);
    tpts1.push_back(vgl_point_2d<double> (tpt.x()/tpt.w(), tpt.y()/tpt.w()));
  }
  for (unsigned int i = 0; i<hpts2.size(); i++) {
    vgl_homg_point_2d<double> tpt = trans2_(hpts2[i]);
    tpts2.push_back(vgl_point_2d<double> (tpt.x()/tpt.w(), tpt.y()/tpt.w()));
  }

  tps_.build(tpts1,tpts2,true);
  vcl_cout << "energy of TPS X: " << tps_.bendingEnergyX() << " Y: " << tps_.bendingEnergyY() << vcl_endl; 
  return true;
}

//: use the dense correspondence to get mutual info
double dber_match::find_global_mi(dbinfo_observation_sptr obs1, dbinfo_observation_sptr obs2) {
  dbru_rcor_sptr rcor = new dbru_rcor(obs1, obs2);

  //: find region correspondence based on TPS, output the map in tps                 
  dbru_rcor_generator::find_correspondence_tps(rcor, tps_, trans1_, trans2_);
  
  double mi = dbinfo_observation_matcher::minfo(obs1, obs2, rcor->get_correspondences(), false);
#if 1
  cor_im_ = rcor->get_appearance2_on_pixels1();
#endif
  return mi;
}

