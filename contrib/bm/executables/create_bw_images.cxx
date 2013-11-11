#include<vcl_string.h>

#include<vil/vil_image_view.h>
#include<vil/vil_save.h>

int main()
{
    vcl_string output_dir = "/media/DATAPART1/BrandonDataFolder/ChangeDetection/westin/bw_images/";

    
    
    vil_image_view<vxl_byte> black_img(20,40,1);
    vil_image_view<vxl_byte> white_img(20,40,1);

    for(unsigned i = 0; i < black_img.ni(); ++i)
        for(unsigned j = 0; j < black_img.nj(); ++j)
        {
            black_img(i,j,0) = 0;
            white_img(i,j,0) = 255;
        }

    vcl_string black_out = output_dir + "black_20_40.png";
    vcl_string white_out = output_dir + "white_20_40.png";
    
    vil_save(black_img,black_out.c_str());
    vil_save(white_img,white_out.c_str());

    return 0;
}
