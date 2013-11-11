
#include "dbrl_denoise_point_set_process.h"
#include<bpro1/bpro1_parameters.h>
#include<vsol/vsol_point_2d.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage.h>
#include<vnl/vnl_vector_fixed.h>
#include "dbrl_id_point_2d_storage.h"
#include "dbrl_id_point_2d_storage_sptr.h"
#include <vcl_cmath.h>
#include <vcl_cstdlib.h> // for rand()
#include <vnl/vnl_sample.h>

//: Constructor
dbrl_denoise_point_set_process::dbrl_denoise_point_set_process(void): bpro1_process()
{
    
    if( !parameters()->add( "Radius" , "-rad" , (int)5) ||
        !parameters()->add( "No of points (threshold)" , "-npoints" , (int)2 ) 
        ) 
     {
        vcl_cerr << "ERROR: Adding parameters in dbrl_denoise_point_set_process::dbrl_denoise_point_set_process()" << vcl_endl;
     }
    
 
}

//: Destructor
dbrl_denoise_point_set_process::~dbrl_denoise_point_set_process()
{

}


//: Return the name of this process
vcl_string
dbrl_denoise_point_set_process::name()
{
  return "Denoise Point set";
}


//: Return the number of input frame for this process
int
dbrl_denoise_point_set_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbrl_denoise_point_set_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_denoise_point_set_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "dbrl_id_point_2d" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_denoise_point_set_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbrl_id_point_2d" );
  return to_return;
}


//: Execute the process
bool
dbrl_denoise_point_set_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbrl_denoise_point_set_process::execute() - "
             << "not exactly two input images \n";
    return false;
  }
  //clear_output();
  dbrl_id_point_2d_storage_sptr frame_pts;
  frame_pts.vertical_cast(input_data_[0][0]);




  static int npoints=2;
  static int rad=5;



  parameters()->get_value("-rad",rad);
  parameters()->get_value("-npoints",npoints);

  vcl_vector<dbrl_id_point_2d_sptr> idpoints;
  vcl_vector<dbrl_id_point_2d_sptr> origpoints=frame_pts->points();

  for(int i=0;i<static_cast<int>(origpoints.size());i++)
      {
        int counter=0;
        for(int j=0;j<static_cast<int>(origpoints.size());j++)
            {
                if((vcl_fabs(origpoints[j]->x()-origpoints[i]->x())<rad ) && (vcl_fabs(origpoints[j]->y()-origpoints[i]->y())<rad))
                    {
                        counter++;
                    }
            }

        if(counter>npoints)
            idpoints.push_back(origpoints[i]);
      }

  vcl_cout<<"\n orid size is "<<origpoints.size();
 vcl_cout<<"\n id size is "<<idpoints.size();   
 dbrl_id_point_2d_storage_sptr output=dbrl_id_point_2d_storage_new(idpoints);
 output_data_[0].push_back(output);


  return true;  
}
//: Clone the process
bpro1_process*
dbrl_denoise_point_set_process::clone() const
{
  return new dbrl_denoise_point_set_process(*this);
}
    
bool
dbrl_denoise_point_set_process::finish()
{
  return true;
}




