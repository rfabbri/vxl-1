#include <testlib/testlib_test.h>
#include <dbsta/algo/dbsta_adaptive_updater.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <dbsta/dbsta_gaussian_full.h>
#include <dbsta/dbsta_gaussian_indep.h>
#include <dbsta/dbsta_gaussian_sphere.h>
#include <vcl_string.h>
#include <vcl_iostream.h>


template <class T>
void test_statistical_updater_type(T epsilon, const vcl_string& type_name)
{
  // a model used when inserting new gaussians
  vnl_matrix_fixed<T,3,3> covar; covar.set_identity();
  vnl_vector_fixed<T,3> mean(T(0));
  dbsta_gaussian_full<T,3> model(mean, covar*T(5));
  model.set_data(dbsta_stats_data<T>(1));
  
  
  dbsta_mg_statistical_updater<T> updater(model,3);
  
  dbsta_mixture<T> mix_gauss;
  
  
  // test data
  const unsigned int data_size = 10;
  vnl_vector<T> data[data_size] = { vnl_vector_fixed<T,3>(2.0f, 3.0f, 10.0f), 
                                    vnl_vector_fixed<T,3>(2.0f, 3.0f, 10.1f), 
                                    vnl_vector_fixed<T,3>(2.5f, 2.9f, 9.8f), 
                                    vnl_vector_fixed<T,3>(2.1f, 3.2f, 8.0f), 
                                    vnl_vector_fixed<T,3>(0.0f, 3.1f, 5.0f),
                                    vnl_vector_fixed<T,3>(2.0f, 3.0f, 10.0f), 
                                    vnl_vector_fixed<T,3>(3.0f, 3.0f, 2.0f), 
                                    vnl_vector_fixed<T,3>(7.0f, 2.5f, -2.0f), 
                                    vnl_vector_fixed<T,3>(1.0f, 3.2f, 7.0f), 
                                    vnl_vector_fixed<T,3>(0.0f, 3.1f, 5.0f) };
                          
  
  for(unsigned int i=0; i<data_size; ++i){
    updater(mix_gauss, data[i]);
    vcl_cout << "components " << mix_gauss.num_components() << vcl_endl;
    for(unsigned int j=0; j<mix_gauss.num_components(); ++j){
      T count = static_cast<const dbsta_stats_data<T>&>(mix_gauss.distribution(j).data()).num_observations;
      const dbsta_gaussian_full<T,3>& g = static_cast<const dbsta_gaussian_full<T,3>&>(mix_gauss.distribution(j));
      vcl_cout << "  G("<<j<<") with " << count << " observations" << vcl_endl;
      vcl_cout << "    weight = "<<mix_gauss.weight(j) << vcl_endl;
      vcl_cout << "    mean = "<<g.mean()<<vcl_endl;
      vcl_cout << "    det(covar) = " << g.det_covar() << vcl_endl;
    }
    vcl_cout << "============================================" << vcl_endl;
  }
     

}



MAIN( test_adaptive_updater )
{
  START ("statistical updater");
  test_statistical_updater_type(float(1e-5),"float");
  test_statistical_updater_type(double(1e-14),"double");
  SUMMARY();
}


