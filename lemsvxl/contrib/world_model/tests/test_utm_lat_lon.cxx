#include <testlib/testlib_test.h>
#include <world_model/utm_lat_lon.h>
#include <vcl_iostream.h>


static void test_utm_to_lat_lon()
{
  double lat, lon;
  utm_lat_lon utm;
  utm.transform(38, 438000, 3689775, lat, lon);
  vcl_cout << "lat=" << lat << " lon=" << lon << vcl_endl;
  TEST_NEAR("LATITUDE", lat, 33.34, 0.01);
  TEST_NEAR("LONGITUDE", lon, 44.33, 0.01);
}

static void test_lat_lon_to_utm()
{
  double x, y;
  int utm_zone;
  utm_lat_lon utm;
  utm.transform(33.3454, 44.3337, x, y, utm_zone);
  vcl_cout << "x=" << x << " y=" << y << vcl_endl;
  TEST_NEAR("EASTING", x, 438000, 3.0);
  TEST_NEAR("NORTHING", y, 3689775, 3.0);
}
static void test_utm_lat_lon()
{
  test_utm_to_lat_lon();
  test_lat_lon_to_utm();
}
TESTMAIN( test_utm_lat_lon);
