// This is brcv/shp/dbsk2d/algo/tests/test_compute_ishock.cxx

#include <testlib/testlib_test.h>
#include <vcl_cstdlib.h>
#include <vcl_ctime.h>
#include <vnl/vnl_random.h>
#include <vsol/vsol_line_2d.h>

#include <dbsk2d/dbsk2d_boundary_sptr.h>
#include <dbsk2d/dbsk2d_boundary.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>

#include <dbsk2d/algo/dbsk2d_compute_shocks.h>
#include <dbsk2d/dbsk2d_file_io.h>

// Amir Tamrakar
// 02/06/2005

//uncomment this line if you want to save the randomly generated bnd
//#define OFFLINE_TESTING

//global random number generator
vnl_random better_rand;

#define rnd_num (better_rand.drand32(-1,1))
#define rnd_pos_num (better_rand.drand32(0,1))
#define rnd_perturb vgl_vector_2d<double>(1e-4*rnd_num, 1e-4*rnd_num)

bool test_compute_shocks(dbsk2d_boundary_sptr bnd)
{
  dbsk2d_ishock_graph_sptr shock = dbsk2d_compute_ishocks (bnd);

  //if computation crashed, the shocks are cleared, so just check for this
  return (shock->number_of_nodes()>0); 
}

bool test_PL_near_degen_1()
{
  bool successful = true;

  //generate 50 near degenerate boundaries and compute shocks on them
  for (int i=0; i<500; i++)
  {
    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list;
    vsol_list.clear();

    //define a line of a random length at a random orientation
    double th = 2*vnl_math::pi*rnd_num;
    vgl_point_2d<double> s(500+rnd_num, 500+rnd_num);
    vgl_point_2d<double> e = _translatePoint(s, th, 200*rnd_pos_num);

    //define 4 points that project close to the end points of this line
    vgl_point_2d<double> p1 = _translatePoint(s, th+vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;
    vgl_point_2d<double> p2 = _translatePoint(s, th-vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;
    vgl_point_2d<double> p3 = _translatePoint(e, th+vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;
    vgl_point_2d<double> p4 = _translatePoint(e, th-vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;

    #ifdef OFFLINE_TESTING
    vcl_cout << "PL_near_degen_1 ::: Test #" << i << " : ";
    vcl_cout << "[("<<s.x()<<", "<<s.y()<<")-("<<e.x()<<", "<<e.y()<<")] ";
    vcl_cout << "("<<p1.x()<<", "<<p1.y()<<")("<<p2.x()<<", "<<p2.y()<<")("<<p3.x()<<", "<<p3.y()<<")("<<p4.x()<<", "<<p4.y()<<")\n";
    #endif

    vsol_point_2d_sptr sp = new vsol_point_2d(s);
    vsol_point_2d_sptr ep = new vsol_point_2d(e);
    vsol_line_2d_sptr line = new vsol_line_2d(sp, ep);
    
    vsol_point_2d_sptr pp1 = new vsol_point_2d(p1);
    vsol_point_2d_sptr pp2 = new vsol_point_2d(p2);
    vsol_point_2d_sptr pp3 = new vsol_point_2d(p3);
    vsol_point_2d_sptr pp4 = new vsol_point_2d(p4);

    vsol_list.push_back(line->cast_to_spatial_object());
    vsol_list.push_back(pp1->cast_to_spatial_object());
    vsol_list.push_back(pp2->cast_to_spatial_object());
    vsol_list.push_back(pp3->cast_to_spatial_object());
    vsol_list.push_back(pp4->cast_to_spatial_object());

    #ifdef OFFLINE_TESTING
    //we should save this boundary so that we can debug it later
    dbsk2d_file_io::save_bnd_v3_0("degen_test.bnd", vsol_list);
    #endif

    //form a boundary class from this geometry
    dbsk2d_boundary_sptr bnd = dbsk2d_create_boundary( 
                                  vsol_list, //list of geometry
                                  false, //just use default partitioning
                                  0.0, 0.0,
                                  1, 1, //size num of cells
                                  1000.0, 1000.0, //dummy size
                                  true, //preprocess
                                  false); //break long lines

    //now compute shocks on it
    bool cur_successful = test_compute_shocks(bnd);

    if (!cur_successful){
      vcl_cout.precision(10);
      vcl_cout << "PL_near_degen_1 :: " ;
      vcl_cout << "[("<<s.x()<<", "<<s.y()<<")-("<<e.x()<<", "<<e.y()<<")] ";
      vcl_cout << "("<<p1.x()<<", "<<p1.y()<<")("<<p2.x()<<", "<<p2.y()<<")("<<p3.x()<<", "<<p3.y()<<")("<<p4.x()<<", "<<p4.y()<<")\n";
    }
    successful = successful && cur_successful;
  }
  return successful;
}

bool test_PL_near_degen_2()
{
  bool successful = true;

  //generate 50 near degenerate boundaries and compute shocks on them
  for (int i=0; i<500; i++)
  {
    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list;
    vsol_list.clear();

    //define a line of a random length at a random orientation
    double th = 2*vnl_math::pi*rnd_num;
    double th2 = th+1.5*CONTACT_EPSILON*rnd_num;
    vgl_point_2d<double> s1(500+rnd_num, 500+rnd_num);
    vgl_point_2d<double> e1 = _translatePoint(s1, th, 200*rnd_pos_num);
    vgl_point_2d<double> e2 = _translatePoint(e1, th2, 200*rnd_pos_num); //perturb collinearity

    //define 4 points that project close to the end points of this line
    vgl_point_2d<double> p1 = _translatePoint(s1, th+vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;
    vgl_point_2d<double> p2 = _translatePoint(s1, th-vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;
    vgl_point_2d<double> p3 = _translatePoint(e1, th+vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;
    vgl_point_2d<double> p4 = _translatePoint(e1, th-vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;
    vgl_point_2d<double> p5 = _translatePoint(e2, th2+vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;
    vgl_point_2d<double> p6 = _translatePoint(e2, th2-vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;

    #ifdef OFFLINE_TESTING
    vcl_cout << "PL_near_degen_2 ::: Test #" << i << " : ";
    vcl_cout << "[("<<s1.x()<<", "<<s1.y()<<")-("<<e1.x()<<", "<<e1.y()<<")] ";
    vcl_cout << "[("<<e1.x()<<", "<<e1.y()<<")-("<<e2.x()<<", "<<e2.y()<<")] ";
    vcl_cout << "("<<p1.x()<<", "<<p1.y()<<")("<<p2.x()<<", "<<p2.y()<<")";
    vcl_cout << "("<<p3.x()<<", "<<p3.y()<<")("<<p4.x()<<", "<<p4.y()<<")";
    vcl_cout << "("<<p5.x()<<", "<<p5.y()<<")("<<p6.x()<<", "<<p6.y()<<")\n";
    #endif

    vsol_point_2d_sptr sp1 = new vsol_point_2d(s1);
    vsol_point_2d_sptr ep1 = new vsol_point_2d(e1);
    vsol_point_2d_sptr ep2 = new vsol_point_2d(e2);
    vsol_line_2d_sptr line1 = new vsol_line_2d(sp1, ep1);
    vsol_line_2d_sptr line2 = new vsol_line_2d(ep1, ep2);
    
    vsol_point_2d_sptr pp1 = new vsol_point_2d(p1);
    vsol_point_2d_sptr pp2 = new vsol_point_2d(p2);
    vsol_point_2d_sptr pp3 = new vsol_point_2d(p3);
    vsol_point_2d_sptr pp4 = new vsol_point_2d(p4);
    vsol_point_2d_sptr pp5 = new vsol_point_2d(p5);
    vsol_point_2d_sptr pp6 = new vsol_point_2d(p6);

    vsol_list.push_back(line1->cast_to_spatial_object());
    vsol_list.push_back(line2->cast_to_spatial_object());
    vsol_list.push_back(pp1->cast_to_spatial_object());
    vsol_list.push_back(pp2->cast_to_spatial_object());
    vsol_list.push_back(pp3->cast_to_spatial_object());
    vsol_list.push_back(pp4->cast_to_spatial_object());
    vsol_list.push_back(pp5->cast_to_spatial_object());
    vsol_list.push_back(pp6->cast_to_spatial_object());

    #ifdef OFFLINE_TESTING
    //we should save this boundary so that we can debug it later
    dbsk2d_file_io::save_bnd_v3_0("degen_test.bnd", vsol_list);
    #endif

    //form a boundary class from this geometry
    dbsk2d_boundary_sptr bnd = dbsk2d_create_boundary( 
                                  vsol_list, //list of geometry
                                  false, //just use default partitioning
                                  0.0, 0.0,
                                  1, 1, //size num of cells
                                  1000.0, 1000.0, //dummy size
                                  true, //preprocess
                                  false); //break long lines

    //now compute shocks on it
    bool cur_successful = test_compute_shocks(bnd);

    if (!cur_successful){
      vcl_cout.precision(10);
      vcl_cout << "PL_near_degen_2 :: ";
      vcl_cout << "[("<<s1.x()<<", "<<s1.y()<<")-("<<e1.x()<<", "<<e1.y()<<")] ";
      vcl_cout << "[("<<e1.x()<<", "<<e1.y()<<")-("<<e2.x()<<", "<<e2.y()<<")] ";
      vcl_cout << "("<<p1.x()<<", "<<p1.y()<<")("<<p2.x()<<", "<<p2.y()<<")";
      vcl_cout << "("<<p3.x()<<", "<<p3.y()<<")("<<p4.x()<<", "<<p4.y()<<")";
      vcl_cout << "("<<p5.x()<<", "<<p5.y()<<")("<<p6.x()<<", "<<p6.y()<<")\n";
    }
    successful = successful && cur_successful;
  }
  return successful;
}

bool test_PL_near_degen_3()
{
  bool successful = true;

  //generate 50 near degenerate boundaries and compute shocks on them
  for (int i=0; i<500; i++)
  {
    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list;
    vsol_list.clear();

    //define a line of a random length at a random orientation
    double th = 2*vnl_math::pi*rnd_num;
    vgl_point_2d<double> s(500+rnd_num, 500+rnd_num);
    vgl_point_2d<double> e = _translatePoint(s, th, 200*rnd_pos_num);

    //define 4 points that project close to the end points of this line
    double l1 = 50*rnd_pos_num;
    vgl_point_2d<double> p1 = _translatePoint(s, th+vnl_math::pi_over_2, l1) + rnd_perturb;
    vgl_point_2d<double> p2 = _translatePoint(s, th-vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;
    vgl_point_2d<double> p3 = _translatePoint(e, th+vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;
    vgl_point_2d<double> p4 = _translatePoint(e, th-vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;

    //define 4 more points that will create a 2-4 degeneracy
    double th2 = th + vnl_math::pi + (3/8)*vnl_math::pi*rnd_num;
    vgl_point_2d<double> mp = midpoint(s, p1);
    vgl_point_2d<double> p5 = _translatePoint(mp, th2, l1/2);
    //vgl_point_2d<double> p6 = _translatePoint(s, th-vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;
    //vgl_point_2d<double> p7 = _translatePoint(e, th+vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;
    //vgl_point_2d<double> p8 = _translatePoint(e, th-vnl_math::pi_over_2, 50*rnd_pos_num) + rnd_perturb;

    #ifdef OFFLINE_TESTING
    vcl_cout << "PL_near_degen_3 ::: Test #" << i << " : ";
    vcl_cout << "[("<<s.x()<<", "<<s.y()<<")-("<<e.x()<<", "<<e.y()<<")] \n";
    vcl_cout << "("<<p1.x()<<", "<<p1.y()<<")("<<p2.x()<<", "<<p2.y()<<")("<<p3.x()<<", "<<p3.y()<<")("<<p4.x()<<", "<<p4.y()<<")\n";
    //vcl_cout << "("<<p1.x()<<", "<<p1.y()<<")("<<p2.x()<<", "<<p2.y()<<")("<<p3.x()<<", "<<p3.y()<<")("<<p4.x()<<", "<<p4.y()<<")\n";
    #endif

    vsol_point_2d_sptr sp = new vsol_point_2d(s);
    vsol_point_2d_sptr ep = new vsol_point_2d(e);
    vsol_line_2d_sptr line = new vsol_line_2d(sp, ep);
    
    vsol_point_2d_sptr pp1 = new vsol_point_2d(p1);
    vsol_point_2d_sptr pp2 = new vsol_point_2d(p2);
    vsol_point_2d_sptr pp3 = new vsol_point_2d(p3);
    vsol_point_2d_sptr pp4 = new vsol_point_2d(p4);

    vsol_point_2d_sptr pp5 = new vsol_point_2d(p5);

    vsol_list.push_back(line->cast_to_spatial_object());
    vsol_list.push_back(pp1->cast_to_spatial_object());
    vsol_list.push_back(pp2->cast_to_spatial_object());
    vsol_list.push_back(pp3->cast_to_spatial_object());
    vsol_list.push_back(pp4->cast_to_spatial_object());
    vsol_list.push_back(pp5->cast_to_spatial_object());

    #ifdef OFFLINE_TESTING
    //we should save this boundary so that we can debug it later
    dbsk2d_file_io::save_bnd_v3_0("degen_test.bnd", vsol_list);
    #endif

    //form a boundary class from this geometry
    dbsk2d_boundary_sptr bnd = dbsk2d_create_boundary( 
                                  vsol_list, //list of geometry
                                  false, //just use default partitioning
                                  0.0, 0.0,
                                  1, 1, //size num of cells
                                  1000.0, 1000.0, //dummy size
                                  true, //preprocess
                                  false); //break long lines

    //now compute shocks on it
    bool cur_successful = test_compute_shocks(bnd);

    if (!cur_successful){
      vcl_cout.precision(10);
      vcl_cout << "PL_near_degen_3 :: ";
      vcl_cout << "[("<<s.x()<<", "<<s.y()<<")-("<<e.x()<<", "<<e.y()<<")] \n";
      vcl_cout << "("<<p1.x()<<", "<<p1.y()<<")("<<p2.x()<<", "<<p2.y()<<")("<<p3.x()<<", "<<p3.y()<<")("<<p4.x()<<", "<<p4.y()<<")\n";
      //vcl_cout << "("<<p1.x()<<", "<<p1.y()<<")("<<p2.x()<<", "<<p2.y()<<")("<<p3.x()<<", "<<p3.y()<<")("<<p4.x()<<", "<<p4.y()<<")\n";
    }
    successful = successful && cur_successful;
  }
  return successful;
}

//L-L-P and P-L-L
bool test_LL_near_degen_1()
{
  bool successful = true;

  //generate 50 near degenerate boundaries and compute shocks on them
  for (int i=0; i<500; i++)
  {
    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list;
    vsol_list.clear();

    //define a line of a random length at a random orientation
    double th = 2*vnl_math::pi*rnd_num;
    vgl_point_2d<double> s1(500+rnd_num, 500+rnd_num);
    vgl_point_2d<double> e1 = _translatePoint(s1, th, 1.0+200*rnd_pos_num);
    
    //compute the midpoint of this line
    vgl_point_2d<double> mp = midpoint(s1, e1);
    
    //define a point that projects on to this mid point
    vgl_point_2d<double> mp2 = _translatePoint(mp, th+vnl_math::pi_over_2, 1.0+50*rnd_pos_num);

    //define a second line appromimately parallel to the first line
    double th2 = th+2*TO_EPSILON*rnd_num;
    vgl_point_2d<double> s2 = _translatePoint(mp2, th2, 0.5+100*rnd_pos_num);
    vgl_point_2d<double> e2 = _translatePoint(mp2, th2+vnl_math::pi, 0.5+100*rnd_pos_num);
    
    #ifdef OFFLINE_TESTING
    vcl_cout << "LL_near_degen_1 ::: Test #" << i << " : ";
    vcl_cout << "[("<<s1.x()<<", "<<s1.y()<<")-("<<e1.x()<<", "<<e1.y()<<")] ";
    vcl_cout << "[("<<s2.x()<<", "<<s2.y()<<")-("<<e2.x()<<", "<<e2.y()<<")]\n";
    #endif

    vsol_point_2d_sptr sp1 = new vsol_point_2d(s1);
    vsol_point_2d_sptr ep1 = new vsol_point_2d(e1);
    vsol_point_2d_sptr sp2 = new vsol_point_2d(s2);
    vsol_point_2d_sptr ep2 = new vsol_point_2d(e2);
    vsol_line_2d_sptr line1 = new vsol_line_2d(sp1, ep1);
    vsol_line_2d_sptr line2 = new vsol_line_2d(sp2, ep2);
    
    vsol_list.push_back(line1->cast_to_spatial_object());
    vsol_list.push_back(line2->cast_to_spatial_object());
    
    #ifdef OFFLINE_TESTING
    //we should save this boundary so that we can debug it later
    dbsk2d_file_io::save_bnd_v3_0("degen_test.bnd", vsol_list);
    #endif

    //form a boundary class from this geometry
    dbsk2d_boundary_sptr bnd = dbsk2d_create_boundary( 
                                  vsol_list, //list of geometry
                                  false, //just use default partitioning
                                  0.0, 0.0,
                                  1, 1, //size num of cells
                                  1000.0, 1000.0, //dummy size
                                  true, //preprocess
                                  false); //break long lines

    //now compute shocks on it
    bool cur_successful = test_compute_shocks(bnd);

    if (!cur_successful){
      vcl_cout.precision(10);
      vcl_cout << "LL_near_degen_1 :: ";
      vcl_cout << "[("<<s1.x()<<", "<<s1.y()<<")-("<<e1.x()<<", "<<e1.y()<<")] ";
      vcl_cout << "[("<<s2.x()<<", "<<s2.y()<<")-("<<e2.x()<<", "<<e2.y()<<")]\n";
    }
    successful = successful && cur_successful;
  }
  return successful;
}

//L-LP-P
bool test_LL_near_degen_2()
{
  bool successful = true;

  //generate 50 near degenerate boundaries and compute shocks on them
  for (int i=0; i<500; i++)
  {
    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list;
    vsol_list.clear();

    //define a line of a random length at a random orientation
    double th = 2*vnl_math::pi*rnd_num;
    double l = 1+200*rnd_pos_num;
    vgl_point_2d<double> s1(500+rnd_num, 500+rnd_num);
    vgl_point_2d<double> e1 = _translatePoint(s1, th, l);
    
    //define a second line appromimately parallel to the first line
    double th2 = th+2*TO_EPSILON*rnd_num;
    vgl_point_2d<double> s2 = _translatePoint(s1, th+vnl_math::pi_over_2, 1+50*rnd_pos_num) + rnd_perturb;
    vgl_point_2d<double> e2 = _translatePoint(s2, th2, l+D_EPSILON*rnd_num);
    
    #ifdef OFFLINE_TESTING
    vcl_cout << "LL_near_degen_2 ::: Test #" << i << " : ";
    vcl_cout << "[("<<s1.x()<<", "<<s1.y()<<")-("<<e1.x()<<", "<<e1.y()<<")] ";
    vcl_cout << "[("<<s2.x()<<", "<<s2.y()<<")-("<<e2.x()<<", "<<e2.y()<<")]\n";
    #endif

    vsol_point_2d_sptr sp1 = new vsol_point_2d(s1);
    vsol_point_2d_sptr ep1 = new vsol_point_2d(e1);
    vsol_point_2d_sptr sp2 = new vsol_point_2d(s2);
    vsol_point_2d_sptr ep2 = new vsol_point_2d(e2);
    vsol_line_2d_sptr line1 = new vsol_line_2d(sp1, ep1);
    vsol_line_2d_sptr line2 = new vsol_line_2d(sp2, ep2);
    
    vsol_list.push_back(line1->cast_to_spatial_object());
    vsol_list.push_back(line2->cast_to_spatial_object());
    
    #ifdef OFFLINE_TESTING
    //we should save this boundary so that we can debug it later
    dbsk2d_file_io::save_bnd_v3_0("degen_test.bnd", vsol_list);
    #endif

    //form a boundary class from this geometry
    dbsk2d_boundary_sptr bnd = dbsk2d_create_boundary( 
                                  vsol_list, //list of geometry
                                  false, //just use default partitioning
                                  0.0, 0.0,
                                  1, 1, //size num of cells
                                  1000.0, 1000.0, //dummy size
                                  true, //preprocess
                                  false); //break long lines

    //now compute shocks on it
    bool cur_successful = test_compute_shocks(bnd);

    if (!cur_successful){
      vcl_cout.precision(10);
      vcl_cout << "LL_near_degen_2 :: ";
      vcl_cout << "[("<<s1.x()<<", "<<s1.y()<<")-("<<e1.x()<<", "<<e1.y()<<")] ";
      vcl_cout << "[("<<s2.x()<<", "<<s2.y()<<")-("<<e2.x()<<", "<<e2.y()<<")]\n";
    }
    successful = successful && cur_successful;
  }
  return successful;
}

//L-PP-L
bool test_LL_near_degen_3()
{
  bool successful = true;

  //generate 50 near degenerate boundaries and compute shocks on them
  for (int i=0; i<500; i++)
  {
    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list;
    vsol_list.clear();

    //define a line of a random length at a random orientation
    double th = 2*vnl_math::pi*rnd_num;
    double l = 1+200*rnd_pos_num;
    vgl_point_2d<double> s1(500+rnd_num, 500+rnd_num);
    vgl_point_2d<double> e1 = _translatePoint(s1, th, l);
    
    //define a second line appromimately parallel to the first line
    double th2 = th+5*TO_EPSILON*rnd_num;
    vgl_point_2d<double> s2 = _translatePoint(e1, th+vnl_math::pi_over_2, 1+50*rnd_pos_num) + rnd_perturb;
    vgl_point_2d<double> e2 = _translatePoint(s2, th2, l);
    
    #ifdef OFFLINE_TESTING
    vcl_cout << "LL_near_degen_3 ::: Test #" << i << " : ";
    vcl_cout << "[("<<s1.x()<<", "<<s1.y()<<")-("<<e1.x()<<", "<<e1.y()<<")] ";
    vcl_cout << "[("<<s2.x()<<", "<<s2.y()<<")-("<<e2.x()<<", "<<e2.y()<<")]\n";
    #endif

    vsol_point_2d_sptr sp1 = new vsol_point_2d(s1);
    vsol_point_2d_sptr ep1 = new vsol_point_2d(e1);
    vsol_point_2d_sptr sp2 = new vsol_point_2d(s2);
    vsol_point_2d_sptr ep2 = new vsol_point_2d(e2);
    vsol_line_2d_sptr line1 = new vsol_line_2d(sp1, ep1);
    vsol_line_2d_sptr line2 = new vsol_line_2d(sp2, ep2);
    
    vsol_list.push_back(line1->cast_to_spatial_object());
    vsol_list.push_back(line2->cast_to_spatial_object());
    
    #ifdef OFFLINE_TESTING
    //we should save this boundary so that we can debug it later
    dbsk2d_file_io::save_bnd_v3_0("degen_test.bnd", vsol_list);
    #endif

    //form a boundary class from this geometry
    dbsk2d_boundary_sptr bnd = dbsk2d_create_boundary( 
                                  vsol_list, //list of geometry
                                  false, //just use default partitioning
                                  0.0, 0.0,
                                  1, 1, //size num of cells
                                  1000.0, 1000.0, //dummy size
                                  true, //preprocess
                                  false); //break long lines

    //now compute shocks on it
    bool cur_successful = test_compute_shocks(bnd);

    if (!cur_successful){
      vcl_cout.precision(10);
      vcl_cout << "LL_near_degen_3 :: ";
      vcl_cout << "[("<<s1.x()<<", "<<s1.y()<<")-("<<e1.x()<<", "<<e1.y()<<")] ";
      vcl_cout << "[("<<s2.x()<<", "<<s2.y()<<")-("<<e2.x()<<", "<<e2.y()<<")]\n";
    }
    successful = successful && cur_successful;
  }
  return successful;
}

//L-LP-L
bool test_LL_near_degen_4()
{
  bool successful = true;

  //generate 50 near degenerate boundaries and compute shocks on them
  for (int i=0; i<500; i++)
  {
    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list;
    vsol_list.clear();

    //define a line of a random length at a random orientation
    double th = 2*vnl_math::pi*rnd_num;
    double l = 1+200*rnd_pos_num;
    vgl_point_2d<double> s1(500+rnd_num, 500+rnd_num);
    vgl_point_2d<double> e1 = _translatePoint(s1, th, l);
    
    //define a second line appromimately collinear to the first line
    double th2 = th+2*CONTACT_EPSILON*rnd_num;
    vgl_point_2d<double> s2 = e1;
    vgl_point_2d<double> e2 = _translatePoint(s2, th2, l);
    
    //define a third line approximately parallel to the first line s.t. its end point projects
    //to the end point of the first line
    double th3 = th+2*TO_EPSILON*rnd_num;
    vgl_point_2d<double> s3 = _translatePoint(e1, th+vnl_math::pi_over_2, 1+50*rnd_pos_num) + rnd_perturb;
    vgl_point_2d<double> e3 = _translatePoint(s3, th3+vnl_math::pi, l+D_EPSILON*rnd_num);
    
    #ifdef OFFLINE_TESTING
    vcl_cout << "LL_near_degen_4 ::: Test #" << i << " : ";
    vcl_cout << "[("<<s1.x()<<", "<<s1.y()<<")-("<<e1.x()<<", "<<e1.y()<<")] ";
    vcl_cout << "[("<<s2.x()<<", "<<s2.y()<<")-("<<e2.x()<<", "<<e2.y()<<")] ";
    vcl_cout << "[("<<s3.x()<<", "<<s3.y()<<")-("<<e3.x()<<", "<<e3.y()<<")]\n";
    #endif

    vsol_point_2d_sptr sp1 = new vsol_point_2d(s1);
    vsol_point_2d_sptr ep1 = new vsol_point_2d(e1);
    vsol_point_2d_sptr sp2 = new vsol_point_2d(s2);
    vsol_point_2d_sptr ep2 = new vsol_point_2d(e2);
    vsol_point_2d_sptr sp3 = new vsol_point_2d(s3);
    vsol_point_2d_sptr ep3 = new vsol_point_2d(e3);
    vsol_line_2d_sptr line1 = new vsol_line_2d(sp1, ep1);
    vsol_line_2d_sptr line2 = new vsol_line_2d(sp2, ep2);
    vsol_line_2d_sptr line3 = new vsol_line_2d(sp3, ep3);
    
    vsol_list.push_back(line1->cast_to_spatial_object());
    vsol_list.push_back(line2->cast_to_spatial_object());
    vsol_list.push_back(line3->cast_to_spatial_object());
    
    #ifdef OFFLINE_TESTING
    //we should save this boundary so that we can debug it later
    dbsk2d_file_io::save_bnd_v3_0("degen_test.bnd", vsol_list);
    #endif

    //form a boundary class from this geometry
    dbsk2d_boundary_sptr bnd = dbsk2d_create_boundary( 
                                  vsol_list, //list of geometry
                                  false, //just use default partitioning
                                  0.0, 0.0,
                                  1, 1, //size num of cells
                                  1000.0, 1000.0, //dummy size
                                  true, //preprocess
                                  false); //break long lines

    //now compute shocks on it
    bool cur_successful = test_compute_shocks(bnd);

    if (!cur_successful){
      vcl_cout.precision(10);
      vcl_cout << "LL_near_degen_4 :: ";
      vcl_cout << "[("<<s1.x()<<", "<<s1.y()<<")-("<<e1.x()<<", "<<e1.y()<<")] ";
      vcl_cout << "[("<<s2.x()<<", "<<s2.y()<<")-("<<e2.x()<<", "<<e2.y()<<")] ";
      vcl_cout << "[("<<s3.x()<<", "<<s3.y()<<")-("<<e3.x()<<", "<<e3.y()<<")]\n";
    }
    successful = successful && cur_successful;
  }
  return successful;
}

//L-LL-L
bool test_LL_near_degen_5()
{
  bool successful = true;

  //generate 50 near degenerate boundaries and compute shocks on them
  for (int i=0; i<500; i++)
  {
    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list;
    vsol_list.clear();

    //define a line of a random length at a random orientation
    double th = 2*vnl_math::pi*rnd_num;
    double l = 1+200*rnd_pos_num;
    vgl_point_2d<double> s1(500+rnd_num, 500+rnd_num);
    vgl_point_2d<double> e1 = _translatePoint(s1, th, l);
    
    //define a second line approximately collinear to the first line
    double th2 = th+5*CONTACT_EPSILON*rnd_num;
    vgl_point_2d<double> s2 = e1;
    vgl_point_2d<double> e2 = _translatePoint(s2, th2, l+D_EPSILON*rnd_num);
    
    //define a third line approximately parallel to the first line s.t. its end point projects
    //to the end point of the first line
    double th3 = th+5*TO_EPSILON*rnd_num;
    vgl_point_2d<double> s3 = _translatePoint(e1, th+vnl_math::pi_over_2, 1+50*rnd_pos_num) + rnd_perturb;
    vgl_point_2d<double> e3 = _translatePoint(s3, th3+vnl_math::pi, l+D_EPSILON*rnd_num);
    
    //define a fourth line approximately collinear to the third line
    double th4 = th3+5*CONTACT_EPSILON*rnd_num;
    vgl_point_2d<double> s4 = s3;
    vgl_point_2d<double> e4 = _translatePoint(s4, th4, l+D_EPSILON*rnd_num);
    
    #ifdef OFFLINE_TESTING
    vcl_cout << "LL_near_degen_5 ::: Test #" << i << " : ";
    vcl_cout << "[("<<s1.x()<<", "<<s1.y()<<")-("<<e1.x()<<", "<<e1.y()<<")] ";
    vcl_cout << "[("<<s2.x()<<", "<<s2.y()<<")-("<<e2.x()<<", "<<e2.y()<<")] ";
    vcl_cout << "[("<<s3.x()<<", "<<s3.y()<<")-("<<e3.x()<<", "<<e3.y()<<")] ";
    vcl_cout << "[("<<s4.x()<<", "<<s4.y()<<")-("<<e4.x()<<", "<<e4.y()<<")]\n";
    #endif

    vsol_point_2d_sptr sp1 = new vsol_point_2d(s1);
    vsol_point_2d_sptr ep1 = new vsol_point_2d(e1);
    vsol_point_2d_sptr sp2 = new vsol_point_2d(s2);
    vsol_point_2d_sptr ep2 = new vsol_point_2d(e2);
    vsol_point_2d_sptr sp3 = new vsol_point_2d(s3);
    vsol_point_2d_sptr ep3 = new vsol_point_2d(e3);
    vsol_point_2d_sptr sp4 = new vsol_point_2d(s4);
    vsol_point_2d_sptr ep4 = new vsol_point_2d(e4);
    vsol_line_2d_sptr line1 = new vsol_line_2d(sp1, ep1);
    vsol_line_2d_sptr line2 = new vsol_line_2d(sp2, ep2);
    vsol_line_2d_sptr line3 = new vsol_line_2d(sp3, ep3);
    vsol_line_2d_sptr line4 = new vsol_line_2d(sp4, ep4);
    
    vsol_list.push_back(line1->cast_to_spatial_object());
    vsol_list.push_back(line2->cast_to_spatial_object());
    vsol_list.push_back(line3->cast_to_spatial_object());
    vsol_list.push_back(line4->cast_to_spatial_object());
    
    #ifdef OFFLINE_TESTING
    //we should save this boundary so that we can debug it later
    dbsk2d_file_io::save_bnd_v3_0("degen_test.bnd", vsol_list);
    #endif

    //form a boundary class from this geometry
    dbsk2d_boundary_sptr bnd = dbsk2d_create_boundary( 
                                  vsol_list, //list of geometry
                                  false, //just use default partitioning
                                  0.0, 0.0,
                                  1, 1, //size num of cells
                                  1000.0, 1000.0, //dummy size
                                  true, //preprocess
                                  false); //break long lines

    //now compute shocks on it
    bool cur_successful = test_compute_shocks(bnd);

    if (!cur_successful){
      vcl_cout.precision(10);
      vcl_cout << "LL_near_degen_5 :: ";
      vcl_cout << "[("<<s1.x()<<", "<<s1.y()<<")-("<<e1.x()<<", "<<e1.y()<<")] ";
      vcl_cout << "[("<<s2.x()<<", "<<s2.y()<<")-("<<e2.x()<<", "<<e2.y()<<")] ";
      vcl_cout << "[("<<s3.x()<<", "<<s3.y()<<")-("<<e3.x()<<", "<<e3.y()<<")] ";
      vcl_cout << "[("<<s4.x()<<", "<<s4.y()<<")-("<<e4.x()<<", "<<e4.y()<<")]\n";
    }
    successful = successful && cur_successful;
  }
  return successful;
}

//test shock computation on randomly generated near degeneracies
bool test_shock_computation_on_degeneracies()
{
  // initialize random generator
  better_rand.reseed(vcl_time(0));

  bool successful = true;

  //perform all the randomized tests
  successful = successful && test_PL_near_degen_1();
  successful = successful && test_PL_near_degen_2();
  successful = successful && test_PL_near_degen_3();
  successful = successful && test_LL_near_degen_1();
  successful = successful && test_LL_near_degen_2();
  successful = successful && test_LL_near_degen_3();
  successful = successful && test_LL_near_degen_4();
  successful = successful && test_LL_near_degen_5();


  return successful;
}

vcl_string file_base;

// Compare the results of loading different files with the true data
// that's supposed to be in those files.
bool load_bnd_and_compute_shock()
{
  //TODO
  //test the allocation of candidate sources in a N^2 loop vs a preallocated 
  //array
  //
  //should improve efficiency quite a bit
  //the other thing is to not make an object at all until source allocation time 
  // and just make a list of candidate pairs to call a candidate source list

  dbsk2d_boundary_sptr bnd = new dbsk2d_boundary();

  vcl_string filename = file_base + "test_bnd_files/test_file2.bnd";
  //bnd->LoadFile(filename.c_str());

  //now compute shocks on it
  return test_compute_shocks(bnd);
}

int test_compute_ishock_main( int argc, char* argv[] )
{
  if ( argc >= 2 ) {
    file_base = argv[1];
#ifdef VCL_WIN32
    file_base += "\\";
#else
    file_base += "/";
#endif
  }

  testlib_test_start(" ishock computation test");
  testlib_test_begin( " dbsk2d_ishock_detection " );
  testlib_test_perform ( test_shock_computation_on_degeneracies() );
  //testlib_test_perform( load_bnd_and_compute_shock());

  return testlib_test_summary();
}
