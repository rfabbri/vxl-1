#include <testlib/testlib_test.h>
#include <dbsta/dbsta_gaussian_indep.h>
#include <dbsta/dbsta_gaussian_full.h>
#include <dbsta/dbsta_gaussian_sphere.h>
#include <dbsta/dbsta_mixture.h>
#include <vnl/vnl_math.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vpl/vpl.h>
#include <dbsta/dbsta_gaussian.h>
#include <dbsta/dbsta_distribution.h>

//template void vsl_add_to_binary_loader(dbsta_distribution<float> const& b);
//template void vsl_add_to_binary_loader(dbsta_distribution<double> const& b);
template <class T>
void test_dbsta_io(T epsilon, const vcl_string& type_name)
{

  vsl_add_to_binary_loader(dbsta_gaussian_indep<T,3>());
  vsl_add_to_binary_loader(dbsta_gaussian_full<T,3>());
  vsl_add_to_binary_loader(dbsta_gaussian_sphere<T,3>());
  vsl_add_to_binary_loader(dbsta_mixture<T>());

 vcl_cout << "Testing I/O for dbsta_gaussian_full\n";

  vnl_vector_fixed<T,3> mean(T(1.0), T(2.0), T(4.0));
  vnl_matrix<T> covar(3,3,T(0));

  covar(0,0) = (T)0.5 ;covar(0,1) = (T)0.2;covar(0,2) = (T)0.1;
  covar(1,0) = (T)0.05;covar(1,1) = (T)0.3;covar(1,2) = (T)0.1;
  covar(2,0) = (T)0.1 ;covar(2,1) = (T)0.1;covar(2,2) = (T)0.4;

  dbsta_gaussian_full<T,3> gauss(mean, covar);
  dbsta_gaussian_full<T,3> *gaussa=new dbsta_gaussian_full<T,3>(mean, covar);

 

  vsl_b_ofstream bp_out("test_dbsta_gaussian_full_io.tmp");
  TEST("Created test_dbsta_gaussian_full_io.tmp for writing",(!bp_out), false);

  vsl_b_write(bp_out, gaussa);
  gauss.b_write(bp_out);

  bp_out.close();

  // binary test input file stream
  vsl_b_ifstream bp_in("test_dbsta_gaussian_full_io.tmp");
  TEST("Opened test_dbsta_gaussian_full_io.tmp for reading",(!bp_in), false);

  dbsta_gaussian_full<T,3> *gaussa_in = new dbsta_gaussian_full<T,3>();
  dbsta_gaussian_full<T,3> gauss_in;
  vsl_b_read(bp_in, gaussa_in);
  gauss_in.b_read(bp_in);
  bp_in.close();


  TEST(("mean <"+type_name+">").c_str(), gauss_in.mean(), mean);
  TEST(("covar <"+type_name+">").c_str(), gauss_in.covar(), covar);

  TEST(("mean (pointer)<"+type_name+">").c_str(), gaussa_in->mean(), mean);
  TEST(("covar (pointer) <"+type_name+">").c_str(), gaussa_in->covar(), covar);

  // remove the temporary file
  vpl_unlink ("test_dbsta_gaussian_full_io.tmp");

   //: independent gaussian

  vcl_cout << "Testing I/O for dbsta_gaussian_indep\n";

  vnl_vector_fixed<T,3> indep_mean(T(1.0), T(2.0), T(4.0));
  vnl_vector_fixed<T,3> indep_diag_covar(T(0.5), T(1.0), T(0.5));

  vnl_matrix<T> indep_covar(3,3,T(0));
  indep_covar(0,0) = indep_diag_covar[0];
  indep_covar(1,1) = indep_diag_covar[1];
  indep_covar(2,2) = indep_diag_covar[2];
  
  dbsta_gaussian_indep<T,3> indep_gauss(indep_mean, indep_diag_covar);
  dbsta_gaussian_indep<T,3> *indep_gaussa=new dbsta_gaussian_indep<T,3>(indep_mean, indep_diag_covar);



  vsl_b_ofstream indep_out("test_dbsta_gaussian_indep_io.tmp");
  TEST("Created test_dbsta_gaussian_indep_io.tmp for writing",(!indep_out), false);

  vsl_b_write(indep_out, indep_gaussa);
  indep_gauss.b_write(indep_out);

  indep_out.close();

  // binary test input file stream
  vsl_b_ifstream indep_in("test_dbsta_gaussian_indep_io.tmp");
  TEST("Opened test_dbsta_dbsta_gaussian_indep.tmp for reading",(!indep_in), false);

  dbsta_gaussian_indep<T,3> *gaussa_indep = new dbsta_gaussian_indep<T,3>();
  dbsta_gaussian_indep<T,3> gauss_indep;
  vsl_b_read(indep_in, gaussa_indep);
  gauss_indep.b_read(indep_in);
  indep_in.close();


  TEST(("mean <"+type_name+">").c_str(), gauss_indep.mean(), indep_mean);
  TEST(("covar <"+type_name+">").c_str(), gauss_indep.covar(), indep_covar);

  TEST(("mean (pointer)<"+type_name+">").c_str(), gaussa_indep->mean(), indep_mean);
  TEST(("covar (pointer) <"+type_name+">").c_str(), gaussa_indep->covar(), indep_covar);

  // remove the temporary file
  vpl_unlink ("test_dbsta_gaussian_indep_io.tmp");


  //: sphere gaussian

  vcl_cout << "Testing I/O for dbsta_gaussian_sphere\n";

  vnl_vector_fixed<T,3> sphere_mean(T(1.0), T(2.0), T(4.0));
  T var=T(0.5);

  dbsta_gaussian_sphere<T,3> sphere_gauss(sphere_mean, var);
  dbsta_gaussian_sphere<T,3> *sphere_gaussa=new dbsta_gaussian_sphere<T,3>(sphere_mean, var);


  vsl_b_ofstream sphere_out("test_dbsta_gaussian_sphere_io.tmp");
  TEST("Created test_dbsta_gaussian_sphere_io.tmp for writing",(!sphere_out), false);

  vsl_b_write(sphere_out, sphere_gaussa);
  sphere_gauss.b_write(sphere_out);

  sphere_out.close();

  // binary test input file stream
  vsl_b_ifstream sphere_in("test_dbsta_gaussian_sphere_io.tmp");
  TEST("Opened test_dbsta_dbsta_gaussian_sphere.tmp for reading",(!sphere_in), false);

  dbsta_gaussian_sphere<T,3> *gaussa_sphere = new dbsta_gaussian_sphere<T,3>();
  dbsta_gaussian_sphere<T,3> gauss_sphere;
  vsl_b_read(sphere_in, gaussa_sphere);
  gauss_sphere.b_read(sphere_in);
  sphere_in.close();


  TEST(("mean <"+type_name+">").c_str(), gauss_sphere.mean(), sphere_mean);
  TEST(("covar <"+type_name+">").c_str(), gauss_sphere.var(), var);

  TEST(("mean (pointer)<"+type_name+">").c_str(), gaussa_sphere->mean(), sphere_mean);
  TEST(("covar (pointer) <"+type_name+">").c_str(), gaussa_sphere->var(), var);

  // remove the temporary file
  vpl_unlink ("test_dbsta_gaussian_sphere_io.tmp");

  //: Mixture of Gaussian

  vcl_cout << "Testing I/O for dbsta_mixture\n";


  vnl_vector_fixed<T,3> indep_mean1(T(1.0), T(2.0), T(4.0));
  vnl_vector_fixed<T,3> indep_diag_covar1(T(0.5), T(1.0), T(0.5));

  vnl_matrix<T> indep_covar1(3,3,T(0));
  indep_covar1(0,0) = indep_diag_covar1[0];
  indep_covar1(1,1) = indep_diag_covar1[1];
  indep_covar1(2,2) = indep_diag_covar1[2];
  
  dbsta_gaussian_indep<T,3> indep_gauss1(indep_mean1, indep_diag_covar1);

  vnl_vector_fixed<T,3> indep_mean2(T(3.0), T(5.0), T(7.0));
  vnl_vector_fixed<T,3> indep_diag_covar2(T(0.4), T(0.6), T(0.8));

  vnl_matrix<T> indep_covar2(3,3,T(0));
  indep_covar2(0,0) = indep_diag_covar2[0];
  indep_covar2(1,1) = indep_diag_covar2[1];
  indep_covar2(2,2) = indep_diag_covar2[2];
  
  dbsta_gaussian_indep<T,3> indep_gauss2(indep_mean2, indep_diag_covar2);

  dbsta_mixture<T> mog;

  mog.insert(indep_gauss1,0.7);
  mog.insert(indep_gauss2,0.5);

  vsl_b_ofstream mix_out("test_dbsta_mixture_io.tmp");
  TEST("Created test_dbsta_mixture_io.tmp for writing",(!mix_out), false);

  mog.b_write(mix_out);
  mix_out.close();

  // binary test input file stream
  vsl_b_ifstream mix_in("test_dbsta_mixture_io.tmp");
  TEST("Opened test_dbsta_mixture_io.tmp for reading",(!mix_in), false);

  dbsta_mixture<T> mogin;
  mogin.b_read(mix_in);
  mix_in.close();


  const dbsta_gaussian<T> *dbgauss1=dynamic_cast<const dbsta_gaussian<T> *>(&mogin.distribution(0));
  const dbsta_gaussian<T> *dbgauss2=dynamic_cast<const dbsta_gaussian<T> *>(&mogin.distribution(1));


  TEST(("mean 1st component<"+type_name+">").c_str(), dbgauss1->mean(), indep_mean1);
  TEST(("covar 1st component<"+type_name+">").c_str(),dbgauss1->covar(), indep_covar1);

  TEST(("mean 2nd component<"+type_name+">").c_str(),  dbgauss2->mean(), indep_mean2);
  TEST(("covar 2nd component <"+type_name+">").c_str(), dbgauss2->covar(), indep_covar2);

  // remove the temporary file
  vpl_unlink ("test_dbsta_mixture_io.tmp");

}

MAIN( test_dbsta_io )
{
  START ("dbsta_io");
  test_dbsta_io(float(1e-5),"float");
  test_dbsta_io(double(1e-14),"double");
  SUMMARY();
}
