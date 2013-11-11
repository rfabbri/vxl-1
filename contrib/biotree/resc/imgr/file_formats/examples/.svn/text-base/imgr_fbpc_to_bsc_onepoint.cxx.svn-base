#include <vcl_fstream.h>
#include <vcl_vector.h>
#include <vcl_sstream.h>
#include <vgl/vgl_vector_3d.h>
#include <xscan/xscan_scan.h>
#include <imgr/file_formats/imgr_skyscan_reconlog.h>

int main(int argc, char* argv[])
{
  if(argc < 5)
  {
    vcl_cout << "Usage: " << argv[0] <<" reconlog_file point_x point_y point_z\n ";
    exit(1);
  }

  double x, y, z;
  /*
  xscan_scan scan;

  //read scan and log files
  //--------------------------
  vcl_ifstream scanfile(argv[2]);
  if(!scanfile)
  {
    vcl_cout <<"Open scan file failed\n";
    exit(2);
  }
//  imgr_skyscan_reconlog skyscan(argv[1], scan);
 */

  imgr_skyscan_reconlog skyscan(argv[1]);
  //--------------------------

  //read point to convert
  //--------------------------
  vcl_stringstream ss;
  ss << argv[2];
  ss >> x;
  ss.clear();
  
  ss << argv[3];
  ss >> y;
  ss.clear();

  ss << argv[4];
  ss >> z;
  ss.clear();
  //--------------------------

  vgl_point_3d<double> recon_point(x,y,z);
  vgl_point_3d<double> biotree_point = skyscan.fbpc_to_bsc(recon_point);

  vcl_cout <<  recon_point << " becomes " << biotree_point << "\n";
  
  return 0;
}
