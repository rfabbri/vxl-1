//this is contrib/bm/smw/executables/car_exp.cxx

#include "smw/smw_world.h"

#include<vcl_iostream.h>
#include<vcl_string.h>

#include<vil/vil_convert.h>
#include<vil/vil_load.h>
#include<vil/vil_save.h>

#include<vul/vul_file.h>
#include<vul/vul_file_iterator.h>

#include<vcl_fstream.h>

int main()
{
    vcl_cout << "----------------------------------------\n"
             << "       Wind Turbine (blue background)\n"
             << "----------------------------------------\n";

    vcl_string root_dir = 
      "/media/DATAPART1/BrandonDataFolder/smw_data/WindTurbine/blue_background";
    vcl_string img_dir = root_dir + "/data";
    vcl_string out_dir = root_dir + "/change_map";
    vcl_string graph_dir = root_dir + "/graphs";
    vcl_string prob_dir = root_dir + "/prob_maps";
    
    vul_file::delete_file_glob(out_dir+"/*.bmp");
    vul_file::delete_file_glob(graph_dir + "/*.dot");
    vul_file::delete_file_glob(graph_dir + "/imgs/*.png");
    vul_file::delete_file_glob(prob_dir+"/*.png");


    //sort filenames
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
    vcl_string curr_graph_name;
    
    vcl_cout <<"***********Creatinging World************\n";
    vcl_cout << "Loading Image " << curr_filename  << "..." << vcl_endl;

    vil_convert_planes_to_grey<vxl_byte,float>
        ( vil_load(curr_filename.c_str()), curr_img );
    
    smw_world world(curr_img,300,0.0001f,40.0f,8.0f);

    unsigned g_col = 138, g_row = 66;
    curr_graph_name = graph_dir + "/" + *filename_itr + ".dot";
    vcl_cout << "Writing Graph Dot File..." << vcl_endl;
    vcl_cout << "Graph filename: " << curr_graph_name << vcl_endl;
    vcl_ofstream outfile(curr_graph_name.c_str());
    world.write_dot_file(outfile,g_col,g_row);
    outfile.close();

    vcl_cout << "Writing Prob Map..." << vcl_endl;
    vcl_string curr_prob_name = prob_dir + "/" + *filename_itr + ".png";
    vcl_cout << "Prob filename: " << curr_prob_name << vcl_endl;
    world.write_max_prob_map(curr_prob_name);
    
    //iterate through all files
    ++filename_itr;
    for(; filename_itr != filename_end; ++filename_itr)
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
        curr_output_name = out_dir + "/" + *filename_itr + "_change_map.bmp"; 
        vil_save(change_map,curr_output_name.c_str()); 

        curr_graph_name = graph_dir + "/" + *filename_itr + ".dot";
        vcl_cout << "Writing Graph Dot File..." << vcl_endl;
        vcl_cout << "Graph filename: " << curr_graph_name << vcl_endl;
        outfile.open(curr_graph_name.c_str());
        world.write_dot_file(outfile,g_col,g_row);
        outfile.close();

        vcl_cout << "Writing Prob Map..." << vcl_endl;
        vcl_string curr_prob_name = prob_dir + "/" + *filename_itr + ".png";
        vcl_cout << "Prob filename: " << curr_prob_name << vcl_endl;
        world.write_max_prob_map(curr_prob_name);

        

        change_map.clear();
        curr_img.clear();
    }

    return 0;
}
