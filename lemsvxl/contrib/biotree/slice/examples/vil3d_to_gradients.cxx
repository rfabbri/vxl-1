#include <slice/sliceFileManager.h>
#include <slice/algo/sliceEdgeDetectProcessor.h>
#include <slice/sliceEngine.h>
#include <vcl_fstream.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/io/vil3d_io_image_view.h>
#include <vil3d/algo/vil3d_grad_1x3.h>
#include <vil3d/vil3d_math.h>
#include <vil3d/vil3d_transform.h>
#include <basic/dbil3d/algo/dbil3d_gauss_filter.h> 
#include <vcl_cmath.h>

int main(int argc, char* argv[])
{        

        if(argc < 2){
                vcl_cerr << "usage : " << argv[0]  << " <vil3d binary image file> \n";
                return 1;
        }

        int w;
        int h;
        int d;
        vil3d_image_view<float> volume;
        vsl_b_ifstream bfs_in(argv[1]);
        vsl_b_read(bfs_in, volume);
        bfs_in.close();

        w = volume.ni();
        h = volume.nj();
        d = volume.nk();

        vil3d_image_view<float> smoothed;
        vil3d_image_view<float> gx;
        vil3d_image_view<float> gy;
        vil3d_image_view<float> gz;
        dbil3d_gauss_filter(volume,1.5,smoothed);
        vil3d_grad_1x3(smoothed,gx,gy,gz);

        sliceFileManager<float>::write(gx,"gx.out");
        sliceFileManager<float>::write(gy,"gy.out");
        sliceFileManager<float>::write(gz,"gz.out");
        return 0;
}
