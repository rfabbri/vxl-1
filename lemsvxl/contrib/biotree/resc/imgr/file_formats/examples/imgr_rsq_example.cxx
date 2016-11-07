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
  static vcl_string fname = "*.rsq";
  static vcl_string ext = "*.*";
  dlg.file("rsq Filename:", ext, fname);
vcl_string txt_file = argv[1];
  if (!dlg.ask())
    return 0;
  else
  {
    vgui::quit();
    vcl_size_t dot_pos = fname.find_first_of(".");
    if(vcl_strcmp(fname.substr(dot_pos+1, 3).data(), "rsq") != 0 && vcl_strcmp(fname.substr(dot_pos+1, 3).data(), "RSQ") != 0)
    {
      vcl_cout << "***************************************" << vcl_endl;
      vcl_cout << "The file does not have an rsq extension" << vcl_endl;
      vcl_cout << "***************************************" << vcl_endl;
      return 0;
    }

    vil_stream* is = new vil_stream_fstream(fname.data(), "r");
    imgr_rsq_header rsq_header(is);
    
    vcl_cout << "---RSQ PARAMETERS---" << vcl_endl;
    vcl_cout << "check_ : "         << rsq_header.check_         << vcl_endl;
    vcl_cout << "data_type_ : "     << rsq_header.data_type_     << vcl_endl;
    vcl_cout << "nr_of_bytes_ : "   << rsq_header.nr_of_bytes_   << vcl_endl;
    vcl_cout << "nr_of_blocks_ : "  << rsq_header.nr_of_blocks_  << vcl_endl;
    vcl_cout << "patient_index_ : " << rsq_header.patient_index_ << vcl_endl;
    vcl_cout << "scanner_id_ : "    << rsq_header.scanner_id_    << vcl_endl;
    vcl_cout << "creation_date_ : " << rsq_header.creation_date_ << vcl_endl;
    vcl_cout << "---" << vcl_endl;
    vcl_cout << "dimx_p_ : "    << rsq_header.dimx_p_    << vcl_endl;
    vcl_cout << "dimy_p_ : "    << rsq_header.dimy_p_    << vcl_endl;
    vcl_cout << "dimz_p_ : "    << rsq_header.dimz_p_    << vcl_endl;
    vcl_cout << "dimx_um_ : "   << rsq_header.dimx_um_   << vcl_endl;
    vcl_cout << "dimy_mdeg_ : " << rsq_header.dimy_mdeg_ << vcl_endl;
    vcl_cout << "dimz_um_ : "   << rsq_header.dimz_um_   << vcl_endl;
    vcl_cout << "---" << vcl_endl;
    vcl_cout << "slice_thickness_um_ : " << rsq_header.slice_thickness_um_ << vcl_endl;
    vcl_cout << "slice_increment_um_ : " << rsq_header.slice_increment_um_ << vcl_endl;
    vcl_cout << "slice_1_pos_um_ : "     << rsq_header.slice_1_pos_um_     << vcl_endl;
    vcl_cout << "---" << vcl_endl;
    vcl_cout << "scanner_type_ : "   << rsq_header.scanner_type_   << vcl_endl;
    vcl_cout << "min_data_value_ : " << rsq_header.min_data_value_ << vcl_endl;
    vcl_cout << "max_data_value_ : " << rsq_header.max_data_value_ << vcl_endl;
    vcl_cout << "---raw area type structure members start here---" << vcl_endl;
    vcl_cout << "nr_of_det_x_ : "                  << rsq_header.u_.area_.nr_of_det_x_                 << vcl_endl;
    vcl_cout << "nr_of_det_y_ : "                  << rsq_header.u_.area_.nr_of_det_y_                 << vcl_endl;
    vcl_cout << "detector_length_x_um_ : "         << rsq_header.u_.area_.detector_length_x_um_        << vcl_endl;
    vcl_cout << "detector_length_y_um_ : "         << rsq_header.u_.area_.detector_length_y_um_        << vcl_endl;
    vcl_cout << "detector_center_x_p_ : "          << rsq_header.u_.area_.detector_center_x_p_         << vcl_endl;
    vcl_cout << "detector_center_y_p_ : "          << rsq_header.u_.area_.detector_center_y_p_         << vcl_endl;
    vcl_cout << "detector_distance_um_ : "         << rsq_header.u_.area_.detector_distance_um_        << vcl_endl;
    vcl_cout << "rotation_center_distance_um_ : "  << rsq_header.u_.area_.rotation_center_distance_um_ << vcl_endl;
    vcl_cout << "detector_angle_x_mdeg_ : "        << rsq_header.u_.area_.detector_angle_x_mdeg_       << vcl_endl;
    vcl_cout << "detector_angle_y_mdeg_ : "        << rsq_header.u_.area_.detector_angle_y_mdeg_       << vcl_endl;
    vcl_cout << "detector_angle_z_mdeg_ : "        << rsq_header.u_.area_.detector_angle_z_mdeg_       << vcl_endl;
    vcl_cout << "slice_angle_increment_mdeg_ : "   << rsq_header.u_.area_.slice_angle_increment_mdeg_  << vcl_endl;
    vcl_cout << "---" << vcl_endl;
    vcl_cout << "i0_pixel_l_x_ : "   << rsq_header.u_.area_.i0_pixel_l_x_   << vcl_endl;
    vcl_cout << "i0_pixel_r_x_ : "   << rsq_header.u_.area_.i0_pixel_r_x_   << vcl_endl;
    vcl_cout << "i0_pixel_u_y_ : "   << rsq_header.u_.area_.i0_pixel_u_y_   << vcl_endl;
    vcl_cout << "i0_pixel_d_y_ : "   << rsq_header.u_.area_.i0_pixel_d_y_   << vcl_endl;
    vcl_cout << "dark_pixel_l_x_ : " << rsq_header.u_.area_.dark_pixel_l_x_ << vcl_endl;
    vcl_cout << "dark_pixel_r_x_ : " << rsq_header.u_.area_.dark_pixel_r_x_ << vcl_endl;
    vcl_cout << "dark_pixel_u_y_ : " << rsq_header.u_.area_.dark_pixel_u_y_ << vcl_endl;
    vcl_cout << "dark_pixel_d_y_ : " << rsq_header.u_.area_.dark_pixel_d_y_ << vcl_endl;
    vcl_cout << "data_pixel_l_x_ : " << rsq_header.u_.area_.data_pixel_l_x_ << vcl_endl;
    vcl_cout << "data_pixel_r_x_ : " << rsq_header.u_.area_.data_pixel_r_x_ << vcl_endl;
    vcl_cout << "data_pixel_u_y_ : " << rsq_header.u_.area_.data_pixel_u_y_ << vcl_endl;
    vcl_cout << "data_pixel_d_y_ : " << rsq_header.u_.area_.data_pixel_d_y_ << vcl_endl;
    vcl_cout << "i0_index_ : "       << rsq_header.u_.area_.i0_index_       << vcl_endl;
    vcl_cout << "dark_index_ : "     << rsq_header.u_.area_.dark_index_     << vcl_endl;
    vcl_cout << "data_index_ : "     << rsq_header.u_.area_.data_index_     << vcl_endl;
    vcl_cout << "---" << vcl_endl;
    vcl_cout << "index_measurement_ : "   << rsq_header.u_.area_.index_measurement_   << vcl_endl;
    vcl_cout << "integration_time_us_ : " << rsq_header.u_.area_.integration_time_us_ << vcl_endl;
    vcl_cout << "name_ : "                << rsq_header.u_.area_.name_                << vcl_endl;
    vcl_cout << "energy_ : "              << rsq_header.u_.area_.energy_              << vcl_endl;
    vcl_cout << "intensity_ : "           << rsq_header.u_.area_.intensity_           << vcl_endl;
    vcl_cout << "scanning_mode_ : "       << rsq_header.u_.area_.scanning_mode_       << vcl_endl;
    vcl_cout << "---" << vcl_endl;
    vcl_cout << "no_frames_per_stack_ : "      << rsq_header.u_.area_.no_frames_per_stack_     << vcl_endl;
    vcl_cout << "no_I0_per_stack_ : "          << rsq_header.u_.area_.no_I0_per_stack_          << vcl_endl;
    vcl_cout << "no_dark_per_stack_ : "        << rsq_header.u_.area_.no_dark_per_stack_        << vcl_endl;
    vcl_cout << "no_pro_per_stack_ : "         << rsq_header.u_.area_.no_pro_per_stack_         << vcl_endl;
    vcl_cout << "z_pos_1stframe_center_um_ : " << rsq_header.u_.area_.z_pos_1stframe_center_um_ << vcl_endl;
    vcl_cout << "z_inc_per_frame_nm_ : "       << rsq_header.u_.area_.z_inc_per_frame_nm_       << vcl_endl;
    vcl_cout << "z_inc_per_stack_um_ : "       << rsq_header.u_.area_.z_inc_per_stack_um_       << vcl_endl;
    vcl_cout << "---" << vcl_endl;
    vcl_cout << "vt_ : "        << rsq_header.u_.area_.vt_        << vcl_endl;
    vcl_cout << "ht_ : "        << rsq_header.u_.area_.ht_        << vcl_endl;
    vcl_cout << "vo_ : "        << rsq_header.u_.area_.vo_        << vcl_endl;
    vcl_cout << "ho_ : "        << rsq_header.u_.area_.ho_        << vcl_endl;
    vcl_cout << "flip_code_ : " << rsq_header.u_.area_.flip_code_ << vcl_endl;
    vcl_cout << "vbinu_ : "     << rsq_header.u_.area_.vbinu_     << vcl_endl;
    vcl_cout << "vbind_ : "     << rsq_header.u_.area_.vbind_     << vcl_endl;
    vcl_cout << "hbinu_ : "     << rsq_header.u_.area_.hbinu_     << vcl_endl;
    vcl_cout << "hbind_ : "     << rsq_header.u_.area_.hbind_     << vcl_endl;
    vcl_cout << "---raw area type structure members end here---" << vcl_endl;
    vcl_cout << "fill_ : "                << rsq_header.fill_                << vcl_endl;
    vcl_cout << "reco_flags_ : "          << rsq_header.reco_flags_          << vcl_endl;
    vcl_cout << "reference_line_ : "      << rsq_header.reference_line_      << vcl_endl;
    vcl_cout << "site_ : "                << rsq_header.site_                << vcl_endl;
    vcl_cout << "def_reco_angle_mdeg_ : " << rsq_header.def_reco_angle_mdeg_ << vcl_endl;
    vcl_cout << "def_reco_size_ : "       << rsq_header.def_reco_size_       << vcl_endl;
    vcl_cout << "dummy1_ : "              << rsq_header.dummy1_              << vcl_endl;
    vcl_cout << "dummy2_ : "              << rsq_header.dummy2_              << vcl_endl;
    vcl_cout << "dummy3_ : "              << rsq_header.dummy3_              << vcl_endl;
    vcl_cout << "dummy4_ : "              << rsq_header.dummy4_              << vcl_endl;
    vcl_cout << "hext_offset_ : "         << rsq_header.hext_offset_         << vcl_endl;
    vcl_cout << "data_offset_ : "         << rsq_header.data_offset_         << vcl_endl;

 // vcl_string txt_file = "C:\\scanco\\rsqheaderinfo.txt";
    
  vcl_ofstream fstream(txt_file.c_str());

  
    fstream << "---RSQ PARAMETERS---" << vcl_endl;
    fstream << "check_ : "         << rsq_header.check_         << vcl_endl;
    fstream << "data_type_ : "     << rsq_header.data_type_     << vcl_endl;
    fstream << "nr_of_bytes_ : "   << rsq_header.nr_of_bytes_   << vcl_endl;
   fstream << "nr_of_blocks_ : "  << rsq_header.nr_of_blocks_  << vcl_endl;
    fstream << "patient_index_ : " << rsq_header.patient_index_ << vcl_endl;
    fstream << "scanner_id_ : "    << rsq_header.scanner_id_    << vcl_endl;
    fstream << "creation_date_ : " << rsq_header.creation_date_ << vcl_endl;
    fstream << "---" << vcl_endl;
    fstream << "dimx_p_ : "    << rsq_header.dimx_p_    << vcl_endl;
    fstream << "dimy_p_ : "    << rsq_header.dimy_p_    << vcl_endl;
    fstream << "dimz_p_ : "    << rsq_header.dimz_p_    << vcl_endl;
    fstream << "dimx_um_ : "   << rsq_header.dimx_um_   << vcl_endl;
    fstream << "dimy_mdeg_ : " << rsq_header.dimy_mdeg_ << vcl_endl;
   fstream << "dimz_um_ : "   << rsq_header.dimz_um_   << vcl_endl;
    fstream << "---" << vcl_endl;
    fstream << "slice_thickness_um_ : " << rsq_header.slice_thickness_um_ << vcl_endl;
    fstream << "slice_increment_um_ : " << rsq_header.slice_increment_um_ << vcl_endl;
   fstream << "slice_1_pos_um_ : "     << rsq_header.slice_1_pos_um_     << vcl_endl;
   fstream<< "---" << vcl_endl;
    fstream << "scanner_type_ : "   << rsq_header.scanner_type_   << vcl_endl;
    fstream<< "min_data_value_ : " << rsq_header.min_data_value_ << vcl_endl;
    fstream << "max_data_value_ : " << rsq_header.max_data_value_ << vcl_endl;
   fstream << "---raw area type structure members start here---" << vcl_endl;
    fstream << "nr_of_det_x_ : "                  << rsq_header.u_.area_.nr_of_det_x_                 << vcl_endl;
    fstream << "nr_of_det_y_ : "                  << rsq_header.u_.area_.nr_of_det_y_                 << vcl_endl;
    fstream << "detector_length_x_um_ : "         << rsq_header.u_.area_.detector_length_x_um_        << vcl_endl;
    fstream << "detector_length_y_um_ : "         << rsq_header.u_.area_.detector_length_y_um_        << vcl_endl;
    fstream << "detector_center_x_p_ : "          << rsq_header.u_.area_.detector_center_x_p_         << vcl_endl;
   fstream << "detector_center_y_p_ : "          << rsq_header.u_.area_.detector_center_y_p_         << vcl_endl;
   fstream << "detector_distance_um_ : "         << rsq_header.u_.area_.detector_distance_um_        << vcl_endl;
   fstream << "rotation_center_distance_um_ : "  << rsq_header.u_.area_.rotation_center_distance_um_ << vcl_endl;
    fstream << "detector_angle_x_mdeg_ : "        << rsq_header.u_.area_.detector_angle_x_mdeg_       << vcl_endl;
    fstream << "detector_angle_y_mdeg_ : "        << rsq_header.u_.area_.detector_angle_y_mdeg_       << vcl_endl;
    fstream << "detector_angle_z_mdeg_ : "        << rsq_header.u_.area_.detector_angle_z_mdeg_       << vcl_endl;
    fstream << "slice_angle_increment_mdeg_ : "   << rsq_header.u_.area_.slice_angle_increment_mdeg_  << vcl_endl;
    fstream << "---" << vcl_endl;
    fstream << "i0_pixel_l_x_ : "   << rsq_header.u_.area_.i0_pixel_l_x_   << vcl_endl;
    fstream << "i0_pixel_r_x_ : "   << rsq_header.u_.area_.i0_pixel_r_x_   << vcl_endl;
    fstream << "i0_pixel_u_y_ : "   << rsq_header.u_.area_.i0_pixel_u_y_   << vcl_endl;
    fstream << "i0_pixel_d_y_ : "   << rsq_header.u_.area_.i0_pixel_d_y_   << vcl_endl;
    fstream << "dark_pixel_l_x_ : " << rsq_header.u_.area_.dark_pixel_l_x_ << vcl_endl;
    fstream << "dark_pixel_r_x_ : " << rsq_header.u_.area_.dark_pixel_r_x_ << vcl_endl;
    fstream << "dark_pixel_u_y_ : " << rsq_header.u_.area_.dark_pixel_u_y_ << vcl_endl;
   fstream << "dark_pixel_d_y_ : " << rsq_header.u_.area_.dark_pixel_d_y_ << vcl_endl;
    fstream<< "data_pixel_l_x_ : " << rsq_header.u_.area_.data_pixel_l_x_ << vcl_endl;
    fstream << "data_pixel_r_x_ : " << rsq_header.u_.area_.data_pixel_r_x_ << vcl_endl;
    fstream << "data_pixel_u_y_ : " << rsq_header.u_.area_.data_pixel_u_y_ << vcl_endl;
   fstream << "data_pixel_d_y_ : " << rsq_header.u_.area_.data_pixel_d_y_ << vcl_endl;
    fstream << "i0_index_ : "       << rsq_header.u_.area_.i0_index_       << vcl_endl;
    fstream << "dark_index_ : "     << rsq_header.u_.area_.dark_index_     << vcl_endl;
    fstream << "data_index_ : "     << rsq_header.u_.area_.data_index_     << vcl_endl;
    fstream << "---" << vcl_endl;
    fstream << "index_measurement_ : "   << rsq_header.u_.area_.index_measurement_   << vcl_endl;
    fstream << "integration_time_us_ : " << rsq_header.u_.area_.integration_time_us_ << vcl_endl;
    fstream << "name_ : "                << rsq_header.u_.area_.name_                << vcl_endl;
    fstream << "energy_ : "              << rsq_header.u_.area_.energy_              << vcl_endl;
    fstream << "intensity_ : "           << rsq_header.u_.area_.intensity_           << vcl_endl;
    fstream << "scanning_mode_ : "       << rsq_header.u_.area_.scanning_mode_       << vcl_endl;
    fstream << "---" << vcl_endl;
    fstream << "no_frames_per_stack_ : "      << rsq_header.u_.area_.no_frames_per_stack_     << vcl_endl;
    fstream << "no_I0_per_stack_ : "          << rsq_header.u_.area_.no_I0_per_stack_          << vcl_endl;
    fstream << "no_dark_per_stack_ : "        << rsq_header.u_.area_.no_dark_per_stack_        << vcl_endl;
    fstream << "no_pro_per_stack_ : "         << rsq_header.u_.area_.no_pro_per_stack_         << vcl_endl;
    fstream << "z_pos_1stframe_center_um_ : " << rsq_header.u_.area_.z_pos_1stframe_center_um_ << vcl_endl;
    fstream << "z_inc_per_frame_nm_ : "       << rsq_header.u_.area_.z_inc_per_frame_nm_       << vcl_endl;
    fstream << "z_inc_per_stack_um_ : "       << rsq_header.u_.area_.z_inc_per_stack_um_       << vcl_endl;
    fstream << "---" << vcl_endl;
    fstream << "vt_ : "        << rsq_header.u_.area_.vt_        << vcl_endl;
    fstream << "ht_ : "        << rsq_header.u_.area_.ht_        << vcl_endl;
    fstream << "vo_ : "        << rsq_header.u_.area_.vo_        << vcl_endl;
    fstream << "ho_ : "        << rsq_header.u_.area_.ho_        << vcl_endl;
    fstream << "flip_code_ : " << rsq_header.u_.area_.flip_code_ << vcl_endl;
    fstream << "vbinu_ : "     << rsq_header.u_.area_.vbinu_     << vcl_endl;
    fstream << "vbind_ : "     << rsq_header.u_.area_.vbind_     << vcl_endl;
    fstream << "hbinu_ : "     << rsq_header.u_.area_.hbinu_     << vcl_endl;
    fstream << "hbind_ : "     << rsq_header.u_.area_.hbind_     << vcl_endl;
    fstream << "---raw area type structure members end here---" << vcl_endl;
    fstream << "fill_ : "                << rsq_header.fill_                << vcl_endl;
    fstream << "reco_flags_ : "          << rsq_header.reco_flags_          << vcl_endl;
    fstream << "reference_line_ : "      << rsq_header.reference_line_      << vcl_endl;
    fstream << "site_ : "                << rsq_header.site_                << vcl_endl;
    fstream << "def_reco_angle_mdeg_ : " << rsq_header.def_reco_angle_mdeg_ << vcl_endl;
    fstream << "def_reco_size_ : "       << rsq_header.def_reco_size_       << vcl_endl;
    fstream << "dummy1_ : "              << rsq_header.dummy1_              << vcl_endl;
    fstream << "dummy2_ : "              << rsq_header.dummy2_              << vcl_endl;
    fstream << "dummy3_ : "              << rsq_header.dummy3_              << vcl_endl;
    fstream << "dummy4_ : "              << rsq_header.dummy4_              << vcl_endl;
    fstream << "hext_offset_ : "         << rsq_header.hext_offset_         << vcl_endl;
    fstream << "data_offset_ : "         << rsq_header.data_offset_         << vcl_endl;
    fstream << "---SCAN---" << vcl_endl;

     imgr_rsq rsq(is);
    fstream << rsq.get_scan();
    fstream.close();

   
    vcl_cout << "---SCAN---" << vcl_endl;
    vcl_cout << rsq.get_scan();
    rsq.get_images();
    return 0;
  }
}
