#include <testlib/testlib_test.h>
#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_scurve.h>
#include <dbskr/dbskr_dpmatch.h>
#include <dbskr/dbskr_tree_sptr.h>
#include <iostream>
//#include <cmath>
 
int read_curve_file(std::ifstream& fs,
                    std::vector<double>& time, 
                    std::vector<double>& alpha, 
                    std::vector <int>& shock_dir, 
                    std::vector< vgl_point_2d<double> >& bdry_plus, 
                    std::vector< vgl_point_2d<double> >& bdry_minus,  
                    std::vector< vgl_point_2d<double> >& pt_array,   // points on shock branch  
                    std::vector<double>& bdry_plus_arclength, 
                    std::vector<double>& bdry_plus_angle, 
                    std::vector<double>& bdry_minus_arclength, 
                    std::vector<double>& bdry_minus_angle,
                    std::vector<double>& angle,
                    std::vector<double>& velocity,
                    std::vector<double>& curvature) {
  int size;
  fs >> size;
  for (int i = 0; i<size; i++) {
    
    //  fprintf(fp,"%7.5f %7.5f %7.5f %7.5f %8.6f %7.5f ",
    //    _ptArray[i].x(),_ptArray[i].y(),_time[i],_arcLength[i],
    //    (999.0<_velocity[i]?999.0:_velocity[i]),_curvature[i]);
    
    float x, y, dummy;
    fs >> x;
    fs >> y;  
    vgl_point_2d<double> p(x, y);   
    pt_array.push_back(p);

    fs >> dummy; time.push_back(dummy);
    fs >> dummy;  // arclength ignored
    fs >> dummy; velocity.push_back(dummy);
    fs >> dummy; curvature.push_back(dummy);
        
    // fprintf(fp,"%7.5f %7.5f %7.5f %7.5f %7.5f %7.5f %2d ",
    //    _alpha[i],_bdryPlusArray[i].x(),_bdryPlusArray[i].y(),
    //    _bdryMinusArray[i].x(),_bdryMinusArray[i].y(),_angle[i],_shockDir[i]);

    fs >> dummy; alpha.push_back(dummy);
    fs >> x;
    fs >> y;  
    vgl_point_2d<double> pp(x, y);   
    bdry_plus.push_back(pp);
    fs >> x;
    fs >> y;  
    vgl_point_2d<double> pm(x, y);   
    bdry_minus.push_back(pm);
    fs >> dummy; angle.push_back(dummy);
    int dir;
    fs >> dir; shock_dir.push_back(dir);
  
    //  fprintf(fp,"%7.5f %7.5f %7.5f %7.5f \n",
    //    _bdryPlusArcLength[i],_bdryMinusArcLength[i],
    //    _bdryPlusAngle[i],_bdryMinusAngle[i]);
    fs >> dummy; bdry_plus_arclength.push_back(dummy);
    fs >> dummy; bdry_minus_arclength.push_back(dummy);
    fs >> dummy; bdry_plus_angle.push_back(dummy);
    fs >> dummy; bdry_minus_angle.push_back(dummy);
  }

  return size;
}
 
void clear_all (    std::vector<double>& time, 
                    std::vector<double>& alpha, 
                    std::vector <int>& shock_dir, 
                    std::vector< vgl_point_2d<double> >& bdry_plus, 
                    std::vector< vgl_point_2d<double> >& bdry_minus,  
                    std::vector< vgl_point_2d<double> >& pt_array,   // points on shock branch  
                    std::vector<double>& bdry_plus_arclength, 
                    std::vector<double>& bdry_plus_angle, 
                    std::vector<double>& bdry_minus_arclength, 
                    std::vector<double>& bdry_minus_angle,
                    std::vector<double>& angle,
                    std::vector<double>& velocity,
                    std::vector<double>& curvature) {
  time.clear();
  alpha.clear();
  shock_dir.clear();
  bdry_plus.clear();
  bdry_minus.clear();
  pt_array.clear();
  bdry_plus_arclength.clear();
  bdry_minus_arclength.clear();
  bdry_plus_angle.clear();
  bdry_minus_angle.clear();
  angle.clear();
  velocity.clear();
  curvature.clear();
}

bool check_properties(dbskr_scurve_sptr c, 
                      std::vector<double>& bdry_plus_arclength, 
                      std::vector<double>& bdry_plus_angle, 
                      std::vector<double>& bdry_minus_arclength, 
                      std::vector<double>& bdry_minus_angle, double precision) {

  int size = c->num_points();
  for (int i = 0; i<size; i++) {
    if (std::fabs(c->boundary_plus_arclength(i) - bdry_plus_arclength[i]) > precision)
      return false;
    if (std::fabs(c->boundary_minus_arclength(i) - bdry_minus_arclength[i]) > precision)
      return false;
    if (std::fabs(c->boundary_plus_angle(i) - bdry_plus_angle[i]) > precision)
      return false;
    if (std::fabs(c->boundary_minus_angle(i) - bdry_minus_angle[i]) > precision)
      return false;
  }
  return true;
}

MAIN_ARGS(test_scvmatch)
{

  std::string dir_base;

  if ( argc >= 2 ) {
    dir_base = argv[1];
#ifdef VCL_WIN32
    dir_base += "\\";
#else
    dir_base += "/";
#endif
  }

  testlib_test_start("testing dbskr_dpmatch class ");

  std::string curve_file1 = "data/brk001-pln006-0-1-vs-0-2-5-curves.txt";
  std::string curve_file2 = "data/brk001-pln006-0-1-vs-1-3-4-13-15-curves.txt";
  std::string curve_file3 = "data/brk001-pln006-0-1-2-vs-0-2-5-curves.txt";
  
  std::ifstream cf1((dir_base+curve_file1).c_str(), std::ios::in);
  TEST("file1 open ", cf1.is_open(), true);
  std::ifstream cf2((dir_base+curve_file2).c_str(), std::ios::in);
  TEST("file2 open ", cf2.is_open(), true);
  std::ifstream cf3((dir_base+curve_file3).c_str(), std::ios::in);
  TEST("file3 open ", cf3.is_open(), true);
  
  //: testing first curve pair's matching
  std::vector<double> time, alpha;
  std::vector <int> shock_dir;
  int num_points;
  std::vector< vgl_point_2d<double> > bdry_plus, bdry_minus, pt_array;
  std::vector<double> bdry_plus_arclength, bdry_plus_angle, bdry_minus_arclength, bdry_minus_angle;
  std::vector<double> angle, velocity, curvature;
  num_points = read_curve_file(cf1, time, alpha, shock_dir, 
                               bdry_plus, bdry_minus, pt_array, 
                               bdry_plus_arclength, bdry_plus_angle, 
                               bdry_minus_arclength, bdry_minus_angle,
                               angle, velocity, curvature);
                               
  TEST("file1 read ok ", (int)bdry_plus.size(), num_points); 
  dbskr_scurve_sptr c1 = new dbskr_scurve(num_points, pt_array, time, angle, alpha);
  TEST("c1 num_points() ", c1->num_points(), num_points);
  TEST("c1 compute_properties() ", check_properties(c1, bdry_plus_arclength, bdry_plus_angle, 
                                                        bdry_minus_arclength, bdry_minus_angle, 0.001), true);
  clear_all(time, alpha, shock_dir,bdry_plus, bdry_minus, pt_array, bdry_plus_arclength, bdry_plus_angle, 
            bdry_minus_arclength, bdry_minus_angle, angle, velocity, curvature);
  
  //: read the second curve from the same file
  num_points = read_curve_file(cf1, time, alpha, shock_dir, 
                               bdry_plus, bdry_minus, pt_array, 
                               bdry_plus_arclength, bdry_plus_angle, 
                               bdry_minus_arclength, bdry_minus_angle,
                               angle, velocity, curvature);
  TEST("file1 read ok ", (int)bdry_plus.size(), num_points); 
  dbskr_scurve_sptr c2 = new dbskr_scurve(num_points, pt_array, time, angle, alpha);
  TEST("c2 num_points() ", c2->num_points(), num_points);
  TEST("c2 compute_properties() ", check_properties(c2, bdry_plus_arclength, bdry_plus_angle, 
                                                        bdry_minus_arclength, bdry_minus_angle, 0.001), true);

  float finalCost;
  cf1 >> finalCost;
  //std::cout << "real final cost of matching c1 and c2: " << finalCost << std::endl;

  std::vector <double> lambda;
  for (int i=0;i<3;i++)
    lambda.push_back(1.0);
  double R = 6.0;
  dbskr_dpmatch d(c1, c2, R, lambda, 3);
  d.Match();
  double finalCostM = d.finalCost();
  //std::cout << "final cost of matching c1 and c2: " << finalCostM << std::endl;
  
  TEST_NEAR("final_cost() ", finalCost, finalCostM, 0.01);
  cf1.close();

  //: testing second curve pair's matching
  clear_all(time, alpha, shock_dir,bdry_plus, bdry_minus, pt_array, bdry_plus_arclength, bdry_plus_angle, 
            bdry_minus_arclength, bdry_minus_angle, angle, velocity, curvature);

  num_points = read_curve_file(cf2, time, alpha, shock_dir, 
                               bdry_plus, bdry_minus, pt_array, 
                               bdry_plus_arclength, bdry_plus_angle, 
                               bdry_minus_arclength, bdry_minus_angle,
                               angle, velocity, curvature);
                               
  TEST("file2 read ok ", (int)bdry_plus.size(), num_points); 
  dbskr_scurve_sptr c21 = new dbskr_scurve(num_points, pt_array, time, angle, alpha);
  TEST("c21 num_points() ", c21->num_points(), num_points);
  TEST("c21 compute_properties() ", check_properties(c21, bdry_plus_arclength, bdry_plus_angle, 
                                                        bdry_minus_arclength, bdry_minus_angle, 0.001), true);
  clear_all(time, alpha, shock_dir,bdry_plus, bdry_minus, pt_array, bdry_plus_arclength, bdry_plus_angle, 
            bdry_minus_arclength, bdry_minus_angle, angle, velocity, curvature);
  
  //: read the second curve from the same file
  num_points = read_curve_file(cf2, time, alpha, shock_dir, 
                               bdry_plus, bdry_minus, pt_array, 
                               bdry_plus_arclength, bdry_plus_angle, 
                               bdry_minus_arclength, bdry_minus_angle,
                               angle, velocity, curvature);
  TEST("file2 read ok ", (int)bdry_plus.size(), num_points); 
  dbskr_scurve_sptr c22 = new dbskr_scurve(num_points, pt_array, time, angle, alpha);
  TEST("c22 num_points() ", c22->num_points(), num_points);
  TEST("c22 compute_properties() ", check_properties(c22, bdry_plus_arclength, bdry_plus_angle, 
                                                        bdry_minus_arclength, bdry_minus_angle, 0.001), true);

  cf2 >> finalCost;
  //std::cout << "real final cost of matching c21 and c22: " << finalCost << std::endl;
  
  dbskr_dpmatch d2(c21, c22, R, lambda, 3);
  d2.Match();
  finalCostM = d2.finalCost();
  //std::cout << "final cost of matching c21 and c22: " << finalCostM << std::endl;
  
  TEST_NEAR("final_cost() ", finalCost, finalCostM, 0.01);

  cf2.close();

  //: testing third curve pair's matching
  clear_all(time, alpha, shock_dir,bdry_plus, bdry_minus, pt_array, bdry_plus_arclength, bdry_plus_angle, 
            bdry_minus_arclength, bdry_minus_angle, angle, velocity, curvature);

  num_points = read_curve_file(cf3, time, alpha, shock_dir, 
                               bdry_plus, bdry_minus, pt_array, 
                               bdry_plus_arclength, bdry_plus_angle, 
                               bdry_minus_arclength, bdry_minus_angle,
                               angle, velocity, curvature);
                               
  TEST("file3 read ok ", (int)bdry_plus.size(), num_points); 
  dbskr_scurve_sptr c31 = new dbskr_scurve(num_points, pt_array, time, angle, alpha);
  TEST("c31 num_points() ", c31->num_points(), num_points);
  TEST("c31 compute_properties() ", check_properties(c31, bdry_plus_arclength, bdry_plus_angle, 
                                                        bdry_minus_arclength, bdry_minus_angle, 0.001), true);
  clear_all(time, alpha, shock_dir,bdry_plus, bdry_minus, pt_array, bdry_plus_arclength, bdry_plus_angle, 
            bdry_minus_arclength, bdry_minus_angle, angle, velocity, curvature);
  
  //: read the second curve from the same file
  num_points = read_curve_file(cf3, time, alpha, shock_dir, 
                               bdry_plus, bdry_minus, pt_array, 
                               bdry_plus_arclength, bdry_plus_angle, 
                               bdry_minus_arclength, bdry_minus_angle,
                               angle, velocity, curvature);
  TEST("file3 read ok ", (int)bdry_plus.size(), num_points); 
  dbskr_scurve_sptr c32 = new dbskr_scurve(num_points, pt_array, time, angle, alpha);
  TEST("c32 num_points() ", c32->num_points(), num_points);
  TEST("c32 compute_properties() ", check_properties(c32, bdry_plus_arclength, bdry_plus_angle, 
                                                        bdry_minus_arclength, bdry_minus_angle, 0.001), true);

  cf3 >> finalCost;
  //std::cout << "real final cost of matching c31 and c32: " << finalCost << std::endl;

  dbskr_dpmatch d3(c31, c32, R, lambda, 3);
  d3.Match();
  finalCostM = d3.finalCost();
  //std::cout << "final cost of matching c31 and c32: " << finalCostM << std::endl;
  
  TEST_NEAR("final_cost() ", finalCost, finalCostM, 0.01);
  cf3.close();

  return testlib_test_summary();
}
