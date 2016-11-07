//file to scale a probability map sequence from .tiff to a .jpg
//must scale values to 0-255 range.

#include <vcl_iostream.h> //io streams
#include <vcl_fstream.h> //file streams
#include <vcl_string.h> //std::string
#include <vcl_map.h> //std:map
#include <vcl_utility.h> //vcl_pair::make_pair

#include <vil/vil_load.h> //load images
#include <vil/vil_save.h> //save images
#include <vil/vil_math.h> //image math functions

#include <vul/vul_file_iterator.h> //file iterator to parse files
#include <vul/vul_file.h> //strip file extensions from directory

int main()
{ 
    //local constants
    vcl_string img_dir = "/media/DATAPART1/BrandonDataFolder/ChangeDetection/results/probMap",
               out_dir = "/media/DATAPART1/BrandonDataFolder/ChangeDetection/results/probMap/scaledProbMap";
    float min = 0.0f, max = 0.0f;
    
    //img.first = filename , img.second = vil_image_view_sptr
    vcl_map<vcl_string,vil_image_view<float> > img_map;

    //load images into map
    vcl_string img_names = img_dir + "*.tiff";
    for(vul_file_iterator fn = img_names; fn; ++fn)
    {
        vcl_string filename = vul_file::strip_directory(fn());
        filename = vul_file::strip_extension(filename);
        vcl_cout << "Loading Image: " << filename << "\n";
        img_map.insert(vcl_make_pair(filename,vil_load(fn())));
    }

    
    //try the first image to see what happens
    vcl_map<vcl_string, vil_image_view<float> >::iterator map_it = img_map.begin();
    vcl_map<vcl_string, vil_image_view<float> >::iterator map_end = img_map.end();
    
    
    
    return 0;
}

