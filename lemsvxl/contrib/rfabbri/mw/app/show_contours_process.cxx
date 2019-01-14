
//:
// \file

#include "show_contours_process.h"
#include <bmcsd/bmcsd_util.h>
#include <iostream>

#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <vil/vil_load.h>
#include <vil/vil_image_resource_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>

#include <cstring>



//: Constructor
show_contours_process::show_contours_process() : bpro1_process()
{
  if( !parameters()->add( "Image file <filename...>" , "-image_filename" , bpro1_filepath("","*") ))
  {
    std::cerr << "ERROR: Adding parameters in " __FILE__ << std::endl;
  }
}


//: Destructor
show_contours_process::~show_contours_process()
{
}


//: Clone the process
bpro1_process*
show_contours_process::clone() const
{
  return new show_contours_process(*this);
}


//: Return the name of the process
std::string show_contours_process::name()
{
  return "Show Contours";
}


//: Returns a vector of strings describing the input types to this process
std::vector< std::string > show_contours_process::get_input_type()
{
  std::vector< std::string > to_return;

  // no input type required
  to_return.clear();

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
std::vector< std::string > show_contours_process::get_output_type()
{
  std::vector< std::string > to_return;

  // output type
  to_return.push_back( "image" );

  return to_return;
}


//: Run the process on the current frame
bool
show_contours_process::execute()
{
   //no input storage class for this process
   clear_output();

   bpro1_filepath image_path;
   parameters()->get_value( "-image_filename" , image_path );
   std::string image_filename = image_path.path;

   // LOAD IMAGE
   vil_image_resource_sptr loaded_image = vil_load_image_resource( image_filename.c_str() );
   if( !loaded_image ) {
     std::cerr << "Failed to load image file" << image_filename << std::endl;
     return false;
   }
   
   // create the storage data structure
   vidpro1_image_storage_sptr image_storage = vidpro1_image_storage_new();
   image_storage->set_image( loaded_image );
 
   output_data_[0].push_back(image_storage);
   
   // LOAD CON FILES

   std::vector<std::string> con_fnames;
   if (!con_filenames(image_filename,con_fnames))
      return false;

   for (unsigned i=0; i< con_fnames.size(); ++i) {
      std::vector<vsol_point_2d_sptr> points;
      bool is_open;

      std::cout << "Reading: " <<  con_fnames[i] << std::endl;
      if (!load_con_file(con_fnames[i],points,&is_open))
         return false;

      std::vector< vsol_spatial_object_2d_sptr > contours; //:< dummy; holds just 1 contour
      if (is_open) {
        vsol_polyline_2d_sptr newContour = new vsol_polyline_2d (points);
        contours.push_back(newContour->cast_to_spatial_object());
      } else {
        vsol_polygon_2d_sptr newContour = new vsol_polygon_2d (points);
        contours.push_back(newContour->cast_to_spatial_object());
      }

      // Create the output storage class
      vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
      output_vsol->add_objects(contours, con_fnames[i]);
      output_data_[0].push_back(output_vsol);

      // Output names

      std::string vsol_name("vsol2D-");
      vsol_name.append(con_fnames[i]);

      std::vector<std::string> new_out_names(output_names());
      new_out_names.push_back(vsol_name);
      set_output_names(new_out_names);
   }
   return true;
}


//: Finish
bool show_contours_process::finish() 
{
  return true;
}


//: Returns the number of input frames to this process
int
show_contours_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
show_contours_process::output_frames()
{
  return 1;
}

bool show_contours_process::
loadCON (std::string filename, float scale)
{
  // new vector to store the contours
  std::vector< vsol_spatial_object_2d_sptr > contours;
  
  // vector to store the points
  std::vector< vsol_point_2d_sptr > points;

  std::ifstream infp(filename.c_str(), std::ios::in);
  bool isOpen_;

  if (!infp) {
    std::cout << " Error opening file  " << filename << std::endl;
    return false;
  }

  char lineBuffer[2000]; //200
  infp.getline(lineBuffer,2000);
  if (std::strncmp(lineBuffer,"CONTOUR",7)) {
    std::cerr << "Invalid File " << filename.c_str() << std::endl
             << "Should be CONTOUR " << lineBuffer << std::endl;
    return false;
  }

  char openFlag[2000];
  infp.getline(openFlag,2000);
  if (!std::strncmp(openFlag,"OPEN",4))
    isOpen_ = true;
  else if (!std::strncmp(openFlag,"CLOSE",5))
    isOpen_ = false;
  else{
    std::cerr << "Invalid File " << filename.c_str() << std::endl
             << "Should be OPEN/CLOSE " << openFlag << std::endl;
    return false;
  }

  int i, numOfPoints;
  infp >> numOfPoints;

  double x,y;
  for (i=0;i<numOfPoints;i++) {
    infp >> x >> y;
    vsol_point_2d_sptr newPt = new vsol_point_2d (x/scale,y/scale);
    points.push_back(newPt);
  }

  //close file
  infp.close();

  // create a polyline or a polygon depending on whether the contour
  // file is labelled OPEn or CLOSE
  if (isOpen_) {
    vsol_polyline_2d_sptr newContour = new vsol_polyline_2d (points);
    contours.push_back(newContour->cast_to_spatial_object());
  }
  else {
    vsol_polygon_2d_sptr newContour = new vsol_polygon_2d (points);
    contours.push_back(newContour->cast_to_spatial_object());
  }

  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(contours, filename);
  output_data_[0].push_back(output_vsol);


  return true;
}
