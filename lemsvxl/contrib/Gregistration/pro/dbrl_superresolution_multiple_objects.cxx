
#include "dbrl_superresolution_multiple_objects.h"
#include<bpro1/bpro1_parameters.h>
#include<vidpro1/storage/vidpro1_image_storage_sptr.h>
#include<vidpro1/storage/vidpro1_image_storage.h>
#include<dbinfo/dbinfo_observation_sptr.h>
#include<dbinfo/dbinfo_observation.h>

#include <vnl/vnl_math.h>
#include <georegister/dbrl_rpm_affine.h>
#include <georegister/dbrl_estimator_thin_plate_spline.h>
#include <georegister/dbrl_thin_plate_spline_transformation.h>
#include <georegister/dbrl_estimator_cubic_patch.h>

#include <georegister/dbrl_match_set_sptr.h>
#include <georegister/dbrl_feature_point_sptr.h>
#include <georegister/dbrl_feature_point.h>
#include <georegister/dbrl_feature_point_tangent.h>
#include <georegister/dbrl_feature_point_tangent_curvature.h>
#include <georegister/dbrl_feature_point_tangent_curvature_groupings.h>
#include <georegister/dbrl_cubic_transformation.h>
#include <dbru/dbru_multiple_instance_object.h>
#include <dt/dbrl_clough_tocher_patch.h>
#include <dt/partial_derivatives.h>
#include <dt/Delaunay.h>

#include <pro/dbrl_id_point_2d_storage.h>
#include <pro/dbrl_id_point_2d_storage_sptr.h>
#include <osl/osl_edge.h>
#include <osl/osl_canny_ox.h>
#include <osl/osl_canny_ox_params.h>
#include <osl/osl_edgel_chain.h>
#include <bseg/brip/brip_vil_float_ops.h>
#include <vnl/vnl_vector_fixed.h>
#include <georegister/dbrl_support.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <vul/vul_sprintf.h>
#include <bsol/bsol_algs.h>
#include <vgl/vgl_polygon.h>
#include <dbbgm/dbbgm_distribution_image.h>
#include <dbbgm/dbbgm_detect.h>
#include <dbsta/dbsta_detector_mixture.h>
#include <dbsta/dbsta_detector_gaussian.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage.h>
#include <dbbgm/pro/dbbgm_distribution_image_storage_sptr.h>
#include <dbbgm/pro/dbbgm_image_storage.h>
#include <dbbgm/pro/dbbgm_image_storage_sptr.h>
#include <dbbgm/dbbgm_detect.h>
#include <dbsta2/dbsta2_gaussian_full.h>
#include <dbsta2/dbsta2_gaussian_sphere.h>
#include <dbsta2/dbsta2_detector_gaussian.h>
#include <dbsta2/dbsta2_detector_mixture.h>
#include <dbsta2/dbsta2_basic_functors.h>
#include <dbsta2/dbsta2_gauss_f1.h>
#include <dbsta2/dbsta2_attributes.h>
#include <dbsta2/dbsta2_mixture.h>
#include <dbsta2/dbsta2_detector_mixture.h>

#include <vil/vil_save.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vil/vil_new.h>
#include <vidpro1/vidpro1_repository_sptr.h>
#include <vidpro1/vidpro1_repository.h>
//#include <bvis1/bvis1_manager.h>
#include <vgl/vgl_box_2d.h>
#include <sdet/sdet_nonmax_suppression_params.h>
#include <sdet/sdet_nonmax_suppression.h>
#include <vsl/vsl_vector_io.h>

#include <dbdet/algo/dbdet_sel_base.h>
#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/algo/dbdet_sel.h>
#include <dbdet/edge/dbdet_third_order_edge_det.h>
#include <dbdet/edge/dbdet_nms.h>

#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/sel/dbdet_edgel.h>

//: Constructor
dbrl_superresolution_multiple_objects::dbrl_superresolution_multiple_objects(void): bpro1_process(),model_storage_(0)
{

    if( !parameters()->add( "Only Fg Edges "                        ,    "-onlyfgedges"     , (bool)false )||
        !parameters()->add( "No of frames on each side to register" , "-winsize"            , (int)2) ||
        !parameters()->add( "Initial temperature "                  , "-initT"              , (float)1) ||
        !parameters()->add( "Annealing Rate"                        , "-annealrate"         , (float) 0.93 )||
        !parameters()->add( "Final Temperature"                     , "-finalT"             , (float) 0.05 )||
        //!parameters()->add( "Initial Lambda"                        , "-initlambda"         , (float) 100 )||
        !parameters()->add( "Covergence Tolerance"                  , "-mconvg"             , (float) 0.1 )||
        !parameters()->add( "Outlier value"                         , "-moutlier"           , (float) 1e-2 )||
        !parameters()->add( "Radius of Dilation(canbe -ve for erosion)" , "-drad"           , (float) 3)||
        !parameters()->add( "Spacing of the grid"                   , "-gridspace"          , (float) 0.2)||
        //!parameters()->add( "Std Dev for proximity"                 , "-std"                , (float)4)||
        //!parameters()->add( "Threshold"                             , "-thresh"             , (float)1.8)||
        !parameters()->add( "Distance Theshold"                     ,  "-dist"              ,  2.5f        ) ||
        !parameters()->add( "Minimum Weight Threshold"              ,  "-minweightthresh"   ,  0.5f        )||
        !parameters()->add( "Radius of uncertainity"                ,  "-rad"               ,  (int)1        )||
        !parameters()->add( "Euclidean or Projected "               ,  "-disttype"          ,  (vcl_string)"Projected"     )||
        !parameters()->add( "Edge Background Model "                ,  "-edgemodel"         ,  bpro1_filepath("","*")      )||
        !parameters()->add( "Output polyfiles "                     ,  "-outpoly"           ,   bpro1_filepath("","*")      )||
        !parameters()->add( "Edge Detector Sigma"                ,  "-esigma"           , (float) 1.0        ) ||
        !parameters()->add( "Edge Detector Theshold"                ,  "-ethresh"           , (float) 5        ) ||
        !parameters()->add( "Edge Detector Type( 0-normal , 1-3rd order)"                ,  "-etype"           , (int)1        )||// 0 for general and 1 for 
        //grouping parameters
        !parameters()->add( "Position uncertainty" , "-dx" , 0.3 ) ||
        !parameters()->add( "Orientation uncertainty(Deg)" , "-dt" , 30.0 ) ||
        !parameters()->add( "Radius of Neighborhood" , "-nrad" , 3 )||
        !parameters()->add( "Maximum # of edgels to group" , "-max_size_to_group", (unsigned) 60 ) ||
        !parameters()->add( "threshold on size to estimate the true edges","-thresh_size", (unsigned) 30 )

)

    {
        vcl_cerr << "ERROR: Adding parameters in dbrl_superresolution_multiple_objects::dbrl_superresolution_multiple_objects()" << vcl_endl;
    }

}

//: Destructor
dbrl_superresolution_multiple_objects::~dbrl_superresolution_multiple_objects()
{

}


//: Return the name of this process
vcl_string
dbrl_superresolution_multiple_objects::name()
{
    return "SuperResolution Multiple Objects";
}


//: Return the number of input frame for this process
int
dbrl_superresolution_multiple_objects::input_frames()
{
    return 1;
}


//: Return the number of output frames for this process
int
dbrl_superresolution_multiple_objects::output_frames()
{
    return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_superresolution_multiple_objects::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "dbinfo_track_storage" );
    to_return.push_back( "image" );
    return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_superresolution_multiple_objects::get_output_type()
{  
    vcl_vector<vcl_string > to_return;

    static bool onlyfgedges=false;
    parameters()->get_value("-onlyfgedges",onlyfgedges);
    if(onlyfgedges)
        to_return.push_back( "vsol2D" );
    else
    {
    to_return.push_back( "vsol2D" );
    to_return.push_back( "vsol2D" );
    to_return.push_back( "vsol2D" );
    //to_return.push_back( "vsol2D" );
    }
    //to_return.push_back( "dbrl_id_point_2d" );
    //to_return.push_back( "vsol2D" );
    // int winsize=0;
    //parameters()->get_value("-winsize",winsize);
    //for (int i=0;i<2*winsize+1;i++)
    //     to_return.push_back( "vsol2D" );

    //: original frame
    //to_return.push_back( "vsol2D" );
    //: original convex hull
    //to_return.push_back( "vsol2D" );
    //: improved convex hull
    //to_return.push_back( "vsol2D" );
    //to_return.push_back( "image" );

    return to_return;
}


//: Execute the process
bool
dbrl_superresolution_multiple_objects::execute()
{
    if ( input_data_.size() != 1 ){
        vcl_cout << "In dbrl_superresolution_multiple_objects::execute() - "
            << "not exactly two input images \n";
        return false;
    }
    clear_output();

    //: obtain params for affine rpm.
    static float Tinit=1.0;
    static float Tfinal=0.0001;
    static float annealrate=0.93;
    static float lambdainit=1000;
    static float mconvg=0.1;
    static float moutlier=1e-5;
    static float scale=1;
    static float rad=10;
    static float std=5;
    static int numiter=3;
    static bool iscocirc=true;
    static float gridspace=0.2;
    bpro1_filepath file_path;
    static float thresh=0.0;
    static bool onlyfgedges=0.0;

    static vcl_string disttype="Eculidean";

    parameters()->get_value("-initT",Tinit);
    parameters()->get_value("-finalT",Tfinal);
    parameters()->get_value("-annealrate",annealrate);
    parameters()->get_value("-initlambda",lambdainit);
    parameters()->get_value("-mconvg",mconvg);
    parameters()->get_value("-moutlier",moutlier);
    parameters()->get_value("-upscale",scale);
    parameters()->get_value("-radius",rad);
    parameters()->get_value("-numiter",numiter);
    parameters()->get_value("-std",std);
    parameters()->get_value("-thresh",thresh);
    parameters()->get_value("-disttype",disttype);
    parameters()->get_value("-gridspace",gridspace);
    int winsize=0;
    parameters()->get_value("-winsize",winsize);
    parameters()->get_value("-onlyfgedges",onlyfgedges);
    parameters()->get_value("-edgemodel" , file_path );

    vcl_string path = file_path.path;


    //if(!model_storage_)
    //{
    //    model_storage_ =  dbbgm_distribution_image_storage_new();
    //    vcl_cout<<"Loading Edge Bg Model \n";
    //    vsl_b_ifstream is(path);
    //    model_storage_->b_read(is);
    //    model_storage_->set_frame(-2);
    //    vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
    //    if(!res)
    //        return false;
    //    res->initialize_global();
    //    // frame index of -2 should indicate global storage
    //    if(!res->store_data_at(model_storage_, -2))
    //    {
    //        vcl_cout << "In dbrl_superresolution_multiple_objects "
    //            << "store to repository failed\n";
    //        return false;
    //    }
    //}

    if(!model_storage_)
    {
        model_storage_ =  dbbgm_image_storage_new();
        vcl_cout<<"Loading Edge Bg Model \n";
        vsl_b_ifstream is(path);
        model_storage_->b_read(is);
        model_storage_->set_frame(-2);
        //vidpro1_repository_sptr res = bvis1_manager::instance()->repository();
        //if(!res)
        //    return false;
        //res->initialize_global();
        //// frame index of -2 should indicate global storage
        //if(!res->store_data_at(model_storage_, -2))
        //{
        //    vcl_cout << "In dbrl_superresolution_multiple_objects "
        //        << "store to repository failed\n";
        //    return false;
        //}
    }

    //if(!open_file)
    //{
    //bpro1_filepath out_file_path;
    //parameters()->get_value("-outpoly" , out_file_path );
    //vcl_string outfile = out_file_path.path;
    //ofile=new vsl_b_ofstream(outfile.c_str());
    //open_file=true;
    //}
    //: initiating the registration process.
    //: obtain the tracks 
    dbinfo_track_storage_sptr track_storage;
    track_storage.vertical_cast(input_data_[0][0]);

    vidpro1_image_storage_sptr image_storage;
    image_storage.vertical_cast(input_data_[0][1]);
    vil_image_resource_sptr image_sptr = image_storage->get_image();

    //dbbgm_image_storage_sptr frame_model;
    //frame_model.vertical_cast(input_data_[0][2]);


    //dbbgm_distribution_image<float> *model = dynamic_cast<dbbgm_distribution_image<float>*>(model_storage_->dist_image().ptr());
    if(tracks_.size()==0)
        tracks_=track_storage->tracks();
    int  frame_no=image_storage->frame();

    vcl_cout<<"Frame no: "<<frame_no<<"\n";
    vidpro1_vsol2D_storage_sptr output_vsol_subpixel = vidpro1_vsol2D_storage_new();
    //output_data_[0].push_back(output_vsol_subpixel);

    vidpro1_vsol2D_storage_sptr output_curr_edges = vidpro1_vsol2D_storage_new();
    vidpro1_vsol2D_storage_sptr output_vsol_superimposed = vidpro1_vsol2D_storage_new();
    vidpro1_vsol2D_storage_sptr output_vsol_superimposed_points = vidpro1_vsol2D_storage_new();
    dbrl_id_point_2d_storage_sptr output_id_points=dbrl_id_point_2d_storage_new();
    vidpro1_vsol2D_storage_sptr origpoints= new vidpro1_vsol2D_storage();
    vidpro1_vsol2D_storage_sptr griddisp= vidpro1_vsol2D_storage_new();
    vidpro1_vsol2D_storage_sptr imphulls= vidpro1_vsol2D_storage_new();
    //vidpro1_image_storage_sptr images= vidpro1_image_storage_new();
    //vcl_vector<dbrl_id_point_2d_sptr> reliable_idpoints;
    vcl_vector<dbru_multiple_instance_object_sptr> multiple_objects;
    int cnt=0;
    for(vcl_vector<dbinfo_track_sptr>::iterator trit = tracks_.begin();
        trit != tracks_.end(); trit++)
    {
        vcl_vector<vil_image_view<float> > superimgs;
        if(frame_no>=(*trit)->start_frame() && frame_no<=(*trit)->end_frame())
        {
            vcl_vector<vcl_vector<dbrl_id_point_2d_sptr> > xformed_all_pivot_pointedges;
            vcl_vector<dbrl_id_point_2d_sptr> all_pivot_pointedges;
            dbinfo_observation_sptr ref_obs=(*trit)->observ(frame_no); //: observation in the current frame
            vcl_vector<dbrl_id_point_2d_sptr>  pivot_pointedges=get_foreground_edges_from_observ(ref_obs,model_storage_);
            dbru_multiple_instance_object_sptr  objects=new dbru_multiple_instance_object();
            vcl_vector<vil_image_view<vxl_byte> > xformedimgs;
            if(ref_obs.ptr())
            {
                vcl_map<dbrl_feature_sptr,unsigned char> superres_samples;
                vnl_matrix<double> globalintensities(ref_obs->obs_snippet()->ni()*2,ref_obs->obs_snippet()->nj()*2);
                globalintensities.fill(0.0);
                vnl_matrix<int> globalfreq(ref_obs->obs_snippet()->ni()*2,ref_obs->obs_snippet()->nj()*2);
                globalfreq.fill(0);
                int globalcmin=ref_obs->ex_roi()->cmin(0);
                int globalrmin=ref_obs->ex_roi()->rmin(0);
                vsol_polygon_2d_sptr polytest=ref_obs->geometry()->poly(0);
                vsol_polygon_2d_sptr newpolytest=dilate_poly(polytest,3.0);

                vcl_vector<dbrl_feature_sptr> f1;
                vcl_vector<vsol_spatial_object_2d_sptr> f1pts;
                double xrefc=0,yrefc=0;
                for(int k=0;k<pivot_pointedges.size();k++)
                {
                    xrefc+=pivot_pointedges[k]->x();
                    yrefc+=pivot_pointedges[k]->y();

                    vsol_point_2d_sptr p=new vsol_point_2d(pivot_pointedges[k]->x(),pivot_pointedges[k]->y());
                    vsol_point_2d_sptr p0=new vsol_point_2d(p->x()+0.2*vcl_cos(pivot_pointedges[k]->tangent()),p->y()+0.2*vcl_sin(pivot_pointedges[k]->tangent()));
                    vsol_point_2d_sptr p1=new vsol_point_2d(p->x()-0.2*vcl_cos(pivot_pointedges[k]->tangent()),p->y()-0.2*vcl_sin(pivot_pointedges[k]->tangent()));
                    vsol_line_2d_sptr l=new vsol_line_2d(p0,p1);
                    f1pts.push_back(l->cast_to_spatial_object());

                }
                xrefc/=pivot_pointedges.size();
                yrefc/=pivot_pointedges.size();

                vcl_vector<dbrl_feature_sptr> ptk_features=get_features(pivot_pointedges,disttype);
                //: converting points into features
                //vcl_vector<dbrl_id_point_2d_sptr> orig_pivot_pointedges;
                //vcl_vector<dbdet_edgel* > all_edgels;
                //for(int k=0;k<pivot_pointedges.size();k++)
                //    all_edgels.push_back(new dbdet_edgel(vgl_point_2d<double>(pivot_pointedges[k]->x(),pivot_pointedges[k]->y()), pivot_pointedges[k]->tangent()));
                //
                ////vcl_vector<dbrl_feature_sptr> ptk_features=get_curvature_edges(all_edgels);
                //vcl_vector<dbrl_feature_sptr> ptk_features=get_neighbor_edges(all_edgels);
                
                vcl_vector<vgl_point_2d<double> > orgpts;
                for(unsigned k=0;k<ptk_features.size();k++)
                {
                    if(dbrl_feature_point_tangent_curvature_groupings * pt
                        = dynamic_cast<dbrl_feature_point_tangent_curvature_groupings* >(ptk_features[k].ptr()))
                    {
                        vnl_vector<double> loc(2);
                        loc[0]=pt->location()[0]-xrefc;
                        loc[1]=pt->location()[1]-yrefc;
                        pt->set_location(loc);
                        orgpts.push_back(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));
                        f1.push_back(pt);
                    }
                    else if(dbrl_feature_point_tangent_curvature * pt= dynamic_cast<dbrl_feature_point_tangent_curvature* >(ptk_features[k].ptr()))
                    {
                        vnl_vector<double> loc(2);
                        loc[0]=pt->location()[0]-xrefc;
                        loc[1]=pt->location()[1]-yrefc;
                        pt->set_location(loc);
                        orgpts.push_back(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));
                        f1.push_back(pt);
                    }

                    else if(dbrl_feature_point_tangent * pt= dynamic_cast<dbrl_feature_point_tangent* >(ptk_features[k].ptr()))
                    {
                        vnl_vector<double> loc(2);
                        loc[0]=pt->location()[0]-xrefc;
                        loc[1]=pt->location()[1]-yrefc;
                        pt->set_location(loc);
                        orgpts.push_back(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));
                        f1.push_back(pt);
                    }
                    else if(dbrl_feature_point * pt= dynamic_cast<dbrl_feature_point * >(ptk_features[k].ptr()))
                    {
                        vnl_vector<double> loc(2);
                        loc[0]=pt->location()[0]-xrefc;
                        loc[1]=pt->location()[1]-yrefc;
                        pt->set_location(loc);
                        orgpts.push_back(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));
                        f1.push_back(pt);
                    }


                }

                //for(int k=0;k<pivot_pointedges.size();k++)
                //{
                //    
                //    vnl_vector_fixed<double,2> pt((pivot_pointedges[k]->x()-xrefc),(pivot_pointedges[k]->y()-yrefc));
                //    dbrl_feature_point_tangent * fpt=new dbrl_feature_point_tangent(pt,pivot_pointedges[k]->tangent());
                //    fpt->setid(pivot_pointedges[k]->id());
                //    f1.push_back(fpt);
                //    vsol_point_2d_sptr p=new vsol_point_2d(pivot_pointedges[k]->x(),pivot_pointedges[k]->y());
                //    vsol_point_2d_sptr p0=new vsol_point_2d(p->x()-0.2*vcl_sin(fpt->dir()),p->y()+0.2*vcl_cos(fpt->dir()));
                //    vsol_point_2d_sptr p1=new vsol_point_2d(p->x()+0.2*vcl_sin(fpt->dir()),p->y()-0.2*vcl_cos(fpt->dir()));

                //    vsol_line_2d_sptr l=new vsol_line_2d(p0,p1);
                //    f1pts.push_back(l->cast_to_spatial_object());

                //    f1pts.push_back(p->cast_to_spatial_object());
                //    double tan = vcl_atan2 (p1->y() - p0->y(), p1->x() - p0->x());
                //    orig_pivot_pointedges.push_back(new dbrl_id_point_2d(p->x(),p->y(),tan,k));
                //}
                //vcl_vector<vgl_point_2d<double> > orgpts;
                //for(unsigned i=0;i<pivot_pointedges.size();i++)
                //    orgpts.push_back(vgl_point_2d<double>(pivot_pointedges[i]->x(),pivot_pointedges[i]->y()));
                if(f1.size()>2)
                {
                    //: computing the convex hull of the original set of edges
                    vgl_convex_hull_2d<double> orghull(orgpts);
                    vgl_polygon<double> orgp=orghull.hull();
                    vcl_vector<vsol_point_2d_sptr> orgcontour;
                    for(int k=0;k<orgp[0].size();k++)
                    {
                        vsol_point_2d_sptr p =new vsol_point_2d(orgp[0][k].x(),orgp[0][k].y());
                        orgcontour.push_back(p);
                    }
                    vsol_polygon_2d_sptr neworgContour = new vsol_polygon_2d (orgcontour);//ctracer.largest_contour()
                    //orgconvhull->add_object(neworgContour->cast_to_spatial_object());
                    //: superimposing neighboring frames      
                    vcl_vector<vcl_vector< vsol_spatial_object_2d_sptr > > xpoints;  

                    if(!onlyfgedges)
                    {
                    for(int i=frame_no-winsize;i<=frame_no+winsize;i++)
                    {
                        vcl_cout<<"Match frame no: "<<i<<"\n";
                        if(i>=(*trit)->start_frame() && i<=(*trit)->end_frame() && i!=frame_no)
                        {
                            dbinfo_observation_sptr temp_obs=(*trit)->observ(i);
                            if(temp_obs.ptr())
                            {
                                if(temp_obs->obs_snippet().ptr())
                                {
                                    vil_image_view<unsigned char> img_view=temp_obs->obs_snippet()->get_view();
                                    //vil_save(img_view,"d:/temp.tif");
                                    brip_roi_sptr exroi=temp_obs->ex_roi();
                                    vcl_vector<dbrl_id_point_2d_sptr>  curr_pointedges=
                                        get_foreground_edges_from_observ(temp_obs,model_storage_);
                                   // vcl_vector<dbdet_edgel* > curr_edgels;
                                   // for(int k=0;k<curr_pointedges.size();k++)
                                   //     curr_edgels.push_back(new dbdet_edgel(vgl_point_2d<double>(curr_pointedges[k]->x(),curr_pointedges[k]->y()), curr_pointedges[k]->tangent()));

                                   //vcl_vector<dbrl_feature_sptr> f2=get_neighbor_edges(curr_edgels);
                                    vcl_vector<dbrl_feature_sptr> f2=get_features(curr_pointedges,disttype);
                                    
                                    //vcl_vector<dbrl_feature_sptr> f2;
                                    double xcurc=0,ycurc=0;
                                    for(int k=0;k<curr_pointedges.size();k++)
                                    {
                                        xcurc+=curr_pointedges[k]->x();
                                        ycurc+=curr_pointedges[k]->y();
                                    }
                                    xcurc/=curr_pointedges.size();
                                    ycurc/=curr_pointedges.size();
                                    vcl_vector<vsol_spatial_object_2d_sptr> f2pts;
                                    for(unsigned k=0;k<curr_pointedges.size();k++)
                                    {
                                        if(dbrl_feature_point_tangent_curvature_groupings * pt
                                            = dynamic_cast<dbrl_feature_point_tangent_curvature_groupings* >(f2[k].ptr()))
                                        {
                                            vnl_vector<double> loc(2);
                                            loc[0]=pt->location()[0]-xcurc;
                                            loc[1]=pt->location()[1]-ycurc;
                                            pt->set_location(loc);
                                        }
                                        else if(dbrl_feature_point_tangent * pt
                                            = dynamic_cast<dbrl_feature_point_tangent* >(f2[k].ptr()))
                                        {
                                            vnl_vector<double> loc(2);
                                            loc[0]=pt->location()[0]-xcurc;
                                            loc[1]=pt->location()[1]-ycurc;
                                            pt->set_location(loc);
                                        }
                                        else if(dbrl_feature_point * pt
                                            = dynamic_cast<dbrl_feature_point * >(f2[k].ptr()))
                                        {
                                            vnl_vector<double> loc(2);
                                            loc[0]=pt->location()[0]-xcurc;
                                            loc[1]=pt->location()[1]-ycurc;
                                            pt->set_location(loc);
                                        }

                                        vsol_point_2d_sptr p=new vsol_point_2d(curr_pointedges[k]->x()-xcurc,curr_pointedges[k]->y()-ycurc);
                                        f2pts.push_back(p->cast_to_spatial_object());
                                    }

                                    //: applying translation 
                                    lambdainit=vcl_max(f1.size(),f2.size());
                                    dbrl_rpm_affine_params affineparams(lambdainit,mconvg,Tinit,Tfinal,moutlier,annealrate);

                                    //: applying affine 
                                    dbrl_rpm_affine match_affinely(affineparams,f1,f2);
                                    dbrl_match_set_sptr match_set=match_affinely.rpm(disttype);

                                    ////: binarize the match matrix
                                    dbrl_correspondence M=match_set->get_correspondence();
                                    M.setinitialoutlier(affineparams.outlier());
                                    dbrl_estimator_point_affine* final_affine_est= new dbrl_estimator_point_affine();
                                    final_affine_est->set_lambda(0.0);
                                    vcl_vector<dbrl_feature_sptr> affinef1xform=f1;
                                    //match_set->normalize_point_set(M.M(),affinef1xform );
                                    dbrl_transformation_sptr final_affine_tform=match_set->get_transformation();//final_affine_est->estimate(f1,f2,M);
                                    dbrl_affine_transformation * final_affine_form
                                        =dynamic_cast<dbrl_affine_transformation *> (final_affine_tform.ptr());
                                    final_affine_form->set_from_features(f2);
                                    final_affine_form->transform();
                                    //vcl_vector<dbrl_feature_sptr> affinef2xformed=final_affine_form->get_to_features();
                                    vcl_vector<dbrl_feature_sptr> affinef2xformed=final_affine_form->get_to_features();
                                    final_affine_form->print_summary(vcl_cout);
                                    //: in order to solve multiple correspondences.
                                    //M.binarize(0.51);
                                    ////: retreiving final transform using TPS
                                    dbrl_estimator_point_thin_plate_spline* tps_est= new dbrl_estimator_point_thin_plate_spline();
                                    tps_est->set_lambda1(0.001);
                                    tps_est->set_lambda2(0.0);

                                    vcl_vector<dbrl_feature_sptr> f1xform=match_set->get_feature_set1();
                                    match_set->normalize_point_set(M.M(),f1xform );
                                    dbrl_transformation_sptr tform=tps_est->estimate(f1xform,affinef2xformed,M);
                                    dbrl_thin_plate_spline_transformation * tpstform=dynamic_cast<dbrl_thin_plate_spline_transformation *> (tform.ptr());
                                    tpstform->set_from_features(affinef2xformed);
                                    tpstform->transform();
                                    vcl_vector<dbrl_feature_sptr> f2xformed=tpstform->get_to_features();

                                    //dbrl_estimator_cubic_patch* cubic_est= new dbrl_estimator_cubic_patch();
                                    //vcl_vector<dbrl_feature_sptr> f1xform=match_set->get_feature_set1();
                                    //////dbrl_transformation_sptr tform=cubic_est->estimate(f1xform,match_set->get_feature_set2(),M);
                                    //dbrl_transformation_sptr tform=cubic_est->estimate(f1xform,affinef2xformed,M);
                                    //dbrl_cubic_transformation * cubictform=dynamic_cast<dbrl_cubic_transformation *> (tform.ptr());
                                    //cubictform->set_from_features(affinef2xformed);
                                    //cubictform->transform();
                                    //vcl_vector<dbrl_feature_sptr> f2xformed=cubictform->get_to_features();

                                    //: grid points for super res
                                    vcl_vector<dbrl_feature_sptr> frame_gridpoints=get_grid_points(f2,1.0);
                                    final_affine_form->set_from_features(frame_gridpoints);
                                    final_affine_form->transform();
                                    vcl_vector<dbrl_feature_sptr> affine_frame_gridpoints=final_affine_form->get_to_features();
                                    //cubictform->set_from_features(affine_frame_gridpoints);
                                    //cubictform->transform();
                                    //vcl_vector<dbrl_feature_sptr> cubic_frame_gridpoints=cubictform->get_to_features();

                                    tpstform->set_from_features(affine_frame_gridpoints);
                                    tpstform->transform();
                                    vcl_vector<dbrl_feature_sptr> tps_frame_gridpoints=tpstform->get_to_features();

                                    for(unsigned m=0;m<tps_frame_gridpoints.size();m++)
                                    {
                                        if(dbrl_feature_point* pt
                                            =dynamic_cast<dbrl_feature_point*>(frame_gridpoints[m].ptr()))
                                        {
                                           int xs=vcl_floor(pt->location()[0]+xcurc)-exroi->cmin(0);
                                            int ys=vcl_floor(pt->location()[1]+ycurc)-exroi->rmin(0);
                                            if(xs<img_view.ni() && ys<img_view.nj() && xs>=0 && ys>=0)
                                            {
                                                if(dbrl_feature_point* pt1=dynamic_cast<dbrl_feature_point*>(tps_frame_gridpoints[m].ptr()))
                                                {
                                                    vnl_vector<double> loc(2);
                                                    loc[0]=pt1->location()[0]+xrefc;
                                                    loc[1]=pt1->location()[1]+yrefc;
                                                    pt1->set_location(loc);
                                                    superres_samples[tps_frame_gridpoints[m]]=img_view(xs,ys);
                                                }
                                            }
                                        }

                                    }

                                    //: xform grid points
                                    vcl_vector<dbrl_feature_sptr> gridpoints=get_grid_points(f2,gridspace);
                                    final_affine_form->set_from_features(gridpoints);
                                    final_affine_form->transform();
                                    vcl_vector<dbrl_feature_sptr> affinexfomredgridpoints=final_affine_form->get_to_features();
                                    tpstform->set_from_features(affinexfomredgridpoints);
                                    tpstform->transform();
                                    vcl_vector<dbrl_feature_sptr> xfomredgridpoints=tpstform->get_to_features();
                                    //vcl_vector<dbrl_feature_sptr> xfomredgridpoints=final_affine_form->get_to_features();

                                    vcl_vector<vsol_spatial_object_2d_sptr> gridpts;
                                    vcl_vector<vsol_spatial_object_2d_sptr> xgridpts;

                                    vnl_matrix<float> intensities(image_sptr->ni(),image_sptr->nj());
                                    intensities.fill(0.0);
                                    vnl_matrix<int> freq(image_sptr->ni(),image_sptr->nj());
                                    freq.fill(0);

                                    for(unsigned m=0;m<gridpoints.size();m++)
                                    {
                                        vsol_point_2d_sptr p1;
                                        if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(gridpoints[m].ptr()))
                                        {
                                            p1=new vsol_point_2d(pt->location()[0]*scale+xcurc,pt->location()[1]*scale+ycurc);
                                            //gridpts.push_back(p1->cast_to_spatial_object());
                                        }
                                        if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(xfomredgridpoints[m].ptr()))
                                        {
                                            vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0]*scale+xrefc,pt->location()[1]*scale+yrefc);
                                            if(vcl_floor(p->x())<intensities.rows() && vcl_floor(p->x())>=0 &&
                                                vcl_floor(p->y())<intensities.cols() && vcl_floor(p->y())>=0)
                                            {
                                                intensities(vcl_floor(p->x()),vcl_floor(p->y()))+=img_view(vcl_floor(p1->x())-exroi->cmin(0),vcl_floor(p1->y())-exroi->rmin(0));
                                                freq(vcl_floor(p->x()),vcl_floor(p->y()))++;
                                            }
                                            else
                                            {
                                                vcl_cout<<"\n Error";
                                            }
                                            //if(vcl_floor(2*(p->x()-globalcmin))<globalintensities.rows() && vcl_floor(2*(p->x()-globalcmin))>=0 &&
                                            //    vcl_floor(2*(p->y()-globalrmin))<globalintensities.cols() &&vcl_floor(2*(p->y()-globalrmin))>=0)
                                            //{
                                            //    globalintensities(vcl_floor(2*(p->x()-globalcmin)),vcl_floor(2*(p->y()-globalrmin)))+=img_view(vcl_floor(p1->x())-exroi->cmin(0),vcl_floor(p1->y())-exroi->rmin(0));
                                            //    globalfreq(vcl_floor(2*(p->x()-globalcmin)),vcl_floor(2*(p->y()-globalrmin)))++;
                                            //}
                                        }
                                    }

                                    vil_image_view<float> img_result(image_sptr->ni(),image_sptr->nj());
                                    
                                    for(unsigned k=0;k<img_result.ni();k++)
                                        for(unsigned l=0;l<img_result.nj();l++)
                                        {
                                            if(freq(k,l)>0)
                                            {
                                                float val=(float)intensities(k,l)/(float)freq(k,l);
                                                img_result(k,l)=val;
                                            }
                                        }
                                        superimgs.push_back(img_result);
                                      //  vil_save(brip_vil_float_ops::convert_to_byte(img_result),"d:/temp.tiff");
                                    //vcl_string namegrid=vul_sprintf("grid%d",i);
                                    vcl_vector<dbrl_id_point_2d_sptr> f2xid;
                                    f2pts.clear();

                                    for(unsigned m=0;m<f2xformed.size();m++)
                                    {
                                        if(dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(f2xformed[m].ptr()))
                                        {
                                            vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0]*scale+xrefc,pt->location()[1]*scale+yrefc);
                                            f2pts.push_back(p->cast_to_spatial_object());
                                            f2xid.push_back(new dbrl_id_point_2d(p->x(),p->y(),m));
                                        }
                                        else if(dbrl_feature_point_tangent* pt=dynamic_cast<dbrl_feature_point_tangent*>(f2xformed[m].ptr()))
                                        {
                                            vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0]*scale+xrefc,pt->location()[1]*scale+yrefc);
                                            vsol_point_2d_sptr p0=new vsol_point_2d(p->x()+0.2*vcl_cos(pt->dir()),p->y()+0.2*vcl_sin(pt->dir()));
                                            vsol_point_2d_sptr p1=new vsol_point_2d(p->x()-0.2*vcl_cos(pt->dir()),p->y()-0.2*vcl_sin(pt->dir()));

                                            vsol_line_2d_sptr l=new vsol_line_2d(p0,p1);
                                            f2pts.push_back(p->cast_to_spatial_object());
                                            f2pts.push_back(l->cast_to_spatial_object());

                                            double tan = vcl_atan2 (p1->y() - p0->y(), p1->x() - p0->x());
                                            f2xid.push_back(new dbrl_id_point_2d(p->x(),p->y(),tan,m));
                                        }
                                         else if(dbrl_feature_point_tangent_curvature* pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f2xformed[m].ptr()))
                                        {
                                            vsol_point_2d_sptr p=new vsol_point_2d(pt->location()[0]*scale+xrefc,pt->location()[1]*scale+yrefc);
                                            vsol_point_2d_sptr p0=new vsol_point_2d(p->x()+0.2*vcl_cos(pt->dir()),p->y()+0.2*vcl_sin(pt->dir()));
                                            vsol_point_2d_sptr p1=new vsol_point_2d(p->x()-0.2*vcl_cos(pt->dir()),p->y()-0.2*vcl_sin(pt->dir()));

                                            vsol_line_2d_sptr l=new vsol_line_2d(p0,p1);
                                            f2pts.push_back(p->cast_to_spatial_object());
                                            f2pts.push_back(l->cast_to_spatial_object());

                                            double tan = vcl_atan2 (p1->y() - p0->y(), p1->x() - p0->x());
                                            f2xid.push_back(new dbrl_id_point_2d(p->x(),p->y(),tan,m));
                                        }
                                    }
                                    vcl_string namepoints=vul_sprintf("points%d",i);
                                    output_vsol_superimposed->add_objects(f2pts,namepoints);
                                    xformed_all_pivot_pointedges.push_back(f2xid);
                                }
                            }
                        }
                        else if(i==frame_no)
                        {
                            vcl_string namepoints=vul_sprintf("points%d",i);
                            //output_vsol_superimposed->add_objects(f1pts,namepoints);
                            //output_curr_edges->add_objects(f1pts);
                            xformed_all_pivot_pointedges.push_back(pivot_pointedges);
                            vil_image_view<float> imgtemp=brip_vil_float_ops::convert_to_float(image_sptr);
                            superimgs.push_back(imgtemp);
                        }
                    }
                    //: code for creating a super-res image
                    //vil_image_view<unsigned char> superimg(globalintensities.rows(),globalintensities.cols());
                    //for(unsigned k=0;k<superimg.ni();k++)
                    //{ 
                    //    for(unsigned l=0;l<superimg.nj();l++)
                    //    {
                    //        if(globalfreq(k,l)>0)
                    //            superimg(k,l)=(unsigned char)((double)globalintensities(k,l)/(double)globalfreq(k,l));
                    //    }
                    //}
                    //
                    //vil_save(superimg,"d:/tempsuper.tiff");
                    vcl_vector<vsol_line_2d_sptr> supportedges=compute_spatial_support(std,thresh,xformed_all_pivot_pointedges);
                    //vcl_vector<vsol_line_2d_sptr> supportedges=compute_curve_model_support(xformed_all_pivot_pointedges,image_sptr->ni(),image_sptr->nj());
                    for(unsigned i=0;i<supportedges.size();i++)
                    {
                        imphulls->add_object(supportedges[i]->cast_to_spatial_object());
                    }
                    vsol_polygon_2d_sptr p=compute_convex_hull(supportedges);
                    if(p->size()>0)
                    {
                        imphulls->add_object(p->cast_to_spatial_object());
                        vsol_box_2d_sptr boxp=p->get_bounding_box();
                        brip_roi_sptr roiptr=new brip_roi(image_sptr->ni(),image_sptr->nj());

                        vsol_box_2d_sptr boxnew=new vsol_box_2d();
                        boxnew->add_point(boxp->get_min_x()-10,boxp->get_min_y()-10);
                        boxnew->add_point(boxp->get_max_x()+10,boxp->get_min_y()-10);
                        boxnew->add_point(boxp->get_min_x()-10,boxp->get_max_y()+10);
                        boxnew->add_point(boxp->get_max_x()+10,boxp->get_max_y()+10);

                        for(unsigned k=0;k<superimgs.size();k++)
                        {
                            vil_image_view<float> chip;
                            brip_vil_float_ops::chip(superimgs[k],boxnew,chip);
                            xformedimgs.push_back(brip_vil_float_ops::convert_to_byte(chip,0,255));
                        }

                        objects->set_images(xformedimgs);
                        objects->set_poly(p);
                        objects->set_edges(supportedges);
                        objects->set_bbox(boxp);
                        objects->set_bigbox(boxnew);
                        multiple_objects.push_back(objects);
                    }
                    }
                }
                output_curr_edges->add_objects(f1pts);
                vcl_string superimgname=vul_sprintf("d:/img%d-%d",frame_no,(*trit)->id());
                //super_resolute(superres_samples,ref_obs,superimgname);

            }
        }
    }

    multiobs.push_back(multiple_objects);
    output_data_[0].push_back(output_curr_edges);
    if(!onlyfgedges)
    {
        output_data_[0].push_back(output_vsol_superimposed);
        //output_data_[0].push_back(output_curr_edges);
        output_data_[0].push_back(imphulls);
    }
    return true;
}
//: Clone the process
bpro1_process*
dbrl_superresolution_multiple_objects::clone() const
{
    return new dbrl_superresolution_multiple_objects(*this);
}

bool
dbrl_superresolution_multiple_objects::finish()
{
    bpro1_filepath file_path;
    parameters()->get_value("-outpoly" , file_path );
    vcl_string path = file_path.path;
    vsl_b_ofstream ofile(path.c_str());
    vsl_b_write(ofile,multiobs.size());
    for(unsigned i=0;i<multiobs.size();i++)
        vsl_b_write(ofile,multiobs[i]);
    ofile.close();

    return true;
}

vcl_vector<vsol_line_2d_sptr>
dbrl_superresolution_multiple_objects::compute_curve_model_support(vcl_vector< vcl_vector< dbrl_id_point_2d_sptr > > pts, int ncols,int nrows)
{
    vcl_vector<dbdet_edgel*> all_edgels;
    for(unsigned j=0;j<pts.size();j++)
        for(unsigned i=0;i<pts[j].size();i++)
            all_edgels.push_back(new dbdet_edgel(vgl_point_2d<double>(pts[j][i]->x(),pts[j][i]->y()),pts[j][i]->tangent()));

    static int nrad;
    static double dx;
    static double dt;
    unsigned max_size_to_group;
    unsigned thresh_size;

    parameters()->get_value( "-nrad", nrad);
    parameters()->get_value( "-dx", dx);
    parameters()->get_value( "-dt", dt);
    parameters()->get_value( "-max_size_to_group", max_size_to_group );
    parameters()->get_value( "-thresh_size", thresh_size );

    dbdet_edgemap_sptr edgemap=new dbdet_edgemap(ncols,nrows,all_edgels);

    //different types of linkers depending on the curve model
    typedef dbdet_sel<dbdet_ES_curve_model> dbdet_sel_ES;
    typedef dbdet_sel<dbdet_CC_curve_model> dbdet_sel_CC;

    dbdet_sel_sptr edge_linker= new dbdet_sel_CC(edgemap, nrad, dt*vnl_math::pi/180, dx);
    edge_linker->build_curvelets_greedy(max_size_to_group);

    vcl_vector<dbdet_edgel*> edgels=edge_linker->get_edgels();
    vcl_vector<dbdet_edgel*> sedgels;
    for(unsigned i=0;i<edgels.size();i++)
    {
            curvelet_list_iter cv_it = edgels[i]->curvelets.begin();
            for ( ; cv_it!=edgels[i]->curvelets.end(); cv_it++)
            {
                if((*cv_it)->edgel_chain.size()>thresh_size)
                {
                    sedgels.push_back(edgels[i]);
                    continue;
                }
            }
        
    }
    vcl_vector<vsol_line_2d_sptr> supportedges;

    for(unsigned j=0;j<sedgels.size();j++)
    {
            vsol_point_2d_sptr p=new vsol_point_2d(sedgels[j]->pt);
            vsol_point_2d_sptr ps=new vsol_point_2d(p->x()+0.2*vcl_cos(sedgels[j]->tangent),p->y()+0.2*vcl_sin(sedgels[j]->tangent));
            vsol_point_2d_sptr pe=new vsol_point_2d(p->x()-0.2*vcl_cos(sedgels[j]->tangent),p->y()-0.2*vcl_sin(sedgels[j]->tangent));
            vsol_line_2d_sptr l=new vsol_line_2d(ps,pe);
            supportedges.push_back(l);
    }

    return supportedges;


}
vcl_vector<dbrl_feature_sptr> dbrl_superresolution_multiple_objects::get_features(vcl_vector<dbrl_id_point_2d_sptr> points,
                                                                                  vcl_string type)
{
    vcl_vector<dbrl_feature_sptr> fs;

    if(type=="Euclidean")
    {
        for(int k=0;k<points.size();k++)
        {
            vnl_vector_fixed<double,2> pt((points[k]->x()),(points[k]->y()));
            dbrl_feature_point* fpt=new dbrl_feature_point(pt);
            fpt->setid(points[k]->id());
            fs.push_back(fpt);
        }
    }
    else if(type=="Projected")
    {
        for(int k=0;k<points.size();k++)
        {
            vnl_vector_fixed<double,2> pt((points[k]->x()),(points[k]->y()));
            dbrl_feature_point_tangent * fpt=new dbrl_feature_point_tangent(pt,points[k]->tangent());
            fpt->setid(points[k]->id());
            fs.push_back(fpt);
        }
    }
    else if(type=="Circular")
    {
        vcl_vector<dbdet_edgel* > all_edgels;
        for(int k=0;k<points.size();k++)
            all_edgels.push_back(new dbdet_edgel(vgl_point_2d<double>(points[k]->x(),points[k]->y()), points[k]->tangent()));

        fs=get_curvature_edges(all_edgels);
        
    }
    else if(type=="NeighborEuclidean")
    {
        vcl_vector<dbdet_edgel* > all_edgels;
        for(int k=0;k<points.size();k++)
            all_edgels.push_back(new dbdet_edgel(vgl_point_2d<double>(points[k]->x(),points[k]->y()), points[k]->tangent()));
        fs=get_neighbor_edges(all_edgels);
    }
    return fs;
}





vcl_vector<dbrl_id_point_2d_sptr> dbrl_superresolution_multiple_objects::get_foreground_edges_from_observ(dbinfo_observation_sptr obs,
                                                                                                          dbbgm_image_storage_sptr  inpmodel)
{
    vcl_vector<dbrl_id_point_2d_sptr> fgidpoints;
    if(!obs.ptr())
        return fgidpoints;
    vil_image_resource_sptr clip_im=obs->obs_snippet();
    vsol_polygon_2d_sptr poly=obs->geometry()->poly(0);
    vsol_polygon_2d_sptr dilated_poly=dilate_poly(poly,3.0);
    vgl_polygon<double> vgp = bsol_algs::vgl_from_poly(dilated_poly);

    if(!clip_im.ptr())
        return fgidpoints;
    vcl_vector<dbdet_edgel*> edges=edge_detector(brip_vil_float_ops::convert_to_byte(clip_im));
    brip_roi_sptr exroi=obs->ex_roi();
    vgl_vector_2d<double> offset(exroi->cmin(0),exroi->rmin(0));
    for(unsigned i=0;i<edges.size();i++)
    {
        edges[i]->pt=edges[i]->pt+offset;
    }

    vcl_vector<dbdet_edgel*> fgedges=fg_detect(edges,inpmodel->dist_image().ptr());
    for(unsigned i=0;i<fgedges.size();i++)
    {
        fgidpoints.push_back(new dbrl_id_point_2d(fgedges[i]->pt.x(),fgedges[i]->pt.y(),fgedges[i]->tangent,i));
    }

    return fgidpoints;
}




vcl_vector<vsol_line_2d_sptr>
dbrl_superresolution_multiple_objects::compute_spatial_support(double sigma_d,double thresh,
                                                               vcl_vector< vcl_vector< dbrl_id_point_2d_sptr > > pts)
{
    unsigned totsize=0;

    if(pts.size()<=0)
        return vcl_vector<vsol_line_2d_sptr>();

    vcl_vector< dbrl_id_point_2d_sptr > allpts;
    for(unsigned i=0;i<pts.size();i++)
    {
        totsize+=pts[i].size();
        allpts.insert(allpts.end(),pts[i].begin(),pts[i].end());
    }
    double *weightst=new double[totsize];

    for(unsigned j=0;j<totsize;j++)
        weightst[j]=0.0;

    for(unsigned j=0,cnt=0;j<pts.size();j++) {
        for(unsigned l=0;l<pts[j].size();l++) {
        double xj=pts[j][l]->x();
        double yj=pts[j][l]->y();
        double tj=pts[j][l]->tangent();
        for(unsigned i=0;i<pts.size();i++){
            double st=0.0;
            if(i!=j)
            {
                for(unsigned k=0;k<pts[i].size();k++){
                    double xi=pts[i][k]->x();
                    double yi=pts[i][k]->y();
                    double ti=pts[i][k]->tangent();
                    st+=compute_support(xj,yj,tj,xi,yi,ti,sigma_d);//vcl_exp(-((xi-xj)*(xi-xj)+(yi-yj)*(yi-yj))/(sigma_d*sigma_d) -(tj-ti)*(tj-ti)/(sigma_a*sigma_a));
                }
                if(weightst[cnt]<st)
                    weightst[cnt]=st;
            }
        }
        cnt++;
        }
    }

    vcl_vector<vsol_spatial_object_2d_sptr> vsol_out;
    vcl_vector<vsol_line_2d_sptr> supportedges;
    for(unsigned j=0;j<totsize;j++)
    {
        if(weightst[j] >thresh)
        {
            vsol_point_2d_sptr p=new vsol_point_2d(allpts[j]->x(),allpts[j]->y());
            vsol_point_2d_sptr ps=new vsol_point_2d(p->x()+0.2*vcl_cos(allpts[j]->tangent()),p->y()+0.2*vcl_sin(allpts[j]->tangent()));
            vsol_point_2d_sptr pe=new vsol_point_2d(p->x()-0.2*vcl_cos(allpts[j]->tangent()),p->y()-0.2*vcl_sin(allpts[j]->tangent()));
            vsol_line_2d_sptr l=new vsol_line_2d(ps,pe);
            supportedges.push_back(l);
            //vsol_out.push_back(p->cast_to_spatial_object());
            //supportpoints.push_back(vgl_point_2d<double>(p->x(),p->y()));
            //vsol_out.push_back(l->cast_to_spatial_object());
        }  
    }


    return supportedges;

    //if(supportpoints.size()<=0)
    //    return new vsol_polygon_2d();
    //vgl_convex_hull_2d<double> hullp(supportpoints);
    //vgl_polygon<double> psg=hullp.hull();

    //if(static_cast<int>(psg[0].size())<3)
    //{
    //    return new vsol_polygon_2d();
    //}

    //vcl_vector<vsol_point_2d_sptr> newpts;
    //for(int k=0;k<static_cast<int>(psg[0].size());k++)
    //{
    //    newpts.push_back(new vsol_point_2d(psg[0][k].x(),psg[0][k].y()));

    //}

    //vidpro1_vsol2D_storage_sptr vsol_storage;
    //vsol_storage.vertical_cast(get_output(0)[0]);

    //vsol_storage->add_objects(vsol_out);
    //return new vsol_polygon_2d(newpts);

}

vsol_polygon_2d_sptr 
dbrl_superresolution_multiple_objects::compute_convex_hull( vcl_vector<vsol_line_2d_sptr > edges)
{
    if(edges.size()<=0)
        return new vsol_polygon_2d();
    vcl_vector<vgl_point_2d<double> > points;
    for(unsigned i=0;i<edges.size();i++)
    {
        vsol_point_2d_sptr p= edges[i]->middle();
        points.push_back(vgl_point_2d<double>(p->x(),p->y()));

    }
    vgl_convex_hull_2d<double> hullp(points);
    vgl_polygon<double> psg=hullp.hull();

    if(static_cast<int>(psg[0].size())<3)
        return new vsol_polygon_2d();

    vcl_vector<vsol_point_2d_sptr> newpts;
    for(int k=0;k<static_cast<int>(psg[0].size());k++)
        newpts.push_back(new vsol_point_2d(psg[0][k].x(),psg[0][k].y()));
    return new vsol_polygon_2d(newpts);
}
vsol_polygon_2d_sptr dbrl_superresolution_multiple_objects::dilate_poly(vsol_polygon_2d_sptr poly, double rad)
{

    vcl_vector<vgl_point_2d<double> > pts;
    for(unsigned i=0;i<poly->size();i++)
        pts.push_back(vgl_point_2d<double>(poly->vertex(i)->x(),poly->vertex(i)->y()));

    vgl_convex_hull_2d<double> hullp(pts);
    vgl_polygon<double> psg=hullp.hull();

    if(static_cast<int>(psg[0].size())<3)
    {
        return new vsol_polygon_2d();
    }

    vcl_vector<vsol_point_2d_sptr> newpts;
    for(int k=0;k<static_cast<int>(psg[0].size())-1;k++)
    {
        vgl_point_2d<double> p0=psg[0][k];
        vgl_point_2d<double> p1=psg[0][k+1];
        vgl_point_2d<double> p=midpoint(p0,p1);

        vgl_vector_2d<double> v(p1.x()-p0.x(),p1.y()-p0.y());
        normalize(v);
        vgl_vector_2d<double> nv(v.y(),-v.x());
        vgl_point_2d<double> outp=p+rad*nv;
        newpts.push_back(new vsol_point_2d(outp.x(),outp.y()));


    }
    vsol_polygon_2d_sptr dilatedpoly=new vsol_polygon_2d(newpts);
    return dilatedpoly;
}




vcl_vector<dbrl_feature_sptr> dbrl_superresolution_multiple_objects::get_grid_points(vcl_vector<dbrl_feature_sptr> f2,
                                                                                     double spacing)
{
    vcl_vector<vgl_point_2d<double> > pts;

    vgl_box_2d<double> box;

    for(unsigned i=0;i<f2.size();i++)
    {
        if(dbrl_feature_point * pt=dynamic_cast<dbrl_feature_point*>(f2[i].ptr()))
            box.add(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));
        else if(dbrl_feature_point_tangent * pt=dynamic_cast<dbrl_feature_point_tangent*>(f2[i].ptr()))
            box.add(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));
                else if(dbrl_feature_point_tangent_curvature * pt=dynamic_cast<dbrl_feature_point_tangent_curvature*>(f2[i].ptr()))
            box.add(vgl_point_2d<double>(pt->location()[0],pt->location()[1]));

    }

    double xmin=vcl_floor(box.min_x());
    double ymin=vcl_floor(box.min_y());
    double xmax=vcl_ceil(box.max_x());
    double ymax=vcl_ceil(box.max_y());

    vcl_vector<dbrl_feature_sptr> fs;
    for(double x=xmin;x<=xmax;)
    {   
        for(double y=ymin;y<=ymax;)
        {
            fs.push_back(new dbrl_feature_point(x,y));
            y+=spacing;
        }
        x+=spacing;
    }
    return fs;

}







vcl_vector<vsol_spatial_object_2d_sptr> dbrl_superresolution_multiple_objects::estimate_support(vcl_vector<dbrl_id_point_2d_sptr> points_list,
                                                                                                vcl_vector<dbrl_id_point_2d_sptr> & supportpoints,
                                                                                                float rad, float std, float numiter, float thresh)
{
    vcl_vector<double>  newweights(points_list.size(),0.0);
    supportpoints.clear();
    bool iscocirc=true;

    parameters()->get_value("-iscocirc",iscocirc);

    //: initializing the weights to 1
    for(unsigned i=0;i<points_list.size();i++)
        points_list[i]->set_weight(1.0);
    for(unsigned itern=0;itern<numiter;itern++)
    {
        double maxweight=0.0;
        for(unsigned i=0;i<points_list.size();i++)
        {
            double weightik=0.0;
            double x0=points_list[i]->x();
            double y0=points_list[i]->y();
            double theta0=points_list[i]->tangent();
            double w0=points_list[i]->weight();
            for(unsigned k=0;k<points_list.size();k++)
            {
                if(i!=k )
                {
                    double x1=points_list[k]->x();
                    double y1=points_list[k]->y();
                    double theta1=points_list[k]->tangent();
                    if(vcl_fabs(x0-x1)<rad && vcl_fabs(y0-y1)<rad  )
                    {   
                        weightik+=points_list[k]->weight()*compute_support(x0,y0,theta0,x1,y1,theta1,std);
                    }
                }

            }
            if(weightik>maxweight)
                maxweight=weightik;
            newweights[i]=weightik;
        }

        for(int i=0;i<points_list.size();i++)
        {
            //vcl_cout<<    newweights[i]/maxweight<<"\t";
            points_list[i]->set_weight(newweights[i]/maxweight);
        }
    }
    vcl_vector<vsol_spatial_object_2d_sptr> pointssubppixel;

    for(int i=0;i<static_cast<int>(points_list.size());i++)
    {
        double w=points_list[i]->weight();
        if(w>thresh)
        {
            supportpoints.push_back(points_list[i]);
            vsol_point_2d_sptr pt=new vsol_point_2d(points_list[i]->x(),points_list[i]->y());
            double angle=points_list[i]->tangent()*vnl_math::pi/180;

            vgl_point_2d<double> p0(pt->x()+0.5*vcl_cos(angle)*w,pt->y()-0.5*vcl_sin(angle)*w);
            vgl_point_2d<double> p1(pt->x()-0.5*vcl_cos(angle)*w,pt->y()+0.5*vcl_sin(angle)*w);
            vgl_line_segment_2d<double> line(p0,p1);
            vsol_line_2d_sptr linevsol=new vsol_line_2d(line);
            pointssubppixel.push_back(pt->cast_to_spatial_object());
            pointssubppixel.push_back(linevsol->cast_to_spatial_object());
        }
    }
    return pointssubppixel;
}







vcl_vector<dbrl_id_point_2d_sptr> dbrl_superresolution_multiple_objects::get_edges_from_observ(dbinfo_observation_sptr obs)
{


    vcl_vector<dbrl_id_point_2d_sptr> pointedges;
    return pointedges;
}

vcl_vector<dbrl_id_point_2d_sptr> dbrl_superresolution_multiple_objects::get_foreground_edges_from_observ(dbinfo_observation_sptr obs,
                                                                                                          dbbgm_distribution_image<float> * model)
{
    vcl_vector<dbrl_id_point_2d_sptr> fgedges;
    vcl_vector<dbrl_id_point_2d_sptr> pointedges;
    if(!obs.ptr())
        return fgedges;

    vil_image_resource_sptr clip_im=obs->obs_snippet();
    vsol_polygon_2d_sptr poly=obs->geometry()->poly(0);
    vsol_polygon_2d_sptr dilated_poly=dilate_poly(poly,3.0);

    vgl_polygon<double> vgp = bsol_algs::vgl_from_poly(dilated_poly);

    if(!clip_im.ptr())
        return fgedges;

    sdet_nonmax_suppression_params nonmax_params(10);
    vil_image_view<double> gradx,grady;
    vil_sobel_3x3<unsigned char,double>(brip_vil_float_ops::convert_to_byte(clip_im),gradx,grady);
    sdet_nonmax_suppression edet(nonmax_params,gradx,grady);
    edet.apply();

    vcl_vector<vsol_point_2d_sptr> points=edet.get_points();
    vcl_vector< vgl_vector_2d<double> >tangents=edet.get_directions();

    brip_roi_sptr exroi=obs->ex_roi();

    vil_image_view<float> masked(model->ni(),model->nj(),1);
    masked.fill(-200);

    static float dist=0.0;
    static float minweightthresh=0.0;
    static int rad=0.0;

    parameters()->get_value("-dist",dist);
    parameters()->get_value("-minweightthresh",minweightthresh);
    parameters()->get_value("-rad",rad);

    //while (edges.size() > 0)
    //    {
    //    osl_edgel_chain * ec=(osl_edgel_chain *)edges.front();
    //    for(unsigned i=0;i<ec->size();i++)
    //        {
    //        masked(exroi->cmin(0)+(unsigned int)vcl_floor(ec->GetY(i)),exroi->rmin(0)+(unsigned int)vcl_floor(ec->GetX(i)))=ec->GetTheta(i)+180;
    //        dbrl_id_point_2d_sptr point=new dbrl_id_point_2d(exroi->cmin(0)+ec->GetY(i),exroi->rmin(0)+ec->GetX(i),ec->GetTheta(i)+180,pointedges.size()+1);
    //        if(vgp.contains(point->x(),point->y()))
    //            pointedges.push_back(point);

    //        }
    //    edges.pop_front();
    //    }

    for(unsigned i=0;i<points.size();i++)
    {
        double dir=vcl_atan2(tangents[i].y(),tangents[i].x());//*vnl_math::pi/180+180;
        masked(exroi->cmin(0)+(unsigned int)vcl_floor(points[i]->x()),exroi->rmin(0)+(unsigned int)vcl_floor(points[i]->y()))=dir;
        dbrl_id_point_2d_sptr point=new dbrl_id_point_2d(exroi->cmin(0)+points[i]->x(),exroi->rmin(0)+points[i]->y(),dir,pointedges.size()+1);
        if(vgp.contains(point->x(),point->y()))
            pointedges.push_back(point);
    }
    dbsta_top_weight_detector<float> fgdetector(new dbsta_g_mdist_detector<float>(dist), minweightthresh);
    vil_image_view<bool> fg(model->ni(),model->nj(),1);
    fg.fill(false);
    detect<float>(*model,masked,fgdetector,vnl_vector_fixed<float,1>(-10.0f),rad,fg);

    vil_image_view<unsigned char> im=brip_vil_float_ops::convert_to_byte(masked);
    vil_image_view<unsigned char> im1(fg.ni(),fg.nj());
    for(unsigned i=0;i<fg.ni();i++)
        for(unsigned j=0;j<fg.nj();j++)
            if(fg(i,j))
                im1(i,j)=255;
            else
                im1(i,j)=0;
    //: retaining the edges which are foreground
    for(int i=0;i<pointedges.size();i++)
    {
        if(fg(vcl_floor(pointedges[i]->x()),vcl_floor(pointedges[i]->y())))
            fgedges.push_back(pointedges[i]);
    }

    return fgedges;

}







vcl_vector<dbrl_feature_sptr> dbrl_superresolution_multiple_objects::get_neighbor_edges(vcl_vector<dbdet_edgel* > all_edgels)
{
    
    double xmax=0.0;
    double ymax=0.0;
    double xmin=1e5;
    double ymin=1e5;
    double nrad=2;
    double dt=30;
    double dx=0.3;

    for(unsigned i=0;i<all_edgels.size();i++)
    {
        if(all_edgels[i]->pt.x()>xmax)
            xmax=all_edgels[i]->pt.x();
        if(all_edgels[i]->pt.y()>ymax)
            ymax=all_edgels[i]->pt.y();
        if(all_edgels[i]->pt.x()<xmin)
            xmin=all_edgels[i]->pt.x();
        if(all_edgels[i]->pt.y()<ymin)
            ymin=all_edgels[i]->pt.y();

    }
    //vcl_vector<dbdet_edgel* > centered_edgels;
    //for(unsigned i=0;i<all_edgels.size();i++)
    //{
    //    vgl_point_2d<double> temp=all_edgels[i]->pt;
    //    //temp.x()-=xmin;
    //    //temp.y()-=ymin;
    //    dbdet_edgel * e=new dbdet_edgel(temp,all_edgels[i]->tangent);
    //    centered_edgels.push_back(e);
    //}
    dbdet_edgemap_sptr edgemap=new dbdet_edgemap(vcl_ceil(ymax), vcl_ceil(xmax),all_edgels);
    typedef dbdet_sel<dbdet_ES_curve_model> dbdet_sel_ES;
    dbdet_sel_sptr edge_linker=new dbdet_sel_ES(edgemap, nrad, dt*vnl_math::pi/180, dx);

    //build the edgel neighborhood first
    //edge_linker->build_edgel_neighborhoods(centered_edgels);

    //perform local edgel grouping

    unsigned max_size_to_group=7;
    edge_linker->build_curvelets_greedy(max_size_to_group);

    vcl_vector<dbdet_edgel*> edgels=edge_linker->get_edgels();
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
        pt= new dbrl_feature_point_tangent_curvature_groupings(edgels[i]->pt.x()/*+xmin*/,edgels[i]->pt.y()/*+ymin*/,
                                                               edgels[i]->tangent,neighbors_id_vec);
        pt->id= edgels[i]->id-1;
        edges.push_back(pt);


    }

    return edges;
}

vcl_vector<dbrl_feature_sptr> dbrl_superresolution_multiple_objects::get_curvature_edges(vcl_vector<dbdet_edgel* > all_edgels)
{
    
    double xmax=0.0;
    double ymax=0.0;
    double xmin=1e5;
    double ymin=1e5;
    double nrad=2;
    double dt=30;
    double dx=0.3;

    for(unsigned i=0;i<all_edgels.size();i++)
    {
        if(all_edgels[i]->pt.x()>xmax)
            xmax=all_edgels[i]->pt.x();
        if(all_edgels[i]->pt.y()>ymax)
            ymax=all_edgels[i]->pt.y();
        if(all_edgels[i]->pt.x()<xmin)
            xmin=all_edgels[i]->pt.x();
        if(all_edgels[i]->pt.y()<ymin)
            ymin=all_edgels[i]->pt.y();

    }
    vcl_vector<dbdet_edgel* > centered_edgels;
    for(unsigned i=0;i<all_edgels.size();i++)
    {
        vgl_point_2d<double> temp=all_edgels[i]->pt;
        temp.x()-=xmin;
        temp.y()-=ymin;
        dbdet_edgel * e=new dbdet_edgel(temp,all_edgels[i]->tangent);
        centered_edgels.push_back(e);
    }

    dbdet_edgemap_sptr edgemap=new dbdet_edgemap(vcl_ceil(ymax-ymin), vcl_ceil(xmax-xmin),centered_edgels);

    typedef dbdet_sel<dbdet_ES_curve_model> dbdet_sel_ES;
    dbdet_sel_sptr edge_linker=new dbdet_sel_ES(edgemap, nrad, dt*vnl_math::pi/180, dx);

    //perform local edgel grouping
    unsigned max_size_to_group=7;

    edge_linker->build_curvelets_greedy(max_size_to_group);
    //dbdet_sel_storage_sptr output_sel = dbdet_sel_storage_new();

    vcl_vector<dbdet_edgel*> edgels=edge_linker->get_edgels();
    vcl_vector<dbrl_feature_sptr> edges;
    for(unsigned i=0;i<edgels.size();i++)
    {
        dbrl_feature_point_tangent_curvature * pt;
        if(edgels[i]->curvelets.size()==1)
        {
            curvelet_list_iter cv_it = edgels[i]->curvelets.begin();
            for ( ; cv_it!=edgels[i]->curvelets.end(); cv_it++)
            {
                dbdet_curvelet* cvlet = (*cv_it);
                if(cvlet->edgel_chain.size()>2)
                {
                    if(dbdet_ES_curve_model * esmodel
                        =dynamic_cast<dbdet_ES_curve_model *>((edgels[i]->curvelets).front()->curve_model))
                    {
                        pt=new dbrl_feature_point_tangent_curvature(esmodel->pt.x(),esmodel->pt.y(),esmodel->theta);
                        pt->set_k(esmodel->k);
                        pt->compute_arc(1.0);
                    }
                }
            }
        }
        else
        {
             pt=new dbrl_feature_point_tangent_curvature(edgels[i]->pt.x(),edgels[i]->pt.y(),edgels[i]->tangent);
        }
        edges.push_back(pt);
    }
    return edges;
}




vcl_vector<dbdet_edgel*> dbrl_superresolution_multiple_objects::fg_detect(vcl_vector<dbdet_edgel*> edges, dbbgm_image_base * model)
{
    float minweightthresh = 0.2f, gdist = 0.0f;
    static int rad=2;
    parameters()->get_value( "-dist" ,   gdist);
    parameters()->get_value( "-minweightthresh" ,   minweightthresh);
    parameters()->get_value( "-rad" ,   rad);

    typedef dbsta2_num_obs<dbsta2_gauss_f1> gauss_type;
    typedef dbsta2_num_obs<dbsta2_mixture<gauss_type> > mix_gauss_type;

    typedef dbsta2_g_mdist_detector<dbsta2_gauss_f1> detector_type;
    typedef dbsta2_top_weight_detector<mix_gauss_type,detector_type> weight_detector_type;
    typedef dbsta2_mix_any_less_index_detector<mix_gauss_type,detector_type> all_weight_detector_type;

    dbbgm_image_of<mix_gauss_type>* emodel = dynamic_cast<dbbgm_image_of<mix_gauss_type>*>(model);
    dbsta2_g_mdist_detector<dbsta2_gauss_f1> det(gdist);
    dbsta2_top_weight_detector<mix_gauss_type,detector_type> detector_top(det, minweightthresh);
    
    vcl_vector<dbdet_edgel*> fgedges;

    int ni=emodel->ni();
    int nj=emodel->nj();
    for(unsigned i=0;i<edges.size();i++)
    {
        vgl_point_2d<double> pt=edges[i]->pt;
        double angle=edges[i]->tangent;

        bool flag=false;
        for(int m=-rad;m<=rad;m++)
        {
            for(int n=-rad;n<=rad;n++)
            {
                int m_index=vcl_floor(pt.x())+m;
                int n_index=vcl_floor(pt.y())+n;

                if(m_index>=0 && n_index>=0 && m_index<ni && n_index<nj)
                {
                    mix_gauss_type * dist=&(*emodel)(m_index,n_index);
                    bool isbg;
                    if(detector_top(*dist, angle,isbg))
                    {
                        if(isbg)
                            flag=isbg;
                    }
                }

            }
        }
        if(!flag)
            fgedges.push_back(new dbdet_edgel(pt,angle));
    }
    return fgedges;
}

vcl_vector<dbdet_edgel*> 
dbrl_superresolution_multiple_objects::edge_detector(vil_image_view<unsigned char> imgview)
{
    float sigma=1.0f;
    float thresh=5;
    int type=0;
    parameters()->get_value( "-esigma" ,   sigma);
    parameters()->get_value( "-ethresh" ,   thresh);
    parameters()->get_value( "-etype" ,   type);

    vcl_vector<dbdet_edgel*> edges;

    if(type==0)
    {
        sdet_nonmax_suppression_params nonmax_params(thresh,0);
        vil_image_view<double> gradx,grady,grad_mag;
        vil_sobel_3x3<unsigned char,double>(imgview,gradx,grady);

        grad_mag.set_size(imgview.ni(), imgview.nj());
        double *g_mag = grad_mag.top_left_ptr();
        double *gx  =  gradx.top_left_ptr();
        double *gy  =  grady.top_left_ptr();
        //compute the gradient magnitude
        for(unsigned long i=0; i<grad_mag.size(); i++)
            g_mag[i] = vcl_sqrt(gx[i]*gx[i] + gy[i]*gy[i]);

        vcl_vector<vgl_point_2d<double> > loc;
        vcl_vector<double> orientation, mag;

        dbdet_nms NMS(dbdet_nms_params(thresh, (dbdet_nms_params::PFIT_TYPE)0), gradx, grady, grad_mag);
        NMS.apply(true, loc, orientation, mag);
        for (unsigned i=0; i<loc.size(); i++){
            vgl_point_2d<double> pt(loc[i].x(), loc[i].y());
            edges.push_back(new dbdet_edgel(pt, orientation[i], mag[i]));
        }
    }
    else
    {
        dbdet_edgemap_sptr edge_map = dbdet_detect_third_order_edges(imgview, sigma, thresh, 1);
        for(dbdet_edgemap_iter iter=edge_map->edge_cells.begin();iter!=edge_map->edge_cells.end();iter++)
        {
            for(unsigned j=0;j<iter->size();j++)
                edges.push_back(new dbdet_edgel((*iter)[j]->pt,(*iter)[j]->tangent));
        }
    }


    return edges;
}







void
dbrl_superresolution_multiple_objects::super_resolute(vcl_map<dbrl_feature_sptr,unsigned char> fmap,
                                                      dbinfo_observation_sptr obs,vcl_string superimgname)
{
    vil_image_resource_sptr img=obs->obs_snippet();
    //vil_image_view<double> superimg(img->ni(),img->nj(),1);

    int minx=obs->ex_roi()->cmin(0);
    int maxx=obs->ex_roi()->cmax(0);
    int miny=obs->ex_roi()->rmin(0);
    int maxy=obs->ex_roi()->rmax(0);

    vcl_map<dbrl_feature_sptr,unsigned char>::iterator iter;
    vcl_map<dbrl_feature_sptr,unsigned char> filtered_samples;
    for(iter=fmap.begin();iter!=fmap.end();iter++)
    {
        if( dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(iter->first.ptr()))
        {
            if(pt->location()[0]>=minx && pt->location()[0]<=maxx &&
                pt->location()[1]>=miny && pt->location()[1]<=maxy)
            {
                filtered_samples[iter->first]=iter->second;
            }
        }
    }
    vnl_matrix<double> pts(filtered_samples.size(),2);
    vnl_vector<double> zs(filtered_samples.size());
    unsigned i=0;
    for(iter=filtered_samples.begin();iter!=filtered_samples.end();iter++)
    {
        if( dbrl_feature_point* pt=dynamic_cast<dbrl_feature_point*>(iter->first.ptr()))
        {
            pts(i,0)=pt->location()[0];
            pts(i,1)=pt->location()[1];
            zs(i)=(double)iter->second;
            i++;
        }
    }
    dbrl_estimator_cubic_patch * cpatch=new dbrl_estimator_cubic_patch();
    vcl_vector<dbrl_clough_tocher_patch> patches=cpatch->estimate_cubic(pts,zs);

    vil_image_view<unsigned char> superimg(img->ni()*4,img->nj()*4);

    for(double is=minx;is<maxx;)
    {
        for(double js=miny;js<maxy;)
        {
            vgl_point_2d<double> p(is,js);
            for(unsigned i=0;i<patches.size();i++)
            {   
                if(patches[i].intriangle(p))
                {
                    double newz=patches[i].interpolate(p);
                    unsigned char intensity=(unsigned char)vcl_floor(newz+0.5);
                    superimg(vcl_floor((is-minx)*4),vcl_floor((js-miny)*4))=intensity;
                    continue;
                }
            }
            js+=0.25;
        }
        is+=0.25;
    }
    vil_save(superimg,superimgname.c_str());
}


