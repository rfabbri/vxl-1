// This is contrib/sadali/gui/dvidpro_affine_homog_est_process.cxx

//:
// \file

#include "vidpro1_affine_homog_est_process.h"
#include <bpro1/bpro1_parameters.h>

// include storage classes needed
// such as ...
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vtol_storage.h>
#include <vidpro1/storage/vidpro1_vtol_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol3D_storage.h>
#include <vidpro1/storage/vidpro1_vsol3D_storage_sptr.h>

// other includes needed

#include <bprt/bprt_homog_interface.h>
#include <vul/vul_awk.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/algo/vgl_h_matrix_2d.h>



//: Constructor
dvidpro_affine_homog_est_process::dvidpro_affine_homog_est_process()
{
  // Set up the parameters for this process
  if( !parameters()->add( "No of frames in reconstruction" , "-num_input_frames" , (int)10 )
 //         ||(
 //  !parameters()->add( "VRML Filename" , "-vrml_fname" , (vcl_string)"d://surfptsnew.txt" ) 
 //           )
          ||(
     !parameters()->add( "No of planes" , "-numplanes" , (int)4 ) 
            )
          ||(
     !parameters()->add( "Use Correlation" , "-use_corr" , (bool) false) 
            )
        )

  {
    vcl_cerr << "ERROR: Adding parameters in dvidpro_affine_homog_est_process::dvidpro_affine_homog_est_process()" << vcl_endl;
  }
}


//: Destructor
dvidpro_affine_homog_est_process::~dvidpro_affine_homog_est_process()
{
}

bpro1_process* dvidpro_affine_homog_est_process::clone() const
{
    return new dvidpro_affine_homog_est_process(*this);
}


//: Return the name of this process
vcl_string
dvidpro_affine_homog_est_process::name()
{
  return "Affine Transform Estimation";
}


//: Return the number of input frame for this process
int
dvidpro_affine_homog_est_process::input_frames()
{
  // input from this frame
  int framenum;
  parameters()->get_value("-num_input_frames",framenum);
  if (framenum<2)
      framenum = 2;
  return framenum;
}


//: Return the number of output frames for this process
int
dvidpro_affine_homog_est_process::output_frames()
{
int framenum;
parameters()->get_value("-num_input_frames",framenum);
 if (framenum<2)
      framenum = 2;

  return framenum;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > 
dvidpro_affine_homog_est_process::get_input_type()
{
  // this process looks for a vsol2D storage class
  // at each input frame
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  bool image_;
  parameters()->get_value("-use_corr",image_);
  if (image_)
      to_return.push_back( "image" );
  
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > 
dvidpro_affine_homog_est_process::get_output_type()
{  
  // this process produces a vsol2D storage class
  vcl_vector<vcl_string > to_return;
 // to_return.push_back( "vsol3D" );
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Execute the process
bool
dvidpro_affine_homog_est_process::execute()
{
int num_of_frames;
  vcl_string vrml_fname;
  parameters()->get_value("-num_input_frames", num_of_frames);
  
  vcl_vector<vcl_vector<vsol_point_2d_sptr > >  obs_matrix;
  int no_pts;
 // if (num_of_frames>2)
   //  {
  // verify that the number of input frames is correct
  if ( input_data_.size() != num_of_frames ){
    vcl_cout << "In dvidpro_affine_homog_est_process::execute() - not exactly one"
             << " input frames" << vcl_endl;
    return false;
  }
  clear_output();

  // get images from the storage classes
  vcl_vector<vidpro1_vsol2D_storage_sptr> feat_pts(num_of_frames); 
  
  for (int i = 0; i<num_of_frames; i++)
      feat_pts[i].vertical_cast(input_data_[i][0]);
    
  


  
  vcl_vector<vsol_spatial_object_2d_sptr>::const_iterator feat;

  
  vcl_vector<vsol_spatial_object_2d_sptr> feat_sovec;
  vcl_vector<vsol_point_2d_sptr> feat_pointvec;
  vcl_vector<vsol_point_2d_sptr> ref_feat_pointvec;
  vcl_vector<vsol_spatial_object_2d_sptr> ref_view;
  vcl_vector<vnl_double_3x3 > affine_list;
  

  feat_sovec = (feat_pts[num_of_frames-1])->all_data();
  no_pts = feat_sovec.size();
  feat_sovec.clear();
  ref_view = feat_pts[0]->all_data();
  int j = 0;
 
  for (int j = 0 ; j<no_pts; j++)
  {
    ref_feat_pointvec.push_back( ref_view[j].ptr()->cast_to_point()  );
   
  }




  for (int i = 1; i<num_of_frames; i++)
  {
  
     feat_sovec = (feat_pts[i])->all_data();  // get all points in ith frame
     assert(feat_sovec.size()>0);
     

     int j = 0;
     vcl_cout<<i<<"Num of points"<<feat_sovec.size()<<"\n";
     for (feat = feat_sovec.begin(); (j<no_pts); feat++)
         {
         feat_pointvec.push_back( feat->ptr()->cast_to_point()  );
         j++;
         }
     int dof = 6;
     int scale = 1;
     bool affine = true;
     int num_planes =3;
     bprt_homog_interface Homography_Calc(ref_feat_pointvec , feat_pointvec, dof,&num_planes, scale, affine );
     Homography_Calc.set_method((vcl_string)"muset" );
     Homography_Calc.compute_homog();
     vnl_double_3x3 AffH;
     for (int p =0; p<num_planes; p++)
     {
         AffH = Homography_Calc.get_H(p);
         affine_list.push_back(AffH);
     }
     feat_pointvec.clear();
     feat_sovec.clear();
  }
      
 




  //=========================================
  // Do something here with all of this data
 
    // create the output storage class
  vcl_vector<vidpro1_vsol2D_storage_sptr> output_vsol(num_of_frames);
  for ( int i = 0; i<num_of_frames; i++)
   output_vsol[i] = vidpro1_vsol2D_storage_new();

 
 for ( int i = 1; i<num_of_frames; i++)
     {
     vgl_h_matrix_2d<double> H(affine_list[i-1]);
     for (int j = 0; j<no_pts; j++)
     {
        vgl_homg_point_2d<double> imag_of_pt = H (vgl_homg_point_2d<double>( ref_feat_pointvec[j]->x() , ref_feat_pointvec[j]->y() ) );
        output_vsol[i]->add_object(new vsol_point_2d(imag_of_pt.x(), imag_of_pt.y()) , "Image of Point");
        
     }
     }


    ///Prepare output
  output_data_[0].push_back (input_data_[0][0]) ;
  for ( int i = 1; i<num_of_frames; i++)
     output_data_[i].push_back(output_vsol[i]);

 
  return true;
}


//: Finish
bool
dvidpro_affine_homog_est_process::finish()
{
  // I'm not really sure what this is for, ask Amir
  // - mleotta
  return true;
}


