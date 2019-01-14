//this is contrib/bm/smw/executables/westin_experiment_1.cxx

#include "smw/smw_world.h"

#include<iostream>
#include<string>

#include<vil/vil_convert.h>
#include<vil/vil_load.h>
#include<vil/vil_save.h>

#include<vul/vul_file.h>
#include<vul/vul_file_iterator.h>

int main()
{

    std::cout << "----------------------------------------\n"
             << "          WESTIN EXPERIMENT 1 \n"
             << "----------------------------------------\n";

    std::string img_dir = "/media/DATAPART1/BrandonDataFolder/smw_data/westin/10fps_input";

    std::string out_dir = "/media/DATAPART1/BrandonDataFolder/smw_data/westin/10fps_change_map";

    //to sort filenames and load them in the correct order.
    std::vector<std::string> filenames;
    for(vul_file_iterator file_itr = img_dir + "/*.jpg";file_itr;++file_itr)
        filenames.push_back(vul_file::strip_extension(file_itr.filename()));

    std::vector<std::string>::iterator filename_itr = filenames.begin();
    std::vector<std::string>::iterator filename_end = filenames.end();
    std::sort(filename_itr,filename_end);

    vil_image_view<float> curr_img;
    vil_image_view<vxl_byte> change_map;

    std::string curr_filename = img_dir + "/" + *filename_itr + ".jpg";
    std::string curr_output_name;

    std::cout <<"***********Creatinging World************\n"
                 <<"Current Image: " << curr_filename << std::endl;

    std::cout << "Loading Image... " << std::endl;
    vil_convert_planes_to_grey<vxl_byte,float>
        ( vil_load(curr_filename.c_str()),curr_img );

    smw_world world(curr_img);
    
  

    //iterate through all files

    for(;filename_itr!=filename_end;++filename_itr)
    {
        curr_filename = img_dir + "/" + *filename_itr + ".jpg";
        std::cout <<"************Updating World************\n"
                 <<"Current Image: " << curr_filename << std::endl;
        
        std::cout << "Loading Image..." << std::endl;
        vil_convert_planes_to_grey<vxl_byte,float>
            ( vil_load(curr_filename.c_str()), curr_img);

        std::cout << "Calling Update Function..." << std::endl;
        world.update(curr_img);  
         
        std::cout << "Creating Change Map..." << std::endl;
        change_map = world.change_map();

        std::cout << "Saving Change Map..." << std::endl;
        curr_output_name = out_dir + "/" + *filename_itr + "_change_map.jpg"; 
        vil_save(change_map,curr_output_name.c_str());       
    }

    return 0;
}
