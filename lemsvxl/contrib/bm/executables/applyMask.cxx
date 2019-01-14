#include<vil/vil_convert.h>
#include<vil/vil_load.h>
#include<vil/vil_image_view.h>
#include<vil/vil_save.h>

#include<map> //for std::map
#include<string> 
#include<utility> //for std::pair::make_pair

#include<vul/vul_file_iterator.h>
#include<vul/vul_file.h>


int main()
{
    std::cout << "Running apply_mask_exec" << '\n' << std::endl;

    std::string img_dir = "/media/DATAPART1/BrandonDataFolder/ChangeDetection/ParkedCarData/";
    std::string mask_fullpath = "/media/DATAPART1/BrandonDataFolder/ChangeDetection/masks/carMask.png";
    std::string output_dir = "/media/DATAPART1/BrandonDataFolder/ChangeDetection/ParkedCarDataUnderMask/";

    std::cout << "Input image directory: " << img_dir << std::endl;
    std::cout << "Full path to mask: " << mask_fullpath << std::endl;
    std::cout << "Output directory: " << output_dir << std::endl << std::endl;

    vil_image_view<vxl_byte> mask = vil_convert_cast(vxl_byte(),vil_load(mask_fullpath.c_str()));

    //load images
    std::string img_names = img_dir + "*.png";
    for(vul_file_iterator fn = img_names; fn; ++fn)
    {
        std::string filename = vul_file::strip_directory(fn());
        filename = vul_file::strip_extension(filename);
        std::cout << "1. Loading Image: " << filename << std::endl;
        vil_image_view<vxl_byte> input;
        input = vil_convert_to_n_planes(1,vil_convert_cast(vxl_byte(), vil_load(fn())));
        std::cout << "input.ni() = " << input.ni() << " " << "input.nj() = " << input.nj() 
                 << " " << "input.nplanes() = " << input.nplanes() << std::endl;
        
        
        //apply mask
        std::cout << "2. Applying Binary Mask" << std::endl;
        if( (input.ni() == mask.ni()) && (input.nj() == mask.nj()))
        {
            vil_image_view<vxl_byte> output;
            output.set_size(input.ni(),input.nj(),1);
            for(unsigned i = 0; i < input.ni(); ++i)
                for(unsigned j = 0; j < input.nj(); ++j)
                    output(i,j,0) = input(i,j,0) * mask(i,j,0);
            //save output
            std::cout << "3. Saving result " << std::endl;
            std::string outName = output_dir+filename+".png";
            vil_save(output,outName.c_str());
            
        }       
    }

return 0;
}
