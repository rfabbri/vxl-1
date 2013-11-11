#include<vcl_string.h>

#include<vil/vil_image_view.h>
#include<vil/vil_load.h>

int main()
{
    vcl_string img_name = 
        "/media/DATAPART1/BrandonDataFolder/";
    img_name = img_name + "ChangeDetection/westin/Data/00000001.png";

    vil_image_view<vxl_byte> img;
    img = vil_load(img_name.c_str());

    vcl_cout << "ni = " << img.ni() << vcl_endl;
    vcl_cout << "nj = " << img.nj() << vcl_endl;

    //nj = image height (rows)
    //ni = image width (columns) 
    return 0;
}
