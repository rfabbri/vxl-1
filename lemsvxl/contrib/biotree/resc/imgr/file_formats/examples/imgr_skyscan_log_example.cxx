//: 
// \file    imgr_skyscan_log_example.cxx
// \brief   an example to test SkyScan log file format
// \author  H. Can Aras
// \date    2005-06-14
// 
#include <cstdio>
#include <iostream>
#include <imgr/file_formats/imgr_skyscan_log_header.h>
#include <imgr/file_formats/imgr_skyscan_log.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>
#include <cstddef>

int main(int argc, char* argv[])
{
  vgui::init(argc, argv);

  vgui_dialog dlg("Load SkyScan LOG File");
  dlg.set_ok_button("LOAD");
  dlg.set_cancel_button("CANCEL");
  static std::string fname = "*.log";
  static std::string ext = "*.*";
  dlg.file("log Filename:", ext, fname);

  if (!dlg.ask())
    return 0;
  else
  {
    vgui::quit();
    std::size_t dot_pos = fname.find_first_of(".");
    if(std::strcmp(fname.substr(dot_pos+1, 3).data(), "log") != 0 && std::strcmp(fname.substr(dot_pos+1, 3).data(), "LOG") != 0)
    {
      std::cout << "***************************************" << std::endl;
      std::cout << "The file does not have an log extension" << std::endl;
      std::cout << "***************************************" << std::endl;
      return 0;
    }

    // You should change the below file directory to the file directory on your machine
    std::FILE *fp = std::fopen(fname.data(),"r");
    assert(fp != NULL);
    imgr_skyscan_log_header skyscan_log_header(fp);

    std::cout << "Camera Pixel Multiplier : " << skyscan_log_header.cam_pixel_multiplier_<< std::endl;
    std::cout << "Camera Pixel Size in um : " << skyscan_log_header.cam_pixel_size_ << std::endl;
    std::cout << "Camera X/Y Ratio : " << skyscan_log_header.cam_xy_ratio_ << std::endl;
    std::cout << "Source to Rotation Axis Distance along Principal Line in mm : " << skyscan_log_header.object_to_source_dist_ << std::endl;
    std::cout << "Source to Principal Point Distance in mm : " << skyscan_log_header.camera_to_source_dist_ << std::endl;
    std::cout << "Number of Projection Data Files: " << skyscan_log_header.number_of_files_ << std::endl;
    std::cout << "Number of Rows of Sensor: " << skyscan_log_header.number_of_rows_ << std::endl;
    std::cout << "Number of Columns of Sensor: " << skyscan_log_header.number_of_columns_ << std::endl;
    std::cout << "Y-coordinate of Principal Point: " << skyscan_log_header.optical_axis_ << std::endl;
    std::cout << "Voxel Size of Reconstructed Volume: " << skyscan_log_header.image_pixel_size_ << std::endl;
    std::cout << "Rotation Step Angle: " << skyscan_log_header.rotation_step_ << std::endl;
    std::cout << "Relative Angular Position of Reconstructed Volume and Scanner Coordinate System: " << skyscan_log_header.cs_static_rotation_ << std::endl;

    std::fclose(fp);

    imgr_skyscan_log skyscan_log(fname.data());
    xscan_scan scan;
    scan = skyscan_log.get_scan();
    std::cout << scan << std::endl;
    return 0;
  }
}
