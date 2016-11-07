#include <vcl_iostream.h>

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


// Get average intensity in a region.
bool average_intensity( float &intensity,
              float &max_intensity,
            bool is_hdr,
                        vcl_string file,            
            const vcl_vector< vgl_point_2d<int> >& region_bounds,
            vcl_string debug_file = "NONE" )
{
  vil_image_view<float> img(640,480);
  bool is_missing = true;

  if(is_hdr)
    img = vil_load(file.c_str() );
  else{
    vil_image_view<vxl_byte> src(vil_load(file.c_str()) );
    src = vil_crop(src, 0,640,3840,480);
    //vil_save(src, "extracted_image.jpg");
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
  float max_int = 0;
  unsigned max_int_i;
  unsigned max_int_j;
  for( int i = 0; i < img.ni(); i++ ){
    for( int j = 0; j < img.nj(); j++ ){
      if( pg.contains(i,j) ){
        num_samples++;
        sum+=img(i,j);
        if (img(i,j) > max_int)
        {
          max_int = img(i,j);
            max_int_i = i;
          max_int_j = j;
        }
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
    debug_img(max_int_i,max_int_j)= 255;
    vil_save( debug_img, debug_file.c_str() );
  }


  if( num_samples == 0 ) intensity= 0;
  intensity= sum/num_samples;
  max_intensity = max_int;
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

//void init_folders(vcl_vector<vcl_string> &in_files)
//{
//  
//  in_files.push_back("e:\\dome_images\\00_calibration");
//  in_files.push_back("e:\\dome_images\\01_shingle");
//  in_files.push_back("e:\\dome_images\\02_car");
//  in_files.push_back("e:\\dome_images\\03_tile");
//  in_files.push_back("e:\\dome_images\\04_aluminum");
//  in_files.push_back("e:\\dome_images\\05_fiberglass");
//  in_files.push_back("e:\\dome_images\\06_glass");
//  in_files.push_back("e:\\dome_images\\07_cement");
//  in_files.push_back("e:\\dome_images\\08_macadam");
//  in_files.push_back("e:\\dome_images\\09_brick");
//  in_files.push_back("e:\\dome_images\\10_wood");
//  in_files.push_back("e:\\dome_images\\11_gravel");
//  in_files.push_back("e:\\dome_images\\12_grass");
//  in_files.push_back("e:\\dome_images\\13_water");
//}
//
//
//void init_folders(vcl_vector<vcl_string> &out_files)
//{
//  
//  in_files.push_back("e:\\dome_images\\00_calibration");
//  in_files.push_back("e:\\dome_images\\01_shingle");
//  in_files.push_back("e:\\dome_images\\02_car");
//  in_files.push_back("e:\\dome_images\\03_tile");
//  in_files.push_back("e:\\dome_images\\04_aluminum");
//  in_files.push_back("e:\\dome_images\\05_fiberglass");
//  in_files.push_back("e:\\dome_images\\06_glass");
//  in_files.push_back("e:\\dome_images\\07_cement");
//  in_files.push_back("e:\\dome_images\\08_macadam");
//  in_files.push_back("e:\\dome_images\\09_brick");
//  in_files.push_back("e:\\dome_images\\10_wood");
//  in_files.push_back("e:\\dome_images\\11_gravel");
//  in_files.push_back("e:\\dome_images\\12_grass");
//  in_files.push_back("e:\\dome_images\\13_water");
//
//}
int main( int argc, char* argv[] )
{  
  
  bool debug = false;
  bool calibration = true;
  bool is_hdr = true;
  
  //for(vcl_vector<vcl_string>.iterator it; 
  vcl_string debug_dir = "c:\\debug";
  vcl_string main_dir = "E:\\dome_images\\00_calibration";
  vcl_string out_file;
  
  if(is_hdr)
    out_file = "e:\\dome_images\\calibration_sphere.txt";
  else
    out_file = "E:\\dome_images\\04_aluminum_png.txt";

  vcl_vector< float > region_times;
  vcl_vector< vcl_vector< vcl_vector< vgl_point_2d<int> > > > regions;
  for( int i = 0; i < 14; i++ )
    regions.push_back( vcl_vector< vcl_vector< vgl_point_2d<int> > >() );


  region_times.push_back( 7.0 );
  get_regions( vcl_string( "e:\\dome_images\\cal_sphere_regions\\7am.txt" ),
    regions[0] );
  region_times.push_back( 7.75 );
  get_regions( vcl_string( "e:\\dome_images\\cal_sphere_regions\\745am.txt" ),
    regions[1] );
  region_times.push_back( 8.5 );
  get_regions( vcl_string( "e:\\dome_images\\cal_sphere_regions\\830am.txt" ),
    regions[2] );
  region_times.push_back( 9.2 );
  get_regions( vcl_string( "e:\\dome_images\\cal_sphere_regions\\930.txt" ),
    regions[3] );
  region_times.push_back( 10 );
  get_regions( vcl_string( "e:\\dome_images\\cal_sphere_regions\\10.txt" ),
    regions[4] );
  region_times.push_back( 11 );
  get_regions( vcl_string( "e:\\dome_images\\cal_sphere_regions\\11.txt" ),
    regions[5] );
  region_times.push_back( 12 );
  get_regions( vcl_string( "e:\\dome_images\\cal_sphere_regions\\12.txt" ),
    regions[6] );
  region_times.push_back( 12.9 );
  get_regions( vcl_string( "e:\\dome_images\\cal_sphere_regions\\1.txt" ),
    regions[7] );
  region_times.push_back( 13.9 );
  get_regions( vcl_string( "e:\\dome_images\\cal_sphere_regions\\2.txt" ),
    regions[8] );
  region_times.push_back( 14.9 );
  get_regions( vcl_string( "e:\\dome_images\\cal_sphere_regions\\3.txt" ),
    regions[9] );
  region_times.push_back( 16 );
  get_regions( vcl_string( "e:\\dome_images\\cal_sphere_regions\\4.txt" ),
    regions[10] );
  region_times.push_back( 17 );
  get_regions( vcl_string( "e:\\dome_images\\cal_sphere_regions\\5.txt" ),
    regions[11] );
  region_times.push_back( 18 );
  get_regions( vcl_string( "e:\\dome_images\\cal_sphere_regions\\6.txt" ),
    regions[12] );
  region_times.push_back( 18.45 );
  get_regions( vcl_string( "e:\\dome_images\\cal_sphere_regions\\7.txt" ),
    regions[13] );

  /*
  vcl_vector< vcl_vector< vgl_point_2d<int> > > day1_regions, day2_regions;
  // DAY 1

  // Camera 1
  vcl_vector< vgl_point_2d<int> > new_region;
  new_region.push_back( vgl_point_2d<int>( 321, 317 ) );
  new_region.push_back( vgl_point_2d<int>( 275, 330 ) );
  new_region.push_back( vgl_point_2d<int>( 266, 291 ) );
  new_region.push_back( vgl_point_2d<int>( 307, 279 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 2
  new_region.push_back( vgl_point_2d<int>( 311, 311 ) );
  new_region.push_back( vgl_point_2d<int>( 267, 316 ) );
  new_region.push_back( vgl_point_2d<int>( 267, 292 ) );
  new_region.push_back( vgl_point_2d<int>( 312, 289 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 3
  new_region.push_back( vgl_point_2d<int>( 360, 347 ) );
  new_region.push_back( vgl_point_2d<int>( 313, 349 ) );
  new_region.push_back( vgl_point_2d<int>( 317, 317 ) );
  new_region.push_back( vgl_point_2d<int>( 361, 314 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 4
  new_region.push_back( vgl_point_2d<int>( 314, 306 ) );
  new_region.push_back( vgl_point_2d<int>( 275, 299 ) );
  new_region.push_back( vgl_point_2d<int>( 292, 273 ) );
  new_region.push_back( vgl_point_2d<int>( 331, 279 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 5
  new_region.push_back( vgl_point_2d<int>( 329, 315 ) );
  new_region.push_back( vgl_point_2d<int>( 295, 292 ) );
  new_region.push_back( vgl_point_2d<int>( 318, 261 ) );
  new_region.push_back( vgl_point_2d<int>( 353, 286 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 6
  new_region.push_back( vgl_point_2d<int>( 294, 239 ) );
  new_region.push_back( vgl_point_2d<int>( 273, 227 ) );
  new_region.push_back( vgl_point_2d<int>( 301, 207 ) );
  new_region.push_back( vgl_point_2d<int>( 327, 223 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 7
  new_region.push_back( vgl_point_2d<int>( 336, 319 ) );
  new_region.push_back( vgl_point_2d<int>( 298, 309 ) );
  new_region.push_back( vgl_point_2d<int>( 318, 277 ) );
  new_region.push_back( vgl_point_2d<int>( 354, 286 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 8
  new_region.push_back( vgl_point_2d<int>( 225, 189 ) );
  new_region.push_back( vgl_point_2d<int>( 233, 147 ) );
  new_region.push_back( vgl_point_2d<int>( 273, 156 ) );
  new_region.push_back( vgl_point_2d<int>( 264, 197 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 9
  new_region.push_back( vgl_point_2d<int>( 313, 284 ) );
  new_region.push_back( vgl_point_2d<int>( 321, 261 ) );
  new_region.push_back( vgl_point_2d<int>( 355, 263 ) );
  new_region.push_back( vgl_point_2d<int>( 349, 289 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 10
  new_region.push_back( vgl_point_2d<int>( 207, 230 ) );
  new_region.push_back( vgl_point_2d<int>( 216, 193 ) );
  new_region.push_back( vgl_point_2d<int>( 255, 202 ) );
  new_region.push_back( vgl_point_2d<int>( 247, 240 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 11
  new_region.push_back( vgl_point_2d<int>( 260, 277 ) );
  new_region.push_back( vgl_point_2d<int>( 269, 252 ) );
  new_region.push_back( vgl_point_2d<int>( 310, 260 ) );
  new_region.push_back( vgl_point_2d<int>( 303, 285 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 12
  new_region.push_back( vgl_point_2d<int>( 213, 279 ) );
  new_region.push_back( vgl_point_2d<int>( 227, 252 ) );
  new_region.push_back( vgl_point_2d<int>( 263, 269 ) );
  new_region.push_back( vgl_point_2d<int>( 250, 295 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 13
  new_region.push_back( vgl_point_2d<int>( 286, 199 ) );
  new_region.push_back( vgl_point_2d<int>( 327, 189 ) );
  new_region.push_back( vgl_point_2d<int>( 341, 213 ) );
  new_region.push_back( vgl_point_2d<int>( 302, 222 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 14
  new_region.push_back( vgl_point_2d<int>( 277, 290 ) );
  new_region.push_back( vgl_point_2d<int>( 301, 270 ) );
  new_region.push_back( vgl_point_2d<int>( 331, 284 ) );
  new_region.push_back( vgl_point_2d<int>( 308, 299 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 15
  new_region.push_back( vgl_point_2d<int>( 247, 253 ) );
  new_region.push_back( vgl_point_2d<int>( 282, 240 ) );
  new_region.push_back( vgl_point_2d<int>( 305, 265 ) );
  new_region.push_back( vgl_point_2d<int>( 270, 279 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // Camera 16
  new_region.push_back( vgl_point_2d<int>( 374, 233 ) );
  new_region.push_back( vgl_point_2d<int>( 413, 220 ) );
  new_region.push_back( vgl_point_2d<int>( 426, 252 ) );
  new_region.push_back( vgl_point_2d<int>( 387, 263 ) );
  day1_regions.push_back( new_region ); new_region.clear();

  // DAY 2

  // Camera 1
  new_region.push_back( vgl_point_2d<int>( 322, 335 ) );
  new_region.push_back( vgl_point_2d<int>( 275, 348 ) );
  new_region.push_back( vgl_point_2d<int>( 262, 307 ) );
  new_region.push_back( vgl_point_2d<int>( 306, 294 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 2
  new_region.push_back( vgl_point_2d<int>( 299, 293 ) );
  new_region.push_back( vgl_point_2d<int>( 253, 280 ) );
  new_region.push_back( vgl_point_2d<int>( 262, 257 ) );
  new_region.push_back( vgl_point_2d<int>( 307, 271 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 3
  new_region.push_back( vgl_point_2d<int>( 379, 346 ) );
  new_region.push_back( vgl_point_2d<int>( 330, 347 ) );
  new_region.push_back( vgl_point_2d<int>( 333, 314 ) );
  new_region.push_back( vgl_point_2d<int>( 381, 310 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 4
  new_region.push_back( vgl_point_2d<int>( 307, 292 ) );
  new_region.push_back( vgl_point_2d<int>( 266, 282 ) );
  new_region.push_back( vgl_point_2d<int>( 287, 255 ) );
  new_region.push_back( vgl_point_2d<int>( 327, 267 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 5
  new_region.push_back( vgl_point_2d<int>( 339, 312 ) );
  new_region.push_back( vgl_point_2d<int>( 307, 282 ) );
  new_region.push_back( vgl_point_2d<int>( 338, 256 ) );
  new_region.push_back( vgl_point_2d<int>( 371, 285 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 6
  new_region.push_back( vgl_point_2d<int>( 322, 243 ) );
  new_region.push_back( vgl_point_2d<int>( 306, 223 ) );
  new_region.push_back( vgl_point_2d<int>( 341, 215 ) );
  new_region.push_back( vgl_point_2d<int>( 359, 238 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 7
  new_region.push_back( vgl_point_2d<int>( 353, 307 ) );
  new_region.push_back( vgl_point_2d<int>( 315, 296 ) );
  new_region.push_back( vgl_point_2d<int>( 330, 265 ) );
  new_region.push_back( vgl_point_2d<int>( 372, 272 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 8
  new_region.push_back( vgl_point_2d<int>( 236, 186 ) );
  new_region.push_back( vgl_point_2d<int>( 242, 145 ) );
  new_region.push_back( vgl_point_2d<int>( 284, 153 ) );
  new_region.push_back( vgl_point_2d<int>( 278, 196 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 9
  new_region.push_back( vgl_point_2d<int>( 303, 286 ) );
  new_region.push_back( vgl_point_2d<int>( 310, 271 ) );
  new_region.push_back( vgl_point_2d<int>( 345, 284 ) );
  new_region.push_back( vgl_point_2d<int>( 336, 302 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 10
  new_region.push_back( vgl_point_2d<int>( 202, 236 ) );
  new_region.push_back( vgl_point_2d<int>( 209, 196 ) );
  new_region.push_back( vgl_point_2d<int>( 250, 202 ) );
  new_region.push_back( vgl_point_2d<int>( 246, 242 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 11
  new_region.push_back( vgl_point_2d<int>( 343, 299 ) );
  new_region.push_back( vgl_point_2d<int>( 361, 275 ) );
  new_region.push_back( vgl_point_2d<int>( 393, 304 ) );
  new_region.push_back( vgl_point_2d<int>( 373, 326 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 12
  new_region.push_back( vgl_point_2d<int>( 302, 287 ) );
  new_region.push_back( vgl_point_2d<int>( 318, 259 ) );
  new_region.push_back( vgl_point_2d<int>( 352, 277 ) );
  new_region.push_back( vgl_point_2d<int>( 336, 303 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 13
  new_region.push_back( vgl_point_2d<int>( 304, 205 ) );
  new_region.push_back( vgl_point_2d<int>( 343, 343 ) );
  new_region.push_back( vgl_point_2d<int>( 356, 356 ) );
  new_region.push_back( vgl_point_2d<int>( 317, 317 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 14
  new_region.push_back( vgl_point_2d<int>( 282, 304 ) );
  new_region.push_back( vgl_point_2d<int>( 305, 281 ) );
  new_region.push_back( vgl_point_2d<int>( 332, 287 ) );
  new_region.push_back( vgl_point_2d<int>( 314, 311 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 15
  new_region.push_back( vgl_point_2d<int>( 263, 272 ) );
  new_region.push_back( vgl_point_2d<int>( 294, 258 ) );
  new_region.push_back( vgl_point_2d<int>( 315, 279 ) );
  new_region.push_back( vgl_point_2d<int>( 284, 295 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  // Camera 16
  new_region.push_back( vgl_point_2d<int>( 388, 200 ) );
  new_region.push_back( vgl_point_2d<int>( 352, 214 ) );
  new_region.push_back( vgl_point_2d<int>( 369, 242 ) );
  new_region.push_back( vgl_point_2d<int>( 404, 228 ) );
  day2_regions.push_back( new_region ); new_region.clear();

  */
  vcl_ofstream ofs( out_file.c_str() );
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



    vcl_cerr << current_file << '\n';

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
    int time_index = 0;
    for( int i = 0; i < regions.size(); i++ ){
      if( region_times[i] > time ) continue;
      time_index = i;
    }

    // Get the mean intensity.

    float intensity;
    float max_intensity;

    //if the image was valid, write avg intensity. Otherwise, skip
    /*if(average_intensity(intensity, is_hdr, current_file, regions[time_index][camera-1],
      debug_file_string.str() ))*/
    if (debug)
    {
      if(average_intensity(intensity, max_intensity, is_hdr, current_file, regions[time_index][camera-1],
        debug_file_string.str()))
      ofs << time << ' ' << rotation << ' ' << camera << ' ' << intensity << max_intensity<<'\n';
    }
    else
    {
      if(average_intensity(intensity, max_intensity, is_hdr, current_file, regions[time_index][camera-1]))
      ofs << time << ' ' << rotation << ' ' << camera << ' ' << intensity << ' ' << max_intensity<<'\n';
    }
    //if( day == 1 ) intensity = average_intensity( img, day1_regions[camera-1] );
    //if( day == 2 ) intensity = average_intensity( img, day2_regions[camera-1] );

    
  }
};



