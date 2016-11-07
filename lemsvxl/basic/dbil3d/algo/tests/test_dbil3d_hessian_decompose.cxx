#include <testlib/testlib_test.h>
#include <vil3d/vil3d_print.h>
#include <vil3d/vil3d_copy.h>
#include <vcl_cstdio.h>
#include <vcl_cmath.h>
#include <vnl/vnl_random.h>
#include <vnl/vnl_math.h>
#include <dbil3d/algo/dbil3d_hessian_decompose.h>
#include <vil3d/io/vil3d_io_image_view.h>

MAIN( test_dbil3d_hessian_decompose )
{
  START ("Hessian Decompose");

  vil3d_image_view<float> img;
  vsl_b_ifstream bfs_in("test.float");
  vsl_b_read(bfs_in, img);
  bfs_in.close();

  vil3d_image_view<float> e1;
  vil3d_image_view<float> e2;
  vil3d_image_view<float> e3;
  vil3d_image_view<float> l1;
  vil3d_image_view<float> l2;
  vil3d_image_view<float> l3;

  dbil3d_hessian_decompose(img,1.5,e1,e2,e3,l1,l2,l3);

  vil3d_image_view<float> result(img.ni(),img.nj(),img.nk());

  for(int z=0; z<l1.nk(); z++) {
          for(int y=0; y<l1.nj(); y++) {
                  for(int x=0; x<l1.ni(); x++) {
                          if(vcl_fabs(l1(x,y,z)) > 800 
                          && vcl_fabs(l2(x,y,z)) > 300 )
//                          && vcl_fabs(l3(x,y,z)) < 300 )
                          result(x,y,z) = 1;
                          else
                          result(x,y,z) = 0;
                  }

          }
  }

  vsl_b_ofstream bfs_out("result.float");
  vsl_b_write(bfs_out, result);
  bfs_out.close();

  SUMMARY();
}
