// This is lemsvxlsrc/contrib/bwm_lidar/algo/world_modeler.cxx

//:
// \file
// \brief Main engine for World Modeling
//
// \author Ibrahim Eden, (ieden@lems.brown.edu)
// \date 12/16/2007
//      
// \endverbatim

#include "world_modeler.h"
#include <vcl_cassert.h>
#include <vcl_set.h>


//: fill infinite values with the median of its 8 neighborhood
void world_modeler::median_fill(vil_image_view<double>& image)
{
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  for(unsigned int i=0; i<ni; ++i){
    for(unsigned int j=0; j<nj; ++j){
      if(!vnl_math_isfinite(image(i,j)))
      {
        vcl_vector<double> v;
        for(int si = i-1; si<=i+1; ++si){
          if(si < 0) continue;
          if(si >= ni) break;
          for(int sj = j-1; sj<=j+1; ++sj){
            if(sj < 0) continue;
            if(si == i && sj == j) continue;
            if(sj >= nj) break;
            if(vnl_math_isfinite(image(si,sj)))
              v.push_back(image(si,sj));
          }
        }
        if(!v.empty()){
          // partial sort
          unsigned int r = v.size()/2;
          vcl_nth_element(v.begin(), v.begin()+r, v.end());
          image(i,j) = v[r];
        }
      }
    }
  }
}


//: read one or more FLIMAP ASCII files and build a pair of LIDAR images (like Buckeye format)
// \param return1 returns the first return (max point) at each pixel
// \param return2 returns the last return (min point) at each pixel
// \param rgb_img return the colors at the first returns
void world_modeler::
generate_lidar_images(const vcl_string& glob,
                      const vgl_box_2d<double>& bbox,
                            vil_image_view<double>& return1,
                            vil_image_view<double>& return2,
                            vil_image_view<vxl_byte>& rgb_img)
{
  // find all files that match the glob and contain ".xyz" in the file name 
  vcl_vector<vcl_string> xyz_file_name_list;
  for (vul_file_iterator fn=glob; fn; ++fn) {
    vcl_string curr_image_filename = fn();
    vcl_string::size_type pos_xyz = curr_image_filename.find(".xyz");
    if (pos_xyz != vcl_string::npos) {
      xyz_file_name_list.push_back(curr_image_filename);
    }
  }
  if(xyz_file_name_list.size() < 1){
    vcl_cerr << "Glob did not match any .xyz files"<<vcl_endl;
    exit(-1);
  }

  double pixel_size = 0.75;
  int ni = (bbox.max_x() - bbox.min_x())/pixel_size;
  int nj = (bbox.max_y() - bbox.min_y())/pixel_size;

  // resize the images and fill with default values
  return1.set_size(ni,nj);
  return1.fill(-vcl_numeric_limits<double>::infinity());
  return2.set_size(ni,nj);
  return2.fill(vcl_numeric_limits<double>::infinity());
  rgb_img.set_size(ni,nj,3);
  rgb_img.fill(0);

  // read each 3D point file and map the points into the images
  unsigned count=0;
  for(unsigned xyz=0; xyz<xyz_file_name_list.size(); ++xyz){
    vnl_matrix<double> points_colors;
    vcl_cout << "Started Reading " << xyz_file_name_list[xyz] << "\n";
    flimap_reader::read_flimap_file(xyz_file_name_list[xyz],points_colors);
    vcl_cout << "Finished Reading " << xyz_file_name_list[xyz] << "\n";

    vcl_cout << "Started Processing " << xyz_file_name_list[xyz] << "\n";
    for(unsigned p=0; p<points_colors.rows(); ++p){
      // Calculate the pixel position for 3D point
      int i = (points_colors(p,0) - bbox.min_x())/pixel_size;
      int j = (points_colors(p,1) - bbox.min_y())/pixel_size;

      // Check if the pixel is inside the image
      if(i < 0 || i >= ni || j < 0 || j >= nj){
        continue;
      }

      ++count;
      // The value of the pixel is basically the height of the 3D point
      double val = points_colors(p,2);

      // Update first return and last return values
      if(val < return2(i,j)){
        return2(i,j) = val;
      }
      if(val > return1(i,j)){
        return1(i,j) = val;
        rgb_img(i,j,0) = static_cast<char>(points_colors(p,3));
        rgb_img(i,j,1) = static_cast<char>(points_colors(p,4));
        rgb_img(i,j,2) = static_cast<char>(points_colors(p,5));
      }
    }
    vcl_cout << "Finished Processing " << xyz_file_name_list[xyz] << " with "<<count<<" points"<< "\n";
  }

  // use a median filter to fill in isolated pixels with missing data
  median_fill(return1);
  median_fill(return2);
}


//: This function is the main engine that generates the urban model using the input LIDAR image.
//:   Outputs:
//:     labels: an image where negative intensities correspond to a vegetation component
//:                      where positive intensities correspond to a building component        
//:     labels_colored: an image where buildings are colored with reddish colors and vegetation are colored with blueish colors
//:     height: a matrix where the height in the LIDAR image is set for each pixel
void world_modeler::generate_model(const vcl_string input_path, const lidar_labeling_params params, vil_image_view<int>& labels, vil_image_view<unsigned char>& labels_colored, vil_image_view<unsigned char>& colors, vnl_matrix<double>& height, vnl_matrix<int>& occupied){
  //  Bounding box for the region of interest. This is now manuel, it should be 
  //    taken from the user or calculated automatically. Not very important at the moment
  vgl_point_2d<double> bb_min(594025.0, 3343770.0);
  vgl_point_2d<double> bb_max(594450.0, 3344020.0);
  
  double INF = 999999999.9;
  double pixel_size = params.pixel_size_;

  // Get the list of all .xyz files in the directory
  vcl_string path_xyz = input_path + "\\*.*";
  vcl_vector<vcl_string> xyz_file_name_list;
  for (vul_file_iterator fn=path_xyz; fn; ++fn) {
    vcl_string curr_image_filename = fn();
    vcl_string::size_type pos_xyz = curr_image_filename.find(".xyz");
    if (pos_xyz != vcl_string::npos) {
      xyz_file_name_list.push_back(curr_image_filename);
    }
  }
  if(xyz_file_name_list.size() < 1){
    vcl_cerr << "Please provide an input directory that contains .xyz files!\n";
    exit(-1);
  }

  // Number of pixels in x and y directions
  int num_pixels_x = (bb_max.x() - bb_min.x())/pixel_size;
  int num_pixels_y = (bb_max.y() - bb_min.y())/pixel_size;

  // Initialized the call-by-reference (output) parameters
  vnl_matrix<double> min_return(num_pixels_x,num_pixels_y,INF);
  vnl_matrix<double> max_return(num_pixels_x,num_pixels_y,-INF);
  colors.set_size(num_pixels_x,num_pixels_y,3);
  colors.fill(0);
  occupied.set_size(num_pixels_x,num_pixels_y);
  occupied.fill(0);
  height.set_size(num_pixels_x,num_pixels_y);
  height.fill(0.0);

  for(unsigned xyz=0; xyz<xyz_file_name_list.size(); xyz++){
    vnl_matrix<double> points_colors;
    vcl_cout << "Started Reading " << xyz_file_name_list[xyz] << "\n";
    flimap_reader::read_flimap_file(xyz_file_name_list[xyz],points_colors);
    vcl_cout << "Finished Reading " << xyz_file_name_list[xyz] << "\n";

    vcl_cout << "Started Processing " << xyz_file_name_list[xyz] << "\n";
    for(unsigned i=0; i<points_colors.rows(); i++){
      // Calculate the pixel position for 3D point
      int curr_pixel_x = (points_colors(i,0) - bb_min.x())/pixel_size;
      int curr_pixel_y = (points_colors(i,1) - bb_min.y())/pixel_size;

      // Check if the pixel is inside the image
      if(curr_pixel_x < 0 || curr_pixel_x >= num_pixels_x || curr_pixel_y < 0 || curr_pixel_y >= num_pixels_y){
        continue;
      }

      occupied(curr_pixel_x,curr_pixel_y) = 1;

      // The value of the pixel is basically the height of the 3D point
      double curr_pixel_val = points_colors(i,2);

      // Update first return and last return values
      if(curr_pixel_val < min_return(curr_pixel_x,curr_pixel_y)){
        min_return(curr_pixel_x,curr_pixel_y) = curr_pixel_val;
      }
      if(curr_pixel_val > max_return(curr_pixel_x,curr_pixel_y)){
        max_return(curr_pixel_x,curr_pixel_y) = curr_pixel_val;
        colors(curr_pixel_x,curr_pixel_y,0) = (int)(points_colors(i,3));
        colors(curr_pixel_x,curr_pixel_y,1) = (int)(points_colors(i,4));
        colors(curr_pixel_x,curr_pixel_y,2) = (int)(points_colors(i,5));
      }
    }
    vcl_cout << "Finished Processing " << xyz_file_name_list[xyz] << "\n";
  }

  // Find maximum and minumum elevations
  double max_elev = -INF;
  double min_elev = INF;
  for(int i=0; i<num_pixels_x; i++){
    for(int j=0; j<num_pixels_y; j++){
      max_elev = vnl_math_max(max_elev,max_return(i,j));
      min_elev = vnl_math_min(min_elev,min_return(i,j));
    }
  }

  // Normalize the height values
  for(int i=0; i<num_pixels_x; i++){
    for(int j=0; j<num_pixels_y; j++){
      if(occupied(i,j)==1){
        height(i,j) = (max_return(i,j)-min_elev)/pixel_size;
      }
    }
  }

  // Create the desired label image for buildings and vegetation
  lidar_labeler::label_lidar(max_return,min_return,params,min_elev,max_elev,input_path,labels);

  // Create the colored label image where buildings have reddish colors and vegetaiton have blueish colors
  get_connected_components::colorize_image(labels,labels_colored);
}
