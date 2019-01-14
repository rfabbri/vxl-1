#include<string>

#include<vil/vil_image_view.h>
#include<vil/vil_load.h>

int main()
{
    std::string img_name = 
        "/media/DATAPART1/BrandonDataFolder/";
    img_name = img_name + "ChangeDetection/westin/Data/00000001.png";

    vil_image_view<vxl_byte> img;
    img = vil_load(img_name.c_str());

    std::cout << "ni = " << img.ni() << std::endl;
    std::cout << "nj = " << img.nj() << std::endl;

    //nj = image height (rows)
    //ni = image width (columns) 
    return 0;
}
