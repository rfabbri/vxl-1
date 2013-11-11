#include "dbctrk_process.h"
#include<vdgl/vdgl_digital_curve_sptr.h>
#include<vdgl/vdgl_interpolator_sptr.h>
#include<vdgl/vdgl_digital_curve.h>
#include<bseg/brip/brip_vil_float_ops.h>
#include<vsol/vsol_polyline_2d.h>
#include<vsol/vsol_curve_2d.h>
#include<vsol/vsol_curve_2d_sptr.h>
#include<vdgl/vdgl_digital_curve_sptr.h>
#include<vdgl/vdgl_interpolator_sptr.h>
#include<vdgl/vdgl_interpolator.h>
#include<vdgl/vdgl_interpolator_linear.h>
#include<vdgl/vdgl_digital_curve.h>
#include<vsol/vsol_polyline_2d_sptr.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage.h>

#include <dbdet/sel/dbdet_curve_fragment_graph.h>
#include <dbdet/sel/dbdet_edgel.h>

#include <dbdet/pro/dbdet_sel_storage_sptr.h>
#include <dbdet/pro/dbdet_sel_storage.h>
dbctrk_process::dbctrk_process() : bpro1_process(),dp(),tp(),tracker_(NULL)
{
    if( //!parameters()->add( "Gaussian sigma" ,            "-ssigma" ,              (float)dp.smooth ) ||
        !parameters()->add( "Is SEl" ,           "-ssel" ,        (bool)false ) ||
        //!parameters()->add( "Is Vsol" ,           "-svsol" ,        (bool)false ) ||
        !parameters()->add( "Noise Threshold" ,           "-snoisethresh" ,        (float)2.5 ) ||
        //!parameters()->add( "Automatic Threshold" ,       "-sauto_thresh" ,        (bool)dp.automatic_threshold ) ||
        //!parameters()->add( "Agressive Closure" ,         "-sagressive_closure" ,  false ) ||
        //!parameters()->add( "Compute Junctions" ,         "-scompute_juncts" ,     (bool)dp.junctionp )||
        //!parameters()->add( "Border" ,                    "-sborder" ,             dp.borderp=false )||      
        !parameters()->add( "Matching" ,                  "-ctmatching" ,          (bool)true) ||
        //!parameters()->add( "Elength on/off" ,            "-ctElength" ,           (int)1 )||
        //!parameters()->add( "Ebending on/off" ,           "-ctEbending" ,          (int)1 )||
        //!parameters()->add( "Eepipole on/off" ,           "-ctEepipole" ,          (int)1 )||    
        //!parameters()->add( "transitive closure" ,        "-cttransitiveclosure" , false) ||
        !parameters()->add( "Clustering" ,                "-ctclustering" ,        true) ||
        !parameters()->add( "No of clusters" ,            "-ctnoofclusters" ,      (int)2) ||
        !parameters()->add( "Inter-cluster translation" , "-ctictranslation" ,     (float)6.0) ||
        !parameters()->add( "Minimum cost" ,              "-ctmincost" ,           (float)5.0) ||
        !parameters()->add( "length of curves" ,          "-ctlenofcurves" ,       (int)20 ) ||
        !parameters()->add( "Estimated Motion" ,          "-ctmotion" ,            (int)10) ||
        //!parameters()->add( "Epipole" ,                   "-ctEpipole" ,           false) ||
        //!parameters()->add( "X Epipole" ,                 "-ctxEpipole" ,          0.0f) ||
        //!parameters()->add( "Y Epipole" ,                 "-ctyEpipole" ,          0.0f) ||
        !parameters()->add( "Lab or IHS" ,                 "-cIHS" ,          true )||
        //!parameters()->add( "alpha" ,                 "-alpha" ,          (int)1 )||
        //!parameters()->add( "beta" ,                  "-beta" ,           (int)1 )||
        //!parameters()->add( "gamma" ,                 "-gamma" ,          (int)0 )||
        //!parameters()->add( "delta" ,                   "-delta" ,          (int)0 )||
        //!parameters()->add( "eta" ,                   "-eta" ,          (int)0 )||
        //!parameters()->add( "zeta" ,                   "-zeta" ,          (int)0 )||
        //!parameters()->add( "theta" ,                   "-theta" ,          (int)0 )||
        !parameters()->add( "No of bins for hue" ,                   "-huebins" ,          (int)6 )||
        !parameters()->add( "R1 for saturation" ,                   "-satr1" ,          (float)0.4 )||
        !parameters()->add( "V1 for value" ,                   "-valuev1" ,          (float)0.5 )||
        !parameters()->add( "threshold for IHS pruning" ,                   "-tauIHS" ,          (float)0.45 )||
        !parameters()->add( "Pruning by Intensity" ,                   "-isintensity" ,          (bool)true )
        )

    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
    else
    {
        tracker_=new dbctrk_curve_tracking(tp);
    }

}


/*************************************************************************
* Function Name: dbctrk_process::~dbctrk_process
* Parameters: 
* Effects: 
*************************************************************************/

dbctrk_process::~dbctrk_process()
{
    if(tracker_)
        delete(tracker_);
}


//: Clone the process
bpro1_process*
dbctrk_process::clone() const
{
    return new dbctrk_process(*this);
}


/*************************************************************************
* Function Name: dbctrk_process::name
* Parameters: 
* Returns: vcl_string
* Effects: 
*************************************************************************/
vcl_string
dbctrk_process::name()
{
    return "Curve Tracking";
}


/*************************************************************************
* Function Name: ddbctrk_process::get_input_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > dbctrk_process::get_input_type()
{
    vcl_vector< vcl_string > to_return;
    bool issel=false;
    parameters()->get_value( "-ssel" ,  issel);
    bool isvsol=false;
    parameters()->get_value( "-svsol" ,  isvsol);

    to_return.push_back( "image" );
    if(issel)
        to_return.push_back( "sel" );
    //if(isvsol)
    //    to_return.push_back( "vsol2D" );

    return to_return;
}


/*************************************************************************
* Function Name: dbctrk_process::get_output_type
* Parameters: 
* Returns: vcl_vector< vcl_string >
* Effects: 
*************************************************************************/
vcl_vector< vcl_string > dbctrk_process::get_output_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back( "ctrk" );
        to_return.push_back( "vsol2D" );

    return to_return;
}
//: Returns the number of input frames to this process
int
dbctrk_process::input_frames()
{
    return 1;
}


//: Returns the number of output frames from this process
int
dbctrk_process::output_frames()
{
    return 1;
}


/*************************************************************************
* Function Name: ddbctrk_process::execute
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
dbctrk_process::execute()
{
    if ( input_data_.size() != 1 ){
        vcl_cout << "In dbctrk_process::execute() - not exactly one"
            << " input image \n";
        return false;
    }
    clear_output();

    // get image from the storage class

    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][0]);

    vil_image_resource_sptr image_sptr = frame_image->get_image();

    vil_image_view< unsigned char > image_view = image_sptr->get_view(0, image_sptr->ni(), 0, image_sptr->nj() );
    vil_image_view< unsigned char > greyscale_view;

    if( image_view.nplanes() == 3 ) {
        vil_convert_planes_to_grey( image_view , greyscale_view );
    } 
    else if ( image_view.nplanes() == 1 ) {
        greyscale_view = image_view;
    }
    else
    {
        vcl_cerr << "Returning false. nplanes(): " << image_view.nplanes() << vcl_endl;
        return false;
    }

    vil1_memory_image_of< unsigned char > img = vil1_from_vil_image_view( greyscale_view );


    parameters()->get_value( "-ssigma" ,  dp.smooth);
    parameters()->get_value( "-snoisethresh" , dp.noise_multiplier );
    //  parameters()->get_value( "-sauto_thresh" , dp.automatic_threshold );
    //parameters()->get_value( "-sagressive_closure" , dp.agressive_junction_closure);
    dp.aggressive_junction_closure=0;
    dp.junctionp=false;
    dp.borderp=false;
    // parameters()->get_value( "-scompute_juncts" , dp.junctionp );    
    // parameters()->get_value( "-sborder" , dp.borderp );    
    static int motion;
    parameters()->get_value( "-ctmatching" ,tp.mp.matching_) ;
    parameters()->get_value( "-ctElength" , tp.mp.alpha_) ;
    parameters()->get_value( "-ctEbending" , tp.mp.beta_) ;
    parameters()->get_value( "-ctEepipole" , tp.mp.gamma_) ;
    parameters()->get_value( "-cttransitiveclosure" , tp.transitive_closure) ;
    parameters()->get_value( "-ctclustering" , tp.clustering_) ;
    parameters()->get_value( "-ctictranslation" , tp.cp.foreg_backg_threshold);
    parameters()->get_value( "-ctnoofclusters" , tp.cp.no_of_clusters);
    parameters()->get_value( "-ctmincost" , tp.cp.min_cost_threshold);
    parameters()->get_value( "-huebins" , tp.thetabins);
    parameters()->get_value( "-satr1" , tp.r1);
    parameters()->get_value( "-valuev1" , tp.v1);
    parameters()->get_value( "-tauihs" , tp.mp.tauihs_);
    tp.transitive_closure=false;
    //tp.mp.matching_=false;
    parameters()->get_value( "-ctmotion" , motion);
    parameters()->get_value( "-ctlenofcurves" , tp.min_length_of_curves);

    float xepipole=0,yepipole=0;
    parameters()->get_value( "-ctxEpipole"  , xepipole);
    parameters()->get_value( "-ctyEpipole" , yepipole);

    parameters()->get_value( "-cIHS" , tp.isIHS);
    parameters()->get_value( "-alpha" , tp.mp.alpha_);
    parameters()->get_value( "-beta" , tp.mp.beta_);
    parameters()->get_value( "-gamma" , tp.mp.gamma_);
    parameters()->get_value( "-delta" , tp.mp.delta_);
    parameters()->get_value( "-eta" , tp.mp.eta_);
    parameters()->get_value( "-zeta" , tp.mp.zeta_);
    parameters()->get_value( "-theta" , tp.mp.theta_);
    parameters()->get_value( "-tauIHS" , tp.mp.tauihs_);
    parameters()->get_value( "-isintensity" , tp.mp.isintensity_);


    tp.mp.e_.set(xepipole,yepipole);
    tp.mp.motion_in_pixels=motion;
        tracker_->set_tracking_params(tp);
    vil_image_view<float> float_image_view= brip_vil_float_ops::convert_to_float(image_view);

    //: separating out the three planes of an image

    vil_image_view<float> p0;
    vil_image_view<float> p1;
    vil_image_view<float> p2;

    if( image_view.nplanes() == 3 ) 
    {
        brip_vil_float_ops::convert_to_IHS(image_view,p0,p1,p2);
        tracker_->plane0.push_back(p0);
        tracker_->plane1.push_back(p1);
        tracker_->plane2.push_back(p2);
    }

    else if ( image_view.nplanes() == 1 ) {
        p0.set_size(image_view.ni(), image_view.nj(), 1);
        p1.set_size(image_view.ni(),image_view.nj(), 1);
        p2.set_size(image_view.ni(),image_view.nj(), 1);
        for (unsigned j = 0; j < image_view.nj(); ++j)
            for (unsigned i = 0; i < image_view.ni(); ++i)
            {
                p2(i,j)=p1(i,j)=p0(i,j)=(float)image_view(i,j);
            }
            tracker_->plane0.push_back(p0);
            tracker_->plane1.push_back(p1);
            tracker_->plane2.push_back(p2);
    }
    vcl_vector<vtol_edge_2d_sptr> ecl;
    ecl.clear();
    vcl_vector<vsol_spatial_object_2d_sptr> contours;

    bool issel=false;
    parameters()->get_value( "-ssel" ,  issel);

    if(issel)
    {
        dbdet_sel_storage_sptr sel_storage;
        sel_storage.vertical_cast(input_data_[0][1]);

        dbdet_curve_fragment_graph cfg=sel_storage->CFG() ;


        for(dbdet_edgel_chain_list_iter iter=cfg.frags.begin();iter!=cfg.frags.end();iter++)
        {
            vcl_deque<dbdet_edgel*>::iterator lit;
            vcl_vector<vdgl_edgel> edgel_chain;
            vcl_vector<vsol_point_2d_sptr> points;

            if ((*iter)->edgels.size()>tp.min_length_of_curves)
            {
                for(lit=(*iter)->edgels.begin();lit!=(*iter)->edgels.end();lit++)
                {

                    edgel_chain.push_back(vdgl_edgel((*lit)->pt.x(),(*lit)->pt.y(),(*lit)->strength,(*lit)->tangent));
                    points.push_back(new vsol_point_2d((*lit)->pt.x(),(*lit)->pt.y()));

                }            
                vsol_polyline_2d_sptr vdc= new vsol_polyline_2d(points);
                contours.push_back(vdc->cast_to_spatial_object());

                vdgl_interpolator_sptr vdinterpolator=new vdgl_interpolator_linear(new vdgl_edgel_chain(edgel_chain));
                vsol_curve_2d * dc=new vdgl_digital_curve(vdinterpolator);
                vtol_edge_2d_sptr e=new vtol_edge_2d(*dc);
                e->set_curve(*dc);
                ecl.push_back(e);

            }
        }

    }
   else{

    dp.borderp=false;

    sdet_detector detector(dp);
    detector.SetImage(img);
    detector.DoContour();
    vcl_vector<vtol_edge_2d_sptr> * edges = detector.GetEdges();



    if (!edges)
        return false;

    // pass the edges
    vsol_curve_2d_sptr c;
    vdgl_digital_curve_sptr dc;
    vdgl_interpolator_sptr interp;
    vdgl_edgel_chain_sptr  ec;

    for (unsigned int i=0; i<edges->size(); i++)
    {
        c  = (*edges)[i]->curve();
        dc = c->cast_to_vdgl_digital_curve();
        if (dc->length()>tp.min_length_of_curves)
            ecl.push_back((*edges)[i]);
    }
    for (unsigned int i=0; i<edges->size(); i++)
    {
        c  = (*edges)[i]->curve();
        ec=c->cast_to_vdgl_digital_curve()->get_interpolator()->get_edgel_chain();
        vcl_vector<vsol_point_2d_sptr> points;
        for(unsigned j=0;j<ec->size();j++)
        {
            points.push_back(new vsol_point_2d(ec->edgel(j).get_pt()));
        }
        vsol_polyline_2d_sptr dc= new vsol_polyline_2d(points);
        contours.push_back(dc->cast_to_spatial_object());

    }
    }
    // sending the input
    tracker_->input_curves_.push_back(ecl);
    // tracking

    int t = tracker_->input_curves_.size()-1;
    tracker_->track_frame(t);


    // storing the data structure
    dbctrk_storage_sptr output_tracks=dbctrk_storage_new();
    output_data_[0].push_back(output_tracks);
    output_tracks->set_tracked_curves(*(tracker_->get_output_curves(t)));

    vidpro1_vsol2D_storage_sptr out_vsol=vidpro1_vsol2D_storage_new();
    out_vsol->add_objects(contours);
    out_vsol->add_objects(contours, "");
    output_data_[0].push_back(out_vsol);
    clear_input();
    return true;
}


/*************************************************************************
* Function Name: dbctrk_process::finish
* Parameters: 
* Returns: bool
* Effects: 
*************************************************************************/
bool
dbctrk_process::finish()
{
    return true;
}


