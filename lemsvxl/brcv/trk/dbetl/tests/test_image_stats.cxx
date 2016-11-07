#include <testlib/testlib_test.h>
#include <dbetl/dbetl_image_stats.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>



static bool approx_equal(double a, double b, double thresh=1e-12)
{
  return vcl_abs(b-a) < thresh;
}

static bool approx_equal(const vil_rgb<double>& a, 
                         const vil_rgb<double>& b, 
                         double thresh=1e-12)
{
  return vcl_abs(b.R()-a.R()) < thresh &&
         vcl_abs(b.G()-a.G()) < thresh &&
         vcl_abs(b.B()-a.B()) < thresh;
}


MAIN( test_image_stats )
{
  START ("image statistics");

  vil_rgb<double> data[10] = { vil_rgb<double>(0.6,  0.2,  0.1),
                               vil_rgb<double>(0.55, 0.2,  0.15),
                               vil_rgb<double>(0.5,  0.2,  0.2),
                               vil_rgb<double>(0.5,  0.2,  0.2),
                               vil_rgb<double>(0.55, 0.15, 0.15),
                               vil_rgb<double>(0.7,  0.1,  0.3),
                               vil_rgb<double>(0.5,  0.5,  0.5),
                               vil_rgb<double>(0.9,  0.7,  0.8),
                               vil_rgb<double>(0.5,  0.2,  0.3),
                               vil_rgb<double>(0.5,  0.5,  0.5) };

  dbetl_image_stats stats1;
  TEST("Empty Stats Mean", stats1.int_mean(), 0.0);
  TEST("Empty Stats Variance", approx_equal(stats1.int_var(), 0.0, 1e-6), true);
  TEST("Empty Stats Color", stats1.color(), vil_rgb<double>(0.0,0.0,0.0));

  stats1.add_sample(data[0]);
  TEST("1 Sample Mean", approx_equal(stats1.int_mean(), data[0].grey()), true);
  TEST("1 Sample Variance", approx_equal(stats1.int_var(), 0.0, 1e-6), true);
  TEST("1 Sample Color", stats1.color(), data[0]);

  vcl_cout << "cost 1 = " << stats1.intensity_cost(stats1.int_mean()) << vcl_endl;

  stats1.add_sample(data[1]);
  double mean = (data[0].grey()+data[1].grey())/2.0;
  TEST("2 Sample Mean", approx_equal(stats1.int_mean(), mean), true);
  double var = (data[0].grey()*data[0].grey()+data[1].grey()*data[1].grey()) - 2.0*mean*mean;
  TEST("2 Sample Variance", approx_equal(stats1.int_var(), var, 1e-6), true);
  TEST("2 Sample Color", approx_equal(stats1.color(), (data[0]+data[1])/2.0), true);

  vcl_cout << "cost 2 = " << stats1.intensity_cost(stats1.int_mean()) << vcl_endl;

  for(int i=2; i<6; ++i)
    stats1.add_sample(data[i]);
  mean = 0.0;
  vil_rgb<double> color(0.0, 0.0, 0.0);
  for(int i=0; i<6; ++i){
    mean += data[i].grey();
    color += data[i];
  }
  mean /= 6;
  color /= 6;
  var = 0.0;
  for(int i=0; i<6; ++i){
    double temp = (data[i].grey()-mean);
    var += temp*temp;
  }
  var /= 5;

  vcl_cout << "6 Samples => mean=" << mean << " var=" << var << vcl_endl;
  
  TEST("6 Sample Mean", approx_equal(stats1.int_mean(), mean), true);
  TEST("6 Sample Variance", approx_equal(stats1.int_var(), var, 1e-6), true);
  TEST("6 Sample Color", approx_equal(stats1.color(), color), true);

  // add the rest of the data to this object
  dbetl_image_stats stats2;
  for(int i=6; i<10; ++i)
    stats2.add_sample(data[i]);

  dbetl_image_stats stats3 = stats1 + stats2;
  stats1 += stats2;
  // stats1 should now equal stats3
  TEST("Combined Mean", stats3.int_mean(), stats1.int_mean());
  TEST("Combined Variance", stats3.int_var(), stats1.int_var());
  TEST("Combined Color", stats3.color(), stats1.color());

  mean = 0.0;
  color = vil_rgb<double>(0.0, 0.0, 0.0);
  for(int i=0; i<10; ++i){
    mean += data[i].grey();
    color += data[i];
  }
  mean /= 10;
  color /= 10;
  var = 0.0;
  for(int i=0; i<10; ++i){
    double temp = (data[i].grey()-mean);
    var += temp*temp;
  }
  var /= 9;

  vcl_cout << "10 Samples => mean=" << mean << " var=" << var << vcl_endl;

  TEST("10 Sample Mean", approx_equal(stats3.int_mean(), mean), true);
  TEST("10 Sample Variance", approx_equal(stats3.int_var(), var, 1e-6), true);
  TEST("10 Sample Color", approx_equal(stats3.color(), color), true);

  SUMMARY();
}
