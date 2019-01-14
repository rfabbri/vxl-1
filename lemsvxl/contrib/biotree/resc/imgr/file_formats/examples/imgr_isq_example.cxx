//: 
// \file     imgr_rsq_example.cxx
// \brief    an example to  test rsq file format
// \author   Kongbin Kang and H. Can Aras
// \date     2005-04-18
// \verbatim
//  06/17/2005 : vgui added (H. Can Aras)
//  06/22/2005 : all members of rsq header printed (H. Can Aras)
// \endverbatim
// 

#include <vil/vil_stream_fstream.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <imgr/file_formats/imgr_isq_header.h>
#include <vgui/vgui_dialog.h>
#include <vgui/vgui.h>
#include <cstddef>
#include <cstring>

int main(int argc, char* argv[])
{
  vgui::init(argc, argv);

  vgui_dialog dlg("Load ISQ File");
  dlg.set_ok_button("LOAD");
  dlg.set_cancel_button("CANCEL");
  static std::string fname = "*.isq";
  static std::string ext = "*.*";
  dlg.file("isq Filename:", ext, fname);

  if (!dlg.ask())
    return 0;
  else
  {
    vgui::quit();
    std::size_t dot_pos = fname.find_first_of(".");
    if(std::strcmp(fname.substr(dot_pos+1, 3).data(), "isq") != 0 && std::strcmp(fname.substr(dot_pos+1, 3).data(), "ISQ") != 0)
    {
      std::cout << "***************************************" << std::endl;
      std::cout << "The file does not have an isq extension" << std::endl;
      std::cout << "***************************************" << std::endl;
      return 0;
    }
    vil_stream* is = new vil_stream_fstream(fname.data(), "r");
    imgr_isq_header isq_header(is);
    std::cout << "---ISQ PARAMETERS---" << std::endl;
    std::cout << "check_ : "         << isq_header.check_         << std::endl;
    std::cout << "data_type_ : "     << isq_header.data_type_     << std::endl;
    std::cout << "nr_of_bytes_ : "   << isq_header.nr_of_bytes_   << std::endl;
    std::cout << "nr_of_blocks_ : "  << isq_header.nr_of_blocks_  << std::endl;
    std::cout << "patient_index_ : " << isq_header.patient_index_ << std::endl;
    std::cout << "scanner_id_ : "    << isq_header.scanner_id_    << std::endl;
    std::cout << "creation_date_ : " << isq_header.creation_date_ << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "dimx_p_ : "    << isq_header.dimx_p_    << std::endl;
    std::cout << "dimy_p_ : "    << isq_header.dimy_p_    << std::endl;
    std::cout << "dimz_p_ : "    << isq_header.dimz_p_    << std::endl;
    std::cout << "dimx_um_ : "   << isq_header.dimx_um_   << std::endl;
    std::cout << "dimy_um_ : "   << isq_header.dimy_um_   << std::endl;
    std::cout << "dimz_um_ : "   << isq_header.dimz_um_   << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "slice_thickness_um_ : " << isq_header.slice_thickness_um_ << std::endl;
    std::cout << "slice_increment_um_ : " << isq_header.slice_increment_um_ << std::endl;
    std::cout << "slice_1_pos_um_ : "     << isq_header.slice_1_pos_um_     << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "min_data_value_ : "    << isq_header.min_data_value_    << std::endl;
    std::cout << "max_data_value_ : "    << isq_header.max_data_value_    << std::endl;
    std::cout << "mu_scaling_ : "        << isq_header.mu_scaling_        << std::endl;
    std::cout << "nr_of_samples_ : "     << isq_header.nr_of_samples_     << std::endl;
    std::cout << "nr_of_projections_ : " << isq_header.nr_of_projections_ << std::endl;
    std::cout << "---" << std::endl;
    std::cout << "scandist_um_ : "       << isq_header.scandist_um_       << std::endl;
    std::cout << "scanner_type_ : "      << isq_header.scanner_type_      << std::endl;
    std::cout << "sampletime_us_ : "     << isq_header.sampletime_us_     << std::endl;
    std::cout << "index_measurement_ : " << isq_header.index_measurement_ << std::endl;
    std::cout << "site_ : "              << isq_header.site_              << std::endl;
    std::cout << "reference_line_um_ : " << isq_header.reference_line_um_ << std::endl;
    std::cout << "recon_alg_ : "         << isq_header.recon_alg_         << std::endl;
    std::cout << "name_ : "              << isq_header.name_              << std::endl;
    std::cout << "energy_ : "            << isq_header.energy_            << std::endl;
    std::cout << "intensity_ : "         << isq_header.intensity_         << std::endl;
    std::cout << "fill_ : "              << isq_header.fill_              << std::endl;
    std::cout << "data_offset_ : "       << isq_header.data_offset_       << std::endl;
    // release the stream
    is->unref();
    return 0;
  }
}
