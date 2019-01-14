//: 
// \file    isq_to_tiff_converter.cxx
// \brief   This program reads an isq file, and saves each slice as a tiff image
//          in the same folder of the isq file. The number of slices in the isq
//          file should be less than 10000.
// \author  H. Can Aras
// \date    2005-09-15
// 

#include <vil/vil_stream_fstream.h>
#if VXL_WIN32 
#include <vil/vil_stream_fstream_64.h>
#endif
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>

#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include <imgr/file_formats/imgr_isq_file_format.h>

int main(int argc, char* argv[])
{
    #if defined(VCL_WIN32)

    std::cout << '\n'<< "Max number of open files has been reset from " << _getmaxstdio();

    _setmaxstdio(2048);

#endif

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
    std::size_t dot_pos = fname.find_first_of(".");
    if(std::strcmp(fname.substr(dot_pos+1, 3).data(), "isq") != 0 && 
       std::strcmp(fname.substr(dot_pos+1, 3).data(), "ISQ") != 0)
    {
      std::cout << "\n***************************************" << std::endl;
      std::cout << "The file does not have an isq extension" << std::endl;
      std::cout << "***************************************" << std::endl;
      return 0;
    }
    else
    {
      std::vector<vil_image_resource_sptr> img_res_sptrs;

      char root[1024] = "\0";
      std::strncpy(root, fname.data(), dot_pos);
      std::strcat(root, "_img");
      std::cout << root << std::endl;

      vil_stream* is = new vil_stream_fstream(fname.c_str(), "r");
      imgr_isq_file_format isq_reader(is);
      img_res_sptrs = isq_reader.get_images();
      double min = isq_reader.min_intensity();
      double max = isq_reader.max_intensity();
      unsigned nk = img_res_sptrs.size();
      std::cout << "\nminimum: " << min << std::endl;
      std::cout << "maximum: " << max << std::endl;
      std::cout << "number of images: " << nk << std::endl;

     // for(unsigned k = 0; k<nk; ++k)
     for(unsigned k = 0; k<nk; k = k+100)
      {
        char out_fname[1024] = "\0";
        std::strcat(out_fname, root);
        char strnum[4] = "\0";
        if(k < 10)
          std::strcat(out_fname, "000");
        else if(k < 100)
          std::strcat(out_fname, "00");
        else
          std::strcat(out_fname, "0");

        std::sprintf(strnum, "%d", k);
        std::strcat(out_fname, strnum);
        std::strcat(out_fname, ".tiff");

        std::cout <<"enter the " << k <<"th resource "<< std::flush;
        vil_image_view<unsigned short> v(img_res_sptrs[k]->get_view());
        std::cout << "open view [ " << k <<"] " << v.ni() << ' ' << v.nj() << '\n';
        vil_save(v, out_fname, "tiff");
      }
    }
  }
  return 0;
}
