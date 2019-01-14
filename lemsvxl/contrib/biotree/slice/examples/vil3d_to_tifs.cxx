
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
    std::cout << "Usage: " << argv[0] << " [vil3d file name] [output prefix ]\n ";
    exit(-1);
  }

  vil3d_image_view<float> img;
  vsl_b_ifstream bfs_in(argv[1]);
  vsl_b_read(bfs_in, img);
  bfs_in.close();

  vil_image_view<float> cur;

  for (int i = 0; i < img.nk(); i++)
  {
          std::string num; std::stringstream s; s << i ; s >> num;
          if(i < 10) num = "0" + num; 
          if(i < 100) num = "0" + num; 
          num = argv[2] + num + ".tif";
          cur.deep_copy(vil3d_slice_ij(img,i));

          vil_save(cur,num.c_str(), "tiff");
  }

  return 0;
}
