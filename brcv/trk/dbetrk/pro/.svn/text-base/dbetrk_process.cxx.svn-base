#include <dbetrk/pro/dbetrk_process.h>
#include <vdgl/vdgl_digital_curve_sptr.h>
#include <vdgl/vdgl_interpolator_sptr.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <bseg/brip/brip_vil_float_ops.h>
#include <vtol/vtol_edge_2d.h>
#include <dbetrk/pro/dbetrk_storage_sptr.h>
#include <dbetrk/pro/dbetrk_storage.h>
#include <dbetrk/dbetrk_edge.h>
#include <vnl/vnl_math.h>
dbetrk_process::dbetrk_process() : bpro1_process(),dp()
{
  if( !parameters()->add( "Gaussian sigma" ,            "-ssigma" ,              (float)dp.smooth ) ||
      !parameters()->add( "Noise Threshold" ,           "-snoisethresh" ,        (float)dp.noise_multiplier ) ||
      !parameters()->add( "Automatic Threshold" ,       "-sauto_thresh" ,        (bool)dp.automatic_threshold ) ||
      !parameters()->add( "Agressive Closure" ,         "-sagressive_closure" ,  (int)dp.aggressive_junction_closure ) ||
      !parameters()->add( "Compute Junctions" ,         "-scompute_juncts" ,     (bool)dp.junctionp )||
      !parameters()->add( "Border" ,                    "-sborder" ,             dp.borderp=false )||
      !parameters()->add( "length of curves" ,          "-elenofcurves" ,       (int)10 ) ||
      !parameters()->add( "Motion" ,                    "-emotion" ,            (int)20 ) ||
      !parameters()->add( "Points filename" ,           "-efile" ,            (vcl_string)"d:\\data\\truck\\truck.tmp" ) ||
      !parameters()->add( "All edges" ,                    "-eall" ,            bool(false) ) ||
      !parameters()->add( "motion in x" ,                    "-ex" ,            (float)10 ) ||
      !parameters()->add( "motion in y" ,                    "-ey" ,            (float)10 ) ||
      !parameters()->add( "rotation theta ",                    "-et" ,            (float) vnl_math::pi/4 ) ||
      !parameters()->add( "particles" ,                    "-enum" ,            (int)100 ) ||
      !parameters()->add( "weight1" ,                    "-ew1" ,            (float)1.0 ) ||
            !parameters()->add( "weight2" ,                    "-ew2" ,            (float)1.0 ) 


      )


    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
  else
  {
          bg=new dbetrk_track();
  }
 
}



dbetrk_process::~dbetrk_process()
{
 if(bg)
     free(bg);
}


//: Clone the process
bpro1_process*
dbetrk_process::clone() const
{
  return new dbetrk_process(*this);
}



vcl_string
dbetrk_process::name()
{
  return "Edge Tracking";
}


vcl_vector< vcl_string > dbetrk_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "image" );
  return to_return;
}


vcl_vector< vcl_string > dbetrk_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "edgetrk" );
  return to_return;
}
//: Returns the number of input frames to this process
int
dbetrk_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbetrk_process::output_frames()
{
  return 1;
}



bool
dbetrk_process::execute()
{
  if ( input_data_.size() != 1 ){
    vcl_cout << "In dbetrk_process::execute() - not exactly one"
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
  parameters()->get_value( "-sauto_thresh" , dp.automatic_threshold );
  parameters()->get_value( "-sagressive_closure" , dp.aggressive_junction_closure);
  parameters()->get_value( "-scompute_juncts" , dp.junctionp );    
  parameters()->get_value( "-sborder" , dp.borderp );    
  
  
  static int min_length_of_curves=40;
  static int motion=20;
  static vcl_string filename="";
  static bool all=false;

  parameters()->get_value( "-elenofcurves" , min_length_of_curves ); 
  parameters()->get_value( "-emotion" , motion ); 
  parameters()->get_value( "-efile" , filename ); 
  parameters()->get_value( "-eall" , all ); 

    
  sdet_detector detector(dp);
  detector.SetImage(img);
  detector.DoContour();

  vcl_vector<vtol_edge_2d_sptr> * edges = detector.GetEdges();
  vil_image_view<float> float_image_view= brip_vil_float_ops::convert_to_float(image_view);

  //: separating out the three planes of an image

  vil_image_view<float> p0;
  vil_image_view<float> p1;
  vil_image_view<float> p2;

  if( image_view.nplanes() == 3 ) 
  {
      brip_vil_float_ops::convert_to_IHS(image_view,p0,p1,p2);
    //  bg->plane0.push_back(p0);
   //   bg->plane1.push_back(p1);
   //   bg->plane2.push_back(p2);
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
  //  bg->plane0.push_back(p0);
  //  bg->plane1.push_back(p1);
  //  bg->plane2.push_back(p2);
  }


  if (!edges)
    return false;
  
  // pass the edges
  vsol_curve_2d_sptr c;
  vdgl_digital_curve_sptr dc;
  vdgl_interpolator_sptr interp;
  vdgl_edgel_chain_sptr  ec;
  vcl_vector<vtol_edge_2d_sptr> ecl;
  bg->input_curves_.clear();
  
  for (unsigned int i=0; i<edges->size(); i++)
    {
      c  = (*edges)[i]->curve();
      dc = c->cast_to_vdgl_digital_curve();
      if (dc->length()>min_length_of_curves)
        bg->input_curves_.push_back((*edges)[i]);
    }
  
    
//    bg->input_curves_.push_back(ecl);
    vcl_cout<<"\n frame no "<<bg->input_curves_.size()-1;
    bg->setmotion(motion);
    bg->fill_dbetrk_edges(bg->frame_);
    vcl_cout<<"\n nodes info computed";

    parameters()->get_value( "-ex" , bg->std_x ); 
    parameters()->get_value( "-ey" , bg->std_y ); 
    parameters()->get_value( "-et" , bg->std_theta ); 
    parameters()->get_value( "-enum" , bg->samplenum ); 
    parameters()->get_value( "-ew1" , bg->weight1 ); 
    parameters()->get_value( "-ew2" , bg->weight2 ); 



    if(bg->frame_>0)
        bg->compute_edges(bg->frame_-1,bg->frame_);
    else if(bg->frame_==0)
    {
        vcl_vector<dbetrk_edge_sptr> nodes;
        vsl_b_ifstream ifs(filename.c_str());
        if(!ifs)
        {vcl_cout <<"\n error opening the file ";}
        else
        {
         short ver;vsl_b_read(ifs, ver);
         int n;vsl_b_read(ifs, n);
         for(int i=0;i<n;i++)
         {
             dbetrk_edge_sptr e=new dbetrk_edge();
             vsl_b_read(ifs, *e);
             e->frame_=0;
             nodes.push_back(e);

         }
        }
        bg->tracked_dbetrk_edges.push_back(nodes);
       
    }

    vcl_cout<<"\n edges info computed";

  
    dbetrk_storage_sptr output_edges=dbetrk_storage_new();
    output_data_[0].push_back(output_edges);
    if(!all)
        output_edges->set_dbetrk_edges((bg->tracked_dbetrk_edges[bg->frame_]));
    else
        output_edges->set_dbetrk_edges((bg->dbetrk_edges));


    bg->frame_++;

    vcl_cout<<"\n  hi";
  clear_input();
  return true;
}


/*************************************************************************
 * Function Name: vidpro1_ctrk_process::finish
 * Parameters: 
 * Returns: bool
 * Effects: 
 *************************************************************************/
bool
dbetrk_process::finish()
{
  return true;
}


