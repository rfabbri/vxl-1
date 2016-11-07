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
#include <vcl_cstddef.h>
#include <vcl_cstring.h>

int main(int argc, char* argv[])
{
  vgui::init(argc, argv);

  vgui_dialog dlg("Load ISQ File");
  dlg.set_ok_button("LOAD");
  dlg.set_cancel_button("CANCEL");
  static vcl_string fname = "*.isq";
  static vcl_string ext = "*.*";
  dlg.file("isq Filename:", ext, fname);

  if (!dlg.ask())
    return 0;
  else
  {
    vgui::quit();
    vcl_size_t dot_pos = fname.find_first_of(".");
    if(vcl_strcmp(fname.substr(dot_pos+1, 3).data(), "isq") != 0 && vcl_strcmp(fname.substr(dot_pos+1, 3).data(), "ISQ") != 0)
    {
      vcl_cout << "***************************************" << vcl_endl;
      vcl_cout << "The file does not have an isq extension" << vcl_endl;
      vcl_cout << "***************************************" << vcl_endl;
      return 0;
    }
    vil_stream* is = new vil_stream_fstream(fname.data(), "r");
    imgr_isq_header isq_header(is);
    vcl_cout << "---ISQ PARAMETERS---" << vcl_endl;
    vcl_cout << "check_ : "         << isq_header.check_         << vcl_endl;
    vcl_cout << "data_type_ : "     << isq_header.data_type_     << vcl_endl;
    vcl_cout << "nr_of_bytes_ : "   << isq_header.nr_of_bytes_   << vcl_endl;
    vcl_cout << "nr_of_blocks_ : "  << isq_header.nr_of_blocks_  << vcl_endl;
    vcl_cout << "patient_index_ : " << isq_header.patient_index_ << vcl_endl;
    vcl_cout << "scanner_id_ : "    << isq_header.scanner_id_    << vcl_endl;
    vcl_cout << "creation_date_ : " << isq_header.creation_date_ << vcl_endl;
    vcl_cout << "---" << vcl_endl;
    vcl_cout << "dimx_p_ : "    << isq_header.dimx_p_    << vcl_endl;
    vcl_cout << "dimy_p_ : "    << isq_header.dimy_p_    << vcl_endl;
    vcl_cout << "dimz_p_ : "    << isq_header.dimz_p_    << vcl_endl;
    vcl_cout << "dimx_um_ : "   << isq_header.dimx_um_   << vcl_endl;
    vcl_cout << "dimy_um_ : "   << isq_header.dimy_um_   << vcl_endl;
    vcl_cout << "dimz_um_ : "   << isq_header.dimz_um_   << vcl_endl;
    vcl_cout << "---" << vcl_endl;
    vcl_cout << "slice_thickness_um_ : " << isq_header.slice_thickness_um_ << vcl_endl;
    vcl_cout << "slice_increment_um_ : " << isq_header.slice_increment_um_ << vcl_endl;
    vcl_cout << "slice_1_pos_um_ : "     << isq_header.slice_1_pos_um_     << vcl_endl;
    vcl_cout << "---" << vcl_endl;
    vcl_cout << "min_data_value_ : "    << isq_header.min_data_value_    << vcl_endl;
    vcl_cout << "max_data_value_ : "    << isq_header.max_data_value_    << vcl_endl;
    vcl_cout << "mu_scaling_ : "        << isq_header.mu_scaling_        << vcl_endl;
    vcl_cout << "nr_of_samples_ : "     << isq_header.nr_of_samples_     << vcl_endl;
    vcl_cout << "nr_of_projections_ : " << isq_header.nr_of_projections_ << vcl_endl;
    vcl_cout << "---" << vcl_endl;
    vcl_cout << "scandist_um_ : "       << isq_header.scandist_um_       << vcl_endl;
    vcl_cout << "scanner_type_ : "      << isq_header.scanner_type_      << vcl_endl;
    vcl_cout << "sampletime_us_ : "     << isq_header.sampletime_us_     << vcl_endl;
    vcl_cout << "index_measurement_ : " << isq_header.index_measurement_ << vcl_endl;
    vcl_cout << "site_ : "              << isq_header.site_              << vcl_endl;
    vcl_cout << "reference_line_um_ : " << isq_header.reference_line_um_ << vcl_endl;
    vcl_cout << "recon_alg_ : "         << isq_header.recon_alg_         << vcl_endl;
    vcl_cout << "name_ : "              << isq_header.name_              << vcl_endl;
    vcl_cout << "energy_ : "            << isq_header.energy_            << vcl_endl;
    vcl_cout << "intensity_ : "         << isq_header.intensity_         << vcl_endl;
    vcl_cout << "fill_ : "              << isq_header.fill_              << vcl_endl;
    vcl_cout << "data_offset_ : "       << isq_header.data_offset_       << vcl_endl;
    // release the stream
    is->unref();
    return 0;
  }
}
