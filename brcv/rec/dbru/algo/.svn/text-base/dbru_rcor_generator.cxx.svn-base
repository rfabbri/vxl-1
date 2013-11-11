
#include <dbru/algo/dbru_rcor_generator.h>
#include <dbru/dbru_rcor.h>
#include <dbcvr/dbcvr_cv_cor.h>
#include <dbskr/dbskr_sm_cor.h>

#include <vsol/vsol_point_2d.h>
#include <btol/btol_face_algs.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/algo/vgl_homg_operators_2d.h>

#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vnl/vnl_double_3x3.h>

#include <bdgl/bdgl_curve_algs.h>

#include <dbsol/dbsol_interp_curve_2d.h>
#include <dbsol/algo/dbsol_curve_algs.h>

#include <dbskr/dbskr_scurve.h>

#include <dbinfo/dbinfo_region_geometry_sptr.h>
#include <dbinfo/dbinfo_region_geometry.h>
#include <dbinfo/dbinfo_observation.h>
#include <bsol/bsol_algs.h>
#include <vcl_algorithm.h> 
#include <assert.h>

#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>


#define LARGE_INT (1000000)
#define SMALL (1E-5)


//: find region correspondence based on TPS, output the map in tps
bool dbru_rcor_generator::find_correspondence_tps_curve(dbru_rcor_sptr cor, 
                                                        mbl_thin_plate_spline_2d& ftps, 
                                                        vgl_norm_trans_2d<double>& trans1, 
                                                        vgl_norm_trans_2d<double>& trans2,
                                                        int increment, bool compute_energy)
{
  if (!cor || cor->halt_) {
    vcl_cout << "Errors in constructor!\n";
    return false;
  }

  // first find curve matching before calling this function
  if (!cor->get_sil_cor()) {
    vcl_cout << "error in dbru_rcor_generator::find_correspondence_tps() - Silhouete contour/polygon correspondence is not set!\n";
    return false;
  }
  vcl_vector<vgl_point_2d<double> >& pts1 = cor->get_sil_cor()->get_contour_pts1();
  vcl_vector<vgl_point_2d<double> >& pts2 = cor->get_sil_cor()->get_contour_pts2();
  
  assert(pts1.size() == pts2.size());

  vcl_vector< vgl_homg_point_2d<double> > hpts1, hpts2;
  for (unsigned int i = 0; i<pts1.size(); ) {
    hpts1.push_back(vgl_homg_point_2d<double> (pts1[i].x(), pts1[i].y()));
    hpts2.push_back(vgl_homg_point_2d<double> (pts2[i].x(), pts2[i].y()));

    i+=increment;
    while (vgl_distance(pts1[i-increment], pts1[i]) < 0.01) {
      i+=increment;
      if (i >= pts1.size()) break;
    }
  }
  
  vcl_cout << "# of correspondence: " << pts1.size() << " using: " << hpts1.size() << " of them.\n";

  if (!find_correspondence_tps(cor, hpts1, hpts2, ftps, trans1, trans2,compute_energy))
    return false;

  return true;
}

//: find region correspondence based on TPS
bool dbru_rcor_generator::find_correspondence_tps(dbru_rcor_sptr cor, 
                                      mbl_thin_plate_spline_2d& tps, 
                                      vgl_norm_trans_2d<double>& trans1, 
                                      vgl_norm_trans_2d<double>& trans2) 
{
  if (!cor || cor->halt_) {
    vcl_cout << "Errors in constructor!\n";
    return false;
  }
  
  //: use the inverse normalization to get image location in the second region
  vgl_h_matrix_2d<double> inv2 = trans2.get_inverse();

  //: prepare a vector to hold corresponding (x,y) in the second region and count
  //                                                     x2 , y2    cnt
  cor->set_save_float(true);
  cor->initialize_float_map(); 
  //cor->cnt1_ = 0;

  //: go over each point in region1
  for (int i = 0; i<cor->upper_x_; i++) {
    for (int j = 0; j<cor->upper_y_; j++) {
      // if not a pixel inside the polygon continue
      if (cor->region1_[i][j] < 0) 
        continue;
      //vcl_cout << "pixel i: " << i << " j: " << j << vcl_endl;
      // create the vertical and horizontal lines that passes from this pixel
      vgl_homg_point_2d<double> hp(i+cor->min1_x_, j+cor->min1_y_);
      vgl_homg_point_2d<double> thp = trans1(hp);
      vgl_point_2d<double> thp_p(thp.x()/thp.w(), thp.y()/thp.w());
      vgl_point_2d<double> tthp_p = tps(thp_p);
      vgl_homg_point_2d<double> tthp(tthp_p.x(), tthp_p.y());
      vgl_homg_point_2d<double> hpp = inv2(tthp);
      //vgl_homg_point_2d<double> hpp = inv2(tps(trans1(hp)));

      vgl_point_2d<float> out_pt_float(float(hpp.x()/hpp.w()), float(hpp.y()/hpp.w()));
      vgl_point_2d<int> out_pt( (int)vcl_floor(out_pt_float.x()+0.5), (int)vcl_floor(out_pt_float.y()+0.5));
      
      if (out_pt.x() < cor->min2_x_ || out_pt.x() > cor->max2_x_ || 
          out_pt.y() < cor->min2_y_ || out_pt.y() > cor->max2_y_) 
          continue;

      if (cor->region2_[out_pt.x()-cor->min2_x_][out_pt.y()-cor->min2_y_] < 0) continue;

      cor->region1_map_output_[i][j] = out_pt;
      cor->region1_map_output_float_[i][j] = out_pt_float;
      vcl_pair<unsigned, unsigned> id_pair(static_cast<unsigned>(cor->region1_[i][j]), 
                                           static_cast<unsigned>(cor->region2_[out_pt.x()-cor->min2_x_][out_pt.y()-cor->min2_y_]));
      cor->correspondences_.push_back(id_pair);      
    }
  }

  return true;



  }

//: find region correspondence based on TPS, output the map in tps
bool dbru_rcor_generator::find_correspondence_tps(dbru_rcor_sptr cor, 
                                                  vcl_vector< vgl_homg_point_2d<double> >& hpts1, 
                                                  vcl_vector< vgl_homg_point_2d<double> >& hpts2, 
                                                  mbl_thin_plate_spline_2d& tps, 
                                                  vgl_norm_trans_2d<double>& trans1, 
                                                  vgl_norm_trans_2d<double>& trans2, bool compute_energy)
{
  if (!cor || cor->halt_) {
    vcl_cout << "Errors in constructor!\n";
    return false;
  }

  // normalize the data points so that SVD finds a solution
  //vgl_norm_trans_2d<double> trans1, trans2;

  trans1.compute_from_points(hpts1);
  trans2.compute_from_points(hpts2);

  vcl_vector<vgl_point_2d<double> > tpts1, tpts2;
  for (unsigned int i = 0; i<hpts1.size(); i++) {
    vgl_homg_point_2d<double> tpt = trans1(hpts1[i]);
    tpts1.push_back(vgl_point_2d<double> (tpt.x()/tpt.w(), tpt.y()/tpt.w()));
  }
  for (unsigned int i = 0; i<hpts2.size(); i++) {
    vgl_homg_point_2d<double> tpt = trans2(hpts2[i]);
    tpts2.push_back(vgl_point_2d<double> (tpt.x()/tpt.w(), tpt.y()/tpt.w()));
  }
  tps.build(tpts1,tpts2,compute_energy);
  
  //ftps = tps;
  // TODO: check the energy if its too large, that might indicate that computation was not good

  //: use the inverse normalization to get image location in the second region
  vgl_h_matrix_2d<double> inv2 = trans2.get_inverse();

  //: prepare a vector to hold corresponding (x,y) in the second region and count
  //                                                     x2 , y2    cnt
  cor->set_save_float(true);
  cor->initialize_float_map(); 
  //cor->cnt1_ = 0;

  //: go over each point in region1
  for (int i = 0; i<cor->upper_x_; i++) {
    for (int j = 0; j<cor->upper_y_; j++) {
      // if not a pixel inside the polygon continue
      if (cor->region1_[i][j] < 0) 
        continue;
      //vcl_cout << "pixel i: " << i << " j: " << j << vcl_endl;
      // create the vertical and horizontal lines that passes from this pixel
      vgl_homg_point_2d<double> hp(i+cor->min1_x_, j+cor->min1_y_);
      vgl_homg_point_2d<double> thp = trans1(hp);
      vgl_point_2d<double> thp_p(thp.x()/thp.w(), thp.y()/thp.w());
      vgl_point_2d<double> tthp_p = tps(thp_p);
      vgl_homg_point_2d<double> tthp(tthp_p.x(), tthp_p.y());
      vgl_homg_point_2d<double> hpp = inv2(tthp);
      //vgl_homg_point_2d<double> hpp = inv2(tps(trans1(hp)));

      vgl_point_2d<float> out_pt_float(float(hpp.x()/hpp.w()), float(hpp.y()/hpp.w()));
      vgl_point_2d<int> out_pt( (int)vcl_floor(out_pt_float.x()+0.5), (int)vcl_floor(out_pt_float.y()+0.5));
      
      if (out_pt.x() < cor->min2_x_ || out_pt.x() > cor->max2_x_ || 
          out_pt.y() < cor->min2_y_ || out_pt.y() > cor->max2_y_) 
          continue;

      if (cor->region2_[out_pt.x()-cor->min2_x_][out_pt.y()-cor->min2_y_] < 0) continue;

      cor->region1_map_output_[i][j] = out_pt;
      cor->region1_map_output_float_[i][j] = out_pt_float;
      vcl_pair<unsigned, unsigned> id_pair(static_cast<unsigned>(cor->region1_[i][j]), 
                                           static_cast<unsigned>(cor->region2_[out_pt.x()-cor->min2_x_][out_pt.y()-cor->min2_y_]));
      cor->correspondences_.push_back(id_pair);      
    }
  }

  return true;
}


//: find the intersection point according to the formula
// given 4 points (x1, y1) (x2, y2) defining line 1
//                (x3, y3) (x4, y4) defining line 2
// the intersection is given by
// x = x1 + ua (x2-x1)
// y = y1 + ub (y2-y1)
// where ua = [ (x4-x3)(y1-y3)-(y4-y3)(x1-x3) ] / [ (y4-y3)(x2-x1)-(x4-x3)(y2-y1) ] 
//       ub = [ (x2-x1)(y1-y3)-(y2-y1)(x1-x3) ] / [ (y4-y3)(x2-x1)-(x4-x3)(y2-y1) ] 
// if the denominator is zero, then the lines are parallel skip
bool dbru_rcor_generator::find_intersection(double x1, double y1, double x2, double y2,
                                            double x3, double y3, double x4, double y4,
                                            //vsol_point_2d_sptr pt1, vsol_point_2d_sptr pt2,
                                            // vsol_point_2d_sptr pt3, vsol_point_2d_sptr pt4,
                                            vgl_point_2d<int> *out_pt,
                                            vgl_point_2d<float> *out_pt_float)
{
  //double x1 = pt1->x(), y1 = pt1->y(), x2 = pt2->x(), y2 = pt2->y();
  //double x3 = pt3->x(), y3 = pt3->y(), x4 = pt4->x(), y4 = pt4->y();
  double den = (y4-y3)*(x2-x1)-(x4-x3)*(y2-y1);

  if (vcl_abs(den) < 1e-5) 
    return false;

  double ua = ((x4-x3)*(y1-y3)-(y4-y3)*(x1-x3))/den;
  double ub = ((x2-x1)*(y1-y3)-(y2-y1)*(x1-x3))/den;
  double int_x1 = x1 + ua*(x2-x1);
  double int_y1 = y1 + ub*(y2-y1);
          
  //: round the values to the nearest integer
  out_pt->set((int)vcl_floor(int_x1+0.5), (int)vcl_floor(int_y1+0.5));
  if (out_pt_float) 
    out_pt_float->set(float(int_x1), float(int_y1));

  return true;
}

bool dbru_rcor_generator::find_correspondence_line(dbru_rcor_sptr cor,
                                                   unsigned int i_increment)
{  
  if (!cor || cor->halt_) {
    vcl_cout << "Errors in constructor!\n";
    return false;
  }

  if (!cor->get_sil_cor()) {
    vcl_cout << "error in dbru_rcor_generator: Silhouete contour/polygon correspondence is not set!\n";
    return false;
  }

  //: prepare a vector to hold corresponding (x,y) in the second region and count
  cor->set_save_float(true);
  cor->initialize_float_map();
  cor->initialize_region1_histograms();

  cor->cnt1_ = 0;
  //cnt2_ = 0;
  vcl_vector<vgl_point_2d<double> >& pts1 = cor->get_sil_cor()->get_contour_pts1();
  vcl_vector<vgl_point_2d<double> >& pts2 = cor->get_sil_cor()->get_contour_pts2();
  vcl_cout << "size of corresponding points: " << pts1.size() << vcl_endl;
  assert(pts1.size() == pts2.size());

  for (unsigned i = 0; i<pts1.size(); i+=i_increment) {
    for (unsigned j = 0; j < pts1.size(); j++) {
      if (i == j) continue;
      if (pts1[i] == pts1[j] || pts2[i] == pts2[j]) continue;

      for (unsigned k = i+1; k<pts1.size(); k++) {
        for (unsigned m = 0; m < pts1.size(); m++) {
          if (k == m) continue;
          if (pts1[k] == pts1[m] || pts2[k] == pts2[m]) continue;

          vgl_point_2d<int> out_pt1;
          vgl_point_2d<float> out_pt1_float;
          if (!find_intersection(pts1[i].x(), pts1[i].y(), 
                                 pts1[j].x(), pts1[j].y(),
                                 pts1[k].x(), pts1[k].y(), 
                                 pts1[m].x(), pts1[m].y(), &out_pt1, &out_pt1_float))
            continue;

          if (out_pt1.x() < cor->min1_x_ || out_pt1.x() > cor->max1_x_ || 
              out_pt1.y() < cor->min1_y_ || out_pt1.y() > cor->max1_y_) 
              continue;
          
          if (cor->region1_[out_pt1.x()-cor->min1_x_][out_pt1.y()-cor->min1_y_] < 0) 
            continue;
          
          vgl_point_2d<int> out_pt2;
          vgl_point_2d<float> out_pt2_float;
          if (!find_intersection(pts2[i].x(), pts2[i].y(), 
                                 pts2[j].x(), pts2[j].y(),
                                 pts2[k].x(), pts2[k].y(), 
                                 pts2[m].x(), pts2[m].y(), &out_pt2, &out_pt2_float))
            continue;

          cor->cnt1_++;

          if (out_pt2.x() < cor->min2_x_ || out_pt2.x() > cor->max2_x_ || 
              out_pt2.y() < cor->min2_y_ || out_pt2.y() > cor->max2_y_) 
              continue;

          if (cor->region2_[out_pt2.x()-cor->min2_x_][out_pt2.y()-cor->min2_y_] < 0) continue;

          //cnt2_++;
     
          bool exists = false;
          int xxx = out_pt1.x() - cor->min1_x_, yyy = out_pt1.y() - cor->min1_y_;
          for (unsigned k = 0; k<cor->region1_histograms_[xxx][yyy].size(); k++) {
            vgl_point_2d<int> p = (cor->region1_histograms_[xxx][yyy][k]).first;
            if (p == out_pt2) {
              (cor->region1_histograms_[xxx][yyy][k]).second += 1;
              exists = true;
              break;
            }
          }
          if (!exists) {
            vcl_pair< vgl_point_2d<int>, int > p(out_pt2, 1);
            vcl_pair< vgl_point_2d<float>, int > p_float(out_pt2_float, 1);
            (cor->region1_histograms_[xxx][yyy]).push_back(p);
            (cor->region1_histograms_float_[xxx][yyy]).push_back(p_float);
          }
          
        }
      }

    }
  }

  cor->cnt3_ = 0;
  for (int i = 0; i<cor->upper_x_; i++) {
    for (int j = 0; j<cor->upper_y_; j++) {
      
      if (!(cor->region1_histograms_[i][j]).size()) continue;
      cor->cnt3_++;
      int cnt_max = (cor->region1_histograms_[i][j][0]).second;
      unsigned id_max = 0;
      for (unsigned k = 1; k < (cor->region1_histograms_[i][j]).size(); k++) 
        if (cnt_max < (cor->region1_histograms_[i][j][k]).second) {
          cnt_max = (cor->region1_histograms_[i][j][k]).second;
          id_max = k;
        }

      //region1_map_output_[i][j] = region1_map[i][j][id_max].first;
      vgl_point_2d<int> outp = (cor->region1_histograms_[i][j][id_max]).first;
      cor->region1_map_output_[i][j] = outp;
      cor->region1_map_output_float_[i][j] = (cor->region1_histograms_float_[i][j][id_max]).first;
      vcl_pair<unsigned, unsigned> p(static_cast<unsigned>(cor->region1_[i][j]), 
                                     static_cast<unsigned>(cor->region2_[outp.x()-cor->min2_x_][outp.y()-cor->min2_y_]));
      cor->correspondences_.push_back(p);
    }
  }

  if (!cor->save_histograms_)
    cor->clear_region1_histograms();

  vcl_cout << "tried: " << cor->cnt1_ << " intersections, correspondence: " << cor->cnt3_ << vcl_endl;
  return true;

}

//: this method uses the same algorithm in find_correspondence_line, 
//  however it uses arclength to select quads
bool dbru_rcor_generator::find_correspondence_line2(dbru_rcor_sptr cor, 
                                                    double ratio)
{
  if (!cor || cor->halt_) {
    vcl_cout << "Errors in constructor!\n";
    return false;
  }

  if (!cor->get_sil_cor()) {
    vcl_cout << "error in dbru_rcor_generator: Silhouete contour/polygon correspondence is not set!\n";
    return false;
  }

  dbcvr_cv_cor_sptr sil_cor = cor->get_sil_cor();
  vsol_polygon_2d_sptr poly0 = sil_cor->get_poly1();
  vsol_polygon_2d_sptr polyi = sil_cor->get_poly2();

  // create interpolated curves from sil_cor's polygons
  dbsol_interp_curve_2d_sptr curve0 = new dbsol_interp_curve_2d();
  dbsol_interp_curve_2d_sptr curvei = new dbsol_interp_curve_2d();
  dbsol_curve_algs::interpolate_linear(curve0.ptr(), poly0);
  dbsol_curve_algs::interpolate_linear(curvei.ptr(), polyi);

  // check if lengths of curves are the same
  vcl_cout << "length1 from sil_cor: " << sil_cor->get_length1() << " length2: " << sil_cor->get_length2() << vcl_endl;
  vcl_cout << "length1 from curve1: " << curve0->length() << " length2: " << curvei->length() << vcl_endl;

  cor->set_save_float(true);
  cor->initialize_region1_histograms();
  cor->cnt1_ = 0;
  
  //cnt2_ = 0;
  double length1 = sil_cor->get_length1();
  double delta_s = ratio*length1;
  double delta_s_ending = 3*delta_s;
  for (double s1_11 = 0; s1_11 < length1; s1_11 += delta_s) {
    vsol_point_2d_sptr pt1_11 = curve0->point_at(s1_11);
    
    double s2_11 = sil_cor->get_arclength_on_curve2(s1_11);
    vsol_point_2d_sptr pt2_11 = curvei->point_at(s2_11);

    double end_s12 = length1 + s1_11; 
    for (double s1_12 = s1_11 + delta_s_ending; s1_12 < end_s12; s1_12 += delta_s_ending) {
      double real_s1_12 = vcl_fmod(s1_12, length1); 
      vsol_point_2d_sptr pt1_12 = curve0->point_at(real_s1_12);
      
      double s2_12 = sil_cor->get_arclength_on_curve2(real_s1_12);
      vsol_point_2d_sptr pt2_12 = curvei->point_at(s2_12);
      
      for (double s1_21 = delta_s/2; s1_21<length1; s1_21 += delta_s) {
        vsol_point_2d_sptr pt1_21 = curve0->point_at(s1_21);

        double s2_21 = sil_cor->get_arclength_on_curve2(s1_21);
        vsol_point_2d_sptr pt2_21 = curvei->point_at(s2_21);

        double end_s22 = length1 + s1_21;
        for (double s1_22 = s1_21 + delta_s_ending; s1_22 < end_s22; s1_22 += delta_s_ending) {
          double real_s1_22 = vcl_fmod(s1_22, length1);
          vsol_point_2d_sptr pt1_22 = curve0->point_at(real_s1_22);
          
          double s2_22 = sil_cor->get_arclength_on_curve2(real_s1_22);
          vsol_point_2d_sptr pt2_22 = curvei->point_at(s2_22);

          vgl_point_2d<int> out_pt1;
          if (!find_intersection(pt1_11->x(), pt1_11->y(), 
                                 pt1_12->x(), pt1_12->y(),
                                 pt1_21->x(), pt1_21->y(), 
                                 pt1_22->x(), pt1_22->y(), &out_pt1))
            continue;

          if (out_pt1.x() < cor->min1_x_ || out_pt1.x() > cor->max1_x_ || 
              out_pt1.y() < cor->min1_y_ || out_pt1.y() > cor->max1_y_) 
              continue;
          
          if (cor->region1_[out_pt1.x()-cor->min1_x_][out_pt1.y()-cor->min1_y_] < 0) 
            continue;
          
          vgl_point_2d<int> out_pt2;
          vgl_point_2d<float> out_pt2_float;
          if (!find_intersection(pt2_11->x(), pt2_11->y(), 
                                 pt2_12->x(), pt2_12->y(),
                                 pt2_21->x(), pt2_21->y(), 
                                 pt2_22->x(), pt2_22->y(), &out_pt2, &out_pt2_float))
            continue;

          cor->cnt1_++;

          if (out_pt2.x() < cor->min2_x_ || out_pt2.x() > cor->max2_x_ || 
              out_pt2.y() < cor->min2_y_ || out_pt2.y() > cor->max2_y_) 
              continue;

          if (cor->region2_[out_pt2.x()-cor->min2_x_][out_pt2.y()-cor->min2_y_] < 0) continue;

          //cnt2_++;
     
          bool exists = false;
          int xxx = out_pt1.x() - cor->min1_x_, yyy = out_pt1.y() - cor->min1_y_;
          for (unsigned k = 0; k<cor->region1_histograms_[xxx][yyy].size(); k++) {
            vgl_point_2d<int> p = (cor->region1_histograms_[xxx][yyy][k]).first;
            if (p == out_pt2) {
              (cor->region1_histograms_[xxx][yyy][k]).second += 1;
              exists = true;
              break;
            }
          }
          if (!exists) {
            vcl_pair< vgl_point_2d<int>, int > p(out_pt2, 1);
            (cor->region1_histograms_[xxx][yyy]).push_back(p);

            vcl_pair< vgl_point_2d<float>, int > p_float(out_pt2_float, 1);
            (cor->region1_histograms_float_[xxx][yyy]).push_back(p_float);
          }
          
        }
      }

    }
  }

  cor->cnt3_ = 0;
  for (int i = 0; i<cor->upper_x_; i++) {
    for (int j = 0; j<cor->upper_y_; j++) {
      
      if (!(cor->region1_histograms_[i][j]).size()) continue;
      cor->cnt3_++;
      int cnt_max = (cor->region1_histograms_[i][j][0]).second;
      unsigned id_max = 0;
      for (unsigned k = 1; k < (cor->region1_histograms_[i][j]).size(); k++) 
        if (cnt_max < (cor->region1_histograms_[i][j][k]).second) {
          cnt_max = (cor->region1_histograms_[i][j][k]).second;
          id_max = k;
        }

      //region1_map_output_[i][j] = region1_map[i][j][id_max].first;
      vgl_point_2d<int> outp = (cor->region1_histograms_[i][j][id_max]).first;
      vgl_point_2d<float> outp_float = (cor->region1_histograms_float_[i][j][id_max]).first;
      cor->region1_map_output_[i][j] = outp;
      cor->region1_map_output_float_[i][j] = outp_float;
      vcl_pair<unsigned, unsigned> p(static_cast<unsigned>(cor->region1_[i][j]), 
                                     static_cast<unsigned>(cor->region2_[outp.x()-cor->min2_x_][outp.y()-cor->min2_y_]));
      cor->correspondences_.push_back(p);
    }
  }

  if (!cor->save_histograms_)
    cor->clear_region1_histograms();

  vcl_cout << "tried: " << cor->cnt1_ << " intersections, correspondence: " << cor->cnt3_ << vcl_endl;

  return true;

}

//: this method uses the same algorithm in find_correspondence_line, 
//  however it distributes increment on the selection of both starting and ending points of 
//  both line segment
bool dbru_rcor_generator::find_correspondence_line3(dbru_rcor_sptr cor, int increment) 
{
  if (!cor || cor->halt_) {
    vcl_cout << "Errors in constructor!\n";
    return false;
  }

  if (!cor->get_sil_cor()) {
    vcl_cout << "error in dbru_rcor_generator: Silhouete contour/polygon correspondence is not set!\n";
    return false;
  }

  //: prepare a vector to hold corresponding (x,y) in the second region and count
  //                                                     x2 , y2    cnt
  cor->set_save_float(true);
  cor->initialize_region1_histograms();

  cor->cnt1_ = 0;
  //cnt2_ = 0;
  vcl_vector<vgl_point_2d<double> >& pts1 = cor->get_sil_cor()->get_contour_pts1();
  vcl_vector<vgl_point_2d<double> >& pts2 = cor->get_sil_cor()->get_contour_pts2();
  vcl_cout << "size of corresponding points: " << pts1.size() << vcl_endl;
  assert(pts1.size() == pts2.size());

  for (unsigned i = 0; i < pts1.size(); i += increment) {
    for (unsigned j = 0; j < pts1.size(); j += increment) {
      if (i == j) continue;
      if (pts1[i] == pts1[j] || pts2[i] == pts2[j]) continue;

      for (unsigned k = unsigned(vcl_ceil(double(increment/2))); k < pts1.size(); k += increment) {
        for (unsigned m = unsigned(vcl_ceil(double(increment/2))); m < pts1.size(); m += increment) {
          if (k == m) continue;
          if (pts1[k] == pts1[m] || pts2[k] == pts2[m]) continue;

          vgl_point_2d<int> out_pt1;
          if (!find_intersection(pts1[i].x(), pts1[i].y(), 
                                 pts1[j].x(), pts1[j].y(),
                                 pts1[k].x(), pts1[k].y(), 
                                 pts1[m].x(), pts1[m].y(), &out_pt1))
            continue;

          if (out_pt1.x() < cor->min1_x_ || out_pt1.x() > cor->max1_x_ || 
              out_pt1.y() < cor->min1_y_ || out_pt1.y() > cor->max1_y_) 
              continue;
          
          if (cor->region1_[out_pt1.x()-cor->min1_x_][out_pt1.y()-cor->min1_y_] < 0) 
            continue;
          
          vgl_point_2d<int> out_pt2;
          vgl_point_2d<float> out_pt2_float;
          if (!find_intersection(pts2[i].x(), pts2[i].y(), 
                                 pts2[j].x(), pts2[j].y(),
                                 pts2[k].x(), pts2[k].y(), 
                                 pts2[m].x(), pts2[m].y(), &out_pt2, &out_pt2_float))
            continue;

          cor->cnt1_++;

          if (out_pt2.x() < cor->min2_x_ || out_pt2.x() > cor->max2_x_ || 
              out_pt2.y() < cor->min2_y_ || out_pt2.y() > cor->max2_y_) 
              continue;

          if (cor->region2_[out_pt2.x()-cor->min2_x_][out_pt2.y()-cor->min2_y_] < 0) continue;

          //cnt2_++;
     
          bool exists = false;
          int xxx = out_pt1.x() - cor->min1_x_, yyy = out_pt1.y() - cor->min1_y_;
          for (unsigned k = 0; k<cor->region1_histograms_[xxx][yyy].size(); k++) {
            vgl_point_2d<int> p = (cor->region1_histograms_[xxx][yyy][k]).first;
            if (p == out_pt2) {
              (cor->region1_histograms_[xxx][yyy][k]).second += 1;
              exists = true;
              break;
            }
          }
          if (!exists) {
            vcl_pair< vgl_point_2d<int>, int > p(out_pt2, 1);
            vcl_pair< vgl_point_2d<float>, int > p_float(out_pt2_float, 1);
            (cor->region1_histograms_[xxx][yyy]).push_back(p);
            (cor->region1_histograms_float_[xxx][yyy]).push_back(p_float);
          }
          
        }
      }

    }
  }

  cor->cnt3_ = 0;
  for (int i = 0; i<cor->upper_x_; i++) {
    for (int j = 0; j<cor->upper_y_; j++) {
      
      if (!(cor->region1_histograms_[i][j]).size()) continue;
      cor->cnt3_++;
      int cnt_max = (cor->region1_histograms_[i][j][0]).second;
      unsigned id_max = 0;
      for (unsigned k = 1; k < (cor->region1_histograms_[i][j]).size(); k++) 
        if (cnt_max < (cor->region1_histograms_[i][j][k]).second) {
          cnt_max = (cor->region1_histograms_[i][j][k]).second;
          id_max = k;
        }

      //region1_map_output_[i][j] = region1_map[i][j][id_max].first;
      vgl_point_2d<int> outp = (cor->region1_histograms_[i][j][id_max]).first;
      cor->region1_map_output_[i][j] = outp;
      cor->region1_map_output_float_[i][j] = (cor->region1_histograms_float_[i][j][id_max]).first;
      vcl_pair<unsigned, unsigned> p(static_cast<unsigned>(cor->region1_[i][j]), 
                                     static_cast<unsigned>(cor->region2_[outp.x()-cor->min2_x_][outp.y()-cor->min2_y_]));
      cor->correspondences_.push_back(p);
    }
  }

  if (!cor->save_histograms_)
    cor->clear_region1_histograms();

  vcl_cout << "tried: " << cor->cnt1_ << " intersections, correspondence: " << cor->cnt3_ << vcl_endl;
  return true;
}

//: this method goes from pixels to the contour via pairs of line segments
// that intersect on that pixel, 
// it rotates the two line segments by delta_theta each time to get a new quad
// delta_theta is determined by the total number of votes 
bool dbru_rcor_generator::find_correspondence_line4(dbru_rcor_sptr cor, 
                                                    int total_votes) 
{
  if (!cor || cor->halt_) {
    vcl_cout << "Errors in constructor!\n";
    return false;
  }

  if (!cor->get_sil_cor()) {
    vcl_cout << "error in dbru_rcor_generator: Silhouete contour/polygon correspondence is not set!\n";
    return false;
  }

  dbcvr_cv_cor_sptr sil_cor = cor->get_sil_cor();
  vsol_polygon_2d_sptr poly0 = sil_cor->get_poly1();
  vsol_polygon_2d_sptr polyi = sil_cor->get_poly2();

  vcl_cout << "creating edgel chains..\n";
  vdgl_edgel_chain_sptr chain1 = new vdgl_edgel_chain();
  for (unsigned int i = 0; i<poly0->size(); i++) {
    vdgl_edgel e(poly0->vertex(i)->x(), poly0->vertex(i)->y());
    chain1->add_edgel(e);
  }
  // add the last interval, since we have a closed curve
  vdgl_edgel e(poly0->vertex(0)->x(), poly0->vertex(0)->y());
  chain1->add_edgel(e);
  
  vdgl_interpolator_sptr interp1 = new vdgl_interpolator_linear(chain1);
  vdgl_digital_curve_sptr dc1 = new vdgl_digital_curve(interp1);

  vdgl_edgel_chain_sptr chain2 = new vdgl_edgel_chain();
  for (unsigned int i = 0; i<polyi->size(); i++) {
    vdgl_edgel e(polyi->vertex(i)->x(), polyi->vertex(i)->y());
    chain2->add_edgel(e);
  }
  // add the last interval, since we have a closed curve
  vdgl_edgel ei(polyi->vertex(0)->x(), polyi->vertex(0)->y());
  chain2->add_edgel(ei);

  vdgl_interpolator_sptr interp2 = new vdgl_interpolator_linear(chain2);
  vdgl_digital_curve_sptr dc2 = new vdgl_digital_curve(interp2);

  dbsol_interp_curve_2d_sptr curve11 = new dbsol_interp_curve_2d();
  dbsol_interp_curve_2d_sptr curve22 = new dbsol_interp_curve_2d();
  dbsol_curve_algs::interpolate_linear(curve11.ptr(), poly0);  // removed closed1, I want closed curves always
  dbsol_curve_algs::interpolate_linear(curve22.ptr(), polyi);

  //: prepare a vector to hold corresponding (x,y) in the second region and count
  //                                                     x2 , y2    cnt
  cor->initialize_region1_histograms();
  cor->cnt1_ = 0;

  double delta_theta = (vnl_math::pi_over_2)/double(total_votes);

  vgl_h_matrix_2d<double> HC, H1, H2, H3;

  //: go over each point in region1
  for (int i = 0; i<cor->upper_x_; i++) {
  //for (int i = 0; i<1; i++) {
    for (int j = 0; j<cor->upper_y_; j++) {
    //for (int j = 0; j<1; j++) {
      // if not a pixel inside the polygon continue
      if (cor->region1_[i][j] < 0) 
        continue;
      //vcl_cout << "pixel i: " << i << " j: " << j << vcl_endl;
      // create the vertical and horizontal lines that passes from this pixel
      vgl_homg_point_2d<double> p1(double(i+cor->min1_x_), double(j+cor->min1_y_));
      vgl_homg_point_2d<double> p2(double(i+cor->min1_x_), double(j+cor->min1_y_-1));
      vgl_homg_point_2d<double> p3(double(i+cor->min1_x_+1), double(j+cor->min1_y_));
      //vcl_cout << "p1:\n" << p1.x() << " " << p1.y() << vcl_endl; 
      
      H1.set_identity();
      H1.set_translation(-p1.x(), -p1.y());
      H3.set_identity();
      H3.set_translation(p1.x(), p1.y());
      H2.set_identity();
      H2.set_rotation(delta_theta);
      HC = H3*H2*H1;
      vgl_homg_point_2d<double> p2_r(p2);
      //vgl_homg_point_2d<double> p3_r(p3);
      vgl_homg_point_2d<double> p3_r(p2);

      //for (double theta = 0; theta < vnl_math::pi_over_2; theta += delta_theta) {
      //for (int kk = 0; kk < total_votes; kk++, p2_r = HC(p2_r), p3_r = HC(p3_r) ) {
      for (int kk = 0; kk < total_votes; kk++, p2_r = HC(p2_r) ) {
        p3_r = HC(p2_r);  // rotate p3 once ahead of p2_r
        for (int mm = 0; mm < total_votes-2; mm++, p3_r = HC(p3_r)) {  // rotate p3 2*pi-delta_theta
        
        //vcl_cout << "\ttheta: " << theta << vcl_endl;
        // arclengths on curve1 and curve2
        double s1[2][2] = { {-1, -1}, {-1, -1} };
        double s2[2][2] = { {-1, -1}, {-1, -1} };

        // rotations is performed at the end of the loop, so that first p2_r used is original p2
        vgl_point_2d<double> p2_rr(p2_r.x(), p2_r.y());
        vgl_point_2d<double> p3_rr(p3_r.x(), p3_r.y());

        vcl_vector<double> indices1;
        vgl_line_2d<double> dummy_line(p1, p2_rr);
        if (bdgl_curve_algs::intersect_line_fast(dc1, dummy_line, indices1) && indices1.size() == 2)
        {
          //vcl_cout << "indices1: ";
          for (unsigned k = 0; k<indices1.size(); k++) {
            //vcl_cout << indices1[k] << " pt from dc1: ";
            //vcl_cout << dc1->get_x(indices1[k]) << " " << dc1->get_y(indices1[k]) << " pt from curve11: ";
            
            // we need length at double indices, these curves are linear interpolations
            // so just interpolate length linearly
            double index = (dc1->n_pts()-1)*indices1[k];
            int int_index = int(vcl_floor(index));
            s1[0][k] = curve11->length_at(int_index);
            s1[0][k] += (index-int_index)*(curve11->length_at(int_index+1)-s1[0][k]);
            //vsol_point_2d_sptr pt = curve11->point_at(s);
            //vcl_cout << pt->x() << " " << pt->y() << vcl_endl;
          }
          //vcl_cout << vcl_endl;
        }

        if (s1[0][0] < 0 || s1[0][1] < 0)
          continue;

        vcl_vector<double> indices2;
        vgl_line_2d<double> dummy_line2(p1, p3_rr);
        if (bdgl_curve_algs::intersect_line_fast(dc1, dummy_line2, indices2) && indices2.size() == 2)
        {
          for (unsigned k = 0; k<indices2.size(); k++) {
            double index = (dc1->n_pts()-1)*indices2[k];
            int int_index = int(vcl_floor(index));
            s1[1][k] = curve11->length_at(int_index);
            s1[1][k] += (index-int_index)*(curve11->length_at(int_index+1)-s1[1][k]);
          }
        }

        if (s1[1][0] >= 0 && s1[1][1] >= 0) {
          s2[0][0] = sil_cor->get_arclength_on_curve2(s1[0][0]);
          s2[0][1] = sil_cor->get_arclength_on_curve2(s1[0][1]);
          s2[1][0] = sil_cor->get_arclength_on_curve2(s1[1][0]);
          s2[1][1] = sil_cor->get_arclength_on_curve2(s1[1][1]);

          if (s2[0][0] < 0 || s2[0][1] < 0 || s2[1][0] < 0 || s2[1][1] < 0)
            continue;

          vsol_point_2d_sptr pt2_11 = curve22->point_at(s2[0][0]);
          vsol_point_2d_sptr pt2_12 = curve22->point_at(s2[0][1]);
          vsol_point_2d_sptr pt2_21 = curve22->point_at(s2[1][0]);
          vsol_point_2d_sptr pt2_22 = curve22->point_at(s2[1][1]);

          vgl_point_2d<int> out_pt2;
          if (!find_intersection(pt2_11->x(), pt2_11->y(), 
                                 pt2_12->x(), pt2_12->y(),
                                 pt2_21->x(), pt2_21->y(), 
                                 pt2_22->x(), pt2_22->y(), &out_pt2))
            continue;

          cor->cnt1_++;

          if (out_pt2.x() < cor->min2_x_ || out_pt2.x() > cor->max2_x_ || 
              out_pt2.y() < cor->min2_y_ || out_pt2.y() > cor->max2_y_) 
              continue;

          if (cor->region2_[out_pt2.x()-cor->min2_x_][out_pt2.y()-cor->min2_y_] < 0) continue;

          //cnt2_++;
          //vcl_cout << "\t\tvalid intersection on region2\n";
          bool exists = false;
          for (unsigned k = 0; k<cor->region1_histograms_[i][j].size(); k++) {
            vgl_point_2d<int> p = (cor->region1_histograms_[i][j][k]).first;
            if (p == out_pt2) {
              (cor->region1_histograms_[i][j][k]).second += 1;
              exists = true;
              break;
            }
          }
          if (!exists) {
            vcl_pair< vgl_point_2d<int>, int > p(out_pt2, 1);
            (cor->region1_histograms_[i][j]).push_back(p);
          }

        }

        
        } // for mm
      }  // for theta
      
    }
  }

  cor->cnt3_ = 0;
  for (int i = 0; i<cor->upper_x_; i++) {
    for (int j = 0; j<cor->upper_y_; j++) {
      
      if (!(cor->region1_histograms_[i][j]).size()) continue;
      cor->cnt3_++;
      int cnt_max = (cor->region1_histograms_[i][j][0]).second;
      unsigned id_max = 0;
      for (unsigned k = 1; k < (cor->region1_histograms_[i][j]).size(); k++) 
        if (cnt_max < (cor->region1_histograms_[i][j][k]).second) {
          cnt_max = (cor->region1_histograms_[i][j][k]).second;
          id_max = k;
        }

      //region1_map_output_[i][j] = region1_map[i][j][id_max].first;
      vgl_point_2d<int> outp = (cor->region1_histograms_[i][j][id_max]).first;
      cor->region1_map_output_[i][j] = outp;
      vcl_pair<unsigned, unsigned> p(static_cast<unsigned>(cor->region1_[i][j]), 
                                     static_cast<unsigned>(cor->region2_[outp.x()-cor->min2_x_][outp.y()-cor->min2_y_]));
      cor->correspondences_.push_back(p);
    }
  }

  if (!cor->save_histograms_)
    cor->clear_region1_histograms();

  vcl_cout << "tried: " << cor->cnt1_ << " intersections, correspondence: " << cor->cnt3_ << vcl_endl;
  return true;
}
  
//: return the distance to the nearest polygon edge, together with arclength 
//  of that nearest point on the polygon
vcl_pair<double, double> dbru_rcor_generator::dt(vgl_polygon<double> const& poly, double x, double y) 
{
  //: number of sheets should be 1
  //vcl_cout << "Number of sheets in the polygon: " << poly.num_sheets() << vcl_endl;

  unsigned int n = poly[0].size();
  assert( n > 1 );
 
  double min_d = vgl_distance_to_linesegment(poly[0][n-1].x(), poly[0][n-1].y(),
                                             poly[0][0  ].x(), poly[0][0  ].y(),
                                             x, y);
  int min_s = 0;
  unsigned min_id = n-1;

  for ( unsigned int s=0; s < poly.num_sheets(); s++)
  {
    n = poly[s].size();
    assert( n > 1 );
   
    for ( unsigned int i=0, j = n-1; i < n; j = i++)
    {
      double nd = vgl_distance_to_linesegment(poly[s][j].x(), poly[s][j].y(),
                                              poly[s][i].x(), poly[s][i].y(),
                                              x, y);
      if ( nd<min_d ) {
        min_d=nd;
        min_s = s;
        min_id = j;
      }

    }
  }
  
  double ret_x, ret_y;
  vgl_closest_point_to_linesegment(ret_x,ret_y, 
                                   poly[min_s][min_id].x(),poly[min_s][min_id].y(), 
                                   poly[min_s][(min_id+1)%n].x(),poly[min_s][(min_id+1)%n].y(), 
                                   x, y);

  //: arclength is just the addition of linear arc lengths from the start up to this point
  double arc_length = 0;
  for (unsigned i = 0; i<min_id; i++) {
    arc_length += (poly[min_s][i+1]-poly[min_s][i]).length();
  }
  vgl_point_2d<double> p(ret_x, ret_y);
  arc_length += (p-poly[min_s][min_id]).length();

  return vcl_pair<double, double> (arc_length, min_d);
}

//: find region correspondence based on distance transform
bool dbru_rcor_generator::find_correspondence_dt(dbru_rcor_sptr cor,
                                                 dbsol_interp_curve_2d_sptr curve1,
                                                 dbsol_interp_curve_2d_sptr curve2,
                                                 float scale) 
{
  if (!cor || cor->halt_) {
    vcl_cout << "Errors in constructor!\n";
    return false;
  }

  if (!cor->get_sil_cor()) {
    vcl_cout << "error in dbru_rcor_generator: Silhouete contour/polygon correspondence is not set!\n";
    return false;
  }
  
  //: prepare a vector to hold distance transform of the first region
  vcl_vector<vcl_vector < vcl_pair<double, double> > > region1_dt;
  for (int i = 0; i<cor->upper_x_; i++) {
    vcl_vector< vcl_pair<double, double> > tmp(cor->upper_y_);
    region1_dt.push_back(tmp);
  }

  for (int i = 0; i<cor->upper_x_; i++) {
    for (int j = 0; j<cor->upper_y_; j++) {
      if (cor->region1_[i][j] < 0) {
        vcl_pair<double, double> p(-1, -1);
        region1_dt[i][j] = p;
        continue;
      }
      vcl_pair<double, double> p = dt(cor->p1_, (double)(i+cor->min1_x_), (double)(j+cor->min1_y_));
      region1_dt[i][j] = p;
    }
  }

  vcl_vector<vgl_point_2d<double> >& pts1 = cor->get_sil_cor()->get_contour_pts1();
  vcl_vector<vgl_point_2d<double> >& pts2 = cor->get_sil_cor()->get_contour_pts2();
  assert(pts1.size() == pts2.size());

  // find s's of pts1 and pts2
  vcl_vector<double> arclengths1, arclengths2;
  for (unsigned i = 0; i<pts1.size(); i++) {
    vcl_pair<double, double> p = dt(cor->p1_, pts1[i].x(), pts1[i].y());
    if (p.second < SMALL) {  // if distance to the contour is sufficiently small
      //vcl_cout << "Point: " << *(pts1[i]) << " is correctly on polygon 1 with arclength: " << p.first << vcl_endl; 
      arclengths1.push_back(p.first);   // push the arclength
    } else {
      vcl_cout << "Point: " << (pts1[i]) << " is NOT on polygon 1 with d: " << p.second << " and arclength: " << p.first << vcl_endl; 
      return false;
    }
  }
  for (unsigned i = 0; i<pts2.size(); i++) {
    vcl_pair<double, double> p = dt(cor->p2_, pts2[i].x(), pts2[i].y());
    if (p.second < SMALL) {  // if distance to the contour is sufficiently small
      //vcl_cout << "Point: " << *(pts2[i]) << " is correctly on polygon 2 with arclength: " << p.first << vcl_endl; 
      arclengths2.push_back(p.first);
    } else {
      vcl_cout << "Point: " << pts2[i] << " is NOT on polygon 2 with d: " << p.second << " and arclength: " << p.first << vcl_endl; 
      return false;
    }
  }

  double len1 = curve1->length();
  double len2 = curve2->length();
  cor->cnt3_ = 0;
  //: for each region pixel
  //  find its s on contour 1 and find corresponding s on the second contour using corresponding points
  for (int i = 0; i<cor->upper_x_; i++) {
    for (int j = 0; j<cor->upper_y_; j++) {
      
      if (region1_dt[i][j].first < 0) continue;
      double s = region1_dt[i][j].first;
      double d = region1_dt[i][j].second;

      // search for s in vector of arclengths  (not necessarily sorted array!!)
      double s_interval_dif1 = 0;
      double s_dif1 = 0;
      unsigned min_id = 0;
      for (unsigned k = 1; k<arclengths1.size(); k++) {
        if (arclengths1[k] - arclengths1[k-1] < 0) { // not a normal interval
          if (s >= arclengths1[k-1]) { 
            s_interval_dif1 = len1-arclengths1[k-1] + arclengths1[k];
            s_dif1 = s - arclengths1[k-1];
            min_id = k;
            break;
          } 
          if (s <= arclengths1[k]) {
            s_interval_dif1 = len1-arclengths1[k-1] + arclengths1[k];
            s_dif1 = len1-arclengths1[k-1] + s;
            min_id = k;
            break;
          }
        } else {  // normal interval
          if (s >= arclengths1[k-1] && s <= arclengths1[k]) {
            s_interval_dif1 = arclengths1[k] - arclengths1[k-1];
            s_dif1 = s - arclengths1[k-1];
            min_id = k;
            break;
          }
        }
      }                 
      unsigned min_id_p = min_id - 1;
      //: if nothing is assigned then interval is [0, arclengths1.size()-1]
      if (min_id == 0) {
        if (arclengths1[0] > arclengths1[arclengths1.size()-1]) {
          s_interval_dif1 = arclengths1[0] - arclengths1[arclengths1.size()-1];
          s_dif1 = s - arclengths1[arclengths1.size()-1];
        } else {
          s_interval_dif1 = len1 - arclengths1[arclengths1.size()-1] + arclengths1[0];
          if (s >= arclengths1[arclengths1.size()-1])
            s_dif1 = s - arclengths1[arclengths1.size()-1];
          if (s <= arclengths1[0])
            s_dif1 = len1-arclengths1[arclengths1.size()-1] + s;
        }
        min_id_p = arclengths1.size()-1;
      }

      //: find the arclength difference of the corresponding points and
      //  assume correspondence is linear (the diagonal in alignment curve space)
      double s_interval_dif2;
      if (arclengths2[min_id]-arclengths2[min_id_p] < 0) { // not a normal interval
        s_interval_dif2 = len2-arclengths2[min_id_p] + arclengths2[min_id];
      } else {  // normal interval
        s_interval_dif2 = arclengths2[min_id] - arclengths2[min_id_p];
      }

      if (!(s_interval_dif1 > 0)) continue;
      double s2 = vcl_fmod(arclengths2[min_id_p] + s_dif1*(s_interval_dif2/s_interval_dif1), len2);
      //vcl_cout << "len2: " << len2 << " s2 on curve 2: " << s2 << vcl_endl;
      if (!(s2 >= 0 && s2 <= len2)) continue;
      vsol_point_2d_sptr pt = curve2->point_at(s2);

      // go perpendicular into region2 from this point by scale*d
      double theta = curve2->tangent_angle_at(s2);  
      double ret_x = pt->x()+scale*(d*sin(theta));
      double ret_y = pt->y()-scale*(d*cos(theta));
      int ret_xx = (int)vcl_floor(ret_x+0.5);
      int ret_yy = (int)vcl_floor(ret_y+0.5);
      if (ret_xx < cor->min2_x_ || ret_yy < cor->min2_y_ || ret_xx > cor->max2_x_ || ret_yy > cor->max2_y_ || 
        cor->region2_[ret_xx-cor->min2_x_][ret_yy-cor->min2_y_] < 0) {  // try going in the other direction
        ret_x = pt->x()-scale*(d*sin(theta));
        ret_y = pt->y()+scale*(d*cos(theta));
        ret_xx = (int)vcl_floor(ret_x+0.5);
        ret_yy = (int)vcl_floor(ret_y+0.5);
        if (ret_xx < cor->min2_x_ || ret_yy < cor->min2_y_ || ret_xx > cor->max2_x_ || ret_yy > cor->max2_y_ || 
          cor->region2_[ret_xx-cor->min2_x_][ret_yy-cor->min2_y_] < 0) {  // if this is also not in region, just continue
          continue;                                                       // this pixel in region1 is not assigned a correspondant
        }
      }

      cor->cnt3_++;
      cor->region1_map_output_[i][j].set(ret_xx, ret_yy);
      cor->region1_map_output_float_[i][j].set(float(ret_x), float(ret_y));
      vcl_pair<unsigned, unsigned> p(static_cast<unsigned>(cor->region1_[i][j]), 
                                     static_cast<unsigned>(cor->region2_[ret_xx-cor->min2_x_][ret_yy-cor->min2_y_]));
      cor->correspondences_.push_back(p);
    }
  }

#if 0   // the following code is to display distance transform
  cor->cnt3_ = 0;
  for (int i = 0; i<cor->upper_x_; i++) {
    for (int j = 0; j<cor->upper_y_; j++) {
      
      if (region1_dt[i][j].first < 0) continue;
      cor->cnt3_++;
      double d = region1_dt[i][j].second;
      vgl_point_2d<int> pt(vcl_fmod( (vcl_floor(d+0.5)*10000 ), cor->upper2_x_ ), 
                           vcl_fmod( (vcl_floor(d+0.5)*10000 ), cor->upper2_y_ ) );
      cor->region1_map_output_[i][j] = pt;
    }
  }
#endif

  return true;
}

//: find region correspondence based on distance transform 
//  VERSION 2
//  This version is symetric and prevents overwrite problem of previous version
//  via arclength test
//  Additional steps:
//  1) compute dt of second region
//  2) given p in region1, find (s,d) on curve1 (s is arclength of nearest point on contour and its distance d), 
//     get corresponding s2 = alpha(s) on curve2
//     get p2 on region2 which is point_at(s2)+Normal_at(s2)  (go along normal direction into region2 from contour point)
//     get (s22, d22) from distance transform of region 2 for p2
//     test if s22 ~ s2 (i.e. |s22-s2|< epsilon, where epsilon = ratio*length of curve2)
//     if not then discard this correspondence
//  This test also prevents tangent problem to some extent
//  tangent problem: tangent direction hence normal direction is not
//  defined uniquely at exact sample points of the polygon
//  3) Also find correspondences going from region2 into region1 and add to the correspondence set
bool dbru_rcor_generator::find_correspondence_dt2(dbru_rcor_sptr cor,
                                                  dbsol_interp_curve_2d_sptr curve1,
                                                  dbsol_interp_curve_2d_sptr curve2,
                                                  float scale, float ratio)
{
  if (!cor || cor->halt_) {
    vcl_cout << "Errors in constructor!\n";
    return false;
  }

  if (!cor->get_sil_cor()) {
    vcl_cout << "error in dbru_rcor_generator: Silhouete contour/polygon correspondence is not set!\n";
    return false;
  }
  
  //: prepare a vector to hold distance transform of regions
  vcl_vector<vcl_vector < vcl_pair<double, double> > > region1_dt, region2_dt;
  for (int i = 0; i<cor->upper_x_; i++) {
    vcl_vector< vcl_pair<double, double> > tmp(cor->upper_y_);
    region1_dt.push_back(tmp);
  }
  for (int i = 0; i<cor->upper2_x_; i++) {
    vcl_vector< vcl_pair<double, double> > tmp(cor->upper2_y_);
    region2_dt.push_back(tmp);
  }

  for (int i = 0; i<cor->upper_x_; i++) {
    for (int j = 0; j<cor->upper_y_; j++) {
      if (cor->region1_[i][j] < 0) {
        vcl_pair<double, double> p(-1, -1);
        region1_dt[i][j] = p;
      } else {
        vcl_pair<double, double> p = dt(cor->p1_, (double)(i+cor->min1_x_), (double)(j+cor->min1_y_));
        region1_dt[i][j] = p;
      }
    }
  }
  for (int i = 0; i<cor->upper2_x_; i++) {
    for (int j = 0; j<cor->upper2_y_; j++) {
      if (cor->region2_[i][j] < 0) {  // not inside region
        vcl_pair<double, double> p(-1, -1);
        region2_dt[i][j] = p;
      } else {
        vcl_pair<double, double> p = dt(cor->p2_, (double)(i+cor->min2_x_), (double)(j+cor->min2_y_));
        region2_dt[i][j] = p;
      }
    }
  }

  dbcvr_cv_cor_sptr sil_cor = cor->get_sil_cor();
  
  double len1 = curve1->length();
  double len2 = curve2->length();
  
  //: epsilon is used in arclength test
  double epsilon = ratio*len2;
  
  cor->set_save_float(true);
  cor->initialize_float_map();

  cor->cnt3_ = 0;
  //: for each region pixel
  //  find its s on contour 1 and find corresponding s on the second contour using corresponding points
  for (int i = 0; i<cor->upper_x_; i++) {
    for (int j = 0; j<cor->upper_y_; j++) {
      
      if (region1_dt[i][j].first < 0) continue;
      double s = region1_dt[i][j].first;
      double d = region1_dt[i][j].second;

      // get linearly interpolated correspondence from sillhouette correspondence
      double s2 = sil_cor->get_arclength_on_curve2(s);

      //vcl_cout << "len2: " << len2 << " s2 on curve 2: " << s2 << vcl_endl;
      if (!(s2 >= 0 && s2 <= len2)) continue;
      vsol_point_2d_sptr pt = curve2->point_at(s2);

      // go perpendicular into region2 from this point by scale*d
      double theta = curve2->tangent_angle_at(s2);  
      double ret_x = pt->x()+scale*(d*sin(theta));
      double ret_y = pt->y()-scale*(d*cos(theta));
      int ret_xx = (int)vcl_floor(ret_x+0.5);
      int ret_yy = (int)vcl_floor(ret_y+0.5);
      if (ret_xx < cor->min2_x_ || ret_yy < cor->min2_y_ || 
          ret_xx > cor->max2_x_ || ret_yy > cor->max2_y_ || 
          cor->region2_[ret_xx-cor->min2_x_][ret_yy-cor->min2_y_] < 0) {  
        //: try going in the other direction
        ret_x = pt->x()-scale*(d*sin(theta));
        ret_y = pt->y()+scale*(d*cos(theta));
        ret_xx = (int)vcl_floor(ret_x+0.5);
        ret_yy = (int)vcl_floor(ret_y+0.5);
        if (ret_xx < cor->min2_x_ || ret_yy < cor->min2_y_ || 
            ret_xx > cor->max2_x_ || ret_yy > cor->max2_y_ || 
            cor->region2_[ret_xx-cor->min2_x_][ret_yy-cor->min2_y_] < 0) {  // if this is also not in region, just continue
          continue;                                                       // this pixel in region1 is not assigned a correspondant
        }
      }

      // at this point we know (ret_xx, ret_yy) is inside region2
      // apply ARCLENGTH TEST: discard if s22 is far from s2
      
      double s22 = region2_dt[ret_xx-cor->min2_x_][ret_yy-cor->min2_y_].first;
      if (s22 < 0) continue;  // this point was not valid to begin with!!!
                              // this should not happen if previous tests were ok

      //: the following is the actual arclength test
      if (vcl_abs(s22-s2) > epsilon)
        continue;

      cor->cnt3_++;
      cor->region1_map_output_[i][j].set(ret_xx, ret_yy);
      cor->region1_map_output_float_[i][j].set(float(ret_x), float(ret_y));
      vcl_pair<unsigned, unsigned> p(static_cast<unsigned>(cor->region1_[i][j]), 
                                     static_cast<unsigned>(cor->region2_[ret_xx-cor->min2_x_][ret_yy-cor->min2_y_]));
      cor->correspondences_.push_back(p);
    }
  }
  // do this again for symmetry
  scale = 1.0f/scale;
  epsilon = ratio*len1;

  vcl_cout << "cor # before symetry: " << cor->cnt3_ << vcl_endl;

  //: for each region2 pixel
  //  find its s on contour 1 and find corresponding s on the second contour using corresponding points
  for (int i = 0; i<cor->upper2_x_; i++) {
    for (int j = 0; j<cor->upper2_y_; j++) {
      
      if (region2_dt[i][j].first < 0) continue;
      double s = region2_dt[i][j].first;
      double d = region2_dt[i][j].second;

      // get linearly interpolated correspondence from sillhouette correspondence
      double s1 = sil_cor->get_arclength_on_curve1(s);

      //vcl_cout << "len2: " << len2 << " s2 on curve 2: " << s2 << vcl_endl;
      if (!(s1 >= 0 && s1 <= len1)) continue;
      vsol_point_2d_sptr pt = curve1->point_at(s1);

      // go perpendicular into region1 from this point by (1/scale)*d
      double theta = curve1->tangent_angle_at(s1);  
      double ret_x = pt->x()+scale*(d*sin(theta));
      double ret_y = pt->y()-scale*(d*cos(theta));
      int ret_xx = (int)vcl_floor(ret_x+0.5);
      int ret_yy = (int)vcl_floor(ret_y+0.5);
      if (ret_xx < cor->min1_x_ || ret_yy < cor->min1_y_ || 
          ret_xx > cor->max1_x_ || ret_yy > cor->max1_y_ || 
          cor->region1_[ret_xx-cor->min1_x_][ret_yy-cor->min1_y_] < 0) {  
        //: try going in the other direction
        ret_x = pt->x()-scale*(d*sin(theta));
        ret_y = pt->y()+scale*(d*cos(theta));
        ret_xx = (int)vcl_floor(ret_x+0.5);
        ret_yy = (int)vcl_floor(ret_y+0.5);
        if (ret_xx < cor->min1_x_ || ret_yy < cor->min1_y_ || 
            ret_xx > cor->max1_x_ || ret_yy > cor->max1_y_ || 
            cor->region1_[ret_xx-cor->min1_x_][ret_yy-cor->min1_y_] < 0) {  // if this is also not in region, just continue
          continue;                                                       // this pixel in region2 is not assigned a correspondant
        }
      }

      // at this point we know (ret_xx, ret_yy) is inside region1
      // apply ARCLENGTH TEST: discard if s11 is far from s1
      
      double s11 = region1_dt[ret_xx-cor->min1_x_][ret_yy-cor->min1_y_].first;
      if (s11 < 0) continue;  // this point was not valid to begin with!!!
                              // this should not happen if previous tests were ok

      //: the following is the actual arclength test
      if (vcl_abs(s11-s1) > epsilon)
        continue;

      // test if this point was already assigned
      if (cor->region1_map_output_[ret_xx-cor->min1_x_][ret_yy-cor->min1_y_].x() >= 0)
        continue;

      cor->cnt3_++;
      cor->region1_map_output_[ret_xx-cor->min1_x_][ret_yy-cor->min1_y_].set(i+cor->min2_x_, j+cor->min2_y_);
      cor->region1_map_output_float_[ret_xx-cor->min1_x_][ret_yy-cor->min1_y_].set(float(i+cor->min2_x_), float(j+cor->min2_y_));
      vcl_pair<unsigned, unsigned> p(static_cast<unsigned>(cor->region1_[ret_xx-cor->min1_x_][ret_yy-cor->min1_y_]), 
                                     static_cast<unsigned>(cor->region2_[i][j]));
      cor->correspondences_.push_back(p);
    }
  }

  vcl_cout << "cor # after symetry: " << cor->cnt3_ << vcl_endl;

  return true;
}


void dbru_rcor_generator::add_to_map(dbru_rcor_sptr cor,
                                     vgl_point_2d<double> r1_pt_p, vgl_point_2d<double> r2_pt_p, 
                                     vgl_point_2d<double> r1_pt_m, vgl_point_2d<double> r2_pt_m) {
  
  //: round the values to the nearest integer
  //  shift shock coordinates wrt to cropped image
  vgl_point_2d<int> r1_pt( (int)vcl_floor(r1_pt_p.x()+0.5), (int)vcl_floor(r1_pt_p.y()+0.5) );
  vgl_point_2d<int> r2_pt( (int)vcl_floor(r2_pt_p.x()+0.5), (int)vcl_floor(r2_pt_p.y()+0.5) );

  if (!(r1_pt.x() < cor->min1_x_ || r1_pt.x() > cor->max1_x_ || 
        r1_pt.y() < cor->min1_y_ || r1_pt.y() > cor->max1_y_)  &&
        cor->region1_[r1_pt.x()-cor->min1_x_][r1_pt.y()-cor->min1_y_] >= 0 &&
       !(r2_pt.x() < cor->min2_x_ || r2_pt.x() > cor->max2_x_ || 
         r2_pt.y() < cor->min2_y_ || r2_pt.y() > cor->max2_y_)  &&
         cor->region2_[r2_pt.x()-cor->min2_x_][r2_pt.y()-cor->min2_y_] >= 0) 
  {
       vgl_point_2d<int> &p = cor->region1_map_output_[r1_pt.x()-cor->min1_x_][r1_pt.y()-cor->min1_y_];        
       if (p.x() < 0 || p.y() < 0) {
            cor->region1_map_output_[r1_pt.x()-cor->min1_x_][r1_pt.y()-cor->min1_y_].set(r2_pt.x(), r2_pt.y());
            cor->region1_map_output_float_[r1_pt.x()-cor->min1_x_][r1_pt.y()-cor->min1_y_].set(float(r2_pt_p.x()), float(r2_pt_p.y()));
            cor->correspondences_.push_back(vcl_pair<unsigned, unsigned>(
                static_cast<unsigned>(cor->region1_[r1_pt.x()-cor->min1_x_][r1_pt.y()-cor->min1_y_]),
                static_cast<unsigned>(cor->region2_[r2_pt.x()-cor->min2_x_][r2_pt.y()-cor->min2_y_])
                                                               ));
       }
  }

  //: do the same for the minus boundary points
  r1_pt.set( (int)vcl_floor(r1_pt_m.x()+0.5), (int)vcl_floor(r1_pt_m.y()+0.5) );
  r2_pt.set( (int)vcl_floor(r2_pt_m.x()+0.5), (int)vcl_floor(r2_pt_m.y()+0.5) );

  if (!(r1_pt.x() < cor->min1_x_ || r1_pt.x() > cor->max1_x_ || 
    r1_pt.y() < cor->min1_y_ || r1_pt.y() > cor->max1_y_)  &&
    cor->region1_[r1_pt.x()-cor->min1_x_][r1_pt.y()-cor->min1_y_] >= 0 &&
    !(r2_pt.x() < cor->min2_x_ || r2_pt.x() > cor->max2_x_ || 
      r2_pt.y() < cor->min2_y_ || r2_pt.y() > cor->max2_y_)  &&
      cor->region2_[r2_pt.x()-cor->min2_x_][r2_pt.y()-cor->min2_y_] >= 0 ) 
  {
    vgl_point_2d<int> &p = cor->region1_map_output_[r1_pt.x()-cor->min1_x_][r1_pt.y()-cor->min1_y_];        
    if (p.x() < 0 || p.y() < 0) {
        cor->region1_map_output_[r1_pt.x()-cor->min1_x_][r1_pt.y()-cor->min1_y_].set(r2_pt.x(), r2_pt.y());
        cor->region1_map_output_float_[r1_pt.x()-cor->min1_x_][r1_pt.y()-cor->min1_y_].set(float(r2_pt_m.x()), float(r2_pt_m.y()));
        cor->correspondences_.push_back(vcl_pair<unsigned, unsigned> (
                static_cast<unsigned>(cor->region1_[r1_pt.x()-cor->min1_x_][r1_pt.y()-cor->min1_y_]),
                static_cast<unsigned>(cor->region2_[r2_pt.x()-cor->min2_x_][r2_pt.y()-cor->min2_y_])
                                                                      ));
    }
  }
  
  return;
}

//: find correspondence of image regions based on correspondence of shock branches
bool dbru_rcor_generator::find_correspondence_shock(dbru_rcor_sptr cor) {

  if (!cor || cor->halt_) {
    vcl_cout << "Errors in constructor!\n";
    return false;
  }

  if (!cor->get_sm_cor()) {
    vcl_cout << "error in dbru_rcor_generator: Shock graph correspondence is not set!\n";
    return false;
  }

  cor->set_save_float(true);
  cor->initialize_float_map();

  vcl_vector<dbskr_scurve_sptr>& curve_list1 = cor->get_sm_cor()->get_curve_list1();
  vcl_vector<dbskr_scurve_sptr>& curve_list2 = cor->get_sm_cor()->get_curve_list2();
  vcl_vector<vcl_vector < vcl_pair <int,int> > >& map_list = cor->get_sm_cor()->get_map_list();

  //: go along shock curves from the lists one by one
  if ((curve_list1.size() != curve_list2.size()) || (curve_list1.size() != map_list.size())) {
    vcl_cout << "different sizes in shock curve correspondence, not able to find region correspondence!\n";
  } else {  // corresponding shock curves

    double step_size = 1.0;

    for (unsigned int i = 0; i<curve_list1.size(); i++) {
      dbskr_scurve_sptr sc1 = curve_list1[i];
      dbskr_scurve_sptr sc2 = curve_list2[i];
      vcl_vector< vcl_pair<int, int> > mapl = map_list[i];

      for (unsigned int j = mapl.size()-1; j>0; j--) {
        int k = mapl[j].first;
        int m = mapl[j].second;
        int k_next = mapl[j-1].first;
        int m_next = mapl[j-1].second;

        //: we want 5 times more samples
        int k_dif = k_next-k;
        int m_dif = m_next-m;
        int N = 10 * ( k_dif > m_dif ? k_dif : m_dif);
        
        for (int n = 0; n<N; n++) { 
          
          double ratio_n = double(n)/N;
          double kk = k + ratio_n*k_dif;
          double mm = m + ratio_n*m_dif;

          double R1 = sc1->interp_radius(kk);
          double R2 = sc2->interp_radius(mm);
          if (R1 == 0 || R2 == 0) continue;
          double ratio = R2/R1;
          double r1 = 0;

          while (r1 <= R1) {
            double r2 = r1*ratio;
            vgl_point_2d<double> r1_pt_p = sc1->fragment_pt(kk, r1);
            vgl_point_2d<double> r2_pt_p = sc2->fragment_pt(mm, r2);
            vgl_point_2d<double> r1_pt_m = sc1->fragment_pt(kk, -r1);
            vgl_point_2d<double> r2_pt_m = sc2->fragment_pt(mm, -r2);
            add_to_map(cor, r1_pt_p, r2_pt_p, r1_pt_m, r2_pt_m);
            r1 += step_size;
          }

        }
      } 
    }
  }

  cor->cnt3_ = 0;
  for (int i = 0; i<cor->region1_map_output_.rows(); i++)
    for (int j = 0; j<cor->region1_map_output_.cols(); j++) {
      vgl_point_2d<int> p = cor->region1_map_output_[i][j];
      if (p.x() < 0 || p.y() < 0) continue;
      cor->cnt3_++;
    }
    
  //vcl_cout << "number of region1 pixels: " << cor->cnt2_ << " number of corresponding pixels: " << cor->cnt3_ << vcl_endl;

  return true;

}

//: find correspondence of image regions based on LINEAR corrospondence of shock curves
bool dbru_rcor_generator::find_correspondence_shock(dbru_rcor_sptr cor, dbskr_scurve_sptr sc1, dbskr_scurve_sptr sc2) {

  if (!cor || cor->halt_) {
    vcl_cout << "Errors in constructor!\n";
    return false;
  }
  
  double step_size = 1.0;
  // assume sc1 and sc2 corresponds linearly  
  int k = 0;
  int m = 0;
  int k_next = sc1->num_points()-1;
  int m_next = sc2->num_points()-1;

  //: we want 5 times more samples
  int k_dif = k_next-k;
  int m_dif = m_next-m;
  int N = 10 * ( k_dif > m_dif ? k_dif : m_dif);
        
  for (int n = 0; n<N; n++) { 
        
    double ratio_n = double(n)/N;
    double kk = k + ratio_n*k_dif;
    double mm = m + ratio_n*m_dif;

    double R1 = sc1->interp_radius(kk);
    double R2 = sc2->interp_radius(mm);
    if (R1 == 0 || R2 == 0) continue;
    double ratio = R2/R1;
    double r1 = 0;

    while (r1 <= R1) {
      double r2 = r1*ratio;
      vgl_point_2d<double> r1_pt_p = sc1->fragment_pt(kk, r1);
      vgl_point_2d<double> r2_pt_p = sc2->fragment_pt(mm, r2);
      vgl_point_2d<double> r1_pt_m = sc1->fragment_pt(kk, -r1);
      vgl_point_2d<double> r2_pt_m = sc2->fragment_pt(mm, -r2);
      add_to_map(cor, r1_pt_p, r2_pt_p, r1_pt_m, r2_pt_m);
      r1 += step_size;
    }
  }

  cor->cnt3_ = 0;
  for (int i = 0; i<cor->region1_map_output_.rows(); i++)
    for (int j = 0; j<cor->region1_map_output_.cols(); j++) {
      vgl_point_2d<int> p = cor->region1_map_output_[i][j];
      if (p.x() < 0 || p.y() < 0) continue;
      cor->cnt3_++;
    }
    
  //vcl_cout << "number of region1 pixels: " << cor->cnt2_ << " number of corresponding pixels: " << cor->cnt3_ << vcl_endl;

  return true;
}


//: find region correspondence based on intersections of lines
bool dbru_rcor_generator::find_correspondence_line(dbru_rcor_sptr cor, dbcvr_cv_cor_sptr sil_cor, 
                                                   unsigned int i_increment) 
{
  cor->set_sil_cor(sil_cor);
  return find_correspondence_line(cor, i_increment);                                                  
}

//: this method uses the same algorithm in find_correspondence_line, 
//  however it uses arclength to select quads
bool dbru_rcor_generator::find_correspondence_line2(dbru_rcor_sptr cor, dbcvr_cv_cor_sptr sil_cor, 
                                                    double ratio) 
{
  cor->set_sil_cor(sil_cor);
  return find_correspondence_line2(cor, ratio);
}

//: this method uses the same algorithm in find_correspondence_line, 
//  however it distributes increment on the selection of both starting and ending points of 
//  both line segment
bool dbru_rcor_generator::find_correspondence_line3(dbru_rcor_sptr cor, dbcvr_cv_cor_sptr sil_cor, int increment) 
{
  cor->set_sil_cor(sil_cor);
  return find_correspondence_line3(cor, increment);
}

//: this method goes from pixels to the contour via pairs of line segments
// that intersect on that pixel, 
// it rotates the two line segments by delta_theta each time to get a new quad
// delta_theta is determined by the total number of votes 
bool dbru_rcor_generator::find_correspondence_line4(dbru_rcor_sptr cor, dbcvr_cv_cor_sptr sil_cor, 
                                                    int total_vote) 
{
  cor->set_sil_cor(sil_cor);
  return find_correspondence_line4(cor, total_vote);
}

//: find region correspondence based on distance transform 
bool dbru_rcor_generator::find_correspondence_dt(dbru_rcor_sptr cor, dbcvr_cv_cor_sptr sil_cor,
                                                 dbsol_interp_curve_2d_sptr curve1,
                                                 dbsol_interp_curve_2d_sptr curve2,
                                                 float scale) 
{
  cor->set_sil_cor(sil_cor);
  return find_correspondence_dt(cor, curve1, curve2, scale);                              
}

bool dbru_rcor_generator::find_correspondence_dt2(dbru_rcor_sptr cor, dbcvr_cv_cor_sptr sil_cor,
                                                  dbsol_interp_curve_2d_sptr curve1,
                                                  dbsol_interp_curve_2d_sptr curve2,
                                                  float scale, float ratio) 
{
  cor->set_sil_cor(sil_cor);
  return find_correspondence_dt2(cor, curve1, curve2, scale, ratio);                              
}

//: find correspondence of image regions based on correspondence of shock branches
bool dbru_rcor_generator::find_correspondence_shock(dbru_rcor_sptr cor, dbskr_sm_cor_sptr sm_cor) 
{
  cor->set_sm_cor(sm_cor);
  return find_correspondence_shock(cor);
}
