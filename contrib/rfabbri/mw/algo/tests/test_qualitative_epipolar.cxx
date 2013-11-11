#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_iterator.h>
#include <mw/algo/mw_qualitative_epipolar.h>
#include <mw/mw_util.h>

static bool basic_test();
static bool test_sectors();
static bool test_partition();
static void test_valid_sectors_by_partition();

static double tol=1e-8;

MAIN( test_qualitative_epipolar )
{
  START ("Qualitative determination of epipolar location from correspondences");

  // THESE TESTS ARE DOCUMENTED IN MY 9/6/7 NOTES.

  //bool ret = 
  basic_test();

//  test_sectors();

//  test_partition();

//  test_valid_sectors_by_partition();









  SUMMARY();
}

//: \return true if success
bool
basic_test()
{

  /* Test case "valid sectors 1 non-degenerate"
  */
  /*
  vgl_box_2d<double> bbox(-30,30,-30,30);
  vcl_vector<vsol_point_2d_sptr> p0,p1;

  p0.push_back(new vsol_point_2d(0,0));
  p0.push_back(new vsol_point_2d(2,1));
  p0.push_back(new vsol_point_2d(1,2));
  p1=p0;
  */

//  mw_qualitative_epipolar epipole_finder(p0,p1,bbox);

//  epipole_finder.compute_epipolar_region();

//  vgl_box_2d<double> bbox(-30+1000,30+1000,-30-31,30-31);
//  vcl_vector<vsol_point_2d_sptr> p0,p1;

  /* Test case "valid sectors 1 non-degenerate translated"
  p0.push_back(new vsol_point_2d(0+1000,0-31));
  p0.push_back(new vsol_point_2d(2+1000,1-31));
  p0.push_back(new vsol_point_2d(1+1000,2-31));
  p1=p0;
  */

  /* Test case "valid sectors 1"
  p0.push_back(new vsol_point_2d(0,0));
  p0.push_back(new vsol_point_2d(1,0));
  p0.push_back(new vsol_point_2d(0,1));
  p1=p0;
  */

  // --------- Test case "valid sectors 2"
  /*
  {
  vgl_box_2d<double> bbox(-100,100,-100,100);
  vcl_vector<vsol_point_2d_sptr> p0,p1;

  p0.push_back(new vsol_point_2d(0,0));
  p0.push_back(new vsol_point_2d(1,1));
  p0.push_back(new vsol_point_2d(4,1));
  p0.push_back(new vsol_point_2d(2,4));

  p1.push_back(new vsol_point_2d(0,0));
  p1.push_back(new vsol_point_2d(1,1));
  p1.push_back(new vsol_point_2d(0,4));
  p1.push_back(new vsol_point_2d(2,4));

  mw_qualitative_epipolar epipole_finder(p0,p1,bbox);

  epipole_finder.compute_epipolar_region();
  epipole_finder.print_polygons();
  }
  */
  // --------- Test case "ge data 8 points"

  /*
  {
  vgl_box_2d<double> bbox(-5000,5000,-4000,4000);
  vcl_vector<vsol_point_2d_sptr> p0,p1;

  p0.push_back(new vsol_point_2d(647,627 ));
  p0.push_back(new vsol_point_2d(96,268  ));
  p0.push_back(new vsol_point_2d(947,516 ));
  p0.push_back(new vsol_point_2d(605,275 ));
  p0.push_back(new vsol_point_2d(776,779 ));
  p0.push_back(new vsol_point_2d(989,155 ));
  p0.push_back(new vsol_point_2d(1244,837));
  p0.push_back(new vsol_point_2d(417,792 ));

  p1.push_back(new vsol_point_2d(651,562 ));
  p1.push_back(new vsol_point_2d(741,346 ));
  p1.push_back(new vsol_point_2d(955,534 ));
  p1.push_back(new vsol_point_2d(1023,378));
  p1.push_back(new vsol_point_2d(544,638 ));
  p1.push_back(new vsol_point_2d(1103,279));
  p1.push_back(new vsol_point_2d(744,699 ));
  p1.push_back(new vsol_point_2d(351,618 ));

  mw_qualitative_epipolar epipole_finder(p0,p1,bbox);

  epipole_finder.compute_epipolar_region();
  epipole_finder.print_polygons();
  }
  */
  
  // --------- Test case "lockheed data 4 points" 

  /*
  {
  vgl_box_2d<double> bbox(-5000,5000,-5000,5000);
  vcl_vector<vsol_point_2d_sptr> p0,p1;

  p0.push_back(new vsol_point_2d(816.7    ,    57.6));
  p0.push_back(new vsol_point_2d(318.8    ,   448.8));
  p0.push_back(new vsol_point_2d(898.8    ,   130.8));
  p0.push_back(new vsol_point_2d( 96.6    ,   144.6));

  p1.push_back(new vsol_point_2d(906.5    ,    24.5));
  p1.push_back(new vsol_point_2d(354.6    ,   410.6));
  p1.push_back(new vsol_point_2d(981.4    ,    97.6));
  p1.push_back(new vsol_point_2d(172.2    ,     115));

  mw_qualitative_epipolar epipole_finder(p0,p1,bbox);

  bool retval = epipole_finder.compute_epipolar_region();
  if (!retval)
    vcl_cout << "Error -- degeneracy encountered for this input\n";
  else
    epipole_finder.print_polygons();
  }
  */
  {
  vgl_box_2d<double> bbox(-2000,2000,-4000,4000);
  vcl_vector<vsol_point_2d_sptr> p0,p1;

  p0.push_back(new vsol_point_2d( 199.216835042951, 86.8853870086034));
  p0.push_back(new vsol_point_2d( 206.647375990655, 134.123531366793));
  p0.push_back(new vsol_point_2d( 263.934029430035, 140.471351776347));
  p0.push_back(new vsol_point_2d( 245.627031966227, 168.718362306814));
  p0.push_back(new vsol_point_2d( 326.268743537626, 133.832778516946));
  p0.push_back(new vsol_point_2d( 232.280011472527, 164.027420489588));
  p0.push_back(new vsol_point_2d( 189.904005451024, 72.5586601904565));

  p1.push_back(new vsol_point_2d( 226.396309751088 ,81.4561992706835));
  p1.push_back(new vsol_point_2d( 216.016393506064 ,129.707980746253));
  p1.push_back(new vsol_point_2d( 243.403256501236 ,139.843119928550));
  p1.push_back(new vsol_point_2d( 275.012778377285 ,170.198506986232));
  p1.push_back(new vsol_point_2d( 337.530398145046 ,145.493688452473));
  p1.push_back(new vsol_point_2d( 265.462366420243 ,163.375087023794));
  p1.push_back(new vsol_point_2d( 221.048605514820 ,64.7719350379463));

  mw_qualitative_epipolar epipole_finder(p0,p1,bbox);

  bool retval = epipole_finder.compute_epipolar_region();
  if (!retval)
    vcl_cout << "Error -- degeneracy encountered for this input\n";
  else
    epipole_finder.print_polygons();
  }

  return true;
}

bool 
test_sectors()
{

  // --------------------------
  vcl_cout << "Test case 1\n";
  {
  vcl_vector<vsol_point_2d_sptr> p;
  vcl_vector<bool> outward;
  vcl_vector<double> angle;

  p.push_back(new vsol_point_2d(1,1));
  p.push_back(new vsol_point_2d(0,0));
  p.push_back(new vsol_point_2d(1,-1));

  mw_qualitative_epipolar::build_sectors(p,1,angle,outward);

  TEST("# angles is 2",angle.size(),2);
  TEST_NEAR("Angle 0", angle[0],vnl_math::pi/4.0,tol);
  TEST_NEAR("Angle 1", angle[1],3*vnl_math::pi/4.0,tol);

  TEST("Outward 0",outward[0],true);
  TEST("Outward 1",outward[1],false);
  }


  // --------------------------
  vcl_cout << "Test case 1 translated\n";
  {
  vcl_vector<vsol_point_2d_sptr> p;
  vcl_vector<bool> outward;
  vcl_vector<double> angle;

  p.push_back(new vsol_point_2d(1+1000,1-31));
  p.push_back(new vsol_point_2d(0+1000,0-31));
  p.push_back(new vsol_point_2d(1+1000,-1-31));

  mw_qualitative_epipolar::build_sectors(p,1,angle,outward);

  TEST("# angles is 2",angle.size(),2);
  TEST_NEAR("Angle 0", angle[0],vnl_math::pi/4.0,tol);
  TEST_NEAR("Angle 1", angle[1],3*vnl_math::pi/4.0,tol);

  TEST("Outward 0",outward[0],true);
  TEST("Outward 1",outward[1],false);

  }
    
  // --------------------------
  vcl_cout << "Test case 2\n";

  {
  vcl_vector<vsol_point_2d_sptr> p;
  vcl_vector<bool> outward;
  vcl_vector<double> angle;

  p.push_back(new vsol_point_2d(0+1000,0-31));
  p.push_back(new vsol_point_2d(1+1000,1-31));
  p.push_back(new vsol_point_2d(1+1000,-1-31));
  p.push_back(new vsol_point_2d(0+1000,1-31));

  mw_qualitative_epipolar::build_sectors(p,0,angle,outward);

  TEST("# angles is 3",angle.size(),3);
  TEST_NEAR("Angle 0", angle[0],vnl_math::pi/4.0,tol);
  TEST_NEAR("Angle 1", angle[1],vnl_math::pi/2.0,tol);
  TEST_NEAR("Angle 2", angle[2],3*vnl_math::pi/4.0,tol);

  TEST("Outward 0",outward[0],true);
  TEST("Outward 1",outward[1],true);
  TEST("Outward 2",outward[2],false);
  }

  // --------------------------
  vcl_cout << "Test case 3a\n";
  {
  vcl_vector<vsol_point_2d_sptr> p;
  vcl_vector<bool> outward;
  vcl_vector<double> angle;

  p.push_back(new vsol_point_2d(0+1000,0-31));
  p.push_back(new vsol_point_2d(1+1000,-1-31));
  p.push_back(new vsol_point_2d(1+1000,1-31));
  p.push_back(new vsol_point_2d(1+1000,0-31));

  mw_qualitative_epipolar::build_sectors(p,0,angle,outward);

  TEST("# angles is 3",angle.size(),3);
  TEST_NEAR("Angle 0", angle[0],0,tol);
  TEST_NEAR("Angle 1", angle[1],vnl_math::pi/4.0,tol);
  TEST_NEAR("Angle 2", angle[2],3*vnl_math::pi/4.0,tol);

  TEST("Outward 0",outward[0],true);
  TEST("Outward 1",outward[1],true);
  TEST("Outward 2",outward[2],false);
  }
  
  // --------------------------
  vcl_cout << "Test case 3b\n";
  {
  vcl_vector<vsol_point_2d_sptr> p;
  vcl_vector<bool> outward;
  vcl_vector<double> angle;

  p.push_back(new vsol_point_2d(0 +1000, 0   -31));
  p.push_back(new vsol_point_2d(1 +1000,-1   -31));
  p.push_back(new vsol_point_2d(1 +1000, 1   -31));
  p.push_back(new vsol_point_2d(-1 +1000, 0   -31));

  mw_qualitative_epipolar::build_sectors(p,0,angle,outward);

  // This behavior is dependent on the vcl_atan2 implementation
  TEST("# angles is 3",angle.size(),3);

  // Behavior 1:
  TEST_NEAR("Angle 0", angle[0],0,tol);
  TEST_NEAR("Angle 1", angle[1],vnl_math::pi/4.0,tol);
  TEST_NEAR("Angle 2", angle[2],3*vnl_math::pi/4.0,tol);
  TEST("Angle 0 non-negative", angle[0]<0,false);

  TEST("Outward 0",outward[0],false);
  TEST("Outward 1",outward[1],true);
  TEST("Outward 2",outward[2],false);

  /* Behavior 2:
  TEST_NEAR("Angle 0", angle[0],vnl_math::pi/4.0,tol);
  TEST_NEAR("Angle 1", angle[1],3*vnl_math::pi/4.0,tol);
  TEST_NEAR("Angle 2 near pi", angle[2],vnl_math::pi,tol);
  TEST("Angle 2 strictly smaller than pi", angle[2]<vnl_math::pi,true);

  TEST("Outward 0",outward[0],true);
  TEST("Outward 1",outward[1],false);
  TEST("Outward 2",outward[2],false);
  */
  }
  
  // --------------------------
  vcl_cout << "Test case 4\n";
  {
  vcl_vector<vsol_point_2d_sptr> p;
  vcl_vector<bool> outward;
  vcl_vector<double> angle;

  vcl_vector<double> gt_angle;
  vcl_vector<double> mag;

  gt_angle.push_back(130); mag.push_back(900);
  gt_angle.push_back(17);  mag.push_back(0.1);
  gt_angle.push_back(15);  mag.push_back(10);
  gt_angle.push_back(200); mag.push_back(100);
  gt_angle.push_back(93);  mag.push_back(1000);
  gt_angle.push_back(-20); mag.push_back(30);


  for (unsigned i=0; i < gt_angle.size(); ++i) {
    gt_angle[i] = vnl_math::pi*gt_angle[i]/180.0;

    p.push_back(new vsol_point_2d(mag[i]*vcl_cos(gt_angle[i]) -1e5,
                                  mag[i]*vcl_sin(gt_angle[i]) -3e3));
  }
  p.push_back(new vsol_point_2d(-1e5,-3e3));

  mw_qualitative_epipolar::build_sectors(p,p.size()-1,angle,outward);

  // transform back to degrees


  TEST("# angles is 6",angle.size(),6);


  vcl_vector<double> gt_angle_sorted;
  vcl_vector<bool> gt_outward;
  gt_angle_sorted.push_back(15); gt_outward.push_back(true);
  gt_angle_sorted.push_back(17); gt_outward.push_back(true);
  gt_angle_sorted.push_back(20); gt_outward.push_back(false);
  gt_angle_sorted.push_back(93); gt_outward.push_back(true);
  gt_angle_sorted.push_back(130); gt_outward.push_back(true);
  gt_angle_sorted.push_back(160); gt_outward.push_back(false);

  for (unsigned i=0; i < angle.size(); ++i) {
    TEST_NEAR("Angle", 180.0*angle[i]/vnl_math::pi,gt_angle_sorted[i],tol);
    TEST("Outward",outward[i],gt_outward[i]);
  }
  }


  return true; 
}

bool 
test_partition()
{
  void
  compare_lr_sets(
      const vcl_set<unsigned> &l,
      const vcl_set<unsigned> &r,
      const vcl_set<unsigned> &l_gt,
      const vcl_set<unsigned> &r_gt);

  // --------------------------
  vcl_cout << "Test case 1\n";
  {
  vcl_vector<vsol_point_2d_sptr> p;

  p.push_back(new vsol_point_2d(1,1));
  p.push_back(new vsol_point_2d(0,0));
  p.push_back(new vsol_point_2d(1,-1));

  vcl_set<unsigned> l,r;

  mw_qualitative_epipolar::partition(p,1,0,true,l,r);

  vcl_set<unsigned> l_gt,r_gt;

  l_gt.insert(0);
  r_gt.insert(2);

  compare_lr_sets(l,r,l_gt,r_gt);

  l.clear(); r.clear();
  mw_qualitative_epipolar::partition(p,1,0,false,l,r);

  compare_lr_sets(l,r,r_gt,l_gt);
  }
  
  // --------------------------
  vcl_cout << "Test case 1 translated\n";
  {
  vcl_vector<vsol_point_2d_sptr> p;

  p.push_back(new vsol_point_2d(1+1000,1-31));
  p.push_back(new vsol_point_2d(0+1000,0-31));
  p.push_back(new vsol_point_2d(1+1000,-1-31));

  vcl_set<unsigned> l,r;

  mw_qualitative_epipolar::partition(p,1,0,true,l,r);

  vcl_set<unsigned> l_gt,r_gt;

  l_gt.insert(0);
  r_gt.insert(2);

  compare_lr_sets(l,r,l_gt,r_gt);

  l.clear(); r.clear();
  mw_qualitative_epipolar::partition(p,1,0,false,l,r);

  compare_lr_sets(l,r,r_gt,l_gt);
  }
  
  // --------------------------
  vcl_cout << "Test case 2\n";
  {
  vcl_vector<vsol_point_2d_sptr> p;

  p.push_back(new vsol_point_2d(0+1000,0-31));
  p.push_back(new vsol_point_2d(1+1000,1-31));
  p.push_back(new vsol_point_2d(1+1000,-1-31));
  p.push_back(new vsol_point_2d(0+1000,1-31));

  vcl_set<unsigned> l,r;

  mw_qualitative_epipolar::partition(p,0,0,true,l,r);

  vcl_set<unsigned> l_gt,r_gt;

  l_gt.insert(1);
  l_gt.insert(3);
  r_gt.insert(2);

  compare_lr_sets(l,r,l_gt,r_gt);

  l.clear(); r.clear();
  mw_qualitative_epipolar::partition(p,0,0,false,l,r);

  compare_lr_sets(l,r,r_gt,l_gt);
  }
  
  // --------------------------
  vcl_cout << "Test case 3a\n";
  {
  vcl_vector<vsol_point_2d_sptr> p;

  p.push_back(new vsol_point_2d(0+1000,0-31));
  p.push_back(new vsol_point_2d(1+1000,-1-31));
  p.push_back(new vsol_point_2d(1+1000,1-31));
  p.push_back(new vsol_point_2d(1+1000,0-31));

  vcl_set<unsigned> l,r;

  mw_qualitative_epipolar::partition(p,0,vnl_math::pi/2.0,true,l,r);

  vcl_set<unsigned> l_gt,r_gt;

  r_gt.insert(1);
  r_gt.insert(2);
  r_gt.insert(3);

  compare_lr_sets(l,r,l_gt,r_gt);

  l.clear(); r.clear();
  mw_qualitative_epipolar::partition(p,0,vnl_math::pi/2.0,false,l,r);

  compare_lr_sets(l,r,r_gt,l_gt);
  }

  // --------------------------
  vcl_cout << "Test case 3b\n";
  {
  vcl_vector<vsol_point_2d_sptr> p;
  vcl_vector<bool> outward;
  vcl_vector<double> angle;

  p.push_back(new vsol_point_2d(0 +1000, 0   -31));
  p.push_back(new vsol_point_2d(1 +1000,-1   -31));
  p.push_back(new vsol_point_2d(1 +1000, 1   -31));
  p.push_back(new vsol_point_2d(-1 +1000, 0   -31));

  vcl_set<unsigned> l,r;

  mw_qualitative_epipolar::partition(p,0,vnl_math::pi/2.0,true,l,r);

  vcl_set<unsigned> l_gt,r_gt;

  r_gt.insert(1);
  r_gt.insert(2);
  l_gt.insert(3);

  compare_lr_sets(l,r,l_gt,r_gt);

  l.clear(); r.clear();
  mw_qualitative_epipolar::partition(p,0,vnl_math::pi/2.0,false,l,r);

  compare_lr_sets(l,r,r_gt,l_gt);
  }
  
  // --------------------------
  vcl_cout << "Test case 4\n";
  {
  vcl_vector<vsol_point_2d_sptr> p;

  vcl_vector<double> gt_angle;
  vcl_vector<double> mag;

  gt_angle.push_back(130); mag.push_back(900);
  gt_angle.push_back(17);  mag.push_back(0.1);
  gt_angle.push_back(15);  mag.push_back(10);
  gt_angle.push_back(200); mag.push_back(100);
  gt_angle.push_back(93);  mag.push_back(1000);
  gt_angle.push_back(-20); mag.push_back(30);


  for (unsigned i=0; i < gt_angle.size(); ++i) {
    gt_angle[i] = vnl_math::pi*gt_angle[i]/180.0;

    p.push_back(new vsol_point_2d(mag[i]*vcl_cos(gt_angle[i]) -1e5,
                                  mag[i]*vcl_sin(gt_angle[i]) -3e3));
  }
  p.push_back(new vsol_point_2d(-1e5,-3e3));

  vcl_set<unsigned> l,r;

  mw_qualitative_epipolar::partition(p,p.size()-1,vnl_math::pi*170.0/180.0,true,l,r);

  vcl_set<unsigned> l_gt,r_gt;

  l_gt.insert(3);
  l_gt.insert(5);
  r_gt.insert(2);
  r_gt.insert(1);
  r_gt.insert(4);
  r_gt.insert(0);

  compare_lr_sets(l,r,l_gt,r_gt);

  l.clear(); r.clear();
  mw_qualitative_epipolar::partition(p,p.size()-1,vnl_math::pi*170.0/180.0,false,l,r);
  compare_lr_sets(l,r,r_gt,l_gt);

  l.clear(); r.clear();
  l_gt.clear(); r_gt.clear();

  l_gt.insert(4);
  l_gt.insert(0);
  l_gt.insert(3);
  r_gt.insert(5);
  r_gt.insert(2);
  r_gt.insert(1);
  
  mw_qualitative_epipolar::partition(p,p.size()-1,vnl_math::pi*60.0/180.0,true,l,r);
  compare_lr_sets(l,r,l_gt,r_gt);

  l.clear(); r.clear();
  mw_qualitative_epipolar::partition(p,p.size()-1,vnl_math::pi*60.0/180.0,false,l,r);
  compare_lr_sets(l,r,r_gt,l_gt);
  }


  return true;
}

void
compare_lr_sets(
    const vcl_set<unsigned> &l,
    const vcl_set<unsigned> &r,
    const vcl_set<unsigned> &l_gt,
    const vcl_set<unsigned> &r_gt)
{
  TEST("left set",l==l_gt,true);
  TEST("right set",r==r_gt,true);
  vcl_cout << " Set l: ";
  vcl_copy(l.begin(), l.end(), vcl_ostream_iterator<unsigned>(vcl_cout, " "));
  vcl_cout << vcl_endl;
   vcl_cout << " Set r: ";
  vcl_copy(r.begin(), r.end(), vcl_ostream_iterator<unsigned>(vcl_cout, " "));
  vcl_cout << vcl_endl;
}


void
test_valid_sectors_by_partition()
{


  // --------------------------
  vcl_cout << "Test case valid_sectors_1\n";
  { 
  vgl_box_2d<double> bbox(-4,4,-3,3);
  vcl_vector<vsol_point_2d_sptr> p0,p1;

  p0.push_back(new vsol_point_2d(0,0));
  p0.push_back(new vsol_point_2d(1,0));
  p0.push_back(new vsol_point_2d(0,1));
  p1=p0;

  mw_qualitative_epipolar epipole_finder(p0,p1,bbox);

  unsigned s1 = (unsigned)-1;
  bool res;

  res = epipole_finder.valid_sector(0, 0, s1);


  TEST("p0 Sector 0 valid?",res,true);
  TEST("p0 Sector 0 on img 0 correspond to sector 0 on img 1?",0,s1);

  res = epipole_finder.valid_sector(0, 1, s1);
  TEST("p0 Sector 1 valid?",res,true);
  TEST("p0 Sector 1 on img 0 correspond to sector 1 on img 1?",1,s1);

  res = epipole_finder.valid_sector(1, 0, s1);
  TEST("p1 Sector 0 valid?",res,true);
  TEST("p1 Sector 0 on img 0 correspond to sector 0 on img 1?",0,s1);

  res = epipole_finder.valid_sector(1, 1, s1);
  TEST("p1 Sector 1 valid?",res,true);
  TEST("p1 Sector 1 on img 0 correspond to sector 1 on img 1?",1,s1);

  }

  // --------------------------
  vcl_cout << "Test case valid_sectors_2\n";
  { 
  vgl_box_2d<double> bbox(-10,10,-10,10);
  vcl_vector<vsol_point_2d_sptr> p0,p1;

  p0.push_back(new vsol_point_2d(0,0));
  p0.push_back(new vsol_point_2d(1,1));
  p0.push_back(new vsol_point_2d(4,1));
  p0.push_back(new vsol_point_2d(2,4));

  p1.push_back(new vsol_point_2d(0,0));
  p1.push_back(new vsol_point_2d(1,1));
  p1.push_back(new vsol_point_2d(0,4));
  p1.push_back(new vsol_point_2d(2,4));

  mw_qualitative_epipolar epipole_finder(p0,p1,bbox);

  unsigned s1 = (unsigned)-1;
  bool res;

  res = epipole_finder.valid_sector(0, 0, s1);

  TEST("p0 Sector 0 valid?",res,true);
  TEST("p0 Sector 0 on img 0 correspond to sector 1 on img 1?",1,s1);

  res = epipole_finder.valid_sector(0, 1, s1);
  TEST("p0 Sector 1 invalid?",res,false);

  res = epipole_finder.valid_sector(0, 2, s1);
  TEST("p0 Sector 2 valid?",res,true);
  TEST("p0 Sector 2 on img 0 correspond to sector 2 on img 1?",2,s1);

  res = epipole_finder.valid_sector(3, 0, s1);
  TEST("p3 Sector 0 invalid?",res,false);

  res = epipole_finder.valid_sector(3, 1, s1);
  TEST("p3 Sector 1 valid?",res,true);
  TEST("p3 Sector 1 on img 0 correspond to sector 0 on img 1?",0,s1);
  vcl_cout << "s1: " << s1 << vcl_endl;

  }

}
