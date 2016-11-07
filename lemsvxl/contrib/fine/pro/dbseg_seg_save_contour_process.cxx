// This is contrib/fine/pro/dbseg_seg_save_contour_process.cxx

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


#include "dbseg_seg_save_contour_process.h"

//: Constructor
dbseg_seg_save_contour_process::dbseg_seg_save_contour_process() : bpro1_process()
{
  if( !parameters()->add( "Filename" , "-filename", bpro1_filepath("","con") ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbseg_seg_save_contour_process::~dbseg_seg_save_contour_process()
{
}


//: Clone the process
bpro1_process*
dbseg_seg_save_contour_process::clone() const
{
  return new dbseg_seg_save_contour_process(*this);
}


//: Return the name of the process
vcl_string dbseg_seg_save_contour_process::name()
{
  return "Save Segmentation Contour";
}


//: Call the parent function and reset num_frames_
void
dbseg_seg_save_contour_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbseg_seg_save_contour_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // seg structure required
  to_return.push_back( "seg" );

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbseg_seg_save_contour_process::get_output_type()
{
  return vcl_vector< vcl_string >();
}


//: Returns the number of input frames to this process
int
dbseg_seg_save_contour_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbseg_seg_save_contour_process::output_frames()
{
  return 0;
}


//: Run the process on the current frame
bool
dbseg_seg_save_contour_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbseg_seg_save_contour_process::execute() - "
             << "not exactly one input seg structure \n";
    return false;
  }
  clear_output();

  // get seg structure from the storage class
   dbseg_seg_storage_sptr seg_storage;
  seg_storage.vertical_cast(input_data_[0][0]);
  bpro1_filepath file;
  parameters()->get_value( "-filename" , file );

  return static_execute(seg_storage->get_object(), file);


}


//: Finish
bool
dbseg_seg_save_contour_process::finish()
{
  return true;
}


//static execute
bool dbseg_seg_save_contour_process::static_execute(dbseg_seg_object_base* obj, bpro1_filepath file, int pixel_format) {

    bool saveMiddle = false;
 
    //dbseg_seg_object* object = seg_stg->get_object();
    if (static_cast<dbseg_seg_object<vxl_byte>* >( obj )->get_outlined_regions().size() > 1) {
        vcl_cout << "In dbseg_seg_save_contour_process::execute() - "
             << "more than one region selected \n";
        false;
    }
    if (static_cast<dbseg_seg_object<vxl_byte>* >( obj )->get_outlined_regions().size() == 0) {
        vcl_cout << "In dbseg_seg_save_contour_process::execute() - "
             << "no region selected, saving middle region contour \n";
        saveMiddle = true;
        //return false;
    }


    //get the polygon from the seg structure
    
    int tempID;
    if (saveMiddle) {
        tempID = static_cast<dbseg_seg_object<vxl_byte>* >( obj )->get_pixel_ID(static_cast<dbseg_seg_object<vxl_byte>* >( obj )->get_labeled_image().ni()/2, static_cast<dbseg_seg_object<vxl_byte>* >( obj )->get_labeled_image().nj()/2, 1);
    }
    else {
        tempID = static_cast<dbseg_seg_object<vxl_byte>* >( obj )->get_outlined_regions().front();
    }
    
    
    vgl_polygon<double> poly = *(static_cast<dbseg_seg_object<vxl_byte>* >( obj )->get_outline(tempID).front());
    
 

 
   fstream out(file.path.c_str(), fstream::out);
  //the actual saving 
   out << "CONTOUR" << vcl_endl << "CLOSED" << vcl_endl << poly[0].size() << vcl_endl;
   for (int i = 0; i < poly[0].size(); i++) {
        out << poly[0][i].x() << " ";
        out << poly[0][i].y() << " ";
   }
  out.close();

 vcl_cout << "Contour of region " << tempID << " saved." << vcl_endl;
    return true;
}



