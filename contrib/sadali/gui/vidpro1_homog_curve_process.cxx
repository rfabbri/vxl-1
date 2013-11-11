// This is contrib/sadali/vidpro1_my_process.cxx

//:
// \file

#include "vidpro1_homog_curve_process.h"
#include <bpro1/bpro1_parameters.h>

// include storage classes needed
// such as ...
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vtol_storage.h>
#include <vidpro1/storage/vidpro1_vtol_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

// other includes needed
#include <bseg/brip/brip_vil_float_ops.h>
#include <bprt/bprt_plane_reconst.h>
#include <vsol/vsol_spatial_object_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <vgl/algo/vgl_h_matrix_2d.h>
#include <vgl/vgl_homg_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <vnl/vnl_math.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vnl/algo/vnl_svd.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_spatial_object_2d.h>

#include <vil/vil_new.h>





//: Constructor
vidpro1_homog_curve_process::vidpro1_homog_curve_process()
{
    // Set up the parameters for this process
    if (
        (!parameters()->add( "BB Filename" , "-BB_fname" , (vcl_string)"c:\\taxi\bbox_cam.txt" ) )
        ||
        (!parameters()->add("Output Filename" , "-ptsfname", (vcl_string)"c:\\curves.vrml" ))
#if 0
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
        ||
        (!parameters()->add( "PL03" , "-PL03" , (float)0.00 ) )

        ||
        (!parameters()->add( "PL11" , "-PL11" , (float)2000.0 ) )
        ||
        (!parameters()->add( "PL12" , "-PL12" , (float)384.0 ) )
        ||
        (!parameters()->add( "PL13" , "-PL13" , (float).00 ) )
        ||
        //  (!parameters()->add( "PL23" , "-PL23" , (float).00 ) )
        //  ||

        (!parameters()->add( "PR03" , "-PR03"  , (float)610.071) )

        ||

        (!parameters()->add( "PR13" , "-PR13"  , (float)-237.358 ) )


        //  (!parameters()->add( "PR23" , "-PR23"  , (float)-1.00 ) )
        //      ||



#endif
        )


    {
        vcl_cerr << "ERROR: Adding parameters in vidpro1_3D_planar_curve_reconst_process::vidpro1_3D_planar_curve_reconst_process()" << vcl_endl;
    }
}



//: Destructor
vidpro1_homog_curve_process::~vidpro1_homog_curve_process()
{
}


//: Return the name of this process
vcl_string
vidpro1_homog_curve_process::name()
{
    return "Curve Homography";
}


bpro1_process *
vidpro1_homog_curve_process::clone() const
{
    return new vidpro1_homog_curve_process(*this);

};
//: Return the number of input frame for this process
int
vidpro1_homog_curve_process::input_frames()
{
    // input from this frame 
    return 1;
}


//: Return the number of output frames for this process
int
vidpro1_homog_curve_process::output_frames()
{
    // output to this frame only
    return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > 
vidpro1_homog_curve_process::get_input_type()
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
vidpro1_homog_curve_process::get_output_type()
{  
    // this process produces a vsol2D storage class
    vcl_vector<vcl_string > to_return;

    to_return.push_back( "vsol2D" );
    to_return.push_back( "vsol2D" );

    return to_return;
}


//: Execute the process
bool
vidpro1_homog_curve_process::execute()
{
    // verify that the number of input frames is correct
    if ( input_data_.size() != 1 ){
        vcl_cout << "In vidpro1_homog_curve_process::execute() - not exactly two"
            << " input frames" << vcl_endl;
        return false;
    }
    clear_output();


    vnl_double_3x4 CurProjMatL, CurProjMatR;
    // get vtol from the storage classes
    vidpro1_vtol_storage_sptr left_img_edge; 
    left_img_edge.vertical_cast(input_data_[0][0]);
    vidpro1_vtol_storage_sptr right_img_edge; 
    right_img_edge.vertical_cast(input_data_[0][1]);


    //Get necessary matrices from bbox_cam file
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



    //Initialize error parameters
    vnl_double_4 cornerpointone(0.0,0.0,0.0,1.0);
    vnl_double_4 cornerpointtwo(0.0,1.0,0.0,1.0);
    vnl_double_4 cornerpointthr(0.0,0.0,1.0,1.0);
    vnl_double_4 cornerpointfou(0.0,1.0,1.0,1.0);

    cornerpointone = BBMatrix*cornerpointone;
    cornerpointtwo = BBMatrix*cornerpointtwo;
    cornerpointthr = BBMatrix*cornerpointthr;
    cornerpointfou = BBMatrix*cornerpointfou;
    cornerpointone = cornerpointone/cornerpointone[3];
    cornerpointtwo = cornerpointtwo/cornerpointtwo[3];
    cornerpointthr = cornerpointthr/cornerpointthr[3];
    cornerpointfou = cornerpointfou/cornerpointfou[3];
    vcl_vector<vnl_double_4> corners(4);
    corners[0] = cornerpointone;
    corners[1] = cornerpointtwo;
    corners[2] = cornerpointthr;
    corners[3] = cornerpointfou;
    vnl_double_4 Normal;

    compute_plane_params(corners, Normal);

    vnl_svd<double> svd_decomp(CurProjMatL);
    vnl_double_4x3 psuedoinverse = svd_decomp.pinverse(3);
    vnl_double_4 CameraCenter = svd_decomp.nullvector();
    double denom = dot_product(Normal,CameraCenter);


    vcl_vector < vsol_spatial_object_2d_sptr > origpts;
    
    vcl_set<vtol_topology_object_sptr>::const_iterator u=left_img_edge->begin();
    vcl_set<vtol_topology_object_sptr>::const_iterator n=right_img_edge->begin();

    double x1,y1;
    vidpro1_vsol2D_storage_sptr image_curve_of_L;
    vidpro1_vsol2D_storage_sptr image_curve_of_R;
   
     vcl_vector< vsol_spatial_object_2d_sptr > left_proj;
      vcl_vector< vsol_spatial_object_2d_sptr > right_proj;

    for ( ;u!=left_img_edge->end(); u++)
    {


        vtol_edge_2d_sptr edge = (*u)->cast_to_edge()->cast_to_edge_2d();
        vsol_curve_2d * curve =edge->curve().ptr();

        vdgl_digital_curve_sptr dc= curve->cast_to_vdgl_digital_curve();

        vdgl_interpolator_sptr itrp = dc->get_interpolator();
        vdgl_edgel_chain_sptr ech = itrp->get_edgel_chain();

        unsigned int chain_size = ech->size();

        vdgl_edgel_chain_sptr newcurve;
        vcl_vector< vsol_point_2d_sptr > points; 
        for (unsigned int i=0; i<chain_size;i++)
        {

            vdgl_edgel ed = (*ech)[i];
            x1 = ed.get_x(); 
            y1 = ed.get_y();
            vnl_double_3 proj_point(x1,y1,1.0);
            vnl_double_4 back_proj_point = psuedoinverse * proj_point;
            double num = dot_product(Normal, back_proj_point);
            double lambda = -num/denom;
             vnl_double_4 threeD_point =  back_proj_point+lambda*CameraCenter;



            vnl_double_3 image_point = CurProjMatR*threeD_point;


            
            
                vsol_point_2d_sptr newPt = new vsol_point_2d (image_point[0]/image_point[2],image_point[1]/image_point[2]);
                points.push_back(newPt);
           
           

                

           
        }
        vsol_polyline_2d_sptr newContour = new vsol_polyline_2d (points);
        right_proj.push_back(newContour->cast_to_spatial_object());
        
    }



    vnl_svd<double> svd_decomp_R(CurProjMatR);
    psuedoinverse = svd_decomp_R.pinverse(3);
    CameraCenter = svd_decomp_R.nullvector();
    denom = dot_product(Normal,CameraCenter);

  
  
    for ( ;n!=right_img_edge->end(); n++)
    {

        vcl_vector< vsol_point_2d_sptr > points; 

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
            vnl_double_3 proj_point(x1,y1,1.0);
            vnl_double_4 back_proj_point = psuedoinverse * proj_point;
            double num = dot_product(Normal, back_proj_point);
            double lambda = -num/denom;
            vnl_double_4 threeD_point =  back_proj_point+lambda*CameraCenter;


            vnl_double_3 image_point = CurProjMatL*threeD_point;

            vsol_point_2d_sptr newPt = new vsol_point_2d (image_point[0]/image_point[2],image_point[1]/image_point[2]);
            points.push_back(newPt);

        }
        vsol_polyline_2d_sptr newContour = new vsol_polyline_2d (points);
        left_proj.push_back(newContour->cast_to_spatial_object());
    }


   



    vidpro1_vsol2D_storage_sptr output_img1 = vidpro1_vsol2D_storage_new();
    vidpro1_vsol2D_storage_sptr output_img2 = vidpro1_vsol2D_storage_new();
 
    output_img1->add_objects(left_proj);
    output_img2->add_objects(right_proj);
    output_data_[0].push_back(output_img1);
    output_data_[0].push_back(output_img2);

    return true;
}


//: Finish
bool
vidpro1_homog_curve_process::finish()
{
    // I'm not really sure what this is for, ask Amir
    // - mleotta
    return true;
}

void
vidpro1_homog_curve_process::compute_plane_params(const vcl_vector<vnl_double_4> & corners, vnl_double_4 &Normal)
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
    Normal[3] = 1.0;
   


}
