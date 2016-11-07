//this is contrib/bm/smw/executables/westin_experiment_1.cxx

#include "smw/smw_world.h"

#include<vcl_iostream.h>
#include<vcl_string.h>

#include<vil/vil_convert.h>
#include<vil/vil_load.h>
#include<vil/vil_save.h>

#include<vul/vul_file.h>
#include<vul/vul_file_iterator.h>

int main()
{

    vcl_cout << "----------------------------------------\n"
             << "          WESTIN EXPERIMENT 1 \n"
             << "----------------------------------------\n";

    vcl_string img_dir = "/media/DATAPART1/BrandonDataFolder/smw_data/westin/10fps_input";

    vcl_string out_dir = "/media/DATAPART1/BrandonDataFolder/smw_data/westin/10fps_change_map";

    //to sort filenames and load them in the correct order.
    vcl_vector<vcl_string> filenames;
    for(vul_file_iterator file_itr = img_dir + "/*.jpg";file_itr;++file_itr)
        filenames.push_back(vul_file::strip_extension(file_itr.filename()));

    vcl_vector<vcl_string>::iterator filename_itr = filenames.begin();
    vcl_vector<vcl_string>::iterator filename_end = filenames.end();
    vcl_sort(filename_itr,filename_end);

    vil_image_view<float> curr_img;
    vil_image_view<vxl_byte> change_map;

    vcl_string curr_filename = img_dir + "/" + *filename_itr + ".jpg";
    vcl_string curr_output_name;

    vcl_cout <<"***********Creatinging World************\n"
                 <<"Current Image: " << curr_filename << vcl_endl;

    vcl_cout << "Loading Image... " << vcl_endl;
    vil_convert_planes_to_grey<vxl_byte,float>
        ( vil_load(curr_filename.c_str()),curr_img );

    smw_world world(curr_img);
    
  

    //iterate through all files

    for(;filename_itr!=filename_end;++filename_itr)
    {
        curr_filename = img_dir + "/" + *filename_itr + ".jpg";
        vcl_cout <<"************Updating World************\n"
                 <<"Current Image: " << curr_filename << vcl_endl;
        
        vcl_cout << "Loading Image..." << vcl_endl;
        vil_convert_planes_to_grey<vxl_byte,float>
            ( vil_load(curr_filename.c_str()), curr_img);

        vcl_cout << "Calling Update Function..." << vcl_endl;
        world.update(curr_img);  
         
        vcl_cout << "Creating Change Map..." << vcl_endl;
        change_map = world.change_map();

        vcl_cout << "Saving Change Map..." << vcl_endl;
        curr_output_name = out_dir + "/" + *filename_itr + "_change_map.jpg"; 
        vil_save(change_map,curr_output_name.c_str());       
    }

    return 0;
}
