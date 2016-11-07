//: 
// \file    rsq_to_tiff_converter.cxx
// \brief   This program reads an rsq file, and saves each slice as a tiff image
//          in the same folder of the rsq file. The number of slices in the rsq
//          file should be less than 10000.
// 

#include <vil/vil_stream_fstream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>

#include <vgui/vgui.h>
#include <vgui/vgui_dialog.h>

#include <vcl_cstddef.h>
#include <vcl_cstring.h>
#include <vcl_cstdlib.h>
#include <vcl_cstdio.h>
#include <imgr/file_formats/imgr_rsq.h>
#include <imgr/file_formats/imgr_isq_file_format.h>

int main(int argc, char* argv[])
{



  vgui::init(argc, argv);

  vgui_dialog dlg("Load RSQ File");
  dlg.set_ok_button("LOAD");
  dlg.set_cancel_button("CANCEL");
  static vcl_string fname = "*.rsq";
  static vcl_string ext = "*.*";
  dlg.file("rsq Filename:", ext, fname);

  if (!dlg.ask())
    return 0;
  else
  {
    vcl_size_t dot_pos = fname.find_first_of(".");
    if(vcl_strcmp(fname.substr(dot_pos+1, 3).data(), "rsq") != 0 && 
       vcl_strcmp(fname.substr(dot_pos+1, 3).data(), "RSQ") != 0)
    {
      vcl_cout << "\n***************************************" << vcl_endl;
      vcl_cout << "The file does not have an rsq extension" << vcl_endl;
      vcl_cout << "***************************************" << vcl_endl;
      return 0;
    }
    else
    {
      vcl_vector<vil_image_resource_sptr> img_res_sptrs;

      char root[1024] = "\0";
      vcl_strncpy(root, fname.data(), dot_pos);
      vcl_strcat(root, "_img");
      vcl_cout << root << vcl_endl;

      vil_stream* is = new vil_stream_fstream(fname.c_str(), "r");
      imgr_rsq rsq_reader(is);

     // unsigned int interval = 10;
      img_res_sptrs = rsq_reader.get_images();
      double min = rsq_reader.min_intensity();
      double max = rsq_reader.max_intensity();
      unsigned nk = img_res_sptrs.size();
      vcl_cout << "\nminimum: " << min << vcl_endl;
      vcl_cout << "maximum: " << max << vcl_endl;
      vcl_cout << "number of images: " << nk << vcl_endl;

      for(unsigned k = 0; k<nk; ++k)
     //   for(unsigned k = 0; k<nk; k = k+100)
  
      {
        char out_fname[1024] = "\0";
        vcl_strcat(out_fname, root);
        char strnum[5] = "\0";
        if(k < 10)
          vcl_strcat(out_fname, "000");
        else if(k < 100)
          vcl_strcat(out_fname, "00");
       
        else
            vcl_strcat(out_fname,"0");

        vcl_sprintf(strnum, "%d", k);
        vcl_strcat(out_fname, strnum);
        vcl_strcat(out_fname, ".tiff");

        vcl_cout <<"enter the " << k <<"th resource "<< vcl_flush;
       
        unsigned ni = img_res_sptrs[k]->ni();
       unsigned nj = img_res_sptrs[k]->nj();

       vcl_cout<<" "<<ni<<vcl_endl;
        vcl_cout<<" "<<nj<<vcl_endl;

       //   vil_image_view<unsigned short> v(img_res_sptrs[k]->get_view(0,ni,0,nj));
        vil_image_view<unsigned short> v(img_res_sptrs[k]->get_view());
        vcl_cout << "open view [ " << k <<"] " << v.ni() << ' ' << v.nj() << '\n';
        vil_save(v, out_fname, "tiff");
      }
    }
  }
  return 0;
}
