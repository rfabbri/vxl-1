#include "superimpose_frames_process.h"
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

#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <dbdet/edge/dbdet_edgemap_sptr.h>

superimpose_frames_process::superimpose_frames_process() : bpro1_process()
{
    vcl_vector<vcl_string> data_input_choices;
    data_input_choices.push_back("Edges from image");
    data_input_choices.push_back("vsol lines as input");
    data_input_choices.push_back("edge map");
    data_input_choices.push_back("image and edge map");

    
    if(  !parameters()->add( "Input Choice"   , "-input_choice" , data_input_choices,1)||
         !parameters()->add( "No of frames to superimpose " , "-numframes" , (int)2) ||
         !parameters()->add( "Initial temperature " , "-initT"             , (float)1) ||
         !parameters()->add( "Annealing Rate" , "-annealrate"              , (float) 0.93 )||
         !parameters()->add( "Final Temperature" , "-finalT"               , (float) 0.1 )||
         !parameters()->add( "Covergence Tolerance" , "-mconvg"            , (float) 0.1 )||
         !parameters()->add( "Outlier value" , "-moutlier"                 , (float) 1e-3 )||
         !parameters()->add( "lambda 1 for TPS" , "-lambda1"                 , (float) 0.1 )||
         !parameters()->add( "Radius of Neighborhood", "-radius"           , (float)5)||
         !parameters()->add( "Std Dev for proximity"                 , "-std" , (float)4)||
         !parameters()->add( "Threshold" , "-thresh"                 , (float)1.8)||
         !parameters()->add( "Euclidean or Projected " ,    "-disttype" ,     (vcl_string)"Projected"     )||
         !parameters()->add( "Crop it from Polygon " , "-ispoly" , (bool)false)||
         !parameters()->add( "    Grid #cols" , "-ncols" , 300 ) ||
         !parameters()->add( "    Grid #rows" , "-nrows" , 300 ) ||
         //grouping parameters
         !parameters()->add( "Position uncertainty" , "-dx" , 0.3 ) ||
         !parameters()->add( "Orientation uncertainty(Deg)" , "-dt" , 30.0 ) ||
         !parameters()->add( "Radius of Neighborhood" , "-nrad" , 3 )||
          !parameters()->add( "Show Grid Deformation " ,    "-isgrid" ,     (bool)false )


       )
         {
            
         }
     }


/*************************************************************************
* Function Name: superimpose_frames_process::~superimpose_frames_process
* Parameters: 
* Effects: 
*************************************************************************/

superimpose_frames_process::~superimpose_frames_process()
{
   
}


//: Clone the process
bpro1_process*
superimpose_frames_process::clone() const
{
    return new superimpose_frames_process(*this);
}


/*************************************************************************
* Function Name: superimpose_frames_process::name
* Parameters: 
* Returns: vcl_string
* Effects: 
*************************************************************************/
vcl_string
superimpose_frames_process::name()
{
    return "Superimpose Registered Frames";
}


/*************************************************************************
* Function Name: dsuperimpose_frames_process::get_input_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > superimpose_frames_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    static bool ispoly=true;
    parameters()->get_value("-ispoly",ispoly);
    unsigned data_choice=0;
    parameters()->get_value("-input_choice",data_choice);
    if(data_choice==0)
        to_return.push_back( "image" );
    else  if(data_choice==1)
        to_return.push_back( "vsol2D" );
    else  if(data_choice==2)
        to_return.push_back( "edge_map" );

    else  if(data_choice==3)
    { 
        to_return.push_back( "edge_map" );
        to_return.push_back( "image" );
    }

    if(ispoly)
        to_return.push_back( "vsol2D" );
    return to_return;
}


/*************************************************************************
* Function Name: superimpose_frames_process::get_output_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > superimpose_frames_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    static int numframes=0;
    unsigned data_choice=0;
    parameters()->get_value("-numframes",numframes);
    //for(unsigned i=0;i<numframes;i++)
    //    to_return.push_back( "sel" );
    bool isgrid=false;
    parameters()->get_value("-isgrid",isgrid);
  
    for(unsigned i=0;i<numframes-1;i++)
    {
        to_return.push_back( "dbrl_match_set" );
        if(isgrid)
            to_return.push_back( "vsol2D" );
    }
    //to_return.push_back( "vsol2D" );

  //if(isgrid)
  //  {
  //      for(unsigned i=0;i<numframes-1;i++)
  //          to_return.push_back( "vsol2D" );
  //       //to_return.push_back( "vsol2D" );
  //       // to_return.push_back( "vsol2D" );
  //  }

        to_return.push_back( "vsol2D" );

    return to_return;
}
//: Returns the number of input frames to this process
int
superimpose_frames_process::input_frames()
{
   static int numframes=0;
   parameters()->get_value("-numframes",numframes);
   return numframes;
}


//: Returns the number of output frames from this process
int
superimpose_frames_process::output_frames()
{
    return 1;
}


/*************************************************************************
* Function Name: dsuperimpose_frames_process::execute
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
superimpose_frames_process::execute()
{
    unsigned data_choice=0;
    parameters()->get_value("-input_choice",data_choice);

    vcl_vector<vcl_vector<dbrl_feature_sptr> > alledges;

    if(data_choice==0)
        alledges=get_features_from_image();
    else if(data_choice==1)
        alledges=get_features_from_vsol();
    else if(data_choice==2)
        alledges=get_features_from_edge_map();
    else if(data_choice==3)
        alledges=get_features_from_edge_map();


    vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
    vcl_string datatype=vul_sprintf("edges%d",0);
    output_vsol->add_objects(feature_to_vsol(alledges[0]),datatype);
    
    for(unsigned i=1;i<alledges.size();i++)
    {
        vcl_vector<dbrl_transformation_sptr> xforms;
        vcl_vector<dbrl_feature_sptr> xformed=registeredges(alledges[0],alledges[i],xforms);
        vcl_string datatype=vul_sprintf("edges%d",i);
        output_vsol->add_objects(feature_to_vsol(xformed),datatype);

    }
    output_data_[0].push_back(output_vsol);
    return true;
}
/*************************************************************************
* Function Name: superimpose_frames_process::finish
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
superimpose_frames_process::finish()
{
return true;
}

vcl_vector<dbrl_feature_sptr> superimpose_frames_process::registeredges(vcl_vector<dbrl_feature_sptr> tgt_orig,
                                                                        vcl_vector<dbrl_feature_sptr> src_orig,
                                                                        vcl_vector<dbrl_transformation_sptr>& xforms)
{
    
    vcl_vector<dbrl_feature_sptr> tgt=copy_features(tgt_orig);
    vcl_vector<dbrl_feature_sptr> src=copy_features(src_orig);

    double lambdainit=vcl_max(tgt.size(),src.size());

    vgl_point_2d<double> cm=center_of_mass(tgt);
    vgl_point_2d<double> cm_src=center_of_mass(src);
    normalize_cm(tgt,-cm.x(),-cm.y());
    normalize_cm(src,-cm_src.x(),-cm_src.y());
    static float Tinit=1.0;
    static float Tfinal=0.0001;
    static float annealrate=0.93;
    static float mconvg=0.1;
    static float moutlier=1e-5;
    static float lambda1=0.1;
    vcl_string disttype="Euclidean";

    parameters()->get_value("-initT",Tinit);
    parameters()->get_value("-finalT",Tfinal);
    parameters()->get_value("-annealrate",annealrate);
    parameters()->get_value("-mconvg",mconvg);
    parameters()->get_value("-moutlier",moutlier);
    parameters()->get_value("-disttype",disttype);
    parameters()->get_value("-lambda1",lambda1);

    dbrl_rpm_affine_params affineparams(lambdainit,mconvg,Tinit,Tfinal,moutlier,annealrate);

    //: applying affine 
    dbrl_rpm_affine match_affinely(affineparams,tgt,src);
    dbrl_match_set_sptr match_set=match_affinely.rpm(disttype);
    ////: binarize the match matrix
    dbrl_correspondence M=match_set->get_correspondence();
    ////M.print_summary(vcl_cout);
    //M.setinitialoutlier(affineparams.outlier());

    //dbrl_estimator_point_affine* final_affine_est= new dbrl_estimator_point_affine();
    //final_affine_est->set_lambda(0.0);
    vcl_vector<dbrl_feature_sptr> affinef1xform=tgt;
    //match_set->normalize_point_set(M.M(),affinef1xform );
    dbrl_transformation_sptr affine_tform=match_set->get_transformation();;
    dbrl_affine_transformation * final_affine_form=dynamic_cast<dbrl_affine_transformation *> (affine_tform.ptr());
    final_affine_form->set_from_features(src);
    final_affine_form->transform();
    vcl_vector<dbrl_feature_sptr> affinef2xformed=final_affine_form->get_to_features();

    dbrl_estimator_point_thin_plate_spline* tps_est= new dbrl_estimator_point_thin_plate_spline();
    tps_est->set_lambda1(lambda1);
    tps_est->set_lambda2(0.0001);

    vcl_vector<dbrl_feature_sptr> f1xform=match_set->get_feature_set1();
    match_set->normalize_point_set(M.M(),f1xform );
    dbrl_transformation_sptr tform=tps_est->estimate(f1xform,affinef2xformed,M);
    dbrl_thin_plate_spline_transformation * tpstform=dynamic_cast<dbrl_thin_plate_spline_transformation *> (tform.ptr());
    tpstform->set_from_features(affinef2xformed);
    tpstform->transform();
    vcl_vector<dbrl_feature_sptr> f2xformed=tpstform->get_to_features();




    xforms.push_back(final_affine_form);
    xforms.push_back(tpstform);

    normalize_cm(f2xformed,cm.x(),cm.y());
    dbrl_match_set_storage_sptr dms=new dbrl_match_set_storage();
    dms->set_match_set(match_set);
    output_data_[0].push_back(dms);


    //vcl_vector<vsol_spatial_object_2d_sptr> obs=feature_to_vsol(f1xform);
    //vidpro1_vsol2D_storage_sptr output_vsol1 = vidpro1_vsol2D_storage_new();
    //output_vsol1->add_objects(obs,"xformed f1");

    //output_data_[0].push_back(output_vsol1);

        //: grid transformation

    //: determin xmin,xmax,ymin,ymax;
    bool isgrid=false;
    parameters()->get_value("-isgrid",isgrid);
    if(isgrid)
    {
        int xmin=0,ymin=0,xmax=0,ymax=0;
        get_box(src,xmin,ymin,xmax,ymax);

        vcl_vector<dbrl_feature_sptr> src_gridpoints=get_grid_points(xmin,xmax,ymin,ymax);
        //normalize_cm(curr_gridpoints,-cm_curr.x(),-cm_curr.y());

        final_affine_form->set_from_features(src_gridpoints);
        final_affine_form->transform();
        vcl_vector<dbrl_feature_sptr> affine_xformed_src_gridpoints=final_affine_form->get_to_features();

        tpstform->set_from_features(affine_xformed_src_gridpoints);
        tpstform->transform();
        vcl_vector<dbrl_feature_sptr> final_xformed_src_gridpoints=tpstform->get_to_features();

        normalize_cm(final_xformed_src_gridpoints,cm.x(),cm.y());
        normalize_cm(src_gridpoints,cm_src.x(),cm_src.y());
        normalize_cm(affine_xformed_src_gridpoints,cm.x(),cm.y());

        vidpro1_vsol2D_storage_sptr grid_storage=new vidpro1_vsol2D_storage();
        vidpro1_vsol2D_storage_sptr affine_grid_storage=new vidpro1_vsol2D_storage();
        vidpro1_vsol2D_storage_sptr tps_affine_grid_storage=new vidpro1_vsol2D_storage();
        grid_storage->add_objects(make_grid_from_points(src_gridpoints,xmin,xmax,ymin,ymax),"orig");
        affine_grid_storage->add_objects(make_grid_from_points(affine_xformed_src_gridpoints,xmin,xmax,ymin,ymax),"affine");
        tps_affine_grid_storage->add_objects(make_grid_from_points(final_xformed_src_gridpoints,xmin,xmax,ymin,ymax),"tps");
        //output_data_[0].push_back(grid_storage);
        //output_data_[0].push_back(affine_grid_storage);
        output_data_[0].push_back(tps_affine_grid_storage);


    }
    return f2xformed;

}

vcl_vector<vsol_spatial_object_2d_sptr> superimpose_frames_process::feature_to_vsol(vcl_vector<dbrl_feature_sptr> & f)
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


vcl_vector<vcl_vector<dbrl_feature_sptr> > superimpose_frames_process::get_features_from_image()
{
    vcl_vector<vcl_vector<dbrl_feature_sptr> > alledges;
    //static int numframes=0;
    //parameters()->get_value("-numframes",numframes);
    //static bool ispoly=true;
    //parameters()->get_value("-ispoly",ispoly);
    //if ( input_data_.size() != numframes ){
    //    vcl_cout << "In superimpose_frames_process::execute() - not exactly one"
    //        << " input image \n";
    //    return alledges;
    //}
    //clear_output();
    //unsigned  parabola_fit;
    //float thresh;


    //parameters()->get_value( "-thresh", thresh);
    //parameters()->get_value( "-parabola_fit", parabola_fit );

    //static int nrad;
    //static double dx;
    //static double dt;

    //parameters()->get_value( "-nrad", nrad);
    //parameters()->get_value( "-dx", dx);
    //parameters()->get_value( "-dt", dt);

    //// get image from the storage class
    //for(int i=0;i<numframes;i++)
    //{
    //    vidpro1_image_storage_sptr frame_image;
    //    frame_image.vertical_cast(input_data_[i][0]);

    //    vidpro1_vsol2D_storage_sptr frame_poly;
    //    if(ispoly)
    //        frame_poly.vertical_cast(input_data_[i][1]);

    //    vil_image_resource_sptr image_sptr = frame_image->get_image();
    //    //convert to grayscale
    //    vil_image_view<vxl_byte> image_view = image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );
    //    vil_image_view<vxl_byte> greyscale_view;


    //    if(image_view.nplanes() == 3){
    //        vil_convert_planes_to_grey(image_view, greyscale_view );
    //    }
    //    else if (image_view.nplanes() == 1){
    //        greyscale_view = image_view;
    //    }
    //    int ncols=greyscale_view.nj();
    //    int nrows=greyscale_view.ni();
    //    vil_image_view<double> grad_x, grad_y;
    //    vil_sobel_3x3 <vxl_byte, double> (greyscale_view, grad_x, grad_y);
    //    static sdet_nonmax_suppression_params nsp;
    //    nsp.thresh_= thresh;
    //    nsp.pfit_type_ = parabola_fit;
    //    sdet_nonmax_suppression ns(nsp, grad_x, grad_y);
    //    ns.apply();
    //    vcl_vector< vsol_spatial_object_2d_sptr > detections;
    //    vgl_polygon<double> * contour;

    //    vcl_vector<vsol_line_2d_sptr> lines=ns.get_lines();
    //    vcl_vector<dbdet_edgel* > all_edgels;
    //    if(ispoly)
    //    {
    //        detections=frame_poly->all_data();
    //        vsol_polygon_2d_sptr poly = detections[0]->cast_to_region()->cast_to_polygon();
    //        vcl_vector<vgl_point_2d<double> > pts;
    //        for (unsigned k=0; k<poly->size(); k++)
    //        {
    //            pts.push_back(vgl_point_2d<double>(poly->vertex(k)->x(),poly->vertex(k)->y()));
    //        }
    //        contour= new  vgl_polygon<double>(pts);
    //    }

    //    for (unsigned k=0; k<lines.size(); k++)
    //    {
    //        vsol_line_2d_sptr l=lines[k]->cast_to_curve()->cast_to_line();
    //        vgl_point_2d<double> start(l->p0()->x(),l->p0()->y());
    //        vgl_point_2d<double> end(l->p1()->x(),l->p1()->y());
    //        //vgl_point_2d<double> pt(l->middle()->x(), l->middle()->y());
    //        vgl_point_2d<double> mid(l->middle()->x(),l->middle()->y());
    //        double tan = dbdet_vPointPoint(start, end);
    //        if(ispoly)
    //        {
    //            if(contour->contains(mid)){
    //                all_edgels.push_back(new dbdet_edgel(mid, tan));
    //            }
    //        }
    //        else
    //        {
    //            all_edgels.push_back(new dbdet_edgel(mid, tan));
    //        }
    //    }

    //    dbdet_edgemap_sptr edgemap=new dbdet_edgemap(nrows, ncols,all_edgels);

    //    typedef dbdet_sel<dbdet_ES_curve_model> dbdet_sel_ES;
    //    dbdet_sel_sptr edge_linker=new dbdet_sel_ES(edgemap, nrad, dt*vnl_math::pi/180, dx);

    //    //perform local edgel grouping
    //    unsigned max_size_to_group=7;

    //    edge_linker->build_curvelets_greedy(max_size_to_group);
    //    vcl_vector<dbdet_edgel*> edgels=edge_linker->get_edgels();

    //    dbdet_sel_storage_sptr output_sel = dbdet_sel_storage_new();

    //    output_sel->set_sel(edge_linker);
    //    output_data_[0].push_back(output_sel);

    //vcl_vector<dbrl_feature_sptr> edges;
    //for(unsigned i=0;i<edgels.size();i++)
    //{
    //    dbrl_feature_point_tangent_curvature_groupings * pt;
    //    vcl_vector<unsigned> neighbors_id_vec;
    //    for(unsigned j=0;j<edgels[i]->curvelets.size();j++)
    //    {
    //        for(curvelet_list_iter iter=edgels[i]->curvelets.begin();
    //            iter!=edgels[i]->curvelets.end();iter++)
    //        {
    //            if((*iter)->edgel_chain.size()>2)
    //            {
    //                for (unsigned k=0; k<(*iter)->edgel_chain.size(); k++)
    //                {
    //                    if((*iter)->edgel_chain[k]->id!=edgels[i]->id)
    //                        neighbors_id_vec.push_back((*iter)->edgel_chain[k]->id-1);
    //                }
    //            }
    //        }

    //    }
    //    pt= new dbrl_feature_point_tangent_curvature_groupings(edgels[i]->pt.x(),edgels[i]->pt.y(),edgels[i]->tangent,neighbors_id_vec);
    //    pt->id= edgels[i]->id-1;
    //    edges.push_back(pt);
    //}
    //    //    if(edgels[i]->num_of_curvelets(4)==1)
    //    //    {
    //    //        for (unsigned o=2; o<edgels[i]->local_curvelets.size(); o++)
    //    //        {
    //    //            if(edgels[i]->local_curvelets[o].size()>0)
    //    //            {
    //    //                if(dbdet_ES_curve_model * esmodel
    //    //                    =dynamic_cast<dbdet_ES_curve_model *>((edgels[i]->local_curvelets[o]).front()->curve_model))
    //    //                {
    //    //                    pt= new dbrl_feature_point_tangent_curvature_groupings(edgels[i]->pt.x(),edgels[i]->pt.y(),edgels[i]->tangent,neighbors_id_vec);
    //    //                    pt->set_k(esmodel->k);
    //    //                    pt->compute_arc(0.75);
    //    //                }
    //    //            }
    //    //        }
    //    //    }
    //    //    else
    //    //    {
    //    //pt= new dbrl_feature_point_tangent_curvature_groupings(edgels[i]->pt.x(),edgels[i]->pt.y(),edgels[i]->tangent,neighbors_id_vec);
    //    //pt->id= edgels[i]->id;
    //    //    }
    //    //features.push_back(pt);

    ////}
    //alledges.push_back(edges);


    //}
    return alledges;
}



vcl_vector<vcl_vector<dbrl_feature_sptr> > superimpose_frames_process::get_features_from_vsol()
{
  
    vcl_vector<vcl_vector<dbrl_feature_sptr> > alledges;
    static int numframes=0;
    parameters()->get_value("-numframes",numframes);
    if ( input_data_.size() != numframes ){
        vcl_cout << "In superimpose_frames_process::execute() - not exactly one"
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



vcl_vector<vcl_vector<dbrl_feature_sptr> > superimpose_frames_process::get_features_from_edge_map()
{
  
   // vcl_vector<vcl_vector<dbrl_feature_sptr> > alledges;
   // static int numframes=0;
   // parameters()->get_value("-numframes",numframes);
   // if ( input_data_.size() != numframes ){
   //     vcl_cout << "In superimpose_frames_process::execute() - not exactly one"
   //         << " input image \n";
   //     return alledges;
   // }
   // clear_output();
   // static int nrad;
   // static double dx;
   // static double dt;
   // static int ncols;
   // static int nrows;

   // parameters()->get_value( "-nrows", nrows);
   // parameters()->get_value( "-ncols", ncols);

   // parameters()->get_value( "-nrad", nrad);
   // parameters()->get_value( "-dx", dx);
   // parameters()->get_value( "-dt", dt);
   // // get image from the storage class
   // for(int i=0;i<numframes;i++)
   //{ 
   //     vcl_vector<dbrl_feature_sptr>   edges;       
   //     dbdet_edgemap_storage_sptr input_edgemap;
   //     input_edgemap.vertical_cast(input_data_[i][0]);



   // typedef dbdet_sel<dbdet_ES_curve_model> dbdet_sel_ES;
   // dbdet_sel_sptr edge_linker=new dbdet_sel_ES(input_edgemap->get_edgemap(), nrad, dt*vnl_math::pi/180, dx);

   // //perform local edgel grouping
   // unsigned max_size_to_group=7;

   // edge_linker->build_curvelets_greedy(max_size_to_group);
   // vcl_vector<dbdet_edgel*> edgels=edge_linker->get_edgels();

   // vcl_vector<dbrl_feature_sptr> features;

   // dbdet_sel_storage_sptr output_sel = dbdet_sel_storage_new();

   // output_sel->set_sel(edge_linker);
   // output_data_[0].push_back(output_sel);

   // for(unsigned i=0;i<edgels.size();i++)
   // {
   //     dbrl_feature_point_tangent_curvature_groupings * pt;
   //     vcl_vector<unsigned> neighbors_id_vec;
   //     for(unsigned j=0;j<edgels[i]->curvelets.size();j++)
   //     {
   //         for(curvelet_list_iter iter=edgels[i]->curvelets.begin();
   //             iter!=edgels[i]->curvelets.end();iter++)
   //         {
   //             for (unsigned k=0; k<(*iter)->edgel_chain.size(); k++)
   //             {
   //                 if((*iter)->edgel_chain[k]->id!=edgels[i]->id)
   //                     neighbors_id_vec.push_back((*iter)->edgel_chain[k]->id-1);
   //             }
   //         }

   //     }
   //     pt= new dbrl_feature_point_tangent_curvature_groupings(edgels[i]->pt.x(),edgels[i]->pt.y(),edgels[i]->tangent,neighbors_id_vec);
   //     pt->id= edgels[i]->id-1;
   //     features.push_back(pt);

   // }
   // alledges.push_back(features);
   // }
   // return alledges;

     int numframes=0;
     parameters()->get_value("-numframes",numframes);

vcl_vector<vcl_vector<dbrl_feature_sptr> > alledges;
for(int i=0;i<numframes;i++)
   { 
       dbdet_edgemap_storage_sptr input_edgemap;
       input_edgemap.vertical_cast(input_data_[i][0]);

       dbdet_edgemap_sptr edgemap=input_edgemap->get_edgemap();
       dbdet_edgemap_iter iter=edgemap->edge_cells.begin();
       vcl_vector<dbrl_feature_sptr> features;
       for(;iter!=edgemap->edge_cells.end();iter++)
       {
           for(unsigned i=0;i<iter->size();i++)
           {
               dbrl_feature_point_tangent * pt=new dbrl_feature_point_tangent((*iter)[i]->pt.x(),
                   (*iter)[i]->pt.y(),
                   (*iter)[i]->tangent);
               features.push_back(pt);
           }
       }
       alledges.push_back(features);
}

return alledges;

}



vgl_point_2d<double> superimpose_frames_process::center_of_mass(vcl_vector<dbrl_feature_sptr> & f)
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


void superimpose_frames_process::normalize_cm(vcl_vector<dbrl_feature_sptr> & f,double xref,double yref)
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


vcl_vector<dbrl_feature_sptr> superimpose_frames_process::copy_features(vcl_vector<dbrl_feature_sptr> f)
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




vcl_vector<vsol_spatial_object_2d_sptr> superimpose_frames_process::make_grid_from_points(vcl_vector<dbrl_feature_sptr> features,int xmin,int xmax,int ymin,int ymax)
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

vcl_vector<dbrl_feature_sptr> superimpose_frames_process::get_grid_points(int xmin,int xmax,int ymin,int ymax)
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

void  superimpose_frames_process::get_box(vcl_vector<dbrl_feature_sptr> f,int & xmin,int &ymin,int &xmax,int &ymax)
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
