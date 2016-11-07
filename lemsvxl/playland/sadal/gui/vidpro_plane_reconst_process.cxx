// This is playland/sadal/vidpro_plane_reconst_process.cxx

//:
// \file

#include "vidpro_plane_reconst_process.h"
#include <vidpro/vidpro_parameters.h>

// include storage classes needed
// such as ...
#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage_sptr.h>
#include <vidpro/storage/vidpro_vtol_storage.h>
#include <vidpro/storage/vidpro_vtol_storage_sptr.h>

// other includes needed

#include <blem/playland/sadal/bprt/bprt_plane_reconst.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>



//: Constructor
vidpro_plane_reconst_process::vidpro_plane_reconst_process()
{
  // Set up the parameters for this process
  if( !parameters()->add( "No of sample points per curve" , "-p1" , (int)100 )
          ||(
      !parameters()->add( "Std of RANSAC algorithm" , "-p2"  , 5.0f ) 
          )
          )
  {
    vcl_cerr << "ERROR: Adding parameters in vidpro_plane_reconst_process::vidpro_plane_reconst_process()" << vcl_endl;
  }
}


//: Destructor
vidpro_plane_reconst_process::~vidpro_plane_reconst_process()
{
}

vidpro_process* vidpro_plane_reconst_process::clone() const
{
    return new vidpro_plane_reconst_process(*this);
}


//: Return the name of this process
vcl_string
vidpro_plane_reconst_process::name()
{
  return "Calc Plane Parameters";
}


//: Return the number of input frame for this process
int
vidpro_plane_reconst_process::input_frames()
{
  // input from this frame and the previous frame
  return 1;
}


//: Return the number of output frames for this process
int
vidpro_plane_reconst_process::output_frames()
{
  // output to this frame only
  return 0;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > 
vidpro_plane_reconst_process::get_input_type()
{
  // this process looks for an image and vsol2D storage class
  // at each input frame
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vtol" );
  to_return.push_back( "vtol" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > 
vidpro_plane_reconst_process::get_output_type()
{  
  // this process produces a vsol2D storage class
  vcl_vector<vcl_string > to_return;
 // to_return.push_back( "vsol2D" );
  return to_return;
}


//: Execute the process
bool
vidpro_plane_reconst_process::execute()
{
  // verify that the number of input frames is correct
  if ( input_data_.size() != 1 ){
    vcl_cout << "In vidpro_plane_reconst_process::execute() - not exactly two"
             << " input frames" << vcl_endl;
    return false;
  }
  clear_output();

  // get images from the storage classes
  vidpro_vtol_storage_sptr curr_frame_edges; 
  vidpro_vtol_storage_sptr curr_frame_edges2;
  curr_frame_edges.vertical_cast(input_data_[0][0]);
  curr_frame_edges2.vertical_cast(input_data_[0][1]);
//  prev_frame_image.vertical_cast(input_data_[1][0]);
  
  vcl_vector< vtol_edge_2d_sptr > datainput;
  vcl_vector< vtol_edge_2d_sptr > datainput2;
  vtol_topology_object_sptr intertol;


  
 vcl_set<vtol_topology_object_sptr>::const_iterator itr;
 for(itr=curr_frame_edges->begin(); itr!=curr_frame_edges->end();++itr)
 {
         datainput.push_back((*itr)->cast_to_edge()->cast_to_edge_2d()); 
                 

 }
  
 vcl_set<vtol_topology_object_sptr>::const_iterator itr2;
 for(itr2=curr_frame_edges->begin(); itr2!=curr_frame_edges->end();++itr2)
 {
         datainput2.push_back((*itr2)->cast_to_edge()->cast_to_edge_2d()); 
                 

 }

  // get vsol2D from the storage classes
  
 
  //get parameters
  int param1;
  float param2;
  parameters()->get_value( "-p1" , param1 );
  parameters()->get_value( "-p2" , param2 );
  bprt_plane_reconst Reconstructor(datainput,datainput2,param1,param2);
  Reconstructor.calibrate();
  vnl_double_3x3 K;
  vnl_double_3x3 R;
  vnl_double_3   t;
  K[0][0] = 841.3804; K[0][1] = 0;        K[0][2] = 331.0916;
  K[1][0] = 0;        K[1][1] = 832.7951; K[1][2] = 221.5451;
  K[2][0] = 0;        K[2][1] = 0;        K[2][2] = 1;

  R[0][0] = 1/2;               R[0][1] = 0;        R[0][2] = vcl_sqrt(3.0f)/2; 
  R[1][0] = 0;                 R[1][1] = 1;        R[1][2] = 0;
  R[2][0] = vcl_sqrt(3.0f)/-2;        R[2][1] = 0;        R[2][2] = 1/2;

  t[0]=560;
  t[1]=10;
  t[2]=50;


  //=========================================
  // Do something here with all of this data
  //=========================================

 
 //Reconstructor.set_cal_params(K,K,R,t);

 Reconstructor.comp_param();
 double p1,p2,p3;
 Reconstructor.param_.get_params(p1,p2,p3);
 vcl_cout<<p1<<"  "<<p2<< "   "<<p3<<"\n";
 
  
 

  // create the output storage class
 // vidpro_vsol2D_storage_sptr output_vsol2D = vidpro_vsol2D_storage_new();
 // vcl_vector<vsol_spatial_object_2d_sptr> point=Reconstructor.get_vsol_point_2ds(1);
 // output_vsol2D->add_vsol_sptr(point);
 // output_data_[0].push_back(output_vsol2D);
  
  return true;
}


//: Finish
bool
vidpro_plane_reconst_process::finish()
{
  // I'm not really sure what this is for, ask Amir
  // - mleotta
  return true;
}
