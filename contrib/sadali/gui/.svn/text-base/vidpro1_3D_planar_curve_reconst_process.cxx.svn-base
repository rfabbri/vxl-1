// This is contrib/sadali/vidpro1_my_process.cxx

//:
// \file

#include "vidpro1_3D_planar_curve_reconst_process.h"
#include <bpro1/bpro1_parameters.h>

// include storage classes needed
// such as ...
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vtol_storage.h>
#include <vidpro1/storage/vidpro1_vtol_storage_sptr.h>

// other includes needed
#include <bmvl/brct/brct_algos.h>
#include <bseg/brip/brip_vil_float_ops.h>
#include <bprt/bprt_plane_reconst.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>

#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/algo/vgl_h_matrix_2d_compute_4point.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_inverse.h>
#include <vnl/algo/vnl_matrix_inverse.h>
#include <vnl/algo/vnl_levenberg_marquardt.h>

#include <vdgl/vdgl_edgel_chain_sptr.h>

#include <vil/vil_new.h>

#include <gui/planar_curve_reconst_problem.h>





//: Constructor
vidpro1_3D_planar_curve_reconst_process::vidpro1_3D_planar_curve_reconst_process()
{
    if (
        (!parameters()->add( "BB Filename" , "-BB_fname" , (vcl_string)"c:\\taxi\bbox_cam.txt" ) )
        ||
        (!parameters()->add("Output Filename" , "-ptsfname", (vcl_string)"c:\\curves.vrml" ))
        ||
        (!parameters()->add( "Get BB from file" , "-use_BBfile" , (bool)false ) )
        ||

    // Set up the parameters for this process
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
     //   ||
     //   (!parameters()->add( "PL03" , "-PL03" , (float)0.00 ) )

        ||
        (!parameters()->add( "PL11" , "-PL11" , (float)2000.0 ) )
        ||
        (!parameters()->add( "PL12" , "-PL12" , (float)384.0 ) )
        ||
   //     (!parameters()->add( "PL13" , "-PL13" , (float).00 ) )
   //     ||
        //  (!parameters()->add( "PL23" , "-PL23" , (float).00 ) )
        //  ||

        (!parameters()->add( "PR03" , "-PR03"  , (float)610.071) )

        ||

        (!parameters()->add( "PR13" , "-PR13"  , (float)-237.358 ) )
       

        //  (!parameters()->add( "PR23" , "-PR23"  , (float)-1.00 ) )
        //      ||
       



        )


    {
        vcl_cerr << "ERROR: Adding parameters in vidpro1_3D_planar_curve_reconst_process::vidpro1_3D_planar_curve_reconst_process()" << vcl_endl;
    }
    initialized = false;
}



//: Destructor
vidpro1_3D_planar_curve_reconst_process::~vidpro1_3D_planar_curve_reconst_process()
{
}


//: Return the name of this process
vcl_string
vidpro1_3D_planar_curve_reconst_process::name()
{
    return "3D Planar Curve Reconstruction";
}


bpro1_process *
vidpro1_3D_planar_curve_reconst_process::clone() const
{
    return new vidpro1_3D_planar_curve_reconst_process(*this);

};
//: Return the number of input frame for this process
int
vidpro1_3D_planar_curve_reconst_process::input_frames()
{
    // input from this frame 
    return 1;
}


//: Return the number of output frames for this process
int
vidpro1_3D_planar_curve_reconst_process::output_frames()
{
    // output to this frame only
    return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > 
vidpro1_3D_planar_curve_reconst_process::get_input_type()
{
    // this process looks for an image and vsol2D storage class
    // at each input frame
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "vtol" );
    to_return.push_back( "vtol" );
    to_return.push_back( "image" );
    to_return.push_back( "image" );
    return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > 
vidpro1_3D_planar_curve_reconst_process::get_output_type()
{  
    // this process produces a vsol2D storage class
    vcl_vector<vcl_string > to_return;

    return to_return;
}


//: Execute the process
bool
vidpro1_3D_planar_curve_reconst_process::execute()
{
    // verify that the number of input frames is correct
    if ( input_data_.size() != 1 ){
        vcl_cout << "In vidpro1_3D_planar_curve_reconst_process::execute() - not exactly two"
            << " input frames" << vcl_endl;
        return false;
    }
    clear_output();
    if (!initialized)
   initialize();
   
    vnl_double_4 CameraCenter;
    
    vcl_vector<vnl_double_4> WorldPoints;


    vnl_double_4 Normal;
    vcl_cout<<"Computing planeparams for BBmatrix =\n"<<BBMatrix;

    compute_plane_params(BBMatrix,Normal);
  //  vcl_cout<<"Normal is "<<Normal<<"\n";
    vnl_double_4 test_point(1.0,0.0,0.0,1.0);
    vnl_double_4 test_BB_point =BBMatrix*test_point;

    vnl_double_4 reconst_point;
    vnl_double_4x4 PN;
    PN.set_row(0, CurProjMatL.get_row(0));
    PN.set_row(1, CurProjMatL.get_row(1));
    PN.set_row(2, CurProjMatL.get_row(2));
    PN.set_row(3, Normal);
    vnl_double_4x4 PNinverse = vnl_matrix_inverse<double>(PN);
    vnl_svd<double> svd_decomp(CurProjMatL);
    vnl_double_4x3 psuedoinverse = svd_decomp.pinverse(3);
    CameraCenter = svd_decomp.nullvector();
    double denom = dot_product(Normal,CameraCenter);
   // vcl_cout<<"Psuedoinv"<< psuedoinverse;
   // vcl_cout<<"\nCameraCenter:\t"<<CameraCenter<<"\n";
   //  vcl_cout<<"Denom:\t"<<denom;
    vnl_double_3x3 test_homog = CurProjMatL.extract(3,3,0,1) ;
    vnl_double_3x3 inv_test_homog = vnl_matrix_inverse<double>(test_homog);

    vcl_vector<vsol_point_3d_sptr> left_curve_reconst;
    vcl_vector<vsol_point_3d_sptr> right_curve_reconst;

  

    // get vtol and image from the storage classes
    vidpro1_vtol_storage_sptr left_img_edge; 
    left_img_edge.vertical_cast(input_data_[0][0]);
    vidpro1_vtol_storage_sptr right_img_edge; 
    right_img_edge.vertical_cast(input_data_[0][1]);
    vidpro1_image_storage_sptr left_mask_stor; 
    left_mask_stor.vertical_cast(input_data_[0][2]);
    vidpro1_image_storage_sptr right_mask_stor; 
    right_mask_stor.vertical_cast(input_data_[0][3]);
    vil_image_resource_sptr left_mask_res = left_mask_stor->get_image();
    vil_image_resource_sptr right_mask_res = right_mask_stor->get_image();

    vcl_vector < vsol_spatial_object_2d_sptr > origpts;
    
    vcl_set<vtol_topology_object_sptr>::const_iterator u=left_img_edge->begin();
    vcl_set<vtol_topology_object_sptr>::const_iterator n=right_img_edge->begin();
    vil_image_view<vxl_byte> left_mask = left_mask_res->get_view();
    vil_image_view<vxl_byte> right_mask = left_mask_res->get_view();

    double x1,y1;
    vnl_vector<double> param_vec(3);
    param_vec[2] = 1.0;


    for ( ;u!=left_img_edge->end(); u++)
    {

        vtol_edge_2d_sptr edge = (*u)->cast_to_edge()->cast_to_edge_2d();
        vsol_curve_2d * curve =edge->curve().ptr();

        vdgl_digital_curve_sptr dc= curve->cast_to_vdgl_digital_curve();

        vdgl_interpolator_sptr itrp = dc->get_interpolator();
        vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

        unsigned int chain_size = ech->size();

        vdgl_edgel_chain_sptr newcurve;
        for (unsigned int i=0; i<chain_size;i++)
        {

            vdgl_edgel ed = (*ech)[i];
            x1 = ed.get_x(); 
            y1 = ed.get_y();
            if (left_mask((int)x1, (int)y1)==vxl_byte(255))
            {
                vgl_homg_point_2d<double> orig_point(x1,y1,1.0);
            #if 0
            vnl_double_3 proj_point(x1,y1,1.0);


            vnl_double_4 back_proj_point = psuedoinverse * proj_point;
            double num = dot_product(Normal, back_proj_point);
            double lambda = -num/denom;

            vnl_double_4 threeD_point =  back_proj_point+lambda*CameraCenter;
         

            vsol_point_3d_sptr newpoint =new vsol_point_3d(threeD_point[0]/threeD_point[3],
                threeD_point[1]/threeD_point[3],threeD_point[2]/threeD_point[3]);

            param_vec[0] = newpoint->y();
            param_vec[1] = newpoint->z();
            param_vec[2] = 1.0;
       
            planar_curve_reconst_problem *reconstructor = new  planar_curve_reconst_problem (BBMatrix,proj_point,CurProjMatL);
            vnl_levenberg_marquardt  *LM_instance= new vnl_levenberg_marquardt(*((vnl_least_squares_function*)(reconstructor)));
            LM_instance->minimize_without_gradient(param_vec);
            LM_instance->diagnose_outcome(vcl_cout);
            vsol_point_3d_sptr newpt = new vsol_point_3d(test_BB_point[0],
                param_vec[0]/param_vec[2], param_vec[1]/param_vec[2]);
#endif
            vgl_homg_point_2d<double> proj_point = Hleft(orig_point);
            vsol_point_3d_sptr newpt = new vsol_point_3d(test_BB_point[0],
                proj_point.x()/proj_point.w(), proj_point.y()/proj_point.w());

             left_curve_reconst.push_back(newpt);


            }
         
        }
        
    }
    
 
   vnl_svd<double> svd_decomp_R(CurProjMatR);
    psuedoinverse = svd_decomp_R.pinverse(3);
  CameraCenter = svd_decomp_R.nullvector();
    denom = dot_product(Normal,CameraCenter);
  //  vcl_cout<<"CameraCenter:\t"<<CameraCenter<<"\n";
 

    for ( ;n!=right_img_edge->end(); n++)
    {


        vtol_edge_2d_sptr edge = (*n)->cast_to_edge()->cast_to_edge_2d();
        vsol_curve_2d * curve =edge->curve().ptr();

        vdgl_digital_curve_sptr dc= curve->cast_to_vdgl_digital_curve();

        vdgl_interpolator_sptr itrp = dc->get_interpolator();
        vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

        unsigned int chain_size = ech->size();



        for (unsigned int i=0; i<chain_size;i++)
        {

            vdgl_edgel ed = (*ech)[i];

            x1 = ed.get_x(); 
            y1 = ed.get_y();
            if (right_mask((int)x1, (int)y1)==vxl_byte(255))
            {
                vgl_homg_point_2d<double> orig_point(x1,y1,1.0);
#if 0
            vnl_double_3 proj_point(x1,y1,1.0);

            vnl_double_4 back_proj_point = psuedoinverse * proj_point;
            double num = dot_product(Normal, back_proj_point);
            double lambda = -num/denom;

            vnl_double_4 threeD_point = back_proj_point+lambda*CameraCenter;

            vsol_point_3d_sptr newpoint =new vsol_point_3d(threeD_point[0]/threeD_point[3],
                threeD_point[1]/threeD_point[3],threeD_point[2]/threeD_point[3]);
         
            param_vec[0] = newpoint->y();
            param_vec[1] = newpoint->z();
            param_vec[2] = 1.0;
            planar_curve_reconst_problem *reconstructor = new  planar_curve_reconst_problem (BBMatrix,proj_point,CurProjMatR);
            vnl_levenberg_marquardt  *LM_instance= new vnl_levenberg_marquardt(*((vnl_least_squares_function*)(reconstructor)));
            LM_instance->minimize_without_gradient(param_vec);
            LM_instance->diagnose_outcome(vcl_cout);
            vsol_point_3d_sptr newpt = new vsol_point_3d(test_BB_point[0],
                param_vec[0]/param_vec[2], param_vec[1]/param_vec[2]);
            delete LM_instance;
            delete reconstructor;
#endif        

            vgl_homg_point_2d<double> proj_point = Hright(orig_point);
            vsol_point_3d_sptr newpt = new vsol_point_3d(test_BB_point[0],
                proj_point.x()/proj_point.w(), proj_point.y()/proj_point.w());
        
            right_curve_reconst.push_back(newpt);
            }


        }
    }
    vcl_string fname;
    parameters()->get_value("-ptsfname",fname);
  //  vcl_ofstream leftcurvepts((fname+vcl_string("left")).c_str(),vcl_ios::out);
  //  vcl_ofstream rightcurvepts((fname+vcl_string("right")).c_str(),vcl_ios::out);
   

    vcl_ofstream vrml_stream_comb((fname+vcl_string(".wrl")).c_str(),vcl_ios::out);
  
    vnl_double_3 red(1.0,0.0,0.0);
    vnl_double_3 green(0.0,1.0,0.0);
    
    brct_algos::write_vrml_header(vrml_stream_comb);
    brct_algos::write_vrml_points(vrml_stream_comb,left_curve_reconst);
    brct_algos::write_vrml_points(vrml_stream_comb,right_curve_reconst);

    vrml_stream_comb.close();
    initialized = false;


#if 0
    leftcurvepts<<"\n";
    for (unsigned int i= 0; i<left_curve_reconst.size(); i++)
    {
        leftcurvepts<<left_curve_reconst[i]->x()<<"\t"<<left_curve_reconst[i]->y()<<"\t"<<left_curve_reconst[i]->z()<<"\n";
    }
    rightcurvepts<<"\n";
    for (unsigned int i= 0; i<right_curve_reconst.size(); i++)
    {
        rightcurvepts<<right_curve_reconst[i]->x()<<"\t"<<right_curve_reconst[i]->y()<<"\t"<<right_curve_reconst[i]->z()<<"\n";
    }
    //get parameters
    leftcurvepts.close();
    rightcurvepts.close();
#endif
    return true;
    }


//: Finish
bool
vidpro1_3D_planar_curve_reconst_process::finish()
{
    // I'm not really sure what this is for, ask Amir
    // - mleotta
    return true;
}

void
vidpro1_3D_planar_curve_reconst_process::compute_plane_params(const vnl_double_4x4  &newBBmatrix, vnl_double_4 &Normal)
{

    ////Depends on the front plane x= 0 and x=1
    //Initialize error parameters
    vnl_double_4 cornerpointone(1.0,0.0,0.0,1.0);
    vnl_double_4 cornerpointtwo(1.0,1.0,0.0,1.0);
    vnl_double_4 cornerpointthr(1.0,0.0,1.0,1.0);
    vnl_double_4 cornerpointfou(1.0,1.0,1.0,1.0);
    ////////////////////////////////////////
    cornerpointone = BBMatrix*cornerpointone;
    cornerpointtwo = BBMatrix*cornerpointtwo;
    cornerpointthr = BBMatrix*cornerpointthr;
    cornerpointfou = BBMatrix*cornerpointfou;
    cornerpointone = cornerpointone/cornerpointone[3];
    cornerpointtwo = cornerpointtwo/cornerpointtwo[3];
    cornerpointthr = cornerpointthr/cornerpointthr[3];
    cornerpointfou = cornerpointfou/cornerpointfou[3];
    vgl_h_matrix_2d_compute_4point hcl;
    vcl_vector <vgl_homg_point_2d <double> > point_set1, point_set2;
    vnl_double_3 projcponeL = CurProjMatL*cornerpointone;
    vnl_double_3 projcptwoL = CurProjMatL*cornerpointtwo;
    vnl_double_3 projcpthrL = CurProjMatL*cornerpointthr;
    vnl_double_3 projcpfouL = CurProjMatL*cornerpointfou;
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
      
        vnl_double_3 projcponeR = CurProjMatR*cornerpointone;
        vnl_double_3 projcptwoR = CurProjMatR*cornerpointtwo;
        vnl_double_3 projcpthrR = CurProjMatR*cornerpointthr;
        vnl_double_3 projcpfouR = CurProjMatR*cornerpointfou;
        point_set1.push_back(vgl_homg_point_2d<double>(projcponeR[0]/projcponeR[2],projcponeR[1]/projcponeR[2]));
        point_set1.push_back(vgl_homg_point_2d<double>(projcptwoR[0]/projcptwoR[2],projcptwoR[1]/projcptwoR[2]));
        point_set1.push_back(vgl_homg_point_2d<double>(projcpthrR[0]/projcpthrR[2],projcpthrR[1]/projcpthrR[2]));
        point_set1.push_back(vgl_homg_point_2d<double>(projcpfouR[0]/projcpfouR[2],projcpfouR[1]/projcpfouR[2]));

    // H represents the homography that
    // transforms points from  plane1 into plane2.
 Hright = hcl.compute(point_set1, point_set2);
 vcl_cout<<Hleft<<"\n"<<Hright<<"\n";




   
    vcl_vector<vnl_double_4> corners(4);
    corners[0] = cornerpointone;
    corners[1] = cornerpointtwo;
    corners[2] = cornerpointthr;
    corners[3] = cornerpointfou;
    assert(corners.size()==4);
  //  for (int i =0 ;i<4;i++)
  // vcl_cout<<corners[i]<<"\n"; 
    vnl_double_3 non_homgcorner1, non_homgcorner2, non_homgcorner3;
    non_homgcorner1 = corners[0].extract(3);
    non_homgcorner2 = corners[1].extract(3);
    non_homgcorner3 = corners[2].extract(3);
    vnl_double_3 v1 = non_homgcorner2-non_homgcorner1;
    vnl_double_3 v2 = non_homgcorner3-non_homgcorner1;
    vnl_double_3 N1 = cross_3d(v2,v1);
    double d = dot_product(non_homgcorner1,N1);
    vcl_cout<<d;
    Normal[0] = -N1[0]/d; 
    Normal[1] = -N1[1]/d;
    Normal[2] = -N1[2]/d;
    Normal[3] = 1.0;
vcl_cout<<"\nNormal is "<<Normal<<"\n";


}

void
vidpro1_3D_planar_curve_reconst_process::set_BB_matrix(vnl_double_4x4  newBBmatrix)
{
    BBMatrix = newBBmatrix;
}

void 
vidpro1_3D_planar_curve_reconst_process::initialize()
{ 
     initialized = true;
     float tempf;

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
parameters()->get_value("-PL00",tempf);
CurProjMatL[0][0] = tempf;
//parameters()->get_value("-PL01",tempf);
CurProjMatL[0][1] = 0.0;
parameters()->get_value("-PL02",tempf);
CurProjMatL[0][2] = tempf;
//parameters()->get_value("-PL03",tempf);
CurProjMatL[0][3] = 0.0;
//parameters()->get_value("-PL10",tempf);
CurProjMatL[1][0] = 0.0;
parameters()->get_value("-PL11",tempf);
CurProjMatL[1][1] = tempf;
parameters()->get_value("-PL12",tempf);
CurProjMatL[1][2] = tempf;
//parameters()->get_value("-PL13",tempf);
CurProjMatL[1][3] = 0.0;
//parameters()->get_value("-PL20",tempf);
CurProjMatL[2][0] = 0.0;
// parameters()->get_value("-PL21",tempf);
CurProjMatL[2][1] = 0.0;
//  parameters()->get_value("-PL22",tempf);
CurProjMatL[2][2] = 1.0;
//  parameters()->get_value("-PL23",tempf);
CurProjMatL[2][3] = 0.0;

CurProjMatR =CurProjMatL;

parameters()->get_value("-PR03",tempf);
CurProjMatR[0][3] = tempf;

parameters()->get_value("-PR13",tempf);
CurProjMatR[1][3] = tempf;

// parameters()->get_value("-PR23",tempf);
CurProjMatR[2][3] = -1.0;

bool use_BB_file;
parameters()->get_value("-use_BBfile", use_BB_file);


if (use_BB_file)
{

vcl_string BB_fname;
parameters()->get_value("-BB_fname",BB_fname);
vcl_ifstream BB_file(BB_fname.c_str(), vcl_ios::in);
vcl_string str("");
while (str!="Transform")
{BB_file>>str;
}
BB_file>>BBMatrix;


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

}
