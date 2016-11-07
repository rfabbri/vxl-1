//this is contrib/bm/ncn1/executables/crop_image_sequence.cxx


#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_convert.h>


#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_save.h>
#include <vil/vil_crop.h>

#include <vul/vul_file.h>

int main(int argc, char *argv[])
{
    
    vcl_string video_glob, out_dir;
 
    if( argc > 2 )
    {
        video_glob = argv[1];
        out_dir = argv[2];
    }
    else
    {
        vcl_cerr << "Error must provide video glob string and output directory." << vcl_flush;
        return 1;
    }

    vidl_image_list_istream video_stream(video_glob.c_str());

    for(unsigned t = 0; t < video_stream.num_frames(); ++t)
    {
        vcl_cout << "Cropping Frame " << t << " out of " << video_stream.num_frames() << vcl_endl;
        vil_image_view<vxl_byte> curr_img,crop_image;
        video_stream.seek_frame(t);
        vidl_convert_to_view(*video_stream.current_frame(),curr_img);
        crop_image = vil_crop(curr_img,185,144,312,45);     
        vcl_string filename = out_dir + '\\' + vul_file::basename(video_stream.current_path());
        vil_save(crop_image,filename.c_str());
    }



    return 0;
}