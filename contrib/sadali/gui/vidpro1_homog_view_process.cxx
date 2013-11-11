// This is contrib/sadali/vidpro1_my_process.cxx

//:
// \file

#include "vidpro1_homog_view_process.h"
#include <bpro1/bpro1_parameters.h>

// include storage classes needed
// such as ...
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

// other includes needed

#include <bprt/bprt_plane_reconst.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_random.h>





//: Constructor
vidpro1_homog_view_process::vidpro1_homog_view_process()
{
  // Set up the parameters for this process
  if( 
      (!parameters()->add( "Outlier Fraction " , "-p1" , 0.35f  ) ) ||
          (!parameters()->add( "Outlier Sigma "    , "-p2" ,  35.0f  ) ) ||
          (!parameters()->add( "Sigma    "         , "-p3" , 0.0f   ) ) ||
          (!parameters()->add( "Random seed"       , "-p4" , (int)40    ) ) ||
                  (!parameters()->add( "Homography Matrix1"       , "-hom1" , 1.0f  ) ) ||
                  (!parameters()->add( "Homography Matrix2"       , "-hom2" , 0.0f    ) ) ||
                  (!parameters()->add( "Homography Matrix3"       , "-hom3" , -4.0f    ) ) ||
                  (!parameters()->add( "Homography Matrix4"       , "-hom4" , 1.0f    ) ) ||
                  (!parameters()->add( "Homography Matrix5"       , "-hom5" , 1.0f    ) ) ||
                  (!parameters()->add( "Homography Matrix6"       , "-hom6" , 2.0f    ) ) ||
                  (!parameters()->add( "Homography Matrix7"       , "-hom7" , 0.0f    ) ) ||
                  (!parameters()->add( "Homography Matrix8"       , "-hom8" , 0.0f    ) ) ||
                  (!parameters()->add( "Homography Matrix9"       , "-hom9" , 1.0f    ) )
                  
          )
          
  {
    vcl_cerr << "ERROR: Adding parameters in vidpro1_homog_view_process::vidpro1_homog_view_process()" << vcl_endl;
  }
}



//: Destructor
vidpro1_homog_view_process::~vidpro1_homog_view_process()
{
}


//: Return the name of this process
vcl_string
vidpro1_homog_view_process::name()
{
  return "Homography Test";
}


bpro1_process *
vidpro1_homog_view_process::clone() const
{
    return new vidpro1_homog_view_process(*this);

};
//: Return the number of input frame for this process
int
vidpro1_homog_view_process::input_frames()
{
  // input from this frame 
  return 1;
}


//: Return the number of output frames for this process
int
vidpro1_homog_view_process::output_frames()
{
  // output to this frame only
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > 
vidpro1_homog_view_process::get_input_type()
{
  // this process looks for an image and vsol2D storage class
  // at each input frame
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > 
vidpro1_homog_view_process::get_output_type()
{  
  // this process produces a vsol2D storage class
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  
  return to_return;
}


//: Execute the process
bool
vidpro1_homog_view_process::execute()
{
  // verify that the number of input frames is correct
  if ( input_data_.size() != 1 ){
    vcl_cout << "In vidpro1_homog_view_process::execute() - not exactly two"
             << " input frames" << vcl_endl;
    return false;
  }
  clear_output();

  // get vsol2d from the storage classes
  vidpro1_vsol2D_storage_sptr curr_frame_corn; 
  
  curr_frame_corn.vertical_cast(input_data_[0][0]);
  
  
  
  

  //get parameters
  float param1;
  float param2;
  float param3;
  int   param4;
  float homparam1,homparam2,homparam3,homparam4,
             homparam5,homparam6,homparam7,homparam8,homparam9;

  
  parameters()->get_value( "-p1" , param1 );
  parameters()->get_value( "-p2" , param2 );
  parameters()->get_value( "-p3" , param3 );
  parameters()->get_value( "-p4" , param4 );
  parameters()->get_value( "-hom1" , homparam1 );
  parameters()->get_value( "-hom2" , homparam2 );
  parameters()->get_value( "-hom3" , homparam3 );
  parameters()->get_value( "-hom4" , homparam4 );
  parameters()->get_value( "-hom5" , homparam5 );
  parameters()->get_value( "-hom6" , homparam6 );
  parameters()->get_value( "-hom7" , homparam7 );
  parameters()->get_value( "-hom8" , homparam8 );
  parameters()->get_value( "-hom9" , homparam9 );
  vnl_double_3x3 homog_mat;
  homog_mat[0][0]=(double)homparam1;
  homog_mat[0][1]=(double)homparam2;
  homog_mat[0][2]=(double)homparam3;
  homog_mat[1][0]=(double)homparam4;
  homog_mat[1][1]=(double)homparam5;
  homog_mat[1][2]=(double)homparam6;
  homog_mat[2][0]=(double)homparam7;
  homog_mat[2][1]=(double)homparam8;
  homog_mat[2][2]=(double)homparam9;
  vgl_h_matrix_2d<double> test_homog_mat(homog_mat);
  vcl_cout<<"Homog mat:"<<homog_mat<<"\n";
  
  
  
 
  //=========================================
  // Do something here with all of this data
  //=========================================
  
  vnl_random mz_random(param4);
 

  vcl_vector<vcl_string> dataname = curr_frame_corn->groups();
  vcl_vector<vsol_spatial_object_2d_sptr> transf_pts;
  vcl_vector < vsol_spatial_object_2d_sptr > origpts;
  for (int nameindex=0; nameindex<dataname.size(); nameindex++)
  {

          vcl_vector < vsol_spatial_object_2d_sptr > curr_group = curr_frame_corn->data_named(dataname[nameindex]);
          for (int m = 0; m<curr_group.size(); m++)
          origpts.push_back(curr_group[m]);
 
  }
 int inliers=0, outliers=0;

 

  for (int i=0;i<origpts.size();i++)
  {
          vgl_homg_point_2d<double> transf_pt=test_homog_mat*vgl_homg_point_2d<double>(origpts[i]->cast_to_point()->x(),origpts[i]->cast_to_point()->y());
            if ( mz_random.drand32() < param1 ) 
                {
                        transf_pt += vgl_vector_2d<double>(param2 * mz_random.drand32(0,2*vnl_math::pi),param2  * mz_random.drand32(0,2*vnl_math::pi));
                outliers ++;
                }
                else
                {
        transf_pt += vgl_vector_2d<double>(param3  * mz_random.drand32(0,2*vnl_math::pi),param3 * mz_random.drand32(0,2*vnl_math::pi));
        inliers ++ ;
                }

          transf_pts.push_back(new vsol_point_2d(transf_pt));
//        vcl_cout<< transf_pt.x()/transf_pt.w()<<"   "<<transf_pt.y()/transf_pt.w()<<"\n";
  }
  vcl_cout<<"pts  "<<transf_pts.size()<<" \n";
  vcl_cout<<"Inliers:"<<inliers<<" \n";
  vcl_cout<<"Outliers:"<<outliers<<" \n";

          
  
 

  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol2D = vidpro1_vsol2D_storage_new();
    
  output_vsol2D->add_objects(transf_pts,"Transf pt");
  output_data_[0].push_back(output_vsol2D);
  
   
  
  return true;
}


//: Finish
bool
vidpro1_homog_view_process::finish()
{
  // I'm not really sure what this is for, ask Amir
  // - mleotta
  return true;
}
