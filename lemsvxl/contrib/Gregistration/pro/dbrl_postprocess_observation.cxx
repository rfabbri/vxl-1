
#include "dbrl_postprocess_observation.h"
#include<bpro1/bpro1_parameters.h>
#include<vsol/vsol_point_2d.h>
#include<vsol/vsol_line_2d.h>
#include<vsol/vsol_polyline_2d.h>
#include<vsol/vsol_polyline_2d_sptr.h>

#include<vidpro1/storage/vidpro1_image_storage_sptr.h>
#include<vidpro1/storage/vidpro1_image_storage.h>
#include<dbinfo/pro/dbinfo_track_storage.h>
#include<dbinfo/pro/dbinfo_track_storage_sptr.h>
#include<dbinfo/dbinfo_observation_sptr.h>
#include<dbinfo/dbinfo_observation.h>

#include <vil1/vil1_memory_image_of.h>
#include <vil1/vil1_vil.h>

#include<pro/dbrl_id_point_2d_storage.h>
#include<pro/dbrl_id_point_2d_storage_sptr.h>
#include <osl/osl_edge.h>
#include <osl/osl_canny_ox.h>
#include <osl/osl_canny_ox_params.h>
#include <osl/osl_edgel_chain.h>
#include<bseg/brip/brip_vil_float_ops.h>
#include <dbdet/pro/dbdet_sel_storage.h>
#include <bsol/bsol_algs.h>
#include <sdet/sdet_nonmax_suppression_params.h>
#include <sdet/sdet_nonmax_suppression.h>
#include <vil/algo/vil_sobel_3x3.h>
#include <vgl/algo/vgl_convex_hull_2d.h>

#include <dbdet/algo/dbdet_sel_base.h>
#include <dbdet/algo/dbdet_sel_sptr.h>
#include <dbdet/sel/dbdet_curve_model.h>
#include <dbdet/algo/dbdet_sel.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

//: Constructor
dbrl_postprocess_observation::dbrl_postprocess_observation(void): bpro1_process()
{
      if( //parameters related to the bucket construction
          !parameters()->add( "Compute Curvelets?" , "-compcurvelets" , false ) ||
            !parameters()->add( "Grid equals image size" , "-use_image_size" , true ) ||
      !parameters()->add( "    Grid #cols" , "-ncols" , 300 ) ||
      !parameters()->add( "    Grid #rows" , "-nrows" , 300 ) ||

      //grouping parameters
      !parameters()->add( "Position uncertainty" , "-dx" , 0.3 ) ||
      !parameters()->add( "Orientation uncertainty(Deg)" , "-dt" , 30.0 ) ||
      !parameters()->add( "Radius of Neighborhood" , "-nrad" , 3 ) ||
      //output type
      !parameters()->add( "Output image contours as vsol"   , "-output_vsol" , false ) ||
      !parameters()->add( "Minimum length of curves to keep" , "-min_size_to_keep", 3 ))
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }

}

//: Destructor
dbrl_postprocess_observation::~dbrl_postprocess_observation()
{

}


//: Return the name of this process
vcl_string
dbrl_postprocess_observation::name()
{
  return "Postprocess Observations";
}


//: Return the number of input frame for this process
int
dbrl_postprocess_observation::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbrl_postprocess_observation::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbrl_postprocess_observation::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "dbinfo_track_storage" );
  to_return.push_back( "image" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbrl_postprocess_observation::get_output_type()
{  
    vcl_vector<vcl_string > to_return;
    //output the sel storage class
    //bool output_vsol;
    //parameters()->get_value( "-output_vsol", output_vsol );

    //output the sel storage class
    bool compcurvelets=false;
    parameters()->get_value("-compcurvelets",compcurvelets);
    if(compcurvelets)
        to_return.push_back( "sel" );

    //if desired, output the vsol (but this takes too long)
    //if (output_vsol)
    //    to_return.push_back( "vsol2D" );

    return to_return;
}


//: Execute the process
bool
dbrl_postprocess_observation::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbrl_postprocess_observation::execute() - "
             << "not exactly two input images \n";
    return false;
  }
  clear_output();
  //: obtain the tracks 
  dbinfo_track_storage_sptr track_storage;
  track_storage.vertical_cast(input_data_[0][0]);

  vidpro1_image_storage_sptr image_storage;
  image_storage.vertical_cast(input_data_[0][1]);

  vcl_vector<dbinfo_observation_sptr> obs_per_frame;
  vil_image_resource_sptr image_sptr = image_storage->get_image();

  vcl_vector<dbinfo_track_sptr> tracks=track_storage->tracks();
  unsigned  frame_no=image_storage->frame();

  for(vcl_vector<dbinfo_track_sptr>::iterator trit = tracks.begin();
      trit != tracks.end(); trit++)
  {
      dbinfo_observation_sptr ref_obs=(*trit)->observ(frame_no);
      if(ref_obs.ptr())
      {
          
          ref_obs->set_margin(7);
          ref_obs->compute_roi();

          compute_observation_snippet(ref_obs.ptr(),image_sptr);
          obs_per_frame.push_back(ref_obs);
      }
  }
  bool compcurvelets=false;
  parameters()->get_value("-compcurvelets",compcurvelets);
  //if(compcurvelets)
  //    compute_curvelets_from_observation(obs_per_frame);
  return true;
}
//: Clone the process
bpro1_process*
dbrl_postprocess_observation::clone() const
{
  return new dbrl_postprocess_observation(*this);
}
    
bool
dbrl_postprocess_observation::finish()
{
  return true;
}



void dbrl_postprocess_observation::compute_clips_from_observ(dbinfo_observation_sptr obs,
                                                             vil_image_resource_sptr img)
    {
        if(obs.ptr())
             compute_observation_snippet(obs.ptr(),img);
    }



bool dbrl_postprocess_observation::compute_curvelets_from_observation(vcl_vector<dbinfo_observation_sptr> obs)
{
    
    
    static int nrad;
    static double dx;
    static double dt;
    static int ncols;
    static int nrows;
    bool use_image_size;
    bool output_vsol;
    int min_size_to_keep;
    parameters()->get_value( "-use_image_size", use_image_size);
    if (!use_image_size) {
        parameters()->get_value( "-nrows", nrows);
        parameters()->get_value( "-ncols", ncols);
    } 
    else {
        vcl_cout << "Using image size for the grid\n";
        // get image from the storage class
        vidpro1_image_storage_sptr frame_image;
        frame_image.vertical_cast(get_input(0)[1]);
        if (!frame_image) {
            vcl_cout << "Error: no image in input storages\n";
            return  false;
        }
        vil_image_resource_sptr image_sptr = frame_image->get_image();
        ncols = image_sptr->ni();
        nrows = image_sptr->nj();
        vcl_cout << "Nrows: " << nrows;
        vcl_cout << "  Ncols: " << ncols << vcl_endl;
    }

    //parameters()->get_value( "-nrad", nrad);
    //parameters()->get_value( "-dx", dx);
    //parameters()->get_value( "-dt", dt);
    //parameters()->get_value( "-output_vsol", output_vsol );
    //parameters()->get_value( "-min_size_to_keep", min_size_to_keep );
    vcl_vector<dbdet_edgel* > all_edgels;
    for(unsigned i=0;i<obs.size();i++)    
    {
        dbdet_sel_storage_sptr output_sel = dbdet_sel_storage_new();   
        vil_image_resource_sptr clip_im=obs[i]->obs_snippet();
        vsol_polygon_2d_sptr poly=obs[i]->geometry()->poly(0);
        vgl_polygon<double> vgp = bsol_algs::vgl_from_poly(poly);

        if(!clip_im.ptr())
            return false;

        sdet_nonmax_suppression_params nonmax_params(10);
        vil_image_view<double> gradx,grady;
        vil_sobel_3x3<unsigned char,double>(brip_vil_float_ops::convert_to_byte(clip_im),gradx,grady);
        sdet_nonmax_suppression edet(nonmax_params,gradx,grady);
        edet.apply();

        brip_roi_sptr exroi=obs[i]->ex_roi();

        //convert from vsol2D to edgels
        vcl_vector<vsol_line_2d_sptr> lines=edet.get_lines();
        for (unsigned int b = 0 ; b < lines.size() ; b++ ) {
            if( lines[b]->cast_to_curve()){
                //LINE
                if( lines[b]->cast_to_curve()->cast_to_line() ){
                    vsol_line_2d_sptr eline = lines[b]->cast_to_curve()->cast_to_line();
                    vgl_point_2d<double> spt(eline->p0()->x(), eline->p0()->y());
                    vgl_point_2d<double> ept(eline->p1()->x(), eline->p1()->y());
                    vgl_point_2d<double> pt(exroi->cmin(0)+eline->middle()->x(), exroi->rmin(0)+eline->middle()->y());
                    double tan = dbdet_vPointPoint(spt, ept);
                    all_edgels.push_back(new dbdet_edgel(pt, tan));
                }
            }
        }
    }
    //different types of linkers depending on the curve model
    //typedef dbdet_sel<dbdet_ES_curve_model> dbdet_sel_ES;
    //dbdet_sel_sptr edge_linker=new dbdet_sel_ES(nrows, ncols, nrad, dt*vnl_math::pi/180, dx);

    ////build the edgel neighborhood first
    //edge_linker->build_edgel_neighborhoods(all_edgels);

    ////perform local edgel grouping
    //edge_linker->build_curvelets_greedy();
    //  dbdet_sel_storage_sptr output_sel = dbdet_sel_storage_new();

    //output_sel->set_sel(edge_linker);
    //output_data_[0].push_back(output_sel);

    //if (output_vsol){
    //    vcl_vector< vsol_spatial_object_2d_sptr > image_curves;

    //    for (unsigned i=0; i<edge_linker->edgel_chains().size(); i++){
    //        //only keep the longer contours
    //        if (edge_linker->edgel_chains()[i]->edgels.size()>=min_size_to_keep){
    //            vcl_vector<vsol_point_2d_sptr> pts;
    //            for (unsigned j=0; j<edge_linker->edgel_chains()[i]->edgels.size(); j++)
    //                pts.push_back(new vsol_point_2d(edge_linker->edgel_chains()[i]->edgels[j]->pt));
    //            vsol_polyline_2d_sptr new_curve = new vsol_polyline_2d(pts);
    //            image_curves.push_back(new_curve->cast_to_spatial_object());
    //        }
    //    }

    //    // create the output storage class
    //    vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
    //    output_vsol->add_objects(image_curves, "contours");
    //    output_data_[0].push_back(output_vsol);
    //}

    return true;
}
