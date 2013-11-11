// This is contrib/sadali/gui/vidpro1_BB_visualize_process.cxx
#define MIN(a,b)  ((a<b) ? a : b)
//:
// \file

#include "vidpro1_BB_visualize_process.h"
#include <bpro1/bpro1_parameters.h>

// include storage classes needed
// such as ...

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h> 
#include <vidpro1/storage/vidpro1_image_storage.h>

// other includes needed
#include <vcl_cmath.h>

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

#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_3x3.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/vnl_matrix_fixed.h>

#include <vgl/vgl_homg_point_2d.h>


//: Constructor
vidpro1_BB_visualize_process::vidpro1_BB_visualize_process()
{
  // Set up the parameters for this process

  if (
      (!parameters()->add( "BB Filename" , "-bb_fname" , (vcl_string)"c://taxi/bbox_cam.txt" ) )
      ||
      (!parameters()->add( "Get BB from file" , "-use_BBfile" , (bool)true ) )
      ||
    
      (!parameters()->add( "Number of Columns in image" , "-col" , (int)1024 ) )
      ||
      (!parameters()->add( "Number of Rows in image" , "-row" , (int)768 ) )
   //   ||
   //   (!parameters()->add( "sampling in WorldPlane for Proj in X" , "-sx" , (float).02 ) )
   //   ||
  //    (!parameters()->add( "sampling in WorldPlane for Proj in Y" , "-sy" , (float).04 ) )
  //    ||
#if 0
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
      (!parameters()->add( "PL03" , "-PL03" , (float)0.00       ) )
      
      ||
      (!parameters()->add( "PL11" , "-PL11" , (float)2000.0     ) )
      ||
      (!parameters()->add( "PL12" , "-PL12" , (float)384.0      ) )
      ||
 //     (!parameters()->add( "PL13" , "-PL13" , (float).00        ) )
 //     ||
 //     (!parameters()->add( "PL23" , "-PL23" , (float).00        ) )
 //     ||
 


      (!parameters()->add( "PR03" , "-PR03"  , (float)610.071     ) )

      ||
     
      (!parameters()->add( "PR13" , "-PR13"  , (float)-237.358    ) )
      ||

      (!parameters()->add( "PR23" , "-PR23"  , (float)-1.00 ) )
   
    
     
#endif

      )
          
  {
    vcl_cerr << "ERROR: Adding parameters in vidpro1_BB_visualize_process::vidpro1_BB_visualize_process()" << vcl_endl;
  }

}



//: Destructor
vidpro1_BB_visualize_process::~vidpro1_BB_visualize_process()
{
}


//: Return the name of this process
vcl_string
vidpro1_BB_visualize_process::name()
{
  return "BB 2D Projection";
}


bpro1_process *
vidpro1_BB_visualize_process::clone() const
{
    return new vidpro1_BB_visualize_process(*this);

};
//: Return the number of input frame for this process
int
vidpro1_BB_visualize_process::input_frames()
{
  // input from this frame 
        return 1;
}


//: Return the number of output frames for this process
int
vidpro1_BB_visualize_process::output_frames()
{
  // output to this frame only
return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > 
vidpro1_BB_visualize_process::get_input_type()
{
  // this process looks for  vsol2D storage classes
  // at each input frame
 
  vcl_vector< vcl_string > to_return;


  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > 
vidpro1_BB_visualize_process::get_output_type()
{  
  // this process produces a vsol2D storage class
        
  vcl_vector<vcl_string > to_return;
 
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  
  return to_return;
}


//: Execute the process
bool
vidpro1_BB_visualize_process::execute()
{
  // verify that the number of input frames is correct
  if ( input_data_.size() != 1 ){
    vcl_cout << "In vidpro1_BB_visualize_process::execute() - not exactly one"
             << " input frames" << vcl_endl;
    return false;
  }
 


  //Initialize Normal, alpha and t parameters

 // Get parameters

  float samplexf, sampleyf;
  double samplex, sampley;
 // parameters()->get_value("-sx",samplexf);
 // parameters()->get_value("-sy",sampleyf);
  samplexf = 0.02f;
  sampleyf = 0.04f;
samplex= samplexf;
sampley = sampleyf;

  
  parameters()->get_value("-row",row);
  parameters()->get_value("-col",col);


  vnl_double_3x4 CurProjMatL;
  vnl_double_3x4 CurProjMatR;
  vnl_double_4 CameraCenter;
  vnl_double_4x4 BBMatrix;

  float tempf;
 #if 0
   parameters()->get_value("-BB00",tempf);
   BBMatrix[0][0] = tempf;
    parameters()->get_value("-BB01",tempf);
    BBMatrix[0][1] = tempf;
     parameters()->get_value("-BB02",tempf);
     BBMatrix[0][2] = tempf;
      parameters()->get_value("-BB03",tempf);
      BBMatrix[0][3] = tempf;
       parameters()->get_value("-BB10",tempf);
       BBMatrix[1][0] = tempf;
        parameters()->get_value("-BB11",tempf);
        BBMatrix[1][1] = tempf;
         parameters()->get_value("-BB12",tempf);
         BBMatrix[1][2] = tempf;
          parameters()->get_value("-BB13",tempf);
          BBMatrix[1][3] = tempf;
           parameters()->get_value("-BB20",tempf);
           BBMatrix[2][0] = tempf;
            parameters()->get_value("-BB21",tempf);
            BBMatrix[2][1] = tempf;
             parameters()->get_value("-BB22",tempf);
             BBMatrix[2][2] = tempf;
              parameters()->get_value("-BB23",tempf);
              BBMatrix[2][3] = tempf;
   BBMatrix[3][0] = BBMatrix[3][1] =BBMatrix[3][2] = 0.0;
   BBMatrix[3][3] = 1.0;
   
 //  parameters()->get_value("-PL00",tempf);
   CurProjMatL[0][0] = 2000;
   //parameters()->get_value("-PL01",tempf);
   CurProjMatL[0][1] = 0.0;
  // parameters()->get_value("-PL02",tempf);
   CurProjMatL[0][2] = 512;
 
       //parameters()->get_value("-PL03",tempf);
   CurProjMatL[0][3] = 0.0;
   //parameters()->get_value("-PL10",tempf);
   CurProjMatL[1][0] = 0.0;
  // parameters()->get_value("-PL11",tempf);
   CurProjMatL[1][1] = 2000;
  // parameters()->get_value("-PL12",tempf);
   CurProjMatL[1][2] = 384;
 //  parameters()->get_value("-PL13",tempf);
   CurProjMatL[1][3] = 0.0;
   //parameters()->get_value("-PL20",tempf);
   CurProjMatL[2][0] = 0.0;
  // parameters()->get_value("-PL21",tempf);
   CurProjMatL[2][1] = 0.0;
 //  parameters()->get_value("-PL22",tempf);
   CurProjMatL[2][2] = 1.0;
  // parameters()->get_value("-PL23",tempf);
   CurProjMatL[2][3] = 0.0;

   CurProjMatR =CurProjMatL;
  
   parameters()->get_value("-PR03",tempf);
   CurProjMatR[0][3] = tempf;
   
   parameters()->get_value("-PR13",tempf);
   CurProjMatR[1][3] = tempf;
  
   parameters()->get_value("-PR23",tempf);
   CurProjMatR[2][3] = tempf;
   
#endif

   bool use_bbfile;
   parameters()->get_value("-use_BBfile",use_bbfile);
   vcl_string BB_fname;
   parameters()->get_value("-bb_fname",BB_fname);
   vcl_ifstream BB_file(BB_fname.c_str(), vcl_ios::in);
   vcl_string str("");
   while (str!="Transform")
   {BB_file>>str;
   }

   vnl_double_4x4 Temp;
   BB_file>>Temp;
   if (use_bbfile)
   {
       BBMatrix = Temp;


   //Initialize error parameters


   while (str!="Camera")
   {BB_file>>str;
   }
   //  BB_file>>str;
   int camno,camno2;
   BB_file>>camno;
   BB_file>>CurProjMatL;
   BB_file>>str;
   BB_file>>camno2;
   BB_file>>CurProjMatR;
   BB_file>>str;
   }
  //InitializeParams(BBMatrix, t, alpha);
  vcl_vector<vnl_double_4> WorldPoints;
 
 //computeWorldPoints(WorldPoints,  samplex, sampley,BBMatrix, 1) ;
  vcl_cout<<CurProjMatL<<"\n"<<CurProjMatR<<"\n";
  vcl_cout<<"BB Matr:  "<<BBMatrix;
   

  //Compute errors
  vil_image_view<vxl_byte> greyvalimg1(col, row );
  vil_image_view<vxl_byte> greyvalimg2(col, row );
  greyvalimg1.fill(vxl_byte(0));
  greyvalimg2.fill(vxl_byte(0));
  vcl_vector<vsol_spatial_object_2d_sptr> project_L,project_R;
  vcl_vector<vsol_spatial_object_2d_sptr> secproject_L,secproject_R;
  vcl_vector<vsol_spatial_object_2d_sptr> thirdproject_L , thirdproject_R;
  vcl_vector<vsol_spatial_object_2d_sptr> fourthproject_L , fourthproject_R;
  
computeWorldPoints(WorldPoints,  samplex, sampley,BBMatrix, 1) ;
 Compute_proj(project_L,project_R,CurProjMatL, CurProjMatR, WorldPoints);
 WorldPoints.clear();
 computeWorldPoints(WorldPoints,  samplex, sampley,BBMatrix, 2) ;
  Compute_proj(secproject_L,secproject_R,CurProjMatL, CurProjMatR, WorldPoints);
  WorldPoints.clear();
  computeWorldPoints(WorldPoints,  samplex, sampley,BBMatrix, 3) ;
   WorldPoints.clear();
    Compute_proj(thirdproject_L,thirdproject_R,CurProjMatL, CurProjMatR, WorldPoints);
  computeWorldPoints(WorldPoints,  samplex, sampley,BBMatrix, 4) ;
  Compute_proj(fourthproject_L,fourthproject_R,CurProjMatL, CurProjMatR, WorldPoints);

 


  
  

  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_img1 = vidpro1_vsol2D_storage_new();
  vidpro1_vsol2D_storage_sptr output_img2 = vidpro1_vsol2D_storage_new();
  output_img1->add_objects(project_L,"Front PlaneL");
  output_img2->add_objects(project_R,"Front PlaneR");

  output_img1->add_objects(secproject_L,"Front PlaneL");
  output_img2->add_objects(secproject_R,"Front PlaneR");
  
 output_img1->add_objects(thirdproject_L,"Origin");
  output_img2->add_objects(thirdproject_R,"Origin");
  output_img1->add_objects(fourthproject_L,"Origin");
  output_img2->add_objects(fourthproject_R,"Origin");
  
      


  output_data_[0].push_back(output_img1);
  output_data_[0].push_back(output_img2);

  
  
   
  
  return true;
}


//: Finish
bool
vidpro1_BB_visualize_process::finish()
{
  // I'm not really sure what this is for, ask Amir
  // - mleotta
  return true;
}

double
vidpro1_BB_visualize_process::Compute_proj(vcl_vector<vsol_spatial_object_2d_sptr> &left_image_project, vcl_vector<vsol_spatial_object_2d_sptr>  &right_image_project,
                                      const vnl_double_3x4 &LeftProjMat , const vnl_double_3x4 &RightProjMat ,const  vcl_vector<vnl_double_4> &WorldPoints)
{
    double error = 0.0;
 for (int i = 0; i<WorldPoints.size(); i++)
 {
     vnl_double_3  LeftProjPoint = LeftProjMat*WorldPoints[i];
     vnl_double_3  RightProjPoint = RightProjMat*WorldPoints[i];
     vgl_homg_point_2d<double> point(LeftProjPoint[0]/LeftProjPoint[2],LeftProjPoint[1]/LeftProjPoint[2]);
     vgl_homg_point_2d<double> right_point(RightProjPoint[0]/RightProjPoint[2],RightProjPoint[1]/RightProjPoint[2]);
      if (
         ((point.x()<0)||(point.y()<0)||(point.x()>=col)||(point.y()>row))
         ||
         ((right_point.x()<0)||(right_point.y()<0)||(right_point.x()>=col)||(right_point.y()>row))
         )
         continue;
     
    left_image_project.push_back (( new vsol_point_2d(point.x(), point.y() ))->cast_to_spatial_object());
     right_image_project.push_back( ( new vsol_point_2d( right_point.x(), right_point.y() ) )->cast_to_spatial_object());
    
          
  }
 
 return error;

} 
#if 0 
 void 
     vidpro1_BB_visualize_process::InitializeParams(const vnl_double_4x4 &UpdatedBBMatrix, double &d, double &aleph)
 {
     vnl_double_3x3 RS = UpdatedBBMatrix.extract(3,3);
     vnl_matrix_fixed<double,3,1> RSt;
     vnl_matrix_fixed<double,3,1> temp;
    // RSt[0][0] = UpdatedBBMatrix[0][3];
  //   RSt[1][0] = UpdatedBBMatrix[1][3];
   //  RSt[2][0] = UpdatedBBMatrix[2][3];
     // vnl_double_3x3  inverse = vnl_matrix_inverse<double>(RS);
//      vcl_cout<<" the inverse of Rs is"<<inverse;
      
    // temp= RSt; //inverse *RSt;
     trans_vec[0]=UpdatedBBMatrix[0][3];
     trans_vec[1]= UpdatedBBMatrix[1][3];
     trans_vec[2]= UpdatedBBMatrix[2][3];
     trans_vec[3] = 0.0;
     d = trans_vec.two_norm();
     vcl_cout<<"d is  "<<d<<"\n";
     aleph = 0.0;

    


 }
#endif
 void
     vidpro1_BB_visualize_process::computeWorldPoints(vcl_vector<vnl_double_4>  &WP, const double sampx,const double sampy,const vnl_double_4x4 &BBMat, int plane_index)
 {
     int  numx = (int)vcl_floor(1.0/sampx);
     int  numy = (int)vcl_floor(1.0/sampy);
     for (int i= 0; i<numx; i++ )
         for (int j=0 ; j<numy; j++)
         {
             vnl_double_4 newp;
             if (plane_index==1)
             {
                 newp[0] = 0.0;
                 newp[1] = i*sampx;
                 newp[2] = j*sampy;
                 newp[3]  = 1.0;
             }
             if (plane_index==2)
             {
                 newp[0] = i*sampx;
                 newp[1] = 1.0;
                 newp[2] = j*sampy;
                 newp[3]  = 1.0;
             }
             if (plane_index==3)
             {
                 newp[0] = 0.0;
                 newp[1] = 0.0;
                 newp[2] = 0.0;
                 newp[3]  = 1.0;
                 vnl_double_4 newpoint_World = BBMat*newp;
                 WP.push_back(newpoint_World);
                 return;
             }
                  if (plane_index==4)
             {
                 newp[0] = 1.0;
                 newp[1] = i*sampx;
                 newp[2] = j*sampy;
                 newp[3]  = 1.0;
             }

             vnl_double_4 newpoint_World = BBMat*newp;
             WP.push_back(newpoint_World);

         }

    //vcl_cout<<" Number of sample points on front plane  "<<WP.size();


 }

#if 0 
vnl_double_4x4
     vidpro1_BB_visualize_process::rotX_mat(const double rad) {
         // TODO: fill this in
         vnl_double_4x4 result;
         result.set_identity();
         
         for (int i = 0; i<4; i++)
             for (int j = 0; j<4; j++)
             {
                 if (((i==2)&&(j==2))||((i==1)&&(j==1)))
                     result[i][j] = cos(rad);
                 else 
                     if ((i==2)&&(j==1))
                         result[i][j] = sin(rad);

                     else if ((i==1)&&(j==2))
                         result[i][j] = -1.0*sin(rad);
                     else 
                         if (i==j)
                             result[i][j] = 1.0;
                         else 
                             result[i][j] =0.0;



             }


             // TODO: this return value is bogus
             return result;
     };

 // Returns a rotation matrix effecting rotation around the Y axis by
 // specified radians
vnl_double_4x4
     vidpro1_BB_visualize_process::rotY_mat(const double radians) {
        vnl_double_4x4 rotY ;
        rotY.set_identity();
         for (int i = 0; i<4; i++)
             for (int j = 0; j<4; j++)
             { if (((i==0)&&(j==0))||((i==2)&&(j==2)))
             rotY[i][j] = cos(radians);
             else
                 if ((i==0)&&(j==2))
                     rotY[i][j] = sin(radians);
                 else if ((i==2)&&(j==0))
                     rotY[i][j] = -1.0*sin(radians);

                 else
                     if (i==j)
                         rotY[i][j] = 1.0;
                     else
                         rotY[i][j] = 0.0;


             }

             // TODO: this return value is bogus
             return rotY;

     };

 // Returns a rotation matrix effecting rotation around the Z axis by
 // specified radians
vnl_double_4x4
     vidpro1_BB_visualize_process::rotZ_mat(const double radians) {
        
        vnl_double_4x4 rotZ;
        rotZ.set_identity();
         for (int i = 0; i<4; i++)
             for (int j = 0; j<4; j++)
             { if (((i==0)&&(j==0))||((i==1)&&(j==1)))
             rotZ[i][j] = cos(radians);
             else
                 if ((i==1)&&(j==0))
                     rotZ[i][j] = sin(radians);
                 else if ((i==0)&&(j==1))
                     rotZ[i][j] = -1.0*sin(radians);

                 else
                     if (i==j)
                         rotZ[i][j] = 1.0;
                     else
                         rotZ[i][j] = 0.0;


             }

            
             return rotZ;
     };

 // Returns a rotation matrix effecting a rotation around the given vector and
 // point, by the specified number of radians.
 vnl_double_4x4
     vidpro1_BB_visualize_process::rot_mat(const vnl_double_4 &p, const  vnl_double_4 &v, const double a){


         double theta;
         double phi;



         theta = -1.00*atan2(v[0],v[2]);
         vnl_double_4 axaz(v[0], 0.0 , v[2], 0.0);
         double magn = axaz.two_norm();
         phi = atan2(v[1],magn);


         vnl_double_4x4 T= (trans_mat(-1.0*p));



        vnl_double_4x4 A = rotY_mat(theta);




         vnl_double_4x4  B  = rotX_mat(phi);


         vnl_double_4x4  C = rotZ_mat((double)a);


         vnl_double_4x4 Binv = rotX_mat(-phi);


         vnl_double_4x4  Ainv = rotY_mat(-theta);


         vnl_double_4x4  Tinv = (trans_mat(p));


         return Tinv*Ainv*Binv*C*B*A*T;


         // TODO: this return value is bogus

     };  


 vnl_double_4x4
     vidpro1_BB_visualize_process::trans_mat(const vnl_double_4 &trans)
 {

     vnl_double_4x4 T;
     T.set_identity();
     T[0][3] = trans_vec[0];
     T[1][3] = trans_vec[1];
     T[2][3] = trans_vec[2];
     return T;

 }
#endif  
