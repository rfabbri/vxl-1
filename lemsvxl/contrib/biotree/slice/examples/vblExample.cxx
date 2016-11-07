#include <slice/sliceFileManager.h>
#include <slice/algo/sliceEdgeDetectProcessor.h>
#include <slice/sliceEngine.h>
#include <vcl_fstream.h>
#include <vil3d/vil3d_image_view.h>
#include <vil3d/io/vil3d_io_image_view.h>
#include <vil3d/algo/vil3d_grad_1x3.h>
#include <vil3d/vil3d_math.h>
#include <vil3d/vil3d_transform.h>
#include <vbl/vbl_array_3d.h>
#include <basic/dbil3d/algo/dbil3d_gauss_filter.h> 
#include <vcl_cmath.h>

int main(int argc, char* argv[])
{        

        vcl_cerr << "creating vbl array" << "\n";

        int ni = 50;
        int nj = 100;
        int nk = 5;
        vcl_cerr << " ni = " << ni << "\n";
        vcl_cerr << " nj = " << nj << "\n";
        vcl_cerr << " nk = " << nk << "\n";


        vbl_array_3d<float> vbl(ni,nj,nk,0.);

        for(int k = 0; k < nk; k++){
                for(int j = 0; j < nj; j++){
                        for(int i = 0; i < ni; i++){
                                vbl(i,j,k) = i;
                        }
                }
        }

        vcl_cerr << "writing vbl.out" << "\n";
        sliceFileManager<float>::write(vbl,"vbl.out");

        vcl_cerr << "reading from vbl.out to vil3d image" << "\n";
        vil3d_image_view<float> vbl_img = sliceFileManager<float>::read("vbl.out");

        {
        vcl_cerr << "writing to vil3d image file " << "\n";
        vsl_b_ofstream bfs_out("vbl.float");
        vsl_b_write(bfs_out, vbl_img);
        bfs_out.close();
        }


        vil3d_image_view<float> control(ni,nj,nk);

        for(int k = 0; k < nk; k++){
                for(int j = 0; j < nj; j++){
                        for(int i = 0; i < ni; i++){
                                control(i,j,k) = k;
                        }
                }
        }

        {
        vcl_cerr << "writing to control.float " << "\n";
        vsl_b_ofstream bfs_out("control.float");
        vsl_b_write(bfs_out, control);
        bfs_out.close();
        }

        return 0;
}
