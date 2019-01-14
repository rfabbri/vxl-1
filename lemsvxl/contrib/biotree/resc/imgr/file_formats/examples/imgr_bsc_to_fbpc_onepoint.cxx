#include <fstream>
#include <vector>
#include <sstream>
#include <vgl/vgl_vector_3d.h>
#include <xscan/xscan_scan.h>
#include <imgr/file_formats/imgr_skyscan_reconlog.h>

int main(int argc, char* argv[])
{
  if(argc < 6)
  {
    std::cout << "Usage: " << argv[0] <<" reconlog_file scan_file  point_x point_y point_z\n ";
    exit(1);
  }

  double x, y, z;
  xscan_scan scan;

  //read scan and log files
  //--------------------------
  std::ifstream scanfile(argv[2]);
  if(!scanfile)
  {
    std::cout <<"Open scan file failed\n";
    exit(2);
  }
  scanfile >> scan;
  imgr_skyscan_reconlog skyscan(argv[1], scan);
//  imgr_skyscan_reconlog skyscan(argv[1]);
  //--------------------------

  //read point to convert
  //--------------------------
  std::stringstream ss;
  ss << argv[3];
  ss >> x;
  ss.clear();
  
  ss << argv[4];
  ss >> y;
  ss.clear();

  ss << argv[5];
  ss >> z;
  ss.clear();
  //--------------------------

  vgl_point_3d<double> biotree_point(x,y,z);
  vgl_point_3d<double> recon_point = skyscan.bsc_to_fbpc(biotree_point);

  std::cout << biotree_point << " becomes " << recon_point << "\n";
  
  return 0;
}
