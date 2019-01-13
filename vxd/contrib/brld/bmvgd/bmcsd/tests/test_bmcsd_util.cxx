#include <testlib/testlib_test.h>

#include <bmcsd/bmcsd_util.h>
#include <algorithm>
#include <bmcsd/bmcsd_view_set.h>
#include <buld/buld_parse_simple_file.h>

static const double tolerance=std::numeric_limits<double>::epsilon()*100;

static void test_min_max_median();
static void test_small_things();
// static void test_view_set_io();

//: tests multiview projection + reconstruction of differential geometry
MAIN( test_bmcsd_util )
{
  START ("General Utilities");

  {
  test_small_things(); 
  test_min_max_median();
// requires a .txt golden file which is a pain in lemsvxlsrc  test_view_set_io();
  }

  SUMMARY();
}

void test_small_things()
{
  {
  bmcsd_vector_3d v1(0,1,0);
  bmcsd_vector_3d v2(1,0,0);
  TEST_NEAR("E1 angle E2",bmcsd_util::angle_unit(v1,v2),vnl_math::pi/2,1e-7);
  }

  {
  bmcsd_vector_3d v1(0,1,0);
  bmcsd_vector_3d v2(1,0,0);
  TEST_NEAR("E2 angle E1",bmcsd_util::angle_unit(v2,v1),vnl_math::pi/2,1e-7);
  }

  {
  bmcsd_vector_3d v1(0,1,0);
  bmcsd_vector_3d v2(1,0,0);
  TEST_NEAR("E2 angle E1",bmcsd_util::angle_unit(v2,v1),vnl_math::pi/2,1e-7);
  }

  {
  bmcsd_vector_3d v1(1,1,0);
  v1.normalize();
  bmcsd_vector_3d v2(1,0,0);
  TEST_NEAR("E2 angle E1",bmcsd_util::angle_unit(v2,v1),vnl_math::pi/4,1e-7);
  }
}

void
test_min_max_median()
{


  {
    std::vector<double> v;
    v.push_back(2);
    v.push_back(25);
    v.push_back(3.33);
    v.push_back(-1.9);
    v.push_back(10);
    v.push_back(2);

    std::vector<double> v_sorted = v;

    std::sort(v_sorted.begin(), v_sorted.end());

    std::cout << "v(size " << v.size() << "): ";
    for (unsigned i=0; i < v.size(); ++i)
      std::cout << v[i] << ' ' ;
    std::cout << std::endl;

    std::cout << "v_sorted(size " << v_sorted.size() << "): ";
    for (unsigned i=0; i < v_sorted.size(); ++i)
      std::cout << v_sorted[i] << ' ' ;
    std::cout << std::endl;

    double max, min, mean, median;
    unsigned imax, imin;

    std::cout 
      <<   "Max: " << (max = bmcsd_util::max(v,imax)) << "," << imax
      << "\tMin: " << (min = bmcsd_util::min(v,imin)) << "," << imin
      << "\tAvg: " << (mean = bmcsd_util::mean(v))
      << "\tMed: " << (median = bmcsd_util::median(v))
      << std::endl;

    TEST("Max", max,25);
    TEST("Max index", imax,1);
    TEST("Min", min,-1.9);
    TEST("Min index", imin,3);
    TEST_NEAR("Mean", mean, 6.73833,1e-5);
    TEST("Median", median, 2.6650);
  }

  {
    std::vector<double> v;
    v.push_back(25);
    v.push_back(3.33);
    v.push_back(-1.9);
    v.push_back(10);
    v.push_back(2);

    std::vector<double> v_sorted = v;

    std::sort(v_sorted.begin(), v_sorted.end());

    std::cout << "v(size " << v.size() << "): ";
    for (unsigned i=0; i < v.size(); ++i)
      std::cout << v[i] << ' ' ;
    std::cout << std::endl;

    std::cout << "v_sorted(size " << v_sorted.size() << "): ";
    for (unsigned i=0; i < v_sorted.size(); ++i)
      std::cout << v_sorted[i] << ' ' ;
    std::cout << std::endl;

    double max, min, mean, median;
    unsigned imax, imin;

    std::cout 
      <<   "Max: " << (max = bmcsd_util::max(v,imax)) << "," << imax
      << "\tMin: " << (min = bmcsd_util::min(v,imin)) << "," << imin
      << "\tAvg: " << (mean = bmcsd_util::mean(v))
      << "\tMed: " << (median = bmcsd_util::median(v))
      << std::endl;

    TEST("Max", max,25);
    TEST("Max index", imax,0);
    TEST("Min", min,-1.9);
    TEST("Min index", imin,2);
    TEST_NEAR("Mean", mean, 7.6860,1e-4);
    TEST("Median", median, 3.33);
  }
}

/*
void test_view_set_io()
{
  bmcsd_stereo_instance_views frames_to_match;

  bmcsd_view_set::read_txt("mcs_stereo_instances_example.txt", &frames_to_match);

  std::cout << frames_to_match << std::endl;
}
*/
