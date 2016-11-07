//this is contrib/bm/smw/executables/around_the_block.cxx

#include "smw/smw_world.h"

#include<vcl_iostream.h>
#include<vcl_string.h>

#include<vil/vil_convert.h>
#include<vil/vil_load.h>
#include<vil/vil_rgb.h>
#include<vil/vil_save.h>
#include<vil/vil_image_view.h>

#include<vul/vul_file.h>
#include<vul/vul_file_iterator.h>

#include<vcl_fstream.h>

void highlight_region(vil_image_view<vil_rgb<vxl_byte> > const& src,
                      vil_image_view<vil_rgb<vxl_byte> >& dest,
                      unsigned x1, unsigned y1,
                      unsigned x2, unsigned y2,
                      unsigned gx, unsigned gy);

int main()
{
    vcl_cout << "----------------------------------------\n"
             << "      Westing Car Around Block\n"
             << "----------------------------------------\n";

    vcl_string root_dir =
        "/media/DATAPART1/BrandonDataFolder/smw_data/car_around_block/";
    vcl_string img_dir = root_dir + "img_dir/";
    vcl_string change_dir = root_dir + "change_dir/";
    vcl_string graph_dir = root_dir + "graph_dir/";
    vcl_string highlight_dir = root_dir + "highlight_dir/";
    vcl_string prob_dir = root_dir +"prob_dir/";

    vil_image_view<float> curr_img;
    vil_image_view< vil_rgb<vxl_byte> > orig_img;
    vil_image_view< vil_rgb<vxl_byte> > highlight_img;
    vil_image_view<vxl_byte> change_map;
    vcl_string curr_filename;
    vcl_string curr_change_name;
    vcl_string curr_graph_name;
    vcl_string curr_prob_name;
    vcl_string curr_highlight_name;

    unsigned x1 = 145, y1 = 300, x2 = 635, y2 = 650;
    //the point where we want to visualize the graph
    unsigned gx = 399,gy = 326;

    //delete old results
    vul_file::delete_file_glob(change_dir + "/*.bmp");
    vul_file::delete_file_glob(graph_dir + "/*.dot");
    vul_file::delete_file_glob(graph_dir + "/graph_png/*.png");
    vul_file::delete_file_glob(prob_dir + "/*.png");
    vul_file::delete_file_glob(highlight_dir + "/*.png");

    //sort filenames
    vcl_vector<vcl_string> filenames;
    for(vul_file_iterator file_itr = img_dir + "/*.jpg"; file_itr; ++file_itr)
        filenames.push_back(vul_file::strip_extension(file_itr.filename()));

    vcl_vector<vcl_string>::iterator filename_itr = filenames.begin();
    vcl_vector<vcl_string>::iterator filename_end = filenames.end();
    vcl_sort(filename_itr,filename_end);

    curr_filename = img_dir + *filename_itr + ".jpg";

    vcl_cout << "-----Creating World-----" << vcl_endl;
    vcl_cout << "Loading Image: " << *filename_itr << vcl_endl;
    vil_convert_planes_to_grey<vxl_byte,float>
        (vil_load(curr_filename.c_str()),curr_img);
    orig_img = vil_load(curr_filename.c_str());

    vcl_cout << "Highlighting ROI" << vcl_endl;
    curr_highlight_name = highlight_dir + *filename_itr + ".png";
    highlight_region(orig_img,highlight_img,x1,y1,x2,y2,gx,gy);
    vil_save(highlight_img,curr_highlight_name.c_str());

    //create world
    smw_world world(curr_img,x1,y1,x2,y2,5,.000001,40.0f,15.0f);
    

    curr_graph_name = graph_dir + *filename_itr + ".dot";
    vcl_cout << "Writing Graph File" << vcl_endl;
    vcl_ofstream outfile(curr_graph_name.c_str());
    world.write_dot_file(outfile,gx,gy);
    outfile.close();

    //go to the second img 
    ++filename_itr;
    //iterate through the rest of the files.
    for(;filename_itr!=filename_end;++filename_itr)
    {
        curr_filename = img_dir + *filename_itr + ".jpg";
        vcl_cout << "-----Updating World-----" << vcl_endl;
        
        vcl_cout << "Loading Image: " << *filename_itr << vcl_endl;
        vil_convert_planes_to_grey<vxl_byte,float>
            ( vil_load(curr_filename.c_str()), curr_img );

        vcl_cout << "Saving Highlighted Region" << vcl_endl;
        orig_img = vil_load(curr_filename.c_str());
        curr_highlight_name = highlight_dir + *filename_itr + ".png";
        highlight_region(orig_img,highlight_img,x1,y1,x2,y2,gx,gy);
        vil_save(highlight_img,curr_highlight_name.c_str());

        vcl_cout << "Calling World Update Function" << vcl_endl;
        world.update(curr_img);

        vcl_cout << "Creating Change Map" << vcl_endl;
        change_map = world.change_map();
        
        vcl_cout << "Saving Change Map" << vcl_endl;
        curr_change_name = change_dir + *filename_itr + ".bmp";
        vil_save(change_map,curr_change_name.c_str());

        vcl_cout << "Writing Graph Dot File" << vcl_endl;
        curr_graph_name = graph_dir + *filename_itr + ".dot";
        outfile.open(curr_graph_name.c_str());
        world.write_dot_file(outfile,gx,gy);
        outfile.close();

        change_map.clear();
        curr_img.clear();
        
    }//end iterate through imgs
    


    return 0;
}



void highlight_region(vil_image_view<vil_rgb<vxl_byte> > const& src,
                      vil_image_view<vil_rgb<vxl_byte> >& dest,
                      unsigned x1, unsigned y1,
                      unsigned x2, unsigned y2,
                      unsigned gx, unsigned gy)
{
    unsigned ni = src.ni();
    unsigned nj = src.nj();
    dest.clear();
    dest.set_size(src.ni(),src.nj(),1);
    int obs_r,obs_g,obs_b;


    for(unsigned row = 0; row < nj; ++row)
        for(unsigned col = 0; col < ni; ++col)
        {
            
            if( (row < y1 || row > y2) || (col < x1 || col > x2) )
            {
                obs_r = src(col,row).r - 60;
                obs_g = src(col,row).g - 60;
                obs_b = src(col,row).b - 60;
                if(obs_r < 0 )
                    obs_r = 0;
                if(obs_g < 0 )
                    obs_g = 0;
                if(obs_b < 0 )
                    obs_b = 0;
                dest(col,row) = vil_rgb<vxl_byte>(obs_r,obs_g,obs_b);
            }
            else
                dest(col,row) = src(col,row);

            if( ( (col >= gy - 10)  && (col <= gy + 10)) &&
                ( (row >= gx - 10 ) && (row <= gx + 10)) )
            {
                dest(col,row) = vil_rgb<vxl_byte>(0,255,0);
            }
            
        }

}//end highlight_region
