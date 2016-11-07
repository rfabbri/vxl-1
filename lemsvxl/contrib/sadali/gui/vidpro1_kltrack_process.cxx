// This is brl/vidpro1/process/vidpro1_kl_process.cxx

//:
// \file

#include "vidpro1_kltrack_process.h"
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vil/vil_math.h>
#include<bpro1/bpro1_parameters.h>
#include<vsol/vsol_point_2d.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage.h>
#include<vtol/vtol_vertex_2d.h>
#include<vtol/vtol_vertex_2d_sptr.h>

//: Constructor
vidpro1_kltrack_process::vidpro1_kltrack_process(void): bpro1_process(),kl_params(),kl_points(NULL)
{
    
    if( !parameters()->add( "No of Points" , "-klnumpoints" , (int)100) ||
      !parameters()->add( "Search Range" , "-klrange" , (int) 15 ) ||
          !parameters()->add( "No. of frames" , "-framenum", (int) 2 )
          ) 
     {
        vcl_cerr << "ERROR: Adding parameters in vidpro1_kl_process::vidpro1_kl_process()" << vcl_endl;
     }
    else
    {
        
    }
}

//: Destructor
vidpro1_kltrack_process::~vidpro1_kltrack_process()
{
    delete(kl_points);
}


//: Return the name of this process
vcl_string
vidpro1_kltrack_process::name()
{
  return "Kanade-Lucas Tracking";
}


//: Return the number of input frame for this process
int
vidpro1_kltrack_process::input_frames()
{
  int num_frames;
  parameters()->get_value("-framenum", num_frames);

  return num_frames;
}


//: Return the number of output frames for this process
int
vidpro1_kltrack_process::output_frames()
{
  return input_frames();
}


//: Provide a vector of required input types
vcl_vector< vcl_string > vidpro1_kltrack_process::get_input_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "image" );
 
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > vidpro1_kltrack_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );

  return to_return;
}


//: Execute the process
bool
vidpro1_kltrack_process::execute()
{
 int num_of_frames;
  parameters()->get_value("-framenum", num_of_frames);
  if ( input_data_.size() !=  num_of_frames ){
    vcl_cout << "In vidpro1_kltrack_process::execute() - "
             << "not exactly one input image \n";
    return false;
  }
  clear_output();
 

  // get image from the storage class
  vcl_vector<vidpro1_image_storage_sptr> frame_image(num_of_frames);
  vcl_vector<vil_image_resource_sptr> images(num_of_frames);
  vcl_vector<vidpro1_image_storage_sptr>::const_iterator frame_iter= frame_image.begin();
  for ( int i = 0; (i<num_of_frames)&&(frame_iter!=frame_image.end() ); i++)
     {
     (frame_image[i]).vertical_cast(input_data_[i][0]);
     images[i]= (*frame_iter)->get_image();
     frame_iter++;
     }
  
  
  parameters()->get_value("-klnumpoints",kl_params.numpoints);
  parameters()->get_value("-klrange",kl_params.search_range);

  kl_points=new vgel_kl(kl_params);
  
  
  vgel_multi_view_data_vertex_sptr matched_points;
  matched_points = new vgel_multi_view_data<vtol_vertex_2d_sptr>(num_of_frames);
  kl_points->match_sequence(images,matched_points);
  
  vcl_vector<vsol_spatial_object_2d_sptr> points_stor;
  vcl_vector<vtol_vertex_2d_sptr> points_prev;
  vcl_vector<vtol_vertex_2d_sptr> points_last;
  vcl_vector<vtol_vertex_2d_sptr> points;

   // create the output storage class
  vcl_vector<vidpro1_vsol2D_storage_sptr> output_vsol(num_of_frames);
  for ( int i = 0; i<num_of_frames; i++)
   output_vsol[i] = vidpro1_vsol2D_storage_new();

  matched_points->get(0,num_of_frames-1, points_prev, points_last);
  assert( points_prev.size()>0 );
   assert( points_last.size()>0 );
  vcl_vector<vcl_vector<vsol_spatial_object_2d_sptr > > first_pass;
  vtol_vertex_2d_sptr point_matchee;
 

  vcl_vector< vsol_spatial_object_2d_sptr > klt_points;
  for( int i = 0 ; i < points_last.size() ; i++ ) 
  {
  point_matchee = points_last[i];
      for(int j =num_of_frames-2 ; j>=0 ; j--) {
          vtol_vertex_2d_sptr feat_candidate;
          
          if (!matched_points->get_pred_match(j, point_matchee, feat_candidate))
          {

              klt_points.clear();
              break;
          }
          vsol_spatial_object_2d_sptr point = (new vsol_point_2d  (point_matchee->x(), point_matchee->y() ) )->cast_to_spatial_object();
          klt_points.push_back(point);
          point_matchee = feat_candidate; 

      }
      if ((klt_points.size()))
          {
          klt_points.push_back((new vsol_point_2d(point_matchee->x(), point_matchee->y() ) )->cast_to_spatial_object() );
          first_pass.push_back(klt_points);
          
          }
          
      klt_points.clear();
  }

  assert(first_pass.size()!=0);
  for (int i = 0; i<first_pass.size(); i++)
  {
  vcl_vector <vsol_spatial_object_2d_sptr> temp = first_pass[i];
  assert(temp.size()==num_of_frames);
  for (int j = 0; j<temp.size(); j++)
   output_vsol[temp.size()-j-1]->add_object(temp[j],"KL feature  point");

  }

  

  

  for ( int i = 0; i<num_of_frames; i++)
     output_data_[i].push_back(output_vsol[i]);

  return true;  
}
//: Clone the process
bpro1_process*
vidpro1_kltrack_process::clone() const
{
  return new vidpro1_kltrack_process(*this);
}
    
bool
vidpro1_kltrack_process::finish()
{
  return true;
}




