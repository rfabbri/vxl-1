// This is contrib/sadali/vidpro1_my_process.cxx

//:
// \file

#include "vidpro1_combine_storage_process.h"
#include <bpro1/bpro1_parameters.h>

// include storage classes needed
// such as ...

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

// other includes needed

#include <bprt/bprt_plane_reconst.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>







//: Constructor
vidpro1_combine_storage_process::vidpro1_combine_storage_process()
{
  // Set up the parameters for this process
  if 
      (!parameters()->add( "Number of storage objects to combine" , "-p1" , (int)2  ) ) 
          
           
          
          
          
  {
    vcl_cerr << "ERROR: Adding parameters in vidpro1_combine_storage_process::vidpro1_combine_storage_process()" << vcl_endl;
  }
}



//: Destructor
vidpro1_combine_storage_process::~vidpro1_combine_storage_process()
{
}


//: Return the name of this process
vcl_string
vidpro1_combine_storage_process::name()
{
  return "Combine Storage";
}


bpro1_process *
vidpro1_combine_storage_process::clone() const
{
    return new vidpro1_combine_storage_process(*this);

};
//: Return the number of input frame for this process
int
vidpro1_combine_storage_process::input_frames()
{
  // input from this frame 
        return 1;
}


//: Return the number of output frames for this process
int
vidpro1_combine_storage_process::output_frames()
{
  // output to this frame only
return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > 
vidpro1_combine_storage_process::get_input_type()
{
  // this process looks for  vsol2D storage classes
  // at each input frame
        int param1;
         parameters()->get_value( "-p1" , param1 );
  vcl_vector< vcl_string > to_return;
  for (int i = 0; i<param1; i++)
  to_return.push_back( "vsol2D" );
  
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > 
vidpro1_combine_storage_process::get_output_type()
{  
  // this process produces a vsol2D storage class
        
  vcl_vector<vcl_string > to_return;
 
  to_return.push_back( "vsol2D" );
  
  return to_return;
}


//: Execute the process
bool
vidpro1_combine_storage_process::execute()
{
  // verify that the number of input frames is correct
  if ( input_data_.size() != 1 ){
    vcl_cout << "In vidpro1_combine_storage_process::execute() - not exactly two"
             << " input frames" << vcl_endl;
    return false;
  }
  clear_output();
 

  int   param1;
   parameters()->get_value( "-p1" , param1 );
  // get vsol2d from the storage classes
  vidpro1_vsol2D_storage_sptr curr_frame_corn; 
  vcl_vector < vsol_spatial_object_2d_sptr > origpts;
  for (int i = 0; i<param1; i++)
  {
          curr_frame_corn.vertical_cast(input_data_[0][i]);
          vcl_vector<vcl_string> dataname = curr_frame_corn->groups();
      
          
      for (int nameindex=0; nameindex<dataname.size(); nameindex++)
      {
                  
          vcl_vector < vsol_spatial_object_2d_sptr > curr_group = curr_frame_corn->data_named(dataname[nameindex]);
          for (int m = 0; m<curr_group.size(); m++)
              origpts.push_back(curr_group[m]);
      }

  }

       
  
 

  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol2D = vidpro1_vsol2D_storage_new();
    
  output_vsol2D->add_objects(origpts,"Combined storage");
  output_data_[0].push_back(output_vsol2D);
  
   
  
  return true;
}


//: Finish
bool
vidpro1_combine_storage_process::finish()
{
  // I'm not really sure what this is for, ask Amir
  // - mleotta
  return true;
}
