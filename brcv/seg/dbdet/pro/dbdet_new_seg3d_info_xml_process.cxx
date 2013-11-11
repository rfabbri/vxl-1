// This is dbdet/pro/dbdet_new_seg3d_info_xml_process.cxx

//:
// \file

#include "dbdet_new_seg3d_info_xml_process.h"

#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_file.h>
#include <bpro1/bpro1_parameters.h>
#include <dbdet/dbdet_seg3d_info.h>
#include <dbdet/dbdet_seg3d_info_sptr.h>
#include <dbdet/xio/dbdet_xio_seg3d_info.h>





// ----------------------------------------------------------------------------
//: Constructor
dbdet_new_seg3d_info_xml_process::
dbdet_new_seg3d_info_xml_process() : bpro1_process()
{
  if( !this->parameters()->add( "xml file <filename...>" , "-xml_filename", 
    bpro1_filepath("","*") ) ||
    !this->parameters()->add("image folder <foldername ... >", "-image_folder",
    bpro1_filepath("", "*") ) ||
    !this->parameters()->add("contour folder <foldername ... >", "-contour_folder",
    bpro1_filepath("", "*") ) ||
    !this->parameters()->add("has contours in repository?", "-has_contours", false ) 
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


// ----------------------------------------------------------------------------
//: Destructor
dbdet_new_seg3d_info_xml_process::
~dbdet_new_seg3d_info_xml_process()
{
}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbdet_new_seg3d_info_xml_process::
clone() const
{
  return new dbdet_new_seg3d_info_xml_process(*this);
}

// ----------------------------------------------------------------------------
//: Return the name of the process
vcl_string dbdet_new_seg3d_info_xml_process::
name()
{
  return "new seg3d_info .xml";
}


// ----------------------------------------------------------------------------
//: Returns the number of input frames to this process
int dbdet_new_seg3d_info_xml_process::
input_frames()
{
  return 0;
}


// ----------------------------------------------------------------------------
//: Returns the number of output frames from this process
int dbdet_new_seg3d_info_xml_process::
output_frames()
{
  return 0;
}



// ----------------------------------------------------------------------------
//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbdet_new_seg3d_info_xml_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;

  // no input type required
  to_return.clear();
  return to_return;
}


// ----------------------------------------------------------------------------
//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbdet_new_seg3d_info_xml_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.clear();
  return to_return;
}


// ----------------------------------------------------------------------------
//: Run the process on the current frame
bool dbdet_new_seg3d_info_xml_process::
execute()
{
  this->clear_output();
  
  // extract data from user input
  bpro1_filepath xml_path;
  this->parameters()->get_value( "-xml_filename" , xml_path );

  bpro1_filepath image_folder;
  this->parameters()->get_value( "-image_folder" , image_folder );

  bpro1_filepath contour_folder;
  this->parameters()->get_value( "-contour_folder" , contour_folder );

  //construct a seg3d_info object
  dbdet_seg3d_info_sptr seg3d = new dbdet_seg3d_info();
  seg3d->set_image_folder(image_folder.path);
  seg3d->set_contour_folder(contour_folder.path);

  seg3d->load_images_from_image_folder();
  seg3d->load_contours_from_image_names();

  //// get the list of files
  //vcl_vector<vcl_string> filenames;
  //for ( vul_file_iterator fit = image_folder.path + "/*.*"; fit; ++fit )
  //{
  //  // check to see if file is a directory.
  //  if (vul_file::is_directory(fit()))
  //    continue;

  //  // ignore hidden files (e.g. Thumbs.db)
  //  if(vul_file::extension(fit()).size() < 4)
  //    continue;
  //  
  //  filenames.push_back(fit());
  //}
  //// Sort - because the file iterator uses readdir() it does not
  ////        iterate over files in alphanumeric order 
  //vcl_sort(filenames.begin(),filenames.end());


  //// Load the images
  //for ( unsigned i=0; i<filenames.size(); ++i )
  //{
  //  vcl_string filename = vul_file::strip_directory(filenames[i].c_str());
  //  dbdet_seg3d_info_frame frame;
  //  frame.image_file = filename;
  //  frame.contour_file_list.clear();
  //  seg3d->add_frame(frame);
  //}


  // write data to an .xml file
  vcl_ofstream xml_file(xml_path.path.c_str());
  x_write(xml_file, seg3d);
  xml_file.close();

  return true;
}


// ----------------------------------------------------------------------------
//: Finish
bool dbdet_new_seg3d_info_xml_process::
finish() 
{
  return true;
}
