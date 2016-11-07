#include <xscan/xscan_dummy_scan.h>
#include <xscan/xscan_uniform_orbit.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_memory_image.h>
#include <vil/vil_new.h>
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vcl_cstdio.h>
#include <vcl_cstdlib.h>
#include <imgr/file_formats/imgr_skyscan_log_header.h>
#include <imgr/file_formats/imgr_skyscan_log.h>

int main(int argc, char* argv[]) {
        if(argc < 3) { 
                vcl_cerr << "Usage : " << argv[0] << " <scan log file> <output scan file name>\n";
                exit(1);
        }
        vcl_string logfile = argv[1];
        vcl_string scanfile = argv[2];
        vcl_cout << "Using logfile " << logfile << ", writing scan file " << scanfile << "\n";


        imgr_skyscan_log skyscan_log(logfile);
        xscan_scan scan = skyscan_log.get_scan();

        vcl_ofstream out (scanfile.c_str());
        out << scan << "\n";
        out.close();

        exit(0); 
}

