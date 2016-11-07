// This is contrib/fine/pro/dbseg_seg_save_process.cxx

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


#include "dbseg_seg_save_process.h"

//: Constructor
dbseg_seg_save_process::dbseg_seg_save_process() : bpro1_process()
{
  if( !parameters()->add( "Filename" , "-filename", bpro1_filepath("","*") ) )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbseg_seg_save_process::~dbseg_seg_save_process()
{
}


//: Clone the process
bpro1_process*
dbseg_seg_save_process::clone() const
{
  return new dbseg_seg_save_process(*this);
}


//: Return the name of the process
vcl_string dbseg_seg_save_process::name()
{
  return "Save Segmentation Structure";
}


//: Call the parent function and reset num_frames_
void
dbseg_seg_save_process::clear_output()
{
  bpro1_process::clear_output();
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string >
dbseg_seg_save_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  // seg type required
  to_return.push_back( "seg" );

  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string >
dbseg_seg_save_process::get_output_type()
{
  return vcl_vector< vcl_string >();
}


//: Returns the number of input frames to this process
int
dbseg_seg_save_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbseg_seg_save_process::output_frames()
{
  return 0;
}


//: Run the process on the current frame
bool
dbseg_seg_save_process::execute()
{
    vul_timer t;
    t.mark();
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbseg_seg_save_process::execute() - "
             << "not exactly one input seg structure \n";
    return false;
  }
  clear_output();

  // get seg structure from the storage class
   dbseg_seg_storage_sptr seg_stg;
  seg_stg.vertical_cast(input_data_[0][0]);

    //seg_object* object = seg_stg->get_object();
 
  bpro1_filepath file;
  parameters()->get_value( "-filename" , file );
  static_execute(seg_stg->get_object(), file);
  vcl_cout << "Struture Saved. Time: " << t.real()/1000 << " seconds." << vcl_endl;
  //vil_save(save_image,file.path.c_str());
 

  return true;

}


//: Finish
bool
dbseg_seg_save_process::finish()
{
  return true;
}

//static execute
void dbseg_seg_save_process::static_execute(dbseg_seg_object_base* obj, bpro1_filepath file, int pixel_format) {
  fstream out(file.path.c_str(), fstream::out);
  //the actual saving function
  //seg_stg->get_object()->save_text(out);
  /*vsl_b_ostream out2(&out);
    vsl_b_write(out2, 1);
    dbseg_seg_object<vxl_byte>* obj2 = static_cast<dbseg_seg_object<vxl_byte>*>(obj);
    vsl_b_write(out2, obj2->get_image().nplanes());
    list<int> tempValidIDs = obj2->get_tree()->get_valid_IDs();
    vsl_b_write(out2, tempValidIDs.size());
    list<int>::iterator i;
    vcl_vector<int> tempIDsVector;
    for (i = tempValidIDs.begin(); i != tempValidIDs.end(); ++i) {
        //vsl_b_write(os, static_cast<short>(rand()%200));
        tempIDsVector.push_back(*i);
    }
    vsl_b_write(out2, tempIDsVector);
  //vsl_b_write(out2, static_cast<dbseg_seg_object<vxl_byte>*>(obj) );*/
    
  dbseg_seg_object<vxl_byte>* obj2 = static_cast<dbseg_seg_object<vxl_byte>*>(obj);
    //vcl_ostream out2(;    
  obj2->save_text(out);
  out.close();


}




