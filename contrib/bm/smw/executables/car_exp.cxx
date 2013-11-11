//this is contrib/bm/smw/executables/car_exp.cxx

#include "smw/smw_world.h"

#include<vcl_iostream.h>
#include<vcl_fstream.h>
#include<vcl_string.h>

#include<vil/vil_convert.h>
#include<vil/vil_load.h>
#include<vil/vil_save.h>

#include<vnl/vnl_math.h>

#include<vul/vul_file.h>
#include<vul/vul_file_iterator.h>

void highlight_region(vil_image_view<vxl_byte> const& src,
                      vil_image_view<vxl_byte>& dest,
                      unsigned x1, unsigned y1,
                      unsigned x2, unsigned y2);

int main()
{
    vcl_cout << "----------------------------------------\n"
             << "       Two Car Experiment: Westin \n"
             << "----------------------------------------\n";

    vcl_string data_dir = "/media/DATAPART1/BrandonDataFolder/smw_data/westin/TwoCars_TopCenter";
    
    vcl_string img_dir = data_dir + "/OriginalData";
    vcl_string out_dir = data_dir + "/change_mask";
    vcl_string region_dir = data_dir + "/highlighted_region";
    vcl_string graph_dir = data_dir + "/graphs";

    vul_file::delete_file_glob(out_dir + "/*.bmp");
    vul_file::delete_file_glob(region_dir + "/*.png");

    //sort filenames
    vcl_vector<vcl_string> filenames;
    for(vul_file_iterator file_itr = img_dir + "/*.jpg";file_itr;++file_itr)
        filenames.push_back(vul_file::strip_extension(file_itr.filename()));

    vcl_vector<vcl_string>::iterator filename_itr = filenames.begin();
    vcl_vector<vcl_string>::iterator filename_end = filenames.end();
    vcl_sort(filename_itr,filename_end);

    vil_image_view<float> curr_img;
    vil_image_view<vxl_byte> change_map;
    vil_image_view<vxl_byte> orig_img;
    vil_image_view<vxl_byte> highlighted_img;

    vcl_string curr_filename = img_dir + "/" + *filename_itr + ".jpg";
    vcl_string curr_output_name;
    vcl_string curr_region_name;

    vcl_cout <<"***********Creatinging World************\n"
                 <<"Current Image: " << curr_filename << vcl_endl;

    curr_region_name =region_dir + "/" + *filename_itr + ".png";

    vcl_cout << "Loading Image... " << vcl_endl;
    vil_convert_planes_to_grey<vxl_byte,float>
        ( vil_load(curr_filename.c_str()),curr_img );
    vil_convert_planes_to_grey<vxl_byte,vxl_byte>
        ( vil_load(curr_filename.c_str()),orig_img );

    unsigned x1 = 688, y1 = 100, x2 = 900, y2 = 300;

    highlight_region(orig_img,highlighted_img,x1,y1,x2,y2);
    vil_save(highlighted_img,curr_region_name.c_str());
    
    smw_world world(curr_img,x1,y1,x2,y2,30,0.000001,40.0f,15.0f);

    vcl_string curr_graph_name = graph_dir + "/" + *filename_itr + ".dot";
    vcl_cout << "Writing Graph Dot File..." << vcl_endl;
    vcl_cout << "Graph filename: " << curr_graph_name << vcl_endl;
    vcl_ofstream outfile(curr_graph_name.c_str());
    world.write_dot_file(outfile,10,10);
    outfile.close();

    //iterate through all files
    ++filename_itr;
    for(;filename_itr!=filename_end;++filename_itr)
    {
        curr_filename = img_dir + "/" + *filename_itr + ".jpg";
        vcl_cout <<"************Updating World************\n"
                 <<"Current Image: " << curr_filename << vcl_endl;
        
        vcl_cout << "Loading Image..." << vcl_endl;
        vil_convert_planes_to_grey<vxl_byte,float>
            ( vil_load(curr_filename.c_str()), curr_img);

        vcl_cout << "Saving High lighted region..." << vcl_endl;
        
        vil_convert_planes_to_grey<vxl_byte,vxl_byte>
        ( vil_load(curr_filename.c_str()),orig_img );
        curr_region_name =region_dir + "/" + *filename_itr + ".png";
        highlight_region(orig_img,highlighted_img,x1,y1,x2,y2);
        vil_save(highlighted_img,curr_region_name.c_str());

        vcl_cout << "Calling Update Function..." << vcl_endl;
        world.update(curr_img);  
         
        vcl_cout << "Creating Change Map..." << vcl_endl;
        change_map = world.change_map();

        vcl_cout << "Saving Change Map..." << vcl_endl;
        curr_output_name = out_dir + "/" + *filename_itr + "_change_map.bmp"; 
        vil_save(change_map,curr_output_name.c_str());  

        vcl_cout << "Writing Graph Dot File..." << vcl_endl;
        vcl_string curr_graph_name = graph_dir + "/" + *filename_itr + ".dot";
        vcl_cout << "Graph filename: " << curr_graph_name << vcl_endl;
        outfile.open(curr_graph_name.c_str());
        world.write_dot_file(outfile,10,10);
        outfile.close();
        
    }

    change_map.clear();
    curr_img.clear();
    
}


void highlight_region(vil_image_view<vxl_byte> const& src,
                      vil_image_view<vxl_byte>& dest,
                      unsigned x1, unsigned y1,
                      unsigned x2, unsigned y2)
{
    unsigned ni = src.ni();
    unsigned nj = src.nj();
    dest.clear();
    dest.set_size(src.ni(),src.nj(),1);
    float obs;


    for(unsigned row = 0; row < nj; ++row)
        for(unsigned col = 0; col < ni; ++col)
        {
            if( (row < y1 || row > y2) || (col < x1 || col > x2) )
            {
                obs = src(col,row,0) - 60;
                if(obs < 0 )
                    obs = 0;
                dest(col,row,0) = obs;
            }
            else
                dest(col,row,0) = src(col,row,0);
        }
}//end highlight_region
