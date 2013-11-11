#include <vcl_iostream.h>

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>

// Code to sort raw image directories by material with name format:
//   time_rotation_camera
// where time is dhhmm


int main( int argc, char* argv[] )
{  
  // Set these:
  vcl_string root_img_dir = "e:\\brdf";
  vcl_string output_dir = "e:\\dome_images";
  int computer_index = 3; // Doug = 0, Isabel = 1, Lab = 2, Thom = 3

  vcl_vector< vcl_string > day2_times;
  day2_times.push_back( "745am" );
  day2_times.push_back( "830am" );
  day2_times.push_back( "7am" );

  int last_slash, last_period, last_underscore;

  // Get the current day + material.
  for( vul_file_iterator f1= (root_img_dir+"\\*").c_str(); f1; ++f1 ){

    vcl_string current_dir1( f1() );
    last_slash = last_underscore = 0;
    for( int i = 0; i < current_dir1.size(); i++ ){
      if( current_dir1[i] == '_' ) last_underscore = i;
      if( current_dir1[i] == '\\' ) last_slash = i;
    }
    if( last_underscore <= last_slash ) continue;

    vcl_string runtime_string;
    for( int i = last_slash+1; i < last_underscore; i++ )
      runtime_string += current_dir1[i];
    int day = 1;
    for( int i = 0; i < day2_times.size(); i++ )
      if( runtime_string == day2_times[i] ) 
        day = 2;

    vcl_string material_string;
    for( int i = last_underscore+1; i < current_dir1.size(); i++ )
      material_string += current_dir1[i];
    int material = 0;
    if( material_string != "L" ) 
      material = atoi( material_string.c_str() );

    // Get the rotation angle.
    for( vul_file_iterator f2= (current_dir1 + "\\*").c_str(); f2; ++f2 ){
          
      vcl_string current_dir2( f2() );
      last_slash = last_period = 0;
      for( int i = 0; i < current_dir2.size(); i++ ){
        if( current_dir2[i] == '.' ) last_period = i;
        if( current_dir2[i] == '\\' ) last_slash = i;
      }
      if( last_period > last_slash ) continue;

      vcl_string rotation_string;
      for( int i = last_slash+1; i < current_dir2.size(); i++ )
        rotation_string += current_dir2[i];
      int rotation = atoi( rotation_string.c_str() );

      // Get the camera.
      for( vul_file_iterator f3= (current_dir2 + "\\*").c_str(); f3; ++f3 ){

        vcl_string current_dir3( f3() );
        last_slash = last_period = 0;
        for( int i = 0; i < current_dir3.size(); i++ ){
          if( current_dir3[i] == '.' ) last_period = i;
          if( current_dir3[i] == '\\' ) last_slash = i;
        }
        if( last_period > last_slash ) continue;

        vcl_string camera_string;
        for( int i = last_slash+4; i < current_dir3.size(); i++ )
          camera_string += current_dir3[i];
        int relative_camera = atoi( camera_string.c_str() );
        int camera = (4*computer_index) + relative_camera + 1;

        // Get time + shutter speed.
        vcl_string time;
        vcl_vector< vcl_string > img_names;
        vcl_vector<int> shutter_indices;
        for( vul_file_iterator f4= (current_dir3 + "\\*").c_str(); f4; ++f4 ){

          vcl_string current_file( f4() );
          int cf_ext_index = current_file.size()-4;
          vcl_string cf_ext;
          cf_ext+=current_file[cf_ext_index]; cf_ext+=current_file[cf_ext_index+1];
          cf_ext+=current_file[cf_ext_index+2]; cf_ext+=current_file[cf_ext_index+3];
          if( cf_ext != ".bmp" ) continue;

          int fourth_us, fifth_us, sixth_us, seventh_us;
          int us_counter = 0;
          for( int i = current_file.size()-1; i > 0; i-- ){
            if( current_file[i] != '_' ) continue;
            if( us_counter == 0 ) seventh_us = i;
            if( us_counter == 1 ) sixth_us = i;
            if( us_counter == 2 ) fifth_us = i;
            if( us_counter == 3 ) fourth_us = i;
            us_counter++;
          }

          time.clear();
          if( fifth_us-fourth_us <= 2 ) time+= '0';
          for( int i = fourth_us+1; i < fifth_us; i++ )
            time += current_file[i];
          if( sixth_us-fifth_us <= 2 ) time+= '0';
          for( int i = fifth_us+1; i < sixth_us; i++ )
            time += current_file[i];

          vcl_string shutter_string;
          for( int i = seventh_us+1; i < current_file.size(); i++ ){
            if( current_file[i] == '.' ) break;
            shutter_string+= current_file[i];
          }
          int shutter_number = atoi( shutter_string.c_str() );
          int shutter_index;
          if( shutter_number == 1 ) shutter_index = 0;
          else if( shutter_number == 2 ) shutter_index = 1;
          else if( shutter_number == 4 ) shutter_index = 2;
          else if( shutter_number == 8 ) shutter_index = 3;
          else if( shutter_number == 16 ) shutter_index = 4;
          else if( shutter_number == 32 ) shutter_index = 5;
          else if( shutter_number == 64 ) shutter_index = 6;
          else if( shutter_number == 128 ) shutter_index = 7;
          else if( shutter_number == 192 ) shutter_index = 8;
          else if( shutter_number == 256 ) shutter_index = 9;
          else if( shutter_number == 384 ) shutter_index = 10;
          else if( shutter_number == 0 ) shutter_index = 11;
          else vcl_cerr << "ERROR: Unknown shutter number: " << shutter_number << '\n';
                    
          if( shutter_index == 0 ) continue; // Simplest solution.
          img_names.push_back( current_file );
          shutter_indices.push_back( shutter_index );
        }

        // Now form the output image name.
        vcl_stringstream png_name;
        png_name << output_dir << "\\";
        if( material == 0 ) png_name << "00_calibration";
        else if( material == 1 ) png_name << "01_shingle";
        else if( material == 2 ) png_name << "02_car";
        else if( material == 3 ) png_name << "03_tile";
        else if( material == 4 ) png_name << "04_aluminum";
        else if( material == 5 ) png_name << "05_fiberglass";
        else if( material == 6 ) png_name << "06_glass";
        else if( material == 7 ) png_name << "07_cement";
        else if( material == 8 ) png_name << "08_macadam";
        else if( material == 9 ) png_name << "09_brick";
        else if( material == 10 ) png_name << "10_wood";
        else if( material == 11 ) png_name << "11_gravel";
        else if( material == 12 ) png_name << "12_grass";
        else if( material == 13 ) png_name << "13_water";
        else if( material == 14 ) png_name << "14_leaves";
        else if( material == 15 ) png_name << "15_tile_shade";
        else if( material == 16 ) png_name << "16_aluminum_shade";
        else vcl_cerr << "ERROR: Unknown material: " << material << '\n';
        png_name << "\\" << day << time << '_';
        if( rotation == 0 ) png_name << "00";
        else if( rotation == 90 ) png_name << "0";
        png_name << rotation << '_';
        if( camera < 10 ) png_name << '0';
        png_name << camera << ".png";
        vcl_cerr << png_name.str() << '\n';

        // Convert to png.
        vcl_vector< vil_image_view<vxl_byte> > imgs;
        for( int i = 0; i < img_names.size(); i++ )
          imgs.push_back( vil_load( img_names[i].c_str() ) );
        if( img_names.size() == 0 ) continue;
        int img_ni = imgs[0].ni(), img_nj = imgs[0].nj();
        vil_image_view<vxl_byte> png_img( img_ni, 12*img_nj, 1 );
        for( int i = 0; i < png_img.ni(); i++ )
          for( int j = 0; j < png_img.nj(); j++ )
            png_img(i,j) = 255;
        for( int img = 0; img < imgs.size(); img++ ){
          int shutter_index = shutter_indices[img];
          for( int i = 0; i < img_ni; i++ )
            for( int j = 0; j < img_nj; j++ )
              png_img( i, shutter_index*img_nj + j ) = imgs[img](i,j);
        }
        vil_save( png_img, png_name.str().c_str() ); //*/
      }

    }
  }

};

