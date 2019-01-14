//this is contrib/bm/smw/executables/bw_1

#include"smw/smw_world.h"

#include<string>
#include<vector>

#include<vil/vil_convert.h>
#include<vil/vil_save.h>

#include<vul/vul_file.h>
#include<vul/vul_file_iterator.h>

#include<fstream>

int main()
{
    std::cout << "-----------------------------------------\n"
             << "      Black and White Experiment\n"
             << "-----------------------------------------\n" << std::endl;

    std::string data_dir = "/media/DATAPART1/BrandonDataFolder/smw_data";
    std::string img_dir = data_dir + "/bw/bw_1_input";
    std::string graph_dir = data_dir + "/bw/graphs";

    std::cout << "img_dir = " << img_dir << std::endl;

    std::string change_map_dir = data_dir + "/bw/bw_1_change_map";

    std::vector<std::string> filenames;
    for(vul_file_iterator file_itr = img_dir +"/*.png";file_itr;++file_itr)
        filenames.push_back(vul_file::strip_extension(file_itr.filename()));
    
    std::vector<std::string>::iterator filename_itr = filenames.begin();
    std::vector<std::string>::iterator filename_end = filenames.end();
    std::sort(filename_itr,filename_end);

    vil_image_view<float> curr_img;
    vil_image_view<vxl_byte> change_map;
    
    std::cout << "*filename_itr = " << *filename_itr << std::endl;

    std::string curr_filename = img_dir + "/" + *filename_itr + ".png";
    std::string curr_output_name;
    std::string curr_graph_name;

    std::cout <<"***********Creating World************\n"
             <<"Current Image: " << curr_filename << std::endl;

    std::cout << "Loading Image " << *filename_itr + ".png" << "..." << std::endl;

    vil_convert_cast<vxl_byte,float>
        ( vil_load(curr_filename.c_str()),curr_img );
    

    std::cout << "Creating World With First Image..." << std::endl;
    smw_world world(curr_img);
    
    curr_graph_name = graph_dir + "/" + *filename_itr + ".dot";
    std::cout << "Writing Graph Dot File..." << std::endl;
    std::cout << "Graph filename: " << curr_graph_name << std::endl;
    std::ofstream outfile(curr_graph_name.c_str());
    world.write_dot_file(outfile,10,10);
    outfile.close();

    ++filename_itr;
    for(; filename_itr != filename_end; ++filename_itr)
    {
        curr_filename = img_dir + "/" + *filename_itr + ".png";
        curr_output_name = change_map_dir + "/" + *filename_itr + ".bmp";
        curr_graph_name = graph_dir + "/" + *filename_itr + ".dot";
        std::cout <<"***********Loading " 
                 << *filename_itr + ".png"
                 << "************\n";
        vil_convert_cast<vxl_byte,float>
            ( vil_load(curr_filename.c_str()), curr_img );
        
        std::cout << "Updating World..." << std::endl;
        world.update(curr_img);

        std::cout << "Creating Change Map..." << std::endl;
        change_map = world.change_map();

        std::cout << "Saving Change Map..." << std::endl;
        vil_save(change_map,curr_output_name.c_str());

        std::cout << world.world_[0][0] << std::endl;

        std::cout << "Writing Graph Dot File..." << std::endl;
        std::cout << "Graph filename: " << curr_graph_name << std::endl;
        outfile.open(curr_graph_name.c_str());
        world.write_dot_file(outfile,10,10);
        outfile.close();
        
    }
    
    return 0;
}

