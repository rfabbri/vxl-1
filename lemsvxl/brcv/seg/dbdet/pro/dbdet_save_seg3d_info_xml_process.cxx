// This is dbdet/pro/dbdet_save_seg3d_info_xml_process.cxx

//:
// \file

#include "dbdet_save_seg3d_info_xml_process.h"
#include <vul/vul_file.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>

#include <bpro1/bpro1_parameters.h>
#include <dbsol/dbsol_file_io.h>
#include <dbdet/xio/dbdet_xio_seg3d_info.h>
#include <dbdet/pro/dbdet_seg3d_info_storage.h>
#include <dbdet/pro/dbdet_seg3d_info_storage_sptr.h>


// ----------------------------------------------------------------------------
//: Constructor
dbdet_save_seg3d_info_xml_process::
dbdet_save_seg3d_info_xml_process() : bpro1_process()
{
  //
  if( !parameters()->add( "xml file <filename...>" , "-xml_filename", 
    bpro1_filepath("","*") ) ||
    !parameters()->add( "save contours in a new folder?" , "-save_new", 
    false ) ||
    !parameters()->add( "new contour folder <folder ... >" , "-new_contour_folder", 
    bpro1_filepath("","*")) ||
    !parameters()->add( "contour suffix" , "-contour_suffix", 
    vcl_string(""))
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}

// ----------------------------------------------------------------------------
//: Destructor
dbdet_save_seg3d_info_xml_process::
~dbdet_save_seg3d_info_xml_process()
{
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbdet_save_seg3d_info_xml_process::
clone() const
{
  return new dbdet_save_seg3d_info_xml_process(*this);
}

// ----------------------------------------------------------------------------
//: Return the name of the process
vcl_string dbdet_save_seg3d_info_xml_process::
name()
{
  return "save seg3d_info .xml";
}


// ----------------------------------------------------------------------------
//: Returns the number of input frames to this process
int dbdet_save_seg3d_info_xml_process::
input_frames()
{
  return 1;
}


// ----------------------------------------------------------------------------
//: Returns the number of output frames from this process
int dbdet_save_seg3d_info_xml_process::
output_frames()
{
  return 0;
}



// ----------------------------------------------------------------------------
//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbdet_save_seg3d_info_xml_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back("seg3d_info");
  return to_return;
}


// ----------------------------------------------------------------------------
//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbdet_save_seg3d_info_xml_process::
get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.clear();
  return to_return;
}


// ----------------------------------------------------------------------------
//: Run the process on the current frame
bool dbdet_save_seg3d_info_xml_process::
execute()
{
  
  this->clear_output();
  
  bpro1_filepath xml_path;
  this->parameters()->get_value( "-xml_filename" , xml_path );    
  vcl_string xml_filename = xml_path.path;
  
  dbdet_seg3d_info_storage_sptr seg3d_info_storage;
  seg3d_info_storage.vertical_cast(input_data_[0][0]);

  dbdet_seg3d_info_sptr seg3d_info = seg3d_info_storage->seg3d_info();

  // determine whether the contours are saved in a new folder
  bool save_new = false;
  this->parameters()->get_value("-save_new", save_new);

  vcl_string con_file_suffix("");
  this->parameters()->get_value("-contour_suffix", con_file_suffix);


  if (save_new)
  {
    // get the new contour folder
    bpro1_filepath contour_path;
    this->parameters()->get_value("-new_contour_folder", contour_path);
    seg3d_info->set_contour_folder(contour_path.path);  
  }

  // update the contours
  for (int i=0; i<seg3d_info->num_frames(); ++i)
  {
    vidpro1_vsol2D_storage_sptr vsol_storage = seg3d_info_storage->vsol_storage_at_frame(i);
    vcl_vector<vsol_spatial_object_2d_sptr > all_vsols = vsol_storage->all_data();

    dbdet_seg3d_info_frame * cur_frame = &seg3d_info->frame(i);

    // update contour info 
    cur_frame->contour_file_list.clear();
    int contour_count = 0;
    for (unsigned k=0; k<all_vsols.size(); ++k)
    {
      // next contour name
      vcl_ostringstream ostring;
      ostring << vul_file::strip_extension(cur_frame->image_file) << "_" 
        << con_file_suffix
        << contour_count << ".con";
      
      vcl_string contour_name = ostring.str();

      vsol_spatial_object_2d_sptr obj = all_vsols[k];
      if (obj->cast_to_curve())
      {
        if (obj->cast_to_curve()->cast_to_polyline())
        {
          vsol_polyline_2d_sptr polyline = obj->cast_to_curve()->cast_to_polyline();
          ++contour_count;

          

          // save the contour
          vcl_string file_fullname = seg3d_info->contour_folder() + "/" + contour_name;
          dbsol_save_con_file(file_fullname.c_str(), polyline);

          // update to cur_frame
          cur_frame->contour_file_list.push_back(contour_name);
        }
      }
      else if (obj->cast_to_region())
      {
        if (obj->cast_to_region()->cast_to_polygon())
        {
          vsol_polygon_2d_sptr polygon = obj->cast_to_region()->cast_to_polygon();
          ++contour_count;

          
          // save the contour
          vcl_string file_fullname = seg3d_info->contour_folder() + "/" + contour_name;
          dbsol_save_con_file(file_fullname.c_str(), polygon);

          // update to cur_frame
          cur_frame->contour_file_list.push_back(contour_name);
        }
      }
    }
  }

  // update xml file
  vcl_ofstream xml_filestream(xml_filename.c_str());
  x_write(xml_filestream, seg3d_info);
  xml_filestream.close();
  return true;
}


// ----------------------------------------------------------------------------
//: Finish
bool dbdet_save_seg3d_info_xml_process::
finish() 
{
  return true;
}








