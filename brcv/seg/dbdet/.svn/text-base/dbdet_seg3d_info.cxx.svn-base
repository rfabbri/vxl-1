// This is dbdet/dbdet_seg3d_info.cxx

//:
// \file

#include "dbdet_seg3d_info.h"

#include <vil/vil_load.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <vcl_algorithm.h>

//: constructor
dbdet_seg3d_info::dbdet_seg3d_info() :
image_folder_(""),
contour_folder_("")
{
  this->frame_list_.clear();
}


//: Load images from the image folder.
bool dbdet_seg3d_info::
load_images_from_image_folder()
{
  // clean the current data structure
  this->frame_list_.clear();

  // get the list of files
  vcl_vector<vcl_string> filenames;
  for ( vul_file_iterator fit = this->image_folder() + "/*.*"; fit; ++fit )
  {
    // check to see if file is a directory.
    if (vul_file::is_directory(fit()))
      continue;
    
    // ignore all non-image files
    if (!vil_load(fit()))
      continue;
    filenames.push_back(fit());
  }

  // Sort - because the file iterator uses readdir() it does not
  //        iterate over files in alphanumeric order 
  vcl_sort(filenames.begin(),filenames.end());


  // Save each image into one frame
  for ( unsigned i=0; i<filenames.size(); ++i )
  {
    vcl_string filename = vul_file::strip_directory(filenames[i].c_str());
    dbdet_seg3d_info_frame frame;
    frame.image_file = filename;
    frame.contour_file_list.clear();
    this->add_frame(frame);
  }

  
  return true;
}





//: Load the contours using the contour folder and image names
bool dbdet_seg3d_info::
load_contours_from_image_names()
{
  
  // get list of contour files in the folder
  vcl_vector<vcl_string> contour_files;
  for ( vul_file_iterator fit = this->contour_folder() + "/*.con"; fit; ++fit )
  {
    // check to see if file is a directory.
    if (vul_file::is_directory(fit()))
      continue;

    contour_files.push_back(vul_file::strip_directory(fit()));
  }

  // Associated the contour files with the image using their names
  // If the begininig of a contour file name is the same as an image's name then
  // associate it with that image
  for (int frame_index=0; frame_index<this->num_frames(); ++frame_index)
  {
    dbdet_seg3d_info_frame frame = this->frame(frame_index);
    vcl_string image_name = vul_file::strip_extension(frame.image_file);

    // clean the current data structure
    this->frame(frame_index).contour_file_list.clear();


    vcl_vector<vcl_string > frame_contours;

    // iterate thru all contour files to check which one belongs to this frame
    for (vcl_vector<vcl_string>::iterator contour_itr = contour_files.begin();
      contour_itr != contour_files.end(); )
    {
      vcl_string contour_name = vul_file::strip_extension(*contour_itr);

      // check if the contour name and image name match
      if (contour_name.size() < image_name.size())
      {
        ++contour_itr;
        continue;
      }
      if (contour_name.substr(0, image_name.size()) != image_name)
      {
        ++contour_itr;
        continue;
      }

      frame_contours.push_back(*contour_itr);
      contour_itr = contour_files.erase(contour_itr);
    }

    // sort the contour files for one frame
    vcl_sort(frame_contours.begin(), frame_contours.end());

    // insert the contours into the frame data
    for (unsigned i=0; i< frame_contours.size(); ++i)
    {
      this->frame(frame_index).contour_file_list.push_back(frame_contours[i]);
    }
  }

  return false;
}





