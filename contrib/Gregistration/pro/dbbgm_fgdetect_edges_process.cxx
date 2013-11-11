#include "dbbgm_fgdetect_edges_process.h"
#include <bseg/brip/brip_vil_float_ops.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vnl/vnl_math.h>
#include <vil/vil_plane.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <dbsta2/dbsta2_gaussian_full.h>
#include <dbsta2/dbsta2_gaussian_sphere.h>
#include <dbsta2/dbsta2_detector_gaussian.h>
#include <dbsta2/dbsta2_detector_mixture.h>
#include <dbsta2/dbsta2_basic_functors.h>
#include <dbsta2/dbsta2_gauss_f1.h>
#include <dbsta2/dbsta2_gauss_ff3.h>
#include <dbsta2/dbsta2_gaussian_angles_1d.h>
#include <dbsta2/dbsta2_gaussian_x_y_theta.h>

#include <dbsta2/dbsta2_attributes.h>
#include <dbsta2/dbsta2_mixture.h>
#include <dbsta2/dbsta2_detector_mixture.h>
#include <sdet/sdet_nonmax_suppression_params.h>
#include <sdet/sdet_nonmax_suppression.h>
#include <dbbgm/pro/dbbgm_image_storage.h>
#include <dbbgm/pro/dbbgm_image_storage_sptr.h>
#include <dbbgm/dbbgm_detect.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vil/vil_new.h>
#include <vil/vil_convert.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <dbdet/sel/dbdet_edgel.h>

dbbgm_fgdetect_edges_process::dbbgm_fgdetect_edges_process() : bpro1_process()
{
    if( !parameters()->add( "Distance Theshold" ,    "-dist" ,    2.5f        ) ||
        !parameters()->add( "Minimum Weight Threshold" ,    "-minweightthresh" ,    0.3f        ) ||
        !parameters()->add( "Radius of Uncertainity" ,    "-rad" ,    (int)2       ) 
        )  
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
}


/*************************************************************************
* Function Name: dbbgm_fgdetect_edges_process::~dbbgm_fgdetect_edges_process
* Parameters: 
* Effects: 
*************************************************************************/

dbbgm_fgdetect_edges_process::~dbbgm_fgdetect_edges_process()
{
   
}


//: Clone the process
bpro1_process*
dbbgm_fgdetect_edges_process::clone() const
{
    return new dbbgm_fgdetect_edges_process(*this);
}


/*************************************************************************
* Function Name: dbbgm_fgdetect_edges_process::name
* Parameters: 
* Returns: vcl_string
* Effects: 
*************************************************************************/
vcl_string
dbbgm_fgdetect_edges_process::name()
{
    return "Fg Detection of Edges";
}


/*************************************************************************
* Function Name: ddbil_osl_canny_edges_process::get_input_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > dbbgm_fgdetect_edges_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "edge_map" );
    to_return.push_back( "dbbgm_image" );
    return to_return;
}


/*************************************************************************
* Function Name: dbbgm_fgdetect_edges_process::get_output_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > dbbgm_fgdetect_edges_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "image" );
        to_return.push_back( "image" );
    to_return.push_back( "edge_map" );
    return to_return;
}
//: Returns the number of input frames to this process
int
dbbgm_fgdetect_edges_process::input_frames()
{
    return 1;
}


//: Returns the number of output frames from this process
int
dbbgm_fgdetect_edges_process::output_frames()
{
    return 1;
}


/*************************************************************************
* Function Name: ddbil_osl_canny_edges_process::execute
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
dbbgm_fgdetect_edges_process::execute()
{
    if ( input_data_.size() != 1 ){
        vcl_cout << "In dbbgm_fgdetect_edges_process::execute() - not exactly one"
            << " input image \n";
        return false;
    }
    clear_output();
    // get image from the storage class
    dbdet_edgemap_storage_sptr input_edgemap;
    input_edgemap.vertical_cast(input_data_[0][0]);

    dbdet_edgemap_sptr edgemap=input_edgemap->get_edgemap();
    vcl_cout<<" .. # Edges : "<<edgemap->num_edgels<<"\n";


    dbdet_edgemap_sptr fgmap=fg_detect(edgemap);

    dbdet_edgemap_storage_sptr output_storage = new dbdet_edgemap_storage();
    output_storage->set_edgemap(fgmap);
    output_data_[0].push_back(output_storage);  

    return true;
}


/*************************************************************************
* Function Name: dbbgm_fgdetect_edges_process::finish
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
dbbgm_fgdetect_edges_process::finish()
{
    return true;
}


dbdet_edgemap_sptr 
dbbgm_fgdetect_edges_process::fg_detect(dbdet_edgemap_sptr edgemap)
{
    //vil_image_resource_sptr image_sptr = frame_image->get_image();
    //vil_image_view<unsigned char> grey_img=brip_vil_float_ops::convert_to_byte(image_sptr);

    //vil_image_view<double> grad_x, grad_y;
    //vil_sobel_3x3 <unsigned char,double > (grey_img, grad_x, grad_y);

    //static float thresh;
    //parameters()->get_value( "-thresh" ,thresh);
    //sdet_nonmax_suppression_params nonmax_paprams(thresh,3);

    //static sdet_nonmax_suppression_params nsp;
    //nsp.thresh_= thresh;
    //nsp.pfit_type_ = 0;

    //sdet_nonmax_suppression ns(nsp, grad_x, grad_y);
    //ns.apply();

    //
    //vil_image_view<float> dir_img(image_sptr->ni(),image_sptr->nj(),3);
    //dir_img.fill(-100.0);
    //vil_image_view<bool> mask_img(image_sptr->ni(),image_sptr->nj(),1);
    //mask_img.fill(false);
    ////vcl_vector<dbrl_id_point_2d_sptr> pointids;

    //vcl_vector<vsol_point_2d_sptr> edges=ns.get_points();
    ////vcl_vector<double> tangents=det.get_tangents();
    //vcl_vector<vgl_vector_2d<double> > dirs=ns.get_directions();
    //for(unsigned i=0;i<edges.size();i++)
    //    {
    //    dir_img((unsigned int)vcl_floor(edges[i]->x()),(unsigned int)vcl_floor(edges[i]->y()),0)=edges[i]->x();
    //    dir_img((unsigned int)vcl_floor(edges[i]->x()),(unsigned int)vcl_floor(edges[i]->y()),1)=edges[i]->y();
    //    double dir=vcl_atan2(dirs[i].y(),dirs[i].x());
    //    dir_img((unsigned int)vcl_floor(edges[i]->x()),(unsigned int)vcl_floor(edges[i]->y()),2)=dir;
    //    mask_img((unsigned int)vcl_floor(edges[i]->x()),(unsigned int)vcl_floor(edges[i]->y()))=true;
    //    }

    //vcl_cout<<"Computed Edges for "<<frame_image->frame()<<" .. # Edges : "<<edges.size()<<"\n";

    //vil_image_view<float> directionplane=vil_plane(dir_img,2);

    //: bg modeling code

    dbbgm_image_storage_sptr frame_model;
    frame_model.vertical_cast(input_data_[0][1]);

    bool is3D=false;
    //typedef dbsta2_num_obs<dbsta2_gauss_f1> gauss_type_1d;
    //typedef dbsta2_num_obs<dbsta2_mixture<gauss_type_1d> > mix_gauss_type_1d;
vidpro_image_storage_sptr output_edgemap = vidpro_image_storage_new();
vidpro_image_storage_sptr output_fgedgemap = vidpro_image_storage_new();
    typedef dbsta2_num_obs<dbsta2_gaussian_angles_1d> gauss_type_1d;
    typedef dbsta2_num_obs<dbsta2_mixture<gauss_type_1d> > mix_gauss_type_1d;

    //typedef dbsta2_num_obs<dbsta2_gauss_ff3> gauss_type_3d;
    //typedef dbsta2_num_obs<dbsta2_mixture<gauss_type_3d> > mix_gauss_type_3d;
    
    typedef dbsta2_num_obs<dbsta2_gaussian_x_y_theta> gauss_type_3d;
    typedef dbsta2_num_obs<dbsta2_mixture<gauss_type_3d> > mix_gauss_type_3d;

    float minweightthresh = 0.2f;
    float gdist = 0.0f;
    static int rad=2;
    parameters()->get_value( "-dist" ,   gdist);
    parameters()->get_value( "-minweightthresh" ,   minweightthresh);
    parameters()->get_value( "-rad" ,   rad);

    vcl_vector<dbdet_edgel*> fgedges;
    vil_image_view<double> edgemapimage(edgemap->edge_cells.cols(),edgemap->edge_cells.rows());
    edgemapimage.fill(-10.0);
    vil_image_view<double> fgedgemapimage(edgemap->edge_cells.cols(),edgemap->edge_cells.rows());
    fgedgemapimage.fill(-10.0);

    if(dbbgm_image_of<mix_gauss_type_1d> * emodel = dynamic_cast<dbbgm_image_of<mix_gauss_type_1d>*>(frame_model->dist_image().ptr()))
    {


    //typedef dbsta2_g_mdist_detector<dbsta2_gauss_f1> detector_type_1d;
    typedef dbsta2_g_mdist_detector<dbsta2_gaussian_angles_1d> detector_type_1d;

    typedef dbsta2_top_weight_detector<mix_gauss_type_1d,detector_type_1d> weight_detector_type_1d;
    typedef dbsta2_mix_any_less_index_detector<mix_gauss_type_1d,weight_detector_type_1d> all_weight_detector_type;


    //dbsta2_g_mdist_detector<dbsta2_gauss_f1> det(gdist);
    dbsta2_g_mdist_detector<dbsta2_gaussian_angles_1d> det(gdist);
    dbsta2_top_weight_detector<mix_gauss_type_1d,detector_type_1d> detector_top(det, minweightthresh);



    unsigned rows=edgemap->edge_cells.rows();
    unsigned cols=edgemap->edge_cells.cols();
    for(unsigned i=0;i<rows;i++)
    {
        for(unsigned j=0;j<cols;j++)
        {
            vcl_vector<dbdet_edgel*> es=edgemap->edge_cells(i,j);
            for(unsigned k=0;k<es.size();k++)
            {
                vgl_point_2d<double> pt=es[k]->pt;
                double angle=es[k]->tangent;

                edgemapimage(vcl_floor(pt.x()),vcl_floor(pt.y()))=255;
                bool flag=false;
                for(int m=-rad;m<=rad;m++)
                {
                    for(int n=-rad;n<=rad;n++)
                    {
                        int m_index=vcl_floor(pt.x())+m;
                        int n_index=vcl_floor(pt.y())+n;

                        if(m_index>=0 && n_index>=0 && m_index<cols && n_index<rows)
                        {
                            mix_gauss_type_1d * dist=&(*emodel)(m_index,n_index);
                            bool isbg;

                                if(detector_top(*dist, angle,isbg))
                                {
                                    if(isbg)
                                    {
                                        flag=isbg;
                                    }
                                }
                        }
                        
                    }

                }
                if(!flag)
                {
                    fgedges.push_back(new dbdet_edgel(pt,angle));
                    fgedgemapimage(vcl_floor(pt.x()),vcl_floor(pt.y()))=255;




                }
            }
        }
    }
    vil_image_view<vxl_byte> edge_byte_image;
    vil_image_view<vxl_byte> fg_edge_byte_image;

    vil_convert_stretch_range(edgemapimage, edgemapimage,0.0, 255.999);
    vil_convert_stretch_range(fgedgemapimage, fgedgemapimage,0.0, 255.999);

    vil_convert_cast<double,vxl_byte>(edgemapimage, edge_byte_image);
    vil_convert_cast<double,vxl_byte>(fgedgemapimage, fg_edge_byte_image);


    vil_image_resource_sptr output_edge_sptr = vil_new_image_resource_of_view(edge_byte_image);
    output_edgemap->set_image(output_edge_sptr);
    vil_image_resource_sptr output_fg_edge_sptr = vil_new_image_resource_of_view(fg_edge_byte_image);
    output_fgedgemap->set_image(output_fg_edge_sptr);


    }
    else if(dbbgm_image_of<mix_gauss_type_3d > *emodel = dynamic_cast<dbbgm_image_of<mix_gauss_type_3d>*>(frame_model->dist_image().ptr()))
    {
        typedef dbsta2_g_mdist_detector<dbsta2_gaussian_x_y_theta> detector_type_3d;
        typedef dbsta2_top_weight_detector<mix_gauss_type_3d,detector_type_3d> weight_detector_type_3d;
        typedef dbsta2_mix_any_less_index_detector<mix_gauss_type_3d,weight_detector_type_3d> all_weight_detector_type_3d;
        dbsta2_g_mdist_detector<dbsta2_gaussian_x_y_theta> det_f3(gdist);
        dbsta2_top_weight_detector<mix_gauss_type_3d,detector_type_3d> detector_top_3d(det_f3, minweightthresh);

    unsigned rows=edgemap->edge_cells.rows();
    unsigned cols=edgemap->edge_cells.cols();
    for(unsigned i=0;i<rows;i++)
    {
        for(unsigned j=0;j<cols;j++)
        {
            vcl_vector<dbdet_edgel*> es=edgemap->edge_cells(i,j);
            for(unsigned k=0;k<es.size();k++)
            {
                vgl_point_2d<double> pt=es[k]->pt;
                double angle=es[k]->tangent;

                bool flag=false;
                vnl_vector_fixed<float,3> data(-10.0f);
                data[0]=pt.x();
                data[1]=pt.y();
                data[2]=angle;

                //for(int m=-rad;m<=rad;m++)
                //{
                //    for(int n=-rad;n<=rad;n++)
                //    {
                //        int m_index=vcl_floor(pt.x())+m;
                //        int n_index=vcl_floor(pt.y())+n;

                //        if(m_index>=0 && n_index>=0 && m_index<cols && n_index<rows)
                //        {
                //            mix_gauss_type_3d * dist=&(*emodel)(m_index,n_index);
                //            bool isbg;
                //            vnl_vector_fixed<float,3> data(-10.0f);
                //            data[0]=pt.x();
                //            data[1]=pt.y();
                //            data[2]=angle;

                //            if(detector_top_3d(*dist, data,isbg))
                //            {
                //                if(isbg)
                //                {
                //                    flag=isbg;
                //                }
                //            }
                //        }
                //    }
                //}
                //mix_gauss_type_3d * dist1=&(*model)(vcl_floor(pt.x()),vcl_floor(pt.y()));

                bool result=false;
                detector_top_3d((*emodel)(vcl_floor(pt.x()),vcl_floor(pt.y())),data,result);//,final_covariance);
                flag=flag || result;                
                result=false;

                // mix_gauss_type_3d * dist2=&(*model)(vcl_ceil(pt.x()),vcl_floor(pt.y()));
                detector_top_3d((*emodel)(vcl_ceil(pt.x()),vcl_floor(pt.y())),data,result);//,final_covariance);
                flag=flag || result;
                result=false;

                // mix_gauss_type_3d * dist3=&(*model)(vcl_floor(pt.x()),vcl_ceil(pt.y()));
                detector_top_3d((*emodel)(vcl_floor(pt.x()),vcl_ceil(pt.y())),data,result);//,final_covariance);
                flag=flag || result;
                result=false;

                // mix_gauss_type_3d * dist4=&(*model)(vcl_ceil(pt.x()),vcl_ceil(pt.y()));
                detector_top_3d((*emodel)(vcl_ceil(pt.x()),vcl_ceil(pt.y())),data,result);//,final_covariance);
                flag=flag || result;
                if(!flag)
                    fgedges.push_back(new dbdet_edgel(pt,angle));
            }
        }
 
    }
        vil_image_view<vxl_byte> edge_byte_image;
    vil_image_view<vxl_byte> fg_edge_byte_image;

    vil_convert_stretch_range(edgemapimage, edgemapimage,0.0, 255.999);
    vil_convert_stretch_range(fgedgemapimage, fgedgemapimage,0.0, 255.999);

    vil_convert_cast<double,vxl_byte>(edgemapimage, edge_byte_image);
    vil_convert_cast<double,vxl_byte>(fgedgemapimage, fg_edge_byte_image);


    vil_image_resource_sptr output_edge_sptr = vil_new_image_resource_of_view(edge_byte_image);
    output_edgemap->set_image(output_edge_sptr);
    vil_image_resource_sptr output_fg_edge_sptr = vil_new_image_resource_of_view(fg_edge_byte_image);
    output_fgedgemap->set_image(output_fg_edge_sptr);

    }
    else 
        return false;




    dbdet_edgemap_sptr emap=new dbdet_edgemap(edgemap->width(),edgemap->height(),fgedges);
    output_data_[0].push_back(output_edgemap);
    output_data_[0].push_back(output_fgedgemap);


    return emap;
}
