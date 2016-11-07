// This is contrib/fine/pro/dbseg_seg_save_view_process.cxx

//:
// \file

#include <vcl_iostream.h>

#include <bpro1/bpro1_parameters.h>
#include <structure/dbseg_seg_storage.h>
#include <structure/dbseg_seg_storage_sptr.h>
#include <structure/dbseg_seg_object.h>

#include <vgui/vgui_dialog.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>


#include "dbseg_seg_save_view_process.h"

//: Constructor
dbseg_seg_save_view_process::dbseg_seg_save_view_process() : bpro1_process()
{
  if( !parameters()->add( "Filename" , "-filename", bpro1_filepath("","*") )  ||
      !parameters()->add( "Depth Level" , "-depth", (int)1 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbseg_seg_save_view_process::~dbseg_seg_save_view_process()
{
}


//: Clone the process
bpro1_process*
dbseg_seg_save_view_process::clone() const
{
  return new dbseg_seg_save_view_process(*this);
}


//: Return the name of the process
vcl_string dbseg_seg_save_view_process::name()
{
  return "Save Segmentation View";
}


//: Call the parent function and reset num_frames_
void
dbseg_seg_save_view_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbseg_seg_save_view_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "seg" );

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbseg_seg_save_view_process::get_output_type()
{
  return vcl_vector< vcl_string >();
}


//: Returns the number of input frames to this process
int
dbseg_seg_save_view_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbseg_seg_save_view_process::output_frames()
{
  return 0;
}


//: Run the process on the current frame
bool
dbseg_seg_save_view_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbseg_seg_save_view_process::execute() - "
             << "not exactly one input seg structure \n";
    return false;
  }
  clear_output();

  // get seg structure from the storage class
   dbseg_seg_storage_sptr seg_storage;
  seg_storage.vertical_cast(input_data_[0][0]);


  int depth;
  parameters()->get_value( "-depth" , depth );
    //seg_object* object = seg_stg->get_object();
    if (depth < 1 || depth > static_cast<dbseg_seg_object<vxl_byte>* >( seg_storage->get_object() )->get_max_depth()) {
        vcl_cout << "In dbseg_seg_save_view_process::execute() - "
             << "depth level not available \n";
        return false;
    }

    //get the image from the seg structure
    vil_image_view<vxl_byte> save_image = static_cast<dbseg_seg_object<vxl_byte>* >( seg_storage->get_object() )->get_display(depth);
 

  bpro1_filepath file;
  parameters()->get_value( "-filename" , file );
 
  vil_save(save_image,file.path.c_str());
 

  return true;

}


//: Finish
bool
dbseg_seg_save_view_process::finish()
{
  return true;
}




