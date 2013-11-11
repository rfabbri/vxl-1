#ifndef btpl_satellite_tools_cxx_
#define btpl_satellite_tools_cxx_

#include <vcl_iostream.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_cstdio.h>
#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_homg_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_box_3d.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_svd.h>
#include <vil/vil_image_view.h>
#include <vil/vil_load.h>
#include <vil/vil_save.h>
#include <vil/vil_resample_bicub.h>
#include <vil/algo/vil_sobel_1x3.h>
#include <vul/vul_awk.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vpgl/vpgl_proj_camera.h>
#include <vpgl/vpgl_perspective_camera.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/algo/vpgl_camera_compute.h>
#include <vpgl/file_formats/vpgl_nitf_rational_camera.h>
#include <vpgl/bgeo/bgeo_lvcs.h>

#include "btpl_satellite_tools.h"
#include "btpl_imd_parser.h"


//-------------------------------------------------------
btpl_satellite_tools::btpl_satellite_tools()
{
  raw_dir_ = "D:/images_multiview/baghdad/_raw/";
  output_dir_ = "D:/images_multiview/baghdad/";
 
  lidar_ref_img_ = "D:/images_multiview/baghdad/lidar_region.png";
  lidar_ref_ni_ = 599; lidar_ref_nj_ = 599;
  //  lidar_ref_ni_ = 855; lidar_ref_nj_ = 844;
  coverage_img_ = output_dir_; coverage_img_+= "/coverage.png";
  directions_file_ = output_dir_; directions_file_ += "/directions.x3d";

  scene_desc_ = "none";
};


//--------------------------------------------------------
void btpl_satellite_tools::set_scene(
  vcl_string scene_name )
{
  scene_desc_ = scene_name;
  camera_cal_pts_ = raw_dir_; camera_cal_pts_ += scene_name; camera_cal_pts_ += "_camera_cal_points.txt";
  img_coverage_list_ = raw_dir_; img_coverage_list_ += scene_name; img_coverage_list_ += "_img_list.txt";
  img_cal_pts_ = raw_dir_; img_cal_pts_ += scene_name; img_cal_pts_ += "_img_cal_points.txt";
  test_pts_ = raw_dir_; test_pts_ += scene_name; test_pts_ += "_test_points.txt";
  img_cal_obs_list_ = raw_dir_; img_cal_obs_list_ += scene_name; img_cal_obs_list_ += "_img_cal_obs.txt";

  subimg_dir_ = output_dir_; subimg_dir_ += scene_name;
  subimg_cameras_ = output_dir_; subimg_cameras_ += scene_name; subimg_cameras_ += "_cameras.txt";
  subimg_lights_ = output_dir_; subimg_lights_ += scene_name; subimg_lights_ += "_lights.txt";
  ref_subimg_dir_ = output_dir_; ref_subimg_dir_ += scene_name; ref_subimg_dir_ += "_ref";

  if( scene_name == "hiafa" ){
    scene_coord_ = vgl_point_3d<double>( 44.38236181, 33.33591705, 86.7 );
    img_cal_ref_img_= 26;
    subimg_vol_ = vgl_box_3d<double>( 44.376970, 33.339381, 0.0, 44.386474, 33.331465, 200.0 );
  }

  if( scene_name == "embassy" ){
    scene_coord_ = vgl_point_3d<double>( 44.378148563434, 33.296723441892, 54.40 );
    img_cal_ref_img_= 26;
    subimg_vol_ = vgl_box_3d<double>( 44.374381, 33.303143, 0.0, 44.386090, 33.296356, 100.0 );
  }

  if( scene_name == "coast" ){
    scene_coord_ = vgl_point_3d<double>( 44.384919, 33.338769, 67.6 );
    img_cal_ref_img_= 26;
    subimg_vol_ = vgl_box_3d<double>( 44.382430, 33.341767, 0.0, 44.386115, 33.337501, 100.0 );
  }

  if( scene_name == "parkway" ){
    scene_coord_ = vgl_point_3d<double>( 44.339841, 33.338166, 44.6 );
    img_cal_ref_img_= 26;
    subimg_vol_ = vgl_box_3d<double>( 44.337894, 33.340854, 0.0, 44.342918, 33.335180, 100.0 );
  }

};



//--------------------------------------------------------
// MAIN FUNCTIONS
//--------------------------------------------------------


//--------------------------------------------------------
void 
btpl_satellite_tools::compute_coverage()
{  
  vcl_vector< vcl_vector< vgl_point_2d<double> > > regions;

  // Parse every IMD file.
  vcl_string main_dir = raw_dir_; main_dir += "/*.*";
  for( vul_file_iterator f = main_dir; f; ++f ){
    vcl_string file_name = f();
    vcl_cerr << file_name << '\n';
    if( vul_file::is_directory(f()) ) continue;
    if( vul_file::extension( file_name ) != ".IMD" ) continue;

    imd_parser imdp( file_name );
    regions.push_back( imdp.region );
  }

  // Draw the regions in a file.
  vnl_matrix<int> region_counter( lidar_ref_ni_, lidar_ref_nj_ );
  for( int i = 0; i < lidar_ref_ni_; i++ )
    for( int j = 0; j < lidar_ref_nj_; j++ )
      region_counter(i,j) = 0;
  for( unsigned r = 0; r < regions.size(); r++ ){
    vcl_cerr << r << ' ';
    vgl_polygon<double> p( regions[r] );
    for( int i = 0; i < lidar_ref_ni_; i++ ){
      for( int j = 0; j < lidar_ref_nj_; j++ ){
        vgl_point_2d<double> region_coords( 
          44.3320+(44.4045-44.3320)*i/(double)lidar_ref_ni_,
          33.3445-(33.3445-33.2867)*j/(double)lidar_ref_nj_ );
        if( p.contains( region_coords ) ){
          region_counter(i,j) += 1; }
      }
    }
  }

  int min_count = 1000, max_count = 0;
  for( int i = 0; i < lidar_ref_ni_; i++ ){
    for( int j = 0; j < lidar_ref_nj_; j++ ){
      if( region_counter(i,j) > max_count ) max_count = region_counter(i,j);
      if( region_counter(i,j) < min_count ) min_count = region_counter(i,j);
    }
  }

  vil_image_view<vxl_byte> region_img = vil_load( lidar_ref_img_.c_str() );
  for( int i = 0; i < lidar_ref_ni_; i++ ){
    for( int j = 0; j < lidar_ref_nj_; j++ ){
      float red_scale = (region_counter(i,j)-min_count)/(float)(max_count-min_count);
      region_img(i,j,0) = (int)floor( red_scale*255 );
    }
  }
  vil_save( region_img, coverage_img_.c_str() );

/* DRAW WITHOUT THE LIDAR REFERENCE IMAGE.
  float min_x = 1000, min_y = 1000, max_x = 0, max_y = 0;
  for( int r = 0; r < regions.size(); r++ ){
    for( int p = 0; p < 4; p++ ){
      if( regions[r][p].x() < min_x ) min_x = regions[r][p].x();
      if( regions[r][p].x() > max_x ) max_x = regions[r][p].x();
      if( regions[r][p].y() < min_y ) min_y = regions[r][p].y();
      if( regions[r][p].y() > max_y ) max_y = regions[r][p].y();
    }
  }
  float dx = max_x-min_x, dy = max_y-min_y;

  vil_image_view<vxl_byte> regions_img( regions_file_size, regions_file_size, 3 );
  vcl_vector< vgl_point_2d<float> > lidar_bounds;
  lidar_bounds.push_back( vgl_point_2d<float>( 44.3320, 33.3445 ) );
  lidar_bounds.push_back( vgl_point_2d<float>( 44.4045, 33.3445 ) );
  lidar_bounds.push_back( vgl_point_2d<float>( 44.4045, 33.2867 ) );
  lidar_bounds.push_back( vgl_point_2d<float>( 44.3320, 33.2867 ) );
  vgl_polygon<float> lidar_region( lidar_bounds );
  for( int i = 0; i < regions_file_size; i++ ){
    for( int j = 0; j < regions_file_size; j++ ){
      vgl_point_2d<float> region_coords( min_x+i*dx/regions_file_size,
          min_y+j*dy/regions_file_size );
      if( lidar_region.contains( region_coords ) )
        regions_img(i,j,0) = 255;
      else
        regions_img(i,j,0) = 0;
      regions_img(i,j,1) = 0; regions_img(i,j,2) = 0;
    }
  }
  for( int r = 0; r < regions.size(); r++ ){
    vgl_polygon<float> p( regions[r] );
    for( int i = 0; i < regions_file_size; i++ ){
      for( int j = 0; j < regions_file_size; j++ ){
        vgl_point_2d<float> region_coords( min_x+i*dx/regions_file_size,
          min_y+j*dy/regions_file_size );
        if( p.contains( region_coords ) ){
          regions_img(i,j,1) += 3; regions_img(i,j,2) += 3; }
      }
    }

  }
  vil_save( regions_img, regions_file.c_str() );
*/
}


//------------------------------------------------------------
void 
btpl_satellite_tools::plot_directions()
{
  vcl_vector< vgl_vector_3d<double> > camera_dir, light_dir;

  // Parse every IMD file.
  vcl_string main_dir = raw_dir_; main_dir += "/*.*";
  for( vul_file_iterator f = main_dir; f; ++f ){
    vcl_string file_name = f();
    vcl_cerr << file_name << '\n';
    if( vul_file::is_directory(f()) ) continue;
    if( vul_file::extension( file_name ) != ".IMD" ) continue;

    imd_parser imdp( file_name );
    camera_dir.push_back( imdp.camera_dir );
    light_dir.push_back( imdp.sun_dir );
  }

  // Draw the camera and sun directions.
  vcl_ofstream ofs( directions_file_.c_str() );
  ofs << "<X3D version='3.0' profile='Immersive'>\n\n"
    << "<Scene>\n"
    << "<Background skyColor='1 1 1'/>\n\n";

  for( unsigned d = 0; d < 2*light_dir.size(); d++ ){
    vgl_vector_3d<double> this_dir;
    vgl_point_3d<int> color;
    if( d < light_dir.size() ){ 
      this_dir = light_dir[d];
      color.set(0,0,1);
    }
    else{
      this_dir = camera_dir[d-light_dir.size()];
      color.set(1,0,0);
    }
    ofs << " <Shape>\n"
      << "  <IndexedLineSet coordIndex='0, 1'>\n"
      << "   <Coordinate point='0 0 0, " << this_dir.x() << ' ' 
        << this_dir.y() << ' ' << this_dir.z() << "'/>\n"
      << "   <Color color='" << color.x() << ' ' << color.y() << ' ' << color.z() 
        << ", " << color.x() << ' ' << color.y() << ' ' << color.z() << "'/>\n"
      << "  </IndexedLineSet>\n"
      << " </Shape>\n\n";
  }

  ofs << "</Scene>\n"
    << "</X3D>";
};


//----------------------------------------------------------
void 
btpl_satellite_tools::get_coverage_list()
{
  vcl_ofstream ofs( img_coverage_list_.c_str() );
  vcl_string main_dir = raw_dir_; main_dir += "/*.*";
  int img_counter = 0;
  for( vul_file_iterator f = main_dir; f; ++f ){
    vcl_string file_name = f();
    if( vul_file::is_directory(f()) ) continue;
    vcl_string file_ext = vul_file::extension( file_name );
    if( file_ext != ".IMD" ) continue;

    imd_parser imdp( file_name );
    vgl_polygon<double> poly_region( imdp.region );
    if( !poly_region.contains( vgl_point_2d<double>(
      scene_coord_.x(), scene_coord_.y() ) ) ) continue;

    vcl_string img_file_name = vul_file::strip_extension( file_name );
    img_file_name += ".NTF";
    vpgl_nitf_rational_camera P( img_file_name );
    vgl_point_2d<double> ip = P.project( scene_coord_ );
    ofs << img_file_name << ' ' << ip.x()-200 << ' ' << ip.y()-200 << '\n'; //<< imdp.sun_dir.x() << ' ' << imdp.sun_dir.y()
     // << ' ' << imdp.camera_dir.x() << ' ' << imdp.camera_dir.y() << '\n';
    img_counter++;
  }
  vcl_cerr << img_counter << " images containing the point";
  return;
};


//------------------------------------------------------
void 
btpl_satellite_tools::refine_rational_cameras()
{
  double max_offset = 100;

  // Adjust all cameras in the points file.
  vcl_ifstream pifs( camera_cal_pts_.c_str() );
  vul_awk pawk( pifs );
  while( pawk ){
    if( pawk.NF() < 3 ){++pawk; continue;}

    // Get the image name and set up camera name.
    vcl_string img_file_name( pawk[0] );
    vpgl_nitf_rational_camera P( img_file_name );
    vcl_stringstream new_camera_name; 
    new_camera_name << vul_file::strip_extension( img_file_name ) << '_' << scene_desc_ << ".cam";

    // Calculate the offset.
    vgl_point_2d<double> true_target_proj( atof(pawk[1]), atof(pawk[2]) );
    vgl_point_2d<double> target_proj = P.project( scene_coord_ );
    vgl_vector_2d<double> offset = true_target_proj - target_proj;
    if( fabs(offset.x()) > max_offset || fabs(offset.x()) > max_offset )
      vcl_cerr << "WARNING: high offset " << offset.x() << ' ' << offset.y() << '\n';
    double old_img_u_off, old_img_v_off;
    P.image_offset( old_img_u_off, old_img_v_off );
    P.set_image_offset( old_img_u_off + offset.x(), old_img_v_off + offset.y() );

    // Test and save.
    vgl_point_2d<double> test_proj =P.project( scene_coord_ );
    vgl_vector_2d<double> test_offset = true_target_proj - test_proj;
    if( abs(test_offset.x())+abs(test_offset.y()) > .1 ) 
      vcl_cerr << "ERROR: bad reprojection\n";
    P.save( new_camera_name.str() );
    ++pawk;
  }
};


//--------------------------------------------------------------------------
void 
btpl_satellite_tools::get_subimages()
{
  double max_intensity = 1500;

  // Get names for new images;
  vcl_vector< vcl_string > camera_names, img_names;
  filenames_for_scene( &img_names, &camera_names, NULL );
  vcl_vector< vcl_string > subimg_names;
  for( unsigned img = 0; img < img_names.size(); img++ ){
    vcl_string subimg_namebase = vul_file::strip_extension( vul_file::strip_directory( camera_names[img] ) );
    vcl_string subimg_name = subimg_dir_; subimg_name += '/';
    subimg_name += subimg_namebase; subimg_name += ".png";
    subimg_names.push_back( subimg_name );
  }

  // Save a subimage of each image, and record new cameras.
  vcl_ofstream camstream( subimg_cameras_.c_str() );
  for( unsigned img = 0; img < img_names.size(); img++ ){

    vcl_cerr << img_names[img] << '\n';
    vil_image_view<unsigned short> subimg_raw;
    vpgl_proj_camera<double> subimg_cam;
    get_projective_subimg(
      img_names[img], camera_names[img], subimg_vol_, subimg_raw, subimg_cam );

    camstream << "IMAGE: " << img_names[img] << '\n';
    camstream << subimg_cam.get_matrix() << '\n';

    // Draw the image using 0-255.
    vil_image_view<vxl_byte> subimg( subimg_raw.ni(), subimg_raw.nj(), 3 );
    for( unsigned i = 0; i < subimg_raw.ni(); i++ )
      for( unsigned j = 0; j < subimg_raw.nj(); j++ ){
        int pixel_intensity = (int)floor( subimg_raw(i,j)*255/max_intensity );
        if( pixel_intensity > 255 ) pixel_intensity = 255;
        subimg(i,j,0) = subimg(i,j,1) = subimg(i,j,2) = pixel_intensity;
      }
    vil_save( subimg, subimg_names[img].c_str() );
  }
};


//--------------------------------------------------------------------------
void 
btpl_satellite_tools::get_img_cal_obs()
{
  int region_rad = 2;
  double max_region_dev = 40;
  int region_diam = 1+2*region_rad;

  // Read the calibration point file.
  vcl_vector< vgl_point_3d<double> > region_centers;
  vcl_ifstream cf( img_cal_pts_.c_str() );
  vul_awk cfawk( cf );
  while( cfawk ){
    if( cfawk.NF() == 0 ){ ++cfawk; continue; }
    region_centers.push_back(
      vgl_point_3d<double>( atof(cfawk[0]), atof(cfawk[1]), atof(cfawk[2]) ) );
    ++cfawk;
  }

  vcl_vector< vcl_string > camera_names, img_names;
  filenames_for_scene( &img_names, &camera_names, NULL );

  vcl_vector< vcl_vector<double> > region_averages;
  for( unsigned img = 0; img < img_names.size(); img++ ){
    vil_image_resource_sptr img_rsc = vil_load_image_resource( img_names[img].c_str() );
    vpgl_rational_camera<double>* P = read_rational_camera<double>( camera_names[img] );
    vcl_cerr << img_names[img] << '\n';

    // Project each point into the image and take an average in a neighborhood
    vcl_vector<double> img_averages;
    for( unsigned r = 0; r < region_centers.size(); r++ ){
      vgl_point_2d<double> ip = P->project( region_centers[r] );
      vgl_point_2d<int> ip1( (int)ip.x()-region_rad, (int)ip.y()-region_rad );
      vgl_point_2d<int> ip2( (int)ip.x()+region_rad, (int)ip.y()+region_rad );
      if( ip1.x() < 0 || ip1.y() < 0 || ip2.x() >= (int)img_rsc->ni() || ip2.y() >= (int)img_rsc->nj() ){
        img_averages.push_back( -1 ); continue; }
      vil_image_view<unsigned short> sub_img = img_rsc->get_view( 
        ip1.x(), region_diam, ip1.y(), region_diam );

      // Take the average in a neighborhood.
      double region_average = 0;
      for( int i = 0; i < region_diam; i++ )
        for( int j = 0; j < region_diam; j++ )
          region_average += sub_img(i,j);
      region_average /= (region_diam*region_diam);

      // Check that the neighborhood is smooth.
      bool smooth_neighborhood = true;
      for( int i = 0; i < region_diam; i++ )
        for( int j = 0; j < region_diam; j++ )
          if( abs( sub_img(i,j) - region_average ) > max_region_dev ) 
            smooth_neighborhood = false;
      if( !smooth_neighborhood )
        img_averages.push_back( -1 );
      else img_averages.push_back( region_average );
    }
    region_averages.push_back( img_averages );
    delete P;
  }

  // Write to file.
  vcl_ofstream obs_fs( img_cal_obs_list_.c_str() );
  for( unsigned img = 0; img < img_names.size(); img++ ){
    for( unsigned rg = 0; rg < region_centers.size(); rg++ ){
      obs_fs << region_averages[img][rg] << '\t';
    }
    obs_fs << '\n';
  }

};


//-----------------------------------------------------------
void 
btpl_satellite_tools::normalize_images()
{
  double max_intensity = 1500;

  // Read the calibration point file.
  vcl_vector< vgl_point_3d<double> > cal_points;
  vcl_ifstream cf( img_cal_pts_.c_str() ); vul_awk cfawk( cf );
  while( cfawk ){
    if( cfawk.NF() == 0 ){ ++cfawk; continue; }
    cal_points.push_back(
      vgl_point_3d<double>( atof(cfawk[0]), atof(cfawk[1]), atof(cfawk[2]) ) );
    ++cfawk;
  }

  // Read the test points file.
  vcl_vector< vgl_point_3d<double> > test_points;
  vcl_ifstream tf( test_pts_.c_str() ); vul_awk tfawk( tf );
  while( tfawk ){
    if( tfawk.NF() == 0 ){ ++tfawk; continue; }
    test_points.push_back(
      vgl_point_3d<double>( atof(tfawk[0]), atof(tfawk[1]), atof(tfawk[2]) ) );
    ++tfawk;
  }

  // Read the observations file.
  vcl_vector< vcl_vector<double> > obs;
  vcl_ifstream of( img_cal_obs_list_.c_str() ); vul_awk ofawk( of );
  while( ofawk ){
    if( ofawk.NF() == 0 ){ ++ofawk; continue; }
    vcl_vector<double> new_obs;
    for( int i = 0; i < ofawk.NF(); i++ )
      new_obs.push_back( atof(ofawk[i]) );
    obs.push_back( new_obs );
    ++ofawk;
  }
  int num_obs = obs[0].size();

  // Compute least squares fit to reference image.
  vcl_vector< vcl_string > camera_names, img_names;
  filenames_for_scene( &img_names, &camera_names, NULL );
  vcl_vector< vnl_vector<double> > norm_coeffs;
  for( unsigned img = 0; img < img_names.size(); img++ ){
    int num_cor = 0;
    for( unsigned r = 0; r < obs[img].size(); r++ )
      if( obs[img][r] > 0 && obs[img_cal_ref_img_][r] > 0 )
        num_cor++;
    vnl_matrix<double> A( num_cor, 2 );
    vnl_vector<double> b( num_cor );
    int rc = 0;
    for( unsigned r = 0; r < obs[img].size(); r++ ){
      if( obs[img][r] < 0 || obs[img_cal_ref_img_][r] < 0 )
        continue;
      A(rc,0) = obs[img][r];
      A(rc,1) = 1.0;
      b(rc) = obs[img_cal_ref_img_][r];
      rc++;
    }

    vnl_svd<double> Asvd(A);
    norm_coeffs.push_back( Asvd.solve( b ) );
    vcl_cerr << norm_coeffs[img] << '\n';
  }

  // Get names for new images;
  vcl_vector< vcl_string > subimg_names, ref_subimg_names;
  for( unsigned img = 0; img < img_names.size(); img++ ){
    vcl_string subimg_namebase = vul_file::strip_extension( vul_file::strip_directory( camera_names[img] ) );
    vcl_string subimg_name = subimg_dir_; subimg_name += '/';
    subimg_name += subimg_namebase; subimg_name += ".tif";
    subimg_names.push_back( subimg_name );
    vcl_string ref_subimg_name = ref_subimg_dir_; ref_subimg_name += '/';
    ref_subimg_name += subimg_namebase; ref_subimg_name += ".png";
    ref_subimg_names.push_back( ref_subimg_name );
  }
  
  // Set up lvcs_converter.
  double lon_low = subimg_vol_.min_x();
  double lon_high = subimg_vol_.max_x();
  double lat_low = subimg_vol_.min_y();
  double lat_high = subimg_vol_.max_y();
  assert( lat_low < lat_high && lon_low < lon_high );
  bgeo_lvcs lvcs_converter( lat_low, lon_low, 
    .5*(subimg_vol_.min_z()+subimg_vol_.max_z()), bgeo_lvcs::wgs84, bgeo_lvcs::DEG );

  // Save normalized and reference subimages of each image, and record new cameras.
  vcl_ofstream camstream( subimg_cameras_.c_str() );
  for( unsigned img = 0; img < img_names.size(); img++ ){

    vcl_cerr << img_names[img] << '\n';
    vil_image_view<unsigned short> subimg_raw;
    vpgl_proj_camera<double> subimg_cam;
    get_projective_subimg(
      img_names[img], camera_names[img], subimg_vol_, subimg_raw, subimg_cam );

    camstream << "IMAGE: " << img_names[img] << '\n';
    camstream << subimg_cam.get_matrix() << '\n';

    // Draw the normalized subimg.
    vil_image_view<vxl_byte> subimg( subimg_raw.ni(), subimg_raw.nj() );
    for( unsigned i = 0; i < subimg_raw.ni(); i++ )
      for( unsigned j = 0; j < subimg_raw.nj(); j++ ){
        int pixel_intensity =  (int)floor( (255/max_intensity)*
          ( norm_coeffs[img](0)*subimg_raw(i,j) + norm_coeffs[img](1) ) );
        if( pixel_intensity > 255 ) pixel_intensity = 255;
        if( pixel_intensity < 0 ) pixel_intensity = 0;
        subimg(i,j) = pixel_intensity;
      }
    vil_save( subimg, subimg_names[img].c_str() );
    
    // Draw reference image using 0-255 and marking points.
    vil_image_view<vxl_byte> ref_subimg( subimg_raw.ni(), subimg_raw.nj(), 3 );
    for( unsigned i = 0; i < subimg_raw.ni(); i++ )
      for( unsigned j = 0; j < subimg_raw.nj(); j++ ){
        int pixel_intensity = (int)floor( subimg_raw(i,j)*255/max_intensity );
        if( pixel_intensity > 255 ) pixel_intensity = 255;
        ref_subimg(i,j,0) = ref_subimg(i,j,1) = ref_subimg(i,j,2) = pixel_intensity;
      }
    for( unsigned p = 0; p < cal_points.size(); p++ ){
      double lcx, lcy, lcz;
      lvcs_converter.global_to_local( 
        cal_points[p].x(), cal_points[p].y(), cal_points[p].z(),
        bgeo_lvcs::wgs84, lcx, lcy, lcz );
      vgl_homg_point_2d<double> iph = subimg_cam.project( 
        vgl_homg_point_3d<double>( lcx, lcy, lcz ) );
      vgl_point_2d<int> ip( (int)floor(iph.x()/iph.w()), (int)floor(iph.y()/iph.w()) );
      if( ip.x() < 0 || ip.y() < 0 || ip.x() >= (int)subimg_raw.ni() || ip.y() >= (int)subimg_raw.nj() ) continue;
      ref_subimg( ip.x(), ip.y(), 0 ) = 255;
      ref_subimg( ip.x(), ip.y(), 1 ) = ref_subimg( ip.x(), ip.y(), 2 ) = 0;
    }
    for( unsigned tp = 0; tp < test_points.size(); tp++ ){
      double lcx, lcy, lcz;
      lvcs_converter.global_to_local( 
        test_points[tp].x(), test_points[tp].y(), test_points[tp].z(),
        bgeo_lvcs::wgs84, lcx, lcy, lcz );
      vgl_homg_point_2d<double> iph = subimg_cam.project( 
        vgl_homg_point_3d<double>( lcx, lcy, lcz ) );
      vgl_point_2d<int> ip( (int)floor(iph.x()/iph.w()), (int)floor(iph.y()/iph.w()) );
      if( ip.x() < 0 || ip.y() < 0 || ip.x() >= (int)subimg_raw.ni() || ip.y() >= (int)subimg_raw.nj() ) continue;
      ref_subimg( ip.x(), ip.y(), 2 ) = 255;
      ref_subimg( ip.x(), ip.y(), 0 ) = ref_subimg( ip.x(), ip.y(), 1 ) = 0;
    }
    vil_save( ref_subimg, ref_subimg_names[img].c_str() );
  }


};


//---------------------------------------------------------------------
void 
btpl_satellite_tools::scene_bounds()
{
  // Set up the geo converter.
  double lon_low = subimg_vol_.min_x();
  double lon_high = subimg_vol_.max_x();
  double lat_low = subimg_vol_.min_y();
  double lat_high = subimg_vol_.max_y();
  assert( lat_low < lat_high && lon_low < lon_high );
  bgeo_lvcs lvcs_converter( lat_low, lon_low, 
    .5*(subimg_vol_.min_z()+subimg_vol_.max_z()), bgeo_lvcs::wgs84, bgeo_lvcs::DEG );

  // Get a new local bounding box.
  double min_lx = 100000000000, min_ly = 100000000000, min_lz = 100000000000;
  double max_lx = -100000000000, max_ly = -100000000000, max_lz = -100000000000;
  for( int cx = 0; cx < 2; cx++ ){
    for( int cy = 0; cy < 2; cy++ ){
      for( int cz = 0; cz < 2; cz++ ){
        vgl_point_3d<double> wc( 
          subimg_vol_.min_x()*cx + subimg_vol_.max_x()*(1-cx),
          subimg_vol_.min_y()*cy + subimg_vol_.max_y()*(1-cy),
          subimg_vol_.min_z()*cz + subimg_vol_.max_z()*(1-cz) );
        double lcx, lcy, lcz;
        lvcs_converter.global_to_local(
          wc.x(), wc.y(), wc.z(), bgeo_lvcs::wgs84, lcx, lcy, lcz );
        vgl_point_3d<double> wc_loc( lcx, lcy, lcz );
        if( wc_loc.x() < min_lx ) min_lx = wc_loc.x();
        if( wc_loc.y() < min_ly ) min_ly = wc_loc.y();
        if( wc_loc.z() < min_lz ) min_lz = wc_loc.z();
        if( wc_loc.x() > max_lx ) max_lx = wc_loc.x();
        if( wc_loc.y() > max_ly ) max_ly = wc_loc.y();
        if( wc_loc.z() > max_lz ) max_lz = wc_loc.z();
      }
    }
  }
  vcl_cerr << min_lx << ' ' << min_ly << ' ' << min_lz << '\n';
  vcl_cerr << max_lx << ' ' << max_ly << ' ' << max_lz << '\n';
};


//--------------------------------------------------------------------
vgl_point_3d<double>
btpl_satellite_tools::transform_wp(
  vgl_point_3d<double> wp )
{
  // Set up the geo converter.
  double lon_low = subimg_vol_.min_x();
  double lat_low = subimg_vol_.min_y();
  bgeo_lvcs lvcs_converter( lat_low, lon_low, 
    .5*(subimg_vol_.min_z()+subimg_vol_.max_z()), bgeo_lvcs::wgs84, bgeo_lvcs::DEG );

  double lcx, lcy, lcz;
  lvcs_converter.global_to_local(
    wp.x(), wp.y(), wp.z(), bgeo_lvcs::wgs84, lcx, lcy, lcz );
  return vgl_point_3d<double>( lcx, lcy, lcz );
};


//------------------------------------------------------------
void 
btpl_satellite_tools::get_lighting_list()
{
  vcl_vector< vcl_string > imd_names;
  filenames_for_scene( NULL, NULL, &imd_names );
  vcl_ofstream ofs( subimg_lights_.c_str() );
  for( unsigned f = 0; f < imd_names.size(); f++ ){
    imd_parser imdp( imd_names[f].c_str() );
    ofs << imdp.sun_dir.x() << ' ' << imdp.sun_dir.y() << ' ' << imdp.sun_dir.z() << '\n';
  }
};


//-----------------------------------------------------------------
// HELPER FUNCTIONS:
//-----------------------------------------------------------------


//-----------------------------------------------------------------
void 
btpl_satellite_tools::filenames_for_scene(
  vcl_vector< vcl_string >* img_names,
  vcl_vector< vcl_string >* cam_names,
  vcl_vector< vcl_string >* imd_names )
{
  vcl_string main_dir = raw_dir_;
  if( img_names != NULL ) img_names->clear(); 
  if( cam_names != NULL ) cam_names->clear(); 
  if( imd_names != NULL ) imd_names->clear();
  main_dir += "/*"; main_dir += scene_desc_; main_dir +=".cam";   
  for( vul_file_iterator f = main_dir; f; ++f ){
    vcl_string file_name = f();
    int last_us;
    for( unsigned c = file_name.size()-1; c >= 0; c-- )
      if( file_name[c] == '_' ){ last_us = c; break; }
    vcl_stringstream file_base;
    for( int c = 0; c < last_us; c++ )
      file_base << file_name[c];

    vcl_string img_name = file_base.str(); img_name+= ".NTF";
    vcl_string imd_name = file_base.str(); imd_name+= ".IMD";
        
    if( cam_names != NULL ) cam_names->push_back( file_name );
    if( img_names != NULL ) img_names->push_back( img_name );
    if( imd_names != NULL ) imd_names->push_back( imd_name );
  }
}


//---------------------------------------------------------
void 
btpl_satellite_tools::get_projective_subimg(
  vcl_string img_file,
  vcl_string cam_file,
  const vgl_box_3d<double>& vol,
  vil_image_view<unsigned short>& subimg,
  vpgl_proj_camera<double>& subimg_cam )
{
  vil_image_resource_sptr img_rsc = vil_load_image_resource( img_file.c_str() );
  int img_size_x = img_rsc->ni(), img_size_y = img_rsc->nj();
  vpgl_rational_camera<double>* cam = read_rational_camera<double>( cam_file );

  // Get bounds of the projected volume in the image.
  double min_x = 1000000, max_x = 0, min_y = 1000000, max_y = 0;
  for( double dx = 0; dx <= 1; dx++ ){
    for( double dy = 0; dy <= 1; dy++ ){
      for( double dz = 0; dz <= 1; dz++ ){
        double x = (1-dx)*vol.min_x() + dx*vol.max_x();
        double y = (1-dy)*vol.min_y() + dy*vol.max_y();
        double z = (1-dz)*vol.min_z() + dz*vol.max_z();
        vgl_point_2d<double> c = cam->project( vgl_point_3d<double>(x,y,z) );
        if( c.x() < min_x ) min_x = c.x(); if( c.x() > max_x ) max_x = c.x();
        if( c.y() < min_y ) min_y = c.y(); if( c.y() > max_y ) max_y = c.y();
      }
    }
  }
  if( min_x < 0 ) min_x = 0; if( max_x >= img_size_x-1 ) max_x = img_size_x-1;
  if( min_y < 0 ) min_y = 0; if( max_y >= img_size_y-1 ) max_y = img_size_y-1;

  // Pull out the subimg with these bounds.
  vgl_point_2d<int> subimg_corner( (int)floor( min_x ), (int)floor( min_y ) );
  vgl_vector_2d<int> subimg_length( (int)floor( max_x-min_x ), (int)floor( max_y-min_y ) );
  subimg = img_rsc->get_view( subimg_corner.x(), subimg_length.x(), 
    subimg_corner.y(), subimg_length.y() );
  vnl_matrix_fixed<double,3,3> subimg_offset(0.0);
  subimg_offset(0,0) = subimg_offset(1,1) = subimg_offset(2,2) = 1.0;
  subimg_offset(0,2) = -subimg_corner.x(); subimg_offset(1,2) = -subimg_corner.y();

  // Get a projective camera and adjust for the subimg.
  vpgl_perspective_camera<double> persp_cam;
  vgl_h_matrix_3d<double> norm_trans;
  vpgl_perspective_camera_compute::compute_local( *cam, vol, persp_cam, norm_trans );

  subimg_cam.set_matrix( persp_cam.get_matrix() );
  subimg_cam = postmultiply( subimg_cam, norm_trans );
  subimg_cam = premultiply( subimg_cam, subimg_offset );
  delete cam;
};


/*
//-------------------------------------------------------
void inspect_ray()
{
  vcl_string main_dir = "D:\\images_multiview\\baghdad\\images";
  vcl_string x3d_file = "D:\\results\\baghdad.x3d";
  vcl_string target_desc = "hiafa";

  vgl_point_2d<double> ray_base_coord( 44.382362-.000317, 33.335917+.000138 );
  double min_elv = 0, max_elv = 100;
  int num_insp_inc = 50;
  double max_color_val = 1000;

  double insp_inc = (max_elv-min_elv)/num_insp_inc;
  vgl_point_3d<double> min_insp( ray_base_coord.x(), ray_base_coord.y(), min_elv );
  vgl_point_3d<double> max_insp( ray_base_coord.x(), ray_base_coord.y(), max_elv );

  vcl_vector< vgl_vector_3d<double> > light_dirs;
  vcl_vector< vcl_vector<double> > ray_vals;

  // Get all cameras, images, and IMD files with the target_desc.
  main_dir += "/*.cam"; target_desc += ".cam";
  int img_counter = 0;
  for( vul_file_iterator f = main_dir; f; ++f ){
    vcl_string file_name = f();
    int last_us;
    for( unsigned c = file_name.size()-1; c >= 0; c-- )
      if( file_name[c] == '_' ){ last_us = c; break; }
    vcl_stringstream file_desc;
    for( unsigned c = last_us+1; c < file_name.size(); c++ )
      file_desc << file_name[c];
    if( file_desc.str() != target_desc ) continue;

    vcl_stringstream img_file_name;
    for( int c = 0; c < last_us; c++ )
      img_file_name << file_name[c];
    img_file_name << ".NTF";

    vcl_stringstream imd_file_name;
    for( int c = 0; c < last_us; c++ )
      imd_file_name << file_name[c];
    imd_file_name << ".IMD";
    imd_parser imdp( imd_file_name.str() );
    light_dirs.push_back( imdp.sun_dir );

    vcl_cerr << img_file_name.str() << '\n';

    // Find an image region containing the whole ray and load it.
    vpgl_rational_camera<double> P( file_name );
    vgl_point_2d<double> min_insp_pixel = P.project( min_insp );
    vgl_point_2d<double> max_insp_pixel = P.project( max_insp );
    double min_x = min_insp_pixel.x(), min_y = min_insp_pixel.y();
    double max_x = min_insp_pixel.x(), max_y = min_insp_pixel.y();
    if( max_insp_pixel.x() < min_x ) min_x = max_insp_pixel.x();
    if( max_insp_pixel.x() > max_x ) max_x = max_insp_pixel.x();
    if( max_insp_pixel.y() < min_y ) min_y = max_insp_pixel.y();
    if( max_insp_pixel.y() > max_y ) max_y = max_insp_pixel.y();
    min_x -= 50; min_y -= 50; max_x += 50; max_y += 50;

    int window_x = (int)floor( min_x ), window_y = (int)floor( min_y );
    int window_dx = (int)ceil( max_x ) - window_x;
    int window_dy = (int)ceil( max_y ) - window_y;
    vil_image_resource_sptr img = 
      vil_load_image_resource( img_file_name.str().c_str() );
    vil_image_view<unsigned short> sub_img = img->get_view( window_x, window_dx, window_y, window_dy );
    
    vil_image_view<vxl_byte> insp_img( sub_img.ni(), sub_img.nj() );
    for( unsigned int i = 0; i < sub_img.ni(); i++ ){
      for( unsigned int j = 0; j < sub_img.nj(); j++ ){
        int this_color = (int)floor( 255*sub_img(i,j)/max_color_val );
        if( this_color > 255 ) this_color = 255;
        insp_img(i,j) = this_color;
      }
    }

    // Record the pixel value of every voxel on the ray.
    vcl_vector<double> these_ray_vals;
    for( int i = 0; i < num_insp_inc; i++ ){
      vgl_point_3d<double> insp_voxel = min_insp + 
        vgl_vector_3d<double>( 0, 0, min_elv+insp_inc*i );
      vgl_point_2d<double> insp_pixel = P.project( insp_voxel );
      vgl_point_2d<int> insp_coords( (int)floor(insp_pixel.x()-min_x),
        (int)floor(insp_pixel.y()-min_y) );
      these_ray_vals.push_back( sub_img( insp_coords.x(), insp_coords.y() ) );
      insp_img( insp_coords.x(), insp_coords.y() ) = 0;
    }
    ray_vals.push_back( these_ray_vals );
    vil_save( insp_img, "D:\\results\\baghdad_sub.png" );

  }

  // Write the x3d file.
  vcl_ofstream ofs( x3d_file.c_str() );
  ofs << "<X3D version='3.0' profile='Immersive'>\n\n"
    << "<Scene>\n"
    << "<Background skyColor='0 0 1'/>\n\n";

  for( unsigned img = 0; img < ray_vals.size(); img++ ){
    for( int inc = 0; inc < num_insp_inc; inc++ ){

      double this_color = ray_vals[img][inc]/max_color_val;
      if( this_color > 1.0 ) this_color = 1.0;

      ofs << "<Transform translation='" << 1.5*inc+light_dirs[img].x() << ' ' 
          << light_dirs[img].y() << ' ' << this_color/2.0 << "'>\n" 
        << " <Shape>\n"
        << "  <Appearance>\n"
        << "   <Material diffuseColor='1 0 0'/>\n"
        << "  </Appearance>\n"
        << "  <Box size='.05 .05 " << this_color << "'/>"
        << " </Shape>\n"
        << "</Transform>\n\n";
    }
  }
      
  ofs << "</Scene>\n"
    << "</X3D>";
};
*/


#endif // btpl_satellite_tools_cxx_
