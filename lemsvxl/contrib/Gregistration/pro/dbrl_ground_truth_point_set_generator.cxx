
#include "dbrl_ground_truth_point_set_generator.h"
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
dbrl_ground_truth_point_set_generator::dbrl_ground_truth_point_set_generator(void): bpro1_process(),tx(0),ty(0)
{
    
    if( !parameters()->add( "No of Outliers" , "-numout" , (int)20) ||
        !parameters()->add( "% of points to be dropped" , "-ppoints" , (int)10 )||
        !parameters()->add( "Noise(sigma) to be added to rest of the points" , "-nsigma" , (float) 0.5 )||
        !parameters()->add( "Translation x (min)" , "-txmin" , (float) 0.0 )||
        !parameters()->add( "Translation x (max)" , "-txmax" , (float) 10.0 )||
        !parameters()->add( "Translation y (min)" , "-tymin" , (float) 0.0 )||
        !parameters()->add( "Translation y (max)" , "-tymax" , (float) 10.0 )||
        !parameters()->add( "Scale  (max)" , "-smax" , (float) 1.05 )||
        !parameters()->add( "Rotation theta (max)" , "-theta" , (float) 0.1 )  ||
        !parameters()->add( "X(max)" , "-xmax" , (float) 100 )  ||
        !parameters()->add( "Y(max)" , "-ymax" , (float) 100 )  
        ) 
     {
        vcl_cerr << "ERROR: Adding parameters in dbrl_ground_truth_point_set_generator::dbrl_ground_truth_point_set_generator()" << vcl_endl;
     }
    
 
}

//: Destructor
dbrl_ground_truth_point_set_generator::~dbrl_ground_truth_point_set_generator()
{

}


//: Return the name of this process
vcl_string
dbrl_ground_truth_point_set_generator::name()
{
  return "Point Generator";
}


//: Return the number of input frame for this process
int
dbrl_ground_truth_point_set_generator::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbrl_ground_truth_point_set_generator::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_ground_truth_point_set_generator::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "dbrl_id_point_2d" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_ground_truth_point_set_generator::get_output_type()
{  
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "dbrl_id_point_2d" );
  return to_return;
}


//: Execute the process
bool
dbrl_ground_truth_point_set_generator::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbrl_ground_truth_point_set_generator::execute() - "
             << "not exactly two input images \n";
    return false;
  }
  //clear_output();
  dbrl_id_point_2d_storage_sptr frame_pts;
  frame_pts.vertical_cast(input_data_[0][0]);



  static float txmin=0.0;
  static float txmax=10.0;
  static float tymin=0.0;
  static float tymax=1.0;

  static int ppoints=10;
  static int numoutlier=10;

  static float sigma=0.0;


  parameters()->get_value("-txmin",txmin);
  parameters()->get_value("-txmax",txmax);
  parameters()->get_value("-tymin",tymin);
  parameters()->get_value("-tymax",tymax);
  parameters()->get_value("-ppoints",ppoints);
  parameters()->get_value("-numout",numoutlier);
  parameters()->get_value("-nsigma",sigma);


  tx += ((vcl_rand() / (double) RAND_MAX)*(txmax-txmin)+txmin);
  ty += ((vcl_rand() / (double) RAND_MAX)*(tymax-tymin)+tymin);

  vcl_vector<dbrl_id_point_2d_sptr> idpoints;
  vcl_vector<dbrl_id_point_2d_sptr> origpoints=frame_pts->points();

  float xmin=0.0,xmax=0.0,ymin=0.0,ymax=0.0;
  parameters()->get_value("-xmax",xmax);
  parameters()->get_value("-ymax",ymax);
  for(int i=0;i<static_cast<int>(origpoints.size());i++)
      {
       /* if(xmin>origpoints[i]->x())
            xmin=origpoints[i]->x();
        if(xmax<origpoints[i]->x())
            xmax=origpoints[i]->x();

        if(ymin>origpoints[i]->y())
            ymin=origpoints[i]->y();
        if(ymax<origpoints[i]->y())
            ymax=origpoints[i]->y();*/

        dbrl_id_point_2d_sptr p=new dbrl_id_point_2d(origpoints[i]->x()+tx,origpoints[i]->y()+ty,origpoints[i]->tangent(),origpoints[i]->id());
        idpoints.push_back(p);
      }

  //: dropping some of the points
  int numofpointsremoved=(origpoints.size()*ppoints)/100;
  vcl_vector<dbrl_id_point_2d_sptr>::iterator iter;
  for(int i=0;i<numofpointsremoved;i++)
      { 
        int pos=vcl_rand()%idpoints.size();
        iter=idpoints.begin();
        iter+=pos;
        if(iter!=idpoints.end())
            idpoints.erase(iter);
      }
  vcl_vector<dbrl_id_point_2d_sptr> noisypointids;
  //: adding noise to the existing points 
  for(int i=0;i<static_cast<int>(idpoints.size());i++)
      {

        double x=idpoints[i]->x()+vnl_sample_normal(0,sigma);
        double y=idpoints[i]->y()+vnl_sample_normal(0,sigma);
        double tangent=idpoints[i]->tangent()+vnl_sample_normal(0,sigma);
        noisypointids.push_back(new dbrl_id_point_2d(x,y,tangent,idpoints[i]->id()));
      }
  //: adding outliers
  for(int i=0;i<numoutlier;i++)
      {
       double xoutlier=((vcl_rand() / (double) RAND_MAX)*(xmax-xmin)+xmin);//+tx;
       double youtlier=((vcl_rand() / (double) RAND_MAX)*(ymax-ymin)+ymin);//+ty;
       dbrl_id_point_2d_sptr p=new dbrl_id_point_2d(xoutlier,youtlier,-1);
       noisypointids.push_back(p);
      }
    
 dbrl_id_point_2d_storage_sptr output=dbrl_id_point_2d_storage_new(noisypointids);
 output_data_[0].push_back(output);


  return true;  
}
//: Clone the process
bpro1_process*
dbrl_ground_truth_point_set_generator::clone() const
{
  return new dbrl_ground_truth_point_set_generator(*this);
}
    
bool
dbrl_ground_truth_point_set_generator::finish()
{
  return true;
}




