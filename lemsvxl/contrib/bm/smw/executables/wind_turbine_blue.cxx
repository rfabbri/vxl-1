//this is contrib/bm/smw/executables/car_exp.cxx

#include "smw/smw_world.h"

#include<iostream>
#include<string>

#include<vil/vil_convert.h>
#include<vil/vil_load.h>
#include<vil/vil_save.h>

#include<vul/vul_file.h>
#include<vul/vul_file_iterator.h>

#include<fstream>

int main()
{
    std::cout << "----------------------------------------\n"
             << "       Wind Turbine (blue background)\n"
             << "----------------------------------------\n";

    std::string root_dir = 
      "/media/DATAPART1/BrandonDataFolder/smw_data/WindTurbine/blue_background";
    std::string img_dir = root_dir + "/data";
    std::string out_dir = root_dir + "/change_map";
    std::string graph_dir = root_dir + "/graphs";
    std::string prob_dir = root_dir + "/prob_maps";
    
    vul_file::delete_file_glob(out_dir+"/*.bmp");
    vul_file::delete_file_glob(graph_dir + "/*.dot");
    vul_file::delete_file_glob(graph_dir + "/imgs/*.png");
    vul_file::delete_file_glob(prob_dir+"/*.png");


    //sort filenames
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
    std::string curr_graph_name;
    
    std::cout <<"***********Creatinging World************\n";
    std::cout << "Loading Image " << curr_filename  << "..." << std::endl;

    vil_convert_planes_to_grey<vxl_byte,float>
        ( vil_load(curr_filename.c_str()), curr_img );
    
    smw_world world(curr_img,300,0.0001f,40.0f,8.0f);

    unsigned g_col = 138, g_row = 66;
    curr_graph_name = graph_dir + "/" + *filename_itr + ".dot";
    std::cout << "Writing Graph Dot File..." << std::endl;
    std::cout << "Graph filename: " << curr_graph_name << std::endl;
    std::ofstream outfile(curr_graph_name.c_str());
    world.write_dot_file(outfile,g_col,g_row);
    outfile.close();

    std::cout << "Writing Prob Map..." << std::endl;
    std::string curr_prob_name = prob_dir + "/" + *filename_itr + ".png";
    std::cout << "Prob filename: " << curr_prob_name << std::endl;
    world.write_max_prob_map(curr_prob_name);
    
    //iterate through all files
    ++filename_itr;
    for(; filename_itr != filename_end; ++filename_itr)
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
        curr_output_name = out_dir + "/" + *filename_itr + "_change_map.bmp"; 
        vil_save(change_map,curr_output_name.c_str()); 

        curr_graph_name = graph_dir + "/" + *filename_itr + ".dot";
        std::cout << "Writing Graph Dot File..." << std::endl;
        std::cout << "Graph filename: " << curr_graph_name << std::endl;
        outfile.open(curr_graph_name.c_str());
        world.write_dot_file(outfile,g_col,g_row);
        outfile.close();

        std::cout << "Writing Prob Map..." << std::endl;
        std::string curr_prob_name = prob_dir + "/" + *filename_itr + ".png";
        std::cout << "Prob filename: " << curr_prob_name << std::endl;
        world.write_max_prob_map(curr_prob_name);

        

        change_map.clear();
        curr_img.clear();
    }

    return 0;
}
