// This is contrib/sadali/gui/vidpro1_plane_opt_process.cxx
#define MIN(a,b)  ((a<b) ? a : b)
//:
// \file

#include "vidpro1_plane_opt_process.h"
#include <bpro1/bpro1_parameters.h>

// include storage classes needed
// such as ...

#include <vidpro1/storage/vidpro1_vtol_storage.h>
#include <vidpro1/storage/vidpro1_vtol_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h> 
#include <vidpro1/storage/vidpro1_image_storage.h>

// other includes needed
#include <vcl_cmath.h>
#include <vcl_limits.h>


#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vtol/vtol_edge_2d_sptr.h>
#include <vtol/vtol_edge_2d.h>
#include <vtol/vtol_topology_object_sptr.h>
#include <vtol/vtol_topology_object.h>
#include <vtol/vtol_vertex_2d_sptr.h>

#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_resource_sptr.h>
#include <vil/algo/vil_convolve_2d.h>
#include <vil/algo/vil_gauss_filter.h>
#include <vil/vil_math.h>
#include <bseg/brip/brip_vil_float_ops.h>

#include <vnl/vnl_inverse.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_double_3x1.h>
#include <vnl/vnl_double_4x3.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_least_squares_function.h>
#include <vnl/algo/vnl_svd.h>
#include <vcsl/vcsl_displacement.h>

#include <vgl/vgl_homg_point_2d.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_point_3d.h>
#include <gui/planar_curve_reconst_problem.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include "vidpro1_3D_planar_curve_reconst_process.h"
#include "BB_optimization_problem.h"


//: Constructor
vidpro1_plane_opt_process::vidpro1_plane_opt_process()
{
  // Set up the parameters for this process

  if (

      (!parameters()->add( "BB Filename" , "-BB_fname" , (vcl_string)"d://error_fname.txt" ) )
      ||
      (!parameters()->add( "Filter Sigma" , "-filter_sigma" , (float)1.5 ) )
      ||
      (!parameters()->add( "3DPoint sampling" , "-3dpoint" , (bool)true ) )
      ||
      (!parameters()->add( "Error plot to c://errors.txt" , "-err_plot" , (bool)false ) )
      ||

#if 0
//unnecessary for LM unless want to change params
      (!parameters()->add( "Mu Factor for trans"  ,        "-mu_t"    ,     (float)1.0 ) )
      ||
      (!parameters()->add( "Mu Factor for angle" ,         "-mu_ang"   ,    (float)1.0 ) )
      ||
      (!parameters()->add( "Gradient Descent threshold" ,  "-GDthres" ,     (float).005 ) )
      ||
      (!parameters()->add( "Step size for t" ,             "-stept" ,       (float).005 ) )
      ||
      (!parameters()->add( "Step size for alpha" ,         "-stepalpha" ,   (float).01 ) )
         
      ||

      (!parameters()->add( "BB00" , "-BB00" , (float)-2.96532    ) )
      ||
      (!parameters()->add( "BB01" , "-BB01" , (float) -0.0710022 ) )
      ||
      (!parameters()->add( "BB02" , "-BB02" , (float)-2.58799) )
      ||
      (!parameters()->add( "BB03" , "-BB03" , (float)-3.36415   ) )
      ||
      (!parameters()->add( "BB10" , "-BB10" , (float)-0.387533   ) )
      ||
      (!parameters()->add( "BB11" , "-BB11" , (float)2.26934  ) )
      ||
      (!parameters()->add( "BB12" , "-BB12" , (float).00        ) )
      ||
      (!parameters()->add( "BB13" , "-BB13" , (float)-1.66451   ) )
      ||
      (!parameters()->add( "BB20" , "-BB20" , (float)5.28543   ) )
      ||
      (!parameters()->add( "BB21" , "-BB21" , (float)0.126556   ) )
      ||
      (!parameters()->add( "BB22" , "-BB22" , (float)-1.45195  ) )
      ||
      (!parameters()->add( "BB23" , "-BB23"  , (float)33.2885   ) )
      ||
      (!parameters()->add( "PL00" , "-PL00" , (float)2000.0     ) )
      
      ||
      (!parameters()->add( "PL02" , "-PL02" , (float)512        ) )
      ||
    //  (!parameters()->add( "PL03" , "-PL03" , (float)0.00 ) )
      
    //  ||
      (!parameters()->add( "PL11" , "-PL11" , (float)2000.0 ) )
      ||
      (!parameters()->add( "PL12" , "-PL12" , (float)384.0 ) )
      ||
  //    (!parameters()->add( "PL13" , "-PL13" , (float).00 ) )
   //   ||
    //  (!parameters()->add( "PL23" , "-PL23" , (float).00 ) )
    //  ||
    
      (!parameters()->add( "PR03" , "-PR03"  , (float)610.071) )

      ||
     
      (!parameters()->add( "PR13" , "-PR13"  , (float)-237.358 ) )
      ||

    //  (!parameters()->add( "PR23" , "-PR23"  , (float)-1.00 ) )
//      ||
#endif

      (!parameters()->add( "Num_of_trans_samp" , "-Trans"  , (int)9 ) )
      ||
      (!parameters()->add( "trans_samp Rate" , "-Trans_rate"  , (float)0.1 ) )
      ||
      (!parameters()->add( "Num_of_ang_samp" , "-Ang"  , (int)0 ) )
      ||
      (!parameters()->add( "ang_samp Rate" , "-Ang_rate"  , (float)0.15 ) )
      ||
      (!parameters()->add( "Translation by Normal Vec" , "-TrNorm"  , (bool)false ) )
      ||
      (!parameters()->add( "Optimize using rotation" , "-rot"  , (bool)false ) )
      ||
      (!parameters()->add( "Optimize using translation" , "-tr"  , (bool)true ) )



      )
          
  {
    vcl_cerr << "ERROR: Adding parameters in vidpro1_plane_opt_process::vidpro1_plane_opt_process()" << vcl_endl;
  }
 
}



//: Destructor
vidpro1_plane_opt_process::~vidpro1_plane_opt_process()
{

}


//: Return the name of this process
vcl_string
vidpro1_plane_opt_process::name()
{
  return "BB Plane Optimization";
}


bpro1_process *
vidpro1_plane_opt_process::clone() const
{
    return new vidpro1_plane_opt_process(*this);

};
//: Return the number of input frame for this process
int
vidpro1_plane_opt_process::input_frames()
{
  // input from this frame 
        return 1;
}


//: Return the number of output frames for this process
int
vidpro1_plane_opt_process::output_frames()
{
  // output to this frame only
return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > 
vidpro1_plane_opt_process::get_input_type()
{
  // this process looks for  vsol2D storage classes
  // at each input frame
 
  vcl_vector< vcl_string > to_return;

  to_return.push_back( "image" );
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  to_return.push_back( "vtol" );
  to_return.push_back( "vtol" );

  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > 
vidpro1_plane_opt_process::get_output_type()
{  
  // this process produces a vsol2D storage class
        
  vcl_vector<vcl_string > to_return;
 
  to_return.push_back( "image" );
  to_return.push_back( "image" );
  
  return to_return;
}


//: Execute the process
bool
vidpro1_plane_opt_process::execute()
{
  // verify that the number of input frames is correct
  if ( input_data_.size() != 1 ){
    vcl_cout << "In vidpro1_plane_opt_process::execute() - not exactly one"
             << " input frames" << vcl_endl;
    return false;
  }
  bool error_plot;
   double number_of_res_from_left, number_of_res_from_right; 
  vcl_ofstream error_fname("c:\\errors.txt", vcl_ios::out);
  
  vcl_string BB_fname;
  
   
  float sigma ;

  int num_of_error_samples = 15;
  int num_of_ang_samples = 5;
  float tinterval = 0.01;
  float alphainterval = 0.05;

 
  parameters()->get_value("-filter_sigma", sigma);

  parameters()->get_value("-Trans",num_of_error_samples);
  parameters()->get_value("-Trans_rate" ,tinterval);
  parameters()->get_value("-Ang",num_of_ang_samples); 
   parameters()->get_value("-Ang_rate",alphainterval); 
   parameters()->get_value("-rot",rotation);
   parameters()->get_value("-tr",translation);
   parameters()->get_value("-TrNorm",trans_from_KT); 
   parameters()->get_value("-3dpoint",WP_point_samp); 
   parameters()->get_value("-err_plot", error_plot);
    parameters()->get_value("-BB_fname",BB_fname);

 
  // get images from the storage classes
  
  vidpro1_image_storage_sptr left_img_stor; 
  left_img_stor.vertical_cast(input_data_[0][0]);
  vidpro1_image_storage_sptr right_img_stor; 
  right_img_stor.vertical_cast(input_data_[0][1]);
  vidpro1_image_storage_sptr left_mask_stor; 
  left_mask_stor.vertical_cast(input_data_[0][2]);
  vidpro1_image_storage_sptr right_mask_stor; 
  right_mask_stor.vertical_cast(input_data_[0][3]);

   vil_image_resource_sptr left_img = left_img_stor->get_image();
   vil_image_resource_sptr right_img = right_img_stor->get_image();
   vil_image_resource_sptr left_mask = left_mask_stor->get_image();
   vil_image_resource_sptr right_mask = right_mask_stor->get_image();

  vil_image_view<float> left_img_view = left_img->get_view();
  vil_image_view<float> right_img_view = right_img->get_view();
  left_mask_view = left_mask->get_view();
  right_mask_view = right_mask->get_view();

  mapleft.set_size( left_img_view.ni() , left_img_view.nj() );
  mapright.set_size( right_img_view.ni() , right_img_view.nj() );

  vil_image_view<float> left_img_view_filt(left_img_view.ni(),left_img_view.nj() );
  vil_image_view<float> right_img_view_filt(right_img_view.ni(),right_img_view.nj() );
  corrleft.set_size(left_img_view.ni(),left_img_view.nj() );
  corrright.set_size(right_img_view.ni(), right_img_view.nj() );
  corrleft.fill(0.0);
  corrright.fill(0.0);




//left_img_view_filt = left_img_view;
//  right_img_view_filt = right_img_view;

  //Initialize Normal, alpha and t parameters
  double t;
  double alpha;
  vnl_double_4 CameraCenter;
  vnl_double_4x4 BBMatrix;
  vcl_vector<vnl_double_4> WorldPoints;

  
 // Get parameters
 
  float samplexf, sampleyf;
  double samplex, sampley;
  //parameters()->get_value("-sx",samplexf);
  //parameters()->get_value("-sy",sampleyf);
  samplexf = 0.005f;
  sampleyf = 0.005f;
  samplex = samplexf;
  sampley = sampleyf;
  


  


//Read BB and Proj Mat info from file.
   
  
   vcl_ifstream BB_file(BB_fname.c_str(), vcl_ios::in);
   vcl_ofstream new_BB_file((BB_fname+vcl_string("opt")).c_str(), vcl_ios::out);
   vcl_string str("");
   while (str!="Transform")
   {BB_file>>str;
   new_BB_file<<str<<"  ";

   }
   BB_file>>BBMatrix;
   
   
   //Initialize error parameters
   
   
   while (str!="Camera")
   {BB_file>>str;
   }

   int camno,camno2;
   BB_file>>camno;
   BB_file>>LeftProjMat;
   BB_file>>str;
   BB_file>>camno2;
   BB_file>>RightProjMat;
  // BB_file>>str;
   

 
  InitializeParams(BBMatrix, t, alpha);
 

 K_ = LeftProjMat.extract(3,3);
 t_[0] = RightProjMat[0][3]-LeftProjMat[0][3];
 t_[1] = RightProjMat[1][3]-LeftProjMat[1][3];
 t_[2] = RightProjMat[2][3]-LeftProjMat[2][3];
  
 
  int minimizing_parami;
  int minimizing_paramj;
  vnl_double_4x4 OptimalBBMatrix;
  vcl_cout<<"Initial BB Matrix:  "<<BBMatrix<<"\n";
 
  double prev_opt_paramt = 0.0;
  double prev_opt_paramalpha = 0.0;

  vnl_double_4x4 BBMatrix_LM;
  vnl_vector<double> param_vec(2);
  double error =0.0;
  vil_math_sum(number_of_res_from_left,left_mask_view, 0);
  vil_math_sum(number_of_res_from_right,right_mask_view, 0);
  number_of_res_from_left  /= 255;
  number_of_res_from_right /= 255;
  number_of_residuals = number_of_res_from_left+ number_of_res_from_right;
  for (int  sigmascale = 1;sigmascale<4; sigmascale++)
  {
      left_img_view_filt = brip_vil_float_ops::gaussian(left_img_view, (float)(sigma/sigmascale));

      right_img_view_filt = brip_vil_float_ops::gaussian(right_img_view, (float)(sigma/sigmascale));
      double minerr = 1E8;
 
  


  //Compute errors
  
  for (int i = -num_of_error_samples;  i< num_of_error_samples+1; i++)
      for (int j = -num_of_ang_samples; j<num_of_ang_samples+1; j++)
  { 
      double newcoef =prev_opt_paramt+i*tinterval;
      double newalpha = prev_opt_paramalpha + j*alphainterval;
      vcl_cout<<"i: "<<i<<"  newcoef:  "<<newcoef<<"\t";
      vcl_cout<<newalpha<<"\n";

      int num_of_res;

      vnl_double_4x4 UpdatedBBMatrix;
      updateBBMat(BBMatrix,UpdatedBBMatrix,newcoef,newalpha);
      compute_plane_params(UpdatedBBMatrix);
  
      computeWorldPoints(WorldPoints, samplex, sampley, UpdatedBBMatrix);  
      
      error = compute_err(left_img_view_filt, right_img_view_filt, WorldPoints,UpdatedBBMatrix, num_of_res);
      if (error<minerr)
      {
          minimizing_parami = i;
          minimizing_paramj = j;
          minerr = error;
          OptimalBBMatrix = UpdatedBBMatrix;
          vcl_cout<<"i:  "<<i<<"   New coef  "<<newcoef;
          vcl_cout<<" \t Best  error up to date:   "<<error<<"\n";
         // number_of_residuals = num_of_res;
      }
      WorldPoints.clear();
      vcl_cout<<"error is "<<error<<"\n";
      error_fname<<newcoef<<"\t"<<newalpha<<"\t"<<error<<"\n";
 
     

  }
  double param1 = prev_opt_paramt+ minimizing_parami*tinterval;
  double param2 = prev_opt_paramalpha + minimizing_paramj*alphainterval;
  double start_paramt = param1;
  double start_paramalpha = param2;
  vcl_cout<<" Starting LM with param1 = "<<param1<<"   and param2 = "<<param2<<"\n";


  double lmsampx= samplexf;
  double lmsampy = sampleyf;
  if (WP_point_samp)
  number_of_residuals = (unsigned int)vcl_floor((1.0/lmsampx)*(1.0/lmsampy));
  
vcl_cout<<"Number of _residuals"<<number_of_residuals<<"\n";
  BB_optimization_problem *BB_opt_inst = new BB_optimization_problem(translation, rotation, trans_from_KT,WP_point_samp, 
      lmsampx,lmsampy,BBMatrix,number_of_residuals,LeftProjMat, RightProjMat);
 
 

  BB_opt_inst->set_img(left_img_view_filt, right_img_view_filt);
 
  //BB_opt_inst->set_left_Proj(LeftProjMat);
  //BB_opt_inst->set_right_Proj(RightProjMat);
  BB_opt_inst->set_masks(left_mask_view,right_mask_view);
 
 param_vec[0] = param1;
 param_vec[1] = param2;

vnl_levenberg_marquardt  *LM_inst= new vnl_levenberg_marquardt(*((vnl_least_squares_function*)(BB_opt_inst)));
 LM_inst->minimize_without_gradient(param_vec);
// LM_inst->diagnose_outcome(vcl_cout);
vcl_cout<<"optimized coef  "<<param_vec[0]<< " optimized angle:  "<<param_vec[1];
updateBBMat(BBMatrix,BBMatrix_LM,param_vec[0],param_vec[1]);
param1 = param_vec[0];
param2 = param_vec[1];
delete LM_inst;
delete BB_opt_inst;
prev_opt_paramt = param1;
prev_opt_paramalpha = param2;


  


  //Printing out results
vcl_cout<<" alpha is " <<param_vec[1]<<"\n";
vcl_cout<<" t is" <<param_vec[0]<<"\n";
vcl_cout<<" Optimal BB Matrix is "<< OptimalBBMatrix<<"\n";
vcl_cout<<" Optimal BB  Matrix after LM is "<< BBMatrix_LM<<"\n";
vcl_cout<<"  minimizing i is  "<<minimizing_parami<<"\n";
vcl_cout<<"  minimizing j is  "<<minimizing_paramj<<"\n";
vcl_cout<<" Started LM with t = "<<start_paramt<<"\t";
vcl_cout<<" alpha = "<<start_paramalpha<<"\n";
//vcl_cout<<" errors are  \n";
//for (int u=0; u<(num_of_error_samples*2+1)*(2*num_of_ang_samples+1); u++)
//vcl_cout<<"\n "<<errorlist[u];
tinterval/=2;
alphainterval/=2;


 
}
      left_img_view_filt = brip_vil_float_ops::gaussian(left_img_view, (float)(sigma));

      right_img_view_filt = brip_vil_float_ops::gaussian(right_img_view, (float)(sigma));
    
      WorldPoints.clear();
//Creating 3d projections onto world plane
      vidpro1_3D_planar_curve_reconst_process *curve_compare = new vidpro1_3D_planar_curve_reconst_process;
      
      vcl_vector<bpro1_storage_sptr> curve_inputs;
      curve_inputs.push_back(input_data_[0][4]);
      curve_inputs.push_back(input_data_[0][5]);
      curve_inputs.push_back(input_data_[0][2]);
      curve_inputs.push_back(input_data_[0][3]);
      curve_compare->set_input(curve_inputs);
      vcl_cout<<"vidpro1_3D_planar_curve_reconst_process::initialize()\n ";
      curve_compare->initialize();
         vcl_cout<<"vidpro1_3D_planar_curve_reconst_process::set_BB_Matrix()\n ";
      curve_compare->set_BB_matrix(BBMatrix_LM);
      curve_compare->execute();
      curve_compare->finish();
 

      new_BB_file<<"\n";
     new_BB_file<<BBMatrix_LM;
     new_BB_file<<"\n";
     new_BB_file<<str<<"  ";
     new_BB_file<<camno<<"\n";
     new_BB_file<<LeftProjMat<<"\n";
     new_BB_file<<str<<"  ";
     new_BB_file<<camno2<<"\n";
     new_BB_file<<RightProjMat<<"\n";
     unsigned char charread;
     BB_file>>charread;
     while (charread!=EOF)
     {
         new_BB_file<<charread;
         BB_file>>charread;
           
     };
//Output an error plot
if (error_plot)
{
      tinterval= 0.005;
      alphainterval= 0.005;
vcl_cout<<"Computing errors to draw 3d plot\n";

      for (int i = -num_of_error_samples;  i< num_of_error_samples+1; i++)
          for (int j = -num_of_ang_samples; j<num_of_ang_samples+1; j++)
          { 
              double newcoef =param_vec[0]+i*tinterval;
              double newalpha = param_vec[1]+ j*alphainterval;
             // vcl_cout<<"i: "<<i<<"  newcoef:  "<<newcoef<<"\t";
             // vcl_cout<<newalpha<<"\n";
              error_fname<<newcoef<<"\t"<<newalpha<<"\t";


              vnl_double_4x4 UpdatedBBMatrix;
              updateBBMat(BBMatrix,UpdatedBBMatrix,newcoef,newalpha);
              compute_plane_params(UpdatedBBMatrix);

              computeWorldPoints(WorldPoints, samplex, sampley, UpdatedBBMatrix);  

              error = compute_err(left_img_view_filt, right_img_view_filt, WorldPoints,UpdatedBBMatrix, number_of_residuals);
              error_fname<<error<<"\n";
             
              WorldPoints.clear();
             // vcl_cout<<"error is "<<error<<"\n";
              //     errorlist[(i+num_of_error_samples)*(2*num_of_error_samples+1)+j+num_of_ang_samples] = error;


          }
}
  error_fname.close();


  // create the output images
  vil_image_view<vxl_byte> greyvalimg1(left_img_view_filt.ni(),left_img_view_filt.nj() );
  vil_image_view<vxl_byte> greyvalimg2(right_img_view_filt.ni(), right_img_view_filt.nj() );

   greyvalimg1 = brip_vil_float_ops::convert_to_byte(corrleft,0.0,1.0);
   greyvalimg2 = brip_vil_float_ops::convert_to_byte(right_img_view_filt,0.0,1.0);


  // create the output storage class
  vidpro1_image_storage_sptr output_img1 = vidpro1_image_storage_new();
  vidpro1_image_storage_sptr output_img2 = vidpro1_image_storage_new();
  vil_image_resource_sptr output_rsrc_img1 =  vil_new_image_resource_of_view(greyvalimg1);
  vil_image_resource_sptr output_rsrc_img2 = vil_new_image_resource_of_view(greyvalimg2);

      

  output_img1->set_image(output_rsrc_img1);
  output_img2->set_image(output_rsrc_img2);
  output_data_[0].push_back(output_img1);
  output_data_[0].push_back(output_img2);

  
  
   
//  delete  [] errorlist;
  
  
  delete curve_compare;
  return true;
}


//: Finish
bool
vidpro1_plane_opt_process::finish()
{
  // I'm not really sure what this is for, ask Amir
  // - mleotta
  return true;
}

double
vidpro1_plane_opt_process::compute_err(const vil_image_view<float>  &left_image_view_filt, const vil_image_view<float>  &right_image_view_filt,
                                      vcl_vector<vnl_double_4> &WorldPoints
                                      ,const vnl_double_4x4 &BoundBoxTransf,int &num_of_samples)
{
if (WP_point_samp)
{
    double error = 0.0;
    mapleft.fill(0.0);
    mapright.fill(0.0);
    int pixels_inside = 0;
    int pixels_outside  = 0;
    num_of_samples = WorldPoints.size();
    for (int i = 0; i<num_of_samples; i++)
    {
        vnl_double_3  LeftProjPoint = LeftProjMat*WorldPoints[i];
        vnl_double_3  RightProjPoint = RightProjMat*WorldPoints[i];
        vgl_homg_point_2d<double> point(LeftProjPoint[0]/LeftProjPoint[2],LeftProjPoint[1]/LeftProjPoint[2]);
        vgl_homg_point_2d<double> right_point(RightProjPoint[0]/RightProjPoint[2],RightProjPoint[1]/RightProjPoint[2]);
        double diff;
        


        int xcoordleft = (int)point.x();
        int ycoordleft = (int)point.y();
        int xcoordright = (int)right_point.x();
        int ycoordright = (int)right_point.y();

        if (
         ((xcoordleft<0)||(ycoordleft<0)||(xcoordleft>=left_image_view_filt.ni())||(ycoordleft>=left_image_view_filt.nj()))
         ||
         ((xcoordright<0)||(ycoordright<0)||(xcoordright>=right_image_view_filt.ni())||(ycoordright>=right_image_view_filt.nj()))
         )
         {
            diff = 1.5;
            error +=  (diff*diff);
            continue;
            
            
         }
        else
        if  ((left_mask_view(xcoordleft, ycoordleft)==vxl_byte(0)) || (right_mask_view(xcoordright, ycoordright)==vxl_byte(0)) )
        {
            diff = .9;
            pixels_outside++;
    
        }

        else
        {
            double leftval  = brip_vil_float_ops::bilinear_interpolation(left_image_view_filt,point.x(),point.y());
            double rightval  = brip_vil_float_ops::bilinear_interpolation(right_image_view_filt,right_point.x(),right_point.y());
            diff = leftval-rightval;
            pixels_inside++;

        }

            error+= (diff*diff);
            mapleft( xcoordleft,ycoordleft) = 1.0;
            mapright(xcoordright,ycoordright) = 1.0;
         
  }

   

 double m, n;

 vil_math_sum(m,mapleft, 0);
 vil_math_sum(n,mapright, 0);
 vcl_cout<<"\n m is "<<m<<"\t n is  "<<n <<"\n";
// vcl_cout<<" inside  "<<pixels_inside<<"    outside:  "<<pixels_outside<<"\n";
 
double normalize = MIN(m,n);
 double normalized_error;
 

 if (normalize != 0.0)
 {
     normalized_error = (error/normalize);
     return normalized_error;
 }
 else
     return 1E9;
}
else
{int num_of_res = 0;
 double m, n;


 mapleft.fill(0.0);
 mapright.fill(0.0);
 double error_left = 0.0;
 double error_right = 0.0;
 double x1,y1;
 double diff;
 vnl_double_4 test_point(1.0,0.0,0.0,1.0);
 vnl_double_4 test_BB_point =BoundBoxTransf*test_point;
 compute_plane_params(BoundBoxTransf);
 vnl_svd<double> svd_decomp(LeftProjMat);
 vnl_double_4x3 psuedoinverse = svd_decomp.pinverse(3);
 vnl_double_4  CameraCenter = svd_decomp.nullvector();
 double denom = dot_product(Planeparam,CameraCenter);
 int pixels_inside = 0;
 
 int pixels_outside = 0;

 for (int i = 0; i<left_image_view_filt.ni(); i++)
     for (int j = 0; j<left_image_view_filt.nj(); j++)
     {
         diff = 0.0;   
        assert(i<left_mask_view.ni());
        assert(j<left_mask_view.nj());
         if  (left_mask_view(i, j)!=vxl_byte(0))
         {
             x1 = (double)i; 
             y1 = (double)j;
             vnl_vector<double> homog_coords(3);
           
             vgl_homg_point_2d<double> orig_pt(x1,y1,1.0);
#if 0
             vnl_double_3 orig_point(x1,y1,1.0);
             vnl_double_4 back_proj_point = psuedoinverse * orig_point;
             double num = dot_product(Planeparam, back_proj_point);
             double lambda = -num/denom;
             vnl_double_4 threeD_point = back_proj_point+lambda*CameraCenter;

             vsol_point_3d_sptr newpoint =new vsol_point_3d(threeD_point[0]/threeD_point[3],
                            threeD_point[1]/threeD_point[3],threeD_point[2]/threeD_point[3]);

             homog_coords[0] = newpoint->y();
             homog_coords[1] = newpoint->z();
             homog_coords[2] = 1.0;


             planar_curve_reconst_problem *reconstructor = new  planar_curve_reconst_problem (BoundBoxTransf,orig_point,LeftProjMat);
             vnl_levenberg_marquardt  *LM_instance= new vnl_levenberg_marquardt(*((vnl_least_squares_function*)(reconstructor)));

             LM_instance->minimize_without_gradient(homog_coords);
             //   LM_instance->diagnose_outcome(vcl_cout);
             vnl_double_4 newpt (test_BB_point[0],homog_coords[0]/homog_coords[2], homog_coords[1]/homog_coords[2],1.0);
             vnl_double_3 proj_point = RightProjMat*newpt;
      
             vgl_homg_point_2d<double> right_point(proj_point[0]/proj_point[2],proj_point[1]/proj_point[2]);
            // vcl_cout<<"right_point :: "<<right_point;
             assert(right_point.x()<right_mask_view.ni());
             assert(right_point.y()<right_mask_view.nj()); 
             assert(right_point.x()>0);
             assert(right_point.y()>0);
         
            

             

             delete reconstructor;
             delete LM_instance;
#endif 
             vgl_homg_point_2d<double> right_point = Left2Right(orig_pt);
             if  ((( right_point.x()/right_point.w())<0.0)||(( right_point.y()/right_point.w())<0.0)||
                 (( right_point.x()/right_point.w())>right_image_view_filt.ni())||(( right_point.y()/right_point.w())>right_image_view_filt.nj()))
                 continue;
              num_of_res++;
             if (right_mask_view(int( right_point.x()/right_point.w()),int( right_point.y()/right_point.w()) )==vxl_byte(0))
             {
                 diff = 0.9;
                 pixels_inside++;
             }
             else
             {
             
             pixels_inside++;
            
             

         
             double leftval  = left_image_view_filt(i,j);
             double rightval  = brip_vil_float_ops::bilinear_interpolation(right_image_view_filt,
                 right_point.x()/right_point.w(), right_point.y()/right_point.w());

             diff = leftval-rightval;
             }
             mapleft(int( right_point.x()/right_point.w()),int( right_point.y()/right_point.w()) ) = 1.0;
}
    error_left+= diff*diff;
  }
      vil_math_sum(m,mapleft ,0);
     error_left/=m;
    // vcl_cout<<"pixels_insidE:: "<<pixels_inside<<"\n";
    // vcl_cout<<"pixels_outsidE:: "<<pixels_outside<<"\n";
     pixels_inside =0;
     pixels_outside = 0;
     vnl_svd<double> svd_decomp_R(RightProjMat);
     psuedoinverse = svd_decomp_R.pinverse(3);
     CameraCenter = svd_decomp_R.nullvector();
     denom = dot_product(Planeparam,CameraCenter);
     for (int i = 0; i<right_image_view_filt.ni(); i++)
         for (int j = 0; j<right_image_view_filt.nj(); j++)
         {
             diff = 0.0;
             assert(i<right_mask_view.ni());
             assert(j<right_mask_view.nj());

             if  ((right_mask_view(i, j)!=vxl_byte(0)))
             {
                  vnl_vector<double> homog_coords(3);
                 x1 = (double)i; 
                 y1 = (double)j;
                 vnl_double_3 orig_point(x1,y1,1.0);
                 vgl_homg_point_2d<double> orig_pt(x1,y1,1.0);
#if 0
                 vnl_double_4 back_proj_point = psuedoinverse * orig_point;
                 double num = dot_product(Planeparam, back_proj_point);
                 double lambda = -num/denom;
                 vnl_double_4 threeD_point =  back_proj_point+lambda*CameraCenter;

                 vsol_point_3d_sptr newpoint =new vsol_point_3d(threeD_point[0]/threeD_point[3],
                     threeD_point[1]/threeD_point[3],threeD_point[2]/threeD_point[3]);

                 homog_coords[0] = newpoint->y();
                 homog_coords[1] = newpoint->z();
                 homog_coords[2] = 1.0;
                 planar_curve_reconst_problem *reconstructor = new  planar_curve_reconst_problem (BoundBoxTransf,orig_point,RightProjMat);
                 vnl_levenberg_marquardt  *LM_instance= new vnl_levenberg_marquardt(*((vnl_least_squares_function*)(reconstructor)));
                 LM_instance->minimize_without_gradient(homog_coords);
             //    LM_instance->diagnose_outcome(vcl_cout);
                 vnl_double_4 newpt(test_BB_point[0], homog_coords[0]/homog_coords[2], homog_coords[1]/homog_coords[2],1.0);
                 vnl_double_3 proj_point= LeftProjMat*newpt;
                 vgl_homg_point_2d<double> left_point(proj_point[0]/proj_point[2],proj_point[1]/proj_point[2]);
               //  vcl_cout<<"left_point :: "<<left_point;
                 assert(left_point.x()<left_mask_view.ni());
                 assert(left_point.y()<left_mask_view.nj());
                 assert(left_point.x()>0);
                 assert(left_point.y()>0);
                
                 pixels_inside++;
                
                 delete reconstructor;
                 delete LM_instance;

#endif
                 vgl_homg_point_2d<double> left_point = Left2Right.preimage(orig_pt);
                  if  ((( left_point.x()/left_point.w() )<0.0)||(( left_point.y()/left_point.w())<0.0)||
                      (( left_point.x()/left_point.w() )>left_image_view_filt.ni())||(( left_point.y()/left_point.w())>left_image_view_filt.nj()))
                      continue;    
                  num_of_res++;
                  if (left_mask_view(int( left_point.x()/left_point.w()),int( left_point.y()/left_point.w()) )==vxl_byte(0))
                    {  diff = 0.9;
                  pixels_inside++;
                    }
                    else
                    {
                    
                  pixels_inside++;
                  double rightval  = right_image_view_filt(i,j);
                  double leftval  = brip_vil_float_ops::bilinear_interpolation(left_image_view_filt,
                      left_point.x()/left_point.w(),left_point.y()/left_point.w());
                  diff = leftval-rightval;
                  
                    }
                    
                    mapright(int( left_point.x()/left_point.w()),int( left_point.y()/left_point.w())) = 1.0;

             }
             error_right+= diff*diff;
         }
         vil_math_sum(n,mapright, 0);
    //     vcl_cout<<"pixels_insidE:: "<<pixels_inside<<"\n";
        // vcl_cout<<"pixels_outsidE:: "<<pixels_outside<<"\n";
         error_right/=n;
     //    vcl_cout<<"m is  "<<m<<"  n is "<<n<<"\n";
        
         
         num_of_samples = num_of_res;
         return (error_left+error_right);
}
} 

 void 
     vidpro1_plane_opt_process::InitializeParams(const vnl_double_4x4 &UpdatedBBMatrix, double &d, double &aleph)
 {

     
     trans_vec[0]= UpdatedBBMatrix[0][3];
     trans_vec[1]= UpdatedBBMatrix[1][3];
     trans_vec[2]= UpdatedBBMatrix[2][3];
     trans_vec[3] = 0.0;
     
     compute_plane_params(UpdatedBBMatrix);

     d = trans_vec.two_norm();
     vcl_cout<<"d is  "<<d<<"\n";
     aleph = 0.0;


 }
 void
     vidpro1_plane_opt_process::computeWorldPoints(vcl_vector<vnl_double_4>  &WP, const double sampx,const double sampy,const vnl_double_4x4 &BBMat)
 {
     int  numx = (int)vcl_floor(1.0/sampx);
     int  numy = (int)vcl_floor(1.0/sampy);
     vnl_double_4 newpoint_World;
     vnl_double_4 newp;
       //*****Depends on front plane x=0 or x=1******/////
     newp[0] = 1.0;
       //*****Depends on front plane x=0 or x=1******/////
     newp[3] = 1.0;

     for (int i= 0; i<numx; i++ )
         for (int j=0 ; j<numy; j++)
         {
             newp[1] = i*sampx;
             newp[2] = j*sampy;
             newpoint_World = BBMat*newp;
             WP.push_back(newpoint_World);

         }





 }

 void
     vidpro1_plane_opt_process::updateBBMat(const vnl_double_4x4 &OrigBBMatrix, vnl_double_4x4 &UpdatedBBMatrix,const double updcoef, const double updalpha)
 {
     vnl_double_4 pivot_point;
     bool translate_by_normal = trans_from_KT ;
     

     vnl_double_4 Normal;

 //    vcl_cout<<trans_vec;
   //  temp= RSt;
//     vnl_double_4 Normal(-1.0,0.0,0.0,1.0);
     vnl_double_3x1 tempt, Normaltemp;
     tempt[0][0] = -t_[0];
     tempt[1][0] = -t_[1];
     tempt[2][0] = -t_[2];

     Normaltemp = vnl_inverse(K_)*(tempt);

     Normal[0] = Normaltemp[0][0];
     Normal[1] = Normaltemp[1][0];
     Normal[2] = Normaltemp[2][0];

     
     double d = trans_vec.two_norm();
  //  vcl_cout<<"Normal1:   "<<Normal<<"\n";
//     vcl_cout<<"d:   "<<d;
//     vcl_cout<<"\t t:  "<<t;
    // double coef= (t-d)/d;
//     vcl_cout<<"\tcoef:  "<<updcoef<<"\t"<<"alpha:  "<<updalpha<<"\n";
     vnl_double_4x4 Temp;
     Temp.set_identity();
     
     vnl_double_4x4 T;
     T.set_identity();

     Temp = OrigBBMatrix;
     if (translation)
     {
         if (translate_by_normal)
         {
             Temp[0][3] = trans_vec[0]+updcoef*Normal[0];
             Temp[1][3] = trans_vec[1]+updcoef*Normal[1];
             Temp[2][3] = trans_vec[2]+updcoef*Normal[2];

         }
         else
         {
  //*****Depends on front plane x=0 or x=1******/////
             vnl_double_4 cornerpointone(1.0,0.0,0.0,1.0);
             vnl_double_4 cornerpointtwo(1.0,1.0,0.0,1.0);
             vnl_double_4 cornerpointthr(1.0,0.0,1.0,1.0);
             vnl_double_4 cornerpointfou(1.0,1.0,1.0,1.0);
  //*****Depends on front plane x=0 or x=1******/////
             cornerpointone = OrigBBMatrix*cornerpointone;
             cornerpointtwo = OrigBBMatrix*cornerpointtwo;
             cornerpointthr = OrigBBMatrix*cornerpointthr;
             cornerpointfou = OrigBBMatrix*cornerpointfou;
             vcl_vector<vnl_double_4> corners(4);
             corners[0] = cornerpointone;
             corners[1] = cornerpointtwo;
             corners[2] = cornerpointthr;
             corners[3] = cornerpointfou;
             compute_plane_Normal(corners, Normal);
  //           vcl_cout<<"\nNormal2:   "<<Normal<<"\n";
             double dist = compute_lengthBB(OrigBBMatrix);
             Temp[0][3] = trans_vec[0]+updcoef*Normal[0]*dist;
             Temp[1][3] = trans_vec[1]+updcoef*Normal[1]*dist;
             Temp[2][3] = trans_vec[2]+updcoef*Normal[2]*dist;

         }
     }
  //*****Depends on front plane x=0 or x=1******/////
     vnl_double_4 pivot(1.0,0.5,0.5, 1.0) ;
     pivot_point = Temp*pivot;
     //pivot_point = pivot;
     vnl_double_4 rot_axis(1.0,1.0,0.0, 0.0) ;
     rot_axis = Temp* rot_axis;

       //*****Depends on front plane x=0 or x=1******/////
     vnl_double_4x4 R;




//vcl_cout<<"Temp is  "<<Temp;


//vnl_double_4x4 testR = rot_mat(pivot_point,rot_axis, 0);
//vcl_cout<<"test: (should be identity) "<<testR<<"\n";
     R = rot_mat(pivot_point, rot_axis, updalpha);
    // vcl_cout<<" R is " <<R;
     if (rotation)
         UpdatedBBMatrix = R*Temp;
     else
         UpdatedBBMatrix = Temp;
  //   vcl_cout<<"New Mat:\n"<<UpdatedBBMatrix;
     

 }

vnl_double_4x4
     vidpro1_plane_opt_process::rotX_mat(const double rad) {
         
         vnl_double_4x4 result;
         result.set_identity();
         
         result[1][1] = cos(rad);
         result[2][2] = cos(rad);
                
         result[2][1] = sin(rad);

                
         result[1][2] = -1.0*sin(rad);
                




             return result;
     };

 // Returns a rotation matrix effecting rotation around the Y axis by
 // specified radians
vnl_double_4x4
     vidpro1_plane_opt_process::rotY_mat(const double radians) {
        vnl_double_4x4 rotY ;
        rotY.set_identity();
    
             rotY[0][0] = cos(radians);
             rotY[2][2] = cos(radians);
        
             rotY[0][2] = sin(radians);
             rotY[2][0] = -sin(radians);


        
             return rotY;

     };

 // Returns a rotation matrix effecting rotation around the Z axis by
 // specified radians
vnl_double_4x4
     vidpro1_plane_opt_process::rotZ_mat(const double radians) {
        
        vnl_double_4x4 rotZ;
        rotZ.set_identity();
            
             rotZ[0][0] = cos(radians);
             rotZ[1][1] = cos(radians);
             rotZ[1][0] = sin(radians);
             rotZ[0][1] = -1.0*sin(radians);

            
             return rotZ;
     };

 // Returns a rotation matrix effecting a rotation around the given vector and
 // point, by the specified number of radians.
 vnl_double_4x4
     vidpro1_plane_opt_process::rot_mat(const vnl_double_4 &p, const  vnl_double_4 &v, const double a){


         double theta;
         double phi;



         theta = -1.00*atan2(v[0],v[2]);
         vnl_double_4 axaz(v[0], 0.0 , v[2], 0.0);
         double magn = axaz.two_norm();
         phi = atan2(v[1],magn);
         vnl_double_4 invp;
         invp[0] = -p[0];
         invp[1] = -p[1];
         invp[2] = -p[2];
         invp[3] = -p[3];


         vnl_double_4x4 T= (trans_mat(invp));
    

         vnl_double_4x4 A = rotY_mat(theta);
    

         vnl_double_4x4  B  = rotX_mat(phi);
    

         vnl_double_4x4  C = rotZ_mat(a);
    
         vnl_double_4x4 Binv = rotX_mat(-phi);
    
         vnl_double_4x4  Ainv = rotY_mat(-theta);
    


         vnl_double_4x4  Tinv = (trans_mat(p));
    
         vnl_double_4x4  eye ;
         eye.set_identity();
         assert(A*Ainv==eye);
         //assert(B*Binv==eye);
        // vcl_cout<<"Test rot matrix: (should be Id)";
        // vcl_cout<<B*Binv;
        


         return Tinv*Ainv*Binv*C*B*A*T;



     };  


 vnl_double_4x4
     vidpro1_plane_opt_process::trans_mat(const vnl_double_4 &trans)
 {

     vnl_double_4x4 T;
     T.set_identity();
     T[0][3] = trans[0];
     T[1][3] = trans[1];
     T[2][3] = trans[2];
     return T;

 }





 void
     vidpro1_plane_opt_process::compute_plane_Normal(const vcl_vector<vnl_double_4> & corners, vnl_double_4 &Normal)
 {
     assert(corners.size()==4);


     vnl_double_3 non_homgcorner1, non_homgcorner2, non_homgcorner3;
     non_homgcorner1 = corners[0].extract(3);
     non_homgcorner2 = corners[1].extract(3);
     non_homgcorner3 = corners[2].extract(3);
     vnl_double_3 v1 = non_homgcorner2-non_homgcorner1;
     vnl_double_3 v2 = non_homgcorner3-non_homgcorner1;
     vnl_double_3 N1 = cross_3d(v2,v1);
     double d = dot_product(non_homgcorner1,N1);
     Normal[0] = N1[0]/-d; 
     Normal[1] = N1[1]/-d;
     Normal[2] = N1[2]/-d;
     Normal[3] = 0.0;
    
     Normal.normalize();
     



 }
 double 
     vidpro1_plane_opt_process::compute_lengthBB(const vnl_double_4x4 &BBMat)
 {
     vnl_double_4 orig(0.0,0.0,0.0,1.0);
     vnl_double_4 axisx(1.0,0.0,0.0,1.0);
     orig= BBMat*orig;
     axisx = BBMat*axisx;
     vnl_double_4 diffx = axisx-orig;
     return diffx.two_norm();

 }

 void
     vidpro1_plane_opt_process::compute_plane_params(const vnl_double_4x4 &BBmatrix)
 {
     //*****Depends on front plane x=0 or x=1******/////
     vnl_double_4 cornerpointone(1.0,0.0,0.0,1.0);
     vnl_double_4 cornerpointtwo(1.0,1.0,0.0,1.0);
     vnl_double_4 cornerpointthr(1.0,0.0,1.0,1.0);
     vnl_double_4 cornerpointfou(1.0,1.0,1.0,1.0);
  //*****Depends on front plane x=0 or x=1******/////
     cornerpointone = BBmatrix*cornerpointone;
     cornerpointtwo = BBmatrix*cornerpointtwo;
     cornerpointthr = BBmatrix*cornerpointthr;
     cornerpointfou = BBmatrix*cornerpointfou;



     //// Calculating Homographies

     vgl_h_matrix_2d_compute_4point hcl;
     vcl_vector <vgl_homg_point_2d <double> > point_set1, point_set2;
     vnl_double_3 projcponeL = LeftProjMat*cornerpointone;
     vnl_double_3 projcptwoL = LeftProjMat*cornerpointtwo;
     vnl_double_3 projcpthrL = LeftProjMat*cornerpointthr;
     vnl_double_3 projcpfouL = LeftProjMat*cornerpointfou;
     point_set1.push_back(vgl_homg_point_2d<double>(projcponeL[0]/projcponeL[2],projcponeL[1]/projcponeL[2]));
     point_set1.push_back(vgl_homg_point_2d<double>(projcptwoL[0]/projcptwoL[2],projcptwoL[1]/projcptwoL[2]));
     point_set1.push_back(vgl_homg_point_2d<double>(projcpthrL[0]/projcpthrL[2],projcpthrL[1]/projcpthrL[2]));
     point_set1.push_back(vgl_homg_point_2d<double>(projcpfouL[0]/projcpfouL[2],projcpfouL[1]/projcpfouL[2]));


     point_set2.push_back(vgl_homg_point_2d<double>(cornerpointone[1],cornerpointone[2]));
     point_set2.push_back(vgl_homg_point_2d<double>(cornerpointtwo[1],cornerpointtwo[2]));
     point_set2.push_back(vgl_homg_point_2d<double>(cornerpointthr[1],cornerpointthr[2]));
     point_set2.push_back(vgl_homg_point_2d<double>(cornerpointfou[1],cornerpointfou[2]));

     Hleft = hcl.compute(point_set1, point_set2);
     point_set1.clear();

     vnl_double_3 projcponeR = RightProjMat*cornerpointone;
     vnl_double_3 projcptwoR = RightProjMat*cornerpointtwo;
     vnl_double_3 projcpthrR = RightProjMat*cornerpointthr;
     vnl_double_3 projcpfouR = RightProjMat*cornerpointfou;
     point_set1.push_back(vgl_homg_point_2d<double>(projcponeR[0]/projcponeR[2],projcponeR[1]/projcponeR[2]));
     point_set1.push_back(vgl_homg_point_2d<double>(projcptwoR[0]/projcptwoR[2],projcptwoR[1]/projcptwoR[2]));
     point_set1.push_back(vgl_homg_point_2d<double>(projcpthrR[0]/projcpthrR[2],projcpthrR[1]/projcpthrR[2]));
     point_set1.push_back(vgl_homg_point_2d<double>(projcpfouR[0]/projcpfouR[2],projcpfouR[1]/projcpfouR[2]));

     // H represents the homography that
     // transforms points from  plane1 into plane2.
     Hright = hcl.compute(point_set1, point_set2);
   //  vcl_cout<<Hleft<<"\n"<<Hright<<"\n";
     Left2Right = Hleft*Hright.get_inverse();






     vcl_vector<vnl_double_4> corners(4);
     corners[0] = cornerpointone;
     corners[1] = cornerpointtwo;
     corners[2] = cornerpointthr;
     corners[3] = cornerpointfou;


     vnl_double_3 non_homgcorner1, non_homgcorner2, non_homgcorner3;
     non_homgcorner1 = corners[0].extract(3);
     non_homgcorner2 = corners[1].extract(3);
     non_homgcorner3 = corners[2].extract(3);
     vnl_double_3 v1 = non_homgcorner2-non_homgcorner1;
     vnl_double_3 v2 = non_homgcorner3-non_homgcorner1;
     vnl_double_3 N1 = cross_3d(v2,v1);
     double d = dot_product(non_homgcorner1,N1);
     Planeparam[0] = N1[0]/-d; 
     Planeparam[1] = N1[1]/-d;
     Planeparam[2] = N1[2]/-d;
     Planeparam[3] = 1.0;

     

 }



