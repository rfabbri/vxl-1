// This is contrib/fine/pro/dbseg_seg_save_binimage_process.cxx

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


#include "dbseg_seg_save_binimage_process.h"

//: Constructor
dbseg_seg_save_binimage_process::dbseg_seg_save_binimage_process() : bpro1_process()
{
  if( !parameters()->add( "Filename" , "-filename", bpro1_filepath("","png") ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbseg_seg_save_binimage_process::~dbseg_seg_save_binimage_process()
{
}


//: Clone the process
bpro1_process*
dbseg_seg_save_binimage_process::clone() const
{
  return new dbseg_seg_save_binimage_process(*this);
}


//: Return the name of the process
vcl_string dbseg_seg_save_binimage_process::name()
{
  return "Save Segmentation Binary Image";
}


//: Call the parent function and reset num_frames_
void
dbseg_seg_save_binimage_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbseg_seg_save_binimage_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // image type required
  to_return.push_back( "seg" );

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbseg_seg_save_binimage_process::get_output_type()
{
  return vcl_vector< vcl_string >();
}


//: Returns the number of input frames to this process
int
dbseg_seg_save_binimage_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbseg_seg_save_binimage_process::output_frames()
{
  return 0;
}


//: Run the process on the current frame
bool
dbseg_seg_save_binimage_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbseg_seg_save_binimage_process::execute() - "
             << "not exactly one input seg structure \n";
    return false;
  }
  clear_output();

  // get seg structure from the storage class
   dbseg_seg_storage_sptr seg_stg;
  seg_stg.vertical_cast(input_data_[0][0]);

    bool saveMiddle = false;
    bool multiple = false;
    dbseg_seg_object<vxl_byte>* object = static_cast<dbseg_seg_object<vxl_byte>*>(seg_stg->get_object());
    if (object->get_outlined_regions().size() > 1) {
        //vcl_cout << "In dbseg_seg_save_binimage_process::execute() - "
        //     << "more than one region selected \n";
        //return false;
        multiple = true;

    }
    if (object->get_outlined_regions().size() == 0) {
        vcl_cout << "In dbseg_seg_save_binimage_process::execute() - "
             << "no region selected, doing nothing \n";
        saveMiddle = true;
        return false;
    }


    //get the polygon from the seg structure
    
    int tempID;
    /*if (saveMiddle) {
        tempID = object->get_pixel_ID(object->get_labeled_image().ni()/2, object->get_labeled_image().nj()/2, 1);
    }*/
    
    
    int ni = object->get_image().ni();
    int nj = object->get_image().nj();
    vil_image_view<vxl_byte> binaryImage = vil_image_view<vxl_byte>(ni, nj);

    for (int i = 0; i < ni; i++) {
        for (int j = 0; j < nj; j++) {
            binaryImage(i,j) = 0;
        }
    }

    list<int> tempOutlinedRegions = object->get_outlined_regions();
    list<int>::iterator i;
    for (i = tempOutlinedRegions.begin(); i != tempOutlinedRegions.end(); ++i) {
        tempID = *i; //object->get_outlined_regions().front();

        int left, right, top, bottom;
        left = object->get_object_list()[tempID]->get_left();
        right = object->get_object_list()[tempID]->get_right();
        top = object->get_object_list()[tempID]->get_top();
        bottom = object->get_object_list()[tempID]->get_bottom();
        vgl_polygon<double> poly = *(object->get_outline(tempID).front());    
     
        for (int i = left; i <= right; i++) {
            for (int j = top; j <= bottom; j++) {
                if (object->get_pixel_ID(i, j, 1)==tempID) {
                //bool test = poly.contains(i,j);
                //if (test) {
                    binaryImage(i,j) = 255;
                }
            }
        }
    }
  bpro1_filepath file;
  parameters()->get_value( "-filename" , file );
 vil_save(binaryImage, file.path.c_str());
   /*fstream out(file.path.c_str(), fstream::out);
  //the actual saving 
   out << "binimage" << vcl_endl << "CLOSED" << vcl_endl << poly[0].size() << vcl_endl;
   for (int i = 0; i < poly[0].size(); i++) {
        out << poly[0][i].x() << " ";
        out << poly[0][i].y() << " ";
   }
  out.close();*/

 vcl_cout << "binimage of region " << tempID << " saved." << vcl_endl;

  return true;

}


//: Finish
bool
dbseg_seg_save_binimage_process::finish()
{
  return true;
}




