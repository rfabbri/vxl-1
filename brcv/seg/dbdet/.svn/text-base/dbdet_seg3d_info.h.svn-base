// This is dbdet/dbdet_seg3d_info.h

#ifndef dbdet_seg3d_info_h_
#define dbdet_seg3d_info_h_

//: 
// \file     
//
// \brief    this class contains information of a 3D segmentation
// including data source, segmentation results. etc.
//
// \author   Nhon Trinh (ntrinh@lems.brown.edu)
// \date     June 2, 2006

#include <vcl_string.h>
#include <vcl_vector.h>
#include <vbl/vbl_ref_count.h>


//: frame struct
struct dbdet_seg3d_info_frame
{
  vcl_string image_file;
  vcl_vector<vcl_string > contour_file_list;
};

class dbdet_seg3d_info: public vbl_ref_count {
public:
  dbdet_seg3d_info();
  ~dbdet_seg3d_info(){};

  // =================== DATA ACCESS FUNCTIONS ==================
  
  //: Image folder
  vcl_string image_folder() const { return this->image_folder_; }
  void set_image_folder( const vcl_string& folder_name )
  {this->image_folder_ = folder_name; }


  //: Contour folder
  vcl_string contour_folder() const {return this->contour_folder_; }
  void set_contour_folder( const vcl_string& folder_name )
  {this->contour_folder_ = folder_name; }

  //: Frame list
  const vcl_vector<dbdet_seg3d_info_frame >& frame_list() const
  {return this->frame_list_; }
  
  //: return number of frames
  int num_frames() const {return this->frame_list_.size();}

  //: Return info on a frame
  dbdet_seg3d_info_frame frame(int i) const
  { return this->frame_list_[i]; }

  
  //: Return reference to a frame
  dbdet_seg3d_info_frame& frame(int i)
  { return this->frame_list_[i]; }

  //: Set frame list
  void set_frame_list(const vcl_vector<dbdet_seg3d_info_frame >& frame_list)
  {this->frame_list_ = frame_list; }

  //: Add a frame
  void add_frame(const dbdet_seg3d_info_frame& frame )
  {this->frame_list_.push_back(frame); }


  // --------------------------------------------------------------------------
  // UTILITY
  // --------------------------------------------------------------------------

  //: Load images from the image folder.
  bool load_images_from_image_folder();

  //: Load the contours using the contour folder and image names
  bool load_contours_from_image_names();

protected:
  vcl_string image_folder_;
  vcl_string contour_folder_;
  vcl_vector<dbdet_seg3d_info_frame > frame_list_;
};

#endif

