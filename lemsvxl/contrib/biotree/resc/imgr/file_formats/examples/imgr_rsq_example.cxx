//: 
// \file    imgr_rsq_example.cxx
// \brief   an example to  test rsq file format
// \author  Kongbin Kang and H. Can Aras
// \date    2005-04-18
// \verbatim
//  06/16/2005 : vgui added (H. Can Aras)
//  06/22/2005 : all members of rsq header printed (H. Can Aras)
//  11/15/2005 : all members of rsq header stored in a file whose path is specified at command line  (pradeep)
// \endverbatim

#include <vil/vil_stream_fstream.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <imgr/file_formats/imgr_rsq_header.h>
#include <imgr/file_formats/imgr_rsq.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>

int main(int argc, char* argv[])
{
  vgui::init(argc, argv);

  vgui_dialog dlg("Load RSQ File");
  dlg.set_ok_button("LOAD");
  dlg.set_cancel_button("CANCEL");
  static std::string fname = "*.rsq";
  static std::string ext = "*.*";
  dlg.file("rsq Filename:", ext, fname);
std::string txt_file = argv[1];
  if (!dlg.ask())
    return 0;
  else
  {
    vgui::quit();
    std::size_t dot_pos = fname.find_first_of(".");
    if(std::strcmp(fname.substr(dot_pos+1, 3).data(), "rsq") != 0 && std::strcmp(fname.substr(dot_pos+1, 3).data(), "RSQ") != 0)
    {
      std::cout << "***************************************" << std::endl;
      std::cout << "The file does not have an rsq extension" << std::endl;
      std::cout << "***************************************" << std::endl;
      return 0;
    }

    vil_stream* is = new vil_stream_fstream(fname.data(), "r");
    imgr_rsq_header rsq_header(is);
    
    std::cout << "---RSQ PARAMETERS---" << std::endl;
    std::cout << "check_ : "         << rsq_header.check_         << std::endl;
    std::cout << "data_type_ : "     << rsq_header.data_type_     << std::endl;
    std::cout << "nr_of_bytes_ : "   << rsq_header.nr_of_bytes_   << std::endl;
    std::cout << "nr_of_blocks_ : "  << rsq_header.nr_of_blocks_  << std::endl;
    std::cout << "patient_index_ : " << rsq_header.patient_index_ << std::endl;
    std::cout << "scanner_id_ : "    << rsq_header.scanner_id_    << std::endl;
    std::cout << "creation_date_ : " << rsq_header.creation_date_ << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "dimx_p_ : "    << rsq_header.dimx_p_    << std::endl;
    std::cout << "dimy_p_ : "    << rsq_header.dimy_p_    << std::endl;
    std::cout << "dimz_p_ : "    << rsq_header.dimz_p_    << std::endl;
    std::cout << "dimx_um_ : "   << rsq_header.dimx_um_   << std::endl;
    std::cout << "dimy_mdeg_ : " << rsq_header.dimy_mdeg_ << std::endl;
    std::cout << "dimz_um_ : "   << rsq_header.dimz_um_   << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "slice_thickness_um_ : " << rsq_header.slice_thickness_um_ << std::endl;
    std::cout << "slice_increment_um_ : " << rsq_header.slice_increment_um_ << std::endl;
    std::cout << "slice_1_pos_um_ : "     << rsq_header.slice_1_pos_um_     << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "scanner_type_ : "   << rsq_header.scanner_type_   << std::endl;
    std::cout << "min_data_value_ : " << rsq_header.min_data_value_ << std::endl;
    std::cout << "max_data_value_ : " << rsq_header.max_data_value_ << std::endl;
    std::cout << "---raw area type structure members start here---" << std::endl;
    std::cout << "nr_of_det_x_ : "                  << rsq_header.u_.area_.nr_of_det_x_                 << std::endl;
    std::cout << "nr_of_det_y_ : "                  << rsq_header.u_.area_.nr_of_det_y_                 << std::endl;
    std::cout << "detector_length_x_um_ : "         << rsq_header.u_.area_.detector_length_x_um_        << std::endl;
    std::cout << "detector_length_y_um_ : "         << rsq_header.u_.area_.detector_length_y_um_        << std::endl;
    std::cout << "detector_center_x_p_ : "          << rsq_header.u_.area_.detector_center_x_p_         << std::endl;
    std::cout << "detector_center_y_p_ : "          << rsq_header.u_.area_.detector_center_y_p_         << std::endl;
    std::cout << "detector_distance_um_ : "         << rsq_header.u_.area_.detector_distance_um_        << std::endl;
    std::cout << "rotation_center_distance_um_ : "  << rsq_header.u_.area_.rotation_center_distance_um_ << std::endl;
    std::cout << "detector_angle_x_mdeg_ : "        << rsq_header.u_.area_.detector_angle_x_mdeg_       << std::endl;
    std::cout << "detector_angle_y_mdeg_ : "        << rsq_header.u_.area_.detector_angle_y_mdeg_       << std::endl;
    std::cout << "detector_angle_z_mdeg_ : "        << rsq_header.u_.area_.detector_angle_z_mdeg_       << std::endl;
    std::cout << "slice_angle_increment_mdeg_ : "   << rsq_header.u_.area_.slice_angle_increment_mdeg_  << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "i0_pixel_l_x_ : "   << rsq_header.u_.area_.i0_pixel_l_x_   << std::endl;
    std::cout << "i0_pixel_r_x_ : "   << rsq_header.u_.area_.i0_pixel_r_x_   << std::endl;
    std::cout << "i0_pixel_u_y_ : "   << rsq_header.u_.area_.i0_pixel_u_y_   << std::endl;
    std::cout << "i0_pixel_d_y_ : "   << rsq_header.u_.area_.i0_pixel_d_y_   << std::endl;
    std::cout << "dark_pixel_l_x_ : " << rsq_header.u_.area_.dark_pixel_l_x_ << std::endl;
    std::cout << "dark_pixel_r_x_ : " << rsq_header.u_.area_.dark_pixel_r_x_ << std::endl;
    std::cout << "dark_pixel_u_y_ : " << rsq_header.u_.area_.dark_pixel_u_y_ << std::endl;
    std::cout << "dark_pixel_d_y_ : " << rsq_header.u_.area_.dark_pixel_d_y_ << std::endl;
    std::cout << "data_pixel_l_x_ : " << rsq_header.u_.area_.data_pixel_l_x_ << std::endl;
    std::cout << "data_pixel_r_x_ : " << rsq_header.u_.area_.data_pixel_r_x_ << std::endl;
    std::cout << "data_pixel_u_y_ : " << rsq_header.u_.area_.data_pixel_u_y_ << std::endl;
    std::cout << "data_pixel_d_y_ : " << rsq_header.u_.area_.data_pixel_d_y_ << std::endl;
    std::cout << "i0_index_ : "       << rsq_header.u_.area_.i0_index_       << std::endl;
    std::cout << "dark_index_ : "     << rsq_header.u_.area_.dark_index_     << std::endl;
    std::cout << "data_index_ : "     << rsq_header.u_.area_.data_index_     << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "index_measurement_ : "   << rsq_header.u_.area_.index_measurement_   << std::endl;
    std::cout << "integration_time_us_ : " << rsq_header.u_.area_.integration_time_us_ << std::endl;
    std::cout << "name_ : "                << rsq_header.u_.area_.name_                << std::endl;
    std::cout << "energy_ : "              << rsq_header.u_.area_.energy_              << std::endl;
    std::cout << "intensity_ : "           << rsq_header.u_.area_.intensity_           << std::endl;
    std::cout << "scanning_mode_ : "       << rsq_header.u_.area_.scanning_mode_       << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "no_frames_per_stack_ : "      << rsq_header.u_.area_.no_frames_per_stack_     << std::endl;
    std::cout << "no_I0_per_stack_ : "          << rsq_header.u_.area_.no_I0_per_stack_          << std::endl;
    std::cout << "no_dark_per_stack_ : "        << rsq_header.u_.area_.no_dark_per_stack_        << std::endl;
    std::cout << "no_pro_per_stack_ : "         << rsq_header.u_.area_.no_pro_per_stack_         << std::endl;
    std::cout << "z_pos_1stframe_center_um_ : " << rsq_header.u_.area_.z_pos_1stframe_center_um_ << std::endl;
    std::cout << "z_inc_per_frame_nm_ : "       << rsq_header.u_.area_.z_inc_per_frame_nm_       << std::endl;
    std::cout << "z_inc_per_stack_um_ : "       << rsq_header.u_.area_.z_inc_per_stack_um_       << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "vt_ : "        << rsq_header.u_.area_.vt_        << std::endl;
    std::cout << "ht_ : "        << rsq_header.u_.area_.ht_        << std::endl;
    std::cout << "vo_ : "        << rsq_header.u_.area_.vo_        << std::endl;
    std::cout << "ho_ : "        << rsq_header.u_.area_.ho_        << std::endl;
    std::cout << "flip_code_ : " << rsq_header.u_.area_.flip_code_ << std::endl;
    std::cout << "vbinu_ : "     << rsq_header.u_.area_.vbinu_     << std::endl;
    std::cout << "vbind_ : "     << rsq_header.u_.area_.vbind_     << std::endl;
    std::cout << "hbinu_ : "     << rsq_header.u_.area_.hbinu_     << std::endl;
    std::cout << "hbind_ : "     << rsq_header.u_.area_.hbind_     << std::endl;
    std::cout << "---raw area type structure members end here---" << std::endl;
    std::cout << "fill_ : "                << rsq_header.fill_                << std::endl;
    std::cout << "reco_flags_ : "          << rsq_header.reco_flags_          << std::endl;
    std::cout << "reference_line_ : "      << rsq_header.reference_line_      << std::endl;
    std::cout << "site_ : "                << rsq_header.site_                << std::endl;
    std::cout << "def_reco_angle_mdeg_ : " << rsq_header.def_reco_angle_mdeg_ << std::endl;
    std::cout << "def_reco_size_ : "       << rsq_header.def_reco_size_       << std::endl;
    std::cout << "dummy1_ : "              << rsq_header.dummy1_              << std::endl;
    std::cout << "dummy2_ : "              << rsq_header.dummy2_              << std::endl;
    std::cout << "dummy3_ : "              << rsq_header.dummy3_              << std::endl;
    std::cout << "dummy4_ : "              << rsq_header.dummy4_              << std::endl;
    std::cout << "hext_offset_ : "         << rsq_header.hext_offset_         << std::endl;
    std::cout << "data_offset_ : "         << rsq_header.data_offset_         << std::endl;

 // std::string txt_file = "C:\\scanco\\rsqheaderinfo.txt";
    
  std::ofstream fstream(txt_file.c_str());

  
    fstream << "---RSQ PARAMETERS---" << std::endl;
    fstream << "check_ : "         << rsq_header.check_         << std::endl;
    fstream << "data_type_ : "     << rsq_header.data_type_     << std::endl;
    fstream << "nr_of_bytes_ : "   << rsq_header.nr_of_bytes_   << std::endl;
   fstream << "nr_of_blocks_ : "  << rsq_header.nr_of_blocks_  << std::endl;
    fstream << "patient_index_ : " << rsq_header.patient_index_ << std::endl;
    fstream << "scanner_id_ : "    << rsq_header.scanner_id_    << std::endl;
    fstream << "creation_date_ : " << rsq_header.creation_date_ << std::endl;
    fstream << "---" << std::endl;
    fstream << "dimx_p_ : "    << rsq_header.dimx_p_    << std::endl;
    fstream << "dimy_p_ : "    << rsq_header.dimy_p_    << std::endl;
    fstream << "dimz_p_ : "    << rsq_header.dimz_p_    << std::endl;
    fstream << "dimx_um_ : "   << rsq_header.dimx_um_   << std::endl;
    fstream << "dimy_mdeg_ : " << rsq_header.dimy_mdeg_ << std::endl;
   fstream << "dimz_um_ : "   << rsq_header.dimz_um_   << std::endl;
    fstream << "---" << std::endl;
    fstream << "slice_thickness_um_ : " << rsq_header.slice_thickness_um_ << std::endl;
    fstream << "slice_increment_um_ : " << rsq_header.slice_increment_um_ << std::endl;
   fstream << "slice_1_pos_um_ : "     << rsq_header.slice_1_pos_um_     << std::endl;
   fstream<< "---" << std::endl;
    fstream << "scanner_type_ : "   << rsq_header.scanner_type_   << std::endl;
    fstream<< "min_data_value_ : " << rsq_header.min_data_value_ << std::endl;
    fstream << "max_data_value_ : " << rsq_header.max_data_value_ << std::endl;
   fstream << "---raw area type structure members start here---" << std::endl;
    fstream << "nr_of_det_x_ : "                  << rsq_header.u_.area_.nr_of_det_x_                 << std::endl;
    fstream << "nr_of_det_y_ : "                  << rsq_header.u_.area_.nr_of_det_y_                 << std::endl;
    fstream << "detector_length_x_um_ : "         << rsq_header.u_.area_.detector_length_x_um_        << std::endl;
    fstream << "detector_length_y_um_ : "         << rsq_header.u_.area_.detector_length_y_um_        << std::endl;
    fstream << "detector_center_x_p_ : "          << rsq_header.u_.area_.detector_center_x_p_         << std::endl;
   fstream << "detector_center_y_p_ : "          << rsq_header.u_.area_.detector_center_y_p_         << std::endl;
   fstream << "detector_distance_um_ : "         << rsq_header.u_.area_.detector_distance_um_        << std::endl;
   fstream << "rotation_center_distance_um_ : "  << rsq_header.u_.area_.rotation_center_distance_um_ << std::endl;
    fstream << "detector_angle_x_mdeg_ : "        << rsq_header.u_.area_.detector_angle_x_mdeg_       << std::endl;
    fstream << "detector_angle_y_mdeg_ : "        << rsq_header.u_.area_.detector_angle_y_mdeg_       << std::endl;
    fstream << "detector_angle_z_mdeg_ : "        << rsq_header.u_.area_.detector_angle_z_mdeg_       << std::endl;
    fstream << "slice_angle_increment_mdeg_ : "   << rsq_header.u_.area_.slice_angle_increment_mdeg_  << std::endl;
    fstream << "---" << std::endl;
    fstream << "i0_pixel_l_x_ : "   << rsq_header.u_.area_.i0_pixel_l_x_   << std::endl;
    fstream << "i0_pixel_r_x_ : "   << rsq_header.u_.area_.i0_pixel_r_x_   << std::endl;
    fstream << "i0_pixel_u_y_ : "   << rsq_header.u_.area_.i0_pixel_u_y_   << std::endl;
    fstream << "i0_pixel_d_y_ : "   << rsq_header.u_.area_.i0_pixel_d_y_   << std::endl;
    fstream << "dark_pixel_l_x_ : " << rsq_header.u_.area_.dark_pixel_l_x_ << std::endl;
    fstream << "dark_pixel_r_x_ : " << rsq_header.u_.area_.dark_pixel_r_x_ << std::endl;
    fstream << "dark_pixel_u_y_ : " << rsq_header.u_.area_.dark_pixel_u_y_ << std::endl;
   fstream << "dark_pixel_d_y_ : " << rsq_header.u_.area_.dark_pixel_d_y_ << std::endl;
    fstream<< "data_pixel_l_x_ : " << rsq_header.u_.area_.data_pixel_l_x_ << std::endl;
    fstream << "data_pixel_r_x_ : " << rsq_header.u_.area_.data_pixel_r_x_ << std::endl;
    fstream << "data_pixel_u_y_ : " << rsq_header.u_.area_.data_pixel_u_y_ << std::endl;
   fstream << "data_pixel_d_y_ : " << rsq_header.u_.area_.data_pixel_d_y_ << std::endl;
    fstream << "i0_index_ : "       << rsq_header.u_.area_.i0_index_       << std::endl;
    fstream << "dark_index_ : "     << rsq_header.u_.area_.dark_index_     << std::endl;
    fstream << "data_index_ : "     << rsq_header.u_.area_.data_index_     << std::endl;
    fstream << "---" << std::endl;
    fstream << "index_measurement_ : "   << rsq_header.u_.area_.index_measurement_   << std::endl;
    fstream << "integration_time_us_ : " << rsq_header.u_.area_.integration_time_us_ << std::endl;
    fstream << "name_ : "                << rsq_header.u_.area_.name_                << std::endl;
    fstream << "energy_ : "              << rsq_header.u_.area_.energy_              << std::endl;
    fstream << "intensity_ : "           << rsq_header.u_.area_.intensity_           << std::endl;
    fstream << "scanning_mode_ : "       << rsq_header.u_.area_.scanning_mode_       << std::endl;
    fstream << "---" << std::endl;
    fstream << "no_frames_per_stack_ : "      << rsq_header.u_.area_.no_frames_per_stack_     << std::endl;
    fstream << "no_I0_per_stack_ : "          << rsq_header.u_.area_.no_I0_per_stack_          << std::endl;
    fstream << "no_dark_per_stack_ : "        << rsq_header.u_.area_.no_dark_per_stack_        << std::endl;
    fstream << "no_pro_per_stack_ : "         << rsq_header.u_.area_.no_pro_per_stack_         << std::endl;
    fstream << "z_pos_1stframe_center_um_ : " << rsq_header.u_.area_.z_pos_1stframe_center_um_ << std::endl;
    fstream << "z_inc_per_frame_nm_ : "       << rsq_header.u_.area_.z_inc_per_frame_nm_       << std::endl;
    fstream << "z_inc_per_stack_um_ : "       << rsq_header.u_.area_.z_inc_per_stack_um_       << std::endl;
    fstream << "---" << std::endl;
    fstream << "vt_ : "        << rsq_header.u_.area_.vt_        << std::endl;
    fstream << "ht_ : "        << rsq_header.u_.area_.ht_        << std::endl;
    fstream << "vo_ : "        << rsq_header.u_.area_.vo_        << std::endl;
    fstream << "ho_ : "        << rsq_header.u_.area_.ho_        << std::endl;
    fstream << "flip_code_ : " << rsq_header.u_.area_.flip_code_ << std::endl;
    fstream << "vbinu_ : "     << rsq_header.u_.area_.vbinu_     << std::endl;
    fstream << "vbind_ : "     << rsq_header.u_.area_.vbind_     << std::endl;
    fstream << "hbinu_ : "     << rsq_header.u_.area_.hbinu_     << std::endl;
    fstream << "hbind_ : "     << rsq_header.u_.area_.hbind_     << std::endl;
    fstream << "---raw area type structure members end here---" << std::endl;
    fstream << "fill_ : "                << rsq_header.fill_                << std::endl;
    fstream << "reco_flags_ : "          << rsq_header.reco_flags_          << std::endl;
    fstream << "reference_line_ : "      << rsq_header.reference_line_      << std::endl;
    fstream << "site_ : "                << rsq_header.site_                << std::endl;
    fstream << "def_reco_angle_mdeg_ : " << rsq_header.def_reco_angle_mdeg_ << std::endl;
    fstream << "def_reco_size_ : "       << rsq_header.def_reco_size_       << std::endl;
    fstream << "dummy1_ : "              << rsq_header.dummy1_              << std::endl;
    fstream << "dummy2_ : "              << rsq_header.dummy2_              << std::endl;
    fstream << "dummy3_ : "              << rsq_header.dummy3_              << std::endl;
    fstream << "dummy4_ : "              << rsq_header.dummy4_              << std::endl;
    fstream << "hext_offset_ : "         << rsq_header.hext_offset_         << std::endl;
    fstream << "data_offset_ : "         << rsq_header.data_offset_         << std::endl;
    fstream << "---SCAN---" << std::endl;

     imgr_rsq rsq(is);
    fstream << rsq.get_scan();
    fstream.close();

   
    std::cout << "---SCAN---" << std::endl;
    std::cout << rsq.get_scan();
    rsq.get_images();
    return 0;
  }
}
