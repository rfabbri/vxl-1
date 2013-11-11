#include "dbrl_compute_motion_field_process.h"
#include <bseg/brip/brip_vil_float_ops.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <georegister/dbrl_rpm_affine.h>
#include <georegister/dbrl_estimator_thin_plate_spline.h>
#include <georegister/dbrl_estimator_cubic_patch.h>

#include <georegister/dbrl_match_set_sptr.h>
#include <georegister/dbrl_feature_point_sptr.h>
#include <georegister/dbrl_feature_point.h>
#include <georegister/dbrl_feature_point_tangent.h>
#include <georegister/dbrl_feature_point_tangent_curvature_groupings.h>
#include <georegister/dbrl_robust_point_matching.h>
#include <georegister/dbrl_thin_plate_spline_transformation.h>
#include <georegister/dbrl_cubic_transformation.h>
#include <georegister/dbrl_affine_transformation.h>
#include <dbru/dbru_multiple_instance_object.h>
#include <pro/dbrl_id_point_2d_storage_sptr.h>
#include <pro/dbrl_id_point_2d_storage.h>
#include <pro/dbrl_match_set_storage.h>
#include <pro/dbrl_match_set_storage_sptr.h>
#include "dbrl_id_point_2d_storage_sptr.h"
#include "dbrl_id_point_2d_storage.h"
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vnl/vnl_math.h>
#include <vul/vul_sprintf.h>
#include <vcl_cstdio.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>

#include <vsol/vsol_polyline_2d.h>

#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <sdet/sdet_nonmax_suppression_params.h>
#include <sdet/sdet_nonmax_suppression.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vgl/vgl_polygon.h>
#include <dbdet/algo/dbdet_sel_base.h>
#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/sel/dbdet_edgel.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix.h>

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

dbrl_compute_motion_field_process::dbrl_compute_motion_field_process() : bpro1_process()
{

    
       if(  !parameters()->add( "Initial Search Radius "   , "-initradius" ,(float) 5.0)
           )
       {
        vcl_cout<<"\n Error in parameters ";
       }

     }


/*************************************************************************
* Function Name: dbrl_compute_motion_field_process::~dbrl_compute_motion_field_process
* Parameters: 
* Effects: 
*************************************************************************/

dbrl_compute_motion_field_process::~dbrl_compute_motion_field_process()
{
   
}


//: Clone the process
bpro1_process*
dbrl_compute_motion_field_process::clone() const
{
    return new dbrl_compute_motion_field_process(*this);
}


/*************************************************************************
* Function Name: dbrl_compute_motion_field_process::name
* Parameters: 
* Returns: vcl_string
* Effects: 
*************************************************************************/
vcl_string
dbrl_compute_motion_field_process::name()
{
    return "Compute Motion Field";
}


/*************************************************************************
* Function Name: dbrl_compute_motion_field_process::get_input_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > dbrl_compute_motion_field_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "edge_map" );
    return to_return;
}


/*************************************************************************
* Function Name: dbrl_compute_motion_field_process::get_output_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > dbrl_compute_motion_field_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "dbrl_match_set" );
    return to_return;
}
//: Returns the number of input frames to this process
int
dbrl_compute_motion_field_process::input_frames()
{
   return 2;
}


//: Returns the number of output frames from this process
int
dbrl_compute_motion_field_process::output_frames()
{
    return 1;
}


/*************************************************************************
* Function Name: ddbrl_compute_motion_field_process::execute
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
dbrl_compute_motion_field_process::execute()
{
    //: get the input edge map from current frame
    dbdet_edgemap_storage_sptr curr_edgemap;
    curr_edgemap.vertical_cast(input_data_[0][0]);
    //: get the input edge map from previous frame
    dbdet_edgemap_storage_sptr prev_edgemap;
    prev_edgemap.vertical_cast(input_data_[1][0]);

    //:convert edgemap to dbrl_feature
    vcl_vector<dbrl_feature_sptr> curr_features=get_features_from_edge_map(curr_edgemap->get_edgemap());
    vcl_vector<dbrl_feature_sptr> prev_features=get_features_from_edge_map(prev_edgemap->get_edgemap());
    
    dbrl_match_set_sptr matchset=compute_motion_field(curr_features,prev_features);
       dbrl_match_set_storage_sptr dms=new dbrl_match_set_storage();
    dms->set_match_set(matchset);
    output_data_[0].push_back(dms);


    
    return true;
}
/*************************************************************************
* Function Name: dbrl_compute_motion_field_process::finish
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
dbrl_compute_motion_field_process::finish()
{
    return true;
}

dbrl_match_set_sptr
dbrl_compute_motion_field_process::compute_motion_field(vcl_vector<dbrl_feature_sptr> tgt_orig,
                                                        vcl_vector<dbrl_feature_sptr> src_orig)
{
    
    vcl_vector<dbrl_feature_sptr> tgt=copy_features(tgt_orig);
    vcl_vector<dbrl_feature_sptr> src=copy_features(src_orig);
    float initr=0;
    parameters()->get_value("-initradius",initr);
    dbrl_rpm_affine_params affine_params;
    dbrl_rpm_affine affine_rpm(affine_params,src,tgt);
    vnl_matrix<double> assignment=affine_rpm.compute_correspondence_point_tangent_weights(1,src,tgt,initr);

    dbrl_correspondence M(assignment);
    M.normalize();
    M.setinitialoutlier(0.00001);
    dbrl_match_set_sptr matchset=new dbrl_match_set();
    matchset->set_original_features(src,tgt);
    matchset->set_correspondence_matrix(M);


    return matchset;
}

vcl_vector<vsol_spatial_object_2d_sptr> dbrl_compute_motion_field_process::feature_to_vsol(vcl_vector<dbrl_feature_sptr> & f)
     {
     vcl_vector<vsol_spatial_object_2d_sptr> vpts;
     for(unsigned i=0;i<f.size();i++)
     {
         if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f[i].ptr()))
             {
             vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0],pt->location()[1]);
             vpts.push_back(p->cast_to_spatial_object());
             }
          if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f[i].ptr()))
             {
                 vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0],pt->location()[1]);
                 vsol_point_2d_sptr p0=new vsol_point_2d(p->x()+0.2*vcl_cos(pt->dir()),p->y()+0.2*vcl_sin(pt->dir()));
                 vsol_point_2d_sptr p1=new vsol_point_2d(p->x()-0.2*vcl_cos(pt->dir()),p->y()-0.2*vcl_sin(pt->dir()));

                 vsol_line_2d_sptr l=new vsol_line_2d(p0,p1);
                 vpts.push_back(p->cast_to_spatial_object());
                 vpts.push_back(l->cast_to_spatial_object());
             }
           if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f[i].ptr()))
             {
                 vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0],pt->location()[1]);
                 vsol_point_2d_sptr p0=new vsol_point_2d(p->x()+0.2*vcl_cos(pt->dir()),p->y()+0.2*vcl_sin(pt->dir()));
                 vsol_point_2d_sptr p1=new vsol_point_2d(p->x()-0.2*vcl_cos(pt->dir()),p->y()-0.2*vcl_sin(pt->dir()));

                 vsol_line_2d_sptr l=new vsol_line_2d(p0,p1);
                 vpts.push_back(p->cast_to_spatial_object());
                 vpts.push_back(l->cast_to_spatial_object());
             }
     }
         return vpts;
     }


vcl_vector<vcl_vector<dbrl_feature_sptr> > dbrl_compute_motion_field_process::get_features_from_image()
{
    vcl_vector<vcl_vector<dbrl_feature_sptr> > alledges;
    static int numframes=0;
    parameters()->get_value("-numframes",numframes);
    static bool ispoly=true;
    parameters()->get_value("-ispoly",ispoly);
    if ( input_data_.size() != numframes ){
        vcl_cout << "In dbrl_compute_motion_field_process::execute() - not exactly one"
            << " input image \n";
        return alledges;
    }
    clear_output();
    unsigned  parabola_fit;
    float thresh;


    parameters()->get_value( "-thresh", thresh);
    parameters()->get_value( "-parabola_fit", parabola_fit );

    static int nrad;
    static double dx;
    static double dt;

    parameters()->get_value( "-nrad", nrad);
    parameters()->get_value( "-dx", dx);
    parameters()->get_value( "-dt", dt);

    // get image from the storage class
    for(int i=0;i<numframes;i++)
    {
        vidpro1_image_storage_sptr frame_image;
        frame_image.vertical_cast(input_data_[i][0]);

        vidpro1_vsol2D_storage_sptr frame_poly;
        if(ispoly)
            frame_poly.vertical_cast(input_data_[i][1]);

        vil_image_resource_sptr image_sptr = frame_image->get_image();
        //convert to grayscale
        vil_image_view<vxl_byte> image_view = image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );
        vil_image_view<vxl_byte> greyscale_view;


        if(image_view.nplanes() == 3){
            vil_convert_planes_to_grey(image_view, greyscale_view );
        }
        else if (image_view.nplanes() == 1){
            greyscale_view = image_view;
        }
        int ncols=greyscale_view.nj();
        int nrows=greyscale_view.ni();
        vil_image_view<double> grad_x, grad_y;
        vil_sobel_3x3 <vxl_byte, double> (greyscale_view, grad_x, grad_y);
        static sdet_nonmax_suppression_params nsp;
        nsp.thresh_= thresh;
        nsp.pfit_type_ = parabola_fit;
        sdet_nonmax_suppression ns(nsp, grad_x, grad_y);
        ns.apply();
        vcl_vector< vsol_spatial_object_2d_sptr > detections;
        vgl_polygon<double> * contour;

        vcl_vector<vsol_line_2d_sptr> lines=ns.get_lines();
        vcl_vector<dbdet_edgel* > all_edgels;
        if(ispoly)
        {
            detections=frame_poly->all_data();
            vsol_polygon_2d_sptr poly = detections[0]->cast_to_region()->cast_to_polygon();
            vcl_vector<vgl_point_2d<double> > pts;
            for (unsigned k=0; k<poly->size(); k++)
            {
                pts.push_back(vgl_point_2d<double>(poly->vertex(k)->x(),poly->vertex(k)->y()));
            }
            contour= new  vgl_polygon<double>(pts);
        }

        for (unsigned k=0; k<lines.size(); k++)
        {
            vsol_line_2d_sptr l=lines[k]->cast_to_curve()->cast_to_line();
            vgl_point_2d<double> start(l->p0()->x(),l->p0()->y());
            vgl_point_2d<double> end(l->p1()->x(),l->p1()->y());
            //vgl_point_2d<double> pt(l->middle()->x(), l->middle()->y());
            vgl_point_2d<double> mid(l->middle()->x(),l->middle()->y());
            double tan = dbdet_vPointPoint(start, end);
            if(ispoly)
            {
                if(contour->contains(mid)){
                    all_edgels.push_back(new dbdet_edgel(mid, tan));
                }
            }
            else
            {
                all_edgels.push_back(new dbdet_edgel(mid, tan));
            }
        }

        dbdet_edgemap_sptr edgemap=new dbdet_edgemap(nrows, ncols,all_edgels);

        typedef dbdet_sel<dbdet_ES_curve_model> dbdet_sel_ES;
        dbdet_sel_sptr edge_linker=new dbdet_sel_ES(edgemap, nrad, dt*vnl_math::pi/180, dx);

        //perform local edgel grouping
        unsigned max_size_to_group=7;

        edge_linker->build_curvelets_greedy(max_size_to_group);
        vcl_vector<dbdet_edgel*> edgels=edge_linker->get_edgels();

        dbdet_sel_storage_sptr output_sel = dbdet_sel_storage_new();

        output_sel->set_sel(edge_linker);
        output_data_[0].push_back(output_sel);

    vcl_vector<dbrl_feature_sptr> edges;
    for(unsigned i=0;i<edgels.size();i++)
    {
        dbrl_feature_point_tangent_curvature_groupings * pt;
        vcl_vector<unsigned> neighbors_id_vec;
        for(unsigned j=0;j<edgels[i]->curvelets.size();j++)
        {
            for(curvelet_list_iter iter=edgels[i]->curvelets.begin();
                iter!=edgels[i]->curvelets.end();iter++)
            {
                if((*iter)->edgel_chain.size()>2)
                {
                    for (unsigned k=0; k<(*iter)->edgel_chain.size(); k++)
                    {
                        if((*iter)->edgel_chain[k]->id!=edgels[i]->id)
                            neighbors_id_vec.push_back((*iter)->edgel_chain[k]->id-1);
                    }
                }
            }

        }
        pt= new dbrl_feature_point_tangent_curvature_groupings(edgels[i]->pt.x(),edgels[i]->pt.y(),edgels[i]->tangent,neighbors_id_vec);
        pt->id= edgels[i]->id-1;
        edges.push_back(pt);
    }
        //    if(edgels[i]->num_of_curvelets(4)==1)
        //    {
        //        for (unsigned o=2; o<edgels[i]->local_curvelets.size(); o++)
        //        {
        //            if(edgels[i]->local_curvelets[o].size()>0)
        //            {
        //                if(dbdet_ES_curve_model * esmodel
        //                    =dynamic_cast<dbdet_ES_curve_model *>((edgels[i]->local_curvelets[o]).front()->curve_model))
        //                {
        //                    pt= new dbrl_feature_point_tangent_curvature_groupings(edgels[i]->pt.x(),edgels[i]->pt.y(),edgels[i]->tangent,neighbors_id_vec);
        //                    pt->set_k(esmodel->k);
        //                    pt->compute_arc(0.75);
        //                }
        //            }
        //        }
        //    }
        //    else
        //    {
        //pt= new dbrl_feature_point_tangent_curvature_groupings(edgels[i]->pt.x(),edgels[i]->pt.y(),edgels[i]->tangent,neighbors_id_vec);
        //pt->id= edgels[i]->id;
        //    }
        //features.push_back(pt);

    //}
    alledges.push_back(edges);


    }
    return alledges;
}



vcl_vector<vcl_vector<dbrl_feature_sptr> > dbrl_compute_motion_field_process::get_features_from_vsol()
{
  
    vcl_vector<vcl_vector<dbrl_feature_sptr> > alledges;
    static int numframes=0;
    parameters()->get_value("-numframes",numframes);
    if ( input_data_.size() != numframes ){
        vcl_cout << "In dbrl_compute_motion_field_process::execute() - not exactly one"
            << " input image \n";
        return alledges;
    }
    clear_output();
    static int nrad;
    static double dx;
    static double dt;
    static int ncols;
    static int nrows;

    parameters()->get_value( "-nrows", nrows);
    parameters()->get_value( "-ncols", ncols);

    parameters()->get_value( "-nrad", nrad);
    parameters()->get_value( "-dx", dx);
    parameters()->get_value( "-dt", dt);
    // get image from the storage class
    for(int i=0;i<numframes;i++)
   { 
        vcl_vector<dbrl_feature_sptr>   edges;       
        vidpro1_vsol2D_storage_sptr frame_edges;
        frame_edges.vertical_cast(input_data_[i][0]);
        vcl_vector< vsol_spatial_object_2d_sptr > lines=frame_edges->all_data();

    vcl_vector<dbdet_edgel* > all_edgels;
    vcl_vector<dbrl_feature_sptr> features;
    for (unsigned k=0; k<lines.size(); k++)
    {
        if(lines[k]->cast_to_curve())
        {
            if(vsol_line_2d_sptr l=lines[k]->cast_to_curve()->cast_to_line())
            {
                vgl_point_2d<double> start(l->p0()->x(),l->p0()->y());
                vgl_point_2d<double> end(l->p1()->x(),l->p1()->y());
                vgl_point_2d<double> pt(l->middle()->x(), l->middle()->y());
                double tan = dbdet_vPointPoint(start, end);
                all_edgels.push_back(new dbdet_edgel(pt, tan));
                dbrl_feature_point_tangent * fpt=new dbrl_feature_point_tangent(pt.x(),pt.y(),tan);
                features.push_back(fpt);
                //all_edgels[k]->id=k;
            }
        }
    }

    //dbdet_edgemap_sptr edgemap=new dbdet_edgemap(nrows, ncols,all_edgels);


    //typedef dbdet_sel<dbdet_ES_curve_model> dbdet_sel_ES;
    //dbdet_sel_sptr edge_linker=new dbdet_sel_ES(edgemap, nrad, dt*vnl_math::pi/180, dx);

    ////perform local edgel grouping
    //unsigned max_size_to_group=7;

    //edge_linker->build_curvelets_greedy(max_size_to_group);
    //vcl_vector<dbdet_edgel*> edgels=edge_linker->get_edgels();

    //vcl_vector<dbrl_feature_sptr> features;

    //dbdet_sel_storage_sptr output_sel = dbdet_sel_storage_new();

    //output_sel->set_sel(edge_linker);
    //output_data_[0].push_back(output_sel);

    //for(unsigned i=0;i<edgels.size();i++)
    //{
    //    dbrl_feature_point_tangent_curvature_groupings * pt;
    //    vcl_vector<unsigned> neighbors_id_vec;
    //    for(unsigned j=0;j<edgels[i]->curvelets.size();j++)
    //    {
    //        for(curvelet_list_iter iter=edgels[i]->curvelets.begin();
    //            iter!=edgels[i]->curvelets.end();iter++)
    //        {
    //            for (unsigned k=0; k<(*iter)->edgel_chain.size(); k++)
    //            {
    //                if((*iter)->edgel_chain[k]->id!=edgels[i]->id)
    //                    neighbors_id_vec.push_back((*iter)->edgel_chain[k]->id-1);
    //            }
    //        }

    //    }
    //    pt= new dbrl_feature_point_tangent_curvature_groupings(edgels[i]->pt.x(),edgels[i]->pt.y(),edgels[i]->tangent,neighbors_id_vec);
    //    pt->id= edgels[i]->id-1;
    //    features.push_back(pt);

    //}
    alledges.push_back(features);
    }
    return alledges;
}



vcl_vector<dbrl_feature_sptr> dbrl_compute_motion_field_process::get_features_from_edge_map(dbdet_edgemap_sptr edgemap)
{
    vcl_vector<dbrl_feature_sptr>  edges;
    

    dbdet_edgemap_iter iter=edgemap->edge_cells.begin();
    vcl_vector<dbrl_feature_sptr> features;
    
    for(;iter!=edgemap->edge_cells.end();iter++)
    {
        for(unsigned i=0;i<iter->size();i++)
        {
            dbrl_feature_point_tangent * pt=new dbrl_feature_point_tangent((*iter)[i]->pt.x(),
                                                                           (*iter)[i]->pt.y(),
                                                                           (*iter)[i]->tangent);
            edges.push_back(pt);
        }
    }
    
    return edges;
}



vgl_point_2d<double> dbrl_compute_motion_field_process::center_of_mass(vcl_vector<dbrl_feature_sptr> & f)
{
    double cx=0.0;
    double cy=0.0;
    for(unsigned i=0;i<f.size();i++)
    {
        if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f[i].ptr()))
        {
            cx+=pt->location()[0];
            cy+=pt->location()[1];
        }
        if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f[i].ptr()))
        {
            cx+=pt->location()[0];
            cy+=pt->location()[1];
        }
        if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f[i].ptr()))
        {
            cx+=pt->location()[0];
            cy+=pt->location()[1];
        }
        else if(dbrl_feature_point_tangent_curvature_groupings* pt=dynamic_cast<dbrl_feature_point_tangent_curvature_groupings*>(f[i].ptr()))
        {
            cx+=pt->location()[0];
            cy+=pt->location()[1];
        }
    }
        vgl_point_2d<double> p(cx/f.size(),cy/f.size());

        return p;
}


void dbrl_compute_motion_field_process::normalize_cm(vcl_vector<dbrl_feature_sptr> & f,double xref,double yref)
{
    for(unsigned i=0;i<f.size();i++)
    {
        if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f[i].ptr()))
            pt->set_location(vnl_vector_fixed<double,2>(pt->location()[0]+xref,pt->location()[1]+yref));
        if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f[i].ptr()))
            pt->set_location(vnl_vector_fixed<double,2>(pt->location()[0]+xref,pt->location()[1]+yref));
        if(dbrl_feature_point_tangent_curvature_groupings* pt=dynamic_cast<dbrl_feature_point_tangent_curvature_groupings*>(f[i].ptr()))
            pt->set_location(vnl_vector_fixed<double,2>(pt->location()[0]+xref,pt->location()[1]+yref));
        else if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f[i].ptr()))
            pt->set_location(vnl_vector_fixed<double,2>(pt->location()[0]+xref,pt->location()[1]+yref));
    }
}


vcl_vector<dbrl_feature_sptr> dbrl_compute_motion_field_process::copy_features(vcl_vector<dbrl_feature_sptr> f)
{
    vcl_vector<dbrl_feature_sptr> fout;
    for(unsigned i=0;i<f.size();i++)
    {
        if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f[i].ptr()))
            fout.push_back(new dbrl_feature_point(pt->location()));
        if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f[i].ptr()))
            fout.push_back(new dbrl_feature_point_tangent(pt->location(),pt->dir()));
        if(dbrl_feature_point_tangent_curvature_groupings* pt=dynamic_cast<dbrl_feature_point_tangent_curvature_groupings*>(f[i].ptr()))
            fout.push_back(new dbrl_feature_point_tangent_curvature_groupings(pt->location(),pt->dir(),pt->feature_neighbor_map_));
        else if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f[i].ptr()))
        {
            
            dbrl_feature_point_tangent_curvature *ptk=new dbrl_feature_point_tangent_curvature(pt->location(),pt->dir());
            if(pt->k_flag())
            {
                ptk->set_k(pt->k());
                ptk->set_k_flag();
            }
            fout.push_back(ptk);
        }
    }
    return fout;
}




vcl_vector<vsol_spatial_object_2d_sptr> dbrl_compute_motion_field_process::make_grid_from_points(vcl_vector<dbrl_feature_sptr> features,int xmin,int xmax,int ymin,int ymax)
{
    int cnt=0;
    vcl_vector<vsol_spatial_object_2d_sptr> toreturn;
    for(int i=xmin;i<xmax;i++)
    {
        vcl_vector<vsol_point_2d_sptr> pts;
        for(int j=ymin;j<ymax;j++)
        {
            if(dbrl_feature_point *pt=dynamic_cast<dbrl_feature_point*> (features[cnt].ptr()))
            {
                vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0],pt->location()[1]);
                pts.push_back(p);
            }
            cnt++;
        }
        for(unsigned k=0;k<pts.size()-1;k++)
        {
            vsol_line_2d_sptr line2d=new vsol_line_2d(pts[k],pts[k+1]);         
            toreturn.push_back(line2d->cast_to_spatial_object());
        }
    }
    cnt=0;
    for(int j=ymin;j<ymax;j++)
    {
        vcl_vector<vsol_point_2d_sptr> pts;
        for(int i=xmin;i<xmax;i++)
        {
            if(dbrl_feature_point *pt=dynamic_cast<dbrl_feature_point*> (features[cnt+(i-xmin)*(ymax-ymin)].ptr()))
            {
                vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0],pt->location()[1]);
                pts.push_back(p);
            }
            
        }
        cnt++;
        for(unsigned k=0;k<pts.size()-1;k++)
        {
            vsol_line_2d_sptr line2d=new vsol_line_2d(pts[k],pts[k+1]);         
            toreturn.push_back(line2d->cast_to_spatial_object());
        }
    }

    return toreturn;
}

vcl_vector<dbrl_feature_sptr> dbrl_compute_motion_field_process::get_grid_points(int xmin,int xmax,int ymin,int ymax)
{
    vcl_vector<dbrl_feature_sptr> features;
    for(int i=xmin;i<xmax;i++)
    {
        for(int j=ymin;j<ymax;j++)
        {
            dbrl_feature_point *pt=new dbrl_feature_point(i,j);
            features.push_back(pt);
        }
    }
    
    return features;
}

void  dbrl_compute_motion_field_process::get_box(vcl_vector<dbrl_feature_sptr> f,int & xmin,int &ymin,int &xmax,int &ymax)
{
    vcl_vector<vgl_point_2d<double> > pts;

    vgl_box_2d<double> box;

    for(unsigned i=0;i<f.size();i++)
    {
        if(dbrl_feature_point * pt=dynamic_cast<dbrl_feature_point*>(f[i].ptr()))
            box.add(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));
        else if(dbrl_feature_point_tangent * pt=dynamic_cast<dbrl_feature_point_tangent*>(f[i].ptr()))
            box.add(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));
        else if(dbrl_feature_point_tangent_curvature * pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f[i].ptr()))
            box.add(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));
    }

    xmin=vcl_floor(box.min_x());
    ymin=vcl_floor(box.min_y());
    xmax=vcl_ceil(box.max_x());
    ymax=vcl_ceil(box.max_y());


}

//dbrl_superresolution_multiple_objects::super_resolute(vcl_map<dbrl_feature_sptr,unsigned char> fmap,
//                                                      dbinfo_observation_sptr obs,vcl_string superimgname)
//{
//    vil_image_resource_sptr img=obs->obs_snippet();
//    //vil_image_view<double> superimg(img->ni(),img->nj(),1);
//
//    int minx=obs->ex_roi()->cmin(0);
//    int maxx=obs->ex_roi()->cmax(0);
//    int miny=obs->ex_roi()->rmin(0);
//    int maxy=obs->ex_roi()->rmax(0);
//
//    vcl_map<dbrl_feature_sptr,unsigned char>::iterator iter;
//    vcl_map<dbrl_feature_sptr,unsigned char> filtered_samples;
//    for(iter=fmap.begin();iter!=fmap.end();iter++)
//    {
//        if( dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(iter->first.ptr()))
//        {
//            if(pt->location()[0]>=minx && pt->location()[0]<=maxx &&
//                pt->location()[1]>=miny && pt->location()[1]<=maxy)
//            {
//                filtered_samples[iter->first]=iter->second;
//            }
//        }
//    }
//    vnl_matrix<double> pts(filtered_samples.size(),2);
//    vnl_vector<double> zs(filtered_samples.size());
//    unsigned i=0;
//    for(iter=filtered_samples.begin();iter!=filtered_samples.end();iter++)
//    {
//        if( dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(iter->first.ptr()))
//        {
//            pts(i,0)=pt->location()[0];
//            pts(i,1)=pt->location()[1];
//            zs(i)=(double)iter->second;
//            i++;
//        }
//    }
//    dbrl_estimator_cubic_patch * cpatch=new dbrl_estimator_cubic_patch();
//    vcl_vector<dbrl_clough_tocher_patch> patches=cpatch->estimate_cubic(pts,zs);
//
//    vil_image_view<unsigned char> superimg(img->ni()*4,img->nj()*4);
//
//    for(double is=minx;is<maxx;)
//    {
//        for(double js=miny;js<maxy;)
//        {
//            vgl_point_2d<double> p(is,js);
//            for(unsigned i=0;i<patches.size();i++)
//            {   
//                if(patches[i].intriangle(p))
//                {
//                    double newz=patches[i].interpolate(p);
//                    unsigned char intensity=(unsigned char)vcl_floor(newz+0.5);
//                    superimg(vcl_floor((is-minx)*4),vcl_floor((js-miny)*4))=intensity;
//                    continue;
//                }
//            }
//            js+=0.25;
//        }
//        is+=0.25;
//    }
//    vil_save(superimg,superimgname.c_str());
//}
//
//vcl_vector<dbrl_feature_sptr> dbrl_superresolution_multiple_objects::get_grid_points(vcl_vector<dbrl_feature_sptr> f2,
//                                                                                     double spacing)
//{
//    vcl_vector<vgl_point_2d<double> > pts;
//
//    vgl_box_2d<double> box;
//
//    for(unsigned i=0;i<f2.size();i++)
//    {
//        if(dbrl_feature_point * pt=dynamic_cast<dbrl_feature_point*>(f2[i].ptr()))
//            box.add(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));
//        else if(dbrl_feature_point_tangent * pt=dynamic_cast<dbrl_feature_point_tangent*>(f2[i].ptr()))
//            box.add(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));
//                else if(dbrl_feature_point_tangent_curvature * pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f2[i].ptr()))
//            box.add(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));
//
//    }
//
//    double xmin=vcl_floor(box.min_x());
//    double ymin=vcl_floor(box.min_y());
//    double xmax=vcl_ceil(box.max_x());
//    double ymax=vcl_ceil(box.max_y());
//
//    vcl_vector<dbrl_feature_sptr> fs;
//    for(double x=xmin;x<=xmax;)
//    {   
//        for(double y=ymin;y<=ymax;)
//        {
//            fs.push_back(new dbrl_feature_point(x,y));
//            y+=spacing;
//        }
//        x+=spacing;
//    }
//    return fs;
//
//}
//
//
//
//
//
//
//
