#include "dbbgm_bgmodel_edges_process.h"
#include <bseg/brip/brip_vil_float_ops.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vnl/vnl_math.h>
#include <vil/vil_plane.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <dbsta2/dbsta2_attributes.h>
#include <dbsta2/dbsta2_mixture.h>
#include <dbsta2/dbsta2_gauss_f1.h>
#include <dbsta2/dbsta2_gauss_ff3.h>
#include <dbsta2/dbsta2_gaussian_x_y_theta.h>
#include <dbsta2/algo/dbsta2_adaptive_updater.h>
#include <sdet/sdet_nonmax_suppression_params.h>
#include <sdet/sdet_nonmax_suppression.h>
#include <dbbgm/pro/dbbgm_image_storage.h>
#include <dbbgm/pro/dbbgm_image_storage_sptr.h>
#include <dbbgm/dbbgm_update.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>
#include <dbsta2/dbsta2_gaussian_angles_1d.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_edgel.h>

#include <vnl/vnl_random.h>
dbbgm_bgmodel_edges_process::dbbgm_bgmodel_edges_process() : bpro_process()
{
    if( !parameters()->add( "Max Num Components"         ,   "-maxcmp"      ,  int(3) ) ||
        !parameters()->add( "Initial Variance (angle)"   ,   "-initv"       ,   1.0f  ) ||
        !parameters()->add( "Initial Variance (x)"       ,   "-initvarx"    ,   0.5f  ) ||
        !parameters()->add( "Initial Variance (y)"       ,   "-initvary"    ,   0.5f  ) ||
        !parameters()->add( "Initial Weight"             ,   "-initw"       ,   0.1f  ) ||
        !parameters()->add( "Is 3D"                      ,   "-is3D"        ,  (bool) true )
        )
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
}


/*************************************************************************
* Function Name: dbbgm_bgmodel_edges_process::~dbbgm_bgmodel_edges_process
* Parameters: 
* Effects: 
*************************************************************************/

dbbgm_bgmodel_edges_process::~dbbgm_bgmodel_edges_process()
{
   
}


//: Clone the process
bpro1_process*
dbbgm_bgmodel_edges_process::clone() const
{
    return new dbbgm_bgmodel_edges_process(*this);
}


/*************************************************************************
* Function Name: dbbgm_bgmodel_edges_process::name
* Parameters: 
* Returns: vcl_string
* Effects: 
*************************************************************************/
vcl_string
dbbgm_bgmodel_edges_process::name()
{
    return "Bg Modeling of Edges";
}


/*************************************************************************
* Function Name: ddbil_osl_canny_edges_process::get_input_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > dbbgm_bgmodel_edges_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "edge_map" );
    to_return.push_back( "dbbgm_image" );
    return to_return;
}


/*************************************************************************
* Function Name: dbbgm_bgmodel_edges_process::get_output_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > dbbgm_bgmodel_edges_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    return to_return;
}
//: Returns the number of input frames to this process
int
dbbgm_bgmodel_edges_process::input_frames()
{
    return 1;
}


//: Returns the number of output frames from this process
int
dbbgm_bgmodel_edges_process::output_frames()
{
    return 1;
}


/*************************************************************************
* Function Name: ddbil_osl_canny_edges_process::execute
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
//bool
//dbbgm_bgmodel_edges_process::execute()
//{
//    if ( input_data_.size() != 1 ){
//        vcl_cout << "In dbbgm_bgmodel_edges_process::execute() - not exactly one"
//            << " input image \n";
//        return false;
//    }
//    clear_output();
//    // get image from the storage class
//
//    vidpro1_image_storage_sptr frame_image;
//    frame_image.vertical_cast(input_data_[0][0]);
//
//    dbdet_edgemap_storage_sptr input_edgemap;
//    input_edgemap.vertical_cast(input_data_[0][1]);
//
//
//
//    vil_image_resource_sptr image_sptr = frame_image->get_image();
//    vil_image_view<unsigned char> grey_img=brip_vil_float_ops::convert_to_byte(image_sptr);
//
//    vil_image_view<double> grad_x, grad_y;
//    vil_sobel_3x3 <unsigned char,double > (grey_img, grad_x, grad_y);
//
//    static float thresh;
//    parameters()->get_value( "-thresh" ,thresh);
//    sdet_nonmax_suppression_params nonmax_paprams(thresh,0);
//
//    static sdet_nonmax_suppression_params nsp;
//    nsp.thresh_= thresh;
//    nsp.pfit_type_ = 0;
//
//    sdet_nonmax_suppression ns(nsp, grad_x, grad_y);
//    ns.apply();
//
//    
//    vil_image_view<float> dir_img(image_sptr->ni(),image_sptr->nj(),3);
//    dir_img.fill(-100.0);
//    //vcl_vector<dbrl_id_point_2d_sptr> pointids;
//
//    vcl_vector<vsol_point_2d_sptr> edges=ns.get_points();
//    //vcl_vector<double> tangents=det.get_tangents();
//    vcl_vector<vgl_vector_2d<double> > dirs=ns.get_directions();
//    for(unsigned i=0;i<edges.size();i++)
//        {
//        dir_img((unsigned int)vcl_floor(edges[i]->x()),(unsigned int)vcl_floor(edges[i]->y()),0)=edges[i]->x();
//        dir_img((unsigned int)vcl_floor(edges[i]->x()),(unsigned int)vcl_floor(edges[i]->y()),1)=edges[i]->y();
//        double dir=vcl_atan2(dirs[i].y(),dirs[i].x());
//        dir_img((unsigned int)vcl_floor(edges[i]->x()),(unsigned int)vcl_floor(edges[i]->y()),2)=dir;
//        }
//
//    vcl_cout<<"Computed Edges for "<<frame_image->frame()<<" .. # Edges : "<<edges.size()<<"\n";
//
//    vil_image_view<float> directionplane=vil_plane(dir_img,2);
//
//    //: bg modeling code
//
//    float init_var = 0.0f;
//    int max_components=3; //unused, window_size=0;
//    parameters()->get_value( "-maxcmp" ,  max_components );
//    parameters()->get_value( "-initv"  ,  init_var );
//
//    dbbgm_image_storage_sptr frame_distimg;
//    frame_distimg.vertical_cast(input_data_[0][1]);
//
//    typedef dbsta2_num_obs<dbsta2_gauss_f1> gauss_type;
//    typedef dbsta2_num_obs<dbsta2_mixture<gauss_type> > mix_gauss_type;
//
//    dbsta2_gauss_f1 init_gauss(0.0f,init_var);
//    dbsta2_mg_statistical_updater<dbsta2_gauss_f1> updater(init_gauss, max_components);
//
//    typedef dbsta2_mg_statistical_updater<dbsta2_gauss_f1> mgstat;
//    //dbsta2_updater_value_threshold_wrapper<mgstat,dbsta2_gauss_f1,1> updater_wrapper(updater,0.5f);
//    if(!frame_distimg){
//        mix_gauss_type mixture;
//        model_ = new dbbgm_image_of<mix_gauss_type>(directionplane.ni(),directionplane.nj(),mixture);
//        dbbgm_image_storage_sptr frame_distimg = dbbgm_image_storage_new(model_);
//        frame_distimg->mark_global();
//        input_data_[0][1] = frame_distimg;
//
//    }
//    else
//    {
//        model_ = frame_distimg->dist_image();
//        // This marks the data as modified so the GUI will redraw
//        output_data_[0].push_back(frame_distimg);
//    }
//
//
//    dbbgm_image_of<mix_gauss_type> *model = static_cast<dbbgm_image_of<mix_gauss_type>*>(model_.ptr());
//    update(*model,directionplane,updater);
// 
//    //vidpro1_image_storage_sptr output_img_storage = vidpro1_image_storage_new();
//    //output_img_storage->set_image(vil_new_image_resource_of_view(dir_img ) );
//    //output_data_[0].push_back(output_img_storage);
//    //output_img_storage->set_name("Dir OSL Canny Edges");
//
////clear_input();
//return true;
//}
//
//
//
//
bool
dbbgm_bgmodel_edges_process::execute()
{
    if ( input_data_.size() != 1 ){
        vcl_cout << "In dbbgm_bgmodel_edges_process::execute() - not exactly one"
            << " input image \n";
        return false;
    }
    clear_output();
    // get image from the storage class


    dbdet_edgemap_storage_sptr input_edgemap;
    input_edgemap.vertical_cast(input_data_[0][0]);

    dbdet_edgemap_sptr edgemap=input_edgemap->get_edgemap();
    vcl_cout<<" .. # Edges : "<<edgemap->num_edgels<<"\n";

    //vil_image_view<float> directionplane=vil_plane(dir_img,2);

    //: bg modeling code

    float init_var = 0.0f;
    int max_components=3; //unused, window_size=0;
    bool is3D=true;
    parameters()->get_value( "-maxcmp" ,  max_components );
    parameters()->get_value( "-initv"  ,  init_var );
    parameters()->get_value( "-is3D"   ,  is3D );


    dbbgm_image_storage_sptr frame_distimg;
    frame_distimg.vertical_cast(input_data_[0][1]);

    if(is3D)
    {
        //typedef dbsta2_num_obs<dbsta2_gauss_ff3> gauss_type_3d;
        //typedef dbsta2_num_obs<dbsta2_mixture<gauss_type_3d> > mix_gauss_type_3d;
        
                typedef dbsta2_num_obs<dbsta2_gaussian_x_y_theta> gauss_type_3d;
        typedef dbsta2_num_obs<dbsta2_mixture<gauss_type_3d> > mix_gauss_type_3d;

        float init_var_x=0.5;
        float init_var_y=0.5;

        parameters()->get_value( "-initvarx"  ,  init_var_x );
        parameters()->get_value( "-initvary"  ,  init_var_y );
        vnl_vector_fixed<float,3> mean(0.0);

        vnl_matrix_fixed<float,3,3> init_covariance;
        init_covariance.set_identity();
        init_covariance(0,0)=init_var_x;
        init_covariance(1,1)=init_var_y;
        init_covariance(2,2)=init_var;

        //dbsta2_gauss_ff3 init_gauss(mean,init_covariance);
        //dbsta2_mg_statistical_updater<dbsta2_gauss_ff3> updater(init_gauss, max_components);
        //typedef dbsta2_mg_statistical_updater<dbsta2_gauss_f1> mgstat;

        dbsta2_gaussian_x_y_theta init_gauss(mean,init_covariance);
        dbsta2_mg_statistical_updater<dbsta2_gaussian_x_y_theta> updater(init_gauss, max_components,3.0,0.0);
        typedef dbsta2_mg_statistical_updater<dbsta2_gaussian_x_y_theta> mgstat;

        if(!frame_distimg){
            mix_gauss_type_3d mixture;
            model_ = new dbbgm_image_of<mix_gauss_type_3d>(edgemap->width(),edgemap->height(),mixture);
            dbbgm_image_storage_sptr frame_distimg = dbbgm_image_storage_new(model_);
            frame_distimg->mark_global();
            input_data_[0][1] = frame_distimg;

        }
        else
        {
            model_ = frame_distimg->dist_image();
            // This marks the data as modified so the GUI will redraw
            output_data_[0].push_back(frame_distimg);
        }


        dbbgm_image_of<mix_gauss_type_3d> *model = static_cast<dbbgm_image_of<mix_gauss_type_3d>*>(model_.ptr());
        vbl_array_2d<vcl_vector<dbdet_edgel*> >::iterator iter=edgemap->edge_cells.begin();

        for(unsigned i=0;i<edgemap->edge_cells.rows();i++)
        {
            for(unsigned j=0;j<edgemap->edge_cells.cols();j++)
            {
                vcl_vector<dbdet_edgel*> es=edgemap->edge_cells(i,j);
                if(es.size()==0)
                {
                    mix_gauss_type_3d * dist=&(*model)(j,i);
                    vnl_vector_fixed<float,3> lobound(-10.0);
                    updater(*dist,lobound);//,init_covariance);
                }
                for(unsigned k=0;k<es.size();k++)
                {
                    vgl_point_2d<double> pt=es[k]->pt;
                    double angle=es[k]->tangent;
                    vnl_vector_fixed<float,3> data(-10.0);
                    data[0]=pt.x();
                    data[1]=pt.y();
                    data[2]=angle;

                    //vnl_matrix_fixed<float,3,3> R;
                    //vnl_matrix_fixed<float,3,3> final_covariance;
                    //R(0,0)=vcl_cos(angle);R(0,1)=-vcl_sin(angle);R(0,2)=0.0;
                    //R(1,0)=vcl_sin(angle);R(1,1)=vcl_cos(angle);R(1,2)=0.0;
                    //R(2,0)=0.0;R(2,1)=0.0;R(2,2)=1.0;
                    //final_covariance=init_covariance*R;


                    //mix_gauss_type_3d * dist1=&(*model)(vcl_floor(pt.x()),vcl_floor(pt.y()));
                    updater((*model)(vcl_floor(pt.x()),vcl_floor(pt.y())),data);//,final_covariance);
                    

                   // mix_gauss_type_3d * dist2=&(*model)(vcl_ceil(pt.x()),vcl_floor(pt.y()));
                    updater((*model)(vcl_ceil(pt.x()),vcl_floor(pt.y())),data);//,final_covariance);

                   // mix_gauss_type_3d * dist3=&(*model)(vcl_floor(pt.x()),vcl_ceil(pt.y()));
                    updater((*model)(vcl_floor(pt.x()),vcl_ceil(pt.y())),data);//,final_covariance);


                   // mix_gauss_type_3d * dist4=&(*model)(vcl_ceil(pt.x()),vcl_ceil(pt.y()));
                    updater((*model)(vcl_ceil(pt.x()),vcl_ceil(pt.y())),data);//,final_covariance);
                }
            }
        }
    }
    else
    {
        typedef dbsta2_num_obs<dbsta2_gaussian_angles_1d> gauss_type;
        typedef dbsta2_num_obs<dbsta2_mixture<gauss_type> > mix_gauss_type;

        dbsta2_gaussian_angles_1d init_gauss(0.0f,init_var);
        dbsta2_mg_statistical_updater<dbsta2_gaussian_angles_1d> updater(init_gauss, max_components);

        typedef dbsta2_mg_statistical_updater<dbsta2_gaussian_angles_1d> mgstat;

        //typedef dbsta2_num_obs<dbsta2_gauss_f1> gauss_type;
        //typedef dbsta2_num_obs<dbsta2_mixture<gauss_type> > mix_gauss_type;

        //dbsta2_gauss_f1 init_gauss(0.0f,init_var);
        //dbsta2_mg_statistical_updater<dbsta2_gauss_f1> updater(init_gauss, max_components);

        //typedef dbsta2_mg_statistical_updater<dbsta2_gauss_f1> mgstat;
 
        //dbsta2_updater_value_threshold_wrapper<mgstat,dbsta2_gauss_f1,1> updater_wrapper(updater,0.5f);
        if(!frame_distimg){
            mix_gauss_type mixture;
            model_ = new dbbgm_image_of<mix_gauss_type>(edgemap->width(),edgemap->height(),mixture);
            dbbgm_image_storage_sptr frame_distimg = dbbgm_image_storage_new(model_);
            frame_distimg->mark_global();
            input_data_[0][1] = frame_distimg;

        }
        else
        {
            model_ = frame_distimg->dist_image();
            // This marks the data as modified so the GUI will redraw
            output_data_[0].push_back(frame_distimg);
        }
        dbbgm_image_of<mix_gauss_type> *model = static_cast<dbbgm_image_of<mix_gauss_type>*>(model_.ptr());
        vbl_array_2d<vcl_vector<dbdet_edgel*> >::iterator iter=edgemap->edge_cells.begin();

        vnl_random rand;
        for(unsigned i=0;i<edgemap->edge_cells.rows();i++)
        {
            for(unsigned j=0;j<edgemap->edge_cells.cols();j++)
            {
                vcl_vector<dbdet_edgel*> es=edgemap->edge_cells(i,j);
                if(es.size()==0)
                {
                    mix_gauss_type * dist=&(*model)(j,i);
                    updater(*dist,-10);
                }
                for(unsigned k=0;k<es.size();k++)
                {
                    vgl_point_2d<double> pt=es[k]->pt;
                    double angle=es[k]->tangent;
                    mix_gauss_type * dist=&(*model)(vcl_floor(pt.x()),vcl_floor(pt.y()));
                    updater(*dist,angle);
                    dist=&(*model)(vcl_ceil(pt.x()),vcl_floor(pt.y()));
                    updater(*dist,angle);
                    dist=&(*model)(vcl_ceil(pt.x()),vcl_ceil(pt.y()));
                    updater(*dist,angle);
                    dist=&(*model)(vcl_floor(pt.x()),vcl_ceil(pt.y()));
                    updater(*dist,angle);

                    //                    mix_gauss_type * dist1=&(*model)(vcl_ceil(pt.x()),vcl_floor(pt.y()));
                    //updater(*dist1,angle);

                }
            }
        }
    }
    //update(*model,directionplane,updater);
 
    //vidpro1_image_storage_sptr output_img_storage = vidpro1_image_storage_new();
    //output_img_storage->set_image(vil_new_image_resource_of_view(dir_img ) );
    //output_data_[0].push_back(output_img_storage);
    //output_img_storage->set_name("Dir OSL Canny Edges");

//clear_input();
return true;
}



/*************************************************************************
* Function Name: dbbgm_bgmodel_edges_process::finish
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
dbbgm_bgmodel_edges_process::finish()
{
    return true;
}


