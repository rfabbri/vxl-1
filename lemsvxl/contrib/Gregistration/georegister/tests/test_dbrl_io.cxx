#include <testlib/testlib_test.h>
#include <georegister/dbrl_feature_point_sptr.h>
#include <georegister/dbrl_feature_point.h>
#include <georegister/dbrl_feature_point_tangent.h>
#include <georegister/dbrl_feature_point_tangent_sptr.h>
#include <georegister/dbrl_correspondence.h>
#include <georegister/dbrl_transformation.h>
#include <georegister/dbrl_transformation_sptr.h>
#include <georegister/dbrl_rigid_transformation.h>
#include <georegister/dbrl_affine_transformation.h>
#include <georegister/dbrl_thin_plate_spline_transformation.h>
#include <georegister/dbrl_estimator_sptr.h>
#include <georegister/dbrl_estimator_point_affine.h>
#include <georegister/dbrl_estimator_thin_plate_spline.h>
#include <georegister/dbrl_match_set_sptr.h>
#include <vnl/vnl_math.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>

void test_dbrl_feature_io()
{

  vsl_add_to_binary_loader(dbrl_feature_point());
    vsl_add_to_binary_loader(dbrl_feature_point_tangent());


  dbrl_feature_point_tangent_sptr p=new dbrl_feature_point_tangent(10.0,20.0,0);
  vsl_b_ofstream bp_out("test_dbrl_feature_io.tmp");
  TEST("Created test_dbrl_feature_io.tmp for writing",(!bp_out), false);

  dbrl_feature_sptr pf=p.ptr();
  vsl_b_write(bp_out, pf);
  bp_out.close();

  dbrl_feature_sptr pinsptr;
  // binary test input file stream
  vsl_b_ifstream bp_in("test_dbrl_feature_io.tmp");
  TEST("Opened test_dbrl_feature_io.tmp for reading",(!bp_in), false);
  vsl_b_read(bp_in, pinsptr);
  bp_in.close();


  TEST("Name of the class", "dbrl_feature_point", pinsptr->is_a());
  //TEST("y for smart pointer", p->location()[1], pinsptr->location()[1]);
  // remove the temporary file
  vpl_unlink ("test_dbrl_feature_point_io.tmp");


}
void test_dbrl_feature_point_io()
{


  dbrl_feature_point_sptr p=new dbrl_feature_point(10.0,20.0);
  vsl_b_ofstream bp_out("test_dbrl_feature_point_io.tmp");
  TEST("Created test_dbrl_feature_point_io.tmp for writing",(!bp_out), false);

  vsl_b_write(bp_out, p);
  p->b_write(bp_out);

  bp_out.close();

  dbrl_feature_point_sptr pinsptr;
  dbrl_feature_point pin;
  // binary test input file stream
  vsl_b_ifstream bp_in("test_dbrl_feature_point_io.tmp");
  TEST("Opened test_dbrl_feature_point_io.tmp for reading",(!bp_in), false);
  vsl_b_read(bp_in, pinsptr);
  pin.b_read(bp_in);
  bp_in.close();


  TEST("x for smart pointer", p->location()[0], pinsptr->location()[0]);
  TEST("y for smart pointer", p->location()[1], pinsptr->location()[1]);
  // remove the temporary file
  vpl_unlink ("test_dbrl_feature_point_io.tmp");


}
void test_dbrl_feature_point_tangent_io()
{


  dbrl_feature_point_tangent * p=new dbrl_feature_point_tangent(10.0,20.0,0.5);
  vsl_b_ofstream bp_out("test_dbrl_feature_point_tangent_io.tmp");
  TEST("Created test_dbrl_feature_point_tangent_io.tmp for writing",(!bp_out), false);

  vsl_b_write(bp_out, p);
  p->b_write(bp_out);

  bp_out.close();

  dbrl_feature_point_tangent * pinsptr;
  dbrl_feature_point_tangent pin;
  // binary test input file stream
  vsl_b_ifstream bp_in("test_dbrl_feature_point_io.tmp");
  TEST("Opened test_dbrl_feature_point_io.tmp for reading",(!bp_in), false);
  vsl_b_read(bp_in, pinsptr);
  pin.b_read(bp_in);
  bp_in.close();


  TEST("x for smart pointer", p->location()[0], pinsptr->location()[0]);
  TEST("y for smart pointer", p->location()[1], pinsptr->location()[1]);
  TEST("dir for smart pointer", p->dir(), pinsptr->dir());
  // remove the temporary file
  vpl_unlink ("test_dbrl_feature_point_io.tmp");


}
void test_dbrl_correspondence_io()
{


  vnl_matrix<double> M(3,2);
  M(0,0)=0.3;M(0,1)=0.4;
  M(1,0)=0.1;M(1,1)=0.3;
  M(2,0)=0.7;M(2,1)=0.1;

  vnl_vector_fixed<double,2> mrows_(0.2,0.1);
  vnl_vector_fixed<double,3> mcols_(0.2,0.7,0.2);


  dbrl_correspondence p(M);
  p.setinitialoutlier(0.2);
  


  vsl_b_ofstream bp_out("test_dbrl_correspondence_io.tmp");
  TEST("Created test_dbrl_correspondence_io.tmp for writing",(!bp_out), false);

  vsl_b_write(bp_out, p);
  bp_out.close();

  dbrl_correspondence  pin;
  // binary test input file stream
  vsl_b_ifstream bp_in("test_dbrl_correspondence_io.tmp");
  TEST("Opened test_dbrl_correspondence_io.tmp for reading",(!bp_in), false);
  vsl_b_read(bp_in, pin);
  bp_in.close();


  TEST("correspondence Matrix", pin.M(), M);
  // remove the temporary file
  vpl_unlink ("test_dbrl_correspondence_io.tmp");


}

void test_dbrl_transformation_io()
{

    vsl_add_to_binary_loader(dbrl_rigid_transformation());
    vsl_add_to_binary_loader(dbrl_affine_transformation());
    vsl_add_to_binary_loader(dbrl_thin_plate_spline_transformation());


    vnl_vector_fixed<double,2> t(10.0,20.0);
    vnl_matrix<double> R(2,2);
    R(0,0)=0.707;R(0,1)=-0.707;
    R(1,0)=0.707;R(1,1)=0.707;
  
    dbrl_rigid_transformation *prigid=new dbrl_rigid_transformation(t,R,1.0);

    vnl_matrix<double> A_(3,3);
    A_(0,0)=0.87;A_(0,1)=0.23;A_(0,2)=10;
    A_(1,0)=0.27;A_(1,1)=0.77;A_(0,2)=-10;
    A_(2,0)=0.0;A_(2,1)=0.0;A_(0,2)=1;

    dbrl_affine_transformation *paffine=new dbrl_affine_transformation(A_);

    vsl_b_ofstream bp_out("test_dbrl_transformation_io.tmp");
    TEST("Created test_dbrl_transformation_io.tmp for writing",(!bp_out), false);

    dbrl_transformation_sptr p1=prigid;
    vsl_b_write(bp_out, p1);
    dbrl_transformation_sptr p2=paffine;
    vsl_b_write(bp_out, p2);
    bp_out.close();

    dbrl_transformation_sptr pinsptr1,pinsptr2;
    // binary test input file stream
    vsl_b_ifstream bp_in("test_dbrl_transformation_io.tmp");
    TEST("Opened test_dbrl_transformation_io.tmp for reading",(!bp_in), false);
    vsl_b_read(bp_in, pinsptr1);
    TEST("Name(Rigid)", "dbrl_rigid_transformation", pinsptr1->is_a());
    vsl_b_read(bp_in, pinsptr2);
    TEST("Name(Affine)", "dbrl_affine_transformation", pinsptr2->is_a());
    bp_in.close();
    vpl_unlink ("test_dbrl_feature_point_io.tmp");
}
void test_dbrl_estimator_io()
{

    vsl_add_to_binary_loader(dbrl_estimator_point_affine());
    vsl_add_to_binary_loader(dbrl_estimator_point_thin_plate_spline());

  
    dbrl_estimator_point_affine *paffine=new dbrl_estimator_point_affine();
    dbrl_estimator_point_thin_plate_spline *ptps=new dbrl_estimator_point_thin_plate_spline();


    vsl_b_ofstream bp_out("test_dbrl_estimator_io.tmp");
    TEST("Created test_dbrl_estimator_io.tmp for writing",(!bp_out), false);
    dbrl_estimator_sptr p1=paffine;
    vsl_b_write(bp_out, p1);
    dbrl_estimator_sptr p2=ptps;
    vsl_b_write(bp_out, p2);
    bp_out.close();

    dbrl_estimator_sptr psptr1,psptr2;
    // binary test input file stream
    vsl_b_ifstream bp_in("test_dbrl_estimator_io.tmp");
    TEST("Opened test_dbrl_estimator_io.tmp for reading",(!bp_in), false);
    vsl_b_read(bp_in, psptr1);
    TEST("Name(Affine)", "dbrl_estimator_point_affine", psptr1->is_a());
    vsl_b_read(bp_in, psptr2);
    TEST("Name(TPS)", "dbrl_estimator_point_thin_plate_spline", psptr2->is_a());
    bp_in.close();
    vpl_unlink ("test_dbrl_estimator_io.tmp");
}
void test_dbrl_match_set_io()
{

    vsl_add_to_binary_loader(dbrl_estimator_point_affine());
    vsl_add_to_binary_loader(dbrl_estimator_point_thin_plate_spline());
    vsl_add_to_binary_loader(dbrl_affine_transformation());
    vsl_add_to_binary_loader(dbrl_thin_plate_spline_transformation());

    vcl_vector<dbrl_feature_sptr> f1;
    vcl_vector<dbrl_feature_sptr> f2;

    f1.push_back(new dbrl_feature_point(0.2,0.4));
    f1.push_back(new dbrl_feature_point(0.1,0.5));
    f1.push_back(new dbrl_feature_point(0.3,0.5));
    f1.push_back(new dbrl_feature_point(0.3,0.7));
    f1.push_back(new dbrl_feature_point(0.1,0.7));
    f1.push_back(new dbrl_feature_point(0.2,0.8));

    f2.push_back(new dbrl_feature_point(0.25,0.4+0.02));
    f2.push_back(new dbrl_feature_point(0.15-0.01,0.5));
    f2.push_back(new dbrl_feature_point(0.35-0.01,0.5));
    f2.push_back(new dbrl_feature_point(0.35,0.7+0.01));
    f2.push_back(new dbrl_feature_point(0.15,0.7+0.015));
    f2.push_back(new dbrl_feature_point(0.25+0.01,0.8));

    dbrl_estimator_point_thin_plate_spline *ptps=new dbrl_estimator_point_thin_plate_spline();
    vnl_matrix<double> M(6,6);
    M.set_identity();
    dbrl_correspondence Mcorr(M);

    ptps->set_lambda1(0.0);
    ptps->set_lambda2(0.0);
    dbrl_transformation_sptr p=ptps->estimate(f1,f2,Mcorr);

    dbrl_match_set_sptr pmatch=new dbrl_match_set(Mcorr,p,ptps);

    vsl_b_ofstream bp_out("test_match_set_io.tmp");
    TEST("Created test_match_set_io.tmp for writing",(!bp_out), false);
    vsl_b_write(bp_out, pmatch);
    bp_out.close();

    dbrl_match_set_sptr pin;
    // binary test input file stream
    vsl_b_ifstream bp_in("test_match_set_io.tmp");
    TEST("Opened test_match_set_io.tmp for reading",(!bp_in), false);
    vsl_b_read(bp_in, pin);


    TEST("Residual ",pin->get_estimator()->residual(f1,f2,pin->get_correspondence(),pin->get_transformation()),ptps->residual(f1,f2,Mcorr,p));
    
    bp_in.close();
    vpl_unlink ("test_match_set_io.tmp");
}
MAIN( test_dbrl_io )
{
START ("dbrl_io");
test_dbrl_feature_io();

test_dbrl_feature_point_io();
test_dbrl_correspondence_io();
test_dbrl_transformation_io();
test_dbrl_estimator_io();
test_dbrl_match_set_io();
SUMMARY();
}
