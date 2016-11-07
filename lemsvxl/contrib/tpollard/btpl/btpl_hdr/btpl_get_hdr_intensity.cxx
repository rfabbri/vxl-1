// This files gets all the inenesities within a region and prints out
// the values to a text file
// @author : Isa Restrepo


#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cstdio.h>
#include <vcl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_polygon.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_crop.h>
#include <vil/vil_convert.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>


void get_max_int(vcl_string ofile,
                 vcl_string file,  
                 const vcl_vector< vgl_point_2d<int> >& region_bounds,
                 vcl_string debug_file = "NONE" )
{


  vcl_ofstream ofs( ofile.c_str() );
  vil_image_view<float> img = vil_load(file.c_str() ) ;

  float x[4], y[4];
  for( int i = 0; i < 4; i++ ){
    x[i] = (float)region_bounds[i].x();
    y[i] = (float)region_bounds[i].y();
  }
  vgl_polygon<float> pg( x, y, 4 );


  float max_int = 0;
  unsigned max_int_i = 0 ;
  unsigned max_int_j = 0;
  for( int i = 0; i < img.ni(); i++ ){
    for( int j = 0; j < img.nj(); j++ ){
      if( pg.contains(i,j) ){
        if (img(i,j) > max_int)
        {
          max_int = img(i,j);
          max_int_i = i;
          max_int_j = j;
        }
      }
    }
  }

  ofs << max_int<<"\n";

    if( debug_file != "NONE" ){
      vil_image_view<vxl_byte> debug_img( img.ni(), img.nj() );
      for( int i = 0; i < img.ni(); i++ ){
        for( int j = 0; j < img.nj(); j++ ){
          int this_pixel = floor( 255*img(i,j) );
          if( this_pixel < 0 ) this_pixel = 0;
          if( this_pixel > 255 ) this_pixel = 255;
          else debug_img(i,j) = this_pixel;
        }
      }
      debug_img(max_int_i,max_int_j)= 0;
      vil_save( debug_img, debug_file.c_str() );
    }

}


// Get all intensities in a region.
bool get_intensity(bool is_hdr, 
                   vcl_string ofile,
                   vcl_string file,             
                   const vcl_vector< vgl_point_2d<int> >& region_bounds,
                   vcl_string debug_file = "NONE" )
{

  vcl_ofstream ofs( ofile.c_str() );
  vil_image_view<float> img(640,480);
  bool is_missing = true;


  if(is_hdr)
    img = vil_load(file.c_str() );
  else{
    vil_image_view<vxl_byte> src(vil_load(file.c_str()) );
    src = vil_crop(src, 0,640,2400,480);
    //check if the image was missing (all white)
    for( int i = 0; i < 640; i++ ){
      for( int j = 0; j < 480; j++ ){
        if( src(i,j) != 255 ){
          is_missing = false;
        }
        img(i,j)=src(i,j)/255.0;
      }
    }
    if(is_missing)
      return false;
    //vil_convert_cast(src, img);
  }


  float x[4], y[4];
  for( int i = 0; i < 4; i++ ){
    x[i] = (float)region_bounds[i].x();
    y[i] = (float)region_bounds[i].y();
  }
  vgl_polygon<float> pg( x, y, 4 );

  int num_samples = 0;
  float sum = 0;
  for( int i = 0; i < img.ni(); i++ ){
    for( int j = 0; j < img.nj(); j++ ){
      if( pg.contains(i,j) ){
        ofs << i << ' ' << j << ' ' <<  img(i,j)<<"\n";

      }
    }
  }

  if( debug_file != "NONE" ){
    vil_image_view<vxl_byte> debug_img( img.ni(), img.nj() );
    for( int i = 0; i < img.ni(); i++ ){
      for( int j = 0; j < img.nj(); j++ ){
        if( pg.contains(i,j) ){ debug_img(i,j) = 0; continue; }
        int this_pixel = floor( 255*img(i,j) );
        if( this_pixel < 0 ) this_pixel = 0;
        if( this_pixel > 255 ) this_pixel = 255;
        else debug_img(i,j) = this_pixel;
      }
    }
   vil_save( debug_img, debug_file.c_str() );
  }


  return true;

};


// Get regions from a file.
void get_regions(
                 vcl_string file,
                 vcl_vector< vcl_vector< vgl_point_2d<int> > >& regions )
{
  regions.clear();
  vcl_ifstream ifs( file.c_str() );
  for( int c = 0; c < 16; c++ ){
    vcl_vector< vgl_point_2d<int> > new_region;
    for( int p = 0; p < 4; p++ ){
      int x, y;
      ifs >> x; ifs >> y;
      new_region.push_back( vgl_point_2d<int>( x, y ) );
    }
    regions.push_back( new_region );
  }
};

int main( int argc, char* argv[] )
{  

  bool debug = true;
  bool calibration = false;
  bool is_hdr = true;
  bool sphere = false;

  //for(vcl_vector<vcl_string>.iterator it; 
  vcl_string debug_dir = "c:\\debug";

  vcl_vector<vcl_string> img_dirs;
  img_dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_207");
  img_dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_208");
  img_dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_209");
  img_dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_109");
  img_dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_110");
  img_dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_111");
  img_dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_112");
  img_dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_113");
  img_dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_114");
  img_dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_115");
  img_dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_116");
  img_dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_117");
  img_dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_118");
  img_dirs.push_back("E:\\dome_images\\08_macadam\\cropped_images\\png_119");


  
  vcl_vector< float > region_times;
  vcl_vector< vcl_vector< vcl_vector< vgl_point_2d<int> > > > regions;
  for( int i = 0; i < 14; i++ )
    regions.push_back( vcl_vector< vcl_vector< vgl_point_2d<int> > >() );

if(!sphere)
{
  region_times.push_back( 7.35 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\macadam\\7am.txt" ),
    regions[0] );
  region_times.push_back( 8.05 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\macadam\\745am.txt" ),
    regions[1] );
  region_times.push_back( 8.75);
  get_regions( vcl_string( "E:\\dome_images\\regions\\macadam\\830am.txt" ),
    regions[2] );
  region_times.push_back( 9.55 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\macadam\\930.txt" ),
    regions[3] );
  region_times.push_back( 10.30 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\macadam\\10.txt" ),
    regions[4] );
  region_times.push_back( 11.35 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\macadam\\11.txt" ),
    regions[5] );
  region_times.push_back( 12.45 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\macadam\\12.txt" ),
    regions[6] );
  region_times.push_back( 13.35 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\macadam\\1.txt" ),
    regions[7] );
  region_times.push_back( 14.25 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\macadam\\2.txt" ),
    regions[8] );
  region_times.push_back( 15.25 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\macadam\\3.txt" ),
    regions[9] );
  region_times.push_back( 16.40);
  get_regions( vcl_string( "E:\\dome_images\\regions\\macadam\\4.txt" ),
    regions[10] );
  region_times.push_back( 17.40 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\macadam\\5.txt" ),
    regions[11] );
  region_times.push_back( 18.35 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\macadam\\6.txt" ),
    regions[12] );
  region_times.push_back( 19 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\macadam\\7.txt" ),
    regions[13] );
}

if(sphere)
{
    region_times.push_back( 7.35 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\lam_cropped\\sphere.txt" ),
    regions[0] );
  region_times.push_back( 8.05 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\lam_cropped\\sphere.txt" ),
    regions[1] );
  region_times.push_back( 8.75);
  get_regions( vcl_string( "E:\\dome_images\\regions\\lam_cropped\\sphere.txt" ),
    regions[2] );
  region_times.push_back( 9.55 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\lam_cropped\\sphere.txt" ),
    regions[3] );
  region_times.push_back( 10.30 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\lam_cropped\\sphere.txt" ),
    regions[4] );
  region_times.push_back( 11.35 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\lam_cropped\\sphere.txt" ),
    regions[5] );
  region_times.push_back( 12.45 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\lam_cropped\\sphere.txt" ),
    regions[6] );
  region_times.push_back( 13.35 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\lam_cropped\\sphere.txt" ),
    regions[7] );
  region_times.push_back( 14.25 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\lam_cropped\\sphere.txt" ),
    regions[8] );
  region_times.push_back( 15.25 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\lam_cropped\\sphere.txt" ),
    regions[9] );
  region_times.push_back( 16.40);
  get_regions( vcl_string( "E:\\dome_images\\regions\\lam_cropped\\sphere.txt" ),
    regions[10] );
  region_times.push_back( 17.40 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\lam_cropped\\sphere.txt" ),
    regions[11] );
  region_times.push_back( 18.35 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\lam_cropped\\sphere.txt" ),
    regions[12] );
  region_times.push_back( 19 );
  get_regions( vcl_string( "E:\\dome_images\\regions\\lam_cropped\\sphere.txt" ),
    regions[13] );

}


// for(unsigned curr_index = 11; curr_index< img_dirs.size(); curr_index++)
 for(unsigned curr_index = 11; curr_index< 12; curr_index++)
  {

    vcl_string main_dir = img_dirs[curr_index];

    for( vul_file_iterator f= (main_dir+"\\*").c_str(); f; ++f ){

      // Extract information from the filename.
      vcl_string current_file( f() );
      int cf_ext_index = current_file.size()-4;
      vcl_string cf_ext;
      cf_ext+=current_file[cf_ext_index]; cf_ext+=current_file[cf_ext_index+1];
      cf_ext+=current_file[cf_ext_index+2]; cf_ext+=current_file[cf_ext_index+3];

      switch(is_hdr){
    case true:
      if( cf_ext != ".tif" ) continue;
      break;
    case false: 
      if ( cf_ext != ".png" ) continue;
      break;
    default:
      vcl_cerr<<"unsopported extension" << vcl_endl;

      }


      int last_slash_loc = 0;
      for( int i = 0; i < current_file.size(); i++ )
        if( current_file[i] == '\\' ) last_slash_loc = i;

      vcl_stringstream camera_string, rotation_string, hour_string, minute_string, 
        day_string, debug_file_string;
      day_string << current_file[last_slash_loc+1];
      hour_string << current_file[last_slash_loc+2] << current_file[last_slash_loc+3];
      minute_string << current_file[last_slash_loc+4] << current_file[last_slash_loc+5];
      if( calibration == false ){
        rotation_string << current_file[last_slash_loc+7] << current_file[last_slash_loc+8] <<
          current_file[last_slash_loc+9];
        camera_string << current_file[last_slash_loc+11] << current_file[last_slash_loc+12];
      }
      else {
        rotation_string << current_file[last_slash_loc+7];
        camera_string << current_file[last_slash_loc+9] << current_file[last_slash_loc+10];
      }
      debug_file_string << debug_dir << "\\";
      for( int i = last_slash_loc+1; i < cf_ext_index; i++ )
        debug_file_string << current_file[i];
      debug_file_string << ".jpg";

      int day = atoi( day_string.str().c_str() );
      float time = atoi( hour_string.str().c_str() ) + atoi( minute_string.str().c_str() )/60.0;
      int rotation = atoi( rotation_string.str().c_str() );
      int camera = atoi( camera_string.str().c_str() );

      // Determine which regions to use.
      
        
      //int time_index = 0;
      //for( int i = 0; i < regions.size(); i++ ){
      //  if( region_times[i] > time ) continue;
      //  //if(( 12 < time)||( 11 > time) )continue;
      //  time_index = i;
      //}

      if(!sphere){
        if(( rotation == 3)||(rotation == 4)) continue;
      }

      if(sphere){
        if(( rotation == 1)||(rotation == 2)) continue;
      }

      // Print the file with intensities within the region.

      vcl_stringstream out_file;
      out_file.clear();
      out_file << "E:\\dome_images\\all_intensities\\macadam\\";
      out_file<<region_times[curr_index]<<'\\'<<rotation<<'_'<<camera<<".txt";

      vcl_string  ofile;
      ofile = out_file.str();
      vcl_cout<< "Writing" <<ofile<<"\n";
     
      vcl_vector<float> intensity;
      //if working with sphere, we can find the max int, check it lies inside sphere and
      //use it as maximum radiance
      if (sphere)
        get_max_int(ofile, current_file, regions[curr_index][camera-1],debug_file_string.str());
      else if (debug)
        get_intensity(is_hdr, ofile, current_file, regions[curr_index][camera-1],
        debug_file_string.str());
      else
        get_intensity(is_hdr, ofile, current_file, regions[curr_index][camera-1]);

    }
  }

};
