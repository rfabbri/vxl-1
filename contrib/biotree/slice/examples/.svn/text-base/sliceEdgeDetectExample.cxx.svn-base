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

/*
void writeToFile(vil3d_image_view<float> im, vcl_string fname)
{
        int ni = im.ni();
        int nj = im.nj();
        int nk = im.nk();

        vcl_ofstream out(fname.c_str(), vcl_ios_binary | vcl_ios_app );
        out.write((char*)&ni, sizeof(int));
        out.write((char*)&nj, sizeof(int));
        out.write((char*)&nk, sizeof(int));
        out.write((char*)im.origin_ptr(), im.ni()*im.nj()*im.nk()*sizeof(float));
        out.close();
}
*/
int main(int argc, char* argv[])
{        

        if(argc < 2){
                vcl_cerr << "usage : " << argv[0]  << " <vil3d binary image file> | <gx slice file> <gy slice file> <gz slice file>\n";
                return 1;
        }
        else if(argc < 4){
                vcl_cerr << "usage : " << argv[0]  << " <vil3d binary image file> | <gx slice file> <gy slice file> <gz slice file>\n";
                return 1;
        }


        int w;
        int h;
        int d;
        vcl_string gx_file,gy_file,gz_file;
        if(argc == 2)
        {
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

          gx_file = "gx.out";
          gy_file = "gy.out";
          gz_file = "gz.out";
        }
        else if (argc == 4){
          gx_file = argv[1];
          gy_file = argv[2];
          gz_file = argv[3];
        }


        /*
        vil3d_image_view<float> xoff(w,h,d);
        vil3d_image_view<float> yoff(w,h,d);
        vil3d_image_view<float> zoff(w,h,d);
        vil3d_image_view<float> strength(w,h,d);
        xoff.fill(0);
        yoff.fill(0);
        zoff.fill(0);
        strength.fill(0);
        */

        //using the engine to save the output to a memory location
        sliceEngine<float> engine;
        engine.addStream(gx_file);
        engine.addStream(gy_file);
        engine.addStream(gz_file);


/*
        vcl_vector<float*> outputs;
        outputs.push_back(xoff.origin_ptr());
        outputs.push_back(yoff.origin_ptr());
        outputs.push_back(zoff.origin_ptr());
        outputs.push_back(strength.origin_ptr());
        engine.setSaveToMemory(outputs);
        */
        vcl_vector<vcl_string> outputs;
        outputs.push_back("xoffset.out");
        outputs.push_back("yoffset.out");
        outputs.push_back("zoffset.out");
        outputs.push_back("strength.out");
        engine.setSaveToDisk(outputs);

        sliceEdgeDetectProcessor proc;
        engine.processWith(&proc);

        {
        vil3d_image_view<float> strength = sliceFileManager<float>::read("strength.out");
        vcl_cerr << " Writing out to strength.float ... \n";
        vsl_b_ofstream bfs_out("strength.float");
        vsl_b_write(bfs_out, strength);
        bfs_out.close();
        }

     return 0;
}
