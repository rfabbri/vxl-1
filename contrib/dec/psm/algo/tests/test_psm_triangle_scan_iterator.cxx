#include <testlib/testlib_test.h>

#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

#include "../psm_triangle_scan_iterator.h"

static void test_psm_triangle_scan_iterator()
{
  START("psm_triangle_scan_iterator test");

  double verts_x1[] = {1.0, 4.0, 1.0};
  double verts_y1[] = {4.0, 4.0, 1.0};
  vil_image_view<unsigned char> img(5,5,1);
  img.fill(0);

  psm_triangle_scan_iterator tri_it(verts_x1, verts_y1, 0, 1, 2);
  tri_it.reset();
  while (tri_it.next()) {
    int scany = tri_it.scany();
    int startx = tri_it.startx();
    int endx = tri_it.endx();
    for (int x=startx; x<endx; ++x) {
      img(x,scany) += 1;
    }
  }
  vil_save(img,"c:/research/psm/output/test_it1.tiff");

  double verts_x2[] = {4.0, 4.0, 1.0};
  double verts_y2[] = {4.0, 1.0, 1.0};
  tri_it = psm_triangle_scan_iterator(verts_x2, verts_y2);
  tri_it.reset();
  while (tri_it.next()) {
    int scany = tri_it.scany();
    int startx = tri_it.startx();
    int endx = tri_it.endx();
    for (int x=startx; x<endx; ++x) {
      img(x,scany) += 1;
    }
  }
  vil_save(img,"c:/research/psm/output/test_it2.tiff");

  // test image values
  bool image_good = true;
  for (unsigned int j=0; j<5; ++j) {
    for (unsigned int i=0; i<5; ++i) {
      if ((i > 0) && (i < 4) && (j > 0) && (j < 4)) {
        image_good &= (img(i,j) == 1);
      } else {
        image_good &= (img(i,j) == 0);
      }
    }
  }
  TEST("formed image is correct", image_good, true);

  return;
}




TESTMAIN( test_psm_triangle_scan_iterator );

