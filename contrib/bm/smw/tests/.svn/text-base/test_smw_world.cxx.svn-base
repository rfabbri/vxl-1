//this is smw/tests/test_smw_world.cxx

#include "smw_world.h"

#include <testlib/testlib_test.h>

#include <vcl_algorithm.h> //for sort algorithm
#include <vcl_map.h> //for map container
#include <vcl_string.h>
#include <vcl_utility.h> //for std::pair
#include <vcl_vector.h>

#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>

#include <vul/vul_file_iterator.h> //file iterator to parse files
#include <vul/vul_file.h>

static void test_smw_world()
{
    //some variables used by all tests
    vcl_string cwd = vul_file::get_cwd();
    #define BW_TEST 0
    #if BW_TEST
    //=============== Black and White Test =================
    {//begin black and white test block (define a local scope)

        // the black and white directory contains 40 images.
        // the first 20 are black, the last 20 are white. 
        // all images are 20x20 pixels in size
        vcl_string bw_dir = 
            "/media/DATAPART1/BrandonDataFolder/ChangeDetection/westin/bw_data";
    
        vcl_string bw_result_dir = cwd+"/bw_results";
        if(!vul_file::exists(bw_result_dir))
            vul_file::make_directory(bw_result_dir);
        vul_file_iterator bw_itr = bw_dir+"/*.png";
    

        smw_world bw_world;
    
        //iterate through the bw directory updating the
        //bw_world with each image. The first change should occur
        //during the transition between the 20th and 21st image.
        vcl_vector<vcl_string> filenames;
        for(;bw_itr;++bw_itr)
        {
            vcl_string filename = 
                vul_file::strip_extension(bw_itr.filename());
            filenames.push_back(filename);
        }
        vcl_vector<vcl_string>::iterator name_itr = filenames.begin();
        vcl_vector<vcl_string>::iterator name_end = filenames.end();

        vcl_sort(name_itr,name_end);//sorts the file names 

    
        vcl_cout << "========== Black and White Test ==========="
                 << vcl_endl;
        vil_image_view<vxl_byte> bw_img;
        vil_image_view<vxl_byte> bw_chg_map;
        vcl_string filename;
        vcl_string output_filename;
        //loop through all images, update the world and output change maps
        bool first_loop = true;
        for(name_itr = filenames.begin();name_itr!=name_end;++name_itr)
        {        
            vcl_cout << "Updating world with image: " 
                     << *name_itr << vcl_endl;
            filename = bw_dir+'/'+*name_itr+".png";
            output_filename = bw_result_dir+'/'+*name_itr+"_chg_map.png";
            bw_img = vil_load(filename.c_str());

            if(first_loop)
            {
                bw_world.set_size(bw_img.ni(),bw_img.nj());
            }

            bw_world.update(bw_img);
            bw_chg_map = bw_world.change_map();
            vil_save(bw_chg_map,output_filename.c_str());  
      
            first_loop = false;
        }
    }
    #endif //end black and white test block

    #define PCM_TEST 1
    
    #if PCM_TEST
    //======== Parked Car Under Mask Test Block ========
    {// begin parked car under mask test block
       //pcm stance for parked car under mask
        vcl_cout << "========== Parked Car Under Mask Test ==========="
             << vcl_endl;
        vcl_string pcm_dir = 
            "/media/DATAPART1/BrandonDataFolder/";
        pcm_dir +="ChangeDetection/westin/ParkedCarDataUnderMask";
        vcl_string pcm_results_dir = cwd+"/pcm_results";
        if(!vul_file::exists(pcm_results_dir))
            vul_file::make_directory(pcm_results_dir.c_str());
          
        smw_world pcm_world;

        vul_file_iterator pcm_itr = pcm_dir + "/*.png";
        
        vcl_vector<vcl_string> filenames;
        
        //load the file names
        for(;pcm_itr;++pcm_itr)
        {
            vcl_string filename = 
                vul_file::strip_extension(pcm_itr.filename());
            filenames.push_back(filename);
        }

        //sort the file names so they are processed
        //in the correct order
        vcl_vector<vcl_string>::iterator filename_itr = 
            filenames.begin();
        vcl_vector<vcl_string>::iterator filename_end =
            filenames.end();
        vcl_sort(filename_itr,filename_end);

        vil_image_view<vxl_byte> pcm_img;
        vil_image_view<vxl_byte> pcm_chg_map;
        vcl_string output_name;
        vcl_string filename;
        filename_itr = filenames.begin();

        //do for the first image outside the loop to set the correct
        //size
        vcl_cout << "========= Updating World with Image: "
                 << *filename_itr  << ".png" << " =========" << vcl_endl;

        filename = pcm_dir+"/"+*filename_itr+".png";
        pcm_img = vil_load(filename.c_str());

        pcm_world.set_size(pcm_img.ni(),pcm_img.nj());
        pcm_world.update(pcm_img);

        vcl_cout << "Creating Change Map..." << vcl_endl;
        pcm_chg_map = pcm_world.change_map();
        output_name = pcm_results_dir + '/' + *filename_itr 
            + ".png";
        vil_save(pcm_chg_map,output_name.c_str());

        ++filename_itr;
        for(;filename_itr!=filename_end;++filename_itr)
        {
            vcl_cout << "========= Updating World with Image: "
                     << *filename_itr << ".png" << " =========" << vcl_endl;
            
            filename = pcm_dir+"/"+*filename_itr+".png";
            pcm_img = vil_load(filename.c_str());

            pcm_world.update(pcm_img);
            vcl_cout << "Creating Change Map..." << vcl_endl;
            pcm_chg_map = pcm_world.change_map();
            output_name = pcm_results_dir + '/' + *filename_itr 
                + ".png";
            vil_save(pcm_chg_map,output_name.c_str());
        }
        


    }// end parked car under maks test block
    #endif //PCM_TEST
}

TESTMAIN( test_smw_world )
