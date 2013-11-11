
#include "dbrl_rpm_rigid_process.h"
#include<bpro1/bpro1_parameters.h>
#include<vsol/vsol_point_2d.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage.h>

//: Constructor
dbrl_rpm_rigid_process::dbrl_rpm_rigid_process(void): bpro1_process()
{
    
    if( !parameters()->add( "Initial temperature" , "-initT" , (float)100) ||
      !parameters()->add( "Annealing Rate" , "-annealrate" , (float) 0.93 ) ) 
     {
        vcl_cerr << "ERROR: Adding parameters in dbrl_rpm_rigid_process::dbrl_rpm_rigid_process()" << vcl_endl;
     }
    
}

//: Destructor
dbrl_rpm_rigid_process::~dbrl_rpm_rigid_process()
{

}


//: Return the name of this process
vcl_string
dbrl_rpm_rigid_process::name()
{
  return "RPM RIGID";
}


//: Return the number of input frame for this process
int
dbrl_rpm_rigid_process::input_frames()
{
  return 2;
}


//: Return the number of output frames for this process
int
dbrl_rpm_rigid_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_rpm_rigid_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_rpm_rigid_process::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbrl_match_set" );
  return to_return;
}


//: Execute the process
bool
dbrl_rpm_rigid_process::execute()
{
  if ( input_data_.size() != 2 ){
    vcl_cout << "In dbrl_rpm_rigid_process::execute() - "
             << "not exactly two input images \n";
    return false;
  }
  clear_output();

  return true;
}
//: Clone the process
bpro1_process*
dbrl_rpm_rigid_process::clone() const
{
  return new dbrl_rpm_rigid_process(*this);
}
    
bool
dbrl_rpm_rigid_process::finish()
{
  return true;
}




