#include <testlib/testlib_test.h>
#include <dbsta/dbsta_histogram.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vcl_string.h>
#include <vcl_iostream.h>


template <class T>
void test_histogram_type(T epsilon, const vcl_string& type_name)
{
  {
    dbsta_histogram<T> hist(1,T(0),T(256),16);
    
    TEST(("dimension <"+type_name+">").c_str(), hist.dim(), 1);
  
    // test data
    const unsigned int data_size = 10;
    T data[data_size] = { 2, 100, 4, 234, 10, 220, 96, 97, 64, 20 };
    
    bool success = true;
    for(unsigned int i=0; i<data_size; ++i)
      success = hist.update(vnl_vector<T>(1,data[i])) && success;
      
    TEST(("update <"+type_name+">").c_str(), success, true);
    TEST(("probability <"+type_name+">").c_str(),
         hist.probability(vnl_vector<T>(1,T(11))), T(0.3));
  }    
  //=============================================================
  // Joint histograme
  {
    dbsta_histogram<T> hist(2,T(0),T(10),5);
    
    // test data
    const unsigned int data_size = 10;
    vnl_vector<T> data[data_size] = { vnl_vector_fixed<T,2>(1.2,0.9),
                                      vnl_vector_fixed<T,2>(1.1,7.4),
                                      vnl_vector_fixed<T,2>(2.5,9.0),
                                      vnl_vector_fixed<T,2>(4.9,6.9),
                                      vnl_vector_fixed<T,2>(0.5,7.9),
                                      vnl_vector_fixed<T,2>(9.2,8.9),
                                      vnl_vector_fixed<T,2>(4.8,7.9),
                                      vnl_vector_fixed<T,2>(1.6,6.1),
                                      vnl_vector_fixed<T,2>(2.0,8.0),
                                      vnl_vector_fixed<T,2>(3.0,9.0) };
    bool success = true;
    for(unsigned int i=0; i<data_size; ++i)
      success = hist.update(data[i]) && success;
      
    TEST(("update <"+type_name+">").c_str(), success, true);
    TEST(("probability <"+type_name+">").c_str(),
         hist.probability(vnl_vector_fixed<T,2>(1.0,1.0)), T(0.1));
  }
}



MAIN( test_histogram )
{
  START ("histogram");
  test_histogram_type(float(1e-5),"float");
  test_histogram_type(double(1e-14),"double");
  SUMMARY();
}


