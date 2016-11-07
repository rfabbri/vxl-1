//this is contrib/bm/smw/executables/bw_1

#include"smw/smw_world.h"

#include<vcl_string.h>
#include<vcl_vector.h>

#include<vil/vil_convert.h>
#include<vil/vil_save.h>

#include<vul/vul_file.h>
#include<vul/vul_file_iterator.h>

#include<vcl_fstream.h>

int main()
{
    vcl_cout << "-----------------------------------------\n"
             << "      Black and White Experiment\n"
             << "-----------------------------------------\n" << vcl_endl;

    vcl_string data_dir = "/media/DATAPART1/BrandonDataFolder/smw_data";
    vcl_string img_dir = data_dir + "/bw/bw_1_input";
    vcl_string graph_dir = data_dir + "/bw/graphs";

    vcl_cout << "img_dir = " << img_dir << vcl_endl;

    vcl_string change_map_dir = data_dir + "/bw/bw_1_change_map";

    vcl_vector<vcl_string> filenames;
    for(vul_file_iterator file_itr = img_dir +"/*.png";file_itr;++file_itr)
        filenames.push_back(vul_file::strip_extension(file_itr.filename()));
    
    vcl_vector<vcl_string>::iterator filename_itr = filenames.begin();
    vcl_vector<vcl_string>::iterator filename_end = filenames.end();
    vcl_sort(filename_itr,filename_end);

    vil_image_view<float> curr_img;
    vil_image_view<vxl_byte> change_map;
    
    vcl_cout << "*filename_itr = " << *filename_itr << vcl_endl;

    vcl_string curr_filename = img_dir + "/" + *filename_itr + ".png";
    vcl_string curr_output_name;
    vcl_string curr_graph_name;

    vcl_cout <<"***********Creating World************\n"
             <<"Current Image: " << curr_filename << vcl_endl;

    vcl_cout << "Loading Image " << *filename_itr + ".png" << "..." << vcl_endl;

    vil_convert_cast<vxl_byte,float>
        ( vil_load(curr_filename.c_str()),curr_img );
    

    vcl_cout << "Creating World With First Image..." << vcl_endl;
    smw_world world(curr_img);
    
    curr_graph_name = graph_dir + "/" + *filename_itr + ".dot";
    vcl_cout << "Writing Graph Dot File..." << vcl_endl;
    vcl_cout << "Graph filename: " << curr_graph_name << vcl_endl;
    vcl_ofstream outfile(curr_graph_name.c_str());
    world.write_dot_file(outfile,10,10);
    outfile.close();

    ++filename_itr;
    for(; filename_itr != filename_end; ++filename_itr)
    {
        curr_filename = img_dir + "/" + *filename_itr + ".png";
        curr_output_name = change_map_dir + "/" + *filename_itr + ".bmp";
        curr_graph_name = graph_dir + "/" + *filename_itr + ".dot";
        vcl_cout <<"***********Loading " 
                 << *filename_itr + ".png"
                 << "************\n";
        vil_convert_cast<vxl_byte,float>
            ( vil_load(curr_filename.c_str()), curr_img );
        
        vcl_cout << "Updating World..." << vcl_endl;
        world.update(curr_img);

        vcl_cout << "Creating Change Map..." << vcl_endl;
        change_map = world.change_map();

        vcl_cout << "Saving Change Map..." << vcl_endl;
        vil_save(change_map,curr_output_name.c_str());

        vcl_cout << world.world_[0][0] << vcl_endl;

        vcl_cout << "Writing Graph Dot File..." << vcl_endl;
        vcl_cout << "Graph filename: " << curr_graph_name << vcl_endl;
        outfile.open(curr_graph_name.c_str());
        world.write_dot_file(outfile,10,10);
        outfile.close();
        
    }
    
    return 0;
}

