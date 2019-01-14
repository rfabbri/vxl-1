
#include <slice/sliceFileManager.h>
#include <ctime>
#include <cstdlib>
#include <vil/vil_save.h>
#include <vil/vil_math.h>
#include <vil/vil_convert.h>
#include <vil3d/vil3d_slice.h>
#include <vil3d/io/vil3d_io_image_view.h>

int main(int argc, char *argv[])
{
  if(argc != 3)
  {
    std::cout << "Usage: " << argv[0] << " [vil3d file name] [output slice file name ]\n ";
    exit(-1);
  }

  vil3d_image_view<float> img;
  vsl_b_ifstream bfs_in(argv[1]);
  vsl_b_read(bfs_in, img);
  bfs_in.close();

  sliceFileManager<float>::write(img,argv[2]);

  return 0;
}
