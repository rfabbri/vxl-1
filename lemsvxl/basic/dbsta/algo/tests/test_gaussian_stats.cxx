#include <testlib/testlib_test.h>
#include <dbsta/algo/dbsta_gaussian_stats.h>
#include <dbsta/dbsta_gaussian_full.h>
#include <dbsta/dbsta_gaussian_indep.h>
#include <dbsta/dbsta_gaussian_sphere.h>
#include <vcl_string.h>
#include <vcl_iostream.h>


template <class T>
void test_gaussian_stats_type(T epsilon, const vcl_string& type_name)
{
  dbsta_gaussian_updater<T> updater;
  
  dbsta_gaussian_full<T,3> gauss3D_full;
  gauss3D_full.set_data(dbsta_stats_data<T>());
  
  dbsta_gaussian_indep<T,3> gauss3D_indep;
  gauss3D_indep.set_data(dbsta_stats_data<T>());
  
  dbsta_gaussian_sphere<T,3> gauss3D_sphere;
  gauss3D_sphere.set_data(dbsta_stats_data<T>());
  
  // test data
  const unsigned int data_size = 5;
  vnl_vector<T> data[data_size] = { vnl_vector_fixed<T,3>(2.0f, 3.0f, 10.0f), 
                                    vnl_vector_fixed<T,3>(3.0f, 3.0f, 2.0f), 
                                    vnl_vector_fixed<T,3>(7.0f, 2.5f, -2.0f), 
                                    vnl_vector_fixed<T,3>(1.0f, 3.2f, 7.0f), 
                                    vnl_vector_fixed<T,3>(0.0f, 3.1f, 5.0f) };
                          
  //======== compute stats for comparison ========= 
  // mean
  vnl_vector_fixed<T,3> mean(T(0));
  for(unsigned int i=0; i<data_size; ++i){
    mean += data[i];
  }
  mean /= data_size;
  
  // variance and covariance
  T var = 0;
  vnl_matrix_fixed<T,3,3> covar(T(0));
  for(unsigned int i=0; i<data_size; ++i){
    vnl_vector_fixed<T,3> dev = data[i] - mean;
    var += dot_product(dev,dev);
    covar += outer_product(dev,dev);
  }
  var /= data_size;
  covar /= data_size; 
   
  vnl_diag_matrix<T> diag_covar(3);
  for(unsigned int i=0; i<3; ++i)
    diag_covar[i] = covar(i,i);
  //=============================================
  
  for(unsigned int i=0; i<data_size; ++i){
    updater(gauss3D_full, data[i]);
    updater(gauss3D_indep, data[i]);
    updater(gauss3D_sphere, data[i]);
  }
  
  TEST(("Full mean <"+type_name+">").c_str(), 
       (gauss3D_full.mean() - mean).inf_norm() < epsilon, true);
  TEST(("Indep mean <"+type_name+">").c_str(), 
       (gauss3D_indep.mean() - mean).inf_norm() < epsilon, true);
  TEST(("Sphere mean <"+type_name+">").c_str(), 
       (gauss3D_sphere.mean() - mean).inf_norm() < epsilon, true); 
  
  TEST(("Full covar <"+type_name+">").c_str(), 
       (gauss3D_full.covar() - covar).absolute_value_max() < epsilon, true);
  TEST(("Indep covar <"+type_name+">").c_str(), 
       (gauss3D_indep.diag_covar() - diag_covar).diagonal().inf_norm() < epsilon, true);
  TEST(("Sphere var <"+type_name+">").c_str(), 
       vcl_abs(gauss3D_sphere.var() - var) < epsilon, true);    

}



MAIN( test_gaussian_stats )
{
  START ("gaussian_stats");
  test_gaussian_stats_type(float(1e-5),"float");
  test_gaussian_stats_type(double(1e-14),"double");
  SUMMARY();
}


