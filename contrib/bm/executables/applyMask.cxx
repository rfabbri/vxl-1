#include<vil/vil_convert.h>
#include<vil/vil_load.h>
#include<vil/vil_image_view.h>
#include<vil/vil_save.h>

#include<vcl_map.h> //for vcl_map
#include<vcl_string.h> 
#include<vcl_utility.h> //for vcl_pair::make_pair

#include<vul/vul_file_iterator.h>
#include<vul/vul_file.h>


int main()
{
    vcl_cout << "Running apply_mask_exec" << '\n' << vcl_endl;

    vcl_string img_dir = "/media/DATAPART1/BrandonDataFolder/ChangeDetection/ParkedCarData/";
    vcl_string mask_fullpath = "/media/DATAPART1/BrandonDataFolder/ChangeDetection/masks/carMask.png";
    vcl_string output_dir = "/media/DATAPART1/BrandonDataFolder/ChangeDetection/ParkedCarDataUnderMask/";

    vcl_cout << "Input image directory: " << img_dir << vcl_endl;
    vcl_cout << "Full path to mask: " << mask_fullpath << vcl_endl;
    vcl_cout << "Output directory: " << output_dir << vcl_endl << vcl_endl;

    vil_image_view<vxl_byte> mask = vil_convert_cast(vxl_byte(),vil_load(mask_fullpath.c_str()));

    //load images
    vcl_string img_names = img_dir + "*.png";
    for(vul_file_iterator fn = img_names; fn; ++fn)
    {
        vcl_string filename = vul_file::strip_directory(fn());
        filename = vul_file::strip_extension(filename);
        vcl_cout << "1. Loading Image: " << filename << vcl_endl;
        vil_image_view<vxl_byte> input;
        input = vil_convert_to_n_planes(1,vil_convert_cast(vxl_byte(), vil_load(fn())));
        vcl_cout << "input.ni() = " << input.ni() << " " << "input.nj() = " << input.nj() 
                 << " " << "input.nplanes() = " << input.nplanes() << vcl_endl;
        
        
        //apply mask
        vcl_cout << "2. Applying Binary Mask" << vcl_endl;
        if( (input.ni() == mask.ni()) && (input.nj() == mask.nj()))
        {
            vil_image_view<vxl_byte> output;
            output.set_size(input.ni(),input.nj(),1);
            for(unsigned i = 0; i < input.ni(); ++i)
                for(unsigned j = 0; j < input.nj(); ++j)
                    output(i,j,0) = input(i,j,0) * mask(i,j,0);
            //save output
            vcl_cout << "3. Saving result " << vcl_endl;
            vcl_string outName = output_dir+filename+".png";
            vil_save(output,outName.c_str());
            
        }       
    }

return 0;
}
