// This is pro/dbcri_whole_process.cxx

//:
// \file
#define DEBUG 0
#define WORLD_CAM true
#define FRAME_OFFSET 2;
//#define ERROR 1000

///  if all pts across frames are good use it or not (in cross ratio sense)
#define ANGLE_FLAG false 
///
#include "dbcri_whole_process.h"
//#include "dbecl_episeg_storage.h"

//: Constructor
dbcri_whole_process::dbcri_whole_process()
: bpro_process()
{

  if(!parameters()->add( "world camera <filename...>" , "-wc_filename", bpro_filepath("","*") )||
    !parameters()->add( "epipole from result file", "-epi_from_result_file",  (bool)false)||
    !parameters()->add( "use epipole from world camera?", "-epi_flag",  (bool) false)||
    !parameters()->add( "manual epipole?", "-man_epi_flag",  (bool) false)||
    !parameters()->add( "epi_x?", "-epi_x",  (float) 0.0f) ||
    !parameters()->add( "epi_y?", "-epi_y",  (float) 0.0f) ||
    !parameters()->add( "epi_search_range_wide? ", "-epi_search_range_wide",  (bool) false) ||
    //!parameters()->add( "epi_search_range_x", "-e_range_x",  (float) 200.0f) ||
    //!parameters()->add( "epi_search_range_y", "-e_range_y",  (float) 50.0f) ||
    //!parameters()->add( "epi_search_interval", "-epi_search_interval",  (float) 50.0f) ||
    !parameters()->add( "WC / con tangent distance ", "-epi_offset_flag",  (bool) false) ||

    !parameters()->add( "3d rec con only 0/near edge 2/both 1/all edge 3(con+near+messy)/?", "-recon_flag",  (int) 0)||
    !parameters()->add( "draw_only?", "-draw_flag",  (bool) false)||
    !parameters()->add( "use weight?", "-weight_flag",  (bool) true)||
    !parameters()->add( "use clustering?", "-cluster_flag",  (bool) true)||
    !parameters()->add( "histogram normalization?", "-histo_normal_flag",  (bool) false)||
    !parameters()->add( "wedge search margin?", "-MARGIN",  0.0f)||
    /*!parameters()->add( "random number threshold?", "-rand_th",  0.2f)||
    !parameters()->add( "frame Number?", "-N",  (int) 7)||
    !parameters()->add( "combination number?", "-Comb",  (int)15)||
    !parameters()->add( "bin minimum?", "-bin_min",  (int)5)||*/


    //!parameters()->add( "draw_only?", "-draw_flag",  (bool) false)||
    !parameters()->add( "use default world camera?", "-wc_flag",  (bool) false) ||
    !parameters()->add( "contour neighbor only?", "-edge_distance",  (bool) true) ||
    !parameters()->add( "contour distance?", "-edge_distance2",  (float) 10.0f) ||
     !parameters()->add( "ec size?", "-ec_size",  (float) 25.0f) ||
    !parameters()->add( "convex hull shift?", "-auto_shift",  (bool) false) ||
    !parameters()->add( "3d recon error tolerance", "-d3_error",  (float) 10.0f) // ||
    // //!parameters()->add( "Gaussian sigma" ,      "-ssigma" ,            (float)dp.smooth ) ||
    // // !parameters()->add( "Noise Threshold" ,     "-snoisethresh" ,      (float)dp.noise_multiplier ) //||
    //!parameters()->add( "Automatic Threshold" , "-sauto_thresh" ,      (bool)dp.automatic_threshold ) ||
    //!parameters()->add( "Agressive Closure" ,   "-sagressive_closure" ,(int)dp.aggressive_junction_closure ) ||
    //!parameters()->add( "Compute Junctions" ,   "-scompute_juncts" ,   (bool)dp.junctionp ) ||
    //!parameters()->add( "Include Borders" ,   "-sinclude_borders" ,   (bool)dp.borderp ) 
    ) 
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  } 
}


//: Copy Constructor
dbcri_whole_process::
dbcri_whole_process(const dbcri_whole_process& other)
: bpro_process(other)
{
}


//: Destructor
dbcri_whole_process::~dbcri_whole_process()
{
}


//: Clone the process
bpro_process* 
dbcri_whole_process::clone() const
{
  return new dbcri_whole_process(*this);
}


//: Return the name of the process
vcl_string
dbcri_whole_process::name()
{
  return "dbcri_whole";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbcri_whole_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;

  to_return.push_back( "vsol2D" );
  to_return.push_back( "image" );
  to_return.push_back( "vtol2D" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbcri_whole_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;

  to_return.push_back( "vtol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );

  return to_return;
}


//: Returns the number of input frames to this process
int
dbcri_whole_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbcri_whole_process::output_frames()
{
  //return nframes_;
  return 1;
}




//: Run the process on the current frame
bool
dbcri_whole_process::execute()
{
  if(0)
    for (int i=7;i<12;i++) {
      vcl_cout<<"--------N is: "<<i<<vcl_endl;
      genarate_table(i) ;

      vcl_cout<<"------------------------"<<vcl_endl;
    }
    if (0) {
      int frame_N;
      parameters()->get_value( "-N" ,   frame_N);

      genarate_table(frame_N) ;
      exit(1);
    }

    bpro_filepath wc_path;
    bool default_wc_flag;


    parameters()->get_value( "-wc_filename" , wc_path );
    parameters()->get_value( "-wc_flag" , default_wc_flag );



    if (!default_wc_flag&&wc_path.path=="") return 0;
    if (!default_wc_flag && 0 /************/) {
      vcl_string wc_filename = wc_path.path;
      vcl_ifstream fp(wc_filename.c_str());
      if (!fp) {
        vcl_cout<<" Unable to Open "<< wc_filename <<vcl_endl;
        return false;
      }
      vnl_double_3x4 C;
      double wc_element;

      fp >> wc_element; C[0][0]= wc_element;
      fp >> wc_element; C[0][1]= wc_element; 
      fp >> wc_element; C[0][2]= wc_element;
      fp >> wc_element; C[0][3]= wc_element;

      fp >> wc_element; C[1][0]= wc_element; 
      fp >> wc_element; C[1][1]= wc_element;
      fp >> wc_element; C[1][2]= wc_element; 
      fp >> wc_element; C[1][3]= wc_element;

      fp >> wc_element; C[2][0]= wc_element;
      fp >> wc_element; C[2][1]= wc_element;
      fp >> wc_element; C[2][2] =wc_element;
      fp >> wc_element; C[2][3]= wc_element;
      //P1=C;
      WC_=C;



      // vul_file::strip_directory(world_camera);
    }
    else if (!default_wc_flag ){

      //    vcl_cout<<vul_file::dirname(wc_path);
      vcl_string wc_filename = wc_path.path;
      // test if fname is a directory
      //vul_file::exists

      vul_reg_exp r1("WC");
      vul_reg_exp r11("wc");
      vul_reg_exp r2("txt");

      vul_file_iterator fn=vul_file::dirname(wc_path.path)+"/*.txt";

      for ( ; fn; ++fn) 
      {
        vcl_string input_file = fn();

        if (r1.find(input_file.c_str())&&r2.find(input_file.c_str())||r11.find(input_file.c_str())&&r2.find(input_file.c_str()) )
        {
          vcl_ifstream fp(input_file.c_str());
          if (!fp) {
            vcl_cout<<" Unable to Open "<< wc_filename <<vcl_endl;
            return false;
          }
          vnl_double_3x4 C;
          double wc_element;

          fp >> wc_element; C[0][0]= wc_element;
          fp >> wc_element; C[0][1]= wc_element; 
          fp >> wc_element; C[0][2]= wc_element;
          fp >> wc_element; C[0][3]= wc_element;

          fp >> wc_element; C[1][0]= wc_element; 
          fp >> wc_element; C[1][1]= wc_element;
          fp >> wc_element; C[1][2]= wc_element; 
          fp >> wc_element; C[1][3]= wc_element;

          fp >> wc_element; C[2][0]= wc_element;
          fp >> wc_element; C[2][1]= wc_element;
          fp >> wc_element; C[2][2] =wc_element;
          fp >> wc_element; C[2][3]= wc_element;
          //P1=C;
          WC_=C;

          vcl_cout<<"World Camera: "<<WC_<<vcl_endl;
        }

        else
        {

          // vcl_cout<<" error # 12: Unable to Open "<< wc_filename<<vcl_endl;
          // return false;
          //  
        }

      } // fn loop
    }
    else {

      read_WC();//default camera..
    }
    // if(vnl_det(vnl_double_3x3(WC_.extract(3,3))) < 0) {
    //     WC_ *= -1;
    //     vcl_cout<<"WC_ *= -1 ***** right hand camera *****"<<vcl_endl;
    //     vcl_cout<<WC_<<vcl_endl;
    // }



    ////////////////////////

    vcl_cout<<  input_data_.size() <<vcl_endl;
    if ( input_data_.size() != 1 ){
      vcl_cout<<  input_data_.size() <<vcl_endl;
      vcl_cout << "In dbcri_whole_process::execute() - "
        << "not exactly one input images \n";
      return false;
    }
    clear_output();

    // get the process parameters 
    float minweightthresh = 0.2f, dist = 0.0f;
    static int rad=2;
    parameters()->get_value( "-dist" ,   dist);
    parameters()->get_value( "-minweightthresh" ,   minweightthresh);
    parameters()->get_value( "-rad" ,   rad);

    // get image from the storage class
    vidpro_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][1]);
    vcl_cout<<input_data_[0][1]<<vcl_endl;


    //Dongjin Han 7-10-06 convert color image into grey
    // convert the image to float and smooth it
    vil_image_resource_sptr image_sptr1 = frame_image->get_image();


    //vil_image_view<float> img=brip_vil_float_ops::convert_to_float(*image_rsc);

    vil_image_view< unsigned char > image_view = image_sptr1->get_view(0, image_sptr1->ni(), 0, image_sptr1->nj() );
    vil_image_view< unsigned char > image_rsc;
    if( image_view.nplanes() == 3 ) {
      vil_convert_planes_to_grey( image_view , image_rsc );
    }
    else if ( image_view.nplanes() == 1 ) {
      image_rsc = image_view;
    } else {
      vcl_cerr << "Returning false. nplanes(): " << image_rsc.nplanes() << vcl_endl;
      return false;
    }

    ///  // convert the image to float and smooth it
    ///  vil_image_resource_sptr image_rsc = frame_image->get_image();

    vil_image_view<float> img=brip_vil_float_ops::convert_to_float(image_rsc);


    //////// contour read....

    int N;
    double *x1,*y1;
    int frame=0;

    for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

      // get image from the storage class

      ////vidpro_image_storage_sptr frame_image;
      vidpro_vsol2D_storage_sptr frame_vsol=vidpro_vsol2D_storage_new();

      ////frame_image.vertical_cast(input_data_[frame][0]);
      frame_vsol.vertical_cast(input_data_[frame][0]);



      double max_x=-100.0f,max_y=-100.0f,min_x=100000.0f,min_y=100000.0f;

      double left_x=100000.0f, left_y=100000.00f,right_x=-100000.0f,right_y=-100000.0f;

      // parse through all the vsol classes and save curve objects only
      vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = frame_vsol->all_data();

      // test if only one region
      // vcl_cout<<frame<<" "<<vsol_list.size()<<vcl_endl;



      //epi_segment construction

      vsol_digital_curve_2d_sptr dc1 = new vsol_digital_curve_2d;
      //dc1=vsol_list[b]->cast_to_region()->cast_to_polygon()->cast_to_digital_curve_2d();
      for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
      {
        // contour case

        if( vsol_list[b]->cast_to_region())
        {
          if( vsol_list[b]->cast_to_region()->cast_to_polygon() )
          {
            vsol_polygon_2d_sptr poly_ = vsol_list[b]->cast_to_region()->cast_to_polygon();

            //////////////////////////
            polys_.push_back(poly_);
            //////////////////////////

            N=poly_->size();
            x1 = (double *)malloc(sizeof(double)*N);
            y1 = (double *)malloc(sizeof(double)*N);

            x1[0]=vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(0)->x();
            y1[0]=vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(0)->y();
            //vgl_polygon<double>
            for (unsigned int i=1; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
            {

              //vcl_cout<<"b: "<<b<<" "<<"i: "<<i<<vcl_endl;
              vsol_point_2d_sptr p1 = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i-1);
              vsol_point_2d_sptr p2 = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i);
              ////psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());

              x1[i]=p2->x();
              y1[i]=p2->y();



            }



          }
        }





      } //vsol_list

      vgl_polygon<double> veh_contour(x1,y1,N);
      veh_cons_.push_back(veh_contour);    
      free(x1);
      free(y1);
    }//loop frame//


    //get parameters
    parameters()->get_value( "-ssigma" ,  dp.smooth);
    parameters()->get_value( "-snoisethresh" , dp.noise_multiplier );
    parameters()->get_value( "-sauto_thresh" , dp.automatic_threshold );
    parameters()->get_value( "-sagressive_closure" , dp.aggressive_junction_closure );
    parameters()->get_value( "-scompute_juncts" , dp.junctionp );
    parameters()->get_value( "-sinclude_borders" , dp.borderp );


    dp.aggressive_junction_closure=0;
    dp.junctionp=false;
    dp.borderp=false;
    //start the timer
    vul_timer t;

    frame=0;
    for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

      // get image from the storage class
      vidpro_image_storage_sptr frame_image;
      frame_image.vertical_cast(input_data_[frame][1]);

      vil_image_resource_sptr image_sptr = frame_image->get_image();
      vil_image_view<vxl_byte> grey_view = vil_convert_stretch_range(vxl_byte(),
        vil_convert_to_grey_using_rgb_weighting(image_sptr->get_view()) );

      if(grey_view.nplanes() != 1) {
        vcl_cerr << "Returning false. nplanes(): " << grey_view.nplanes() << vcl_endl;
        return false;
      }

      vil1_memory_image_of< unsigned char > img = vil1_from_vil_image_view( grey_view );


      //initialize the detector
      sdet_detector detector(dp);
      detector.SetImage(img);
      //process edges
      detector.DoContour();
      vcl_vector<vtol_edge_2d_sptr> * edges = detector.GetEdges();

      if (!edges)
        return false;


      vcl_cout << "process " << edges->size() << " edges in " << t.real() << " msecs." << vcl_endl;

      // create the output storage class
      vidpro_vtol_storage_sptr output_vtol = vidpro_vtol_storage_new();
      //vsol_digital_curve_2d_sptr dc_line = new vsol_digital_curve_2d;
      // add the edges as topology objects to to storage class



      for ( vcl_vector<vtol_edge_2d_sptr>::iterator e_itr = edges->begin();
        e_itr != edges->end();  ++e_itr ) {
          vtol_topology_object_sptr edge = e_itr->ptr();  


          output_vtol->add_vtol(edge);


        }

        output_data_[frame].push_back(output_vtol);
    }




    return true;







}





static bool approx_equal(double a, double b, double thresh=1e-12)
{
  return vcl_abs(b-a) < thresh;
}


//: Finish
bool
dbcri_whole_process::finish()
{



  bool world_color_flag_=false;

  if(vnl_det(vnl_double_3x3(WC_.extract(3,3))) < 0) {
    WC_ *= -1;
    vcl_cout<<"WC_ *= -1 ***** right hand camera *****"<<vcl_endl;
    vcl_cout<<WC_<<vcl_endl;
  }


  good_3d_curves_.clear();


  //initialize scale
  scale_=1.0;


  //read world camera
  //read_WC();
  // find the first frame with input
  unsigned int frame = 0;
  for(; frame < input_data_.size(); ++frame)
    if(!input_data_[frame].empty() && input_data_[frame][0])
      break;

  if(frame >= input_data_.size())
    return false;

  //vcl_vector<vil_image_view<float> > images;
  vcl_vector <vgl_point_2d <double> >pts_upper;
  vcl_vector <vgl_point_2d <double> >pts_lower;


  //4-21-07
  pts_upper_line_.clear();
  pts_lower_line_.clear();
  //

  //vcl_vector <vsol_digital_curve_2d_sptr> dcl;
  vcl_vector <dbecl_episeg_sptr> episegl;
  int N;
  double *x1,*y1;

  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

    // get image from the storage class

    ////vidpro_image_storage_sptr frame_image;
    vidpro_vsol2D_storage_sptr frame_vsol=vidpro_vsol2D_storage_new();

    ////frame_image.vertical_cast(input_data_[frame][0]);
    frame_vsol.vertical_cast(input_data_[frame][0]);



    double max_x=-100.0f,max_y=-100.0f,min_x=100000.0f,min_y=100000.0f;

    double left_x=100000.0f, left_y=100000.00f,right_x=-100000.0f,right_y=-100000.0f;

    // parse through all the vsol classes and save curve objects only
    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = frame_vsol->all_data();

    // test if only one region
    // vcl_cout<<frame<<" "<<vsol_list.size()<<vcl_endl;



    //epi_segment construction

    vsol_digital_curve_2d_sptr dc1 = new vsol_digital_curve_2d;
    //dc1=vsol_list[b]->cast_to_region()->cast_to_polygon()->cast_to_digital_curve_2d();
    for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
    {
      if( vsol_list[b]->cast_to_curve())
      {
        if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
        {
          for (unsigned int i=1; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
          {
            vsol_point_2d_sptr p1 = vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i-1);
            vsol_point_2d_sptr p2 = vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i);
            ////psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
            if ((float)p1->y()>max_y) {
              max_x=(float)p1->x();
              max_y=(float)p1->y();
            }
            if ((float)p1->y()<min_y) {
              min_x=(float)p1->x();
              min_y=(float)p1->y();
            }

            //front end rear end
            if ((float)p1->x()>right_x) {
              right_x=(float)p1->x();
              right_y=(float)p1->y();
            }
            if ((float)p1->x()<left_x) {
              left_x=(float)p1->x();
              left_y=(float)p1->y();
            }

          }
        }
      }
      else {// contour case

        if( vsol_list[b]->cast_to_region())
        {
          if( vsol_list[b]->cast_to_region()->cast_to_polygon() )
          {
            vsol_polygon_2d_sptr poly_ = vsol_list[b]->cast_to_region()->cast_to_polygon();

            //////////////////////////
            polys_.push_back(poly_);
            //////////////////////////

            N=poly_->size();
            x1 = (double *)malloc(sizeof(double)*N);
            y1 = (double *)malloc(sizeof(double)*N);

            x1[0]=vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(0)->x();
            y1[0]=vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(0)->y();
            //vgl_polygon<double>
            for (unsigned int i=1; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
            {

              //vcl_cout<<"b: "<<b<<" "<<"i: "<<i<<vcl_endl;
              vsol_point_2d_sptr p1 = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i-1);
              vsol_point_2d_sptr p2 = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i);
              ////psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());

              x1[i]=p2->x();
              y1[i]=p2->y();


              //     vcl_cout<<p1->x()<<" "<<p1->y()<<" "<<p2->x()<<" "<<p2->y()<<vcl_endl;

              //episeg generation
              //
              dc1->add_vertex(p1);
              //dc1->add_vertex(p2);

              if ((float)p1->y()>max_y) {
                max_x=(float)p1->x();
                max_y=(float)p1->y();
              }
              if ((float)p1->y()<min_y) {
                min_x=(float)p1->x();
                min_y=(float)p1->y();
              }


              //front end rear end
              if ((float)p1->x()>right_x) {
                right_x=(float)p1->x();
                right_y=(float)p1->y();
              }
              if ((float)p1->x()<left_x) {
                left_x=(float)p1->x();
                left_y=(float)p1->y();
              }


            }



          }
        }


      }


    } //vsol_list

    // small y number -> upper line
    vgl_point_2d <double> p_lower(max_x,max_y);
    vgl_point_2d <double> p_upper(min_x,min_y);
    vcl_cout<<p_upper<<" "<<p_lower<<vcl_endl;
    pts_upper.push_back(p_upper);
    pts_lower.push_back(p_lower);

    /// 4-21-2007
    pts_upper_line_.push_back(p_upper);
    pts_lower_line_.push_back(p_lower);
    // 

    vgl_point_2d <double> left_pt(left_x,left_y);
    vgl_point_2d <double> right_pt(right_x,right_y);
    left_pts_.push_back(left_pt);
    right_pts_.push_back(right_pt);





    dcl_.push_back(dc1);

    /////    vgl_polygon<double> veh_contour(x1,y1,N);
    /////    veh_cons_.push_back(veh_contour);    
    /////   free(x1);
    /////    free(y1);
  }//loop frame//



  vgl_line_2d_regression<double> reg_upper;
  vgl_line_2d_regression<double> reg_lower;

  for (unsigned i=0;i<pts_upper.size();i++) {
    reg_upper.increment_partial_sums(pts_upper[i].x(), pts_upper[i].y());
    reg_lower.increment_partial_sums(pts_lower[i].x(), pts_lower[i].y());
  }


  reg_upper.fit();
  reg_lower.fit();
  //vcl_cout << "fitting error " << reg.get_rms_error() << '\n'
  //         << "fitted line " << reg.get_line() << '\n';

  //
  //
  //find the meeting point of upper and lower line
  //define initial epipole  as that point
  //

  vgl_line_2d <double> line_upper=reg_upper.get_line();
  vgl_line_2d <double> line_lower=reg_lower.get_line();

  line_upper_=line_upper;
  line_lower_=line_lower;

  vcl_cout<<"line upper and lower: "<<line_upper_<<" and "<<line_lower_<<vcl_endl;
  double a,b,c,d;
  a=line_upper.b()/line_upper.a(); b=line_upper.c()/line_upper.a();
  c=line_lower.b()/line_lower.a(); d=line_lower.c()/line_lower.a();

  double epi_x=(a*d-b*c)/(c-a);
  double epi_y=(b-d)/(c-a);



  //deal with when a==0 (ax+by=c)
  if (line_upper.a()==0.0) {


    epi_y=-line_upper.c()/line_upper.b();
    epi_x=1/(line_lower.a())*
      (line_lower.b()*line_upper.c()/line_upper.b()-line_lower.c());

  }

  epi_x_=epi_x;
  epi_y_=epi_y;


  /// deal with bad conditioned epipole ..
  bool epi_flag;
  bool draw_only;

  bool cluster_flag;
  parameters()->get_value( "-cluster_flag" , cluster_flag );
  cluster_flag_=cluster_flag;

  parameters()->get_value( "-epi_flag" , epi_flag );
  parameters()->get_value( "-draw_flag" , draw_only );
  // if (epi_x_<-400.0&&epi_flag) {
  if (epi_flag&& 0) { //turned off 11-27-07 

    vnl_double_3x4 WC=WC_;
    vnl_double_3 nep = WC.get_column(0);   
    epi_x_= float(nep[0]/nep[2]);
    epi_y_= float(nep[1]/nep[2]);

    epi_x_WC_=epi_x_;
    epi_y_WC_=epi_y_;

    vcl_cout<<"epipole from camera used"<<vcl_endl;
    vcl_cout<< epi_x_<<" "<<epi_y_<<vcl_endl;


  }


  /// deal with bad conditioned epipole ..
  float  manual_epi_x;
  float manual_epi_y;

  parameters()->get_value( "-epi_x" , manual_epi_x );
  parameters()->get_value( "-epi_y" , manual_epi_y );
  // if (epi_x_<-400.0&&epi_flag) {
  if (manual_epi_x!=0.0 && manual_epi_y!=0.0) {



    epi_x_= manual_epi_x;
    epi_y_= manual_epi_y;

    vcl_cout<<"Manual epipole used"<<vcl_endl;
    vcl_cout<< epi_x_<<" "<<epi_y_<<vcl_endl;


  }


  nframes_=frame;

  // get line to display

  //4-23-07 test epipole from contour if too far from WC discard..
  vnl_double_3x4 WC=WC_;
  vnl_double_3 nep = WC.get_column(0);   
  epi_x_WC_= float(nep[0]/nep[2]);
  epi_y_WC_= float(nep[1]/nep[2]);

  bool epi_offset_flag=false;

  parameters()->get_value( "-epi_offset_flag" , epi_offset_flag );
  double epi_offset=(epi_x_WC_-epi_x_)*(epi_x_WC_-epi_x_)+
    (epi_y_WC_-epi_y_)*(epi_y_WC_-epi_y_);
  vcl_cout<<"epi WC: "<<epi_x_WC_<<" "<<epi_y_WC_<<" || epi con tangent: "<<epi_x_<<" "<<epi_y_<<vcl_endl;
  vcl_cerr<<"epi WC: "<<epi_x_WC_<<" "<<epi_y_WC_<<" || epi con tangent: "<<epi_x_<<" "<<epi_y_<<vcl_endl;
  vcl_cerr<<"*%* epi_offset: "<<vcl_sqrt(epi_offset)<<vcl_endl;
  vcl_cout<<"*%* epi_offset: "<<vcl_sqrt(epi_offset)<<vcl_endl;

  if (epi_offset>100000.0 && epi_offset_flag)  //default NOT use
  {
    epi_x_=epi_x_WC_;
    epi_y_=epi_y_WC_;
    vcl_cerr<<"**big epi_offset** using WC epipole"<<vcl_endl;
    vcl_cout<<"**big epi_offset** using WC epipole"<<vcl_endl;
  }
  else
  {
    vcl_cerr<<" epi_offset small enough to use tangential epipole"<<vcl_endl;
    vcl_cout<<" epi_offset small enough to use tangential epipole"<<vcl_endl;
  }

  vgl_point_2d<double> epipole(epi_x_,epi_y_);
  vcl_cout<<"epipole: "<<epipole<<vcl_endl;
  vgl_point_2d <double> uppoint(1000.0,(-1000.0f*line_upper_.a()-line_upper_.c())/line_upper_.b());
  vgl_point_2d <double> lowpoint(1000.0,(-1000.0f*line_lower_.a()-line_lower_.c())/line_lower_.b());
  vsol_line_2d_sptr newhline_upper= new vsol_line_2d(epipole,uppoint);
  //newhline_upper->vgl_hline_2d();
  vsol_line_2d_sptr newhline_lower= new vsol_line_2d(epipole,lowpoint);

  //get angle..

  vcl_cout<<uppoint<<" "<<lowpoint<<vcl_endl;
  theta_pos_=-vcl_atan((uppoint.y()-epi_y_)/(1000.0f-epi_x_));
  theta_neg_=-vcl_atan((lowpoint.y()-epi_y_)/(1000.0f-epi_x_));

  //4-21-07
  angle_modify();

  /// generate angle_flag;
  angle_f_flag_.clear();
  angle_r_flag_.clear();

  //////////////////////////////////////////
  /////////////////////////////////////////
  for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=ang_ /*was .002*/) { /////////// !!!!!!!  must find better way !!!!!!!!
    bool angle_flag=true;

    angle_f_flag_.push_back(angle_flag);
    angle_r_flag_.push_back(angle_flag);
  }

  /////////////////////////////////////////////////////////////////////////////////////

  //generate polygon again...TT..
  vcl_vector <double *> veh_con_pol_x,veh_con_pol_y;
  vcl_vector <int> Nl;
  for (unsigned con_i=0;con_i<veh_cons_.size();con_i++)
  {

    //66////for epi seg for con file..bbbb
    vcl_vector <vsol_digital_curve_2d_sptr>  dclist;
    vsol_digital_curve_2d_sptr dc = new vsol_digital_curve_2d;


    int N=veh_cons_[con_i][0].size();
    double *px,*py;
    px = (double *)malloc(sizeof(double)*N);
    py = (double *)malloc(sizeof(double)*N);
    for (unsigned int s = 0; s < veh_cons_[con_i].num_sheets(); ++s)
      for (unsigned int p = 0; p < veh_cons_[con_i][s].size(); ++p)
      {
        //do_something(polygon[s][p].x(), polygon[s][p].y());
        px[p]=veh_cons_[con_i][s][p].x();
        py[p]=veh_cons_[con_i][s][p].y();

        //66////for epi seg for con file..bbbb
        vsol_digital_curve_2d_sptr dc = new vsol_digital_curve_2d;
        //dc->add_vertex(new vsol_point_2d(px[p],py[p]));

        // vcl_cout<<px[p]<<" "<<py[p]<<vcl_endl;
      }


      //66////for epi seg for con file..bbbb
      dclist.push_back(dc);
      veh_con_edge_.push_back(dclist);


      veh_con_pol_x.push_back(px);
      veh_con_pol_y.push_back(py);


      // free(px);free(py);
      Nl.push_back(N);
  }


  //  vgl_polygon<double> veh_contour(x1,y1,N);
  //   veh_cons_.push_back(veh_contour);    
  //   free(x1);
  //   free(y1);
  /////////////////////////////////////////////////////////////////////////////////////
  frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

    vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();


    //add to the storage class
    output_vsol->add_object( newhline_upper->cast_to_spatial_object() ,"output_upperline" );
    output_vsol->add_object( newhline_lower->cast_to_spatial_object() ,"output_lowerline" );
    // output_data_[frame].push_back(output_vsol);


    vcl_vector< vsol_spatial_object_2d_sptr > sh_pts;

    vidpro_vtol_storage_sptr frame_vtol;

    ////frame_image.vertical_cast(input_data_[frame][0]);
    vcl_cout<<output_data_.size()<<vcl_endl;
    vcl_cout<<output_data_[frame].size()<<vcl_endl;
    vcl_cout<<input_data_[frame][0]<<vcl_endl;
    frame_vtol.vertical_cast(input_data_[frame][2]);
    vcl_cout<<frame_vtol<<vcl_endl;
    vcl_vector <vsol_digital_curve_2d_sptr>  dclist;

    vcl_vector<vtol_edge_2d_sptr> edges;


    bool distance_measure;
    parameters()->get_value( "-edge_distance" , distance_measure );
    float distance_measure2;
    parameters()->get_value( "-edge_distance2" , distance_measure2 );
    float ec_size=4.0;
    parameters()->get_value( "-ec_size" , ec_size );

    for ( vcl_set<vtol_topology_object_sptr>::const_iterator itr = frame_vtol->begin();
      itr != frame_vtol->end();  ++itr ) {

        vtol_edge *edge = (*itr)->cast_to_edge();
        if (edge){
          vtol_edge_2d *edge_2d = edge->cast_to_edge_2d();
          if (edge_2d){
            vsol_curve_2d_sptr c = edge_2d->curve();

            vdgl_digital_curve_sptr vdc = c->cast_to_vdgl_digital_curve();
            if(vdc){
              // Extract the points along the edgel chain and make a vsol_digital_curve_2d
              vdgl_edgel_chain_sptr ec = vdc->get_interpolator()->get_edgel_chain();
              vsol_digital_curve_2d_sptr dc = new vsol_digital_curve_2d;

              /*if ( !veh_cons_[frame].contains((*ec)[0].get_x(),(*ec)[0].get_y())&&
              !veh_cons_[frame].contains((*ec)[ec->size()-1].get_x(),(*ec)[ec->size()-1].get_y()))
              continue;*/


              //if (ec->size()<4) continue;
              if (ec->size()<ec_size) continue; //25 good?
              for(unsigned int i=0; i<ec->size(); ++i){

                // For some reason vtol edges often contain duplicate points
                if ( dc->size() > 0 && dc->p1()->get_p() == ec->edgel(i).get_pt() )
                  continue;  
                // if outside contour ignore..
                if ( !veh_cons_[frame].contains((*ec)[i].get_x(),(*ec)[i].get_y()) )
                  continue;





                double xx=(*ec)[i].get_x();double yy=(*ec)[i].get_y();

                //vcl_cout<<veh_con_pol_x[frame][0]<<" "<<veh_con_pol_y[frame][0]<<vcl_endl;
                //vcl_cout<<veh_con_pol_x[frame][100]<<" "<<veh_con_pol_y[frame][100]<<vcl_endl;





                if (distance_measure) //
                {
                  double distance_from_contour = vgl_distance_to_closed_polygon( veh_con_pol_x[frame] ,veh_con_pol_y[frame] ,Nl[frame],xx ,yy);


                  //!!!!!!!!!!!!!!!!!
                  //!!!!!!!!!!!!!!!!!!!!
                  //!!!!!!!!!!!!!!!!!!!!!
                  //!!!!!!!!!!!!!!!!!!!!
                  //!!!!!!!!!!!!!!!!!!!!!!
                  //!!!!!!!!!!!!!!!!!!!!!!!
                  //!!!!!!!!!!!!!!!!!!!!!!!
                  if (distance_from_contour>distance_measure2) continue;
                  //!!!!!!!!!!!!!!!!!
                  //!!!!!!!!!!!!!!!!!!!!
                  //!!!!!!!!!!!!!!!!!!!!!
                  //!!!!!!!!!!!!!!!!!!!!
                  //!!!!!!!!!!!!!!!!!!!!!!
                  //!!!!!!!!!!!!!!!!!!!!!!!
                  //!!!!!!!!!!!!!!!!!!!!!!!

                }
                dc->add_vertex(new vsol_point_2d(ec->edgel(i).get_pt()));
                // vcl_cout<< (*ec)[i].get_x()<<" "<<(*ec)[i].get_y()<<vcl_endl;

                sh_pts.push_back(new vsol_point_2d(ec->edgel(i).get_pt()));
              }
              // Cover the digital curve with episegs
              /*  vcl_vector<dbecl_episeg_sptr> eps = factory.convert_curve(dc);
              for(vcl_vector<dbecl_episeg_sptr>::iterator itr = eps.begin();
              itr != eps.end();  ++itr)
              episegs.push_back(*itr);
              }*/
              dclist.push_back(dc);

            }
          }
        }
      }

      dcl_edge_.push_back(dclist);
      output_vsol->add_objects(sh_pts);
      output_data_[frame].push_back(output_vsol);

  }



  vnl_vector <double> tnew_selected;

  float epi_search_range_x,epi_search_range_y,epi_search_interval;
  parameters()->get_value( "-e_range_x" , epi_search_range_x );
  parameters()->get_value( "-e_range_y" , epi_search_range_y );
  parameters()->get_value( "-epi_search_interval" , epi_search_interval );
  parameters()->get_value( "-MARGIN" , MARGIN_ );

  // epi_search(tnew_selected,epi_search_range_x,epi_search_range_y,epi_search_interval);
  // epi_search(tnew_selected,epi_search_range_x/10.0,epi_search_range_y/10.0,epi_search_interval/10.0);
  // epi_search(tnew_selected,epi_search_range_x/20.0,epi_search_range_y/20.0,epi_search_interval/20.0);

  vul_timer t;


  ///
  ///4-20-2007 epi_search 200 100 might be too big?
  ///if (output_data_.size() <7) {
  ///epi_search(tnew_selected,200.0,100.0,50.0);
  ///  epi_search(tnew_selected,50.0,25.0,25.0);
  ///  epi_search(tnew_selected,30.0,20.0,5.0);
  ///  epi_search(tnew_selected,10.0,5.0,2.5);
  ///  epi_search(tnew_selected,3,2,1);
  ///}

  bool test_epipole=false;

  bool wide=false;
  parameters()->get_value( "-man_epi_flag" , test_epipole );
  parameters()->get_value( "-epi_search_range_wide", wide );
  //epi_search_fast_lite(tnew_selected,500.0,100.0,50.0);exit(1);
  if (!test_epipole)
    if (1) {
      //read epi pole from old result to file to save searching time.. 5-26-07
      bool epi_from_result_file;
      parameters()->get_value( "-epi_from_result_file" , epi_from_result_file );
      if (epi_from_result_file &&1 && read_epi_search())
      {
        if (output_data_.size() <7) {
          epi_search(tnew_selected,0,0,25.0);//100 50 50  4-22-07 old values
        }
        else {
          epi_search_lite(tnew_selected,0,0,5.0);
        }
      }

      else {
        if (epi_flag) {

          vnl_double_3x4 WC=WC_;
          vnl_double_3 nep = WC.get_column(0);   
          epi_x_= float(nep[0]/nep[2]);
          epi_y_= float(nep[1]/nep[2]);

          epi_x_WC_=epi_x_;
          epi_y_WC_=epi_y_;

          vcl_cout<<"epipole from camera used"<<vcl_endl;
          vcl_cout<< epi_x_<<" "<<epi_y_<<vcl_endl;


        }
        bool fast=true;
        if (!fast)
          if (output_data_.size() <7) {
            epi_search(tnew_selected,100.0,25.0,25.0);//100 50 25
            epi_search(tnew_selected,30.0,10.0,10.0); // 60 20 20
            epi_search(tnew_selected,10.0,5.0,2.5);  //20 10 5
            epi_search(tnew_selected,2.5,2.0,1.0);

            //7-1-07 test
            //  epi_search(tnew_selected,200.0,25.0,25.0);//100 50 25
            //epi_search(tnew_selected,50.0,20.0,20.0); // 60 20 20
            //epi_search(tnew_selected,10.0,5.0,5);  //20 10 5
          }
          else {
            epi_search_lite(tnew_selected,100.0,25.0,25.0);
            epi_search_lite(tnew_selected,30.0,10.0,10.0);
            epi_search_lite(tnew_selected,10.0,5.0,2.5);
            epi_search_lite(tnew_selected,2.5,2.0,1.0);


            //epi_search_lite(tnew_selected,10.0,5.0,2.5);
            // epi_search_lite(tnew_selected,3,2,1);

          }

        else
          if (output_data_.size() <7) {
            if (wide)
              epi_search_fast(tnew_selected,250.0,100.0,25.0);
            else epi_search_fast(tnew_selected,100.0,25.0,25.0);//100 50 25
            epi_search_fast(tnew_selected,30.0,10.0,10.0); // 60 20 20
            epi_search_fast(tnew_selected,10.0,5.0,2.5);  //20 10 5
            epi_search_fast(tnew_selected,2.5,2.0,1.0);

            //7-1-07 test
            //  epi_search(tnew_selected,200.0,25.0,25.0);//100 50 25
            //epi_search(tnew_selected,50.0,20.0,20.0); // 60 20 20
            //epi_search(tnew_selected,10.0,5.0,5);  //20 10 5
          }
          else {
            if (wide)
              epi_search_fast_lite(tnew_selected,250.0,100.0,25.0);
            else
              epi_search_fast_lite(tnew_selected,100.0,25.0,25.0);
            epi_search_fast_lite(tnew_selected,30.0,10.0,10.0);
            epi_search_fast_lite(tnew_selected,10.0,5.0,2.5);
            epi_search_fast_lite(tnew_selected,2.5,2.0,1.0);


            //epi_search_lite(tnew_selected,10.0,5.0,2.5);
            // epi_search_lite(tnew_selected,3,2,1);

          }


      }
    }
    else {//epipole constraint on the plane --no good

      tnew_selected=epi_constraint_on_the_plane(  epi_x_, epi_y_, 150, 25);//100 too small
      tnew_selected=epi_constraint_on_the_plane(  epi_x_, epi_y_, 20, 5);
      //tnew_selected=epi_constraint_on_the_plane(  epi_x_, epi_y_, 4, 1);
      if (output_data_.size() <7) {

        epi_search(tnew_selected,6.0,9.0,3.0);  //10 10 5    4-22-07 old values
      }
      else {

        epi_search_lite(tnew_selected,6.0,9.0,3.0);
        //epi_search_lite(tnew_selected,10.0,5.0,2.5);
        // epi_search_lite(tnew_selected,3,2,1);

      }

      /// epi constraint onto plane..
      /*vcl_vector <vnl_double_2> epi_const=epi_constraint_on_the_plane(  epi_x_, epi_y_, 100, 2);
      //read epi pole from old result to file to save searching time.. 5-26-07
      bool epi_from_result_file;
      parameters()->get_value( "-epi_from_result_file" , epi_from_result_file );
      if (epi_from_result_file && read_epi_search())
      {
      if (output_data_.size() <7) {
      epi_search(tnew_selected,0,0,25.0);//100 50 50  4-22-07 old values
      }
      else {
      epi_search_lite(tnew_selected,0,0,5.0);
      }
      }

      else
      {*/
      /*double sum;double max_sum=-100000000000.0;
      vnl_vector <double> tnew_max;
      double max_epi_x,max_epi_y;
      for (unsigned i=0;i<epi_const.size();i++) {
      epi_x_=epi_const[i][0];epi_y_=epi_const[i][1];
      if (output_data_.size() <7)
      sum=epi_search(tnew_selected,0,0,25.0);// 25.0 means nothing
      else
      sum=epi_search_lite(tnew_selected,0,0,25.0);//100 50 50  4-22-07 old values

      if (sum>max_sum) {

      max_sum=sum;

      tnew_max=tnew_selected;

      max_epi_x=epi_x_;
      max_epi_y=epi_y_;

      }
      }
      tnew_selected=tnew_max;
      init_cameras(tnew_selected);
      epi_x_=max_epi_x;
      epi_y_=max_epi_y;



      vcl_cout<<"epi plane constraint"<<vcl_endl;
      vcl_cout<<"linear search max val epi is : "<<epi_x_<<" "<<epi_y_<<vcl_endl;
      }*/

    }

    //temp
    if (test_epipole)
    {
      epi_x_=-707.253;epi_y_= 168.348;// constraint on plane id 3935
      epi_x_=-554.753;epi_y_= 199.104; //free

      epi_x_=-385.669; epi_y_=190.178; //4202
      epi_x_=-371.581; epi_y_=272.204; //4476


      // 
      float  manual_epi_x;
      float manual_epi_y;
      parameters()->get_value( "-epi_x" , manual_epi_x );
      parameters()->get_value( "-epi_y" , manual_epi_y );
      if (manual_epi_x!=0.0 && manual_epi_y!=0.0) {
        epi_x_= manual_epi_x;
        epi_y_= manual_epi_y;
        vcl_cout << "**manual epi** " << epi_x_<<" "<<epi_y_ << " for test" << vcl_endl;

      }

      if (output_data_.size() <7) {
        epi_search(tnew_selected,0,0,3.0);  //10 10 5    4-22-07 old values
      }
      else {
        epi_search_lite(tnew_selected,0,0,3.0);
      }
    }
    //temp end


    vcl_cout << "**search process** " <<  t.real() << " msecs." << vcl_endl;
    vcl_cerr << "**search process** " <<  t.real() << " msecs." << vcl_endl;


    frame = 0;
    for(; frame < input_data_.size(); ++frame)
      if(!input_data_[frame].empty() && input_data_[frame][0])
        break;

    if(frame >= input_data_.size())
      return false;

    int frame_counter=0;


    ///////////////////////
    //display pts
    ///////////////////////

    double total_t_selected=0.0;
    for (unsigned i=0;i<tnew_selected.size();i++) 
      total_t_selected+=tnew_selected[i];
    total_t_selected+=1.0;
    frame = 0;
    for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){


      vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();

      if (frame==0||frame==input_data_.size()-1)
      {
        vsol_point_2d_sptr a=new vsol_point_2d(epi_x_,epi_y_);
        output_vsol->add_object(a->cast_to_spatial_object() ,"output_midpoint");
        output_data_[frame].push_back(output_vsol);
        continue;
      }

      //////////////////
      //front display
      //////////////////

      for (unsigned i=0 ; i< display_sample_front_.size() ; i++) { 

        vgl_point_2d<double> endpoint=display_sample_front_[i].pt1;
        vgl_point_2d<double> startpoint=display_sample_front_[i].pt3;

        vsol_line_2d_sptr line_mid= new vsol_line_2d(epipole,endpoint);
        double x_dist=endpoint.x()-epipole.x();
        double y_dist=endpoint.y()-epipole.y();


        //double line_scale=x_dist/total_t;

        double a=display_sample_front_[i].fm3;
        double c=display_sample_front_[i].fm1;

        double test_b=display_sample_front_[i].fm2;

        //8-1-2006// vcl_cout<<vcl_sqrt(x_dist*x_dist+y_dist*y_dist)<<" "<<c<<vcl_endl;

        double temp_r=0;

        for (unsigned tr=0;tr<frame;tr++) temp_r+=tnew_selected[tr];

        double r=(total_t_selected-temp_r)/total_t_selected;
        //double r=temp_r/total_t;
        double r_prime=r*(c-a)/c;
        double b=a/(1-r_prime);


        vsol_point_2d_sptr point_mid=new vsol_point_2d(epipole.x()+b/c*x_dist,epipole.y()+b/c*y_dist);

        //8-1-2006//vcl_cout<<(epipole.x()+b/c*x_dist)<<" "<<epipole.y()+b/c*y_dist<<vcl_endl;
        //output_vsol->add_object( line_mid->cast_to_spatial_object() ,"output_midline" );
        output_vsol->add_object( point_mid->cast_to_spatial_object() ,"output_midpoint" );

      }
      output_data_[frame].push_back(output_vsol);
      //rear display


    }

    //return true;

    /////////////////////
    // 3d reconstruction
    // 3d reconstruction
    // 3d reconstruction
    // 3d reconstruction
    // 3d reconstruction
    ////////////////////////////////
    //d3_build();



    //9-22-2006 *********************************
    // getting scale from contour points
    //then use real curves to get 3D points..
    // this can result in something strange...
    ///******************************************


    vcl_vector <vgl_point_3d<double> > point_3d_for_out;
    double real_scale;

    for (unsigned d3_rec=0;d3_rec<2;d3_rec++) {

      /////////////////////////////
      vcl_cout<<"RECON ============================================== "<<vcl_endl;
      vcl_cout<<"RECON METHOD: "<< d3_rec<<" epipole used:"<<epi_x_<<" "<<epi_y_<<vcl_endl;
      vcl_cout<<"RECON ============================================== "<<vcl_endl;
      init_cameras(tnew_selected);

      ////////////////////////////

      pts_3d_a_.clear();pts_3d_.clear();
      vcl_vector <vgl_point_3d<double> > point_3d_mean_pts;

      d3_build_points(vcl_numeric_limits<double>::infinity(), point_3d_mean_pts); //no threshold

      ///////////get bb and orentation and scale 

      vcl_cout<<"Delete Contour 1============================================== "<<vcl_endl;
      delete_contour();
      vcl_cout<<"Delete Contour 1 END======================================== "<<vcl_endl;
      pts_3d_.clear();
      int point_counter=0;
      for (unsigned i=0;i <pts_3d_a_.size();i++) {
        point_3d_for_out.push_back(pts_3d_a_[i].pt3d);
        //vcl_cout<<point_3d_mean_pts_with_scale[i]<<vcl_endl;

        if (0) {
          vcl_cout<<pts_3d_a_[i].pt3d<<vcl_endl;
          vcl_cout<<pts_3d_a_[i].position<<vcl_endl;
          vcl_cout<<pts_3d_a_[i].angle<<vcl_endl;
          vcl_cout<<pts_3d_a_[i].weight<<vcl_endl;
          vcl_cout<<vcl_endl;

        }


        ////////////
        if (pts_3d_a_[i].usage)
        {
          pts_3d_.push_back(pts_3d_a_[i].pt3d); 
          point_counter++;

        }
      }

      vcl_cout<<"## step0 ## points used(all-> weight..) counter 1: "<<point_counter<<vcl_endl;

      vcl_cout<<"get BB and scale # 1.5============================================== "<<vcl_endl;
      get_BB_and_scale(pts_3d_,pts_3d_);
      vcl_cout<<"get BB and scale # 1.5     end   =================================== "<<vcl_endl;

      init_cameras(tnew_selected,scale_);

      ////////////////////////////

      pts_3d_a_.clear();pts_3d_.clear();
      //      vcl_vector <vgl_point_3d<double> > point_3d_mean_pts;
      // vcl_vector <vgl_point_3d<double> > point_3d_for_out;
      //*//d3_build_points(10000000000000.0, point_3d_mean_pts);
      d3_build_points(vcl_numeric_limits<double>::infinity(), point_3d_mean_pts); //no threshold
      //d3_build_points_edge_con(10000000000000.0, point_3d_mean_pts);
      //d3_build_points_edge(10000000000000.0, point_3d_mean_pts);


      point_counter=0;
      for (unsigned i=0;i <pts_3d_a_.size();i++) {
        point_3d_for_out.push_back(pts_3d_a_[i].pt3d);
        //vcl_cout<<point_3d_mean_pts_with_scale[i]<<vcl_endl;

        if (0) {
          vcl_cout<<pts_3d_a_[i].pt3d<<vcl_endl;
          vcl_cout<<pts_3d_a_[i].position<<vcl_endl;
          vcl_cout<<pts_3d_a_[i].angle<<vcl_endl;
          vcl_cout<<pts_3d_a_[i].weight<<vcl_endl;
          vcl_cout<<vcl_endl;

        }


        ////////////
        if (pts_3d_a_[i].usage)
        {
          pts_3d_.push_back(pts_3d_a_[i].pt3d); 
          point_counter++;

        }
      }
      vcl_cout<<"## step1.5 ## points used(all-> weight..) counter 1:"<<point_counter<<vcl_endl;



      ///////////get bb and orentation and scale 

      vcl_cout<<"Delete Contour 1.5============================================== "<<vcl_endl;
      delete_contour();
      vcl_cout<<"Delete Contour 1.5 END======================================== "<<vcl_endl;


      ////////////////////////////////////////////////////////
      vcl_cout<<"RECON METHOD: "<< d3_rec<<" SCALE: "<<scale_<<vcl_endl; //this is real scale.../////
      real_scale=scale_;   // so i copy and save  /////
      ///////     use it out_pts = out_pts x real scale  /////
      ////////////////////////////////////////////////////////

      //get_BB_and_scale(point_3d_mean_pts,point_3d_mean_pts);


      // redo it with new scale
      vcl_cout<<"init camera with correct scale ==================================== "<<vcl_endl;
      init_cameras(tnew_selected,scale_);
      vcl_cout<<"init camera with correct scale end   ============================== "<<vcl_endl;

      vcl_vector <vgl_point_3d<double> > point_3d_mean_pts_with_scale;




      float error_recon_float;
      parameters()->get_value( "-d3_error" , error_recon_float );//5.0
      double error_recon=error_recon_float;
      ///d3_build_points(error_recon, point_3d_mean_pts_with_scale);
      ///d3_build_points_edge(3.0, point_3d_mean_pts_with_scale);


      //9-22-2006 *********************************
      // getting scale from contour points
      //then use real curves to get 3D points..
      // this can result in something strange...
      ///******************************************

      vcl_cout<<" d3_build_points(error_recon, point_3d_mean_pts_with_scale,true)============ "<<vcl_endl;
      d3_build_points(error_recon, point_3d_mean_pts_with_scale,true); // here pts_3d_a_ is   cleared
      vcl_cout<<"d3_build_points(------> recon method id "<<d3_rec<<"    pts_3d_a_ size "<<pts_3d_a_.size()<<vcl_endl;
      vcl_cout<<" d3_build_points(error_recon, point_3d_mean_pts_with_scale,true)  end  ===== "<<vcl_endl;

      int recon_flag;
      parameters()->get_value( "-recon_flag" , recon_flag );//int
      recon_flag=d3_rec;
      vcl_cout<<"recon_flag: "<<recon_flag<<vcl_endl;


      vcl_cout<<"Final Reconstruction ========================================= "<<vcl_endl;

      if (recon_flag==1) //both aabb
        d3_build_points_edge_con(error_recon, point_3d_mean_pts_with_scale,false); // here pts_3d_a_ is not  cleared, just addition
      else if (recon_flag==0) //con
        // d3_build_points(error_recon, point_3d_mean_pts_with_scale); // here pts_3d_a_ is   cleared
        d3_build_points(10000.0, point_3d_mean_pts_with_scale); // here pts_3d_a_ is   cleared
      else if (recon_flag==2) //messy
        d3_build_points_edge(error_recon, point_3d_mean_pts_with_scale);
      else if (recon_flag==3) //0+1+2
      {
        d3_build_points_edge(error_recon, point_3d_mean_pts_with_scale); // always cleared...
        d3_build_points(error_recon, point_3d_mean_pts_with_scale,false); // here pts_3d_a_ is not  cleared
        d3_build_points_edge_con(error_recon, point_3d_mean_pts_with_scale,false); // here pts_3d_a_ is not  cleared, just addition
      }
      else ;

      //return false;


      vcl_cout<<"*&*&*&* recon method id "<<d3_rec<<"    pts_3d_a_ size "<<pts_3d_a_.size()<<vcl_endl;
      if (0) for (unsigned i=0;i <pts_3d_a_.size();i++) {//d3_build_points  clear pts_3_a_




        if (1){ 
          vcl_cout<<"recon method id "<<d3_rec<<"   aa#a"<<pts_3d_a_[i].pt3d<<vcl_endl;
          vcl_cout<<"aapos   # "<<pts_3d_a_[i].position<<vcl_endl;
          vcl_cout<<"aaangle # "<<pts_3d_a_[i].angle<<vcl_endl;
          vcl_cout<<"aaweight# "<<pts_3d_a_[i].weight<<vcl_endl;
          vcl_cout<<"aausage # "<<pts_3d_a_[i].usage<<vcl_endl;
          vcl_cout<<vcl_endl;

        }
      }






      vcl_cout<<"# Delete Contour 2 (0.5 ft delete)  ========================================= "<<vcl_endl;
      vcl_cout<<"*&*&*&* recon method id "<<d3_rec<<" pts_3d_a_.size() "<<pts_3d_a_.size()<<vcl_endl;
      //delete_contour(.5);//delete 1 feet high too.
      delete_contour_05(.5);//delete 1 feet high too.
      vcl_cout<<"# Delete Contour 2 (0.5 ft delete) end  ========================================= "<<vcl_endl;

      vcl_cout<<"=+=+=+=+=+=+"<<vcl_endl;
      pts_3d_.clear(); point_counter=0;
      pts_3d_front_.clear();pts_3d_rear_.clear();
      vcl_cout<<"##final recon points size: "<<pts_3d_a_.size()<<vcl_endl;
      vcl_cout<<"*&*&*&* recon method id "<<d3_rec<<"pts_3d_a_.size() "<<pts_3d_a_.size()<<vcl_endl;
      for (unsigned i=0;i <pts_3d_a_.size();i++) {//d3_build_points  clear pts_3_a_

        //vcl_cout<<point_3d_mean_pts_with_scale[i]<<vcl_endl;


        if (1){ 
          vcl_cout<<"#"<<pts_3d_a_[i].pt3d<<vcl_endl;
          vcl_cout<<"pos   # "<<pts_3d_a_[i].position<<vcl_endl;
          vcl_cout<<"angle # "<<pts_3d_a_[i].angle<<vcl_endl;
          vcl_cout<<"weight# "<<pts_3d_a_[i].weight<<vcl_endl;
          vcl_cout<<"usage # "<<pts_3d_a_[i].usage<<vcl_endl;
          vcl_cout<<vcl_endl;

        }


        ////////////
        if (pts_3d_a_[i].usage) {
          pts_3d_.push_back(pts_3d_a_[i].pt3d); 
          point_counter++;

          if (pts_3d_a_[i].position==false)
            pts_3d_rear_.push_back(pts_3d_a_[i].pt3d);
          if (pts_3d_a_[i].position==true)
            pts_3d_front_.push_back(pts_3d_a_[i].pt3d);
        }
      } 
      vcl_cout<<"points used counter del05 2: "<<point_counter<<vcl_endl;
      vcl_cout<<"*&*&*&* recon method id "<<d3_rec<<"pts_3d_a_.size() "<<pts_3d_a_.size()<<vcl_endl;
      vcl_cout<<"=+=+=+=+=+=+"<<vcl_endl;




      if (0) {
        // 5-25-2007 bug fixed.. 
        //after delete contour 
        // pts_3d_rear_ and front_ 

        pts_3d_front_.clear();pts_3d_rear_.clear();
        for (unsigned i=0;i <pts_3d_a_.size();i++) {//d3_build_points  clear pts_3_a_
          //vcl_cout<<point_3d_mean_pts_with_scale[i]<<vcl_endl;
          ////////////
          if (pts_3d_a_[i].usage) {

            if (pts_3d_a_[i].position==false)
              pts_3d_rear_.push_back(pts_3d_a_[i].pt3d);
            if (pts_3d_a_[i].position==true)
              pts_3d_front_.push_back(pts_3d_a_[i].pt3d);
          }
        } 
        // ^^ 5-25-07
        //////
      }




      vcl_cout<<"get BB and scale # 2  ============================================ "<<vcl_endl;
      vcl_cout<<"*&*&*&* recon method id "<<d3_rec<<"pts_3d_a_.size() "<<pts_3d_a_.size()<<vcl_endl;
      get_BB_and_scale(pts_3d_,pts_3d_);
      vcl_cout<<"get BB and scale # 2 end  ======================================== "<<vcl_endl;



      // test if fname is a directory
      //vul_file::exists

      bpro_filepath wc_path;
      parameters()->get_value( "-wc_filename" , wc_path );


      vcl_string out_file_dir=vul_file::dirname(wc_path.path);



      vcl_string vr_dir;
      vcl_string mode;
      if (recon_flag==0) { vr_dir="contour_0_"; mode="0";}
      else if (recon_flag==1) {vr_dir="correct_1_";mode="1";}
      else if (recon_flag==2) {vr_dir="messy_2_";mode="2";}
      else if (recon_flag==3) {vr_dir="all_3_";mode="3";}
      else ;
      int secs,msecs;
      vul_get_timestamp(secs,msecs);

      char buffer[33];
      //itoa (secs,buffer,10);

      //vr_dir+=buffer;
      //vr_dir+=secs;



      char dateStr [12];
      char tmpbuf[17], ampm[] = "AM";
      time_t ltime;
      struct _timeb tstruct;
      //struct tm *today, *gmt, xmas = { 0, 0, 12, 25, 11, 93 };
      struct tm *today;
      time( &ltime );
      today = localtime( &ltime );


      printf ( "The current date/time is: %s", asctime (today) );
      //strftime( tmpbuf, 128,"Today is %A, day %d of %B in the year %Y.\n", today );
      strftime( tmpbuf, 16,"%m-%d-%Y-%I%p", today );

      printf( tmpbuf );

      //_strtime( tmpbuf );

      vcl_string strDate;
      strDate=tmpbuf;
      //_strdate(dateStr);
      //strDate=dateStr;
      //strDate= strDate.replace("/","-");

      vcl_cout<<"date is: "<<today<<" "<<strDate<<vcl_endl;

      vr_dir+=strDate;

      vul_file::make_directory(vr_dir);


      vcl_string vrml_file =out_file_dir+"//"+vr_dir+"//"+"del-AEC-"+mode+".wrl";
      vcl_string vrml_filef=out_file_dir+"//"+vr_dir+"//"+"delf-AEC-"+mode+".wrl";
      vcl_string vrml_filer=out_file_dir+"//"+vr_dir+"//"+"delr-AEC-"+mode+".wrl";
      vcl_string vrml_file3=out_file_dir+"//"+vr_dir+"//"+"del-AEC-all-"+mode+".wrl";

      vcl_ofstream vrml(vrml_file.c_str());
      vcl_ofstream vrmlf(vrml_filef.c_str());
      vcl_ofstream vrmlr(vrml_filer.c_str());
      vcl_ofstream vrml3(vrml_file3.c_str());

      write_vrml_2(vrml,pts_3d_);
      write_vrml_2(vrmlf,pts_3d_front_);
      write_vrml_2(vrmlr,pts_3d_rear_);

      write_vrml_3(vrml3);

      ///find mean_y_1 and mean_y_2 for calculation of z..using tangent line.
      // Prof. Coopers idea,.. thanks... 12-5-2006
      mean_y_1_=0;
      for (unsigned i=0;i<pts_3d_front_.size();i++)
        mean_y_1_+=pts_3d_front_[i].y();
      mean_y_1_/=(double)pts_3d_front_.size();

      mean_y_2_=0;
      for (unsigned i=0;i<pts_3d_rear_.size();i++)
        mean_y_2_+=pts_3d_rear_[i].y();
      mean_y_2_/=(double)pts_3d_rear_.size();



      //get_BB_and_scale(pts_3d_,pts_3d_);
      write_vrml_bbox(vrml, bb_xform_);
      write_vrml_bbox(vrmlf, bb_xform_);
      write_vrml_bbox(vrmlr, bb_xform_);
      write_vrml_bbox(vrml3, bb_xform_);

      /////////////////////////////////////////////
      /////////////////////////////////////////////
      /////////////////////////////////////////////
      //return true;
      bool auto_shiftp;
      parameters()->get_value( "-auto_shift" , auto_shiftp );

      if (auto_shiftp)
        Estimation_BB_shift_using_convex_hull_in_2D();// reduce the BB size

      /////////////////////////////////////////////
      /////////////////////////////////////////////
      /////////////////////////////////////////////
      double r=.1;
      double r1,r2;
      spacial_filtering_x(r1,r2,pts_3d_);
      //spacial_filtering_x_with_z(r1,r2,pts_3d_);//6-22-07//



      double tube_radius=.1; // 2R
      double tube_length=2.0;
      vgl_vector_3d<double> t1(-1,0,.1); //slightly up
      vgl_vector_3d<double> t2(0,0,-1);
      vgl_vector_3d<double> t3(0,0,3);
      vgl_vector_3d<double> t4(0,0,3);


      //tube aTube(pts_3d_rear_[0],t1,t2,t3,t4,tube_radius,tube_length,1,1,0,0);
      //aTube.set_length(4.0);
      //aTube.ranger(pts_3d_rear_);

      vcl_string bb_dir =out_file_dir+"//"+vr_dir;


      // solve_z has to come before bb_box camera writing routine. 
      //  5-23-07
      if (d3_rec==1||d3_rec==0) {
        solve_z(pts_upper,pts_lower);
        //solve_front_windshield(pts_upper,pts_lower);
        solve_front_windshield(); //method 2 subtraction
        //        solve_front_windshield1();
      }




      print_bb_and_camera(bb_dir);
      print_con_probe(bb_dir);
      ///7-1-07/// spacial_filtering_x(r1,r2,pts_3d_front_);
      ///7-1-07/// spacial_filtering_x(r1,r2,pts_3d_rear_);
      ///7-1-07/// spacial_filtering(r,pts_3d_);




      /* Estimation_BB_and_bb_using_convex_hull_in2D(0);
      Estimation_BB_and_bb_using_convex_hull_in2D(1);
      Estimation_BB_and_bb_using_convex_hull_in2D(2);
      generate_mean_contour_model(0);
      generate_mean_contour_model(1);
      generate_mean_contour_model(2);
      */
      /////////////////////////////////////////////
      /////////////////////////////////////////////
      /////////////////////////////////////////////

      /*    write_vrml_bbox(vrml, bb_xform_);
      write_vrml_bbox(vrmlf, bb_xform_);
      write_vrml_bbox(vrmlr, bb_xform_);
      write_vrml_bbox(vrml3, bb_xform_);*/
      vrml.close();
      vrmlf.close();
      vrmlr.close();
      vrml3.close();


      vcl_cout<<"RECON END============================================== "<<vcl_endl;
      vcl_cout<<"RECON END============================================== "<<vcl_endl;
      vcl_cout<<"RECON END============================================== "<<vcl_endl;

    }/// d3_recon loop end

    //vcl_cout << "process " << t.real() << " msecs." << vcl_endl;


    // print_bb_and_camera();
    // redo it with new scale
    // init_cameras(tnew,scale_);


    /////////// vrml generation
    //  return(1);
    vcl_ofstream out("out-AEC.wrl");

    if(!out.is_open()){
      std::cerr<<"Cannot open the input file.\n";
      exit(2);
    }



    out <<      "#VRML V2.0 utf8\n";
    out <<      "Background { \n";
    out <<      "    skyColor [ 1 1 1 ]\n";
    out <<      "    groundColor [ 1 1 1 ]\n";
    out <<      "}\n";
    out <<      "PointLight {\n";
    out <<      "    on FALSE\n";
    out <<      "    intensity 1 \n";
    out <<      "ambientIntensity 0 \n";
    out <<      "color 1 1 1 \n";
    out <<      "location 0 0 0 \n";
    out <<      "attenuation 1 0 0 \n";
    out <<      "radius 100  \n";
    out <<      "}\n";
    out <<      "Shape {\n";
    out <<      "   #make the points white\n";
    out <<      "    appearance Appearance {\n";
    out <<      "       material Material { emissiveColor 1 0 0 }\n";
    out <<      "   } \n";
    out <<      "   geometry PointSet {\n";
    out <<      "      coord Coordinate{\n";
    out <<      "         point[\n";
    out <<      "         ]\n";
    out <<      "      }\n";
    out <<      "      color Color { color [ 1 1 1 ] }\n";
    out <<      "   }\n";
    out <<      "}\n";


    float xx,yy,zz;
    float red=1,green=0,blue=0;

    double radius=0.05;
    blue=1.0;

    for (unsigned i=0;i <point_3d_for_out.size();i++) {
      //point_3d_mean_pts[i].set(scale_*point_3d_mean_pts[i].x(),scale_*point_3d_mean_pts[i].y(),scale_*point_3d_mean_pts[i].z());
      //vcl_cout<<point_3d_mean_pts[i]<<vcl_endl;








      out<<"Transform {  \n";
      // out<<"translation "<<point_3d_mean_pts_with_scale[i].x()<<" "<<point_3d_mean_pts_with_scale[i].y()<<" "<<point_3d_mean_pts_with_scale[i].z()<< "\n";  
      out<<"translation "<<real_scale*point_3d_for_out[i].x()<<" "<<real_scale*point_3d_for_out[i].y()<<" "<<real_scale*point_3d_for_out[i].z()<< "\n";  
      out<<"Shape { \n";
      out<<"appearance Appearance { \n";
      //out<<" material Material { diffuseColor "<<r*(j+1)/15 <<" "<<g*(13-j)/13 <<" "<<b*(12+j)/24 <<"}\n";

      out<<" material Material { diffuseColor "<<red <<" "<<green <<" "<<blue <<"}\n";


      out<<" }   \n";
      out<<"  geometry  Sphere{  \n"; 
      //out<<" radius "<< radius*(j+5)/7 << "}  \n"; 
      //out<<" radius "<< radius*scale_ << "}  \n"; 
      out<<" radius "<< radius*real_scale << "}  \n"; 
      out<<" } \n"; 
      out<<"} \n"; 

    }vcl_cout<<"#### end ####"<<vcl_endl;



    vcl_cout<<"epipole used was_ "<< epi_x_<<" "<<epi_y_<<vcl_endl;
    return true;

}






///// cross ratio calculetion

double dbcri_whole_process::cameras(int fm1, int fm2, int fm3,
                                    vcl_vector <vsol_digital_curve_2d_sptr> &dcl_,
                                    vcl_vector <vcl_vector <dbecl_episeg_sptr> > &episeglist,
                                    dbecl_epipole_sptr &epipole)
{

  vnl_double_3x3 M_in;
  M_in[0][0] = 2000.0; M_in[0][1] = 0;        M_in[0][2] = 1280.0f/2.0f;//512.0;
  M_in[1][0] = 0;        M_in[1][1] = 2000.0; M_in[1][2] = 768.0f/2.0f;//384.0;
  M_in[2][0] = 0;        M_in[2][1] = 0;      M_in[2][2] = 1;

  vnl_double_3x4 E,E1,E2,E3;  


  // get translation
  //trans_dist = 125.0; // 105mm

  vnl_double_3 epi;

  double xe=epi_x_;
  double ye=epi_y_;
  double ze=1.0f;

  epi[0]=epi_x_; epi[1]=epi_y_; epi[2]=1.0f;

  double x0,y0,x1,y1,x2,y2;
  bool flag=false;
  char ch;
  vnl_double_3 T;






  vnl_double_3x4 C;

  C[0][0]= 2.71954;    C[0][1]= 12.1025;    C[0][2]=0;         C[0][3]=63.9305;
  C[1][0]=-1.00509;    C[1][1]= 0.757923;   C[1][2]=-11.6824;  C[1][3]= 159.883;
  C[2][0]=-0.00516136; C[2][1]= 0.0026911;  C[2][2]= 0;        C[2][3]= 0.33245;


  C=WC_;

  //vcl_cout<<C<<vcl_endl;
  vnl_double_3x3 M = C.extract(3,3);

  //T = vnl_inverse(M_in) * epi;
  T = vnl_inverse(M) * epi;
  double T_normal=vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
  T /= T_normal;
  //T *= trans_dist;



  E1[0][0] = 1;   E1[0][1] = 0;    E1[0][2] = 0;      E1[0][3] = 0;
  E1[1][0] = 0;   E1[1][1] = 1;    E1[1][2] = 0;      E1[1][3] = 0;
  E1[2][0] = 0;   E1[2][1] = 0;    E1[2][2] = 1;      E1[2][3] = 0;

  E3[0][0] = 1;   E3[0][1] = 0;    E3[0][2] = 0;      E3[0][3] = T[0];
  E3[1][0] = 0;   E3[1][1] = 1;    E3[1][2] = 0;      E3[1][3] = T[1];
  E3[2][0] = 0;   E3[2][1] = 0;    E3[2][2] = 1;      E3[2][3] = T[2];


  vnl_double_3x4 P1 = M_in*E1,/*P2 =M_in_*E2,*/ P3=M_in*E3;





  double ratio;
  vnl_double_3 ratio_3;
  vcl_vector <ratios> ratios_front_list;
  vcl_vector <ratios> ratios_rear_list;

  bool selected0,selected1,selected2;
  double distance0,distance1,distance2;

  vcl_vector <vgl_point_3d<double> > point_3d_front_list, point_3d_rear_list;
  dbecl_episeg_point  x0pt,x1pt,x2pt;
  //dbecl_episeg_point  x0pt(episeglist[fm1][i0], episeglist[fm1][i0]->index(ang));


  // 0.01 radian corres
  //for (double ang=theta_neg_ + .01; ang<theta_pos_ - .01; ang+=.01) { 

  ////angle goes clockwise..

  int angle_index=-1;

  //4-21-07
  angle_modify();
  for (double ang=-theta_pos_-MARGIN_ ; ang<-theta_neg_+MARGIN_ ; ang+=ang_ /*was .002*/) { 

    angle_index++;
    selected0=false; 
    selected1=false;
    selected2=false;
    x0=-100.0f;x1=-100.0f;x2=-100.f;
    //vcl_cout<<ang<<" "<<episeglist[fm1].size()<<" "<<episeglist[fm2].size()<<" "<<episeglist[fm3].size()<<vcl_endl;

    for (unsigned i0=0;i0<episeglist[fm1].size();i0++) {

      //vcl_cout<<episeglist[fm1][i0]->min_angle()<<" "<<episeglist[fm1][i0]->max_angle()<<vcl_endl;
      if (episeglist[fm1][i0]->min_angle() <=ang&&episeglist[fm1][i0]->max_angle() >=ang){
        //if (episeglist[fm1][i0]->index(ang)) {

        if (episeglist[fm1][i0]->dist(ang)>x0){
          x0=episeglist[fm1][i0]->dist(ang);
          selected0=true;
          x0pt.set_point(episeglist[fm1][i0],episeglist[fm1][i0]->index(ang));
        }
      }
    }

    for (unsigned i1=0;i1<episeglist[fm2].size();i1++) {
      //vcl_cout<<episeglist[fm2][i1]->min_angle()<<" "<<episeglist[fm2][i1]->max_angle()<<vcl_endl;
      if (episeglist[fm2][i1]->min_angle() <=ang&&episeglist[fm2][i1]->max_angle() >=ang) {
        //if (episeglist[fm2][i1]->index(ang)) {
        if (episeglist[fm2][i1]->dist(ang)>x1) {
          x1=episeglist[fm2][i1]->dist(ang);
          selected1=true;
          x1pt.set_point(episeglist[fm2][i1],episeglist[fm2][i1]->index(ang));
        }
      }
    }

    for (unsigned i2=0;i2<episeglist[fm3].size();i2++) {
      //vcl_cout<<episeglist[fm3][i2]->min_angle()<<" "<<episeglist[fm3][i2]->max_angle()<<vcl_endl;
      if (episeglist[fm3][i2]->min_angle() <=ang&&episeglist[fm3][i2]->max_angle() >=ang) {
        //if (episeglist[fm3][i2]->index(ang)) {
        if (episeglist[fm3][i2]->dist(ang)>x2) {
          x2=episeglist[fm3][i2]->dist(ang);
          selected2=true;
          x2pt.set_point(episeglist[fm3][i2],episeglist[fm3][i2]->index(ang));

          ///////////////debug
          //vcl_cout<<episeglist[fm3][i2]->min_angle()<<" "<<episeglist[fm3][i2]->max_angle()<<vcl_endl;
          //vcl_cout<<episeglist[fm3][i2]<<" "<<episeglist[fm3][i2]->index(ang)<<vcl_endl;
        }
      }
    }

    // if 3 points are not determined continue
    if (!selected0 || !selected1 || !selected2) continue;

    /////////////////////////
    /////////////////////////
    ///////////////////////// angle flag



    /////////////////////////
    /////////////////////////
    /////////////////////////

    // double c=vcl_sqrt(vcl_pow(x2-xe,2)+vcl_pow(y2-ye,2));
    // double b=vcl_sqrt(vcl_pow(x1-xe,2)+vcl_pow(y1-ye,2));
    // double a=vcl_sqrt(vcl_pow(x0-xe,2)+vcl_pow(y0-ye,2));
    double c=x2; double  b=x1;double a=x0;

    double cx=vcl_fabs(x2-xe),cy=vcl_fabs(y2-ye);
    double bx=vcl_fabs(x1-xe),by=vcl_fabs(y1-ye);
    double ax=vcl_fabs(x0-xe),ay=vcl_fabs(y0-ye);

    ratio=(b-a)/(c-a)*c/b;

    if (DEBUG) vcl_cout<<" camera ratio is: "<< ang <<" "<<ratio<<vcl_endl;
    /////  vcl_cout<< ang <<" "<<ratio<<vcl_endl;



    /// dont have camera yet.. 3d reconstruction is wrong..
    vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt.pt(), P1, x2pt.pt(), P3);

    //// 9-21-2006 commented out ///// vcl_cout<<"contour point:"<<point_3d<<vcl_endl;
    point_3d_front_list.push_back(point_3d);
    ratios temp_ratios;
    temp_ratios.angle=ang;
    temp_ratios.angle_index=angle_index;
    temp_ratios.ratio=ratio;
    temp_ratios.pt3d=point_3d;
    temp_ratios.pt1=x0pt.pt();
    temp_ratios.pt2=x1pt.pt();
    temp_ratios.pt3=x2pt.pt();


    temp_ratios.fm1=x0;
    temp_ratios.fm2=x1;
    temp_ratios.fm3=x2;

    ratios_front_list.push_back(temp_ratios);

  }




  //for left side curve (rear of car)

  angle_index=-1;

  //4-21-07
  angle_modify();
  for (double ang=-theta_pos_ -MARGIN_; ang<-theta_neg_+MARGIN_ ; ang+=ang_ /*was .002*/) { ////angle goes clockwise..
    angle_index++;
    selected0=false; 
    selected1=false;
    selected2=false;
    x0=10000.0f;x1=10000.0f;x2=10000.f;
    //vcl_cout<<ang<<" "<<episeglist[fm1].size()<<" "<<episeglist[fm2].size()<<" "<<episeglist[fm3].size()<<vcl_endl;

    for (unsigned i0=0;i0<episeglist[fm1].size();i0++) {

      //vcl_cout<<episeglist[fm1][i0]->min_angle()<<" "<<episeglist[fm1][i0]->max_angle()<<vcl_endl;
      if (episeglist[fm1][i0]->min_angle() <=ang&&episeglist[fm1][i0]->max_angle() >=ang) {
        //if (episeglist[fm1][i0]->index(ang)) {

        if (episeglist[fm1][i0]->dist(ang)<x0) {
          x0=episeglist[fm1][i0]->dist(ang);
          selected0=true;

          x0pt.set_point(episeglist[fm1][i0],episeglist[fm1][i0]->index(ang));
        }

      }
    }

    for (unsigned i1=0;i1<episeglist[fm2].size();i1++) {
      //vcl_cout<<episeglist[fm2][i1]->min_angle()<<" "<<episeglist[fm2][i1]->max_angle()<<vcl_endl;
      if (episeglist[fm2][i1]->min_angle() <=ang&&episeglist[fm2][i1]->max_angle() >=ang) {
        //if (episeglist[fm2][i1]->index(ang)) {
        if (episeglist[fm2][i1]->dist(ang)<x1) {
          x1=episeglist[fm2][i1]->dist(ang);
          selected1=true;
          x1pt.set_point(episeglist[fm2][i1],episeglist[fm2][i1]->index(ang));
        }

      }
    }

    for (unsigned i2=0;i2<episeglist[fm3].size();i2++) {
      //vcl_cout<<episeglist[fm3][i2]->min_angle()<<" "<<episeglist[fm3][i2]->max_angle()<<vcl_endl;
      if (episeglist[fm3][i2]->min_angle() <=ang&&episeglist[fm3][i2]->max_angle() >=ang) {
        //if (episeglist[fm3][i2]->index(ang)) {
        if (episeglist[fm3][i2]->dist(ang)<x2) {
          x2=episeglist[fm3][i2]->dist(ang);
          selected2=true;
          x2pt.set_point(episeglist[fm3][i2],episeglist[fm3][i2]->index(ang));
        }
      }
    }

    // if 3 points are not determined continue
    if (!selected0 || !selected1 || !selected2) continue;

    // double c=vcl_sqrt(vcl_pow(x2-xe,2)+vcl_pow(y2-ye,2));
    // double b=vcl_sqrt(vcl_pow(x1-xe,2)+vcl_pow(y1-ye,2));
    // double a=vcl_sqrt(vcl_pow(x0-xe,2)+vcl_pow(y0-ye,2));
    double c=x2; double  b=x1;double a=x0;

    double cx=vcl_fabs(x2-xe),cy=vcl_fabs(y2-ye);
    double bx=vcl_fabs(x1-xe),by=vcl_fabs(y1-ye);
    double ax=vcl_fabs(x0-xe),ay=vcl_fabs(y0-ye);

    ratio=(b-a)/(c-a)*c/b;

    if (DEBUG) vcl_cout<<" camera ratio is: "<< ang <<" "<<ratio<<vcl_endl;
    //vcl_cout<< ang <<" "<<ratio<<vcl_endl;



    vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt.pt(), P1, x2pt.pt(), P3);
    //vcl_cout<<point_3d<<vcl_endl;
    //vcl_cout<<"1 vs 3: "<<point_3d<<vcl_endl;
    point_3d_rear_list.push_back(point_3d);

    ratios temp_ratios;
    temp_ratios.angle=ang;

    temp_ratios.angle_index=angle_index;
    temp_ratios.ratio=ratio;
    temp_ratios.pt3d=point_3d;

    temp_ratios.pt1=x0pt.pt();
    temp_ratios.pt2=x1pt.pt();
    temp_ratios.pt3=x2pt.pt();

    temp_ratios.fm1=x0;
    temp_ratios.fm2=x1;
    temp_ratios.fm3=x2;

    ratios_rear_list.push_back(temp_ratios);
  }






  E2[0][0] = 1;  E2[0][1] = 0;   E2[0][2] = 0;   E2[0][3] =ratio*T[0];
  E2[1][0] = 0;  E2[1][1] = 1;   E2[1][2] = 0;   E2[1][3] =ratio*T[1];
  E2[2][0] = 0;  E2[2][1] = 0;   E2[2][2] = 1;   E2[2][3] =ratio*T[2];

  //vnl_double_3x4 P1 = M_in_*E1,P2 =M_in_*E2, P3=M_in_*E3;
  // vnl_double_3x4 P1 = M_in*E1,P2 =M_in*E2, P3=M_in*E3;



  //for ( vcl_map<int,vnl_double_3x4>::const_iterator C_itr = cameras_.begin();
  //  C_itr != cameras_.end();  ++C_itr )
  //vcl_cout << "Camera " << C_itr->first << "\n" << C_itr->second << vcl_endl;


  double r=histogram_method_for_finding_cross_ratio(ratios_front_list,ratios_rear_list);


  for (unsigned j=0;j<ratios_front_list.size();j++) {


    if (ratios_front_list[j].label==false) {

      angle_f_flag_[ratios_front_list[j].angle_index]=false;

    }


  }

  for (unsigned j=0;j<ratios_rear_list.size();j++) {



    if (ratios_rear_list[j].label==false) {

      angle_r_flag_[ratios_rear_list[j].angle_index]=false;

    }


  }



  //write_vrml("front.wrl",point_3d_front_list);
  //write_vrml("rear.wrl",point_3d_rear_list);
  //write_vrml("front.wrl",point_3d_front_list);

  return r;
}


///// accurate.....cross ratio calculetion

double dbcri_whole_process::cameras_accu(int fm1, int fm2, int fm3,
                                         vcl_vector <vcl_vector <vsol_digital_curve_2d_sptr> >&dcl_edge_,
                                         vcl_vector <vcl_vector <dbecl_episeg_sptr> > &episeglist_edge_,
                                         dbecl_epipole_sptr &epipole, double r_from_contour,double tiny_number)
{

  vnl_double_3x3 M_in;
  M_in[0][0] = 2000.0; M_in[0][1] = 0;        M_in[0][2] = 1280.0f/2.0f;//512.0;
  M_in[1][0] = 0;        M_in[1][1] = 2000.0; M_in[1][2] = 768.0f/2.0f;//384.0;
  M_in[2][0] = 0;        M_in[2][1] = 0;      M_in[2][2] = 1;

  vnl_double_3x4 E,E1,E2,E3;  


  // get translation
  //trans_dist = 125.0; // 105mm

  vnl_double_3 epi;

  double xe=epi_x_;
  double ye=epi_y_;
  double ze=1.0f;

  epi[0]=epi_x_; epi[1]=epi_y_; epi[2]=1.0f;

  double x0,y0,x1,y1,x2,y2;
  bool flag=false;
  char ch;
  vnl_double_3 T;






  vnl_double_3x4 C;

  C[0][0]= 2.71954;    C[0][1]= 12.1025;    C[0][2]=0;         C[0][3]=63.9305;
  C[1][0]=-1.00509;    C[1][1]= 0.757923;   C[1][2]=-11.6824;  C[1][3]= 159.883;
  C[2][0]=-0.00516136; C[2][1]= 0.0026911;  C[2][2]= 0;        C[2][3]= 0.33245;


  C=WC_;

  //vcl_cout<<C<<vcl_endl;
  vnl_double_3x3 M = C.extract(3,3);

  //T = vnl_inverse(M_in) * epi;
  T = vnl_inverse(M) * epi;
  double T_normal=vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
  T /= T_normal;
  //T *= trans_dist;



  E1[0][0] = 1;   E1[0][1] = 0;    E1[0][2] = 0;      E1[0][3] = 0;
  E1[1][0] = 0;   E1[1][1] = 1;    E1[1][2] = 0;      E1[1][3] = 0;
  E1[2][0] = 0;   E1[2][1] = 0;    E1[2][2] = 1;      E1[2][3] = 0;

  E3[0][0] = 1;   E3[0][1] = 0;    E3[0][2] = 0;      E3[0][3] = T[0];
  E3[1][0] = 0;   E3[1][1] = 1;    E3[1][2] = 0;      E3[1][3] = T[1];
  E3[2][0] = 0;   E3[2][1] = 0;    E3[2][2] = 1;      E3[2][3] = T[2];


  vnl_double_3x4 P1 = M_in*E1,/*P2 =M_in_*E2,*/ P3=M_in*E3;




  good_ratios_edge net;
  double ratio;
  vnl_double_3 ratio_3;
  vcl_vector <ratios> ratios_front_list;


  //vcl_vector <ratios> ratios_rear_list;

  bool selected0,selected1,selected2;
  double distance0,distance1,distance2;

  vcl_vector <vgl_point_3d<double> > point_3d_front_list, point_3d_rear_list;
  dbecl_episeg_point  x0pt,x1pt,x2pt;
  //dbecl_episeg_point  x0pt(episeglist[fm1][i0], episeglist[fm1][i0]->index(ang));


  // 0.01 radian corres
  //for (double ang=theta_neg_ + .01; ang<theta_pos_ - .01; ang+=.01) { 
  int angle_index=-1;
  //  double tiny_number=.1;


  //4-21-07
  angle_modify();
  for (double ang=-theta_pos_-MARGIN_ ; ang<-theta_neg_+MARGIN_ ; ang+=ang_ /*was .002*/) { ////angle goes clockwise..

    //vcl_vector <vcl_vector <double> >network_vectvect;
    angle_index++;
    selected0=false; 
    selected1=false;
    selected2=false;
    x0=-100.0f;x1=-100.0f;x2=-100.f;
    //vcl_cout<<ang<<" "<<episeglist[fm1].size()<<" "<<episeglist[fm2].size()<<" "<<episeglist[fm3].size()<<vcl_endl;

    for (unsigned i0=0;i0<episeglist_edge_[fm1].size();i0++) {
      selected0=false;

      //vcl_cout<<episeglist_edge_[fm1][i0]->min_angle()<<" "<<episeglist_edge_[fm1][i0]->max_angle()<<vcl_endl;
      if (episeglist_edge_[fm1][i0]->min_angle() <=ang&&episeglist_edge_[fm1][i0]->max_angle() >=ang){

        if (episeglist_edge_[fm1][i0]->dist(ang)){
          x0=episeglist_edge_[fm1][i0]->dist(ang);
          selected0=true;
          x0pt.set_point(episeglist_edge_[fm1][i0],episeglist_edge_[fm1][i0]->index(ang));
        }

      }
      else continue;



      for (unsigned i1=0;i1<episeglist_edge_[fm2].size();i1++) {
        selected1=false;
        //vcl_cout<<episeglist_edge_[fm2][i1]->min_angle()<<" "<<episeglist_edge_[fm2][i1]->max_angle()<<vcl_endl;
        if (episeglist_edge_[fm2][i1]->min_angle() <=ang&&episeglist_edge_[fm2][i1]->max_angle() >=ang) {
          //if (episeglist_edge_[fm2][i1]->index(ang)) {
          if (episeglist_edge_[fm2][i1]->dist(ang)) {
            x1=episeglist_edge_[fm2][i1]->dist(ang);
            selected1=true;
            x1pt.set_point(episeglist_edge_[fm2][i1],episeglist_edge_[fm2][i1]->index(ang));
          }
        }
        else continue;


        for (unsigned i2=0;i2<episeglist_edge_[fm3].size();i2++) {
          selected2=false;
          //vcl_cout<<episeglist_edge_[fm3][i2]->min_angle()<<" "<<episeglist_edge_[fm3][i2]->max_angle()<<vcl_endl;
          if (episeglist_edge_[fm3][i2]->min_angle() <=ang&&episeglist_edge_[fm3][i2]->max_angle() >=ang) {
            //if (episeglist_edge_[fm3][i2]->index(ang)) {
            if (episeglist_edge_[fm3][i2]->dist(ang)) {
              x2=episeglist_edge_[fm3][i2]->dist(ang);
              selected2=true;
              x2pt.set_point(episeglist_edge_[fm3][i2],episeglist_edge_[fm3][i2]->index(ang));

              ///////////////debug
              //vcl_cout<<episeglist_edge_[fm3][i2]->min_angle()<<" "<<episeglist_edge_[fm3][i2]->max_angle()<<vcl_endl;
              //vcl_cout<<episeglist_edge_[fm3][i2]<<" "<<episeglist_edge_[fm3][i2]->index(ang)<<vcl_endl;
            }
          }
          else continue;



          // if 3 points are not determined continue
          if (!selected0 || !selected1 || !selected2) continue;


          double c=x2; double  b=x1;double a=x0;

          double cx=vcl_fabs(x2-xe),cy=vcl_fabs(y2-ye);
          double bx=vcl_fabs(x1-xe),by=vcl_fabs(y1-ye);
          double ax=vcl_fabs(x0-xe),ay=vcl_fabs(y0-ye);

          ratio=(b-a)/(c-a)*c/b;

          //..//vcl_cout<<ratio-r_from_contour<<vcl_endl;
          if ( vcl_fabs(ratio-r_from_contour) < tiny_number) {

            vcl_vector <double> net_vector;

            //////****************///////
            net_vector.push_back(ang);
            //////****************///////


            //////****************///////
            net_vector.push_back(x0pt.pt().x());
            net_vector.push_back(x0pt.pt().y());
            net_vector.push_back(x1pt.pt().x());
            net_vector.push_back(x1pt.pt().y());
            net_vector.push_back(x2pt.pt().x());
            net_vector.push_back(x2pt.pt().y());
            net_vector.push_back(ratio);
            net.horizon.push_back(net_vector);
            //////****************///////

            vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt.pt(), P1, x2pt.pt(), P3);

            // vcl_cout<<"3d point from edge element:"<<point_3d<<vcl_endl;
            point_3d_front_list.push_back(point_3d);
            ratios temp_ratios;
            temp_ratios.angle=ang;
            temp_ratios.angle_index=angle_index;
            temp_ratios.ratio=ratio;
            temp_ratios.pt3d=point_3d;
            temp_ratios.pt1=x0pt.pt();
            temp_ratios.pt2=x1pt.pt();
            temp_ratios.pt3=x2pt.pt();


            temp_ratios.fm1=fm1;
            temp_ratios.fm2=fm2;
            temp_ratios.fm3=fm3;

            ratios_front_list.push_back(temp_ratios);
            //vcl_cout<<"[x0~x2pt.pt()]: "<<x0pt.pt()<<" "<<x1pt.pt()<<" "<<x2pt.pt()<<vcl_endl;

          }
          else continue;

          if (DEBUG) vcl_cout<<" camera ratio is: "<< ang <<" "<<ratio<<vcl_endl;
          /////  vcl_cout<< ang <<" "<<ratio<<vcl_endl;

        } //fm3


      } //fm2
    } //fm1
  } // angle



  net.fm1=fm1;
  net.fm2=fm2;
  net.fm3=fm3;


  ///i dont need it yet
  ////////
  ////network_.push_back(net);
  ///////////


  vcl_cout<<fm1<<" "<<fm2<<" "<<fm3<<" "<<ratios_front_list.size()<<vcl_endl;
  double r=histogram_method_for_finding_cross_ratio_finer(ratios_front_list);


  for (unsigned j=0;j<ratios_front_list.size();j++) {


    if (ratios_front_list[j].label==false) {

      angle_f_flag_[ratios_front_list[j].angle_index]=false;

    }


  }




  //write_vrml("front.wrl",point_3d_front_list);
  //write_vrml("rear.wrl",point_3d_rear_list);
  //write_vrml("front.wrl",point_3d_front_list);
  ratios_front_list.clear();
  return r;
}




///// cross ratio calculetion

double dbcri_whole_process::cameras_fast(int fm1, int fm2, int fm3,
                                         vcl_vector <vsol_digital_curve_2d_sptr> &dcl_,
                                         vcl_vector <vcl_vector <dbecl_episeg_sptr> > &episeglist,
                                         dbecl_epipole_sptr &epipole)
{

  vnl_double_3x3 M_in;
  M_in[0][0] = 2000.0; M_in[0][1] = 0;        M_in[0][2] = 1280.0f/2.0f;//512.0;
  M_in[1][0] = 0;        M_in[1][1] = 2000.0; M_in[1][2] = 768.0f/2.0f;//384.0;
  M_in[2][0] = 0;        M_in[2][1] = 0;      M_in[2][2] = 1;

  vnl_double_3x4 E,E1,E2,E3;  


  // get translation
  //trans_dist = 125.0; // 105mm

  vnl_double_3 epi;

  double xe=epi_x_;
  double ye=epi_y_;
  double ze=1.0f;

  epi[0]=epi_x_; epi[1]=epi_y_; epi[2]=1.0f;

  double x0,y0,x1,y1,x2,y2;
  bool flag=false;
  char ch;
  vnl_double_3 T;


  vnl_double_3x4 C;

  C[0][0]= 2.71954;    C[0][1]= 12.1025;    C[0][2]=0;         C[0][3]=63.9305;
  C[1][0]=-1.00509;    C[1][1]= 0.757923;   C[1][2]=-11.6824;  C[1][3]= 159.883;
  C[2][0]=-0.00516136; C[2][1]= 0.0026911;  C[2][2]= 0;        C[2][3]= 0.33245;


  C=WC_;

  //vcl_cout<<C<<vcl_endl;
  vnl_double_3x3 M = C.extract(3,3);

  //T = vnl_inverse(M_in) * epi;
  T = vnl_inverse(M) * epi;
  double T_normal=vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
  T /= T_normal;
  //T *= trans_dist;



  E1[0][0] = 1;   E1[0][1] = 0;    E1[0][2] = 0;      E1[0][3] = 0;
  E1[1][0] = 0;   E1[1][1] = 1;    E1[1][2] = 0;      E1[1][3] = 0;
  E1[2][0] = 0;   E1[2][1] = 0;    E1[2][2] = 1;      E1[2][3] = 0;

  E3[0][0] = 1;   E3[0][1] = 0;    E3[0][2] = 0;      E3[0][3] = T[0];
  E3[1][0] = 0;   E3[1][1] = 1;    E3[1][2] = 0;      E3[1][3] = T[1];
  E3[2][0] = 0;   E3[2][1] = 0;    E3[2][2] = 1;      E3[2][3] = T[2];


  vnl_double_3x4 P1 = M_in*E1,/*P2 =M_in_*E2,*/ P3=M_in*E3;





  double ratio;
  vnl_double_3 ratio_3;
  vcl_vector <ratios> ratios_front_list;
  vcl_vector <ratios> ratios_rear_list;

  bool selected0,selected1,selected2;
  double distance0,distance1,distance2;

  vcl_vector <vgl_point_3d<double> > point_3d_front_list, point_3d_rear_list;
  dbecl_episeg_point  x0pt,x1pt,x2pt;
  //dbecl_episeg_point  x0pt(episeglist[fm1][i0], episeglist[fm1][i0]->index(ang));



  int angle_index=-1;

  //4-21-07
  angle_modify();
  for (double ang=-theta_pos_-MARGIN_ ; ang<-theta_neg_+MARGIN_ ; ang+=ang_ /*was .002*/) { 

    angle_index++;
    selected0=false; 
    selected1=false;
    selected2=false;
    x0=-100.0f;x1=-100.0f;x2=-100.f;
    //vcl_cout<<ang<<" "<<episeglist[fm1].size()<<" "<<episeglist[fm2].size()<<" "<<episeglist[fm3].size()<<vcl_endl;

    for (unsigned i0=0;i0<episeglist[fm1].size();i0++) {

      //vcl_cout<<episeglist[fm1][i0]->min_angle()<<" "<<episeglist[fm1][i0]->max_angle()<<vcl_endl;
      if (episeglist[fm1][i0]->min_angle() <=ang&&episeglist[fm1][i0]->max_angle() >=ang){
        //if (episeglist[fm1][i0]->index(ang)) {

        if (episeglist[fm1][i0]->dist(ang)>x0){
          x0=episeglist[fm1][i0]->dist(ang);
          selected0=true;
          x0pt.set_point(episeglist[fm1][i0],episeglist[fm1][i0]->index(ang));
        }
      }
    }

    for (unsigned i1=0;i1<episeglist[fm2].size();i1++) {
      //vcl_cout<<episeglist[fm2][i1]->min_angle()<<" "<<episeglist[fm2][i1]->max_angle()<<vcl_endl;
      if (episeglist[fm2][i1]->min_angle() <=ang&&episeglist[fm2][i1]->max_angle() >=ang) {
        //if (episeglist[fm2][i1]->index(ang)) {
        if (episeglist[fm2][i1]->dist(ang)>x1) {
          x1=episeglist[fm2][i1]->dist(ang);
          selected1=true;
          x1pt.set_point(episeglist[fm2][i1],episeglist[fm2][i1]->index(ang));
        }
      }
    }

    for (unsigned i2=0;i2<episeglist[fm3].size();i2++) {
      //vcl_cout<<episeglist[fm3][i2]->min_angle()<<" "<<episeglist[fm3][i2]->max_angle()<<vcl_endl;
      if (episeglist[fm3][i2]->min_angle() <=ang&&episeglist[fm3][i2]->max_angle() >=ang) {
        //if (episeglist[fm3][i2]->index(ang)) {
        if (episeglist[fm3][i2]->dist(ang)>x2) {
          x2=episeglist[fm3][i2]->dist(ang);
          selected2=true;
          x2pt.set_point(episeglist[fm3][i2],episeglist[fm3][i2]->index(ang));

          ///////////////debug
          //vcl_cout<<episeglist[fm3][i2]->min_angle()<<" "<<episeglist[fm3][i2]->max_angle()<<vcl_endl;
          //vcl_cout<<episeglist[fm3][i2]<<" "<<episeglist[fm3][i2]->index(ang)<<vcl_endl;
        }
      }
    }

    // if 3 points are not determined continue
    if (!selected0 || !selected1 || !selected2) continue;

    /////////////////////////
    /////////////////////////
    ///////////////////////// angle flag



    /////////////////////////
    /////////////////////////
    /////////////////////////

    // double c=vcl_sqrt(vcl_pow(x2-xe,2)+vcl_pow(y2-ye,2));
    // double b=vcl_sqrt(vcl_pow(x1-xe,2)+vcl_pow(y1-ye,2));
    // double a=vcl_sqrt(vcl_pow(x0-xe,2)+vcl_pow(y0-ye,2));
    double c=x2; double  b=x1;double a=x0;

    double cx=vcl_fabs(x2-xe),cy=vcl_fabs(y2-ye);
    double bx=vcl_fabs(x1-xe),by=vcl_fabs(y1-ye);
    double ax=vcl_fabs(x0-xe),ay=vcl_fabs(y0-ye);

    ratio=(b-a)/(c-a)*c/b;

    if (DEBUG) vcl_cout<<" camera ratio is: "<< ang <<" "<<ratio<<vcl_endl;
    /////  vcl_cout<< ang <<" "<<ratio<<vcl_endl;



    /// dont have camera yet.. 3d reconstruction is wrong..
    vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt.pt(), P1, x2pt.pt(), P3);

    //// 9-21-2006 commented out ///// vcl_cout<<"contour point:"<<point_3d<<vcl_endl;
    point_3d_front_list.push_back(point_3d);
    ratios temp_ratios;
    temp_ratios.angle=ang;
    temp_ratios.angle_index=angle_index;
    temp_ratios.ratio=ratio;
    temp_ratios.pt3d=point_3d;
    temp_ratios.pt1=x0pt.pt();
    temp_ratios.pt2=x1pt.pt();
    temp_ratios.pt3=x2pt.pt();


    temp_ratios.fm1=x0;
    temp_ratios.fm2=x1;
    temp_ratios.fm3=x2;

    ratios_front_list.push_back(temp_ratios);

  }




  //for left side curve (rear of car)

  angle_index=-1;

  //4-21-07
  angle_modify();
  for (double ang=-theta_pos_ -MARGIN_; ang<-theta_neg_+MARGIN_ ; ang+=ang_ /*was .002*/) { ////angle goes clockwise..
    angle_index++;
    selected0=false; 
    selected1=false;
    selected2=false;
    x0=10000.0f;x1=10000.0f;x2=10000.f;
    //vcl_cout<<ang<<" "<<episeglist[fm1].size()<<" "<<episeglist[fm2].size()<<" "<<episeglist[fm3].size()<<vcl_endl;

    for (unsigned i0=0;i0<episeglist[fm1].size();i0++) {

      //vcl_cout<<episeglist[fm1][i0]->min_angle()<<" "<<episeglist[fm1][i0]->max_angle()<<vcl_endl;
      if (episeglist[fm1][i0]->min_angle() <=ang&&episeglist[fm1][i0]->max_angle() >=ang) {
        //if (episeglist[fm1][i0]->index(ang)) {

        if (episeglist[fm1][i0]->dist(ang)<x0) {
          x0=episeglist[fm1][i0]->dist(ang);
          selected0=true;

          x0pt.set_point(episeglist[fm1][i0],episeglist[fm1][i0]->index(ang));
        }

      }
    }

    for (unsigned i1=0;i1<episeglist[fm2].size();i1++) {
      //vcl_cout<<episeglist[fm2][i1]->min_angle()<<" "<<episeglist[fm2][i1]->max_angle()<<vcl_endl;
      if (episeglist[fm2][i1]->min_angle() <=ang&&episeglist[fm2][i1]->max_angle() >=ang) {
        //if (episeglist[fm2][i1]->index(ang)) {
        if (episeglist[fm2][i1]->dist(ang)<x1) {
          x1=episeglist[fm2][i1]->dist(ang);
          selected1=true;
          x1pt.set_point(episeglist[fm2][i1],episeglist[fm2][i1]->index(ang));
        }

      }
    }

    for (unsigned i2=0;i2<episeglist[fm3].size();i2++) {
      //vcl_cout<<episeglist[fm3][i2]->min_angle()<<" "<<episeglist[fm3][i2]->max_angle()<<vcl_endl;
      if (episeglist[fm3][i2]->min_angle() <=ang&&episeglist[fm3][i2]->max_angle() >=ang) {
        //if (episeglist[fm3][i2]->index(ang)) {
        if (episeglist[fm3][i2]->dist(ang)<x2) {
          x2=episeglist[fm3][i2]->dist(ang);
          selected2=true;
          x2pt.set_point(episeglist[fm3][i2],episeglist[fm3][i2]->index(ang));
        }
      }
    }

    // if 3 points are not determined continue
    if (!selected0 || !selected1 || !selected2) continue;

    // double c=vcl_sqrt(vcl_pow(x2-xe,2)+vcl_pow(y2-ye,2));
    // double b=vcl_sqrt(vcl_pow(x1-xe,2)+vcl_pow(y1-ye,2));
    // double a=vcl_sqrt(vcl_pow(x0-xe,2)+vcl_pow(y0-ye,2));
    double c=x2; double  b=x1;double a=x0;

    double cx=vcl_fabs(x2-xe),cy=vcl_fabs(y2-ye);
    double bx=vcl_fabs(x1-xe),by=vcl_fabs(y1-ye);
    double ax=vcl_fabs(x0-xe),ay=vcl_fabs(y0-ye);

    ratio=(b-a)/(c-a)*c/b;

    if (DEBUG) vcl_cout<<" camera ratio is: "<< ang <<" "<<ratio<<vcl_endl;
    //vcl_cout<< ang <<" "<<ratio<<vcl_endl;



    vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt.pt(), P1, x2pt.pt(), P3);
    //vcl_cout<<point_3d<<vcl_endl;
    //vcl_cout<<"1 vs 3: "<<point_3d<<vcl_endl;
    point_3d_rear_list.push_back(point_3d);

    ratios temp_ratios;
    temp_ratios.angle=ang;

    temp_ratios.angle_index=angle_index;
    temp_ratios.ratio=ratio;
    temp_ratios.pt3d=point_3d;

    temp_ratios.pt1=x0pt.pt();
    temp_ratios.pt2=x1pt.pt();
    temp_ratios.pt3=x2pt.pt();

    temp_ratios.fm1=x0;
    temp_ratios.fm2=x1;
    temp_ratios.fm3=x2;

    ratios_rear_list.push_back(temp_ratios);
  }






  E2[0][0] = 1;  E2[0][1] = 0;   E2[0][2] = 0;   E2[0][3] =ratio*T[0];
  E2[1][0] = 0;  E2[1][1] = 1;   E2[1][2] = 0;   E2[1][3] =ratio*T[1];
  E2[2][0] = 0;  E2[2][1] = 0;   E2[2][2] = 1;   E2[2][3] =ratio*T[2];

  //vnl_double_3x4 P1 = M_in_*E1,P2 =M_in_*E2, P3=M_in_*E3;
  // vnl_double_3x4 P1 = M_in*E1,P2 =M_in*E2, P3=M_in*E3;



  //for ( vcl_map<int,vnl_double_3x4>::const_iterator C_itr = cameras_.begin();
  //  C_itr != cameras_.end();  ++C_itr )
  //vcl_cout << "Camera " << C_itr->first << "\n" << C_itr->second << vcl_endl;


  double r=histogram_method_for_finding_cross_ratio(ratios_front_list,ratios_rear_list);


  for (unsigned j=0;j<ratios_front_list.size();j++) {


    if (ratios_front_list[j].label==false) {

      angle_f_flag_[ratios_front_list[j].angle_index]=false;

    }


  }

  for (unsigned j=0;j<ratios_rear_list.size();j++) {



    if (ratios_rear_list[j].label==false) {

      angle_r_flag_[ratios_rear_list[j].angle_index]=false;

    }


  }



  //write_vrml("front.wrl",point_3d_front_list);
  //write_vrml("rear.wrl",point_3d_rear_list);
  //write_vrml("front.wrl",point_3d_front_list);

  return r;
}




///// accurate.....cross ratio calculetion

double dbcri_whole_process::cameras_accu_fast(const vcl_vector < vcl_vector <vcl_vector <vnl_float_3> > > & simple_wedge_list, int fm1, int fm2, int fm3,
                                              vcl_vector <vcl_vector <vsol_digital_curve_2d_sptr> >&dcl_edge_,
                                              vcl_vector <vcl_vector <dbecl_episeg_sptr> > &episeglist_edge_,
                                              dbecl_epipole_sptr &epipole, double r_from_contour,double tiny_number)
{

  vnl_double_3x3 M_in;
  M_in[0][0] = 2000.0; M_in[0][1] = 0;        M_in[0][2] = 1280.0f/2.0f;//512.0;
  M_in[1][0] = 0;        M_in[1][1] = 2000.0; M_in[1][2] = 768.0f/2.0f;//384.0;
  M_in[2][0] = 0;        M_in[2][1] = 0;      M_in[2][2] = 1;

  vnl_double_3x4 E,E1,E2,E3;  


  // get translation
  //trans_dist = 125.0; // 105mm

  vnl_double_3 epi;

  double xe=epi_x_;
  double ye=epi_y_;
  double ze=1.0f;

  epi[0]=epi_x_; epi[1]=epi_y_; epi[2]=1.0f;

  double x0,y0,x1,y1,x2,y2;
  bool flag=false;
  char ch;
  vnl_double_3 T;

  vnl_double_3x4 C;

  C[0][0]= 2.71954;    C[0][1]= 12.1025;    C[0][2]=0;         C[0][3]=63.9305;
  C[1][0]=-1.00509;    C[1][1]= 0.757923;   C[1][2]=-11.6824;  C[1][3]= 159.883;
  C[2][0]=-0.00516136; C[2][1]= 0.0026911;  C[2][2]= 0;        C[2][3]= 0.33245;


  C=WC_;

  //vcl_cout<<C<<vcl_endl;
  vnl_double_3x3 M = C.extract(3,3);

  //T = vnl_inverse(M_in) * epi;
  T = vnl_inverse(M) * epi;
  double T_normal=vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
  T /= T_normal;
  //T *= trans_dist;



  E1[0][0] = 1;   E1[0][1] = 0;    E1[0][2] = 0;      E1[0][3] = 0;
  E1[1][0] = 0;   E1[1][1] = 1;    E1[1][2] = 0;      E1[1][3] = 0;
  E1[2][0] = 0;   E1[2][1] = 0;    E1[2][2] = 1;      E1[2][3] = 0;

  E3[0][0] = 1;   E3[0][1] = 0;    E3[0][2] = 0;      E3[0][3] = T[0];
  E3[1][0] = 0;   E3[1][1] = 1;    E3[1][2] = 0;      E3[1][3] = T[1];
  E3[2][0] = 0;   E3[2][1] = 0;    E3[2][2] = 1;      E3[2][3] = T[2];


  vnl_double_3x4 P1 = M_in*E1,/*P2 =M_in_*E2,*/ P3=M_in*E3;




  good_ratios_edge net;
  double ratio;
  vnl_double_3 ratio_3;
  vcl_vector <ratios> ratios_front_list;


  //vcl_vector <ratios> ratios_rear_list;

  bool selected0,selected1,selected2;
  double distance0,distance1,distance2;

  vcl_vector <vgl_point_3d<double> > point_3d_front_list, point_3d_rear_list;
  //dbecl_episeg_point  x0pt,x1pt,x2pt;


  vgl_point_2d<double> x0pt,x1pt,x2pt;
  //dbecl_episeg_point  x0pt(episeglist[fm1][i0], episeglist[fm1][i0]->index(ang));


  // 0.01 radian corres
  //for (double ang=theta_neg_ + .01; ang<theta_pos_ - .01; ang+=.01) { 
  int angle_index=-1;
  //  double tiny_number=.1;


  //4-21-07
  angle_modify();
  for (double ang=-theta_pos_-MARGIN_ ; ang<-theta_neg_+MARGIN_ ; ang+=ang_ /*was .002*/) { ////angle goes clockwise..

    //vcl_vector <vcl_vector <double> >network_vectvect;
    angle_index++;
    selected0=false; 
    selected1=false;
    selected2=false;
    x0=-100.0f;x1=-100.0f;x2=-100.f;
    //vcl_cout<<ang<<" "<<episeglist[fm1].size()<<" "<<episeglist[fm2].size()<<" "<<episeglist[fm3].size()<<vcl_endl;

    ////////////////////
    ////////////////////  11-30-2007
    vcl_vector <vnl_float_3> sw1=simple_wedge_list[fm1][angle_index];
    vcl_vector <vnl_float_3> sw2=simple_wedge_list[fm2][angle_index];
    vcl_vector <vnl_float_3> sw3=simple_wedge_list[fm3][angle_index];

    //vcl_cerr<<ang<< "\t"<<sw1.size()<<"\t "<<sw2.size()<<" \t"<<sw3.size()<<vcl_endl;
    if (sw1.size()==0||sw2.size()==0||sw3.size()==0) continue;

    for (unsigned i0=0;i0<sw1.size();i0++) {
      //selected0=false;

      //vcl_cout<<episeglist_edge_[fm1][i0]->min_angle()<<" "<<episeglist_edge_[fm1][i0]->max_angle()<<vcl_endl;
      // if (episeglist_edge_[fm1][i0]->min_angle() <=ang&&episeglist_edge_[fm1][i0]->max_angle() >=ang){

      x0=sw1[i0][2];//dist
      selected0=true;
      x0pt.set(sw1[i0][0],sw1[i0][1]);


      //}




      for (unsigned i1=0;i1<sw2.size();i1++) {

        selected1=true;
        x1pt.set(sw2[i1][0],sw2[i1][1]);
        x1=sw2[i1][2];

        for (unsigned i2=0;i2<sw3.size();i2++) {

          selected2=true;
          x2pt.set(sw3[i2][0],sw3[i2][1]);
          x2=sw3[i2][2];

          ///////////////debug
          //vcl_cout<<episeglist_edge_[fm3][i2]->min_angle()<<" "<<episeglist_edge_[fm3][i2]->max_angle()<<vcl_endl;
          //vcl_cout<<episeglist_edge_[fm3][i2]<<" "<<episeglist_edge_[fm3][i2]->index(ang)<<vcl_endl;




          // if 3 points are not determined continue
          if (!selected0 || !selected1 || !selected2) continue;


          double c=x2; double  b=x1;double a=x0;

          double cx=vcl_fabs(x2-xe),cy=vcl_fabs(y2-ye);
          double bx=vcl_fabs(x1-xe),by=vcl_fabs(y1-ye);
          double ax=vcl_fabs(x0-xe),ay=vcl_fabs(y0-ye);

          ratio=(b-a)/(c-a)*c/b;

          //..//vcl_cout<<ratio-r_from_contour<<vcl_endl;
          if ( vcl_fabs(ratio-r_from_contour) < tiny_number) {

            vcl_vector <double> net_vector;

            //////****************///////
            net_vector.push_back(ang);
            //////****************///////


            //////****************///////
            net_vector.push_back(x0pt.x());
            net_vector.push_back(x0pt.y());
            net_vector.push_back(x1pt.x());
            net_vector.push_back(x1pt.y());
            net_vector.push_back(x2pt.x());
            net_vector.push_back(x2pt.y());
            net_vector.push_back(ratio);
            net.horizon.push_back(net_vector);
            //////****************///////

            vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt, P1, x2pt, P3);

            // vcl_cout<<"3d point from edge element:"<<point_3d<<vcl_endl;
            point_3d_front_list.push_back(point_3d);
            ratios temp_ratios;
            temp_ratios.angle=ang;
            temp_ratios.angle_index=angle_index;
            temp_ratios.ratio=ratio;
            temp_ratios.pt3d=point_3d;
            temp_ratios.pt1=x0pt;
            temp_ratios.pt2=x1pt;
            temp_ratios.pt3=x2pt;


            temp_ratios.fm1=fm1;
            temp_ratios.fm2=fm2;
            temp_ratios.fm3=fm3;

            ratios_front_list.push_back(temp_ratios);
            //vcl_cout<<"[x0~x2pt.pt()]: "<<x0pt.pt()<<" "<<x1pt.pt()<<" "<<x2pt.pt()<<vcl_endl;

          }
          else continue;

          if (DEBUG) vcl_cout<<" camera ratio is: "<< ang <<" "<<ratio<<vcl_endl;
          /////  vcl_cout<< ang <<" "<<ratio<<vcl_endl;

        } //fm3


      } //fm2
    } //fm1
  } // angle



  net.fm1=fm1;
  net.fm2=fm2;
  net.fm3=fm3;


  ///i dont need it yet
  ////////
  ////network_.push_back(net);
  ///////////


  vcl_cout<<fm1<<" "<<fm2<<" "<<fm3<<" "<<ratios_front_list.size()<<vcl_endl;
  double r=histogram_method_for_finding_cross_ratio_finer(ratios_front_list);


  for (unsigned j=0;j<ratios_front_list.size();j++) {


    if (ratios_front_list[j].label==false) {

      angle_f_flag_[ratios_front_list[j].angle_index]=false;

    }


  }




  //write_vrml("front.wrl",point_3d_front_list);
  //write_vrml("rear.wrl",point_3d_rear_list);
  //write_vrml("front.wrl",point_3d_front_list);
  ratios_front_list.clear();
  return r;
}










double dbcri_whole_process::episeg_simplify(vcl_vector <vcl_vector <vcl_vector <vnl_float_3> > >& epi_wedge_list_N)
{

  //vcl_vector <vcl_vector <float> > epi_wedge_list;

  vnl_double_3 epi;

  double xe=epi_x_;
  double ye=epi_y_;
  double ze=1.0f;

  epi[0]=epi_x_; epi[1]=epi_y_; epi[2]=1.0f;

  double x0,y0,x1,y1,x2,y2;



  bool selected0,selected1,selected2;
  double distance0,distance1,distance2;


  // 0.01 radian corres
  //for (double ang=theta_neg_ + .01; ang<theta_pos_ - .01; ang+=.01) { 
  int angle_index=-1;

  //  double tiny_number=.1;


  //4-21-07
  angle_modify();
  for (unsigned fm1=0;fm1<episeglist_edge_.size();fm1++) {

    vcl_vector <vcl_vector <vnl_float_3> >epi_wedge_list_angle;

    for (double ang=-theta_pos_-MARGIN_ ; ang<-theta_neg_+MARGIN_ ; ang+=ang_ /*was .002*/) { ////angle goes clockwise..

      //vcl_vector <vcl_vector <double> >network_vectvect;
      angle_index++;
      selected0=false; 
      selected1=false;
      selected2=false;
      x0=-100.0f;x1=-100.0f;x2=-100.f;
      //vcl_cout<<ang<<" "<<episeglist[fm1].size()<<" "<<episeglist[fm2].size()<<" "<<episeglist[fm3].size()<<vcl_endl;

      vcl_vector <vnl_float_3> epi_wedge_point;

      for (unsigned i0=0;i0<episeglist_edge_[fm1].size();i0++) {
        selected0=false;

        //vcl_cout<<episeglist_edge_[fm1][i0]->min_angle()<<" "<<episeglist_edge_[fm1][i0]->max_angle()<<vcl_endl;
        if (episeglist_edge_[fm1][i0]->min_angle() <=ang&&episeglist_edge_[fm1][i0]->max_angle() >=ang){

          if (episeglist_edge_[fm1][i0]->dist(ang)){
            float d=episeglist_edge_[fm1][i0]->dist(ang);
            x1= episeglist_edge_[fm1][i0]->point(ang).x();
            y1=episeglist_edge_[fm1][i0]->point(ang).y();
            selected0=true;
            vnl_float_3 p(x1,y1,d);
            epi_wedge_point.push_back(p);
            //x0pt.set_point(episeglist_edge_[fm1][i0],episeglist_edge_[fm1][i0]->index(ang));
          }

        }

      } //i0

      epi_wedge_list_angle.push_back(epi_wedge_point);
    } // angle

    epi_wedge_list_N.push_back(epi_wedge_list_angle);
  }//fm


  return 0;
}










////angle goes clockwise..


//3 frames given, compute cross ratio
double dbcri_whole_process::cross_ratio(dbecl_epipole_sptr &epipole,dbecl_episeg_sptr & epi_segl1,
                                        dbecl_episeg_sptr & epi_segl2,dbecl_episeg_sptr & epi_segl3) 
{




  return 0.0;
}



//: Write cameras and points to a file in VRML 2.0 for debugging
void 
dbcri_whole_process::write_vrml(const vcl_string& filename,
                                //vcl_vector<vpgl_perspective_camera<double> >& cameras,
                                vcl_vector<vgl_point_3d<double> > world_points)
{
  vcl_ofstream os(filename.c_str());
  os << "#VRML V2.0 utf8\n\n";
  /*
  for(unsigned int i=0; i<cameras.size(); ++i){
  vnl_double_3x3 K = cameras[i].get_calibration().get_matrix();

  vnl_double_3x3 R = cameras[i].get_rotation_matrix().get_matrix().extract(3,3);
  R.set_row(1,-1.0*R.get_row(1));
  R.set_row(2,-1.0*R.get_row(2));
  vgl_point_3d<double> ctr = cameras[i].get_camera_center();
  vnl_quaternion<double> ornt(R);
  double fov = 2.0*vcl_max(vcl_atan(K[1][2]/K[1][1]), vcl_atan(K[0][2]/K[0][0]));
  os  << "Viewpoint {\n"
  << "  position    "<< ctr.x() << ' ' << ctr.y() << ' ' << ctr.z() << '\n'
  << "  orientation "<< ornt.axis() << ' '<< ornt.angle() << '\n'
  << "  fieldOfView "<< fov << '\n'
  << "  description \"Camera" << i << "\"\n"
  << "}\n";
  }
  */
  os << "Shape {\n  appearance NULL\n    geometry PointSet {\n"
    << "      color Color { color [1 0 0] }\n      coord Coordinate{\n"
    << "       point[\n";

  for(unsigned int j=0; j<world_points.size(); ++j){
    os  << world_points[j].x() << " "
      << world_points[j].y() << " "
      << world_points[j].z() << "\n";
  }
  os << "   ]\n  }\n }\n}\n";

  os.close();
}


void dbcri_whole_process::write_vrml_2(vcl_ofstream& out,//const vcl_string& filename,
                                       vcl_vector<vgl_point_3d<double> > pts_3d)
{

  //  vcl_ofstream out(filename.c_str());
  //    if(!out.is_open()){
  //      std::cerr<<"Cannot open the input file.\n";
  //      return;
  //    }



  out <<      "#VRML V2.0 utf8\n";
  out <<      "Background { \n";
  out <<      "    skyColor [ 1 1 1 ]\n";
  out <<      "    groundColor [ 1 1 1 ]\n";
  out <<      "}\n";
  out <<      "PointLight {\n";
  out <<      "    on FALSE\n";
  out <<      "    intensity 1 \n";
  out <<      "ambientIntensity 0 \n";
  out <<      "color 1 1 1 \n";
  out <<      "location 0 0 0 \n";
  out <<      "attenuation 1 0 0 \n";
  out <<      "radius 100  \n";
  out <<      "}\n";
  out <<      "Shape {\n";
  out <<      "   #make the points white\n";
  out <<      "    appearance Appearance {\n";
  out <<      "       material Material { emissiveColor 1 0 0 }\n";
  out <<      "   } \n";
  out <<      "   geometry PointSet {\n";
  out <<      "      coord Coordinate{\n";
  out <<      "         point[\n";
  out <<      "         ]\n";
  out <<      "      }\n";
  out <<      "      color Color { color [ 1 1 1 ] }\n";
  out <<      "   }\n";
  out <<      "}\n";


  float xx,yy,zz;
  float red=0,green=0,blue=1;

  double radius=0.05;
  blue=1.0;

  for (unsigned i=0;i <pts_3d.size();i++) {
    //point_3d_mean_pts[i].set(scale_*point_3d_mean_pts[i].x(),scale_*point_3d_mean_pts[i].y(),scale_*point_3d_mean_pts[i].z());
    //vcl_cout<<point_3d_mean_pts[i]<<vcl_endl;








    out<<"Transform {  \n";
    out<<"translation "<<pts_3d[i].x()<<" "<<pts_3d[i].y()<<" "<<pts_3d[i].z()<< "\n";  
    out<<"Shape { \n";
    out<<"appearance Appearance { \n";
    //out<<" material Material { diffuseColor "<<r*(j+1)/15 <<" "<<g*(13-j)/13 <<" "<<b*(12+j)/24 <<"}\n";

    if (world_color_flag_)
      out<<" material Material { diffuseColor "<<r_world_ <<" "<<g_world_ <<" "<<b_world_ <<"}\n";

    else out<<" material Material { diffuseColor "<<red <<" "<<green <<" "<<blue <<"}\n";


    out<<" }   \n";
    out<<"  geometry  Sphere{  \n"; 
    //out<<" radius "<< radius*(j+5)/7 << "}  \n"; 
    out<<" radius "<< radius*scale_ << "}  \n"; 
    out<<" } \n"; 
    out<<"} \n"; 

  }


  vcl_cout<<"#### write_vrml_2 end ####"<<vcl_endl;
}




void dbcri_whole_process::write_vrml_3(vcl_ofstream& out)
{

  //  vcl_ofstream out(filename.c_str());
  //    if(!out.is_open()){
  //      std::cerr<<"Cannot open the input file.\n";
  //      return;
  //    }



  out <<      "#VRML V2.0 utf8\n";
  out <<      "Background { \n";
  out <<      "    skyColor [ 1 1 1 ]\n";
  out <<      "    groundColor [ 1 1 1 ]\n";
  out <<      "}\n";
  out <<      "PointLight {\n";
  out <<      "    on FALSE\n";
  out <<      "    intensity 1 \n";
  out <<      "ambientIntensity 0 \n";
  out <<      "color 1 1 1 \n";
  out <<      "location 0 0 0 \n";
  out <<      "attenuation 1 0 0 \n";
  out <<      "radius 100  \n";
  out <<      "}\n";
  out <<      "Shape {\n";
  out <<      "   #make the points white\n";
  out <<      "    appearance Appearance {\n";
  out <<      "       material Material { emissiveColor 1 0 0 }\n";
  out <<      "   } \n";
  out <<      "   geometry PointSet {\n";
  out <<      "      coord Coordinate{\n";
  out <<      "         point[\n";
  out <<      "         ]\n";
  out <<      "      }\n";
  out <<      "      color Color { color [ 1 1 1 ] }\n";
  out <<      "   }\n";
  out <<      "}\n";


  float xx,yy,zz;
  float red=0,green=0,blue=1;

  double radius=0.05;
  blue=1.0;
  //vcl_vector <pts_3d_attribute>
  for (unsigned i=0;i <pts_3d_a_.size();i++) {
    //point_3d_mean_pts[i].set(scale_*point_3d_mean_pts[i].x(),scale_*point_3d_mean_pts[i].y(),scale_*point_3d_mean_pts[i].z());
    //vcl_cout<<point_3d_mean_pts[i]<<vcl_endl;


    radius=0.05;


    if (pts_3d_a_[i].usage) {
      //pts_3d_.push_back(pts_3d_a_[i].pt3d); 
      //point_counter++;

      if (pts_3d_a_[i].position==false) { //rear of vehicle
        //pts_3d_rear_.push_back(pts_3d_a_[i].pt3d);
        red=0,green=1,blue=1;
        if (pts_3d_a_[i].con_or_edge==true) { //edge 
          red=1,green=0,blue=0;
          radius=0.07;
          if (pts_3d_a_[i].type==2) // clustering..
          {
            red=1,green=0.3,blue=.2;
            radius=0.10;

          }
        }
      }
      if (pts_3d_a_[i].position==true) { //front of vehicle
        //pts_3d_front_.push_back(pts_3d_a_[i].pt3d);
        red=1,green=1,blue=0;
        if (pts_3d_a_[i].con_or_edge==true) { //edge 
          red=0,green=0,blue=1;
          radius=0.07;
          if (pts_3d_a_[i].type==2) // clustering..
          {
            red=0.2,green=0.3,blue=1;
            radius=0.10;

          }
        }
      }
    }
    else continue;





    out<<"Transform {  \n";
    out<<"translation "<<pts_3d_a_[i].pt3d.x()<<" "<<pts_3d_a_[i].pt3d.y()<<" "<<pts_3d_a_[i].pt3d.z()<< "\n";  
    out<<"Shape { \n";
    out<<"appearance Appearance { \n";
    //out<<" material Material { diffuseColor "<<r*(j+1)/15 <<" "<<g*(13-j)/13 <<" "<<b*(12+j)/24 <<"}\n";

    out<<" material Material { diffuseColor "<<red <<" "<<green <<" "<<blue <<"}\n";


    out<<" }   \n";
    out<<"  geometry  Sphere{  \n"; 
    //out<<" radius "<< radius*(j+5)/7 << "}  \n"; 
    out<<" radius "<< radius*scale_ << "}  \n"; 
    out<<" } \n"; 
    out<<"} \n"; 

  }


  vcl_cout<<"#### write_vrml_3 end ####"<<vcl_endl;
}





double 
dbcri_whole_process::histogram_method_for_finding_cross_ratio(vcl_vector <ratios >& ratios_front_list, vcl_vector < ratios >& ratios_rear_list) 
{

  //#mothod 1... find out lier
  vcl_vector<int> bin(BIN_SIZE_,0);
  for (unsigned i=0;i<ratios_front_list.size(); i++) {

    ratios_front_list[i].label=false;
    double r=ratios_front_list[i].ratio;
    if (vcl_floor(BIN_SIZE_*r)>=0&&vcl_floor(BIN_SIZE_*r)<=99)
      bin[(int) vcl_floor(BIN_SIZE_*r)]++;
    ratios_front_list[i].bin_number=vcl_floor(BIN_SIZE_*r);
    if (DEBUG) vcl_cout<< ratios_front_list[i].bin_number<<vcl_endl;
  }
  // find max bin
  int max_bin=-100;
  int max_bin_id=-100;
  double mean_r=0.0;
  int counter=0; // will be used for both front and rear..
  for (unsigned i=0;i<BIN_SIZE_;i++) {
    if (bin[i]>max_bin) {
      max_bin=bin[i];
      max_bin_id=i;
    }
  }
  // label max_bin as true
  for (unsigned i=0;i<ratios_front_list.size(); i++) {

    if (max_bin_id==ratios_front_list[i].bin_number) {
      ratios_front_list[i].label=true;
      mean_r+=ratios_front_list[i].ratio;
      counter++;


      if (counter%2==0&&display_0_frame_&&display_n_frame_)
        display_sample_front_.push_back(ratios_front_list[i]);
    }
  }



  for (unsigned i=0;i<ratios_rear_list.size(); i++) {
    ratios_rear_list[i].label=false;
    double r=ratios_rear_list[i].ratio;
    if (vcl_floor(BIN_SIZE_*r)>=0&&vcl_floor(BIN_SIZE_*r)<=99)
      bin[(int) vcl_floor(BIN_SIZE_*r)]++;
    ratios_rear_list[i].bin_number=vcl_floor(BIN_SIZE_*r);
  }
  // find max bin
  max_bin=-100;
  max_bin_id=-100;

  //int counter=0; // will be used for both front and rear..
  for (unsigned i=0;i<BIN_SIZE_;i++) {
    if (bin[i]>max_bin) {
      max_bin=bin[i];
      max_bin_id=i;
    }
  }
  // label max_bin as true
  for (unsigned i=0;i<ratios_rear_list.size(); i++) {

    if (max_bin_id==ratios_rear_list[i].bin_number) {
      ratios_rear_list[i].label=true;
      mean_r+=ratios_rear_list[i].ratio;
      counter++;


      if (counter%3==0&&display_0_frame_&&display_n_frame_)
        display_sample_rear_.push_back(ratios_rear_list[i]);
    }

  }

  //vcl_cout<<mean_r/(double)counter<<vcl_endl;


  //display_0_frame_=false; // display sample needed for 0 frame once..
  return mean_r/counter;

}



double 
dbcri_whole_process::histogram_method_for_finding_cross_ratio_finer(vcl_vector <ratios >& ratios_list) 
{


  float bin_counter=0.0;
  double max_r=-1000000.0;
  double min_r=10000000.0;
  //,max ,min
  for (unsigned i=0;i<ratios_list.size(); i++) {

    double r=ratios_list[i].ratio;
    if (r>max_r) max_r=r;
    if (r<min_r) min_r=r;
    if (display_0_frame_&&display_n_frame_)
      display_sample_front_.push_back(ratios_list[i]);


  }


  double diff_r=max_r-min_r;
  vcl_cout<<max_r<<" "<<min_r<<" "<<BIN_SIZE_/diff_r<<" "<<ratios_list.size()<<vcl_endl;

  //#mothod 1... find out lier
  vcl_vector<int> bin(BIN_SIZE_,0);
  for (unsigned i=0;i<ratios_list.size(); i++) {

    ratios_list[i].label=false;
    double r=ratios_list[i].ratio-min_r;

    //..// vcl_cout<<BIN_SIZE_*r/diff_r<<vcl_endl;
    if (vcl_floor(BIN_SIZE_*r/diff_r)>=0&&vcl_floor(BIN_SIZE_*r/diff_r)<=99)
      bin[(int) vcl_floor(BIN_SIZE_*r/diff_r)]++;
    ratios_list[i].bin_number=vcl_floor(BIN_SIZE_*r/diff_r);
    if (DEBUG) vcl_cout<< ratios_list[i].bin_number<<vcl_endl;
  }
  // find max bin
  int max_bin=-100,second_max_bin=-100;
  int max_bin_id=-100;
  double mean_r=0.0;
  int counter=0; // will be used for both front and rear..
  for (unsigned i=0;i<BIN_SIZE_;i++) {
    bin_counter+=bin[i];
    if (bin[i]>max_bin) {

      max_bin=bin[i];
      max_bin_id=i;
    }
    vcl_cout<<bin[i]<<" ";
  }

  vcl_cout<<"->all # is:"<<bin_counter<<vcl_endl;

  /////////////////////
  /////////////////////
  // distribution check
  /////////////////////
  /////////////////////


  //delete max_bin
  for (int del=-10;del<=10;del++) {
    if (max_bin_id+del<0) bin[0]=0;
    else if (max_bin_id+del>99) bin[99]=0;
    else bin[max_bin_id+del]=0;

  }
  //find second best
  for (unsigned i=0;i<BIN_SIZE_;i++) {
    if (bin[i]>second_max_bin) {

      second_max_bin=bin[i];
      //max_bin_id=i;
    }
    //
  }

  double first_minus_second=max_bin-second_max_bin;
  first_minus_second_=first_minus_second;

  /////////////////////////////////////////
  //Limiter// histogram normalization
  /////////////////////////////////////////
  ///////////////////////////////////////////
  if (first_minus_second_>3) first_minus_second_=1.0;
  else first_minus_second_=0.0;
  first_minus_second_=max_bin;  //just using best one..
  vcl_cout<<"distribution check:-->***  "<<first_minus_second_<<vcl_endl;


  bool histo_normal_flag;
  parameters()->get_value( "-histo_normal_flag" , histo_normal_flag );//int
  if (histo_normal_flag) {
    first_minus_second_=max_bin/bin_counter;  //normaization of histogram
    vcl_cout<<"normalized distribution check: "<<first_minus_second_<<vcl_endl;
  }
  ////////////////
  ////////////////
  ////////////////

  // label max_bin as true
  for (unsigned i=0;i<ratios_list.size(); i++) {

    if (max_bin_id==ratios_list[i].bin_number) {
      ratios_list[i].label=true;
      mean_r+=ratios_list[i].ratio;
      counter++;  //this counter is just for best ones..


      ////good_3d_curves_.push_back(ratios_list[i]);


    }
  }
  //


  for (unsigned i=0;i<ratios_list.size(); i++) {

    if (vcl_abs(ratios_list[i].ratio-mean_r/counter)<0.0001) {
      ratios_list[i].label=true;
      //mean_r+=ratios_list[i].ratio;
      //counter++;


      good_3d_curves_.push_back(ratios_list[i]);


    }
    ratios_list[i].label=false;////
  }



  //display_0_frame_=false; // display sample needed for 0 frame once..
  if (counter==0) return -9999.0;
  return mean_r/counter;

}


void dbcri_whole_process::init_cameras(vnl_vector <double> t,double scale)
{

  cameras_.clear();
  vnl_double_3x3 M_in;
  M_in[0][0] = 2000.0; M_in[0][1] = 0;        M_in[0][2] = 1280.0f/2.0f;//512.0;
  M_in[1][0] = 0;        M_in[1][1] = 2000.0; M_in[1][2] = 768.0f/2.0f;//384.0;
  M_in[2][0] = 0;        M_in[2][1] = 0;      M_in[2][2] = 1;

  vnl_double_3x4 E,E1,E2,E3;  


  // get translation
  //trans_dist = 125.0; // 105mm

  vnl_double_3 epi;

  double xe=epi_x_;
  double ye=epi_y_;
  double ze=1.0f;

  epi[0]=epi_x_; epi[1]=epi_y_; epi[2]=1.0f;

  double x0,y0,x1,y1,x2,y2;
  bool flag=false;
  char ch;
  vnl_double_3 T;

  T = vnl_inverse(M_in) * epi;
  double T_normal=vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
  T /= T_normal;
  //T *= trans_dist;



  ////##########################  when loading image they reversed.
  T=-T;
  T*=0.0111554;
  ////##########################  when loading image they reversed.

  E1[0][0] = 1;   E1[0][1] = 0;    E1[0][2] = 0;      E1[0][3] = 0;
  E1[1][0] = 0;   E1[1][1] = 1;    E1[1][2] = 0;      E1[1][3] = 0;
  E1[2][0] = 0;   E1[2][1] = 0;    E1[2][2] = 1;      E1[2][3] = 0;
  int  frame = 0;
  for(; frame < input_data_.size(); ++frame)
    if(!input_data_[frame].empty() && input_data_[frame][0])
      break;

  if(frame >= input_data_.size())
    return ;
  vnl_double_3x4 P1 = M_in*E1;


  //world camera 4-25-06
  // bool WORLD_CAM=false;
  if (WORLD_CAM) {
    vnl_double_3x4 C;

    //C[0][0]= 2.71954;    C[0][1]= 12.1025;    C[0][2]=0;         C[0][3]=63.9305;
    //C[1][0]=-1.00509;    C[1][1]= 0.757923;   C[1][2]=-11.6824;  C[1][3]= 159.883;
    //C[2][0]=-0.00516136; C[2][1]= 0.0026911;  C[2][2]= 0;        C[2][3]= 0.33245;
    C=WC_;
    P1=C;

    vnl_double_3x3 M = C.extract(3,3);
    //T = vnl_inverse(M_in) * epi;
    T = vnl_inverse(M) * epi;
    T_normal=vcl_sqrt(T[0]*T[0] + T[1]*T[1] + T[2]*T[2]);
    T /= T_normal;
    //T*=0.0111554;//test
    // T=-T;
    E1=C;
    //world camera


    //WC case initialize 
    C_=C;
  }

  cameras_.push_back(P1);
  vcl_cout<<"camera scale:" <<scale<<vcl_endl;
  vcl_cout<<"\n"<<P1<<vcl_endl;


  for(; (frame < input_data_.size()-1) && input_data_[frame][0]; ++frame){


    if (!WORLD_CAM)
    {
      E1[0][0] = 1;   E1[0][1] = 0;    E1[0][2] = 0;      E1[0][3] += t(frame)*T[0];
      E1[1][0] = 0;   E1[1][1] = 1;    E1[1][2] = 0;      E1[1][3] += t(frame)*T[1];
      E1[2][0] = 0;   E1[2][1] = 0;    E1[2][2] = 1;      E1[2][3] += t(frame)*T[2];
      P1 = M_in*E1;


    }

    else {
      /* E1[0][3] += t(frame)*T[0];
      E1[1][3] += t(frame)*T[1];
      E1[2][3] += t(frame)*T[2];*/

      E1[0][3] += scale*t(frame)*epi_x_;
      E1[1][3] += scale*t(frame)*epi_y_;
      E1[2][3] += scale*t(frame)*1.0;
      P1 = E1;
    }


    //   E3[0][0] = 1;   E3[0][1] = 0;    E3[0][2] = 0;      E3[0][3] = t(frame)*T[0];
    //   E3[1][0] = 0;   E3[1][1] = 1;    E3[1][2] = 0;      E3[1][3] = t(frame)*T[1];
    //   E3[2][0] = 0;   E3[2][1] = 0;    E3[2][2] = 1;      E3[2][3] = t(frame)*T[2];


    //,/*P2 =M_in_*E2,*/ P3=M_in*E3;

    //vnl_double_3x4 P1 = E1;
    cameras_.push_back(P1);
    vcl_cout<<P1<<vcl_endl;
  }


  vcl_cout<<"t: "<<t<<" T: "<<T<<vcl_endl;
  ///////////////reverse camera


  //vcl_reverse(cameras_.begin(), cameras_.end());
  //for (unsigned i=0;i<cameras_.size();i++)
  //vcl_cout<<cameras_[i]<<vcl_endl;
  ///////////////
}



void dbcri_whole_process::d3_build(vcl_vector <vsol_digital_curve_2d_sptr> &dcl_,
                                   vcl_vector< vcl_vector <dbecl_episeg_sptr> >&episegl)

{
}

void dbcri_whole_process::d3_build()

{



  // find the first frame with input
  unsigned int frame = 0;
  for(; frame < input_data_.size(); ++frame)
    if(!input_data_[frame].empty() && input_data_[frame][0])
      break;

  if(frame >= input_data_.size())
    return ;

  //vcl_vector<vil_image_view<float> > images;
  vcl_vector <vgl_point_2d <double> >pts_upper;
  vcl_vector <vgl_point_2d <double> >pts_lower;

  vcl_vector <float * > pxl;
  vcl_vector <float * > pyl;
  vcl_vector <unsigned> num;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){



    vcl_vector <double >px_tmp;
    vcl_vector <double >py_tmp;



    vidpro_vsol2D_storage_sptr frame_vsol=vidpro_vsol2D_storage_new();
    frame_vsol.vertical_cast(input_data_[frame][0]);
    double max_x=-100.0f,max_y=-100.0f,min_x=100000.0f,min_y=100000.0f;

    // parse through all the vsol classes and save curve objects only
    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = frame_vsol->all_data();
    for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
    {
      if( vsol_list[b]->cast_to_curve())
      {
        if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
        {
          for (unsigned int i=1; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
          {
            vsol_point_2d_sptr p1 = vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i-1);
            vsol_point_2d_sptr p2 = vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i);
            ////psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
            if ((float)p1->y()>max_y) {
              max_x=(float)p1->x();
              max_y=(float)p1->y();
            }
            if ((float)p1->y()<min_y) {
              min_x=(float)p1->x();
              min_y=(float)p1->y();
            }
          }
        }
      }
      else {// contour case

        if( vsol_list[b]->cast_to_region())
        {
          if( vsol_list[b]->cast_to_region()->cast_to_polygon() )
          {
            for (unsigned int i=1; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
            {
              vsol_point_2d_sptr p1 = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i-1);
              vsol_point_2d_sptr p2 = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i);
              ////psfile1.line((float)p1->x(), (float)p1->y(), (float)p2->x(), (float)p2->y());
              if ((float)p1->y()>max_y) {
                max_x=(float)p1->x();
                max_y=(float)p1->y();
              }
              if ((float)p1->y()<min_y) {
                min_x=(float)p1->x();
                min_y=(float)p1->y();
              }


              px_tmp.push_back(p1->x());
              py_tmp.push_back(p1->y());


            }
          }
        }

      }


    }



    float* xptr = new float[px_tmp.size()];
    float* yptr = new float[py_tmp.size()];

    for (unsigned j=0;j<px_tmp.size();j++) {

      xptr[j]=px_tmp[j];
      yptr[j]=py_tmp[j];
    }
    pxl.push_back(xptr);
    pyl.push_back(yptr);
    num.push_back(px_tmp.size());

  }//loop frame//



  for (unsigned ii=0;ii<num.size();ii++) {


    vcl_cout<<num[ii]<<vcl_endl;

  }



  frame = 0;
  for(; frame < input_data_.size(); ++frame)
    if(!input_data_[frame].empty() && input_data_[frame][0])
      break;

  if(frame >= input_data_.size())
    return ;




  vcl_vector <float> ptx1,ptx2,ptx3,ptx4;
  //for (double x=-3;x<0;x+=.1)
  //  for (double y=-1;y<1;y+=.1)
  //    for (double z=13;z<26;z+=1) {



  // for (double x=-30;x<30;x+=.5)
  //   for (double y=-15;y<15;y+=.5)
  //     for (double z=0;z<60;z+=2) {



  //test//

  frame=1;
  float aa=(pxl[1][1]+3.2);
  double tmp_p2l = vgl_distance_to_non_closed_polygon( pxl[frame] , pyl[frame] ,num[frame], aa,pyl[1][1] );
  //double tmp_p2p=vgl_distance(vgl_point_2d<T >const&,vgl_point_2d<T >const&);


  if (1) vcl_cout<<pxl[1][2]<<" " <<pyl[1][2]<<" "<<tmp_p2l<<vcl_endl;

  vgl_point_2d<double> pd1(634.0,329.0);
  vcl_cout<<is_inwedge(pd1,0)<<vcl_endl;

  vgl_point_2d<double> pd2(554.0,523.0);
  vcl_cout<<is_inwedge(pd2,0)<<vcl_endl;

  vgl_point_2d<double> pd3(566.0,257.0);
  vcl_cout<<is_inwedge(pd3,0)<<vcl_endl;


  vgl_point_2d<double> pd4(566.0,250.0);
  vcl_cout<<is_inwedge(pd4,0)<<vcl_endl;

  /*for (double jj=0.0;jj<1000.0;jj+=20.0)
  for (double kk=0.0;kk<700.0;kk+=20.0) {


  vgl_point_2d<double> pdjjkk(jj,kk);
  vcl_cout<<is_inwedge(pdjjkk,0)<<vcl_endl;


  }*/

  for (double x=-10;x<3;x+=.5)
    for (double y=-5;y<5;y+=.5)
      for (double z=10;z<40;z+=1) {

        //          for (double x=-4;x<-3.1;x+=1)
        //    for (double y=3;y<3.1;y+=1)
        //      for (double z=55;z<55.1;z+=1) {

        //x=-2.0; y=0; z=23;
        if (DEBUG) vcl_cout<<"=== \n"<<x<<" "<<y<<" "<<z<<"\n"<<vcl_endl;
        double value=1.0;
        frame = 0;
        for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

          vgl_point_3d<double> Xd(x,y,z); 
          vgl_point_2d<double> pd = brct_algos::projection_3d_point(Xd, cameras_[frame]);
          float tx=pd.x();
          float ty=pd.y();


          if (!is_inwedge(pd,frame)) {
            value=-1000.0f;
            break;
          }

          //double pxl[3],pyl[3];

          double tmp_p2l = vgl_distance_to_non_closed_polygon( pxl[frame] , pyl[frame] ,num[frame], tx ,ty );
          //double tmp_p2p=vgl_distance(vgl_point_2d<T >const&,vgl_point_2d<T >const&);


          if (DEBUG) vcl_cout<<frame<<" "<<tx<<" "<<ty<<" "<<tmp_p2l<<vcl_endl;


          value*=vcl_exp(-tmp_p2l*tmp_p2l/100.0);
        }
        if (value<0.00001f) continue;

        ptx1.push_back(x);
        ptx2.push_back(y);
        ptx3.push_back(z);
        // if (value<0.000000000001f) value=0.0f;
        ptx4.push_back(value);



      }//x,y,z




      //vcl_cout<<





      // pts vnl_matrix (4,10,0.0)
      float r=1,g=0,b=0;
      double  radius=2;




      //load_model_dlg.field("model type?", model_type );

      //if (!show_model_dlg.ask())
      //   return;

      vcl_ofstream out("out.wrl");
      //out<<vcl_ofstream(out_filename);

      //std::ofstream out(out_filename, std::ios_base::out | std::ios_base::app);

      if(!out.is_open()){
        std::cerr<<"Cannot open the input file.\n";
        exit(2);
      }



      out <<      "#VRML V2.0 utf8\n";
      out <<      "Background { \n";
      out <<      "    skyColor [ 1 1 1 ]\n";
      out <<      "    groundColor [ 1 1 1 ]\n";
      out <<      "}\n";
      out <<      "PointLight {\n";
      out <<      "    on FALSE\n";
      out <<      "    intensity 1 \n";
      out <<      "ambientIntensity 0 \n";
      out <<      "color 1 1 1 \n";
      out <<      "location 0 0 0 \n";
      out <<      "attenuation 1 0 0 \n";
      out <<      "radius 100  \n";
      out <<      "}\n";
      out <<      "Shape {\n";
      out <<      "   #make the points white\n";
      out <<      "    appearance Appearance {\n";
      out <<      "       material Material { emissiveColor 1 0 0 }\n";
      out <<      "   } \n";
      out <<      "   geometry PointSet {\n";
      out <<      "      coord Coordinate{\n";
      out <<      "         point[\n";
      out <<      "         ]\n";
      out <<      "      }\n";
      out <<      "      color Color { color [ 1 1 1 ] }\n";
      out <<      "   }\n";
      out <<      "}\n";


      float xx,yy,zz;
      unsigned c=0;
      //   for (unsigned c=0;c<4;c++)
      //     for (unsigned l=0;l<model_count;l++) {
      for (unsigned i=0;i< ptx1.size();i++) {    
        switch(c)
        {
        case 0 :r=1; g=0; b=0; break;
        case 1 :r=0; g=1; b=0 ; break;
        case 2 :r=0; g=0; b=1 ; break;
        case 3 :r=1; g=1; b=0 ; break;

        default:r=1; g=1; b=1 ;break; // this will never happen
        }
        //         if (feature[j][3]!=-1) {


        xx=ptx1[i];
        yy=ptx2[i];
        zz=ptx3[i];
        radius=ptx4[i];
        //double tt=radius;

        if (vnl_math_isnan(radius)) continue;
        if (radius - 0.01f <0.0) continue;

        out<<"Transform {  \n";
        out<<"translation "<<xx<<" "<<yy<<" "<<zz<< "\n";  
        out<<"Shape { \n";
        out<<"appearance Appearance { \n";
        //out<<" material Material { diffuseColor "<<r*(j+1)/15 <<" "<<g*(13-j)/13 <<" "<<b*(12+j)/24 <<"}\n";

        out<<" material Material { diffuseColor "<<r <<" "<<g <<" "<<b <<"}\n";


        out<<" }   \n";
        out<<"  geometry  Sphere{  \n"; 
        //out<<" radius "<< radius*(j+5)/7 << "}  \n"; 
        out<<" radius "<< radius << "}  \n"; 
        out<<" } \n"; 
        out<<"} \n";
      }



}



bool dbcri_whole_process::is_inwedge(vgl_point_2d<double> p, int frame) 
{



  double a1,b1,c1,a2,b2,c2;

  a1=line_upper_.a(); a2=line_lower_.a();
  b1=line_upper_.b(); b2=line_lower_.b();
  c1=line_upper_.c(); c2=line_lower_.c();
  c1+=15; c2-=15;

  // vcl_cout<<a1*p.x()+b1*p.y()+c1<<" "<<a2*p.x()+b2*p.y()+c2<<vcl_endl;
  if (a1*p.x()+b1*p.y()+c1<0&&a2*p.x()+b2*p.y()+c2>0) return true;
  // a=line_upper.b()/line_upper.a(); b=line_upper.c()/line_upper.a();
  // c=line_lower.b()/line_lower.a(); d=line_lower.c()/line_lower.a();

  return false;
}



void dbcri_whole_process::d3_build_points(double err, vcl_vector <vgl_point_3d<double> >& point_3d_mean_pts, bool clear) 
{
  vcl_cout<<"---------------------------"<<vcl_endl;
  vcl_cout<<"d3_build_points("<<err<<","<<clear<<")"<<vcl_endl;
  vcl_cout<<"pts_3d_a_.size() "<<pts_3d_a_.size()<<vcl_endl;
  vcl_cout<<"---------------------------"<<vcl_endl;

  if (clear)
    pts_3d_a_.clear();

  pts_3d_attribute p_att;
  p_att.usage=true;
  p_att.error=0.0;
  p_att.count=0;
  p_att.con_or_edge=false; //false => con
  p_att.type=0; // confile

  double x0,x1,x2;
  double ratio;
  vnl_double_3 ratio_3;
  vcl_vector <ratios> ratios_front_list;
  vcl_vector <ratios> ratios_rear_list;

  bool selected0,selected1;
  double distance0,distance1,distance2;

  vcl_vector <vgl_point_3d<double> > point_3d_front_list, point_3d_rear_list;
  // vcl_vector <vgl_point_3d<double> > point_3d_mean_pts;

  dbecl_episeg_point  x0pt,x1pt;
  //dbecl_episeg_point  x0pt(episeglist[fm1][i0], episeglist[fm1][i0]->index(ang));




  float xx,yy,zz;
  float r=1,g=0,b=0;

  double radius=0;


  // 0.01 radian corres
  //for (double ang=theta_neg_ + .01; ang<theta_pos_ - .01; ang+=.01) {
  int angle_index=-1;

  //4-21-07
  angle_modify();
  for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=ang_ /*was .002*/) { 

    p_att.angle=ang;
    p_att.position=true; //riught side(front)
    p_att.weight=(ang+theta_pos_)/(-theta_neg_+theta_pos_);




    angle_index++;
    if (ANGLE_FLAG)
      if (angle_f_flag_[angle_index]==false) continue;




    for (unsigned fm1=0;fm1<nframes_;fm1++)
      for (unsigned fm2=fm1+1;fm2<nframes_;fm2++) {

        //if (fm1==fm2) continue;
        selected0=false; 
        selected1=false;

        x0=-100.0f;x1=-100.0f;x2=-100.f;
        //vcl_cout<<ang<<" "<<episeglist[fm1].size()<<" "<<episeglist[fm2].size()<<" "<<episeglist[fm3].size()<<vcl_endl;

        for (unsigned i0=0;i0<episeglist[fm1].size();i0++) {

          //vcl_cout<<episeglist[fm1][i0]->min_angle()<<" "<<episeglist[fm1][i0]->max_angle()<<vcl_endl;
          if (episeglist[fm1][i0]->min_angle() <=ang&&episeglist[fm1][i0]->max_angle() >=ang){
            //if (episeglist[fm1][i0]->index(ang)) {

            if (episeglist[fm1][i0]->dist(ang)>x0){
              x0=episeglist[fm1][i0]->dist(ang);
              selected0=true;
              x0pt.set_point(episeglist[fm1][i0],episeglist[fm1][i0]->index(ang));

            }
          }
        }

        if (!selected0) continue;
        for (unsigned i1=0;i1<episeglist[fm2].size();i1++) {
          //vcl_cout<<episeglist[fm2][i1]->min_angle()<<" "<<episeglist[fm2][i1]->max_angle()<<vcl_endl;
          if (episeglist[fm2][i1]->min_angle() <=ang&&episeglist[fm2][i1]->max_angle() >=ang) {
            //if (episeglist[fm2][i1]->index(ang)) {
            if (episeglist[fm2][i1]->dist(ang)>x1) {
              x1=episeglist[fm2][i1]->dist(ang);
              selected1=true;
              x1pt.set_point(episeglist[fm2][i1],episeglist[fm2][i1]->index(ang));
              //vcl_cout<<episeglist[fm2][i1]<<" "<<episeglist[fm2][i1]->index(ang)<<vcl_endl;
            }
          }
        }


        // if 3 points are not determined continue
        if (!selected0 || !selected1) continue;

        vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt.pt(), cameras_[fm1], x1pt.pt(), cameras_[fm2]);

        //vcl_cout<<x0pt.pt()<<x1pt.pt()<<fm1<<" "<<fm2<<"\n"<<cameras_[fm1]<<cameras_[fm2]<<vcl_endl;
        //vcl_cout<<point_3d<<vcl_endl;

        // vgl_point_2d <double> test1pt(425,370),test2pt(502,387);
        //vgl_point_3d<double> point_3d1 = brct_algos::triangulate_3d_point( test1pt, cameras_[4],test2pt, cameras_[3]);

        //vcl_cout<<cameras_[4]<<cameras_[3]<<vcl_endl;
        //vcl_cout<<point_3d1<<vcl_endl;

        point_3d_front_list.push_back(point_3d);
        /*ratios temp_ratios;
        temp_ratios.angle=ang;
        temp_ratios.ratio=ratio;
        temp_ratios.pt3d=point_3d;
        temp_ratios.pt1=x0pt.pt();
        temp_ratios.pt2=x1pt.pt();
        temp_ratios.pt3=x2pt.pt();


        temp_ratios.fm1=x0;
        temp_ratios.fm2=x1;
        temp_ratios.fm3=x2;

        ratios_front_list.push_back(temp_ratios);*/

      } //frame//

      double error=0.0;
      double count=0.0;
      double temp_x,temp_y,temp_z;

      count=0;
      temp_x=0,temp_y=0,temp_z=0;
      for (unsigned kk=0;kk<point_3d_front_list.size();kk++)
      {
        temp_x+=(point_3d_front_list[kk].x());
        temp_y+=(point_3d_front_list[kk].y());
        temp_z+=(point_3d_front_list[kk].z());

        count++;
      }
      xx=temp_x/count;yy=temp_y/count;zz=temp_z/count;
      count=0;
      for (unsigned jj=0;jj<point_3d_front_list.size();jj++)

      {
        temp_x=(point_3d_front_list[jj].x()-xx);
        temp_y=(point_3d_front_list[jj].y()-yy);
        temp_z=(point_3d_front_list[jj].z()-zz);
        error+=temp_x*temp_x+temp_y*temp_y+temp_z*temp_z;
        count++;
      }

      radius=5;
      vcl_cout<<"f1 "<<ang<<" (error/count):"<<(error/count)<<" "<<count<<" "<<xx<<" "<<yy<<" "<<zz<<vcl_endl;

      if (error/count< err&& count>nframes_*(nframes_-1)*.25/2.0) {   



        vgl_point_3d <double> mean_point_3d(xx,yy,zz);

        p_att.pt3d=mean_point_3d;
        p_att.error=error;
        p_att.count=count;

        point_3d_mean_pts.push_back(mean_point_3d);

        pts_3d_a_.push_back(p_att);
      }




      point_3d_front_list.clear();

  }




  /// same copy for rear

  // 0.01 radian corres
  //for (double ang=theta_neg_ + .01; ang<theta_pos_ - .01; ang+=.01) { 

  angle_index=-1;

  //4-21-07
  angle_modify();
  for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=ang_ /*was .002*/) { 

    p_att.angle=ang;
    p_att.position=false; //left side(rear)
    p_att.weight=(ang+theta_pos_)/(-theta_neg_+theta_pos_);


    angle_index++;
    if (ANGLE_FLAG)
      if (angle_r_flag_[angle_index]==false) continue;

    for (unsigned fm1=0;fm1<nframes_;fm1++)
      for (unsigned fm2=fm1+1;fm2<nframes_;fm2++) {

        //if (fm1==fm2) continue;

        selected0=false; 
        selected1=false;

        x0=10000.0f;x1=10000.0f;x2=10000.f;
        //vcl_cout<<ang<<" "<<episeglist[fm1].size()<<" "<<episeglist[fm2].size()<<" "<<episeglist[fm3].size()<<vcl_endl;

        for (unsigned i0=0;i0<episeglist[fm1].size();i0++) {

          //vcl_cout<<episeglist[fm1][i0]->min_angle()<<" "<<episeglist[fm1][i0]->max_angle()<<vcl_endl;
          if (episeglist[fm1][i0]->min_angle() <=ang&&episeglist[fm1][i0]->max_angle() >=ang){
            //if (episeglist[fm1][i0]->index(ang)) {

            if (episeglist[fm1][i0]->dist(ang)<x0){
              x0=episeglist[fm1][i0]->dist(ang);
              selected0=true;
              x0pt.set_point(episeglist[fm1][i0],episeglist[fm1][i0]->index(ang));
            }
          }
        }

        for (unsigned i1=0;i1<episeglist[fm2].size();i1++) {
          //vcl_cout<<episeglist[fm2][i1]->min_angle()<<" "<<episeglist[fm2][i1]->max_angle()<<vcl_endl;
          if (episeglist[fm2][i1]->min_angle() <=ang&&episeglist[fm2][i1]->max_angle() >=ang) {
            //if (episeglist[fm2][i1]->index(ang)) {
            if (episeglist[fm2][i1]->dist(ang)<x1) {
              x1=episeglist[fm2][i1]->dist(ang);
              selected1=true;
              x1pt.set_point(episeglist[fm2][i1],episeglist[fm2][i1]->index(ang));
            }
          }
        }


        // if 3 points are not determined continue
        if (!selected0 || !selected1) continue;

        vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt.pt(), cameras_[fm1], x1pt.pt(), cameras_[fm2]);

        //vcl_cout<<point_3d<<vcl_endl;
        point_3d_rear_list.push_back(point_3d);


      } //frame//

      double error=0.0;
      double count=0.0;
      double temp_x,temp_y,temp_z;

      count=0;
      temp_x=0,temp_y=0,temp_z=0;
      for (unsigned kk=1;kk<point_3d_rear_list.size();kk++)
      {
        temp_x+=(point_3d_rear_list[kk].x());
        temp_y+=(point_3d_rear_list[kk].y());
        temp_z+=(point_3d_rear_list[kk].z());

        count++;
      }
      xx=temp_x/count;yy=temp_y/count;zz=temp_z/count;
      count=0;
      for (unsigned jj=0;jj<point_3d_rear_list.size();jj++)

      {
        temp_x=(point_3d_rear_list[jj].x()-xx);
        temp_y=(point_3d_rear_list[jj].y()-yy);
        temp_z=(point_3d_rear_list[jj].z()-zz);
        error+=temp_x*temp_x+temp_y*temp_y+temp_z*temp_z;
        count++;
      }

      radius=4;
      vcl_cout<<"r1 "<<ang<<" (error/count): " <<(error/count)<<" "<<count<<" "<<xx<<" "<<yy<<" "<<zz<<vcl_endl;

      if (error/count< err&& count>nframes_*(nframes_-1)*.25/2.0) {   



        vgl_point_3d <double> mean_point_3d(xx,yy,zz);
        point_3d_mean_pts.push_back(mean_point_3d);

        p_att.error=error;
        p_att.count=count;

        p_att.pt3d=mean_point_3d;
        pts_3d_a_.push_back(p_att);
      }






      point_3d_rear_list.clear();

  }



  return;




}


void dbcri_whole_process::d3_build_points_edge_con(double err, vcl_vector <vgl_point_3d<double> >& point_3d_mean_pts,bool clear) 
{
  if (clear)// false
    pts_3d_a_.clear();

  vcl_cout<<"---------------------------"<<vcl_endl;
  vcl_cout<<"d3_build_points_edge_con("<<err<<","<<clear<<")"<<vcl_endl;
  vcl_cout<<"pts_3d_a_.size() "<<pts_3d_a_.size()<<vcl_endl;
  vcl_cout<<"---------------------------"<<vcl_endl;
  pts_3d_attribute p_att;
  p_att.usage=true;
  p_att.error=0.0;
  p_att.count=0;
  p_att.type= 1; //edege
  ////////////////////////////////////////////////////////
  p_att.con_or_edge=true;// true == edge  ////////////////
  ////////////////////////////////////////////////////////

  double x0,x1,x2;
  double ratio;
  vnl_double_3 ratio_3;
  vcl_vector <ratios> ratios_front_list;
  vcl_vector <ratios> ratios_rear_list;

  bool selected0,selected1;
  double distance0,distance1,distance2;

  vcl_vector <vgl_point_3d<double> > point_3d_front_list, point_3d_rear_list;
  // vcl_vector <vgl_point_3d<double> > point_3d_mean_pts;

  dbecl_episeg_point  x0pt,x1pt;
  //dbecl_episeg_point  x0pt(episeglist_edge_[fm1][i0], episeglist_edge_[fm1][i0]->index(ang));




  float xx,yy,zz;
  float r=1,g=0,b=0;

  double radius=0;


  // 0.01 radian corres
  //for (double ang=theta_neg_ + .01; ang<theta_pos_ - .01; ang+=.01) {
  int angle_index=-1;


  ////angle goes clockwise..

  //4-21-07
  angle_modify();
  for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=ang_ /*was .002*/) { 

    p_att.angle=ang;
    p_att.position=true; //right side(front)
    p_att.weight=(ang+theta_pos_)/(-theta_neg_+theta_pos_);




    angle_index++;
    if (ANGLE_FLAG)
      if (angle_f_flag_[angle_index]==false) continue;




    for (unsigned fm1=0;fm1<nframes_;fm1++)
      for (unsigned fm2=fm1+1;fm2<nframes_;fm2++) {

        //if (fm1==fm2) continue;

        selected0=false; 
        selected1=false;

        x0=-100.0f;x1=-100.0f;x2=-100.f;
        //vcl_cout<<ang<<" "<<episeglist_edge_[fm1].size()<<" "<<episeglist_edge_[fm2].size()<<" "<<episeglist_edge_[fm3].size()<<vcl_endl;

        for (unsigned i0=0;i0<episeglist_edge_[fm1].size();i0++) {

          //vcl_cout<<episeglist_edge_[fm1][i0]->min_angle()<<" "<<episeglist_edge_[fm1][i0]->max_angle()<<vcl_endl;
          if (episeglist_edge_[fm1][i0]->min_angle() <=ang&&episeglist_edge_[fm1][i0]->max_angle() >=ang){
            //if (episeglist_edge_[fm1][i0]->index(ang)) {

            if (episeglist_edge_[fm1][i0]->dist(ang)>x0){
              x0=episeglist_edge_[fm1][i0]->dist(ang);
              selected0=true;
              x0pt.set_point(episeglist_edge_[fm1][i0],episeglist_edge_[fm1][i0]->index(ang));

            }
          }
        }

        for (unsigned i1=0;i1<episeglist_edge_[fm2].size();i1++) {
          //vcl_cout<<episeglist_edge_[fm2][i1]->min_angle()<<" "<<episeglist_edge_[fm2][i1]->max_angle()<<vcl_endl;
          if (episeglist_edge_[fm2][i1]->min_angle() <=ang&&episeglist_edge_[fm2][i1]->max_angle() >=ang) {
            //if (episeglist_edge_[fm2][i1]->index(ang)) {
            if (episeglist_edge_[fm2][i1]->dist(ang)>x1) {
              x1=episeglist_edge_[fm2][i1]->dist(ang);
              selected1=true;
              x1pt.set_point(episeglist_edge_[fm2][i1],episeglist_edge_[fm2][i1]->index(ang));
              //vcl_cout<<episeglist_edge_[fm2][i1]<<" "<<episeglist_edge_[fm2][i1]->index(ang)<<vcl_endl;
            }
          }
        }


        // if 3 points are not determined continue
        if (!selected0 || !selected1) continue;

        vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt.pt(), cameras_[fm1], x1pt.pt(), cameras_[fm2]);

        //vcl_cout<<x0pt.pt()<<x1pt.pt()<<fm1<<" "<<fm2<<"\n"<<cameras_[fm1]<<cameras_[fm2]<<vcl_endl;
        //vcl_cout<<point_3d<<vcl_endl;

        // vgl_point_2d <double> test1pt(425,370),test2pt(502,387);
        //vgl_point_3d<double> point_3d1 = brct_algos::triangulate_3d_point( test1pt, cameras_[4],test2pt, cameras_[3]);

        //vcl_cout<<cameras_[4]<<cameras_[3]<<vcl_endl;
        //vcl_cout<<point_3d1<<vcl_endl;

        point_3d_front_list.push_back(point_3d);

      } //frame//

      double error=0.0;
      double count=0.0;
      double temp_x,temp_y,temp_z;

      count=0;
      temp_x=0,temp_y=0,temp_z=0;
      for (unsigned kk=0;kk<point_3d_front_list.size();kk++)
      {
        temp_x+=(point_3d_front_list[kk].x());
        temp_y+=(point_3d_front_list[kk].y());
        temp_z+=(point_3d_front_list[kk].z());

        count++;
      }
      xx=temp_x/count;yy=temp_y/count;zz=temp_z/count;
      count=0;
      for (unsigned jj=0;jj<point_3d_front_list.size();jj++)

      {
        temp_x=(point_3d_front_list[jj].x()-xx);
        temp_y=(point_3d_front_list[jj].y()-yy);
        temp_z=(point_3d_front_list[jj].z()-zz);
        error+=temp_x*temp_x+temp_y*temp_y+temp_z*temp_z;
        count++;
      }

      radius=5;
      vcl_cout<<"--f--\n recon1 \t"<<"ang: "<<ang<<"count:#  "<<count<<": f2 "<<"(error/count):"<<(error/count)<<" "<<count;

      cluster C;
      if (cluster_flag_)
        if (point_3d_front_list.size()>1) {
          cluster dummy;
          dummy  =outliers(point_3d_front_list,.75*err,C);
          //vcl_cout<<" "<<C.error<<" "<<C.size<<" "<<C.mean_pt<<vcl_endl;

          vcl_cout<<"error is "<<C.error<<" "<<"C.index:"<<C.index<<" "<<vcl_endl;
          for (unsigned k=0;k<C.pts.size();k++)
            vcl_cout<<C.pts[k]<<vcl_endl;
          vcl_cout<<C.size<<" "<<C.mean_pt<<vcl_endl;
        }
        bool check1=false,check2=false;

        if (error/count< err&& count>nframes_*(nframes_-1)*.25/2.0) check1=true;
        //if (C.size>nframes_*(nframes_-1)*.5/2.0) check2=true;   

        //3-16-2008 non-linearity of spoliled # of frames.
        if (C.size>.5*nframes_*(.5*nframes_-1)/2.0) check2=true;   
        
        vcl_cout<<"nframes_*(nframes_-1)*.25/2.0: "<<nframes_*(nframes_-1)*.25/2.0<<" check1 & cheack2 "<<check1<< " "<<check2<<vcl_endl;
        
        if (check1||check2) {


          vcl_cout<< "*"<<vcl_endl;



          //only selected points
          vgl_point_3d <double> mean_point_3d(xx,yy,zz);

          p_att.pt3d=mean_point_3d;
          if (cluster_flag_&&check2&&error/count<err/4.0) {
            p_att.pt3d=mean_point_3d;

          }
          else if (cluster_flag_&&!check1&&check2) {
            p_att.pt3d=C.mean_pt;
            p_att.type=2;// cluster point..
          }
          else p_att.type=1; // just using edge(correct apprant contour)

          p_att.error=error;
          p_att.count=count;

          point_3d_mean_pts.push_back(mean_point_3d);

          int index_con=angle_check(ang,true);


          vcl_cout<<"point type: "<<p_att.type<<" "<<index_con<<vcl_endl;

          // if already exist in contour.. substitute or add
          if (index_con!=-999) //true-> front
          {
            pts_3d_a_[index_con]=p_att;
          }
          else pts_3d_a_.push_back(p_att);
        }
        else vcl_cout<< " No good pt== "<<vcl_endl;





        point_3d_front_list.clear();

  }//angle end




  /// same copy for rear

  // 0.01 radian corres
  //for (double ang=theta_neg_ + .01; ang<theta_pos_ - .01; ang+=.01) { 
  ////angle goes clockwise..
  angle_index=-1;

  //4-21-07
  angle_modify();
  for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=ang_ /*was .002*/) { 

    p_att.angle=ang;
    p_att.position=false; //left side(rear)
    p_att.weight=(ang+theta_pos_)/(-theta_neg_+theta_pos_);


    angle_index++;
    if (ANGLE_FLAG)
      if (angle_r_flag_[angle_index]==false) continue;

    for (unsigned fm1=0;fm1<nframes_;fm1++)
      for (unsigned fm2=fm1+1;fm2<nframes_;fm2++) {

        //if (fm1==fm2) continue;

        selected0=false; 
        selected1=false;

        x0=10000.0f;x1=10000.0f;x2=10000.f;
        //vcl_cout<<ang<<" "<<episeglist_edge_[fm1].size()<<" "<<episeglist_edge_[fm2].size()<<" "<<episeglist_edge_[fm3].size()<<vcl_endl;

        for (unsigned i0=0;i0<episeglist_edge_[fm1].size();i0++) {

          //vcl_cout<<episeglist_edge_[fm1][i0]->min_angle()<<" "<<episeglist_edge_[fm1][i0]->max_angle()<<vcl_endl;
          if (episeglist_edge_[fm1][i0]->min_angle() <=ang&&episeglist_edge_[fm1][i0]->max_angle() >=ang){
            //if (episeglist_edge_[fm1][i0]->index(ang)) {

            if (episeglist_edge_[fm1][i0]->dist(ang)<x0){
              x0=episeglist_edge_[fm1][i0]->dist(ang);
              selected0=true;
              x0pt.set_point(episeglist_edge_[fm1][i0],episeglist_edge_[fm1][i0]->index(ang));
            }
          }
        }

        for (unsigned i1=0;i1<episeglist_edge_[fm2].size();i1++) {
          //vcl_cout<<episeglist_edge_[fm2][i1]->min_angle()<<" "<<episeglist_edge_[fm2][i1]->max_angle()<<vcl_endl;
          if (episeglist_edge_[fm2][i1]->min_angle() <=ang&&episeglist_edge_[fm2][i1]->max_angle() >=ang) {
            //if (episeglist_edge_[fm2][i1]->index(ang)) {
            if (episeglist_edge_[fm2][i1]->dist(ang)<x1) {
              x1=episeglist_edge_[fm2][i1]->dist(ang);
              selected1=true;
              x1pt.set_point(episeglist_edge_[fm2][i1],episeglist_edge_[fm2][i1]->index(ang));
            }
          }
        }


        // if 3 points are not determined continue
        if (!selected0 || !selected1) continue;

        vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt.pt(), cameras_[fm1], x1pt.pt(), cameras_[fm2]);

        //vcl_cout<<point_3d<<vcl_endl;
        point_3d_rear_list.push_back(point_3d);


      } //frame//

      double error=0.0;
      double count=0.0;
      double temp_x,temp_y,temp_z;

      count=0;
      temp_x=0,temp_y=0,temp_z=0;
      for (unsigned kk=1;kk<point_3d_rear_list.size();kk++)
      {
        temp_x+=(point_3d_rear_list[kk].x());
        temp_y+=(point_3d_rear_list[kk].y());
        temp_z+=(point_3d_rear_list[kk].z());

        count++;
      }
      xx=temp_x/count;yy=temp_y/count;zz=temp_z/count;
      count=0;
      for (unsigned jj=0;jj<point_3d_rear_list.size();jj++)

      {
        temp_x=(point_3d_rear_list[jj].x()-xx);
        temp_y=(point_3d_rear_list[jj].y()-yy);
        temp_z=(point_3d_rear_list[jj].z()-zz);
        error+=temp_x*temp_x+temp_y*temp_y+temp_z*temp_z;
        count++;
      }

      radius=4;
      vcl_cout<<"--r--\n recon1 \t"<<"ang: "<<ang<<"count:#  "<<count<<": r2 "<<"(error/count): " <<(error/count)<<" "<<count;



      cluster C;
      if (cluster_flag_)
        if (point_3d_rear_list.size()>1) {
          cluster dummy;
          dummy  =outliers(point_3d_rear_list,0.75*err,C);
          //vcl_cout<<" "<<C.error<<" "<<C.size<<" "<<C.mean_pt<<vcl_endl;

          vcl_cout<<"error(rear) is"<<C.error<<" "<<C.index<<" ";
          for (unsigned k=0;k<C.pts.size();k++)
            vcl_cout<<C.pts[k];
          vcl_cout<<C.size<<" "<<C.mean_pt<<vcl_endl;
        }


        bool check1=false,check2=false;

              if (error/count< err&& count>nframes_*(nframes_-1)*.25/2.0) check1=true;
        //if (C.size>nframes_*(nframes_-1)*.5/2.0) check2=true;   

        //3-16-2008 non-linearity of spoliled # of frames.
        if (C.size>.5*nframes_*(.5*nframes_-1)/2.0) check2=true;   
        vcl_cout<<"nframes_*(nframes_-1)*.25/2.0: "<<nframes_*(nframes_-1)*.25/2.0<<" check1 & cheack2 "<<check1<< " "<<check2<<vcl_endl;

        if (check1||check2) {
          //if (error/count< err&& count>nframes_*(nframes_-1)*.25/2.0||C.size>nframes_*(nframes_-1)*.5/2.0-1) {   

          vcl_cout<< "*"<<vcl_endl;

          //only selected points
          vgl_point_3d <double> mean_point_3d(xx,yy,zz);
          point_3d_mean_pts.push_back(mean_point_3d);

          p_att.error=error;
          p_att.count=count;

          p_att.pt3d=mean_point_3d;
          if (cluster_flag_&&check2&&error/count<err/4.0) { ///why err? 
            p_att.pt3d=mean_point_3d;

          }
          else if (cluster_flag_&&!check1&&check2) {
            p_att.pt3d=C.mean_pt;
            p_att.type=2;// cluster point..
          }
          else p_att.type=1; // just using edge(correct apprant contour)

          int index_con=angle_check(ang,false);// here false means position rear
          vcl_cout<<"point type: "<<p_att.type<<" "<<index_con<<vcl_endl;
          vcl_cout<<"ang: "<< ang<<" "<<index_con<<vcl_endl;
          // if already exist in contour.. substitute or add
          if (index_con!=-999) //true-> front
          {
            pts_3d_a_[index_con]=p_att;
          }
          else pts_3d_a_.push_back(p_att);
        }
        else vcl_cout<< " No good pt== "<<vcl_endl;






        point_3d_rear_list.clear();

  }


  return;




}

//
//3-17-2008
// inside the car reconstruction
//
void dbcri_whole_process::d3_build_points_edge_con_inside(double err, vcl_vector <vgl_point_3d<double> >& point_3d_mean_pts,bool clear) 
{
  if (clear)// false
    pts_3d_a_.clear();

  vcl_cout<<"---------------------------"<<vcl_endl;
  vcl_cout<<"d3_build_points_edge_con_inside("<<err<<","<<clear<<")"<<vcl_endl;
  vcl_cout<<"pts_3d_a_.size() "<<pts_3d_a_.size()<<vcl_endl;
  vcl_cout<<"---------------------------"<<vcl_endl;
  pts_3d_attribute p_att;
  p_att.usage=true;
  p_att.error=0.0;
  p_att.count=0;
  p_att.type= 1; //edege
  ////////////////////////////////////////////////////////
  p_att.con_or_edge=true;// true == edge  ////////////////
  ////////////////////////////////////////////////////////

  double x0,x1,x2;
  double ratio;
  vnl_double_3 ratio_3;
  vcl_vector <ratios> ratios_front_list;
  vcl_vector <ratios> ratios_rear_list;

  bool selected0,selected1;
  double distance0,distance1,distance2;

  vcl_vector <vgl_point_3d<double> > point_3d_front_list, point_3d_rear_list;
  // vcl_vector <vgl_point_3d<double> > point_3d_mean_pts;

  dbecl_episeg_point  x0pt,x1pt;
  //dbecl_episeg_point  x0pt(episeglist_edge_[fm1][i0], episeglist_edge_[fm1][i0]->index(ang));




  float xx,yy,zz;
  float r=1,g=0,b=0;

  double radius=0;


  // 0.01 radian corres
  //for (double ang=theta_neg_ + .01; ang<theta_pos_ - .01; ang+=.01) {
  int angle_index=-1;


  ////angle goes clockwise..

  //4-21-07
  angle_modify();
  for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=ang_ /*was .002*/) { 
    p_att.angle=ang;
    p_att.position=true; //right side(front)
    p_att.weight=(ang+theta_pos_)/(-theta_neg_+theta_pos_);
    angle_index++;
    if (ANGLE_FLAG)
      if (angle_f_flag_[angle_index]==false) continue;

    for (unsigned fm1=0;fm1<nframes_;fm1++)
      for (unsigned fm2=fm1+1;fm2<nframes_;fm2++) {
        selected0=false; 
        selected1=false;

        x0=-100.0f;x1=-100.0f;x2=-100.f;
        //vcl_cout<<ang<<" "<<episeglist_edge_[fm1].size()<<" "<<episeglist_edge_[fm2].size()<<" "<<episeglist_edge_[fm3].size()<<vcl_endl;

        for (unsigned i0=0;i0<episeglist_edge_[fm1].size();i0++) {

          //vcl_cout<<episeglist_edge_[fm1][i0]->min_angle()<<" "<<episeglist_edge_[fm1][i0]->max_angle()<<vcl_endl;
          if (episeglist_edge_[fm1][i0]->min_angle() <=ang&&episeglist_edge_[fm1][i0]->max_angle() >=ang){
            //if (episeglist_edge_[fm1][i0]->index(ang)) {

            if (episeglist_edge_[fm1][i0]->dist(ang)>x0){
              x0=episeglist_edge_[fm1][i0]->dist(ang);
              selected0=true;
              x0pt.set_point(episeglist_edge_[fm1][i0],episeglist_edge_[fm1][i0]->index(ang));

            }
          }
        }

        for (unsigned i1=0;i1<episeglist_edge_[fm2].size();i1++) {
          //vcl_cout<<episeglist_edge_[fm2][i1]->min_angle()<<" "<<episeglist_edge_[fm2][i1]->max_angle()<<vcl_endl;
          if (episeglist_edge_[fm2][i1]->min_angle() <=ang&&episeglist_edge_[fm2][i1]->max_angle() >=ang) {
            //if (episeglist_edge_[fm2][i1]->index(ang)) {
            if (episeglist_edge_[fm2][i1]->dist(ang)>x1) {
              x1=episeglist_edge_[fm2][i1]->dist(ang);
              selected1=true;
              x1pt.set_point(episeglist_edge_[fm2][i1],episeglist_edge_[fm2][i1]->index(ang));
              //vcl_cout<<episeglist_edge_[fm2][i1]<<" "<<episeglist_edge_[fm2][i1]->index(ang)<<vcl_endl;
            }
          }
        }


        // if 3 points are not determined continue
        if (!selected0 || !selected1) continue;

        vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt.pt(), cameras_[fm1], x1pt.pt(), cameras_[fm2]);

        //vcl_cout<<x0pt.pt()<<x1pt.pt()<<fm1<<" "<<fm2<<"\n"<<cameras_[fm1]<<cameras_[fm2]<<vcl_endl;
        //vcl_cout<<point_3d<<vcl_endl;

        // vgl_point_2d <double> test1pt(425,370),test2pt(502,387);
        //vgl_point_3d<double> point_3d1 = brct_algos::triangulate_3d_point( test1pt, cameras_[4],test2pt, cameras_[3]);

        //vcl_cout<<cameras_[4]<<cameras_[3]<<vcl_endl;
        //vcl_cout<<point_3d1<<vcl_endl;

        point_3d_front_list.push_back(point_3d);

      } //frame//

      double error=0.0;
      double count=0.0;
      double temp_x,temp_y,temp_z;

      count=0;
      temp_x=0,temp_y=0,temp_z=0;
      for (unsigned kk=0;kk<point_3d_front_list.size();kk++)
      {
        temp_x+=(point_3d_front_list[kk].x());
        temp_y+=(point_3d_front_list[kk].y());
        temp_z+=(point_3d_front_list[kk].z());

        count++;
      }
      xx=temp_x/count;yy=temp_y/count;zz=temp_z/count;
      count=0;
      for (unsigned jj=0;jj<point_3d_front_list.size();jj++)

      {
        temp_x=(point_3d_front_list[jj].x()-xx);
        temp_y=(point_3d_front_list[jj].y()-yy);
        temp_z=(point_3d_front_list[jj].z()-zz);
        error+=temp_x*temp_x+temp_y*temp_y+temp_z*temp_z;
        count++;
      }

      radius=5;
      vcl_cout<<"--f--\n recon1 \t"<<"ang: "<<ang<<"count:#  "<<count<<": f2 "<<"(error/count):"<<(error/count)<<" "<<count;

      cluster C;
      if (cluster_flag_)
        if (point_3d_front_list.size()>1) {
          cluster dummy;
          dummy  =outliers(point_3d_front_list,.75*err,C);
          //vcl_cout<<" "<<C.error<<" "<<C.size<<" "<<C.mean_pt<<vcl_endl;

          vcl_cout<<"error is "<<C.error<<" "<<"C.index:"<<C.index<<" "<<vcl_endl;
          for (unsigned k=0;k<C.pts.size();k++)
            vcl_cout<<C.pts[k]<<vcl_endl;
          vcl_cout<<C.size<<" "<<C.mean_pt<<vcl_endl;
        }
        bool check1=false,check2=false;

        if (error/count< err&& count>nframes_*(nframes_-1)*.25/2.0) check1=true;
        //if (C.size>nframes_*(nframes_-1)*.5/2.0) check2=true;   

        //3-16-2008 non-linearity of spoliled # of frames.
        if (C.size>.5*nframes_*(.5*nframes_-1)/2.0) check2=true;   
        
        vcl_cout<<"nframes_*(nframes_-1)*.25/2.0: "<<nframes_*(nframes_-1)*.25/2.0<<" check1 & cheack2 "<<check1<< " "<<check2<<vcl_endl;
        
        if (check1||check2) {


          vcl_cout<< "*"<<vcl_endl;



          //only selected points
          vgl_point_3d <double> mean_point_3d(xx,yy,zz);

          p_att.pt3d=mean_point_3d;
          if (cluster_flag_&&check2&&error/count<err/4.0) {
            p_att.pt3d=mean_point_3d;

          }
          else if (cluster_flag_&&!check1&&check2) {
            p_att.pt3d=C.mean_pt;
            p_att.type=2;// cluster point..
          }
          else p_att.type=1; // just using edge(correct apprant contour)

          p_att.error=error;
          p_att.count=count;

          point_3d_mean_pts.push_back(mean_point_3d);

          int index_con=angle_check(ang,true);


          vcl_cout<<"point type: "<<p_att.type<<" "<<index_con<<vcl_endl;

          // if already exist in contour.. substitute or add
          if (index_con!=-999) //true-> front
          {
            pts_3d_a_[index_con]=p_att;
          }
          else pts_3d_a_.push_back(p_att);
        }
        else vcl_cout<< " No good pt== "<<vcl_endl;





        point_3d_front_list.clear();

  }//angle end




  /// same copy for rear

  // 0.01 radian corres
  //for (double ang=theta_neg_ + .01; ang<theta_pos_ - .01; ang+=.01) { 
  ////angle goes clockwise..
  angle_index=-1;

  //4-21-07
  angle_modify();
  for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=ang_ /*was .002*/) { 

    p_att.angle=ang;
    p_att.position=false; //left side(rear)
    p_att.weight=(ang+theta_pos_)/(-theta_neg_+theta_pos_);


    angle_index++;
    if (ANGLE_FLAG)
      if (angle_r_flag_[angle_index]==false) continue;

    for (unsigned fm1=0;fm1<nframes_;fm1++)
      for (unsigned fm2=fm1+1;fm2<nframes_;fm2++) {

        //if (fm1==fm2) continue;

        selected0=false; 
        selected1=false;

        x0=10000.0f;x1=10000.0f;x2=10000.f;
        //vcl_cout<<ang<<" "<<episeglist_edge_[fm1].size()<<" "<<episeglist_edge_[fm2].size()<<" "<<episeglist_edge_[fm3].size()<<vcl_endl;

        for (unsigned i0=0;i0<episeglist_edge_[fm1].size();i0++) {

          //vcl_cout<<episeglist_edge_[fm1][i0]->min_angle()<<" "<<episeglist_edge_[fm1][i0]->max_angle()<<vcl_endl;
          if (episeglist_edge_[fm1][i0]->min_angle() <=ang&&episeglist_edge_[fm1][i0]->max_angle() >=ang){
            //if (episeglist_edge_[fm1][i0]->index(ang)) {

            if (episeglist_edge_[fm1][i0]->dist(ang)<x0){
              x0=episeglist_edge_[fm1][i0]->dist(ang);
              selected0=true;
              x0pt.set_point(episeglist_edge_[fm1][i0],episeglist_edge_[fm1][i0]->index(ang));
            }
          }
        }

        for (unsigned i1=0;i1<episeglist_edge_[fm2].size();i1++) {
          //vcl_cout<<episeglist_edge_[fm2][i1]->min_angle()<<" "<<episeglist_edge_[fm2][i1]->max_angle()<<vcl_endl;
          if (episeglist_edge_[fm2][i1]->min_angle() <=ang&&episeglist_edge_[fm2][i1]->max_angle() >=ang) {
            //if (episeglist_edge_[fm2][i1]->index(ang)) {
            if (episeglist_edge_[fm2][i1]->dist(ang)<x1) {
              x1=episeglist_edge_[fm2][i1]->dist(ang);
              selected1=true;
              x1pt.set_point(episeglist_edge_[fm2][i1],episeglist_edge_[fm2][i1]->index(ang));
            }
          }
        }


        // if 3 points are not determined continue
        if (!selected0 || !selected1) continue;

        vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt.pt(), cameras_[fm1], x1pt.pt(), cameras_[fm2]);

        //vcl_cout<<point_3d<<vcl_endl;
        point_3d_rear_list.push_back(point_3d);


      } //frame//

      double error=0.0;
      double count=0.0;
      double temp_x,temp_y,temp_z;

      count=0;
      temp_x=0,temp_y=0,temp_z=0;
      for (unsigned kk=1;kk<point_3d_rear_list.size();kk++)
      {
        temp_x+=(point_3d_rear_list[kk].x());
        temp_y+=(point_3d_rear_list[kk].y());
        temp_z+=(point_3d_rear_list[kk].z());

        count++;
      }
      xx=temp_x/count;yy=temp_y/count;zz=temp_z/count;
      count=0;
      for (unsigned jj=0;jj<point_3d_rear_list.size();jj++)

      {
        temp_x=(point_3d_rear_list[jj].x()-xx);
        temp_y=(point_3d_rear_list[jj].y()-yy);
        temp_z=(point_3d_rear_list[jj].z()-zz);
        error+=temp_x*temp_x+temp_y*temp_y+temp_z*temp_z;
        count++;
      }

      radius=4;
      vcl_cout<<"--r--\n recon1 \t"<<"ang: "<<ang<<"count:#  "<<count<<": r2 "<<"(error/count): " <<(error/count)<<" "<<count;



      cluster C;
      if (cluster_flag_)
        if (point_3d_rear_list.size()>1) {
          cluster dummy;
          dummy  =outliers(point_3d_rear_list,0.75*err,C);
          //vcl_cout<<" "<<C.error<<" "<<C.size<<" "<<C.mean_pt<<vcl_endl;

          vcl_cout<<"error(rear) is"<<C.error<<" "<<C.index<<" ";
          for (unsigned k=0;k<C.pts.size();k++)
            vcl_cout<<C.pts[k];
          vcl_cout<<C.size<<" "<<C.mean_pt<<vcl_endl;
        }


        bool check1=false,check2=false;

              if (error/count< err&& count>nframes_*(nframes_-1)*.25/2.0) check1=true;
        //if (C.size>nframes_*(nframes_-1)*.5/2.0) check2=true;   

        //3-16-2008 non-linearity of spoliled # of frames.
        if (C.size>.5*nframes_*(.5*nframes_-1)/2.0) check2=true;   
        vcl_cout<<"nframes_*(nframes_-1)*.25/2.0: "<<nframes_*(nframes_-1)*.25/2.0<<" check1 & cheack2 "<<check1<< " "<<check2<<vcl_endl;

        if (check1||check2) {
          //if (error/count< err&& count>nframes_*(nframes_-1)*.25/2.0||C.size>nframes_*(nframes_-1)*.5/2.0-1) {   

          vcl_cout<< "*"<<vcl_endl;

          //only selected points
          vgl_point_3d <double> mean_point_3d(xx,yy,zz);
          point_3d_mean_pts.push_back(mean_point_3d);

          p_att.error=error;
          p_att.count=count;

          p_att.pt3d=mean_point_3d;
          if (cluster_flag_&&check2&&error/count<err/4.0) { ///why err? 
            p_att.pt3d=mean_point_3d;

          }
          else if (cluster_flag_&&!check1&&check2) {
            p_att.pt3d=C.mean_pt;
            p_att.type=2;// cluster point..
          }
          else p_att.type=1; // just using edge(correct apprant contour)

          int index_con=angle_check(ang,false);// here false means position rear
          vcl_cout<<"point type: "<<p_att.type<<" "<<index_con<<vcl_endl;
          vcl_cout<<"ang: "<< ang<<" "<<index_con<<vcl_endl;
          // if already exist in contour.. substitute or add
          if (index_con!=-999) //true-> front
          {
            pts_3d_a_[index_con]=p_att;
          }
          else pts_3d_a_.push_back(p_att);
        }
        else vcl_cout<< " No good pt== "<<vcl_endl;
        point_3d_rear_list.clear();
  }
  return;
}



int dbcri_whole_process::angle_check(double ang,bool position) {



  for (unsigned i=0;i<pts_3d_a_.size();i++) {
    if (vcl_fabs(pts_3d_a_[i].angle-ang)<0.00001 &&pts_3d_a_[i].position==position ) return i;
  }

  return -999;
}
void dbcri_whole_process::d3_build_points_edge(double err, vcl_vector <vgl_point_3d<double> >& point_3d_mean_pts) 
{
  pts_3d_a_.clear();

  /*pts_3d_attribute p_att;
  p_att.usage=true;
  p_att.error=0.0;
  p_att.count=0;





  //p_att.angle=ang;
  //  p_att.position=true; //riught side(front)
  //  p_att.weight=(ang+theta_pos_)/(-theta_neg_+theta_pos_);

  p_att.weight=.88888888888;
  */

  double x0,x1,x2;
  double ratio;
  vnl_double_3 ratio_3;
  vcl_vector <ratios> ratios_front_list;
  vcl_vector <ratios> ratios_rear_list;

  bool selected0,selected1;
  double distance0,distance1,distance2;

  vcl_vector <vgl_point_3d<double> > point_3d_front_list, point_3d_rear_list;
  // vcl_vector <vgl_point_3d<double> > point_3d_mean_pts;

  dbecl_episeg_point  x0pt,x1pt;
  //dbecl_episeg_point  x0pt(episeglist[fm1][i0], episeglist[fm1][i0]->index(ang));




  double xx,yy,zz;
  float r=1,g=0,b=0;

  double radius=0;

  vcl_cout<<"good_3d_curves_.size: "<<good_3d_curves_.size()<<vcl_endl;
  for (unsigned i=0;i<good_3d_curves_.size();i++) {






    int fm1=good_3d_curves_[i].fm1;
    int fm2=good_3d_curves_[i].fm2;

    int fm3=good_3d_curves_[i].fm3;
    vgl_point_2d <double> x0pt=good_3d_curves_[i].pt1;
    vgl_point_2d <double> x1pt=good_3d_curves_[i].pt2;
    vgl_point_2d <double> x2pt=good_3d_curves_[i].pt3;
    vgl_point_3d<double> point_3d_12 = brct_algos::triangulate_3d_point(x0pt, cameras_[fm1], x1pt ,cameras_[fm2]);
    vgl_point_3d<double> point_3d_23 = brct_algos::triangulate_3d_point(x1pt, cameras_[fm2], x2pt ,cameras_[fm3]);
    vgl_point_3d<double> point_3d_13 = brct_algos::triangulate_3d_point(x0pt, cameras_[fm1], x2pt, cameras_[fm3]);


    xx=(point_3d_12.x()+point_3d_23.x()+point_3d_13.x());
    yy=(point_3d_12.y()+point_3d_23.y()+point_3d_13.y());
    zz=(point_3d_12.z()+point_3d_23.z()+point_3d_13.z());

    // point_3d_front_list.push_back(point_3d);


    vgl_point_3d <double> mean_point_3d(xx/3.0,yy/3.0,zz/3.0);

    pts_3d_attribute p_att;
    p_att.usage=true;
    p_att.error=0.0;
    p_att.count=0;
    p_att.weight=   (good_3d_curves_[i].angle+theta_pos_)/(-theta_neg_+theta_pos_);
    p_att.position=true;///later  avoid left side 
    p_att.pt3d=mean_point_3d;
    p_att.angle= good_3d_curves_[i].angle;



    double error=0;
    double temp_x12=point_3d_12.x()-xx;double temp_x13=point_3d_13.x()-xx;double temp_x23=point_3d_23.x()-xx;
    double temp_y12=point_3d_12.y()-yy;double temp_y13=point_3d_13.y()-yy;double temp_y23=point_3d_23.y()-yy;
    double temp_z12=point_3d_12.z()-zz;double temp_z13=point_3d_13.x()-zz;double temp_z23=point_3d_23.z()-zz;

    double error_12=temp_x12*temp_x12+temp_y12*temp_y12+temp_z12*temp_z12;
    double error_13=temp_x13*temp_x13+temp_y13*temp_y13+temp_z13*temp_z13;
    double error_23=temp_x23*temp_x23+temp_y23*temp_y23+temp_z23*temp_z23;


    error=error_12+error_13+error_23;


    p_att.error=error;
    p_att.error=0;
    //p_att.count=count;
    p_att.count=3;  //3 pairs..
    //p_att.weight=.88888888888;
    pts_3d_a_.push_back(p_att);
    /*ratios temp_ratios;
    temp_ratios.angle=ang;
    temp_ratios.ratio=ratio;
    temp_ratios.pt3d=point_3d;
    temp_ratios.pt1=x0pt.pt();
    temp_ratios.pt2=x1pt.pt();
    temp_ratios.pt3=x2pt.pt();


    temp_ratios.fm1=x0;
    temp_ratios.fm2=x1;
    temp_ratios.fm3=x2;

    ratios_front_list.push_back(temp_ratios);*/

  } //frame//

  return;
}

void dbcri_whole_process::d3_build_network(double err, vcl_vector <vgl_point_3d<double> >& point_3d_mean_pts) 
{

  pts_3d_a_.clear();

  /*pts_3d_attribute p_att;
  p_att.usage=true;
  p_att.error=0.0;
  p_att.count=0;





  //p_att.angle=ang;
  //  p_att.position=true; //riught side(front)
  //  p_att.weight=(ang+theta_pos_)/(-theta_neg_+theta_pos_);

  p_att.weight=.88888888888;
  */
  /*
  double x0,x1,x2;
  double ratio;
  vnl_double_3 ratio_3;
  vcl_vector <ratios> ratios_front_list;
  vcl_vector <ratios> ratios_rear_list;

  bool selected0,selected1;
  double distance0,distance1,distance2;

  vcl_vector <vgl_point_3d<double> > point_3d_front_list, point_3d_rear_list;
  // vcl_vector <vgl_point_3d<double> > point_3d_mean_pts;

  dbecl_episeg_point  x0pt,x1pt;
  //dbecl_episeg_point  x0pt(episeglist[fm1][i0], episeglist[fm1][i0]->index(ang));




  double xx,yy,zz;
  float r=1,g=0,b=0;

  double radius=0;

  vcl_cout<<"good_3d_curves_.size: "<<good_3d_curves_.size()<<vcl_endl;
  vnl_matrix <double> BB((int)combination,C-2,0.0);
  for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=ang_ ///) { 

  for (unsigned i=0;i<network_.size();i++) {

  int fm1=network_[i].fm1;
  int fm2=network_[i].fm2;
  int fm3=network_[i].fm3;

  for (unsigned j=0;j<network_[i].horizon.size();j++) {
  if (network_[i].horizon[j][0]==ang) {
  vgl_point_2d <double> x0pt(network_[i].horizon[j][1],network_[i].horizon[j][2]);
  vgl_point_2d <double> x1pt(network_[i].horizon[j][3],network_[i].horizon[j][4]);
  vgl_point_2d <double> x2pt(network_[i].horizon[j][5],network_[i].horizon[j][6]);
  node_edge n(fm1,fm2,fm3,x0pt,x1pt,x2pt);

  n_list.push_back(n);

  }
  }

  vgl_point_2d <double> x1pt=network_[i].pt2;
  vgl_point_2d <double> x2pt=network_[i].pt3;
  vgl_point_3d<double> point_3d_12 = brct_algos::triangulate_3d_point(x0pt, cameras_[fm1], x1pt ,cameras_[fm2]);
  vgl_point_3d<double> point_3d_23 = brct_algos::triangulate_3d_point(x1pt, cameras_[fm2], x2pt ,cameras_[fm3]);
  vgl_point_3d<double> point_3d_13 = brct_algos::triangulate_3d_point(x0pt, cameras_[fm1], x2pt, cameras_[fm3]);


  xx=(point_3d_12.x()+point_3d_23.x()+point_3d_13.x());
  yy=(point_3d_12.y()+point_3d_23.y()+point_3d_13.y());
  zz=(point_3d_12.z()+point_3d_23.z()+point_3d_13.z());

  // point_3d_front_list.push_back(point_3d);


  vgl_point_3d <double> mean_point_3d(xx/3.0,yy/3.0,zz/3.0);

  pts_3d_attribute p_att;
  p_att.usage=true;
  p_att.error=0.0;
  p_att.count=0;
  p_att.weight=   (good_3d_curves_[i].angle+theta_pos_)/(-theta_neg_+theta_pos_);
  p_att.position=true;///later  avoid left side 
  p_att.pt3d=mean_point_3d;
  p_att.angle= good_3d_curves_[i].angle;



  double error=0;
  double temp_x12=point_3d_12.x()-xx;double temp_x13=point_3d_13.x()-xx;double temp_x23=point_3d_23.x()-xx;
  double temp_y12=point_3d_12.y()-yy;double temp_y13=point_3d_13.y()-yy;double temp_y23=point_3d_23.y()-yy;
  double temp_z12=point_3d_12.z()-zz;double temp_z13=point_3d_13.x()-zz;double temp_z23=point_3d_23.z()-zz;

  double error_12=temp_x12*temp_x12+temp_y12*temp_y12+temp_z12*temp_z12;
  double error_13=temp_x13*temp_x13+temp_y13*temp_y13+temp_z13*temp_z13;
  double error_23=temp_x23*temp_x23+temp_y23*temp_y23+temp_z23*temp_z23;


  error=error_12+error_13+error_23;


  p_att.error=error;
  p_att.error=0;
  //p_att.count=count;
  p_att.count=3;  //3 pairs..
  //p_att.weight=.88888888888;
  pts_3d_a_.push_back(p_att);*/
  /*ratios temp_ratios;
  temp_ratios.angle=ang;
  temp_ratios.ratio=ratio;
  temp_ratios.pt3d=point_3d;
  temp_ratios.pt1=x0pt.pt();
  temp_ratios.pt2=x1pt.pt();
  temp_ratios.pt3=x2pt.pt();


  temp_ratios.fm1=x0;
  temp_ratios.fm2=x1;
  temp_ratios.fm3=x2;

  ratios_front_list.push_back(temp_ratios);*/

  ////   } //frame//

  return;
}

bool node_edge::compare_and_add(node_edge *a)
{

  bool same_group=false;
  for (unsigned i=0;i<this->pt_.size();i++)
  {
    for (unsigned j=0;j<a->pt_.size();j++) {

      for (unsigned k=0;k<3;k++)
        for (unsigned l=k+1;l<3;l++) 
          for (unsigned m=0; m<3; m++)
            for (unsigned n=m+1;n<3;n++) { 

              if (this->pt_[i][k]==a->pt_[j][m] && this->pt_[i][l]==a->pt_[j][n]) //same group
                same_group=true;
            }
    }
  }

  if (same_group)
    for (unsigned j=0;j<a->pt_.size();j++) {
      this->pt_.push_back(a->pt_[j]);
      this->ijk_.push_back(a->ijk_[j]);
    }


    //delete a;
    return same_group;
}

/*struct cluster {
int index;
// int fm1,fm2,fm3;
//double x1,x2,x3,y1,y2,y3;
vcl_vector <vgl_point_3d <double> >pts;
double mean;
bool label;
double error;//variance
//vgl_point_3d<double> pt3d;
int size;

};

typedef struct cluster cluster;*/


cluster  dbcri_whole_process::outliers(vcl_vector <vgl_point_3d<double> > point_3d_list,double th,cluster & sel_D) {

  int N=point_3d_list.size();
  vcl_vector <cluster>CL;
  for (unsigned i=0;i<N;i++) {
    cluster C;
    C.pts.push_back(point_3d_list[i]);
    C.index=i;
    C.label=true;
    C.error=0;
    C.mean_pt=point_3d_list[i];
    C.size=1;

    CL.push_back(C);
  }

  bool action=true;

  int sel_i,sel_j;
  //  cluster sel_D;
  cluster D;
  while (action) {

    double min_error=9999999999.0;
    for (unsigned i=0;i<CL.size();i++) {
      for (unsigned j=i+1;j<CL.size();j++) {


        //double error=cluster_cmp(CL[i],CL[j],D); // 
        double error=cluster_cmp_distance(CL[i],CL[j],D); th=0.4;
        if (error<min_error) {
          min_error=error;
          sel_i=i;sel_j=j;
          cluster_copy(D,sel_D);
        }
      }
    }

    if (min_error < th) {
      vcl_vector<cluster>::iterator it;
      it = CL.begin();
      CL.erase(it+sel_j); //sel_j is alway bigger than sel_i so erase sel_j first
      CL.erase(it+sel_i);
      CL.push_back(sel_D);
    }
    else {
      action=false;
    }

  } //while end


  // if there is dominant cluster return that...
  // else...???

  float max_size=0;int max_index=-1;
  for (unsigned i=0;i<CL.size();i++) {

    vcl_cout<<"\n CL[i] index: "<<i<<"*: "<<CL[i].mean_pt<<vcl_endl;
    if (CL[i].pts.size()>1)
      for (unsigned j=0;j<CL[i].pts.size();j++)
        vcl_cout<<i<<"'th "<<j<<": "<<CL[i].pts[j]<<vcl_endl;

    int  size=CL[i].size;
    if (size>max_size) {
      max_size=size;
      max_index=i;

    }
  }

  cluster dummy;
  dummy.label=false;

  //3-17-2008 max_size should differ depending on nframes_
  // since nonlinearity of combination N*(N-1)/2 
  //e.g, half of the frames can be screwed up 

  float max_control=.5*nframes_*(.5*nframes_-1)/2.0; //only half of them are good

  //if (max_size> (float)N/4.0) {  //7-01-07 N/2.0->N/4.0
  if (max_size> max_control) {  //3-18-2008
    cluster_copy(CL[max_index],sel_D);
    vcl_cout<<"\n==> mean pt: "<<sel_D.mean_pt<<vcl_endl;
    vcl_cout<<"sel_D error: "<<sel_D.error<<" "<<"index: "<<sel_D.index<<" size:"<<sel_D.size<<vcl_endl;
    for (unsigned k=0;k<sel_D.pts.size();k++)
      vcl_cout<<sel_D.pts[k]<<vcl_endl;
    return CL[max_index];
  }


  else return dummy;
  //return 1.0;
}

cluster dbcri_whole_process::cluster_add(cluster A,cluster B) {

  cluster C;
  for (unsigned i=0;i<A.size;i++)
  {
    C.pts.push_back(A.pts[i]);
  }

  for (unsigned i=0;i<B.size;i++)
  {
    C.pts.push_back(B.pts[i]);
  }

  //C.size=A.size+B.size;

  return C;
}

double dbcri_whole_process::cluster_cmp(cluster A,cluster B, cluster &C) {


  double x=0,y=0,z=0;
  for (unsigned i=0;i<A.size;i++)
  {
    x+=A.pts[i].x();
    y+=A.pts[i].y();
    z+=A.pts[i].z();
  }

  for (unsigned i=0;i<B.size;i++)
  {
    x+=B.pts[i].x();
    y+=B.pts[i].y();
    z+=B.pts[i].z();
  }

  double mx,my,mz;
  mx=x/(A.size+B.size);
  my=y/(A.size+B.size);
  mz=z/(A.size+B.size);

  double error=0.0;

  //x=0;y=0;z=0;
  for (unsigned i=0;i<A.size;i++)
  {
    error+=(A.pts[i].x()-mx)*(A.pts[i].x()-mx);
    error+=(A.pts[i].y()-my)*(A.pts[i].y()-my);
    error+=(A.pts[i].z()-mz)*(A.pts[i].z()-mz);
  }

  for (unsigned i=0;i<B.size;i++)
  {
    error+=(B.pts[i].x()-mx)*(B.pts[i].x()-mx);
    error+=(B.pts[i].y()-my)*(B.pts[i].y()-my);
    error+=(B.pts[i].z()-mz)*(B.pts[i].z()-mz);
  }

  error/=(A.size+B.size);


  //if (error<th) {
  C=cluster_add(A,B);
  C.error=error;
  C.label=true;
  if (A.size>=B.size) C.index=A.index;
  else C.index=B.index;
  C.mean_pt.set(mx,my,mz);
  C.size=A.size+B.size;
  return error;
  //}
  //else return 0;
}

double dbcri_whole_process::cluster_cmp_distance(cluster A,cluster B, cluster &C) {


  double x=0,y=0,z=0;
  for (unsigned i=0;i<A.size;i++)
  {
    x+=A.pts[i].x();
    y+=A.pts[i].y();
    z+=A.pts[i].z();
  }

  for (unsigned i=0;i<B.size;i++)
  {
    x+=B.pts[i].x();
    y+=B.pts[i].y();
    z+=B.pts[i].z();
  }

  double mx,my,mz;
  mx=x/(A.size+B.size);
  my=y/(A.size+B.size);
  mz=z/(A.size+B.size);


  double error=0.0;

  double dist=0.0;
  double min_distance=100000000.0;
  //x=0;y=0;z=0;
  int sel_i=0,sel_j=0;
  for (unsigned i=0;i<A.size;i++)
    for (unsigned j=0;j<B.size;j++)

    {
      dist=vcl_sqrt ((A.pts[i].x()-B.pts[j].x())*(A.pts[i].x()-B.pts[j].x()) +
        (A.pts[i].y()-B.pts[j].y())*(A.pts[i].y()-B.pts[j].y()) + 
        (A.pts[i].z()-B.pts[j].z())*(A.pts[i].z()-B.pts[j].z()));
      if (dist<min_distance) {

        min_distance=dist;
        sel_i=i;
        sel_j=j;
      }

    }
    error=min_distance;



    //if (error<th) {


    C.pts.clear();
    C=cluster_add(A,B);
    C.error=min_distance;
    C.label=true;
    if (A.size>=B.size) C.index=A.index;
    else C.index=B.index;
    C.mean_pt.set(mx,my,mz);
    C.size=A.size+B.size;
    return error;
    //}
    //else return 0;
}


void dbcri_whole_process::cluster_copy(cluster A,cluster &B) {

  B.error=A.error;
  B.index=A.index;
  B.label=A.label;
  B.mean_pt=A.mean_pt;
  B.pts.clear();
  for (unsigned i=0;i<A.pts.size();i++)
    B.pts.push_back(A.pts[i]);
  B.size=A.size;

}
bool dbcri_accu_edge_sort(const double& rhs, const double& lhs)
{
  return rhs < lhs;
}
bool dbcri_accu_edge_cmp_x(const vnl_double_3& rhs, const vnl_double_3& lhs)
{
  return rhs[0] < lhs[0];
}

bool dbcri_accu_edge_cmp_y(const vnl_double_3& rhs, const vnl_double_3& lhs)
{
  return rhs[1] < lhs[1];
}

bool dbcri_accu_edge_cmp_z(const vnl_double_3& rhs, const vnl_double_3& lhs)
{
  return rhs[2] < lhs[2];
}



void dbcri_whole_process::get_BB_and_scale(vcl_vector <vgl_point_3d<double> > point_3d_list_f,vcl_vector <vgl_point_3d<double> > point_3d_list_r) {

  vnl_double_3x4 C;

  C[0][0]= 2.71954;    C[0][1]= 12.1025;    C[0][2]=0;         C[0][3]=63.9305;
  C[1][0]=-1.00509;    C[1][1]= 0.757923;   C[1][2]=-11.6824;  C[1][3]= 159.883;
  C[2][0]=-0.00516136; C[2][1]= 0.0026911;  C[2][2]= 0;        C[2][3]= 0.33245;


  C=WC_;

  //epipole from world camera
  vnl_double_3 nep = C.get_column(0);   
  vgl_vector_3d<double> direction;

  vnl_double_3x3 M = C.extract(3,3); //correct??
  // compute the 3d direction unit vector
  vnl_double_3 e;
  //e[0] = ep.x();
  //e[1] = ep.y();

  //epipole from world camera
  e[0] =nep[0]/nep[2];
  e[1] =nep[1]/nep[2];

  vcl_cout<<e<<vcl_endl;
  //epipole from 2 tangent line
  e[0] = epi_x_;
  e[1]=  epi_y_;

  e[2] = 1.0;
  vcl_cout<<e<<vcl_endl;


  vnl_double_3 dir = vnl_inverse(M)*e;
  direction.set(dir[0],dir[1],dir[2]);
  normalize(direction);




  // from brl/bseg/bmrf/bmrf_curve_3d_builder.cxx
  //builder.compute_bb


  bool align_ep=false;

  vnl_double_3x3 rot;
  if ( align_ep ) //something wrong
  {
    vnl_double_3 base_axis(1.0, 0.0, 0.0);
    vnl_double_3 rot_axis(0.0, 1.0, 0.0);
    vnl_double_3 xz_proj(direction.x(), 0.0, direction.z());
    xz_proj.normalize();
    double ang = angle(xz_proj, base_axis);// before pos.
    rot_axis *= ang;
    vnl_double_3x3 rot_y = vnl_rotation_matrix(rot_axis);

    rot_axis = vnl_double_3(0.0, 0.0, 1.0);
    vnl_double_3 xy_proj = rot_y * vnl_double_3(direction.x(), direction.y(), direction.z());
    xy_proj.normalize();
    //ang = angle(xy_proj, base_axis);
    ang = angle(xy_proj, -base_axis); //6-23-07
    rot_axis *= ang;
    vnl_double_3x3 rot_z = vnl_rotation_matrix(rot_axis);

    rot = rot_z*rot_y;
  }

  else {
    vnl_double_3 x_axis_here(1.0, 0.0, 0.0);
    vnl_double_3 xy_proj(direction.x(), direction.y(), 0.0);
    xy_proj.normalize();
    double ang = angle(xy_proj, -x_axis_here);
    vcl_cerr << "bbox rotation angle: " <<ang << vcl_endl;
    vnl_double_3 rot_axis(0.0, 0.0, ang);

    rot = vnl_identity_3x3();
    rot = vnl_rotation_matrix(rot_axis);
  }


  rot_=rot; //


  vcl_cout<<"rot: "<<rot<<vcl_endl;
  vcl_vector<vnl_double_3> pts_z;
  for (unsigned i=0;i<point_3d_list_f.size();i++) {
    vnl_double_3 p(point_3d_list_f[i].x(),point_3d_list_f[i].y(),point_3d_list_f[i].z());

    pts_z.push_back(rot*p);
    //vcl_cout<<rot*p<<vcl_endl;
  }



  vcl_cout<<"checking pts # delivered to get bb: "<<point_3d_list_f.size()<<vcl_endl;
  if (point_3d_list_f.size()==0) return;
  vcl_sort(pts_z.begin(), pts_z.end(), dbcri_accu_edge_cmp_z);
  vcl_vector<vnl_double_3> pts_x;

  // only consider points in x and y above 0.5 feet  //i don't use it..
  for ( vcl_vector<vnl_double_3>::const_iterator itr = pts_z.begin();
    itr != pts_z.end();  ++itr)
  {
    //if ((*itr)[2] > 0.5) { pts_x.push_back(*itr); }
    pts_x.push_back(*itr); 
  }
  if (pts_x.empty()) { // first loop
    pts_x = pts_z;
  }

  vcl_vector<vnl_double_3> pts_y = pts_x;

  vcl_sort(pts_x.begin(), pts_x.end(), dbcri_accu_edge_cmp_x);
  vcl_sort(pts_y.begin(), pts_y.end(), dbcri_accu_edge_cmp_y);

  /* vcl_cout<<"::get_BB_and_scale ### pts_y start ###"<<vcl_endl;
  for (vcl_vector<vnl_double_3>::const_iterator itr = pts_y.begin();
  itr != pts_y.end();  ++itr)
  vcl_cout<<(*itr)<<vcl_endl;
  vcl_cout<<"::get_BB_and_scale ### pts_y end ###"<<vcl_endl;*/

  unsigned int min_ind_x = 0, min_ind_y = 0, min_ind_z = 0;
  unsigned int max_ind_x = pts_x.size()-1, max_ind_y = pts_y.size()-1, max_ind_z = pts_z.size()-1;


  /*if (inlier_fractions) {
  float fraction_out = (1.f - inlier_fractions[0])*.5f;
  min_ind_x = (unsigned int)(max_ind_x*fraction_out);
  max_ind_x = (unsigned int)(max_ind_x*(1.f-fraction_out));
  fraction_out = (1.f - inlier_fractions[1])*.5f;
  min_ind_y = (unsigned int)(max_ind_y*fraction_out);
  max_ind_y = (unsigned int)(max_ind_y*(1.f-fraction_out));
  fraction_out = (1.f - inlier_fractions[2])*.5f;
  min_ind_z = (unsigned int)(max_ind_z*fraction_out);
  max_ind_z = (unsigned int)(max_ind_z*(1.f-fraction_out));
  }*/

  vnl_double_3 min_point(pts_x[min_ind_x][0], pts_y[min_ind_y][1], pts_z[min_ind_z][2]);
  vnl_double_3 max_point(pts_x[max_ind_x][0], pts_y[max_ind_y][1], pts_z[max_ind_z][2]);

  min_point_=min_point;
  max_point_=max_point;

  vcl_cout<<"::get_BB_and_scale min_point:"<<min_point<<" "<<"max_point:"<< max_point<<vcl_endl;

  vnl_double_3 diag_vector = max_point - min_point;
  vnl_vector_fixed<double,3> x_axis(0.0), y_axis(0.0), z_axis(0.0);
  x_axis[0] = diag_vector[0];
  y_axis[1] = diag_vector[1];
  z_axis[2] = diag_vector[2];

  vnl_double_3x3 inv_rot = rot.transpose();

  inv_rot_=inv_rot;

  x_axis = inv_rot * x_axis;
  y_axis = inv_rot * y_axis;
  z_axis = inv_rot * z_axis;

  vnl_double_3 origin = inv_rot * min_point;



  vnl_double_4x4 bb_xform;

  bb_xform.set_identity();
  bb_xform(0,0)=x_axis[0]; bb_xform(1,0)=x_axis[1]; bb_xform(2,0)=x_axis[2];
  bb_xform(0,1)=y_axis[0]; bb_xform(1,1)=y_axis[1]; bb_xform(2,1)=y_axis[2];
  bb_xform(0,2)=z_axis[0]; bb_xform(1,2)=z_axis[1]; bb_xform(2,2)=z_axis[2];
  bb_xform(0,3)=origin[0]; bb_xform(1,3)=origin[1]; bb_xform(2,3)=origin[2];


  vcl_cout<<"bb_xform"<<vcl_endl;
  vcl_cout<<bb_xform<<vcl_endl;

  bb_xform_=bb_xform;

  // compute the camera height
  vnl_double_4 center = vnl_svd<double>(C).nullvector();
  double cam_height = center[2]/center[3];

  // find the bottom of the bounding box

  //builder.compute_bounding_box(inlier);
  //vnl_double_4x4 bb_xform = builder.bb_xform();
  double min_z = vcl_numeric_limits<double>::infinity();
  for(int i=0; i<8; ++i){ // for each corner
    vnl_double_4 corner = bb_xform*vnl_double_4(double(i/4),double((i/2)%2),double(i%2),1.0);
    double z = corner[2]/corner[3];
    if(z < min_z)
      min_z = z;
  }

  //SCALE
  // find a scale that puts the bottom of bounding box at z=0.0 
  double scale = (cam_height-0.0)/(cam_height - min_z );
  vcl_cout<<"cam_height: "<<cam_height<<vcl_endl;
  vcl_cerr << "scale = " << scale << "  min z = " << min_z << vcl_endl;

  //         scale = (cam_height-0.0)/(cam_height - min_z );
  //    vcl_cout<<"cam_height: "<<cam_height<<vcl_endl;
  //    vcl_cerr << "scale = " << scale << "  min z = " << min_z << vcl_endl;

  if(bool reverse=false)
    scale *= -1;

  // recompute everything with the new scale
  //builder.init_cameras(C,scale);
  //builder.reconstruct(sigma);
  //builder.compute_bounding_box(inlier);


  scale_=scale;

  ///////////final printing

  vcl_cout<<"\nBounding Box Transform\n"<<scale_*bb_xform<<vcl_endl;


  //vnl_double_3x4 Ef = cameras_[0];
  vnl_double_3x4 Ef = C_;
  vcl_cout<<Ef<<vcl_endl;
  double dt=0.0;
  for (unsigned i=1; i<cameras_.size();i++) {

    Ef=cameras_[i];
    //=scale_;
    dt+=tnew_[i-1];
    //Ef.set_column(3,Ef.get_column(3) - dt*e);
    Ef.set_column(3,C_.get_column(3) + scale_*dt*e);  //peositive directionn ???????????
    // Ef.set_column(3,Ef.get_column(3)*scale_);
    //  vcl_cout<< Ef <<vcl_endl;
    //vcl_cout<< cameras_[i] <<vcl_endl;

  }


}


void dbcri_whole_process::left_right_line_bb(vcl_vector <vgl_point_3d<double> > &point_3d_left_list,vcl_vector <vgl_point_3d<double> > &point_3d_right_list) {






  vgl_point_2d <double> epi_point(epi_x_,epi_y_);
  for (unsigned fm1=0;fm1<5;fm1++)
    for (unsigned fm2=fm1+1;fm2<5;fm2++) {

      //left points
      vgl_line_2d<double> l1(epi_point,left_pts_[fm1]);

      double y=-(l1.a()*left_pts_[fm2].x()+l1.c())/l1.b();
      vgl_point_2d <double> second_pt(left_pts_[fm2].x(),y);
      vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(left_pts_[fm1], cameras_[fm1], second_pt, cameras_[fm2]);


      vcl_cout<<point_3d<<vcl_endl;
      point_3d_left_list.push_back(point_3d);

      //right points
      vgl_line_2d<double> l2(epi_point,right_pts_[fm1]);

      y=-(l1.a()*right_pts_[fm2].x()+l1.c())/l1.b();
      second_pt.set(right_pts_[fm2].x(),y);
      point_3d = brct_algos::triangulate_3d_point(right_pts_[fm1], cameras_[fm1], second_pt, cameras_[fm2]);



      vcl_cout<<point_3d<<vcl_endl;
      point_3d_right_list.push_back(point_3d);


    } //frame//

    return ;

}




void dbcri_whole_process::contour() {

  double threshold=0.5;
  vgl_point_3d <double> max_x_pts_3d,min_x_pts_3d;
  vgl_point_3d <double> max_y_pts_3d,min_y_pts_3d;
  vgl_point_3d <double> max_z_pts_3d,min_z_pts_3d;

  double max_x=vcl_numeric_limits<double>::min();
  double min_x=vcl_numeric_limits<double>::infinity();
  double max_y=vcl_numeric_limits<double>::min();
  double min_y=vcl_numeric_limits<double>::infinity();
  double max_z=vcl_numeric_limits<double>::min();
  double min_z=vcl_numeric_limits<double>::infinity();

  int size=pts_3d_.size();

  vcl_cout<<"before deletion size is: "<<size<<vcl_endl;

  for (unsigned i=0;i< size; i++)
  {  

    vcl_cout<< pts_3d_[i].z()<<vcl_endl;
    if (threshold>pts_3d_[i].z()) {
      pts_3d_.erase(pts_3d_.begin()+(int)i);
      i--;
      size--;
    }

  }

  vcl_cout<<"after deletion the number of pts  is: "<<size<<vcl_endl;

  for (unsigned i=0;i< pts_3d_.size(); i++)
  {  

    if (max_x<pts_3d_[i].x()) {
      max_x=pts_3d_[i].x();
      max_x_pts_3d=pts_3d_[i];
    }
    if (min_x>pts_3d_[i].x()) {
      min_x=pts_3d_[i].x();
      min_x_pts_3d=pts_3d_[i];
    }
    //vcl_cout<< pts_3d_[i].x()<<vcl_endl;

  }
  vcl_cout<<vcl_endl;
  for (unsigned i=0;i< pts_3d_.size(); i++)
  {  

    if (max_y<pts_3d_[i].y()) {
      max_y=pts_3d_[i].y();
      max_y_pts_3d=pts_3d_[i];
    }
    if (min_y>pts_3d_[i].y()) {
      min_y=pts_3d_[i].y();
      min_y_pts_3d=pts_3d_[i];
    }
    //vcl_cout<< pts_3d_[i].y()<<vcl_endl;

  }

  vcl_cout<<vcl_endl;


  //  int size=pts_3d_.size();
  for (unsigned i=0;i< pts_3d_.size(); i++)
  {  
    if (max_z<pts_3d_[i].z()) {
      max_z=pts_3d_[i].z();
      max_z_pts_3d=pts_3d_[i];
    }
    if (min_z>pts_3d_[i].z()) {
      min_z=pts_3d_[i].z();
      min_z_pts_3d=pts_3d_[i];
    }

    vcl_cout<< pts_3d_[i].z()<<vcl_endl;

  }
}

void dbcri_whole_process::delete_contour(double thresh) 
{


  // pts_3d_.clear();
  vcl_vector <vgl_point_3d  <double> > pts_3d;
  vgl_point_3d <double> p;
  for (unsigned i=0;i<pts_3d_a_.size();i++) {

    vcl_cout<<pts_3d_a_[i].pt3d<<" usage: "<<pts_3d_a_[i].usage<<" pos: "<<pts_3d_a_[i].position<<" weight:"<< pts_3d_a_[i].weight<<vcl_endl;
    //pts_3d_a_[i].usage=true;
    if (pts_3d_a_[i].position&& pts_3d_a_[i].weight>.5)// use lower wedge points only  //angle goes clockwise
    {
      p=pts_3d_a_[i].pt3d;
      pts_3d.push_back(p);
    }




  }

  vgl_point_3d <double> max_x_pts_3d,min_x_pts_3d;
  vgl_point_3d <double> max_y_pts_3d,min_y_pts_3d;
  vgl_point_3d <double> max_z_pts_3d,min_z_pts_3d;

  double max_x=vcl_numeric_limits<double>::min();
  double min_x=vcl_numeric_limits<double>::infinity();
  double max_y=vcl_numeric_limits<double>::min();
  double min_y=vcl_numeric_limits<double>::infinity();
  double max_z=vcl_numeric_limits<double>::min();
  double min_z=vcl_numeric_limits<double>::infinity();

  //6-30-07 if (pts_3d.size()<10) return;
  vcl_cout<<"Before out_removal points size: "<<pts_3d.size()<<vcl_endl;
  if (pts_3d.size()<10) {
    vcl_cout<<"too small num of points delvered to del con\n just returning"<<vcl_endl;
    return;
  }
  out_removal(pts_3d);
  //out_removal_1(pts_3d);  
  for (unsigned i=0;i< pts_3d.size(); i++)
  {  
    if (max_z<pts_3d[i].z()) {
      max_z=pts_3d[i].z();
      max_z_pts_3d=pts_3d[i];
    }
    if (min_z>pts_3d[i].z()) {
      min_z=pts_3d[i].z();
      min_z_pts_3d=pts_3d[i];
    }
    //vcl_cout<< pts_3d[i].z()<<vcl_endl;   
  }




  //statstical filtering...

  float count=0;
  float max_count=0;
  double di_max=-1000.0;

  vcl_cout<<"-----------------delete loop---------------------"<<vcl_endl;   
  vcl_cout<<"***max_count: "<<max_count<<": "<<di_max<<vcl_endl;
  for (double di=min_z;di<max_z;di+=(max_z-min_z)/100.0 )
  {
    for (unsigned j=0;j< pts_3d_a_.size(); j++) {
      if (vcl_fabs(pts_3d_a_[j].pt3d.z()-di)<2.0*(max_z-min_z)/100.0) //2.0*(max_z-min_z)/100.0 ==>bin size
      {

        //vcl_cout<<"weight:"<<pts_3d_a_[j].weight<<vcl_endl;
        if (pts_3d_a_[j].position&&pts_3d_a_[j].weight>.5)
          //count+=5.0*pts_3d_a_[j].weight;  //adding linear bias to prefer high wedge angle
          count++;
      }
    }

    vcl_cout<<di<<" "<< count<<vcl_endl;
    if (count>max_count) {
      max_count=count;
      di_max=di;
    }
    count=0;
  }


  vcl_cout<<"max_count: "<<max_count<<": "<<di_max<<vcl_endl;


  for (unsigned j=0;j< pts_3d_a_.size(); j++) {
    vcl_cout<<pts_3d_a_[j].angle<<" "<<pts_3d_a_[j].pt3d;

    if (0)  //don't use it
      if (vcl_fabs(pts_3d_a_[j].pt3d.z()-di_max)<3.0*(max_z-min_z)/100.0) //2.0*(max_z-min_z)/100.0 ==>bin size
      {
        pts_3d_a_[j].usage=false;

        vcl_cout<<"*** deleted (in the slot)"<<vcl_endl;
        continue;
      }


      ///if (thresh>0.01) //skip first time...
      if (pts_3d_a_[j].pt3d.z()-di_max<thresh-(max_z-min_z)/100.0) //2.0*(max_z-min_z)/100.0 ==>bin size
      {
        pts_3d_a_[j].usage=false;

        vcl_cout<<"*** deleted (negative)"<<vcl_endl;
        continue;
      }


      //////////////
      if (thresh>0.01) //skip first time...
        if (pts_3d_a_[j].pt3d.z()<thresh) //2.0*(max_z-min_z)/100.0 ==>bin size
        {
          pts_3d_a_[j].usage=false;

          vcl_cout<<"*** deleted (0.5 negative)"<<vcl_endl;
          continue;
        }
        //////////////

        bool weight_flag=false;
        parameters()->get_value( "-weight_flag" ,  weight_flag);

        if (weight_flag)
          if (pts_3d_a_[j].weight<.1 || pts_3d_a_[j].weight>.9) //weight is angle..
          {
            pts_3d_a_[j].usage=false;
            vcl_cout<<"*** deleted due to weight"<<vcl_endl;
            continue;
          }

          vcl_cout<<vcl_endl;
          //

  }








}



void dbcri_whole_process::delete_contour_05(double thresh) 
{


  vcl_cout<<"::delete_contour_05 " <<vcl_endl;

  // pts_3d_.clear();
  vcl_vector <vgl_point_3d  <double> > pts_3d;
  vgl_point_3d <double> p;
  for (unsigned i=0;i<pts_3d_a_.size();i++) {

    vcl_cout<<pts_3d_a_[i].pt3d<<pts_3d_a_[i].position<<" "<< pts_3d_a_[i].weight<<vcl_endl;
    //pts_3d_a_[i].usage=true;
    if (pts_3d_a_[i].position&& pts_3d_a_[i].weight>.5)// use lower wedge points only  //angle goes clockwise
    {
      p=pts_3d_a_[i].pt3d;
      pts_3d.push_back(p);
    }




  }

  vgl_point_3d <double> max_x_pts_3d,min_x_pts_3d;
  vgl_point_3d <double> max_y_pts_3d,min_y_pts_3d;
  vgl_point_3d <double> max_z_pts_3d,min_z_pts_3d;

  double max_x=vcl_numeric_limits<double>::min();
  double min_x=vcl_numeric_limits<double>::infinity();
  double max_y=vcl_numeric_limits<double>::min();
  double min_y=vcl_numeric_limits<double>::infinity();
  double max_z=vcl_numeric_limits<double>::min();
  double min_z=vcl_numeric_limits<double>::infinity();

  //6-30-07 if (pts_3d.size()<10) return;
  /*vcl_cout<<"out_removal points size: "<<pts_3d.size()<<vcl_endl;
  if (pts_3d.size()<10) {
  vcl_cout<<"too small num of points delvered to del con\n just returning"<<vcl_endl;
  return;
  }
  out_removal(pts_3d);*/

  //10-20-07 
  vcl_cout<<"*==*"<<pts_3d.size()<<vcl_endl;

  if (pts_3d.size()>5)
    out_removal_1(pts_3d);  

  for (unsigned i=0;i< pts_3d.size(); i++)
  {  
    if (max_z<pts_3d[i].z()) {
      max_z=pts_3d[i].z();
      max_z_pts_3d=pts_3d[i];
    }
    if (min_z>pts_3d[i].z()) {
      min_z=pts_3d[i].z();
      min_z_pts_3d=pts_3d[i];
    }
    vcl_cout<< pts_3d[i].z()<<vcl_endl;   
  }




  //statstical filtering...

  float count=0;
  float max_count=0;
  double di_max=-1000.0;

  vcl_cout<<"------05-----------delete loop---------------------"<<vcl_endl;   
  vcl_cout<<"***max_count: "<<max_count<<": "<<di_max<<vcl_endl;
  for (double di=min_z;di<max_z;di+=(max_z-min_z)/100.0 )
  {
    for (unsigned j=0;j< pts_3d_a_.size(); j++) {
      if (vcl_fabs(pts_3d_a_[j].pt3d.z()-di)<2.0*(max_z-min_z)/100.0) //2.0*(max_z-min_z)/100.0 ==>bin size
      {

        //vcl_cout<<"weight:"<<pts_3d_a_[j].weight<<vcl_endl;
        if (pts_3d_a_[j].position&&pts_3d_a_[j].weight>.5)
          //count+=5.0*pts_3d_a_[j].weight;  //adding linear bias to prefer high wedge angle
          count++;
      }
    }

    vcl_cout<<di<<" "<< count<<vcl_endl;
    if (count>max_count) {
      max_count=count;
      di_max=di;
    }
    count=0;
  }


  vcl_cout<<"max_count: "<<max_count<<": "<<di_max<<vcl_endl;


  for (unsigned j=0;j< pts_3d_a_.size(); j++) {
    vcl_cout<<pts_3d_a_[j].angle<<" "<<pts_3d_a_[j].pt3d;


    if (pts_3d_a_[j].pt3d.z()-di_max<thresh-(max_z-min_z)/100.0) //2.0*(max_z-min_z)/100.0 ==>bin size
    {
      pts_3d_a_[j].usage=false;

      vcl_cout<<"*** deleted (negative)"<<vcl_endl;
      continue;
    }


    //////////////
    if (thresh>0.01) //skip first time...
      if (pts_3d_a_[j].pt3d.z()<thresh) //2.0*(max_z-min_z)/100.0 ==>bin size
      {
        pts_3d_a_[j].usage=false;

        vcl_cout<<"*** deleted (0.5 negative)"<<vcl_endl;
        continue;
      }
      //////////////

      bool weight_flag=false;
      parameters()->get_value( "-weight_flag" ,  weight_flag);

      if (weight_flag)
        if (pts_3d_a_[j].weight<.1 || pts_3d_a_[j].weight>.9) //weight is angle..
        {
          pts_3d_a_[j].usage=false;
          vcl_cout<<"*** deleted due to weight"<<vcl_endl;
          continue;
        }

        vcl_cout<<vcl_endl;
        //

  }

}


void dbcri_whole_process::write_vrml_bbox( vcl_ofstream& str,
                                          const vnl_double_4x4& bbox_xform )
{
  str << "DEF BoundingBox Shape {\n"
    << "  geometry \n"
    << "  IndexedLineSet {\n"
    << "    coord \n"
    << "    Coordinate {\n"
    << "      point [ ";
  for (int i=0; i<8; ++i){
    vnl_double_4 p((i/4)%2, (i/2)%2, i%2, 1);
    vnl_double_4 pt = bbox_xform * p;
    str << pt[0]/pt[3] << " " << pt[1]/pt[3] << " " << pt[2]/pt[3] << ", ";
  }
  str << " ]\n"
    << "    }\n"
    << "    colorPerVertex FALSE\n"
    << "    color Color {\n"
    << "      color [ 0 0 1, 0 0 1, 0 0 1, 0 0 1 ]\n"
    << "    }\n"
    << "    coordIndex [ 0, 1, 3, 2, 0, 4, 5, 7, 6, 4, -1\n"
    << "                 1, 5, -1, 2, 6, -1, 3, 7, -1 ]\n"
    << "  }\n"
    << "} \n";

}

void dbcri_whole_process::print_bb_and_camera(vcl_string bb_dir)
{



  vcl_vector<vnl_double_3x4> cameras;
  vcl_vector <int> nums_list;
  int counter=0;



  int fram_start;
  int fram_end;

  char tf[20];char f11[22]; 

  vcl_string input_file_path=".";
  vul_file_iterator fn=input_file_path+"/*.con";//nframes_=5;
  for ( ; fn; ++fn) 
  {
    vcl_string input_file = fn();
    vcl_cout<<input_file<<vcl_endl;


    vcl_string aaa=vul_file::strip_directory(input_file);
    vcl_string nums1=vul_file::strip_extension(aaa);
    vcl_cout<<nums1<<vcl_endl;
    nums_list.push_back(atoi(nums1.c_str()));

    if (counter==0)  fram_start= atoi(nums1.c_str());
    if (counter==nframes_-1) fram_end= atoi(nums1.c_str());

    counter++;
  }

  int how_many_frames=(int) (fram_end-fram_start);




  //vcl_ofstream bbout(vcl_string(out_dir+"/bbox_cam.txt").c_str());
  vcl_string bb_file=bb_dir+"/bbox_cam_my.txt";
  //vcl_ofstream bbout(vcl_string("bbox_cam_my.txt").c_str());
  vcl_ofstream bbout(bb_file.c_str());
  bbout << "Bounding Box Transform\n" << bb_xform_ << '\n';


  int camera_count=nframes_;
  int search_num;bool search_num_flag;
  for (int i=fram_start;i<=fram_end; i++) {


    search_num=false;
    for (unsigned j=0;j<nums_list.size();j++) {
      if (i==nums_list[j]) {
        camera_count--;
        search_num_flag=true;
        //search_num=i;
      }

    }

    if (search_num_flag) bbout << "Camera " << i << '\n' << cameras_[camera_count] << '\n';
    else bbout << "Camera " << i << '\n' <<  cameras_[camera_count] << '\n';

  }


  // return;

  // for (int  i=cameras_.size()-1;i>=0;i--) {

  //   bbout << "Camera " << i << '\n' << cameras_[i] << '\n';

  // } 

  //p7 and p0 are switched, but since used to it.. leave it..
  bbout<< p7_[0]<<" "<<p7_[1]<<" "<<p7_[2]<<'\n';
  bbout<< p0_[0]<<" "<<p0_[1]<<" "<<p0_[2]<<'\n';
  bbout<< solve_z_<<'\n';

  // bbout<< p2_[0]<<" "<<p2_[1]<<" "<<p2_[2]<<'\n';
  // bbout<< p8_[0]<<" "<<p8_[1]<<" "<<p8_[2]<<'\n';
  // bbout<< p_cabin_length_[0]<<" "<<p_cabin_length_[1]<<" "<<p_cabin_length_[2]<<'\n';
  bbout<< p2_<<'\n';
  bbout<< p8_<<'\n';
  bbout<< p_cabin_length_<<'\n';



  bbout.close();



  return;
}


void dbcri_whole_process::print_con_probe(vcl_string bb_dir)
{
  vcl_string bb_file=bb_dir+"/con_probe.txt";
  vcl_ofstream bbout(bb_file.c_str());
  bbout<< p7_[0]<<" "<<p7_[1]<<" "<<p7_[2]<<'\n';
  bbout<< p0_[0]<<" "<<p0_[1]<<" "<<p0_[2]<<'\n';
  bbout<< solve_z_<<'\n';

  // bbout<< p2_[0]<<" "<<p2_[1]<<" "<<p2_[2]<<'\n';
  // bbout<< p8_[0]<<" "<<p8_[1]<<" "<<p8_[2]<<'\n';
  // bbout<< p_cabin_length_[0]<<" "<<p_cabin_length_[1]<<" "<<p_cabin_length_[2]<<'\n';
  bbout<< p2_<<'\n';
  bbout<< p8_<<'\n';
  bbout<< p_cabin_length_<<'\n';

  //p7 and p0 are switched, but since used to it.. leave it..

  bbout<< p0_con_[0]<<" "<<p0_con_[1]<<" "<<p0_con_[2]<<'\n';
  bbout<< p7_con_[0]<<" "<<p7_con_[1]<<" "<<p7_con_[2]<<'\n';
  bbout.close();
  return;
}

void dbcri_whole_process::read_WC()
{

  vnl_double_3x4 C;

  C[0][0]= 2.71954;    C[0][1]= 12.1025;    C[0][2]=0;         C[0][3]=63.9305;
  C[1][0]=-1.00509;    C[1][1]= 0.757923;   C[1][2]=-11.6824;  C[1][3]= 159.883;
  C[2][0]=-0.00516136; C[2][1]= 0.0026911;  C[2][2]= 0;        C[2][3]= 0.33245;
  //P1=C;
  WC_=C;


  //44
  C[0][0]= -39851.3;    C[0][1]= -135162;    C[0][2]= 0 ;    C[0][3]=-401667;
  C[1][0]=12495 ;       C[1][1]=-6936.71 ;   C[1][2]=126874; C[1][3]=-1.77544e+006;
  C[2][0]=54.2932;      C[2][1]= -35.5521;   C[2][2]= 0 ;    C[2][3]=-3623.91;

  WC_=-C;

  vcl_cout<<WC_<<vcl_endl;


  // cam1 HD
  C[0][0]=3.18664;    C[0][1]=19.0478;    C[0][2]=0;        C[0][3]=224.502;
  C[1][0]=-0.958451;  C[1][1]=1.07767;    C[1][2]=-17.0004; C[1][3]=145.466;
  C[2][0]=-0.0102312; C[2][1]=0.00647459; C[2][2]=0;        C[2][3]=0.204632;
  vcl_cout<<WC_<<vcl_endl;




}



void dbcri_whole_process::Estimation_BB_shift_using_convex_hull_in_2D() {

  //
  //static void test_4_point_hull()
  //{
  vnl_double_4x4 bb_xform=bb_xform_;


  int frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){


    vcl_vector<vgl_point_2d<double> > points;
    for (int i=0; i<8; ++i){
      vnl_double_4 p((i/4)%2, (i/2)%2, i%2, 1);
      //vcl_cout<<p<<vcl_endl;
      vnl_double_4 pt = bb_xform * p;

      vnl_double_3 pt2=cameras_[frame]*pt;
      //vcl_cout<<pt2<<vcl_endl;
      vgl_point_2d<double> p2d(pt2[0]/pt2[2],pt2[1]/pt2[2]);
      points.push_back(p2d);

    } 
    vgl_convex_hull_2d<double> ch(points);
    vgl_polygon<double> poly = ch.hull();

    vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
    vsol_polygon_2d_sptr poly_vsol = bsol_algs::poly_from_vgl(poly);
    output_vsol->add_object( poly_vsol->cast_to_spatial_object() ,"output_bb" );
    output_data_[frame].push_back(output_vsol);

  }


  int idx=0;


  int ref_num=0;

  frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
    ref_num +=points_inside_of_the_box(frame,  bb_xform);
  }

  bool x0_flag=false,y0_flag=false,z0_flag=false,x1_flag=false,y1_flag=false,z1_flag=false;

  double y0_sel=0.0,y0_just_before=0.0;
  double y1_sel=0.0,y1_just_before=0.0;
  double x0_sel=0.0,x0_just_before=0.0;

  double y1_escape=0.0;
  //y1_sel=0.0;


  for (double y1=0.0;y1>=-20.0;y1-=1.0+y1_escape)  {

    int count=0;
    vnl_double_4x4 bb_xform_reformed=bb_reform(0.0,0.0,0.0,0.0,y1,0.0);

    frame=0;
    for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
      count +=points_inside_of_the_box(frame,  bb_xform_reformed);
    }

    vcl_cout<<count<<vcl_endl;
    if (!y1_flag) {
      if (count < ref_num-500)  {
        y1_flag=true;
        //y1_sel=y1_just_before;
        y1_sel=y1;
        y1_escape=1000.0;
      }
      else {

        y1_just_before=y1_sel;
        y1_sel=y1;
      }
    }

    else {
      if (count < ref_num)  {


        //vcl_cout<<count<<" "<< x0<<" "<<y0<<vcl_endl;
        //y1_flag=true;
        //y1_sel=y1_just_before;
        //y1_escape=1000.0;
      }
      else {

        //y1_just_before=y1_sel;
        //y1_sel=y1;
      }

    }

  }






  int ref_num_x0=0;

  frame=0;


  //find y0 using histogram
  double r;
  statitical_filtering(r,pts_3d_);
  y0_sel=r;

  vnl_double_4x4 bb_xform_y1_fixed=bb_reform(0.0,y0_sel,0.0,0.0,y1_sel,0.0);
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
    ref_num_x0 +=points_inside_of_the_box(frame,  bb_xform_y1_fixed);
  }

  vcl_cout<<"ref: "<<ref_num_x0<<vcl_endl;
  double x0_escape=0.0;
  for (double x0=0.0;x0<=40.0;x0+=1.0+x0_escape) {
    for (double y0=0.0;y0<=0.0;y0+=1.0) {




      int count=0;
      vnl_double_4x4 bb_xform_reformed=bb_reform(x0,y0_sel,0.0,0.0,y1_sel,0.0);

      frame=0;
      for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
        count +=points_inside_of_the_box(frame,  bb_xform_reformed);
      }

      vcl_cout<<count<<vcl_endl;

      if (!x0_flag) {
        if (count < ref_num_x0-1)  {
          x0_flag=true;
          //y1_sel=y1_just_before;
          x0_sel=x0;
          x0_escape=1000.0;
        }
        else {

          x0_just_before=x0_sel;
          x0_sel=x0;
        }
      }



    }
  }

  vnl_double_4x4 bb_xform_reformed=bb_reform(x0_sel-1.0, y0_sel, 0.0, 0.0, y1_sel+1.0, 0.0);

  bb_xform_=bb_xform_reformed;

  frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
    vcl_vector<vgl_point_2d<double> > points;
    for (int i=0; i<8; ++i){
      vnl_double_4 p((i/4)%2, (i/2)%2, i%2, 1);
      //vcl_cout<<p<<vcl_endl;
      vnl_double_4 pt = bb_xform_reformed * p;

      vnl_double_3 pt2=cameras_[frame]*pt;
      //vcl_cout<<pt2<<vcl_endl;
      vgl_point_2d<double> p2d(pt2[0]/pt2[2],pt2[1]/pt2[2]);
      points.push_back(p2d);

    } 
    vgl_convex_hull_2d<double> ch(points);
    vgl_polygon<double> poly = ch.hull();


    vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
    vsol_polygon_2d_sptr poly_vsol = bsol_algs::poly_from_vgl(poly);
    output_vsol->add_object( poly_vsol->cast_to_spatial_object() ,"output_bb" );
    output_data_[frame].push_back(output_vsol);
  }






  //   p.contains( 0.0,  0.0 );

}


vnl_double_4x4 dbcri_whole_process::bb_reform(double x0, double y0, double z0, double x1, double y1, double z1)
{
  vnl_double_3 min_point(min_point_);
  vnl_double_3 max_point(max_point_);

  min_point(0)+=x0; min_point(1)+=y0; min_point(2)+=z0;
  max_point[0]+=x1; max_point[1]+=y1; max_point[2]+=z1;

  //vcl_cout<<min_point<<" "<< max_point<<vcl_endl;

  vnl_double_3 diag_vector = max_point - min_point;
  vnl_vector_fixed<double,3> x_axis(0.0), y_axis(0.0), z_axis(0.0);
  x_axis[0] = diag_vector[0];
  y_axis[1] = diag_vector[1];
  z_axis[2] = diag_vector[2];

  vnl_double_3x3 inv_rot = inv_rot_;

  x_axis = inv_rot * x_axis;
  y_axis = inv_rot * y_axis;
  z_axis = inv_rot * z_axis;

  //vcl_cout<<y_axis<<vcl_endl;
  //vcl_cout<<z_axis<<vcl_endl;
  vnl_double_3 origin = inv_rot * min_point;



  vnl_double_4x4 bb_xform_deform;

  bb_xform_deform.set_identity();
  bb_xform_deform(0,0)=x_axis[0]; bb_xform_deform(1,0)=x_axis[1]; bb_xform_deform(2,0)=x_axis[2];
  bb_xform_deform(0,1)=y_axis[0]; bb_xform_deform(1,1)=y_axis[1]; bb_xform_deform(2,1)=y_axis[2];
  bb_xform_deform(0,2)=z_axis[0]; bb_xform_deform(1,2)=z_axis[1]; bb_xform_deform(2,2)=z_axis[2];
  bb_xform_deform(0,3)=origin[0]; bb_xform_deform(1,3)=origin[1]; bb_xform_deform(2,3)=origin[2];

  //vcl_cout<<bb_xform_deform<<bb_xform<<vcl_endl;


  return bb_xform_deform;
}

int dbcri_whole_process::points_inside_of_the_box(int index, vnl_double_4x4 bb_xform) {

  vcl_vector<vgl_point_2d<double> > points;

  for (int i=0; i<8; ++i){
    vnl_double_4 p((i/4)%2, (i/2)%2, i%2, 1);
    //vcl_cout<<p<<vcl_endl;
    vnl_double_4 pt = bb_xform * p;

    vnl_double_3 pt2=cameras_[index]*pt;
    //vcl_cout<<pt2<<vcl_endl;
    vgl_point_2d<double> p2d(pt2[0]/pt2[2],pt2[1]/pt2[2]);
    points.push_back(p2d);

  } 
  vgl_convex_hull_2d<double> ch(points);
  vgl_polygon<double> poly = ch.hull();

  int count=0;

  for (unsigned i=0;i<(dcl_[index])->size();i++) {

    if ( poly.contains(dcl_[index]->point(i)->x(),dcl_[index]->point(i)->y() ) ) { 

      double temp_x= (dcl_[index]->point(i)->x()-epi_x_);
      double temp_y= (dcl_[index]->point(i)->y()-epi_y_);
      double ang=vcl_atan(temp_y/(temp_x));

      // vcl_cout<<ang<<vcl_endl;
      if  (ang>-theta_pos_+.03 && ang<-theta_neg_-.03 ) 
        count++ ;


    }
  }

  //vcl_cout<<count<<vcl_endl;
  return count;
}

void dbcri_whole_process::statitical_filtering(double &r, vcl_vector <vgl_point_3d <double> > pts) {


  vnl_double_3 min_point(min_point_);
  vnl_double_3 max_point(max_point_);

  vnl_double_3 diag_vector = max_point - min_point;
  vnl_vector_fixed<double,3> x_axis(0.0), y_axis(0.0), z_axis(0.0);
  x_axis[0] = diag_vector[0];
  y_axis[1] = diag_vector[1];
  z_axis[2] = diag_vector[2];

  double max_y=max_point(1);
  double min_y=min_point(1);
  vcl_cout<<min_point<<vcl_endl;


  max_y=-vcl_numeric_limits<double>::infinity();

  min_y= vcl_numeric_limits<double>::infinity();

  vcl_cout<<pts.size()<<vcl_endl;
  //normalization of y
  for (unsigned i=0;i<pts.size();i++) {
    //pts[i].set(pts[i].x(),(pts[i].y()-min_y)*(max_y-min_y),pts[i].z());
    if (pts[i].y()>max_y) max_y=pts[i].y();
    if (pts[i].y()<min_y) min_y=pts[i].y();

    vcl_cout<<pts[i]<<vcl_endl;
  }



  /*  for (unsigned i=0;i<point_3d_list_f.size();i++) {
  vnl_double_3 p(point_3d_list_f[i].x(),point_3d_list_f[i].y(),point_3d_list_f[i].z());

  pts_z.push_back(rot*p);
  vcl_cout<<rot*p<<vcl_endl;
  }*/


  vcl_vector<vnl_double_3> pts_z;
  vcl_vector<vnl_double_3> pts_y;
  for (unsigned i=0;i<pts.size();i++) {
    vnl_double_3 p0(pts[i].x(),pts[i].y(),pts[i].z());
    vnl_double_3 p1=rot_*p0;
    //vnl_double_3 p(pts[i].x(),(pts[i].y()-min_y)/(max_y-min_y),pts[i].z());
    vnl_double_3 p(p1[0],(p1[1]-min_y)/(max_y-min_y),p1[2]);
    pts_z.push_back(p);
  }



  //vcl_sort(pts_z.begin(), pts_z.end(), dbcri_accu_edge_cmp_z);
  //vcl_vector<vnl_double_3> pts_x;

  // only consider points in x and y above 0.5 feet  //i don't use it..
  for ( vcl_vector<vnl_double_3>::const_iterator itr = pts_z.begin();
    itr != pts_z.end();  ++itr)
  {
    //if ((*itr)[2] > 0.5) { pts_x.push_back(*itr); }
    pts_y.push_back(*itr); 
  }

  //vcl_vector<vnl_double_3> pts_y = pts_x;

  // vcl_sort(pts_x.begin(), pts_x.end(), dbcri_accu_edge_cmp_x);
  vcl_sort(pts_y.begin(), pts_y.end(), dbcri_accu_edge_cmp_y);



  //#mothod 1... find out lier
  vcl_vector<int> bin(BIN_SIZE_,0);
  for (unsigned i=0;i<pts_y.size(); i++) {


    double r=(pts_y[i])[1];


    if (vcl_floor(BIN_SIZE_*r)>=0&&vcl_floor(BIN_SIZE_*r)<=99)
      bin[(int) vcl_floor(BIN_SIZE_*r)]++;


  }

  // find max bin
  int max_bin=-100;
  int max_bin_id=-100;
  double mean_r=0.0;
  int counter=0; 

  //NOTICE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //***************// BIN_SIZE_/2 avoid driver side ...**********
  for (unsigned i=0;i<(int)(BIN_SIZE_/2.0);i++) {

    vcl_cout<<bin[i]<<vcl_endl;
    if (bin[i]>max_bin) {
      max_bin=bin[i];
      max_bin_id=i;
    }
  }
  //vcl_cout<<mean_r/(double)counter<<vcl_endl;

  // check !!! fix later..// min_point and min_y is different
  r=max_bin_id*(max_y-min_y)/100.0+min_y-min_point(1);// min_point and min_y is different
  // // min_point and min_y is different

  //display_0_frame_=false; // display sample needed for 0 frame once..
  return;

}


void dbcri_whole_process::spacial_filtering(double &r, vcl_vector <vgl_point_3d <double> > pts) {


  vnl_double_3 min_point(min_point_);
  vnl_double_3 max_point(max_point_);

  vnl_double_3 diag_vector = max_point - min_point;
  vnl_vector_fixed<double,3> x_axis(0.0), y_axis(0.0), z_axis(0.0);
  x_axis[0] = diag_vector[0];
  y_axis[1] = diag_vector[1];
  z_axis[2] = diag_vector[2];

  double max_y=max_point(1);
  double min_y=min_point(1);
  vcl_cout<<"::space_filtering"<<vcl_endl;
  vcl_cout<<min_point<<vcl_endl;


  max_y=-vcl_numeric_limits<double>::infinity();

  min_y= vcl_numeric_limits<double>::infinity();

  vcl_cout<<pts.size()<<vcl_endl;
  //normalization of y
  for (unsigned i=0;i<pts.size();i++) {
    //pts[i].set(pts[i].x(),(pts[i].y()-min_y)*(max_y-min_y),pts[i].z());
    if (pts[i].y()>max_y) max_y=pts[i].y();
    if (pts[i].y()<min_y) min_y=pts[i].y();

    vcl_cout<<pts[i]<<vcl_endl;
  }






  vcl_vector<vnl_double_3> pts_z;
  vcl_vector<vnl_double_3> pts_y;
  for (unsigned i=0;i<pts.size();i++) {
    vnl_double_3 p0(pts[i].x(),pts[i].y(),pts[i].z());
    vnl_double_3 p1=rot_*p0;
    //vnl_double_3 p(pts[i].x(),(pts[i].y()-min_y)/(max_y-min_y),pts[i].z());
    vnl_double_3 p(p1[0],(p1[1]-min_y)/(max_y-min_y),p1[2]);
    pts_z.push_back(p);
  }



  //vcl_sort(pts_z.begin(), pts_z.end(), dbcri_accu_edge_cmp_z);
  //vcl_vector<vnl_double_3> pts_x;

  // only consider points in x and y above 0.5 feet  //i don't use it..
  for ( vcl_vector<vnl_double_3>::const_iterator itr = pts_z.begin();
    itr != pts_z.end();  ++itr)
  {
    //if ((*itr)[2] > 0.5) { pts_x.push_back(*itr); }
    pts_y.push_back(*itr); 
  }

  //vcl_vector<vnl_double_3> pts_y = pts_x;

  // vcl_sort(pts_x.begin(), pts_x.end(), dbcri_accu_edge_cmp_x);
  vcl_sort(pts_y.begin(), pts_y.end(), dbcri_accu_edge_cmp_y);



  //#mothod 1... find out lier
  vcl_vector<int> bin(BIN_SIZE_,0);
  for (unsigned i=0;i<pts_y.size(); i++) {


    double r=(pts_y[i])[1];


    if (vcl_floor(BIN_SIZE_*r)>=0&&vcl_floor(BIN_SIZE_*r)<=99)
      bin[(int) vcl_floor(BIN_SIZE_*r)]++;


  }

  // find max bin
  int max_bin=-100;
  int max_bin_id=-100;
  double mean_r=0.0;
  int counter=0; 


  //NOTICE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //***************// BIN_SIZE_/2 avoid driver side ...**********
  for (unsigned i=0;i<(int)(BIN_SIZE_/2.0);i++) {// BIN_SIZE_/2 avoid driver side ...**********
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

    vcl_cout<<bin[i]<<vcl_endl;
    if (bin[i]>max_bin) {
      max_bin=bin[i];
      max_bin_id=i;
    }
  }
  //vcl_cout<<mean_r/(double)counter<<vcl_endl;

  // check !!! fix later..// min_point and min_y is different
  r=max_bin_id*(max_y-min_y)/100.0+min_y-min_point(1);// min_point and min_y is different
  // // min_point and min_y is different

  //display_0_frame_=false; // display sample needed for 0 frame once..


  vcl_vector<vnl_double_3> sel_pts_y;

  for (unsigned i=0;i<pts_y.size(); i++) {


    double near=(pts_y[i])[1];

    vcl_cout<<vcl_fabs(near-max_bin_id/100.0)<<vcl_endl;

    if ( vcl_fabs(near-max_bin_id/100.0)<0.1) {
      sel_pts_y.push_back(pts_y[i]);
    }


  }

  vcl_sort(sel_pts_y.begin(), sel_pts_y.end(), dbcri_accu_edge_cmp_x);

  double max_x=-vcl_numeric_limits<double>::infinity();

  double min_x= vcl_numeric_limits<double>::infinity();

  vcl_cout<<sel_pts_y.size()<<vcl_endl;
  //normalization of y
  for (unsigned i=0;i<sel_pts_y.size();i++) {
    //pts[i].set(pts[i].x(),(pts[i].y()-min_y)*(max_y-min_y),pts[i].z());
    if (sel_pts_y[i][0]>max_x) max_x=sel_pts_y[i][0];
    if (sel_pts_y[i][0]<min_x) min_x=sel_pts_y[i][0];

    vcl_cout<<sel_pts_y[i]<<vcl_endl;
  }


  vcl_vector<int> binx(21,0);
  for (unsigned i=0;i<sel_pts_y.size(); i++) {


    double r=((sel_pts_y[i])[0]-min_x)/(max_x-min_x);

    vcl_cout<<r<<vcl_endl;

    binx[(int) vcl_floor(20*r)]++;
  }


  max_bin=-1000;
  for (unsigned i=0;i<(21);i++) {

    vcl_cout<<binx[i]<<vcl_endl;
    if (binx[i]>max_bin) {
      max_bin=binx[i];
      max_bin_id=i;
    }
  }



  vcl_cout<<"r: "<<r<<" ::space_filtering end"<<vcl_endl;
  // return;

}


void dbcri_whole_process::spacial_filtering_x(double &r1,double &r2, vcl_vector <vgl_point_3d <double> > pts) {

  vcl_cout<<"==========================================================="<<vcl_endl;
  vcl_cout<<"r1& r2: "<<r1<<" "<<r2<<" ::space_filtering_x start"<<vcl_endl;
  vcl_cout<<"==========================================================="<<vcl_endl;
  vnl_double_3 min_point(min_point_);
  vnl_double_3 max_point(max_point_);

  vnl_double_3 diag_vector = max_point - min_point;
  vnl_vector_fixed<double,3> x_axis(0.0), y_axis(0.0), z_axis(0.0);
  x_axis[0] = diag_vector[0];
  y_axis[1] = diag_vector[1];
  z_axis[2] = diag_vector[2];

  double max_y=max_point(1);
  double min_y=min_point(1);
  // vcl_cout<<min_point<<vcl_endl;
  double max_z0,min_z0;

  double max_x=max_point(0);
  double min_x=min_point(0);
  vcl_cout<<"min_point:"<<min_point<<vcl_endl;
  vcl_cout<<"max_point:"<<max_point<<vcl_endl;


  max_y=-vcl_numeric_limits<double>::infinity();

  min_y= vcl_numeric_limits<double>::infinity();
  max_x=-vcl_numeric_limits<double>::infinity();

  min_x= vcl_numeric_limits<double>::infinity();

  vcl_cout<<"spacial filtering x pts.size: "<<pts.size()<<vcl_endl;
  //normalization of y
  for (unsigned i=0;i<pts.size();i++) {
    //pts[i].set(pts[i].x(),(pts[i].y()-min_y)*(max_y-min_y),pts[i].z());
    if (pts[i].y()>max_y) max_y=pts[i].y();
    if (pts[i].y()<min_y) min_y=pts[i].y();

    if (pts[i].z()>max_z0) max_z0=pts[i].z();
    if (pts[i].z()<min_z0) min_z0=pts[i].z();
    //  vcl_cout<<pts[i]<<vcl_endl;
  }


  /*  for (unsigned i=0;i<point_3d_list_f.size();i++) {
  vnl_double_3 p(point_3d_list_f[i].x(),point_3d_list_f[i].y(),point_3d_list_f[i].z());

  pts_z.push_back(rot*p);
  vcl_cout<<rot*p<<vcl_endl;
  }*/


  vcl_vector<vnl_double_3> pts_x;
  vcl_vector<vnl_double_3> pts_temp;
  vcl_vector<vnl_double_3> pts_z;
  vcl_vector<vnl_double_3> pts_y;
  for (unsigned i=0;i<pts.size();i++) {
    vnl_double_3 p0(pts[i].x(),pts[i].y(),pts[i].z());
    vnl_double_3 p1=rot_*p0;

    //vnl_double_3 p(p1[0],(p1[1]-min_y)/(max_y-min_y),p1[2]);
    vnl_double_3 p(p1[0],p1[1],p1[2]);
    pts_temp.push_back(p);

  }


  //find max x and min x after rotation...
  for (unsigned i=0;i<pts_temp.size();i++) {
    //pts[i].set(pts[i].x(),(pts[i].y()-min_y)*(max_y-min_y),pts[i].z());
    if (pts_temp[i][0]>max_x) max_x=pts_temp[i][0];
    if (pts_temp[i][0]<min_x) min_x=pts_temp[i][0];

    //vcl_cout<<pts[i]<<vcl_endl;
  }


  for (unsigned i=0;i<pts_temp.size();i++) {
    vnl_double_3 p((pts_temp[i][0]-min_x)/(max_x-min_x),pts_temp[i][1],pts_temp[i][2]);


    //vnl_double_3 p(p1[0],(p1[1]-min_y)/(max_y-min_y),p1[2]);
    //vnl_double_3 p(p1[0],p1[0],p1[2]);
    pts_x.push_back(p);

  }

  //vcl_sort(pts_z.begin(), pts_z.end(), dbcri_accu_edge_cmp_z);
  //vcl_vector<vnl_double_3> pts_x;

  // only consider points in x and y above 0.5 feet  //i don't use it..
  for ( vcl_vector<vnl_double_3>::const_iterator itr = pts_z.begin();
    itr != pts_z.end();  ++itr)
  {
    //if ((*itr)[2] > 0.5) { pts_x.push_back(*itr); }
    pts_y.push_back(*itr); 
  }

  //vcl_vector<vnl_double_3> pts_y = pts_x;

  // vcl_sort(pts_x.begin(), pts_x.end(), dbcri_accu_edge_cmp_x);
  vcl_sort(pts_x.begin(), pts_x.end(), dbcri_accu_edge_cmp_x);



  //#mothod 1... find out lier
  vcl_vector<int> bin_1(BIN_SIZE_,0);
  for (unsigned i=0;i<vcl_floor(pts_x.size()/5.0); i++) {// 6-22-07 if there is outlier, problem!
    // i,e. if x={-100, 0, 1,1, 2, 3, 4, 5, 6,6,6,7.1,10,11}
    // x=-100 makes problem.... since it search only 1/5 portion..
    double r=(pts_x[i])[0];
    if (vcl_floor(BIN_SIZE_*r)>=0&&vcl_floor(BIN_SIZE_*r)<=99)
    {
      if ( (int) vcl_floor(BIN_SIZE_*r-1)>=0)
        bin_1[(int) vcl_floor(BIN_SIZE_*r)-1]++;

      bin_1[(int) vcl_floor(BIN_SIZE_*r)]++;bin_1[(int) vcl_floor(BIN_SIZE_*r)]++;
      if ((int) vcl_floor(BIN_SIZE_*r+1)<=99)
        bin_1[(int) vcl_floor(BIN_SIZE_*r)+1]++;
    }
  }

  vcl_vector<int> bin_2(BIN_SIZE_,0);
  for (unsigned i=vcl_floor(pts_x.size()*4/5.0);i<pts_x.size(); i++) {
    double r=(pts_x[i])[0];
    if (vcl_floor(BIN_SIZE_*r)>=0&&vcl_floor(BIN_SIZE_*r)<=99)
    {
      if ((int) vcl_floor(BIN_SIZE_*r-1)>=0)
        bin_2[(int) vcl_floor(BIN_SIZE_*r)-1]++;

      bin_2[(int) vcl_floor(BIN_SIZE_*r)]++;bin_2[(int) vcl_floor(BIN_SIZE_*r)]++;
      if ((int) vcl_floor(BIN_SIZE_*r+1)<=99)
        bin_2[(int) vcl_floor(BIN_SIZE_*r)+1]++;
    }

  }


  // find max bin
  int max_bin_1=-100;
  int max_bin_id_1=-100;
  int max_bin_2=-100;
  int max_bin_id_2=-100;
  double mean_r=0.0;
  int counter=0; 

  //NOTICE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //***************// BIN_SIZE_/2 avoid driver side ...**********
  for (unsigned i=0;i<(int)(BIN_SIZE_/2.0);i++) {

    vcl_cout<<bin_1[i]<<vcl_endl;
    if (bin_1[i]>max_bin_1) {
      max_bin_1=bin_1[i];
      max_bin_id_1=i;
    }
  }


  vcl_cout<<"====front and top ==================="<<vcl_endl;
  //NOTICE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //***************// BIN_SIZE_/2 avoid rear side ...**********
  for (unsigned i=(int)(BIN_SIZE_/2.0);i<BIN_SIZE_;i++) {

    vcl_cout<<bin_2[i]<<vcl_endl;
    if (bin_2[i]>max_bin_2) {
      max_bin_2=bin_2[i];
      max_bin_id_2=i;
    }
  }


  //vcl_cout<<mean_r/(double)counter<<vcl_endl;

  // check !!! fix later..// min_point and min_y is different
  r1=max_bin_id_1*(max_x-min_x)/100.0+min_x-min_point(0);// min_point and min_y is different
  r2=max_bin_id_2*(max_x-min_x)/100.0+min_x-min_point(0);// min_point and min_y is different
  // // min_point and min_y is different

  //display_0_frame_=false; // display sample needed for 0 frame once..


  double m_x_1=0.0,m_y_1=0.0,m_z_1=0.0;
  double m_x_2=0.0,m_y_2=0.0,m_z_2=0.0;
  double count1=0,count2=0;

  double max_z=-100.0;
  for (unsigned i=0;i<pts_x.size();i++) {

    double r=(pts_x[i])[0];
    if ((int) vcl_floor(BIN_SIZE_*r)<max_bin_id_1+1&&(int) vcl_floor(BIN_SIZE_*r)>max_bin_id_1-1)
    {

      double temp_x=(max_x-min_x)*r+min_x;
      m_x_1+=temp_x;
      m_y_1+=pts_x[i][1];
      m_z_1+=pts_x[i][2];

      count1++;
    }


    if ((int) vcl_floor(BIN_SIZE_*r)<max_bin_id_2+1&&(int) vcl_floor(BIN_SIZE_*r)>max_bin_id_2-1)
    {

      double temp_x=(max_x-min_x)*r+min_x;
      m_x_2+=temp_x;
      m_y_2+=pts_x[i][1];
      m_z_2+=pts_x[i][2];
      if (max_z<pts_x[i][2]) max_z=pts_x[i][2];
      count2++;
    }

  }
  vnl_double_3x3 inv_rot = inv_rot_;
  vnl_double_3 pm1_temp(m_x_1/count1,m_y_1/count1,m_z_1/count1);
  vnl_double_3 pm1=inv_rot*pm1_temp;

  //vnl_double_3 pm2_temp(m_x_2/count2,m_y_2/count2,m_z_2/count2);
  vnl_double_3 pm2_temp(m_x_2/count2,m_y_2/count2,max_z); //use maximum z for probe 7'th z location
  vnl_double_3 pm2=inv_rot*pm2_temp;

  vcl_cout<<"front & rear points:"<<pm1<<" "<<pm2<<vcl_endl;
  vcl_cout<<"max z- probe 7'th z:"<<max_z-pm2(2)<<vcl_endl;

  p7_=pm1;// actually, this is probe 0
  p0_=pm2;// actually, this is probe 7


  //////////////////////////////////////////

  //


  vcl_cout<<"r1& r2: "<<r1<<" "<<r2<<" ::space_filtering_x end"<<vcl_endl;

  vcl_cout<<"==========================================================="<<vcl_endl;
  vcl_cout<<"==========================================================="<<vcl_endl;

  return;




}

double dbcri_whole_process::bb_contour() {




  return 1.0;
}



void dbcri_whole_process::read_bb_and_probe(float scale, double &cube_x, double &cube_y, double &cube_z,
                                            vgl_point_3d<double> &X,int  probe_id, int class_id)
{

  double x,y,z;
  if (class_id==0) 
    switch(probe_id)
  {
    case 0 :    x=-179.21471; y= -25.48460;  z= 28.59190;  cube_x=12.97000;  cube_y= 6.39340;  cube_z= 7.46310;         break;
    case 1 :    x=-132.89429; y= -22.26056;  z= 42.49110;  cube_x= 7.18900;  cube_y= 4.76700;  cube_z= 4.86460;         break;
    case 2 :    x=-102.15789; y= -23.64121;  z= 42.24343;  cube_x= 6.18280;  cube_y= 4.44620;  cube_z= 3.01100;         break;
    case 3 :    x=-98.18877;  y= -30.71624;  z= 27.09789;  cube_x= 5.98590;  cube_y= 4.40370;  cube_z= 3.62240;         break;
    case 4 :    x=-81.45436;  y= -22.11407;  z= 41.59693;  cube_x= 8.65970;  cube_y= 2.83270;  cube_z= 3.51150;         break;
    case 5 :    x=-57.59297;  y= -28.70307;  z= 26.55219;  cube_x=10.84180;  cube_y= 4.76220;  cube_z= 4.84040;         break;
    case 6 :    x=-12.12567;  y= -27.03229;  z= 18.72117;  cube_x= 4.24410;  cube_y= 3.92240;  cube_z= 2.59930;   break;
    case 7 :    x=-12.23820;  y=  26.67023;  z= 18.98727;  cube_x= 4.93660;  cube_y= 3.81930;  cube_z= 1.78740;         break;
    case 8 :    x=-57.12216;  y=  28.55730;  z= 26.44823;  cube_x= 9.86200;  cube_y= 4.98380;  cube_z= 2.76390;         break;
    case 9 :    x=-81.82614;  y=  22.49641;  z= 41.58564;  cube_x= 7.21800;  cube_y= 3.10390;  cube_z= 2.85120;         break;
    case 10:    x=-179.21471; y= +25.48460;  z= 28.59190;  cube_x=12.97000;  cube_y= 6.39340;  cube_z= 7.46310;         break;
    case 11:    x=-132.89429; y= +22.26056;  z= 42.49110;  cube_x= 7.18900;  cube_y= 4.76700;  cube_z= 4.86460;         break;
    case 12:                    break;
    case 13:                    break;

    default:  break; // this will never happen
  }
  //sedan
  /*  0 -179.21471  -25.48460   28.59190   12.97000    6.39340    7.46310
  1 -132.89429  -22.26056   42.49110    7.18900    4.76700    4.86460
  2 -102.15789  -23.64121   42.24343    6.18280    4.44620    3.01100
  3  -98.18877  -30.71624   27.09789    5.98590    4.40370    3.62240
  4  -81.45436  -22.11407   41.59693    8.65970    2.83270    3.51150
  5  -57.59297  -28.70307   26.55219   10.84180    4.76220    4.84040
  6  -12.12567  -27.03229   18.72117    4.24410    3.92240    2.59930
  7  -12.23820   26.67023   18.98727    4.93660    3.81930    1.78740
  8  -57.12216   28.55730   26.44823    9.86200    4.98380    2.76390
  9  -81.82614   22.49641   41.58564    7.21800    3.10390    2.85120
  10  -54.61116  -31.05936    0.00000    9.91360    0.00000    6.44160
  11    0.00000   -0.00004    8.33844    0.00000    5.96009    0.00877
  */



  if (class_id==1) 
    switch(probe_id)
  {
    case 0 : x= -172.91833; y=  -30.13112;  z=  30.19587;  cube_x=10.38700 ;  cube_y=   7.46940;  cube_z=    3.44540;           break;
    case 1 : x= -162.98083; y=  -23.80763;  z=  50.85197;  cube_x=11.55100;  cube_y=    8.99240;  cube_z=    5.95980;           break;
    case 2 : x= -95.42235;  y=  -26.12093;  z=  48.01413;  cube_x=12.22960 ;  cube_y=   8.72150;  cube_z=    2.70570;           break;
    case 3 : x= -94.42485;  y=  -30.89255;  z=  31.54360;  cube_x=10.12040;  cube_y=    6.78130;  cube_z=    2.97720;           break;
    case 4 : x= -73.21947;  y=  -24.48868;  z=  48.17738;  cube_x=10.70360;  cube_y=    7.92530;  cube_z=    3.49310;           break;
    case 5 : x= -54.77978;  y=  -28.78197;  z=  32.33453;  cube_x= 9.52330;  cube_y=    7.59310;  cube_z=    1.74170;           break;
    case 6 : x= -9.01974;   y=  -27.10253;  z=  24.14972;  cube_x= 8.19839;  cube_y=    7.95370;  cube_z=   11.26400;           break;
    case 7 : x= -8.92432;   y=   26.29308;  z=  24.41268;  cube_x= 7.36289;  cube_y=    7.28910;  cube_z=   11.13400;           break;
    case 8 : x= -54.49910;  y=   28.81713;  z=  32.24610;  cube_x=10.78870 ;  cube_y=   8.53330;  cube_z=    1.81310;   break;
    case 9 : x= -73.28730 ; y=   24.19770;  z=  48.27782;  cube_x=10.53240 ;  cube_y=   7.51110;  cube_z=    4.48150;           break;

    case 10: x= -172.91833; y=  +30.13112;  z=  30.19587;  cube_x=10.38700 ;  cube_y=   7.46940;  cube_z=    3.44540;           break;
    case 11: x= -162.98083; y=  +23.80763;  z=  50.85197;  cube_x=11.55100;   cube_y=   8.99240;  cube_z=    5.95980;           break;
    case 12:                    break;
    case 13:                    break;

    default:  break; // this will never happen
  }
  //suv
  /*0 -172.91833  -30.13112   30.19587   10.38700    7.46940    3.44540
  1 -162.98083  -23.80763   50.85197   11.55100    8.99240    5.95980
  2  -95.42235  -26.12093   48.01413   12.22960    8.72150    2.70570
  3  -94.42485  -30.89255   31.54360   10.12040    6.78130    2.97720
  4  -73.21947  -24.48868   48.17738   10.70360    7.92530    3.49310
  5  -54.77978  -28.78197   32.33453    9.52330    7.59310    1.74170
  6   -9.01974  -27.10253   24.14972    8.19839    7.95370   11.26400
  7   -8.92432   26.29308   24.41268    7.36289    7.28910   11.13400
  8  -54.49910   28.81713   32.24610   10.78870    8.53330    1.81310
  9  -73.28730   24.19770   48.27782   10.53240    7.51110    4.48150
  10  -51.36803  -32.44052    0.00000    8.44800    0.00000    5.16380
  11    0.00000    0.98671    8.96675    0.00000   15.57830    5.91014
  */
  if (class_id==2) 
    switch(probe_id)
  {
    case 0 : x= -182.70140; y=  -33.00098;  z=  36.03196;  cube_x= 11.32500;  cube_y=  2.66220;  cube_z=  5.30500;              break;
    case 1 : x= -172.25520; y=  -25.58108;  z=  57.31650;  cube_x= 10.41800;  cube_y=  8.78890;  cube_z=  6.26010;              break;
    case 2 : x= -93.23690;  y=  -28.57264;  z=  54.92082;  cube_x= 18.29850;  cube_y=  8.09550;  cube_z=  4.93300;              break;
    case 3 : x= -92.03020;  y=  -35.13788;  z=  35.35490;  cube_x= 15.75600;  cube_y=  5.29950;  cube_z=  4.74010;              break;
    case 4 : x= -71.11060;  y=  -25.93910;  z=  54.39806;  cube_x= 17.44500;  cube_y=  8.24900;  cube_z=  4.54850;              break;
    case 5 : x= -45.97132;  y=  -31.52034;  z=  35.75554;  cube_x= 14.54310;  cube_y=  5.06220;  cube_z=  2.10600;              break;
    case 6 : x= -9.98616;   y=  -25.91536;  z=  23.66608;  cube_x= 6.10600;   cube_y=  9.14360;  cube_z= 15.62190;              break;
    case 7 : x= -9.95002;   y=   25.62242;  z=  23.88524;  cube_x= 6.10850;   cube_y=  9.14360;  cube_z= 15.62690;              break;
    case 8 : x= -46.07720;  y=   31.31362;  z=  35.90390;  cube_x= 14.54140;  cube_y=  5.06220;  cube_z=  3.15410;              break;
    case 9 : x= -71.21410;  y=   25.79662;  z=  54.47188;  cube_x= 17.44350;  cube_y=  8.24900;  cube_z=  4.91610;              break;

    case 10: x= -182.70140; y=  +33.00098;  z=  36.03196;  cube_x= 11.32500;  cube_y=  2.66220;  cube_z=  5.30500;                      break;
    case 11: x= -172.25520; y=  +25.58108;  z=  57.31650;  cube_x= 10.41800;  cube_y=  8.78890;  cube_z=  6.26010;                      break;
    case 12:                    break;
    case 13:                    break;

    default:  break; // this will never happen
  }
  //van
  /*  
  0 -182.70140  -33.00098   36.03196   11.32500    2.66220    5.30500
  1 -172.25520  -25.58108   57.31650   10.41800    8.78890    6.26010
  2  -93.23690  -28.57264   54.92082   18.29850    8.09550    4.93300
  3  -92.03020  -35.13788   35.35490   15.75600    5.29950    4.74010
  4  -71.11060  -25.93910   54.39806   17.44500    8.24900    4.54850
  5  -45.97132  -31.52034   35.75554   14.54310    5.06220    2.10600
  6   -9.98616  -25.91536   23.66608    6.10600    9.14360   15.62190
  7   -9.95002   25.62242   23.88524    6.10850    9.14360   15.62690
  8  -46.07720   31.31362   35.90390   14.54140    5.06220    3.15410
  9  -71.21410   25.79662   54.47188   17.44350    8.24900    4.91610
  10  -52.44508  -32.67410    0.00000    9.30490    0.00000    6.65280
  11    0.00000    0.31814    9.79452    0.00000    6.25540    1.59071
  */

  // 7-26-06
  if (probe_id==0) z=z-12;

  X.set(-scale*x,-scale*z,-scale*y); 
  cube_x*=scale;cube_y*=scale;cube_z*=scale;
  //     vcl_cout<<probe_id<<X<<cube_x<<" "<<cube_y<<" "<<cube_z<<vcl_endl;
}

void dbcri_whole_process::read_bb_and_probe_test(float scale, double &cube_x, double &cube_y, double &cube_z,  //test for same cube_size
                                                 vgl_point_3d<double> &X,int  probe_id, int class_id)
{

  double x,y,z;
  if (class_id==0) 
    switch(probe_id)
  {
    case 0 :    x=-179.21471; y= -25.48460;  z= 28.59190;  cube_x=12.97000;  cube_y= 6.39340;  cube_z= 7.46310;         break;
    case 1 :    x=-132.89429; y= -22.26056;  z= 42.49110;  cube_x= 7.18900;  cube_y= 4.76700;  cube_z= 4.86460;         break;
      //case 2 :    x=-102.15789; y= -23.64121;  z= 42.24343;  cube_x= 6.18280;  cube_y= 4.44620;  cube_z= 3.01100;         break;
    case 2 :    x=-150.15789; y= -23.64121;  z= 30.24343;  cube_x= 6.18280;  cube_y= 4.44620;  cube_z= 3.01100;         break;
    case 3 :    x=-98.18877;  y= -30.71624;  z= 27.09789;  cube_x= 5.98590;  cube_y= 4.40370;  cube_z= 3.62240;         break;
    case 4 :    x=-81.45436;  y= -22.11407;  z= 41.59693;  cube_x= 8.65970;  cube_y= 2.83270;  cube_z= 3.51150;         break;
    case 5 :    x=-57.59297;  y= -28.70307;  z= 26.55219;  cube_x=10.84180;  cube_y= 4.76220;  cube_z= 4.84040;         break;
    case 6 :    x=-12.12567;  y= -27.03229;  z= 18.72117;  cube_x= 4.24410;  cube_y= 3.92240;  cube_z= 2.59930;   break;
    case 7 :    x=-12.23820;  y=  26.67023;  z= 18.98727;  cube_x= 4.93660;  cube_y= 3.81930;  cube_z= 1.78740;         break;
    case 8 :    x=-57.12216;  y=  28.55730;  z= 26.44823;  cube_x= 9.86200;  cube_y= 4.98380;  cube_z= 2.76390;         break;
    case 9 :    x=-81.82614;  y=  22.49641;  z= 41.58564;  cube_x= 7.21800;  cube_y= 3.10390;  cube_z= 2.85120;         break;
    case 10:    x=-179.21471; y= +25.48460;  z= 28.59190;  cube_x=12.97000;  cube_y= 6.39340;  cube_z= 7.46310;         break;
    case 11:    x=-132.89429; y= +22.26056;  z= 42.49110;  cube_x= 7.18900;  cube_y= 4.76700;  cube_z= 4.86460;         break;
    case 12:                    break;
    case 13:                    break;

    default:  break; // this will never happen
  }
  //sedan

  if (class_id==1) 
    switch(probe_id)
  {
    case 0 : x= -172.91833; y=  -30.13112;  z=  30.19587;  cube_x=12.97000;  cube_y= 6.39340;  cube_z= 7.46310;            break;
    case 1 : x= -162.98083; y=  -23.80763;  z=  50.85197;  cube_x= 7.18900;  cube_y= 4.76700;  cube_z= 4.86460;           break;
      //case 2 : x= -95.42235;  y=  -26.12093;  z=  48.01413;  cube_x=12.22960 ;  cube_y=   8.72150;  cube_z=    2.70570;    break;

    case 2 : x= -167.42235;  y=  -26.12093;  z=  40.01413;  cube_x=12.22960 ;  cube_y=   8.72150;  cube_z=    2.70570;    break;
    case 3 : x= -94.42485;  y=  -30.89255;  z=  31.54360;  cube_x=10.12040;  cube_y=    6.78130;  cube_z=    2.97720;           break;
    case 4 : x= -73.21947;  y=  -24.48868;  z=  48.17738;  cube_x=10.70360;  cube_y=    7.92530;  cube_z=    3.49310;           break;
    case 5 : x= -54.77978;  y=  -28.78197;  z=  32.33453;  cube_x= 9.52330;  cube_y=    7.59310;  cube_z=    1.74170;           break;
    case 6 : x= -9.01974;   y=  -27.10253;  z=  24.14972;  cube_x= 8.19839;  cube_y=    7.95370;  cube_z=   11.26400;           break;
    case 7 : x= -8.92432;   y=   26.29308;  z=  24.41268;  cube_x= 4.93660;  cube_y= 3.81930;  cube_z= 1.78740;            break;
    case 8 : x= -54.49910;  y=   28.81713;  z=  32.24610;  cube_x= 9.86200;  cube_y= 4.98380;  cube_z= 2.76390;    break;
    case 9 : x= -73.28730 ; y=   24.19770;  z=  48.27782;  cube_x= 7.21800;  cube_y= 3.10390;  cube_z= 2.85120;           break;

    case 10: x= -172.91833; y=  +30.13112;  z=  30.19587;  cube_x=10.38700 ;  cube_y=   7.46940;  cube_z=    3.44540;           break;
    case 11: x= -162.98083; y=  +23.80763;  z=  50.85197;  cube_x=11.55100;   cube_y=   8.99240;  cube_z=    5.95980;           break;
    case 12:                    break;
    case 13:                    break;

    default:  break; // this will never happen
  }
  //suv




  //van
  if (class_id==2) 
    switch(probe_id)
  {
    case 0 : x= -182.70140; y=  -33.00098;  z=  36.03196;  cube_x=12.97000;  cube_y= 6.39340;  cube_z= 7.46310;              break;
    case 1 : x= -172.25520; y=  -25.58108;  z=  57.31650;  cube_x= 7.18900;  cube_y= 4.76700;  cube_z= 4.86460;              break;
      //    case 2 : x= -93.23690;  y=  -28.57264;  z=  54.92082;  cube_x= 18.29850;  cube_y=  8.09550;  cube_z=  4.93300;              break;
    case 2 : x= -177.23690;  y=  -28.57264;  z=  47.92082;  cube_x= 18.29850;  cube_y=  8.09550;  cube_z=  4.93300;              break;
    case 3 : x= -92.03020;  y=  -35.13788;  z=  35.35490;  cube_x= 15.75600;  cube_y=  5.29950;  cube_z=  4.74010;              break;
    case 4 : x= -71.11060;  y=  -25.93910;  z=  54.39806;  cube_x= 17.44500;  cube_y=  8.24900;  cube_z=  4.54850;              break;
    case 5 : x= -45.97132;  y=  -31.52034;  z=  35.75554;  cube_x= 14.54310;  cube_y=  5.06220;  cube_z=  2.10600;              break;
    case 6 : x= -9.98616;   y=  -25.91536;  z=  23.66608;  cube_x= 6.10600;   cube_y=  9.14360;  cube_z= 15.62190;              break;
    case 7 : x= -9.95002;   y=   25.62242;  z=  23.88524;  cube_x= 4.93660;  cube_y= 3.81930;  cube_z= 1.78740;              break;
    case 8 : x= -46.07720;  y=   31.31362;  z=  35.90390;  cube_x= 9.86200;  cube_y= 4.98380;  cube_z= 2.76390;               break;
    case 9 : x= -71.21410;  y=   25.79662;  z=  54.47188;  cube_x= 7.21800;  cube_y= 3.10390;  cube_z= 2.85120;              break;

    case 10: x= -182.70140; y=  +33.00098;  z=  36.03196;  cube_x= 11.32500;  cube_y=  2.66220;  cube_z=  5.30500;                      break;
    case 11: x= -172.25520; y=  +25.58108;  z=  57.31650;  cube_x= 10.41800;  cube_y=  8.78890;  cube_z=  6.26010;                      break;
    case 12:                    break;
    case 13:                    break;

    default:  break; // this will never happen
  }
  //van

  X.set(-scale*x,-scale*z,-scale*y); 
  cube_x*=scale;cube_y*=scale;cube_z*=scale;
  //     vcl_cout<<probe_id<<X<<cube_x<<" "<<cube_y<<" "<<cube_z<<vcl_endl;
}


double dbcri_whole_process::Estimation_BB_and_bb_using_convex_hull_in2D(int some_index) {


  bool debug_1=false;

  //vnl_matrix <double> R(BB_Rot_);
  vnl_matrix <double> RT(bb_xform_);
  //
  vnl_matrix <double> t(3,1,0.0);
  t[0][0]=0; t[1][0]=0;t[2][0]=7;


  vcl_vector <vnl_matrix<float> >p_list;
  vgl_point_3d<double> X;


  /*   double shift_x=BB_shift_.x(); // bug...should be changed 6-15-2004 fixed 6-16_04
  double shift_y=BB_shift_.y();
  double shift_z=BB_shift_.z();

  */

  ///////////

  vnl_vector_fixed <double,4 > x1(1,0,0,1);
  vnl_vector_fixed <double,4 > x2(1,1,0,1);
  vnl_vector <double> center_3d(.5*RT*(x1+x2));

  if (debug_1) vcl_cout<<RT<<vcl_endl;


  ///////////

  vnl_double_3x3 R(0.0);

  R[0][0]=RT[0][0];R[0][1]=RT[0][1];R[0][2]=RT[0][2];
  R[1][0]=RT[1][0];R[1][1]=RT[1][1];R[1][2]=RT[1][2];
  R[2][0]=RT[2][0];R[2][1]=RT[2][1];R[2][2]=RT[2][2];

  double Rx=0,Ry=0,Rz=0;

  double theta_x=90.0;
  double theta_y=0.0;
  double theta_z=180.0;


  //World CAM FULL AUTO  //1-11-2005
  vnl_double_3x3 R_out(0.0);

  rotate_bb(R, R_out,theta_x*vnl_math::pi/180.0,  theta_y*vnl_math::pi/180.0,theta_z*vnl_math::pi/180.0);
  if (debug_1) vcl_cout<<R_out<<vcl_endl;
  R=R_out;
  float Rx_new=-Rx,Ry_new=-Rz,Rz_new=-Ry;
  Rx=Rx_new;Ry=Ry_new;Rz=Rz_new;
  // World CAM END



  // normalize R along columns..

  Rx=vcl_sqrt(R[0][0]*R[0][0]+R[1][0]*R[1][0]+R[2][0]*R[2][0]);
  Ry=vcl_sqrt(R[0][1]*R[0][1]+R[1][1]*R[1][1]+R[2][1]*R[2][1]);
  Rz=vcl_sqrt(R[0][2]*R[0][2]+R[1][2]*R[1][2]+R[2][2]*R[2][2]);


  //Scale=Rz/vcl_abs(-28.0527 -18.6547);
  if (debug_1) vcl_cout<<"suggested scale"<<Rx/(vcl_abs( 198.7+130.053)/2.0)<<vcl_endl;
  if (debug_1) vcl_cout<<"Scale: "<<scale_<<"Rx: "<<Rx<<"Ry: "<<Ry<<"Rz: "<<Rz<<vcl_endl;

  //model dimension.. 9-17-04
  double Mx=vcl_abs( 198.7+130.053)/2.0;
  double My=vcl_abs(20.6846+37.5776);
  double Mz=vcl_abs(32.2621+24.6169);
  if (debug_1) vcl_cout<<"mean(?) model_dimension(Mx,My,Mz) is "<<Mx<<" "<<My<<" "<<Mz<<vcl_endl;
  if (debug_1) vcl_cout<<"and  "<<Rx/Mx<<" "<<Ry/My<<" "<<Rz/Mz<<vcl_endl;
  Mx=vcl_abs( 198.7);
  My=vcl_abs(20.6846)*2.0;
  Mz=vcl_abs(32.2621)*2.0;
  if (debug_1) vcl_cout<<"model_dimension(Mx,My,Mz) is "<<Mx<<" "<<My<<" "<<Mz<<vcl_endl;
  if (debug_1) vcl_cout<<"and  "<<Rx/Mx<<" "<<Ry/My<<" "<<Rz/Mz<<vcl_endl;
  //return;
  // model dimension ends..
  R.normalize_columns();

  if (debug_1) vcl_cout<<R<<vcl_endl;
  //R.normalize_rows();
  //vcl_cout<<"after normalization\n"<<R<<vcl_endl;
  //shift_x=RT[0][3];shift_y=RT[1][3];shift_z=RT[2][3];

  // 2-5-2005 shift insert
  //shift_x=center_3d(0);shift_y=center_3d(1);shift_z=center_3d(2);
  // feet -> inch conversion


  //*******
  //*******
  double shift_x=0,shift_y=0,shift_z=0,shift_x_arg=0,shift_y_arg=0,shift_z_arg=0;
  //*******

  double max_val=-100.0;
  double max_shift_x_arg, max_shift_y_arg, max_shift_z_arg;
  vnl_matrix <double> p8_max(3,8,0.0);
  // vcl_vector <vcl_vector <vgl_point_3d <double> > > p_list;
  vcl_vector <vnl_matrix <double> > p8_max_list;
  double total_val=-1000.0;
  vcl_vector <double> val_list;
  vcl_vector <vcl_vector <vgl_point_3d <double> > >p8_list;  //for all baby box projection
  for (shift_z_arg=0; shift_z_arg<=0; shift_z_arg+=6)  //at least z should move same way
    for (shift_x_arg=-24; shift_x_arg<=24; shift_x_arg+=6)
      for (shift_y_arg=-12; shift_y_arg<=12; shift_y_arg+=6){

        shift_x=center_3d(0)+shift_x_arg/12.0;shift_y=center_3d(1)+shift_y_arg/12.0;shift_z=center_3d(2)+shift_z_arg/12.0;


        /* vgl_point_3d<double> BB_shift_;

        BB_shift_.set(shift_x,shift_y,shift_z);

        //1-14-2005

        double Scale=vcl_fabs(1.0*Rx/(Mx));

        vcl_cout<<"Scale: "<<Scale<<vcl_endl;

        ///////////////////////////
        */



        double cube_x=0,cube_y=0,cube_z=0;
        double cube_x_pos=0,cube_y_pos=0,cube_z_pos=0;
        double cube_x_neg=0,cube_y_neg=0,cube_z_neg=0;

        double Box_Add=.3;//////////////////////check


        int class_sweep=0; int probe_id=1; //test

        vcl_vector <vgl_point_3d <double> > p8;
        p8_list.clear(); total_val=0.0;
        val_list.clear();

        for (  probe_id=0;probe_id<10;probe_id++) {  //probe_id
          if (probe_id==2||probe_id==3||probe_id==5||probe_id==6||probe_id==4) continue;
          // ||probe_id==7||probe_id==8||probe_id==9||probe_id==4||probe_id==6) continue; //
          for (class_sweep=0;class_sweep<=0;class_sweep++) {

            //read_bb_and_probe(1.0/12.0,cube_x,cube_y,cube_z,X,probe_id,class_sweep);
            read_bb_and_probe_test(1.0/12.0,cube_x,cube_y,cube_z,X,probe_id,some_index);




            // X.set(X.x()+back_spread*Scale,X.y()-up_spread*Scale,X.z()+left_spread*Scale);


            cube_x_pos=cube_x/2+Box_Add;cube_x_neg=cube_x/2+Box_Add;
            cube_y_pos=cube_y/2+Box_Add;cube_y_neg=cube_y/2+Box_Add;
            cube_z_pos=cube_z/2+Box_Add;cube_z_neg=cube_z/2+Box_Add;

            double x_center_3d=X.x();
            double y_center_3d=X.y();
            double z_center_3d=X.z();




            //d3_search_trip_2( X,cube_x_times*cube_x_pos,cube_x_times*cube_x_neg,cube_y_pos,cube_y_neg,cube_z_pos,cube_z_neg);

            ////vcl_vector <vgl_point_3d <double> > p8; //p8 bb corner points..

            p8.clear();
            for (double mix=x_center_3d-cube_x_neg;mix<=x_center_3d+cube_x_pos+0.01;mix+=cube_x_neg+cube_x_pos) {
              for (double miy=y_center_3d-cube_y_neg;miy<=y_center_3d+cube_y_pos+0.01;miy+=cube_y_neg+cube_y_pos) {
                for (double miz=z_center_3d-cube_z_neg;miz<=z_center_3d+cube_z_pos+0.01;miz+=cube_z_neg+cube_z_pos) {
                  //counter++;         
                  t[0][0]=mix;
                  t[1][0]=miy;
                  t[2][0]=miz;
                  vnl_matrix<double> tt4=(R*t);
                  X.set(tt4[0][0]+shift_x,tt4[1][0]+shift_y,tt4[2][0]+shift_z);
                  vgl_point_3d <double> p(X);
                  //vcl_cout<<p<<vcl_endl;
                  //vcl_cout<<mix<<" "<<miy<<" "<<miz<<vcl_endl;
                  p8.push_back(p);


                }
              }
            }

            p8_list.push_back(p8);
            // double val= bb_box_contour_measure(p8);
            double val= bb_box_contour_measure_clip(p8);


            val_list.push_back(val);

            total_val+=val;
          }//calss sweep, but it does not play
          ///vcl_cout<<total_val<<"        "<<shift_x_arg<<" "<<shift_y_arg<<" "<<shift_z_arg<<" "<<vcl_endl;
        } ////probe_id

        //double val_check=val_list_check(val_list);
        // total_val=val_check;

        // vcl_cout<<total_val<<"        "<<shift_x_arg<<" "<<shift_y_arg<<" "<<shift_z_arg<<" "<<vcl_endl;
        if (max_val<total_val) {
          max_val=total_val;
          p8_max_list.clear();
          max_shift_x_arg=shift_x_arg;
          max_shift_y_arg=shift_y_arg;
          max_shift_z_arg=shift_z_arg;
          for (unsigned i=0;i<p8_list.size();i++) {
            for (unsigned j=0;j<p8.size();j++){
              p8_max[0][j]=(p8_list[i])[j].x();
              p8_max[1][j]=(p8_list[i])[j].y();
              p8_max[2][j]=(p8_list[i])[j].z();
            }
            p8_max_list.push_back(p8_max);
          }

        }
      }

      vcl_cout<<"**max_val:"<<max_val<<"============== "<<max_shift_x_arg<<" "<<max_shift_y_arg<<" "<<max_shift_z_arg<<" "<<vcl_endl;




      draw_vsol(p8_max_list);



      return max_val;
}


double dbcri_whole_process::bb_box_contour_measure(vcl_vector<vgl_point_3d<double> > points) {

  //: Distance between point \a (x,y,z) and closest point on closed polygon \a (px[i],py[i]),pz[i]
  //template <class T>
  //double vgl_distance_to_closed_polygon(T const px[], T const py[], T const pz[], unsigned int n,
  //                                      T x, T y, T z);






  int frame=0;double all_sum=0.0;double max_distance_sum=0.0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
    //  ref_num +=points_inside_of_the_box(frame,  bb_xform);
    //  }

    vcl_vector<vgl_point_2d<double> > p2d;
    for (unsigned i=0;i<points.size();i++)
    {
      vnl_double_4 pt(points[i].x(),points[i].y(),points[i].z(),1.0);
      vnl_double_3 pt2=cameras_[frame]*pt;

      //vcl_cout<<pt2<<vcl_endl;
      vgl_point_2d<double> pt3(pt2[0]/pt2[2],pt2[1]/pt2[2]);
      //vcl_cout<<pt3<<vcl_endl;
      p2d.push_back(pt3);
    }
    vgl_convex_hull_2d<double> ch(p2d);
    vgl_polygon<double> polygon = ch.hull();


    double max_distance=0.0;
    double px[8],py[8];



    for (unsigned int s = 0; s < polygon.num_sheets(); ++s)
      for (unsigned int p = 0; p < polygon[s].size(); ++p)
      {
        //do_something(polygon[s][p].x(), polygon[s][p].y());
        px[p]=polygon[s][p].x();
        py[p]=polygon[s][p].y();

        //     vcl_cout<<px[p]<<" "<<py[p]<<vcl_endl;
      }

      int count=0;
      double sum=0.0;
      for (unsigned i=0;i<(dcl_[frame])->size();i++) {

        if ( polygon.contains(dcl_[frame]->point(i)->x(),dcl_[frame]->point(i)->y() ) ) { 


          double temp_x= (dcl_[frame]->point(i)->x());
          double temp_y= (dcl_[frame]->point(i)->y());
          //double ang=vcl_atan(temp_y/(temp_x));

          // vcl_cout<<ang<<vcl_endl;
          // if  (ang>-theta_pos_+.03 && ang<-theta_neg_-.03 ) 
          count++ ;



          //use exponential???
          double distance=vgl_distance_to_closed_polygon(px,  py, 8,temp_x, temp_y);

          sum+=distance*distance;
          if (distance>max_distance)
            max_distance=distance*distance;

        }

      }

      ///sum/=count;

      if (count==0) sum=0;
      ////////// vcl_cout<<sum<<" "<<count<<vcl_endl;;
      all_sum+=sum;
      max_distance_sum+=max_distance;
  }

  return all_sum;

  //return max_distance_sum;
}

double dbcri_whole_process::val_list_check(vcl_vector <double> val_list)
{

  double mean=0.0,var=0.0;
  for (unsigned i=0;i<val_list.size();i++) {
    mean+=val_list[i];
    // vcl_cout<<mean<<" ";
  }
  mean/=val_list.size();

  for (unsigned i=0;i<val_list.size();i++) {
    var+=(val_list[i]-mean)*(val_list[i]-mean);
    //  vcl_cout<<var<<" ";
  }
  var/=val_list.size();

  //vcl_cout<<"\n"<<var<<vcl_endl;

  double prob=0;
  vcl_vector <double> prob_list;
  for (unsigned i=0;i<val_list.size();i++) {
    prob=(val_list[i]-mean)*(val_list[i]-mean)/var;
    ///vcl_cout<<prob<<" ";
    prob_list.push_back(prob);
  }
  ///vcl_cout<<vcl_endl;


  vcl_sort(prob_list.begin(), prob_list.end(), dbcri_accu_edge_sort);

  /////return mean/var;
  vcl_cout<<prob_list[0]-prob_list[4]<<vcl_endl;
  return (prob_list[0]-prob_list[4]);
}

void dbcri_whole_process::draw_vsol(vcl_vector <vnl_matrix <double> >p8_max_list) {

  int        frame=0;
  if (p8_max_list.size()==6) 
    for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
      vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
      vcl_vector <vgl_point_2d<double> > points8;
      for (unsigned j=0;j<p8_max_list.size();j++) {

        vnl_matrix <double>  p8;
        p8=p8_max_list[j];

        //for (int i=0; i<6; ++i){
        //vnl_double_4 p((i/4)%2, (i/2)%2, i%2, 1);
        //vcl_cout<<p<<vcl_endl;

        vnl_double_4 pt(p8[0][0],p8[1][0],p8[2][0],1.0);

        //vcl_cout<<pt<<vcl_endl;
        vnl_double_3 pt2=cameras_[frame]*pt;
        //vcl_cout<<pt2<<vcl_endl;
        vgl_point_2d<double> p2d(pt2[0]/pt2[2],pt2[1]/pt2[2]);
        points8.push_back(p2d);

      } 
      vgl_convex_hull_2d<double> ch8(points8);
      vgl_polygon<double> poly8 = ch8.hull();



      vsol_polygon_2d_sptr poly_vsol = bsol_algs::poly_from_vgl(poly8);
      output_vsol->add_object( poly_vsol->cast_to_spatial_object() ,"output_bb_shift" );

      output_data_[frame].push_back(output_vsol);

    }
  else
    for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
      vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
      for (unsigned j=0;j<p8_max_list.size();j++) {

        vnl_matrix <double>  p8;
        p8=p8_max_list[j];
        vcl_vector <vgl_point_2d<double> > points8;
        for (int i=0; i<8; ++i){
          //vnl_double_4 p((i/4)%2, (i/2)%2, i%2, 1);
          //vcl_cout<<p<<vcl_endl;

          vnl_double_4 pt(p8[0][i],p8[1][i],p8[2][i],1.0);

          vnl_double_3 pt2=cameras_[frame]*pt;
          //vcl_cout<<pt2<<vcl_endl;
          vgl_point_2d<double> p2d(pt2[0]/pt2[2],pt2[1]/pt2[2]);
          points8.push_back(p2d);

        } 
        vgl_convex_hull_2d<double> ch8(points8);
        vgl_polygon<double> poly8 = ch8.hull();



        vsol_polygon_2d_sptr poly_vsol = bsol_algs::poly_from_vgl(poly8);
        output_vsol->add_object( poly_vsol->cast_to_spatial_object() ,"output_bb_shift" );

      }
      output_data_[frame].push_back(output_vsol);
    }

}



void dbcri_whole_process::rotate_bb( vnl_double_3x3 R_in, vnl_double_3x3 & R_out,double theta_x,double theta_y,double theta_z ) {

  vnl_double_3x3 Rx(0.0);
  vnl_double_3x3 Ry(0.0);
  vnl_double_3x3 Rz(0.0);


  //float theta_x=theta_x*vnl_math::pi/180.0;
  //z-axis
  Rz(0,0)=vcl_cos(theta_z);
  Rz(0,1)=vcl_sin(theta_z);
  Rz(1,0)=-vcl_sin(theta_z);
  Rz(1,1)=vcl_cos(theta_z);
  Rz(2,2)=1.0;

  //along y_axis
  Ry(0,0)=vcl_cos(theta_y);
  Ry(0,2)=-vcl_sin(theta_y);
  Ry(2,0)=vcl_sin(theta_y);
  Ry(2,2)=vcl_cos(theta_y);
  Ry(1,1)=1.0;


  //along x_axis
  Rx(1,1)=vcl_cos(theta_x);
  Rx(1,2)=vcl_sin(theta_x);
  Rx(2,1)=-vcl_sin(theta_x);
  Rx(2,2)=vcl_cos(theta_x);
  Rx(0,0)=1.0;

  R_out=R_in*Rz*Ry*Rx;
}



double dbcri_whole_process::generate_mean_contour_model(int class_index) {



  bool debug_1=false;

  //vnl_matrix <double> R(BB_Rot_);
  vnl_matrix <double> RT(bb_xform_);
  //
  vnl_matrix <double> t(3,1,0.0);
  t[0][0]=0; t[1][0]=0;t[2][0]=7;


  vcl_vector <vnl_matrix<float> >p_list;
  vgl_point_3d<double> X;



  vnl_vector_fixed <double,4 > x1(1,0,0,1);
  vnl_vector_fixed <double,4 > x2(1,1,0,1);
  vnl_vector <double> center_3d(.5*RT*(x1+x2));

  if (debug_1) vcl_cout<<RT<<vcl_endl;


  ///////////

  vnl_double_3x3 R(0.0);

  R[0][0]=RT[0][0];R[0][1]=RT[0][1];R[0][2]=RT[0][2];
  R[1][0]=RT[1][0];R[1][1]=RT[1][1];R[1][2]=RT[1][2];
  R[2][0]=RT[2][0];R[2][1]=RT[2][1];R[2][2]=RT[2][2];

  double Rx=0,Ry=0,Rz=0;

  double theta_x=90.0;
  double theta_y=0.0;
  double theta_z=180.0;


  //World CAM FULL AUTO  //1-11-2005
  vnl_double_3x3 R_out(0.0);

  rotate_bb(R, R_out,theta_x*vnl_math::pi/180.0,  theta_y*vnl_math::pi/180.0,theta_z*vnl_math::pi/180.0);
  if (debug_1) vcl_cout<<R_out<<vcl_endl;
  R=R_out;
  float Rx_new=-Rx,Ry_new=-Rz,Rz_new=-Ry;
  Rx=Rx_new;Ry=Ry_new;Rz=Rz_new;
  // World CAM END



  // normalize R along columns..

  Rx=vcl_sqrt(R[0][0]*R[0][0]+R[1][0]*R[1][0]+R[2][0]*R[2][0]);
  Ry=vcl_sqrt(R[0][1]*R[0][1]+R[1][1]*R[1][1]+R[2][1]*R[2][1]);
  Rz=vcl_sqrt(R[0][2]*R[0][2]+R[1][2]*R[1][2]+R[2][2]*R[2][2]);


  //Scale=Rz/vcl_abs(-28.0527 -18.6547);
  if (debug_1) vcl_cout<<"suggested scale"<<Rx/(vcl_abs( 198.7+130.053)/2.0)<<vcl_endl;
  if (debug_1) vcl_cout<<"Scale: "<<scale_<<"Rx: "<<Rx<<"Ry: "<<Ry<<"Rz: "<<Rz<<vcl_endl;

  //model dimension.. 9-17-04
  double Mx=vcl_abs( 198.7+130.053)/2.0;
  double My=vcl_abs(20.6846+37.5776);
  double Mz=vcl_abs(32.2621+24.6169);
  if (debug_1) vcl_cout<<"mean(?) model_dimension(Mx,My,Mz) is "<<Mx<<" "<<My<<" "<<Mz<<vcl_endl;
  if (debug_1) vcl_cout<<"and  "<<Rx/Mx<<" "<<Ry/My<<" "<<Rz/Mz<<vcl_endl;
  Mx=vcl_abs( 198.7);
  My=vcl_abs(20.6846)*2.0;
  Mz=vcl_abs(32.2621)*2.0;
  if (debug_1) vcl_cout<<"model_dimension(Mx,My,Mz) is "<<Mx<<" "<<My<<" "<<Mz<<vcl_endl;
  if (debug_1) vcl_cout<<"and  "<<Rx/Mx<<" "<<Ry/My<<" "<<Rz/Mz<<vcl_endl;
  //return;
  // model dimension ends..
  R.normalize_columns();

  if (debug_1) vcl_cout<<R<<vcl_endl;
  //R.normalize_rows();
  //vcl_cout<<"after normalization\n"<<R<<vcl_endl;
  //shift_x=RT[0][3];shift_y=RT[1][3];shift_z=RT[2][3];

  // 2-5-2005 shift insert
  //shift_x=center_3d(0);shift_y=center_3d(1);shift_z=center_3d(2);
  // feet -> inch conversion


  //*******
  //*******
  double shift_x=0,shift_y=0,shift_z=0,shift_x_arg=0,shift_y_arg=0,shift_z_arg=0;
  //*******

  double min_val=1000000000.0;
  double min_shift_x_arg, min_shift_y_arg, min_shift_z_arg;
  vnl_matrix <double> p8_min(3,8,0.0);
  // vcl_vector <vcl_vector <vgl_point_3d <double> > > p_list;
  vcl_vector <vnl_matrix <double> > p8_min_list;
  double total_val=-1000.0;
  vcl_vector <vcl_vector <vgl_point_3d <double> > >p8_list;  //for all baby box projection
  for (shift_z_arg=0; shift_z_arg<=0; shift_z_arg+=6)  //at least z should move same way
    for (shift_x_arg=-48; shift_x_arg<=12; shift_x_arg+=6)
      for (shift_y_arg=-12; shift_y_arg<=12; shift_y_arg+=6){

        shift_x=center_3d(0)+shift_x_arg/12.0;shift_y=center_3d(1)+shift_y_arg/12.0;shift_z=center_3d(2)+shift_z_arg/12.0;


        /* vgl_point_3d<double> BB_shift_;

        BB_shift_.set(shift_x,shift_y,shift_z);

        //1-14-2005

        double Scale=vcl_fabs(1.0*Rx/(Mx));

        vcl_cout<<"Scale: "<<Scale<<vcl_endl;

        ///////////////////////////
        */



        double cube_x=0,cube_y=0,cube_z=0;
        double cube_x_pos=0,cube_y_pos=0,cube_z_pos=0;
        double cube_x_neg=0,cube_y_neg=0,cube_z_neg=0;

        double Box_Add=.3;//////////////////////check


        int class_sweep=0; int probe_id=1; //test

        vcl_vector <vgl_point_3d <double> > p8;
        p8_list.clear(); total_val=0.0;
        p8.clear();
        vcl_vector<int> probe_list;
        probe_list.push_back(0);
        probe_list.push_back(2);
        probe_list.push_back(1);
        probe_list.push_back(9);
        probe_list.push_back(8);
        probe_list.push_back(7);
        //prob_list.push_back(0);
        //prob_list.push_back(0);

        for (  probe_id=0;probe_id<probe_list.size();probe_id++) {  //probe_id
          //if (probe_id==2||probe_id==3||probe_id==5||probe_id==6||probe_id==11) continue;
          //||probe_id==7||probe_id==8||probe_id==9||probe_id==4||probe_id==6) continue; //
          //for (class_sweep=0;class_sweep<=0;class_sweep++) {

          //read_bb_and_probe(1.0/12.0,cube_x,cube_y,cube_z,X,probe_id,class_sweep);
          read_bb_and_probe_test(1.0/12.0,cube_x,cube_y,cube_z,X,probe_list[probe_id],class_index);

          cube_x_pos=cube_x/2+Box_Add;cube_x_neg=cube_x/2+Box_Add;
          cube_y_pos=cube_y/2+Box_Add;cube_y_neg=cube_y/2+Box_Add;
          cube_z_pos=cube_z/2+Box_Add;cube_z_neg=cube_z/2+Box_Add;

          double x_center_3d=X.x();
          double y_center_3d=X.y();
          double z_center_3d=X.z();

          ////vcl_vector <vgl_point_3d <double> > p8; //p8 bb corner points..

          double mix=x_center_3d;
          double miy=y_center_3d;
          double miz=z_center_3d;
          //counter++;         
          t[0][0]=mix;
          t[1][0]=miy;
          t[2][0]=miz;
          vnl_matrix<double> tt4=(R*t);
          X.set(tt4[0][0]+shift_x,tt4[1][0]+shift_y,tt4[2][0]+shift_z);
          vgl_point_3d <double> p(X);
          //vcl_cout<<p<<vcl_endl;
          //vcl_cout<<mix<<" "<<miy<<" "<<miz<<vcl_endl;
          p8.clear() ;
          p8.push_back(p);


          p8_list.push_back(p8);



        }//probe_id


        double val= bb_box_contour_measure(p8_list);


        total_val=val;

        if (debug_1) vcl_cout<<total_val<<"        "<<shift_x_arg<<" "<<shift_y_arg<<" "<<shift_z_arg<<" "<<vcl_endl;
        ////
        if (min_val>total_val) {
          min_val=total_val;
          p8_min_list.clear();
          min_shift_x_arg=shift_x_arg;
          min_shift_y_arg=shift_y_arg;
          min_shift_z_arg=shift_z_arg;
          for (unsigned i=0;i<p8_list.size();i++) {
            for (unsigned j=0;j<p8.size();j++){
              p8_min[0][j]=(p8_list[i])[j].x();
              p8_min[1][j]=(p8_list[i])[j].y();
              p8_min[2][j]=(p8_list[i])[j].z();
            }
            p8_min_list.push_back(p8_min);
            //vcl_cout<<p8_min<<vcl_endl;
          }


        }
      }

      vcl_cout<<"chosen****************"<<min_val<<" "<<min_shift_x_arg<<" "<<min_shift_y_arg<<" "<<min_shift_z_arg<<" "<<vcl_endl;




      draw_vsol(p8_min_list);



      return min_val;



}



double dbcri_whole_process::bb_box_contour_measure(vcl_vector< vcl_vector<vgl_point_3d<double> > >points) {


  int frame=0;double all_sum=0.0;double max_distance_sum=0.0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
    //  ref_num +=points_inside_of_the_box(frame,  bb_xform);
    //  }

    vcl_vector<vgl_point_2d<double> > p2d;
    for (unsigned i=0;i<points.size();i++)
    {
      vnl_double_4 pt(points[i][0].x(),points[i][0].y(),points[i][0].z(),1.0);
      vnl_double_3 pt2=cameras_[frame]*pt;

      //vcl_cout<<pt2<<vcl_endl;
      vgl_point_2d<double> pt3(pt2[0]/pt2[2],pt2[1]/pt2[2]);
      //vcl_cout<<pt3<<vcl_endl;
      p2d.push_back(pt3);
    }
    vgl_convex_hull_2d<double> ch(p2d);
    vgl_polygon<double> polygon = ch.hull();


    double max_distance=0.0;
    double px[8],py[8]; 
    double poly_x[] = {p2d[0].x(), p2d[1].x(), p2d[2].x(), p2d[3].x(), p2d[4].x(), p2d[5].x()};
    double poly_y[] = {p2d[0].y(), p2d[1].y(), p2d[2].y(), p2d[3].y(), p2d[4].y(), p2d[5].y()};
    /*for (unsigned int s = 0; s < polygon.num_sheets(); ++s)
    for (unsigned int p = 0; p < polygon[s].size(); ++p)
    {
    //do_something(polygon[s][p].x(), polygon[s][p].y());
    px[p]=polygon[s][p].x();
    py[p]=polygon[s][p].y();

    //vcl_cout<<px[p]<<" "<<py[p]<<vcl_endl;
    }
    */
    int count=0;
    double sum=0.0;
    for (unsigned i=0;i<(dcl_[frame])->size();i++) {
      //count=0;
      //if ( polygon.contains(dcl_[frame]->point(i)->x(),dcl_[frame]->point(i)->y() ) ) { 
      double temp_x= (dcl_[frame]->point(i)->x()-epi_x_);
      double temp_y= (dcl_[frame]->point(i)->y()-epi_y_);
      double ang=vcl_atan(temp_y/(temp_x));
      // vcl_cout<<ang<<vcl_endl;
      //if  (ang>-theta_pos_+.03 )//&& ang<-theta_neg_-.03 ) 
      if  (ang<1*(-theta_pos_-theta_neg_)/3.0 ) 
      {
        count++ ;
        double temp_x1= (dcl_[frame]->point(i)->x());
        double temp_y1= (dcl_[frame]->point(i)->y());
        //use exponential???
        double distance=vgl_distance_to_non_closed_polygon(poly_x,  poly_y, 6,temp_x1, temp_y1);
        sum+=distance*distance;
        if (distance>max_distance)
          max_distance=distance;

      }
    }



    sum/=count;
    if (count==0) sum=0;
    ////////// vcl_cout<<sum<<" "<<count<<vcl_endl;;
    all_sum+=sum;
    max_distance_sum+=max_distance;
  }



  return all_sum;

  //return max_distance_sum;
}


double dbcri_whole_process::bb_box_contour_measure_clip(vcl_vector<vgl_point_3d<double> > points) {

  int frame=0;double all_sum=0.0;double max_distance_sum=0.0;
  bool con_in_box_flag=0; float penalty=0; float value=0.0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
    //  ref_num +=points_inside_of_the_box(frame,  bb_xform);
    //  }


    vcl_vector<vgl_point_2d<double> > p2d;
    for (unsigned i=0;i<points.size();i++)
    {
      vnl_double_4 pt(points[i].x(),points[i].y(),points[i].z(),1.0);
      vnl_double_3 pt2=cameras_[frame]*pt;

      //vcl_cout<<pt2<<vcl_endl;
      vgl_point_2d<double> pt3(pt2[0]/pt2[2],pt2[1]/pt2[2]);
      //vcl_cout<<pt3<<vcl_endl;
      p2d.push_back(pt3);
    }
    vgl_convex_hull_2d<double> ch(p2d);
    vgl_polygon<double> polygon = ch.hull();
    vgl_polygon<double> result = vgl_clip( polygon, veh_cons_[frame], vgl_clip_type_intersect );

    double area1= vgl_area(polygon);//.area();
    double area2= vgl_area(result) ;
    //float area2;

    float r=area2/area1;

    // vcl_cout<<area2<<vcl_endl;

    /*if (r<0.0001) con_in_box_flag=false;
    else con_in_box_flag=true;

    if (!con_in_box_flag) penalty=.5;
    else penalty=0;
    */

    if (r<0.0001) value=-.5;
    else if (r>.9) value=-.5;
    else value=vcl_exp(-(r-.5)*(r-.5));
    //value-=penalty;
    all_sum+=value;
  }

  return all_sum;

  //return max_distance_sum;
}


double dbcri_whole_process::epi_search(vnl_vector <double> & tnew_selected,float epi_search_range_x,float epi_search_range_y,float epi_search_interval) {


  //vcl_cout.precision(4);


  //newly added

  int frame=0;

  //addition end

  ///newly added comment out//vnl_vector <double> tnew_selected;

  vnl_vector <double> tnew_max;
  vcl_vector <vnl_vector <double> >tnew_list;
  vcl_vector <double> sum_f_minus_s;
  vcl_vector <double> epi_x_l,epi_y_l;

  double epi_x_save=epi_x_;double epi_y_save=epi_y_;




  vcl_cout<<"search range: "<<epi_search_range_x<<" "<<epi_search_range_y<<vcl_endl;

  for(double epi_y_loop=-epi_search_range_y;epi_y_loop<=epi_search_range_y;epi_y_loop+=epi_search_interval)
    for(double epi_x_loop=-epi_search_range_x;epi_x_loop<=epi_search_range_x;epi_x_loop+=epi_search_interval) {// before -60:20:60{//-60:20:60

      vcl_cout<<"\===================================================================="<<vcl_endl;
      epi_x_=epi_x_loop+epi_x_save;epi_y_=epi_y_loop+epi_y_save;
      vcl_cout<<"epi search: "<<epi_x_<<" "<<epi_y_<<vcl_endl;
      frame=0;
      episeglist.clear();
      episeglist_edge_.clear();

      for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

        dbecl_epipole_sptr epipole1 = new dbecl_epipole(epi_x_,epi_y_);
        //dbecl_episeg_from_curve_converter factory(p->epipole());
        dbecl_episeg_from_curve_converter factory(epipole1);
        //for(int i = 0; i < _curves.size(); i++) {
        vsol_digital_curve_2d_sptr curve = dcl_[frame];
        vcl_vector<dbecl_episeg_sptr> episegs = factory.convert_curve(curve);


        dbecl_epipole_sptr epipole11 = new dbecl_epipole(epi_x_,epi_y_);
        vcl_vector<dbecl_episeg_sptr> episegs_edge;


        vcl_vector <vsol_digital_curve_2d_sptr> curve_edge_list = dcl_edge_[frame];
        //vcl_cout<<"curve_edge_list size:"<<curve_edge_list.size()<<vcl_endl;
        for (unsigned j=0;j<curve_edge_list.size();j++) {
          dbecl_episeg_from_curve_converter factory_edge(epipole11);
          vsol_digital_curve_2d_sptr dc_edge=curve_edge_list[j];

          vcl_vector<dbecl_episeg_sptr> eps = factory_edge.convert_curve(dc_edge);
          for(vcl_vector<dbecl_episeg_sptr>::iterator itr = eps.begin();
            itr != eps.end();  ++itr)
            episegs_edge.push_back(*itr);
        }





        // frame 마다 에피세그먼트를 가지고 잇어야 한다.
        episeglist.push_back(episegs);
        episeglist_edge_.push_back(episegs_edge);


      }




      ///newly added comment out//      if (draw_only) return true;
      network_.clear();
      dbecl_epipole_sptr epipole2 = new dbecl_epipole(epi_x_,epi_y_);
      // double r=cameras(0,1,2,dcl_,episeglist,epipole2);

      vcl_cout<<"\n---------------------------------------"<<vcl_endl;
      int C=episeglist.size();
      int combination=C*(C-1)*(C-2);
      combination/=6;
      vnl_matrix <double> B((int)combination,C-1,0.0);


      vnl_matrix <double> BB((int)combination,C-2,0.0);
      combination_=combination;
      display_0_frame_=true;
      //display_n_frame_=true;
      int counter=0;


      // vcl_cout<<ang<<" episeglist.size() "<<episeglist.size()<<vcl_endl;
      // vcl_cout<<ang<<" episeglist_edge_.size() "<<episeglist_edge_.size()<<vcl_endl;


      double sum_fisrt_second_diff=0.0;
      for (unsigned i=0; i<episeglist.size();i++) {//frame 0 to N
        for (unsigned j=i+1; j<episeglist.size();j++) { //frame i+1 to N
          for (unsigned k=j+1; k<episeglist.size();k++) { //frame j+1 to N

            if (k==episeglist.size()-1) display_n_frame_=true;
            else display_n_frame_=false;
            if (i==0&&j==1) display_0_frame_=true;
            else display_0_frame_=false;


            //generate vector 
            //vcl_cout<<ang<<" "<<episeglist[i].size()<<" "<<episeglist[j].size()<<" "<<episeglist[k].size()<<vcl_endl;
            // vcl_cout<<ang<<" "<<episeglist_edge_[i].size()<<" "<<episeglist_edge_[j].size()<<" "<<episeglist_edge_[k].size()<<vcl_endl;
            // vcl_cout<<ang<<" "<<episeglist_edge_[i][33].size()<<" "<<episeglist_edge_[j][33].size()<<" "<<episeglist_edge_[k][33].size()<<vcl_endl;
            // vcl_cout<<"-------------------- "<<vcl_endl;

            double r_con=cameras(i,j,k,dcl_,episeglist,epipole2);
            double r=cameras_accu(i,j,k,dcl_edge_,episeglist_edge_,epipole2,r_con,.1);
            //if (r<-100.0f) continue; //if histogram is empty 
            sum_fisrt_second_diff+=first_minus_second_;


            vcl_cout<<"r from contour is: "<<r_con<<" r from all edge is: "<<r<<vcl_endl;
            //double r=r_con;
            for (unsigned mi=i;mi<j;mi++) {
              //B(counter,mi) =100.0f-r;
              // B(counter,mi) =r;
              //B(counter,mi) =-100.0f+r;
              B(counter,mi) =-1.0f+r;
              vcl_cout<<r<<" ";
            }

            for (unsigned mj=j;mj<k;mj++) {
              //B(counter,mj) =r;
              //B(counter,mj) =-100.0f+r;
              B(counter,mj) =r;
              vcl_cout<<r<<" ";
            }

            vcl_cout<<vcl_endl;

            counter++;

            //vcl_cout<< i<<" "<<j<<" "<<k<<" "<<r<<vcl_endl;


          }
        }
      }




      vnl_vector <double>  bb((int)combination,0,0);
      for (unsigned i=0;i<(int) combination;i++) {
        bb(i)=-B(i,0);

        for (unsigned j=1;j<C-1;j++)
          BB(i,j-1)=B(i,j);

      }

      vcl_cout<<B<<"\n---------------------------------------"<<vcl_endl;
      vcl_cout<<BB<<vcl_endl;


      if (BB.min_value()<-100) {
        vcl_cout<<"**bad histogram**"<<vcl_endl; //escape when 0 histogram
        sum_f_minus_s.push_back(-1);
        vnl_vector <double> dummy_tnew(1,0.0);
        tnew_list.push_back(dummy_tnew);
        epi_x_l.push_back(-9999999);
        epi_y_l.push_back(-9999999);
      }
      else {


        vnl_svd<double> svd(BB.transpose()*BB);

        //vnl_matrix<T> B=svd.recompose();

        vnl_matrix<double> B1 =svd.inverse();
        vnl_vector <double> t=B1*BB.transpose()*bb ;

        vcl_cout<<t<<vcl_endl;


        double total_t=0.0;
        for (unsigned i=0;i<t.size();i++) 
          total_t+=t[i];
        total_t+=1.0;
        // return true;

        vnl_vector <double> tnew(t.size()+1,0.0);
        tnew_.set_size(t.size()+1);
        for (unsigned i=0;i<t.size();i++) {
          tnew[i+1]=t[i];
          tnew_[i+1]=t[i];
        }
        tnew[0]=1.0;tnew_[0]=1.0;


        vcl_cout<<tnew<<vcl_endl;
        //getting all cameras
        init_cameras(tnew); //here tnew and tnew_ are equal.
        //

      }
      sum_f_minus_s.push_back(sum_fisrt_second_diff);
      tnew_list.push_back(tnew_);// tnew_ ? or tnew?
      epi_x_l.push_back(epi_x_);
      epi_y_l.push_back(epi_y_);
      vcl_cout<<epi_x_loop<<" "<<epi_y_loop<<" sum: "<<sum_fisrt_second_diff<<vcl_endl;

    }
    ////////////////////
    ////////////////////
    ////////////////////
    // epi_loop loop
    ////////////////////
    ////////////////////
    ////////////////////
    //return true;
    double max_sum=-1000.0; int max_index=99999;
    vcl_cout<<"=====epipole search=============="<<vcl_endl;
    vcl_cerr<<"=====epipole search=============="<<vcl_endl;
    vcl_cerr<<"search range: "<<epi_search_range_x<<" "<<epi_search_range_y<<" "<<epi_search_interval<<vcl_endl;
    for (unsigned iii=0;iii<sum_f_minus_s.size();iii++)  {
      vcl_cout<<iii<<'\t'<<sum_f_minus_s[iii]<<'\t'<<epi_x_l[iii]<<'\t'<<epi_y_l[iii]<<vcl_endl;
      vcl_cerr<<iii<<'\t'<<sum_f_minus_s[iii]<<'\t'<<epi_x_l[iii]<<'\t'<<epi_y_l[iii]<<vcl_endl;
      if (sum_f_minus_s[iii]>max_sum) {
        max_sum=sum_f_minus_s[iii];
        max_index=iii;
        tnew_selected=tnew_list[iii];

        epi_x_=epi_x_l[iii];
        epi_y_=epi_y_l[iii];
      }

    }

    vcl_cout<<"max index: \t"<<max_index <<'\t'<<max_sum<<" *"<<epi_x_<<'\t'<<epi_y_<<vcl_endl;
    vcl_cerr<<"max index: \t"<<max_index <<'\t'<<max_sum<<" *"<<epi_x_<<'\t'<<epi_y_<<vcl_endl;
    vcl_cout<<"=====epipole search end==========="<<vcl_endl;
    vcl_cerr<<"=====epipole search end==========="<<vcl_endl;
    init_cameras(tnew_selected);

    /////////////////////
    ///////////////////// epi_loop subsidary end..
    /////////////////////
    /////////////////////


    return max_sum;



}



double dbcri_whole_process::epi_search_lite(vnl_vector <double> & tnew_selected,float epi_search_range_x,float epi_search_range_y,float epi_search_interval) {


  //newly added

  int frame=0;

  //addition end

  ///newly added comment out//vnl_vector <double> tnew_selected;

  vnl_vector <double> tnew_max;
  vcl_vector <vnl_vector <double> >tnew_list;
  vcl_vector <double> sum_f_minus_s;
  vcl_vector <double> epi_x_l,epi_y_l;

  double epi_x_save=epi_x_;double epi_y_save=epi_y_;


  vnl_matrix <int> Table=read_table(input_data_.size());

  ///newly added comment out//  float epi_search_range_x,epi_search_range_y,epi_search_interval;
  ///newly added comment out//  parameters()->get_value( "-e_range_x" , epi_search_range_x );
  ///newly added comment out//  parameters()->get_value( "-e_range_y" , epi_search_range_y );
  ///newly added comment out//  parameters()->get_value( "-epi_search_interval" , epi_search_interval );
  ///newly added comment out//  parameters()->get_value( "-MARGIN" , MARGIN_ );

  vcl_cout<<"search range: "<<epi_search_range_x<<" "<<epi_search_range_y<<vcl_endl;

  for(double epi_y_loop=-epi_search_range_y;epi_y_loop<=epi_search_range_y;epi_y_loop+=epi_search_interval)
    for(double epi_x_loop=-epi_search_range_x;epi_x_loop<=epi_search_range_x;epi_x_loop+=epi_search_interval) {// before -60:20:60{//-60:20:60

      vcl_cout<<"\===================================================================="<<vcl_endl;
      epi_x_=epi_x_loop+epi_x_save;epi_y_=epi_y_loop+epi_y_save;
      vcl_cout<<"epi search: "<<epi_x_<<" "<<epi_y_<<vcl_endl;
      frame=0;
      episeglist.clear();
      episeglist_edge_.clear();

      for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

        dbecl_epipole_sptr epipole1 = new dbecl_epipole(epi_x_,epi_y_);
        //dbecl_episeg_from_curve_converter factory(p->epipole());
        dbecl_episeg_from_curve_converter factory(epipole1);
        //for(int i = 0; i < _curves.size(); i++) {
        vsol_digital_curve_2d_sptr curve = dcl_[frame];
        vcl_vector<dbecl_episeg_sptr> episegs = factory.convert_curve(curve);


        dbecl_epipole_sptr epipole11 = new dbecl_epipole(epi_x_,epi_y_);
        vcl_vector<dbecl_episeg_sptr> episegs_edge;


        vcl_vector <vsol_digital_curve_2d_sptr> curve_edge_list = dcl_edge_[frame];
        //vcl_cout<<"curve_edge_list size:"<<curve_edge_list.size()<<vcl_endl;
        for (unsigned j=0;j<curve_edge_list.size();j++) {
          dbecl_episeg_from_curve_converter factory_edge(epipole11);
          vsol_digital_curve_2d_sptr dc_edge=curve_edge_list[j];

          vcl_vector<dbecl_episeg_sptr> eps = factory_edge.convert_curve(dc_edge);
          for(vcl_vector<dbecl_episeg_sptr>::iterator itr = eps.begin();
            itr != eps.end();  ++itr)
            episegs_edge.push_back(*itr);
        }





        // frame 마다 에피세그먼트를 가지고 잇어야 한다.
        episeglist.push_back(episegs);
        episeglist_edge_.push_back(episegs_edge);


      }




      ///newly added comment out//      if (draw_only) return true;
      network_.clear();
      dbecl_epipole_sptr epipole2 = new dbecl_epipole(epi_x_,epi_y_);
      // double r=cameras(0,1,2,dcl_,episeglist,epipole2);

      vcl_cout<<"\n---------------------------------------"<<vcl_endl;
      int C=episeglist.size();
      int combination=C*(C-1)*(C-2);
      combination/=6;
      vnl_matrix <double> B((int)combination,C-1,0.0);


      vnl_matrix <double> BB((int)combination,C-2,0.0);
      combination_=combination;
      display_0_frame_=true;
      //display_n_frame_=true;
      int counter=0;


      // vcl_cout<<ang<<" episeglist.size() "<<episeglist.size()<<vcl_endl;
      // vcl_cout<<ang<<" episeglist_edge_.size() "<<episeglist_edge_.size()<<vcl_endl;


      double sum_fisrt_second_diff=0.0;

      int sampling_counter=0;
      float rand_th=.1;
      parameters()->get_value( "-rand_th" ,   rand_th);

      for (unsigned i=0; i<episeglist.size();i++) {//frame 0 to N
        for (unsigned j=i+1; j<episeglist.size();j++) { //frame i+1 to N
          for (unsigned k=j+1; k<episeglist.size();k++) { //frame j+1 to N


            /*if (k==j+1&& j==i+1) continue;
            double random01 = (double)rand() / (double)RAND_MAX;
            vcl_cout <<"***random :" <<random01<<vcl_endl;
            if (random01> rand_th) continue;*/

            if (Table[i*C*C+j*C+k][0]==0) continue;
            sampling_counter++;


            if (k==episeglist.size()-1) display_n_frame_=true;
            else display_n_frame_=false;
            if (i==0&&j==1) display_0_frame_=true;
            else display_0_frame_=false;


            //generate vector 
            //vcl_cout<<ang<<" "<<episeglist[i].size()<<" "<<episeglist[j].size()<<" "<<episeglist[k].size()<<vcl_endl;
            // vcl_cout<<ang<<" "<<episeglist_edge_[i].size()<<" "<<episeglist_edge_[j].size()<<" "<<episeglist_edge_[k].size()<<vcl_endl;
            // vcl_cout<<ang<<" "<<episeglist_edge_[i][33].size()<<" "<<episeglist_edge_[j][33].size()<<" "<<episeglist_edge_[k][33].size()<<vcl_endl;
            // vcl_cout<<"-------------------- "<<vcl_endl;

            double r_con=cameras(i,j,k,dcl_,episeglist,epipole2);
            double r=cameras_accu(i,j,k,dcl_edge_,episeglist_edge_,epipole2,r_con,.1);
            //if (r<-100.0f) continue; //if histogram is empty 
            sum_fisrt_second_diff+=first_minus_second_;


            vcl_cout<<"r from contour is: "<<r_con<<" r from all edge is: "<<r<<vcl_endl;
            //double r=r_con;
            for (unsigned mi=i;mi<j;mi++) {
              //B(counter,mi) =100.0f-r;
              // B(counter,mi) =r;
              //B(counter,mi) =-100.0f+r;
              B(counter,mi) =-1.0f+r;
              vcl_cout<<r<<" ";
            }

            for (unsigned mj=j;mj<k;mj++) {
              //B(counter,mj) =r;
              //B(counter,mj) =-100.0f+r;
              B(counter,mj) =r;
              vcl_cout<<r<<" ";
            }

            vcl_cout<<vcl_endl;

            counter++;

            //vcl_cout<< i<<" "<<j<<" "<<k<<" "<<r<<vcl_endl;


          }
        }
      }


      combination=sampling_counter;
      vnl_matrix <double> B_new((int)combination,C-1,0.0);


      vnl_matrix <double> BB_new((int)combination,C-2,0.0);
      for (unsigned i=0;i<(unsigned int)combination;i++)
        for (unsigned j=0;j<C-1;j++)
          B_new[i][j]=B[i][j];
      for (unsigned i=0;i<(unsigned int)combination;i++)
        for (unsigned j=0;j<C-2;j++)
          BB_new[i][j]=BB[i][j];


      vnl_vector <double>  bb((int)combination,0,0);
      for (unsigned i=0;i<(int) combination;i++) {
        bb(i)=-B_new(i,0);

        for (unsigned j=1;j<C-1;j++)
          BB_new(i,j-1)=B_new(i,j);

      }

      vcl_cout<<B_new<<"\n---------------------------------------"<<vcl_endl;
      vcl_cout<<BB_new<<vcl_endl;


      if (BB_new.min_value()<-100) {
        vcl_cout<<"**bad histogram**"<<vcl_endl; //escape when 0 histogram
        sum_f_minus_s.push_back(-1);
        vnl_vector <double> dummy_tnew(1,0.0);
        tnew_list.push_back(dummy_tnew);
        epi_x_l.push_back(-9999999);
        epi_y_l.push_back(-9999999);
      }
      else {


        vnl_svd<double> svd(BB_new.transpose()*BB_new);

        //vnl_matrix<T> B=svd.recompose();

        vnl_matrix<double> B1 =svd.inverse();
        vnl_vector <double> t=B1*BB_new.transpose()*bb ;

        vcl_cout<<t<<vcl_endl;


        double total_t=0.0;
        for (unsigned i=0;i<t.size();i++) 
          total_t+=t[i];
        total_t+=1.0;
        // return true;

        vnl_vector <double> tnew(t.size()+1,0.0);
        tnew_.set_size(t.size()+1);
        for (unsigned i=0;i<t.size();i++) {
          tnew[i+1]=t[i];
          tnew_[i+1]=t[i];
        }
        tnew[0]=1.0;tnew_[0]=1.0;


        vcl_cout<<tnew<<vcl_endl;
        //getting all cameras
        init_cameras(tnew); //here tnew and tnew_ are equal.
        //

      }
      sum_f_minus_s.push_back(sum_fisrt_second_diff);
      tnew_list.push_back(tnew_);// tnew_ ? or tnew?
      epi_x_l.push_back(epi_x_);
      epi_y_l.push_back(epi_y_);

      vcl_cout<<epi_x_loop<<" "<<epi_y_loop<<" sum: "<<sum_fisrt_second_diff<<vcl_endl;

    }
    ////////////////////
    ////////////////////
    ////////////////////
    // epi_loop loop
    ////////////////////
    ////////////////////
    ////////////////////
    //return true;
    double max_sum=-1000.0; int max_index=99999;
    vcl_cout<<"=====epipole search=============="<<vcl_endl;
    vcl_cerr<<"=====epipole search=============="<<vcl_endl;
    vcl_cerr<<"search range: "<<epi_search_range_x<<" "<<epi_search_range_y<<" "<<epi_search_interval<<vcl_endl;
    for (unsigned iii=0;iii<sum_f_minus_s.size();iii++)  {
      vcl_cout<<iii<<'\t'<<sum_f_minus_s[iii]<<" "<<epi_x_l[iii]<<" "<<epi_y_l[iii]<<vcl_endl;
      vcl_cerr<<iii<<'\t'<<sum_f_minus_s[iii]<<" "<<epi_x_l[iii]<<" "<<epi_y_l[iii]<<vcl_endl;
      if (sum_f_minus_s[iii]>max_sum) {
        max_sum=sum_f_minus_s[iii];
        max_index=iii;
        tnew_selected=tnew_list[iii];

        epi_x_=epi_x_l[iii];
        epi_y_=epi_y_l[iii];
      }

    }

    vcl_cout<<"max index: "<<max_index <<" "<<max_sum<<" *"<<epi_x_<<" "<<epi_y_<<vcl_endl;
    vcl_cerr<<"max index: "<<max_index <<" "<<max_sum<<" *"<<epi_x_<<" "<<epi_y_<<vcl_endl;
    vcl_cout<<"=====epipole search end==========="<<vcl_endl;
    vcl_cerr<<"=====epipole search end==========="<<vcl_endl;
    init_cameras(tnew_selected);

    /////////////////////
    ///////////////////// epi_loop subsidary end..
    /////////////////////
    /////////////////////
    return max_sum;




}



//
//11-29-07 make epi search faster

//
double dbcri_whole_process::epi_search_fast(vnl_vector <double> & tnew_selected,float epi_search_range_x,float epi_search_range_y,float epi_search_interval) {


  //vcl_cout.precision(4);


  //newly added

  int frame=0;

  //addition end

  ///newly added comment out//vnl_vector <double> tnew_selected;

  vnl_vector <double> tnew_max;
  vcl_vector <vnl_vector <double> >tnew_list;
  vcl_vector <double> sum_f_minus_s;
  vcl_vector <double> epi_x_l,epi_y_l;

  double epi_x_save=epi_x_;double epi_y_save=epi_y_;




  vcl_cout<<"search range: "<<epi_search_range_x<<" "<<epi_search_range_y<<vcl_endl;

  float display_count=0;
  for(double epi_y_loop=-epi_search_range_y;epi_y_loop<=epi_search_range_y;epi_y_loop+=epi_search_interval)
    for(double epi_x_loop=-epi_search_range_x;epi_x_loop<=epi_search_range_x;epi_x_loop+=epi_search_interval) {// before -60:20:60{//-60:20:60

      vcl_cout<<"\===================================================================="<<vcl_endl;
      epi_x_=epi_x_loop+epi_x_save;epi_y_=epi_y_loop+epi_y_save;
      vcl_cout<<"epi search: "<<epi_x_<<" "<<epi_y_<<vcl_endl;

      vcl_cout<<display_count<<vcl_endl;
      vcl_cerr<<display_count<<vcl_endl;
      display_count++;
      frame=0;
      episeglist.clear();
      episeglist_edge_.clear();


      vcl_vector <float> vl; 


      for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

        dbecl_epipole_sptr epipole1 = new dbecl_epipole(epi_x_,epi_y_);
        //dbecl_episeg_from_curve_converter factory(p->epipole());
        dbecl_episeg_from_curve_converter factory(epipole1);
        //for(int i = 0; i < _curves.size(); i++) {
        vsol_digital_curve_2d_sptr curve = dcl_[frame];
        vcl_vector<dbecl_episeg_sptr> episegs = factory.convert_curve(curve);


        dbecl_epipole_sptr epipole11 = new dbecl_epipole(epi_x_,epi_y_);
        vcl_vector<dbecl_episeg_sptr> episegs_edge;


        vcl_vector <vsol_digital_curve_2d_sptr> curve_edge_list = dcl_edge_[frame];
        //vcl_cout<<"curve_edge_list size:"<<curve_edge_list.size()<<vcl_endl;
        for (unsigned j=0;j<curve_edge_list.size();j++) {
          dbecl_episeg_from_curve_converter factory_edge(epipole11);
          vsol_digital_curve_2d_sptr dc_edge=curve_edge_list[j];

          vcl_vector<dbecl_episeg_sptr> eps = factory_edge.convert_curve(dc_edge);
          for(vcl_vector<dbecl_episeg_sptr>::iterator itr = eps.begin();
            itr != eps.end();  ++itr)
            episegs_edge.push_back(*itr);
        }





        // frame 마다 에피세그먼트를 가지고 잇어야 한다.
        episeglist.push_back(episegs);
        episeglist_edge_.push_back(episegs_edge);




      }




      ///newly added comment out//      if (draw_only) return true;
      network_.clear();
      dbecl_epipole_sptr epipole2 = new dbecl_epipole(epi_x_,epi_y_);
      // double r=cameras(0,1,2,dcl_,episeglist,epipole2);

      vcl_cout<<"\n---------------------------------------"<<vcl_endl;
      int C=episeglist.size();
      int combination=C*(C-1)*(C-2);
      combination/=6;
      vnl_matrix <double> B((int)combination,C-1,0.0);


      vnl_matrix <double> BB((int)combination,C-2,0.0);
      combination_=combination;
      display_0_frame_=true;
      //display_n_frame_=true;
      int counter=0;



      //11-29-2007
      vcl_vector <vcl_vector <vcl_vector <vnl_float_3> > > simple_wedge_list_N;
      episeg_simplify(simple_wedge_list_N);

      double sum_fisrt_second_diff=0.0;
      for (unsigned i=0; i<episeglist.size();i++) {//frame 0 to N
        for (unsigned j=i+1; j<episeglist.size();j++) { //frame i+1 to N
          for (unsigned k=j+1; k<episeglist.size();k++) { //frame j+1 to N

            if (k==episeglist.size()-1) display_n_frame_=true;
            else display_n_frame_=false;
            if (i==0&&j==1) display_0_frame_=true;
            else display_0_frame_=false;


            //generate vector 
            //vcl_cout<<ang<<" "<<episeglist[i].size()<<" "<<episeglist[j].size()<<" "<<episeglist[k].size()<<vcl_endl;
            // vcl_cout<<ang<<" "<<episeglist_edge_[i].size()<<" "<<episeglist_edge_[j].size()<<" "<<episeglist_edge_[k].size()<<vcl_endl;
            // vcl_cout<<ang<<" "<<episeglist_edge_[i][33].size()<<" "<<episeglist_edge_[j][33].size()<<" "<<episeglist_edge_[k][33].size()<<vcl_endl;
            // vcl_cout<<"-------------------- "<<vcl_endl;

            double r_con=cameras_fast(i,j,k,dcl_,episeglist,epipole2);
            double r=cameras_accu_fast(simple_wedge_list_N,i,j,k,dcl_edge_,episeglist_edge_,epipole2,r_con,.1);
            //if (r<-100.0f) continue; //if histogram is empty 
            sum_fisrt_second_diff+=first_minus_second_;


            vcl_cout<<"r from contour is: "<<r_con<<" r from all edge is: "<<r<<vcl_endl;
            //double r=r_con;
            for (unsigned mi=i;mi<j;mi++) {
              //B(counter,mi) =100.0f-r;
              // B(counter,mi) =r;
              //B(counter,mi) =-100.0f+r;
              B(counter,mi) =-1.0f+r;
              vcl_cout<<r<<" ";
            }

            for (unsigned mj=j;mj<k;mj++) {
              //B(counter,mj) =r;
              //B(counter,mj) =-100.0f+r;
              B(counter,mj) =r;
              vcl_cout<<r<<" ";
            }

            vcl_cout<<vcl_endl;

            counter++;

            //vcl_cout<< i<<" "<<j<<" "<<k<<" "<<r<<vcl_endl;


          }
        }
      }




      vnl_vector <double>  bb((int)combination,0,0);
      for (unsigned i=0;i<(int) combination;i++) {
        bb(i)=-B(i,0);

        for (unsigned j=1;j<C-1;j++)
          BB(i,j-1)=B(i,j);

      }

      vcl_cout<<B<<"\n---------------------------------------"<<vcl_endl;
      vcl_cout<<BB<<vcl_endl;


      if (BB.min_value()<-100) {
        vcl_cout<<"**bad histogram**"<<vcl_endl; //escape when 0 histogram
        sum_f_minus_s.push_back(-1);
        vnl_vector <double> dummy_tnew(1,0.0);
        tnew_list.push_back(dummy_tnew);
        epi_x_l.push_back(-9999999);
        epi_y_l.push_back(-9999999);
      }
      else {


        vnl_svd<double> svd(BB.transpose()*BB);

        //vnl_matrix<T> B=svd.recompose();

        vnl_matrix<double> B1 =svd.inverse();
        vnl_vector <double> t=B1*BB.transpose()*bb ;

        vcl_cout<<t<<vcl_endl;


        double total_t=0.0;
        for (unsigned i=0;i<t.size();i++) 
          total_t+=t[i];
        total_t+=1.0;
        // return true;

        vnl_vector <double> tnew(t.size()+1,0.0);
        tnew_.set_size(t.size()+1);
        for (unsigned i=0;i<t.size();i++) {
          tnew[i+1]=t[i];
          tnew_[i+1]=t[i];
        }
        tnew[0]=1.0;tnew_[0]=1.0;


        vcl_cout<<tnew<<vcl_endl;
        //getting all cameras
        init_cameras(tnew); //here tnew and tnew_ are equal.
        //

      }
      sum_f_minus_s.push_back(sum_fisrt_second_diff);
      tnew_list.push_back(tnew_);// tnew_ ? or tnew?
      epi_x_l.push_back(epi_x_);
      epi_y_l.push_back(epi_y_);
      vcl_cout<<epi_x_loop<<" "<<epi_y_loop<<" sum: "<<sum_fisrt_second_diff<<vcl_endl;

    }
    ////////////////////
    ////////////////////
    ////////////////////
    // epi_loop loop
    ////////////////////
    ////////////////////
    ////////////////////
    //return true;
    double max_sum=-1000.0; int max_index=99999;
    vcl_cout<<"=====epipole search=============="<<vcl_endl;
    vcl_cerr<<"=====epipole search=============="<<vcl_endl;
    vcl_cerr<<"search range: "<<epi_search_range_x<<" "<<epi_search_range_y<<" "<<epi_search_interval<<vcl_endl;
    for (unsigned iii=0;iii<sum_f_minus_s.size();iii++)  {
      vcl_cout<<iii<<'\t'<<sum_f_minus_s[iii]<<'\t'<<epi_x_l[iii]<<'\t'<<epi_y_l[iii]<<vcl_endl;
      vcl_cerr<<iii<<'\t'<<sum_f_minus_s[iii]<<'\t'<<epi_x_l[iii]<<'\t'<<epi_y_l[iii]<<vcl_endl;
      if (sum_f_minus_s[iii]>max_sum) {
        max_sum=sum_f_minus_s[iii];
        max_index=iii;
        tnew_selected=tnew_list[iii];

        epi_x_=epi_x_l[iii];
        epi_y_=epi_y_l[iii];
      }

    }

    vcl_cout<<"max index: \t"<<max_index <<'\t'<<max_sum<<" *"<<epi_x_<<'\t'<<epi_y_<<vcl_endl;
    vcl_cerr<<"max index: \t"<<max_index <<'\t'<<max_sum<<" *"<<epi_x_<<'\t'<<epi_y_<<vcl_endl;
    vcl_cout<<"=====epipole search end==========="<<vcl_endl;
    vcl_cerr<<"=====epipole search end==========="<<vcl_endl;
    init_cameras(tnew_selected);

    /////////////////////
    ///////////////////// epi_loop subsidary end..
    /////////////////////
    /////////////////////


    return max_sum;



}



double dbcri_whole_process::epi_search_fast_lite(vnl_vector <double> & tnew_selected,float epi_search_range_x,float epi_search_range_y,float epi_search_interval) {


  //newly added

  int frame=0;

  //addition end

  ///newly added comment out//vnl_vector <double> tnew_selected;

  vnl_vector <double> tnew_max;
  vcl_vector <vnl_vector <double> >tnew_list;
  vcl_vector <double> sum_f_minus_s;
  vcl_vector <double> epi_x_l,epi_y_l;

  double epi_x_save=epi_x_;double epi_y_save=epi_y_;


  vnl_matrix <int> Table=read_table(input_data_.size());

  ///newly added comment out//  float epi_search_range_x,epi_search_range_y,epi_search_interval;
  ///newly added comment out//  parameters()->get_value( "-e_range_x" , epi_search_range_x );
  ///newly added comment out//  parameters()->get_value( "-e_range_y" , epi_search_range_y );
  ///newly added comment out//  parameters()->get_value( "-epi_search_interval" , epi_search_interval );
  ///newly added comment out//  parameters()->get_value( "-MARGIN" , MARGIN_ );

  vcl_cout<<"search range: "<<epi_search_range_x<<" "<<epi_search_range_y<<vcl_endl;

  float display_count=0;
  for(double epi_y_loop=-epi_search_range_y;epi_y_loop<=epi_search_range_y;epi_y_loop+=epi_search_interval)
    for(double epi_x_loop=-epi_search_range_x;epi_x_loop<=epi_search_range_x;epi_x_loop+=epi_search_interval) {// before -60:20:60{//-60:20:60

      vcl_cout<<"\===================================================================="<<vcl_endl;
      epi_x_=epi_x_loop+epi_x_save;epi_y_=epi_y_loop+epi_y_save;
      vcl_cout<<"epi search: "<<epi_x_<<" "<<epi_y_<<vcl_endl;

      vcl_cout<<display_count<<vcl_endl;
      vcl_cerr<<display_count<<vcl_endl;
      display_count++;


      frame=0;
      episeglist.clear();
      episeglist_edge_.clear();

      for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

        dbecl_epipole_sptr epipole1 = new dbecl_epipole(epi_x_,epi_y_);
        //dbecl_episeg_from_curve_converter factory(p->epipole());
        dbecl_episeg_from_curve_converter factory(epipole1);
        //for(int i = 0; i < _curves.size(); i++) {
        vsol_digital_curve_2d_sptr curve = dcl_[frame];
        vcl_vector<dbecl_episeg_sptr> episegs = factory.convert_curve(curve);


        dbecl_epipole_sptr epipole11 = new dbecl_epipole(epi_x_,epi_y_);
        vcl_vector<dbecl_episeg_sptr> episegs_edge;


        vcl_vector <vsol_digital_curve_2d_sptr> curve_edge_list = dcl_edge_[frame];
        //vcl_cout<<"curve_edge_list size:"<<curve_edge_list.size()<<vcl_endl;
        for (unsigned j=0;j<curve_edge_list.size();j++) {
          dbecl_episeg_from_curve_converter factory_edge(epipole11);
          vsol_digital_curve_2d_sptr dc_edge=curve_edge_list[j];

          vcl_vector<dbecl_episeg_sptr> eps = factory_edge.convert_curve(dc_edge);
          for(vcl_vector<dbecl_episeg_sptr>::iterator itr = eps.begin();
            itr != eps.end();  ++itr)
            episegs_edge.push_back(*itr);
        }





        // frame 마다 에피세그먼트를 가지고 잇어야 한다.
        episeglist.push_back(episegs);
        episeglist_edge_.push_back(episegs_edge);


      }




      ///newly added comment out//      if (draw_only) return true;
      network_.clear();
      dbecl_epipole_sptr epipole2 = new dbecl_epipole(epi_x_,epi_y_);
      // double r=cameras(0,1,2,dcl_,episeglist,epipole2);

      vcl_cout<<"\n---------------------------------------"<<vcl_endl;
      int C=episeglist.size();
      int combination=C*(C-1)*(C-2);
      combination/=6;
      vnl_matrix <double> B((int)combination,C-1,0.0);


      vnl_matrix <double> BB((int)combination,C-2,0.0);
      combination_=combination;
      display_0_frame_=true;
      //display_n_frame_=true;
      int counter=0;


      // vcl_cout<<ang<<" episeglist.size() "<<episeglist.size()<<vcl_endl;
      // vcl_cout<<ang<<" episeglist_edge_.size() "<<episeglist_edge_.size()<<vcl_endl;



      //11-29-2007
      vcl_vector <vcl_vector <vcl_vector <vnl_float_3> > > simple_wedge_list_N;
      episeg_simplify(simple_wedge_list_N);



      double sum_fisrt_second_diff=0.0;

      int sampling_counter=0;
      float rand_th=.1;
      parameters()->get_value( "-rand_th" ,   rand_th);

      for (unsigned i=0; i<episeglist.size();i++) {//frame 0 to N
        for (unsigned j=i+1; j<episeglist.size();j++) { //frame i+1 to N
          for (unsigned k=j+1; k<episeglist.size();k++) { //frame j+1 to N


            /*if (k==j+1&& j==i+1) continue;
            double random01 = (double)rand() / (double)RAND_MAX;
            vcl_cout <<"***random :" <<random01<<vcl_endl;
            if (random01> rand_th) continue;*/

            if (Table[i*C*C+j*C+k][0]==0) continue;
            sampling_counter++;


            if (k==episeglist.size()-1) display_n_frame_=true;
            else display_n_frame_=false;
            if (i==0&&j==1) display_0_frame_=true;
            else display_0_frame_=false;


            //generate vector 
            //vcl_cout<<ang<<" "<<episeglist[i].size()<<" "<<episeglist[j].size()<<" "<<episeglist[k].size()<<vcl_endl;
            // vcl_cout<<ang<<" "<<episeglist_edge_[i].size()<<" "<<episeglist_edge_[j].size()<<" "<<episeglist_edge_[k].size()<<vcl_endl;
            // vcl_cout<<ang<<" "<<episeglist_edge_[i][33].size()<<" "<<episeglist_edge_[j][33].size()<<" "<<episeglist_edge_[k][33].size()<<vcl_endl;
            // vcl_cout<<"-------------------- "<<vcl_endl;

            double r_con=cameras(i,j,k,dcl_,episeglist,epipole2);
            //double r=cameras_accu(i,j,k,dcl_edge_,episeglist_edge_,epipole2,r_con,.1);
            double r=cameras_accu_fast(simple_wedge_list_N,i,j,k,dcl_edge_,episeglist_edge_,epipole2,r_con,.1);

            //if (r<-100.0f) continue; //if histogram is empty 
            sum_fisrt_second_diff+=first_minus_second_;


            vcl_cout<<"r from contour is: "<<r_con<<" r from all edge is: "<<r<<vcl_endl;
            //double r=r_con;
            for (unsigned mi=i;mi<j;mi++) {
              //B(counter,mi) =100.0f-r;
              // B(counter,mi) =r;
              //B(counter,mi) =-100.0f+r;
              B(counter,mi) =-1.0f+r;
              vcl_cout<<r<<" ";
            }

            for (unsigned mj=j;mj<k;mj++) {
              //B(counter,mj) =r;
              //B(counter,mj) =-100.0f+r;
              B(counter,mj) =r;
              vcl_cout<<r<<" ";
            }

            vcl_cout<<vcl_endl;

            counter++;

            //vcl_cout<< i<<" "<<j<<" "<<k<<" "<<r<<vcl_endl;


          }
        }
      }


      combination=sampling_counter;
      vnl_matrix <double> B_new((int)combination,C-1,0.0);


      vnl_matrix <double> BB_new((int)combination,C-2,0.0);
      for (unsigned i=0;i<(unsigned int)combination;i++)
        for (unsigned j=0;j<C-1;j++)
          B_new[i][j]=B[i][j];
      for (unsigned i=0;i<(unsigned int)combination;i++)
        for (unsigned j=0;j<C-2;j++)
          BB_new[i][j]=BB[i][j];


      vnl_vector <double>  bb((int)combination,0,0);
      for (unsigned i=0;i<(int) combination;i++) {
        bb(i)=-B_new(i,0);

        for (unsigned j=1;j<C-1;j++)
          BB_new(i,j-1)=B_new(i,j);

      }

      vcl_cout<<B_new<<"\n---------------------------------------"<<vcl_endl;
      vcl_cout<<BB_new<<vcl_endl;


      if (BB_new.min_value()<-100) {
        vcl_cout<<"**bad histogram**"<<vcl_endl; //escape when 0 histogram
        sum_f_minus_s.push_back(-1);
        vnl_vector <double> dummy_tnew(1,0.0);
        tnew_list.push_back(dummy_tnew);
        epi_x_l.push_back(-9999999);
        epi_y_l.push_back(-9999999);
      }
      else {


        vnl_svd<double> svd(BB_new.transpose()*BB_new);

        //vnl_matrix<T> B=svd.recompose();

        vnl_matrix<double> B1 =svd.inverse();
        vnl_vector <double> t=B1*BB_new.transpose()*bb ;

        vcl_cout<<t<<vcl_endl;


        double total_t=0.0;
        for (unsigned i=0;i<t.size();i++) 
          total_t+=t[i];
        total_t+=1.0;
        // return true;

        vnl_vector <double> tnew(t.size()+1,0.0);
        tnew_.set_size(t.size()+1);
        for (unsigned i=0;i<t.size();i++) {
          tnew[i+1]=t[i];
          tnew_[i+1]=t[i];
        }
        tnew[0]=1.0;tnew_[0]=1.0;


        vcl_cout<<tnew<<vcl_endl;
        //getting all cameras
        init_cameras(tnew); //here tnew and tnew_ are equal.
        //

      }
      sum_f_minus_s.push_back(sum_fisrt_second_diff);
      tnew_list.push_back(tnew_);// tnew_ ? or tnew?
      epi_x_l.push_back(epi_x_);
      epi_y_l.push_back(epi_y_);

      vcl_cout<<epi_x_loop<<" "<<epi_y_loop<<" sum: "<<sum_fisrt_second_diff<<vcl_endl;

    }
    ////////////////////
    ////////////////////
    ////////////////////
    // epi_loop loop
    ////////////////////
    ////////////////////
    ////////////////////
    //return true;
    double max_sum=-1000.0; int max_index=99999;
    vcl_cout<<"=====epipole search=============="<<vcl_endl;
    vcl_cerr<<"=====epipole search=============="<<vcl_endl;
    vcl_cerr<<"search range: "<<epi_search_range_x<<" "<<epi_search_range_y<<" "<<epi_search_interval<<vcl_endl;
    for (unsigned iii=0;iii<sum_f_minus_s.size();iii++)  {
      vcl_cout<<iii<<'\t'<<sum_f_minus_s[iii]<<" "<<epi_x_l[iii]<<" "<<epi_y_l[iii]<<vcl_endl;
      vcl_cerr<<iii<<'\t'<<sum_f_minus_s[iii]<<" "<<epi_x_l[iii]<<" "<<epi_y_l[iii]<<vcl_endl;
      if (sum_f_minus_s[iii]>max_sum) {
        max_sum=sum_f_minus_s[iii];
        max_index=iii;
        tnew_selected=tnew_list[iii];

        epi_x_=epi_x_l[iii];
        epi_y_=epi_y_l[iii];
      }

    }

    vcl_cout<<"max index: "<<max_index <<" "<<max_sum<<" *"<<epi_x_<<" "<<epi_y_<<vcl_endl;
    vcl_cerr<<"max index: "<<max_index <<" "<<max_sum<<" *"<<epi_x_<<" "<<epi_y_<<vcl_endl;
    vcl_cout<<"=====epipole search end==========="<<vcl_endl;
    vcl_cerr<<"=====epipole search end==========="<<vcl_endl;
    init_cameras(tnew_selected);

    /////////////////////
    ///////////////////// epi_loop subsidary end..
    /////////////////////
    /////////////////////
    return max_sum;




}





//end 



vnl_matrix <int> dbcri_whole_process::genarate_table(int N) 
{
  int Comb=0;
  Comb=N*(N-1)*(N-2)/6;

  vnl_matrix <int> table(N*N*N,4,0);
  //vnl_matrix <int> bin(N,1,0);
  bool bin_flag=false;


  float th=0.2;
  //if (N>8) th=0.1;

  int min_Comb,bin_min;
  parameters()->get_value( "-rand_th" ,   th);
  parameters()->get_value( "-Comb" ,   min_Comb);
  parameters()->get_value( "-bin_min" ,   bin_min);
  while (bin_flag==false) {
    int count=0,bin_counter=0;
    vnl_matrix <int> bin(N,1,0);

    for (unsigned i=0;i<N;i++)
      for (unsigned j=i+1;j<N;j++)
        for (unsigned k=j+1;k<N;k++) {

          count ++;count ++; count ++;
          double random01 = (double)rand() / (double)RAND_MAX;
          if (random01<=th) {

            if (table(i*N*N+j*N+k,3)!=0) continue;
            table(i*N*N+j*N+k,0) =i;
            table(i*N*N+j*N+k,1) =j;
            table(i*N*N+j*N+k,2) =k;
            table(i*N*N+j*N+k,3) ++;
            bin[i][0]++;bin[j][0]++;bin[k][0]++;
            //count ++;count ++; count ++;
            bin_counter++;
          }


        }
        // if (bin.min_value() > count/N-1) {
        if (bin.min_value() > bin_min&&bin_counter<min_Comb) {
          bin_flag=true;
          vcl_cout<<bin<<"\n--------"<<vcl_endl;
          // vcl_cout<<table<<vcl_endl;
        }

        else table.fill(0);
  }
  // vcl_cout <<"***random :" <<random01<<vcl_endl;

  int row_counter=0;
  for (unsigned i=0;i<N*N*N;i++)
    if (table[i][3]!=0) row_counter++;

  vnl_matrix <int> table_to_return(row_counter,4,0);
  int j=0;
  for (unsigned i=0;i<N*N*N;i++) {
    if (table[i][3]!=0) {
      table_to_return[j][0]=table[i][0];
      table_to_return[j][1]=table[i][1];
      table_to_return[j][2]=table[i][2];
      table_to_return[j][3]=table[i][3];
      j++;
    }
  }
  vcl_cout<<j<<vcl_endl;
  for(unsigned i=0;i<j;i++) {
    vcl_cout<<" TB["<<i<<"][0]="<<table_to_return[i][0]<<"; TB["<<i<<"][1]="<<table_to_return[i][1]<<";  TB["<<i<<"][2]="<<table_to_return[i][2]<<";  TB["<<i<<"][3]= 1;"<<vcl_endl;
  }
  return table_to_return;
}


vnl_matrix <int> dbcri_whole_process::read_table(int N) 
{
  int Comb=0;
  Comb=N*(N-1)*(N-2)/6;

  vnl_matrix <int> TB;
  if (N==7) {


    /* TB.set_size(19,4);
    TB[0][0]=0;  TB[0][1]=1;  TB[0][2]=3;  TB[0][3]=1;
    TB[1][0]=0;  TB[1][1]=1;  TB[1][2]=4;  TB[1][3]=1;
    TB[2][0]=0;  TB[2][1]=1;  TB[2][2]=6;  TB[2][3]=1;
    TB[3][0]=0;  TB[3][1]=2;  TB[3][2]=3;  TB[3][3]=1;
    TB[4][0]=0;  TB[4][1]=2;  TB[4][2]=4;  TB[4][3]=1;
    TB[5][0]=0;  TB[5][1]=2;  TB[5][2]=5;  TB[5][3]=1;
    TB[6][0]=0;  TB[6][1]=3;  TB[6][2]=5;  TB[6][3]=1;
    TB[7][0]=0;  TB[7][1]=5;  TB[7][2]=6;  TB[7][3]=1;
    TB[8][0]=1;  TB[8][1]=2;  TB[8][2]=3;  TB[8][3]=1;
    TB[9][0]=1;  TB[9][1]=2;  TB[9][2]=6;  TB[9][3]=1;
    TB[10][0]=1; TB[10][1]=3; TB[10][2]=4; TB[10][3]=1;
    TB[11][0]=1; TB[11][1]=3; TB[11][2]=5; TB[11][3]=1;
    TB[12][0]=1; TB[12][1]=4; TB[12][2]=5; TB[12][3]=1;
    TB[13][0]=1; TB[13][1]=4; TB[13][2]=6; TB[13][3]=1;
    TB[14][0]=2; TB[14][1]=3; TB[14][2]=4; TB[14][3]=1;
    TB[15][0]=2; TB[15][1]=4; TB[15][2]=6; TB[15][3]=1;
    TB[16][0]=2; TB[16][1]=5; TB[16][2]=6; TB[16][3]=1;
    TB[17][0]=3; TB[17][1]=5; TB[17][2]=6; TB[17][3]=1;
    TB[18][0]=4; TB[18][1]=5; TB[18][2]=6; TB[18][3]=1;*/
    TB.set_size(14,4);
    TB[0][0]=0; TB[0][1]=1;  TB[0][2]=4;  TB[0][3]= 1;
    TB[1][0]=0; TB[1][1]=2;  TB[1][2]=3;  TB[1][3]= 1;
    TB[2][0]=0; TB[2][1]=2;  TB[2][2]=4;  TB[2][3]= 1;
    TB[3][0]=0; TB[3][1]=2;  TB[3][2]=6;  TB[3][3]= 1;
    TB[4][0]=0; TB[4][1]=3;  TB[4][2]=5;  TB[4][3]= 1;
    TB[5][0]=0; TB[5][1]=4;  TB[5][2]=5;  TB[5][3]= 1;
    TB[6][0]=1; TB[6][1]=2;  TB[6][2]=6;  TB[6][3]= 1;
    TB[7][0]=1; TB[7][1]=3;  TB[7][2]=5;  TB[7][3]= 1;
    TB[8][0]=1; TB[8][1]=3;  TB[8][2]=6;  TB[8][3]= 1;
    TB[9][0]=1; TB[9][1]=4;  TB[9][2]=6;  TB[9][3]= 1;
    TB[10][0]=1; TB[10][1]=5;  TB[10][2]=6;  TB[10][3]= 1;
    TB[11][0]=2; TB[11][1]=3;  TB[11][2]=5;  TB[11][3]= 1;
    TB[12][0]=2; TB[12][1]=4;  TB[12][2]=6;  TB[12][3]= 1;
    TB[13][0]=3; TB[13][1]=4;  TB[13][2]=5;  TB[13][3]= 1;
  }


  if (N==8) {
    /* --------N is: 8
    7 7 7 7 7 7 7 8
    --------
    19*/
    TB.set_size(14,4);
    TB[0][0]=0; TB[0][1]=1;  TB[0][2]=5;  TB[0][3]= 1;
    TB[1][0]=0; TB[1][1]=1;  TB[1][2]=6;  TB[1][3]= 1;
    TB[2][0]=0; TB[2][1]=2;  TB[2][2]=7;  TB[2][3]= 1;
    TB[3][0]=0; TB[3][1]=3;  TB[3][2]=4;  TB[3][3]= 1;
    TB[4][0]=0; TB[4][1]=4;  TB[4][2]=6;  TB[4][3]= 1;
    TB[5][0]=1; TB[5][1]=2;  TB[5][2]=4;  TB[5][3]= 1;
    TB[6][0]=1; TB[6][1]=3;  TB[6][2]=7;  TB[6][3]= 1;
    TB[7][0]=1; TB[7][1]=4;  TB[7][2]=6;  TB[7][3]= 1;
    TB[8][0]=2; TB[8][1]=3;  TB[8][2]=4;  TB[8][3]= 1;
    TB[9][0]=2; TB[9][1]=3;  TB[9][2]=5;  TB[9][3]= 1;
    TB[10][0]=2; TB[10][1]=5;  TB[10][2]=6;  TB[10][3]= 1;
    TB[11][0]=2; TB[11][1]=5;  TB[11][2]=7;  TB[11][3]= 1;
    TB[12][0]=3; TB[12][1]=5;  TB[12][2]=7;  TB[12][3]= 1;
    TB[13][0]=4; TB[13][1]=6;  TB[13][2]=7;  TB[13][3]= 1;
  }


  if (N==9) {

    TB.set_size(15,4);

    TB[0][0]=0; TB[0][1]=1;  TB[0][2]=5;  TB[0][3]= 1;
    TB[1][0]=0; TB[1][1]=1;  TB[1][2]=8;  TB[1][3]= 1;
    TB[2][0]=0; TB[2][1]=3;  TB[2][2]=6;  TB[2][3]= 1;
    TB[3][0]=0; TB[3][1]=3;  TB[3][2]=8;  TB[3][3]= 1;
    TB[4][0]=0; TB[4][1]=4;  TB[4][2]=8;  TB[4][3]= 1;
    TB[5][0]=1; TB[5][1]=2;  TB[5][2]=5;  TB[5][3]= 1;
    TB[6][0]=1; TB[6][1]=3;  TB[6][2]=6;  TB[6][3]= 1;
    TB[7][0]=1; TB[7][1]=3;  TB[7][2]=8;  TB[7][3]= 1;
    TB[8][0]=2; TB[8][1]=4;  TB[8][2]=5;  TB[8][3]= 1;
    TB[9][0]=2; TB[9][1]=4;  TB[9][2]=7;  TB[9][3]= 1;
    TB[10][0]=2; TB[10][1]=4;  TB[10][2]=8;  TB[10][3]= 1;
    TB[11][0]=2; TB[11][1]=5;  TB[11][2]=7;  TB[11][3]= 1;
    TB[12][0]=3; TB[12][1]=6;  TB[12][2]=7;  TB[12][3]= 1;
    TB[13][0]=4; TB[13][1]=6;  TB[13][2]=7;  TB[13][3]= 1;
    TB[14][0]=5; TB[14][1]=6;  TB[14][2]=7;  TB[14][3]= 1;
  }


  if (N==10) {

    TB.set_size(14,4);
    TB[0][0]=0; TB[0][1]=2;  TB[0][2]=8;  TB[0][3]= 1;
    TB[1][0]=0; TB[1][1]=3;  TB[1][2]=9;  TB[1][3]= 1;
    TB[2][0]=0; TB[2][1]=5;  TB[2][2]=8;  TB[2][3]= 1;
    TB[3][0]=0; TB[3][1]=6;  TB[3][2]=8;  TB[3][3]= 1;
    TB[4][0]=1; TB[4][1]=2;  TB[4][2]=7;  TB[4][3]= 1;
    TB[5][0]=1; TB[5][1]=2;  TB[5][2]=9;  TB[5][3]= 1;
    TB[6][0]=1; TB[6][1]=4;  TB[6][2]=7;  TB[6][3]= 1;
    TB[7][0]=1; TB[7][1]=5;  TB[7][2]=8;  TB[7][3]= 1;
    TB[8][0]=2; TB[8][1]=3;  TB[8][2]=5;  TB[8][3]= 1;
    TB[9][0]=2; TB[9][1]=4;  TB[9][2]=7;  TB[9][3]= 1;
    TB[10][0]=2; TB[10][1]=6;  TB[10][2]=9;  TB[10][3]= 1;
    TB[11][0]=3; TB[11][1]=4;  TB[11][2]=7;  TB[11][3]= 1;
    TB[12][0]=3; TB[12][1]=5;  TB[12][2]=6;  TB[12][3]= 1;
    TB[13][0]=4; TB[13][1]=6;  TB[13][2]=9;  TB[13][3]= 1;


  }
  if (N==11) {

    TB.set_size(17,4);
    TB[0][0]=0; TB[0][1]=1;  TB[0][2]=6;  TB[0][3]= 1;
    TB[1][0]=0; TB[1][1]=2;  TB[1][2]=4;  TB[1][3]= 1;
    TB[2][0]=0; TB[2][1]=5;  TB[2][2]=6;  TB[2][3]= 1;
    TB[3][0]=0; TB[3][1]=5;  TB[3][2]=7;  TB[3][3]= 1;
    TB[4][0]=0; TB[4][1]=5;  TB[4][2]=9;  TB[4][3]= 1;
    TB[5][0]=1; TB[5][1]=2;  TB[5][2]=4;  TB[5][3]= 1;
    TB[6][0]=1; TB[6][1]=2;  TB[6][2]=7;  TB[6][3]= 1;
    TB[7][0]=1; TB[7][1]=4;  TB[7][2]=6;  TB[7][3]= 1;
    TB[8][0]=2; TB[8][1]=8;  TB[8][2]=10;  TB[8][3]= 1;
    TB[9][0]=3; TB[9][1]=5;  TB[9][2]=6;  TB[9][3]= 1;
    TB[10][0]=3; TB[10][1]=6;  TB[10][2]=9;  TB[10][3]= 1;
    TB[11][0]=3; TB[11][1]=6;  TB[11][2]=10;  TB[11][3]= 1;
    TB[12][0]=3; TB[12][1]=7;  TB[12][2]=8;  TB[12][3]= 1;
    TB[13][0]=3; TB[13][1]=9;  TB[13][2]=10;  TB[13][3]= 1;
    TB[14][0]=4; TB[14][1]=7;  TB[14][2]=10;  TB[14][3]= 1;
    TB[15][0]=4; TB[15][1]=8;  TB[15][2]=9;  TB[15][3]= 1;
    TB[16][0]=5; TB[16][1]=6;  TB[16][2]=8;  TB[16][3]= 1;


  }
  if (N==12) {

    TB.set_size(19,4);

    TB[0][0]=0; TB[0][1]=1;  TB[0][2]=4;  TB[0][3]= 1;
    TB[1][0]=0; TB[1][1]=1;  TB[1][2]=5;  TB[1][3]= 1;
    TB[2][0]=0; TB[2][1]=5;  TB[2][2]=8;  TB[2][3]= 1;
    TB[3][0]=0; TB[3][1]=7;  TB[3][2]=11;  TB[3][3]= 1;
    TB[4][0]=0; TB[4][1]=9;  TB[4][2]=11;  TB[4][3]= 1;
    TB[5][0]=1; TB[5][1]=2;  TB[5][2]=3;  TB[5][3]= 1;
    TB[6][0]=1; TB[6][1]=2;  TB[6][2]=4;  TB[6][3]= 1;
    TB[7][0]=1; TB[7][1]=2;  TB[7][2]=6;  TB[7][3]= 1;
    TB[8][0]=1; TB[8][1]=5;  TB[8][2]=11;  TB[8][3]= 1;
    TB[9][0]=1; TB[9][1]=7;  TB[9][2]=11;  TB[9][3]= 1;
    TB[10][0]=2; TB[10][1]=3;  TB[10][2]=8;  TB[10][3]= 1;
    TB[11][0]=3; TB[11][1]=4;  TB[11][2]=6;  TB[11][3]= 1;
    TB[12][0]=3; TB[12][1]=7;  TB[12][2]=9;  TB[12][3]= 1;
    TB[13][0]=4; TB[13][1]=5;  TB[13][2]=9;  TB[13][3]= 1;
    TB[14][0]=5; TB[14][1]=9;  TB[14][2]=10;  TB[14][3]= 1;
    TB[15][0]=6; TB[15][1]=8;  TB[15][2]=10;  TB[15][3]= 1;
    TB[16][0]=6; TB[16][1]=8;  TB[16][2]=11;  TB[16][3]= 1;
    TB[17][0]=7; TB[17][1]=9;  TB[17][2]=10;  TB[17][3]= 1;
    TB[18][0]=9; TB[18][1]=10;  TB[18][2]=11;  TB[18][3]= 1;


  }
  vcl_cout<<TB;



  vnl_matrix <int> TBbool(N*N*N,1,0);
  for (unsigned i=0;i<TB.rows();i++)
    TBbool(TB[i][0]*N*N+TB[i][1]*N+TB[i][2],0)=1;
  return TBbool;
}


/*
--------N is: 7
10
8
8
9
8
9
8

--------
0 1 2 1
0 1 5 1
0 2 3 1
0 2 5 1
0 2 6 1
0 3 5 1
0 3 6 1
0 4 5 1
0 4 6 1
0 5 6 1
1 2 3 1
1 2 5 1
1 3 4 1
1 3 6 1
1 4 5 1
1 4 6 1
2 3 4 1
2 3 6 1
3 4 5 1
4 5 6 1
------------------------
--------N is: 8
10
8
9
8
10
8
9
10

--------
0 1 3 1
0 1 6 1
0 2 7 1
0 3 5 1
0 3 6 1
0 4 5 1
0 4 6 1
0 4 7 1
0 5 7 1
0 6 7 1
1 2 3 1
1 2 4 1
1 2 6 1
1 2 7 1
1 3 7 1
1 4 5 1
2 3 5 1
2 4 5 1
2 4 6 1
2 6 7 1
3 4 5 1
3 4 7 1
4 6 7 1
5 6 7 1
------------------------
--------N is: 9
12
8
8
8
8
11
9
9
8

--------
0 1 3 1
0 1 4 1
0 1 7 1
0 2 3 1
0 2 6 1
0 2 8 1
0 3 5 1
0 4 6 1
0 4 7 1
0 5 7 1
0 6 7 1
0 6 8 1
1 2 6 1
1 3 4 1
1 4 5 1
1 5 6 1
1 5 7 1
2 4 7 1
2 5 6 1
2 5 8 1
2 7 8 1
3 4 5 1
3 4 8 1
3 5 8 1
3 6 7 1
5 6 8 1
5 7 8 1
------------------------
--------N is: 10
12
11
10
9
10
9
9
10
8
8

--------
0 1 4 1
0 1 6 1
0 1 8 1
0 2 3 1
0 2 4 1
0 2 9 1
0 3 5 1
0 3 6 1
0 3 7 1
0 4 5 1
0 5 8 1
0 5 9 1
1 2 4 1
1 2 5 1
1 2 7 1
1 2 8 1
1 3 9 1
1 4 7 1
1 5 6 1
1 6 9 1
2 3 9 1
2 4 6 1
2 7 8 1
3 4 8 1
3 5 7 1
3 6 7 1
4 6 7 1
4 6 8 1
4 7 9 1
5 6 8 1
5 7 9 1
7 8 9 1
------------------------
--------N is: 11
10
8
8
14
11
10
13
13
11
10
9

--------
0 1 6 1
0 1 7 1
0 1 10 1
0 2 3 1
0 2 4 1
0 2 5 1
0 3 4 1
0 4 6 1
0 5 6 1
0 7 9 1
1 3 7 1
1 4 7 1
1 6 8 1
1 7 8 1
1 9 10 1
2 4 5 1
2 5 8 1
2 5 9 1
2 6 8 1
2 7 9 1
3 4 6 1
3 5 7 1
3 5 8 1
3 6 7 1
3 6 8 1
3 6 9 1
3 6 10 1
3 7 8 1
3 7 9 1
3 8 10 1
3 9 10 1
4 5 6 1
4 5 10 1
4 6 7 1
4 6 10 1
4 8 9 1
5 7 9 1
7 8 10 1
8 9 10 1
*/



bool  ddd(const float& rhs, const float& lhs)
{
  return rhs < lhs;
};

void dbcri_whole_process::out_removal(vcl_vector <vgl_point_3d <double> > & pts_3d) 
{

  vgl_point_3d <double> max_x_pts_3d,min_x_pts_3d;
  vgl_point_3d <double> max_y_pts_3d,min_y_pts_3d;
  vgl_point_3d <double> max_z_pts_3d,min_z_pts_3d;
  double max_x=vcl_numeric_limits<double>::min();
  double min_x=vcl_numeric_limits<double>::infinity();
  double max_y=vcl_numeric_limits<double>::min();
  double min_y=vcl_numeric_limits<double>::infinity();
  double max_z=vcl_numeric_limits<double>::min();
  double min_z=vcl_numeric_limits<double>::infinity();

  double mean_dist=0.0;
  int N=0;
  N=pts_3d.size();

  ////min max
  for (unsigned i=0;i< pts_3d.size(); i++)
  {  
    if (max_z<pts_3d[i].z()) {
      max_z=pts_3d[i].z();
      max_z_pts_3d=pts_3d[i];
    }
    if (min_z>pts_3d[i].z()) {
      min_z=pts_3d[i].z();
      min_z_pts_3d=pts_3d[i];
    }
  }

  //// mean distance
  int d_counter=0;
  for (unsigned i=0;i< pts_3d.size(); i++){

    for (unsigned j=i+1;j< pts_3d.size(); j++)
    {
      double d=vgl_distance(pts_3d[i],pts_3d[j]);
      d_counter++;
      // vcl_cout<< pts_3d[i].z()<<vcl_endl;   
      mean_dist+=d;

    }
  }
  //mean_dist=2.0*mean_dist/N*(N-1);
  mean_dist/=d_counter;
  vcl_cout<<"out removal ->mean distance:"<< mean_dist<<vcl_endl;
  vcl_cout<<mean_dist<<"N: "<< N<<vcl_endl;





  vcl_vector <float> d_list;
  double global_min_d=0.0;
  for (unsigned i=0;i< pts_3d.size(); i++){

    for (unsigned j=0;j< pts_3d.size(); j++)
    {
      if (i==j) continue;
      double d=vgl_distance(pts_3d[i],pts_3d[j]);
      // d_counter++;
      // vcl_cout<< pts_3d[i].z()<<vcl_endl;   
      // mean_dist+=d;
      d_list.push_back( d);
      if (global_min_d>d) global_min_d=d;
    }
  }

  vcl_sort(d_list.begin(), d_list.end(), ddd);

  int mid_index=vcl_floor(d_list.size()/3.0);
  double mid_distance=d_list[mid_index];

  vcl_cout<<"mid index and distace :" << mid_index<<" "<<mid_distance<<vcl_endl;
  d_counter=0;
  for (unsigned i=0;i< pts_3d.size(); i++){
    double min_d=vcl_numeric_limits<double>::infinity();
    for (unsigned j=0;j< pts_3d.size(); j++)
    {
      if (i==j) continue;
      double d=vgl_distance(pts_3d[i],pts_3d[j]);
      // d_counter++;
      // vcl_cout<< pts_3d[i].z()<<vcl_endl;   
      // mean_dist+=d;
      if (min_d>d) min_d=d;
    }

    if (min_d> 5*mid_distance) {
      vcl_vector<vgl_point_3d <double> >::iterator it;
      it = pts_3d.begin();
      vcl_cout<<"removed min dist: "<<min_d<<" "<<pts_3d[i]<<vcl_endl;
      pts_3d.erase(it+i); //sel_j is alway bigger than sel_i so erase sel_j first
      i--;
    }
  }

  vcl_cout<<"reduced_size -+-+-="<<pts_3d.size()<<vcl_endl;
  vcl_cout<<"*&* --- After deletion ---"<<vcl_endl;
  if (0)
    for (unsigned i=0;i< pts_3d.size(); i++){
      vcl_cout<<pts_3d[i]<<vcl_endl;
    }

    vcl_cout<<"*&* --- After deletion End ---"<<vcl_endl;
}


void dbcri_whole_process::out_removal_1(vcl_vector <vgl_point_3d <double> > & pts_3d) 
{

  vgl_point_3d <double> max_x_pts_3d,min_x_pts_3d;
  vgl_point_3d <double> max_y_pts_3d,min_y_pts_3d;
  vgl_point_3d <double> max_z_pts_3d,min_z_pts_3d;
  double max_x=vcl_numeric_limits<double>::min();
  double min_x=vcl_numeric_limits<double>::infinity();
  double max_y=vcl_numeric_limits<double>::min();
  double min_y=vcl_numeric_limits<double>::infinity();
  double max_z=vcl_numeric_limits<double>::min();
  double min_z=vcl_numeric_limits<double>::infinity();

  double mean_dist=0.0;
  int N=0;
  N=pts_3d.size();

  ////min max
  for (unsigned i=0;i< pts_3d.size(); i++)
  {  
    if (max_z<pts_3d[i].z()) {
      max_z=pts_3d[i].z();
      max_z_pts_3d=pts_3d[i];
    }
    if (min_z>pts_3d[i].z()) {
      min_z=pts_3d[i].z();
      min_z_pts_3d=pts_3d[i];
    }
  }

  //// mean distance
  int d_counter=0;
  for (unsigned i=0;i< pts_3d.size(); i++){

    for (unsigned j=i+1;j< pts_3d.size(); j++)
    {
      double d=vgl_distance(pts_3d[i],pts_3d[j]);
      d_counter++;
      // vcl_cout<< pts_3d[i].z()<<vcl_endl;   
      mean_dist+=d;

    }
  }
  //mean_dist=2.0*mean_dist/N*(N-1);
  mean_dist/=d_counter;
  vcl_cout<<"out removal ->mean distance:"<< mean_dist<<vcl_endl;
  vcl_cout<<mean_dist<<"N: "<< N<<vcl_endl;





  vcl_vector <float> d_list;
  double global_min_d=0.0;
  for (unsigned i=0;i< pts_3d.size(); i++){

    for (unsigned j=0;j< pts_3d.size(); j++)
    {
      if (i==j) continue;
      double d=vgl_distance(pts_3d[i],pts_3d[j]);
      // d_counter++;
      // vcl_cout<< pts_3d[i].z()<<vcl_endl;   
      // mean_dist+=d;
      d_list.push_back( d);
      if (global_min_d>d) global_min_d=d;
    }
  }

  vcl_sort(d_list.begin(), d_list.end(), ddd);

  int mid_index=vcl_floor(d_list.size()/3.0);
  double mid_distance=d_list[mid_index];

  vcl_cout<<"mid index and distace :" << mid_index<<" "<<mid_distance<<vcl_endl;
  d_counter=0;
  for (unsigned i=0;i< pts_3d.size(); i++){
    double min_d=vcl_numeric_limits<double>::infinity();
    for (unsigned j=0;j< pts_3d.size(); j++)
    {
      if (i==j) continue;
      double d=vgl_distance(pts_3d[i],pts_3d[j]);
      // d_counter++;
      // vcl_cout<< pts_3d[i].z()<<vcl_endl;   
      // mean_dist+=d;
      if (min_d>d) min_d=d;
    }

    if (min_d> 5*mid_distance) {
      vcl_vector<vgl_point_3d <double> >::iterator it;
      it = pts_3d.begin();
      vcl_cout<<"removed min dist: "<<min_d<<" "<<pts_3d[i]<<vcl_endl;
      pts_3d.erase(it+i); //sel_j is alway bigger than sel_i so erase sel_j first

      vcl_vector <pts_3d_attribute>::iterator it_a;
      it_a = pts_3d_a_.begin();
      pts_3d_a_.erase(it_a+i);



      i--;
    }
  }

  vcl_cout<<"reduced_size -+-+-="<<pts_3d.size()<<vcl_endl;
  vcl_cout<<"*&* --- After deletion ---"<<vcl_endl;
  for (unsigned i=0;i< pts_3d.size(); i++){
    vcl_cout<<pts_3d[i]<<vcl_endl;
  }

  vcl_cout<<"*&* --- out removal_1 After deletion End ---"<<vcl_endl;
}


//4-21-07//4-21-07
void dbcri_whole_process::angle_modify() {

  bool adaptive_angle_adjustment=true;  //if want to fixed pos_angle and neg_angle necessary and fixed incerement as old
  //use false

  if (adaptive_angle_adjustment) {
    vgl_point_2d <double> top_p=*(pts_upper_line_.end()-1);
    vgl_point_2d <double> bot_p=*(pts_lower_line_.end()-1);
    //theta_pos_=-vcl_atan((uppoint.y()-epi_y_)/(1000.0f-epi_x_));
    //theta_neg_=-vcl_atan((lowpoint.y()-epi_y_)/(1000.0f-epi_x_));

    vcl_cout<<"*&* "<<-theta_pos_<<" "<<-theta_neg_;
    theta_pos_=-vcl_atan((top_p.y()-epi_y_)/(top_p.x()-epi_x_));
    theta_neg_=-vcl_atan((bot_p.y()-epi_y_)/(bot_p.x()-epi_x_));

    ang_=(-theta_neg_-(-theta_pos_))/200.0;
    vcl_cout<<"--> thera_pos_ and neg_ changed to: "<<-theta_pos_<<" "<<-theta_neg_<<"ang_: "<<ang_<<vcl_endl;
  }

  else  ang_=.002; 
}


// epipole from previous result file..
bool dbcri_whole_process::read_epi_search()
{
  bpro_filepath wc_path;

  bool epi_found=false;

  parameters()->get_value( "-wc_filename" , wc_path );



  //    vcl_cout<<vul_file::dirname(wc_path);
  vcl_string wc_filename = wc_path.path;
  // test if fname is a directory
  //vul_file::exists

  //vul_reg_exp r1("res_5_2");
  //vul_reg_exp r3("res_6_");
  vul_reg_exp r1("res");
  vul_reg_exp r2("res");
  vul_reg_exp r3("07.txt");
  vul_reg_exp r4("08.txt");

  vul_file_iterator fn=vul_file::dirname(wc_path.path)+"/*.txt";

  for ( ; fn; ++fn) 
  {
    vcl_string input_file = fn();

    if (
      r1.find(input_file.c_str()) && r3.find(input_file.c_str()) ||
      r1.find(input_file.c_str()) && r4.find(input_file.c_str()) 
      )
    {
      vcl_ifstream fp(input_file.c_str());
      if (!fp) {
        vcl_cout<<" Unable to Open "<< wc_filename <<vcl_endl;
        return false;
      }

      vcl_string hhh;
      while(!fp.eof()) {
        fp>>hhh;
        if (hhh=="was_") {
          fp>> epi_x_;
          fp>> epi_y_;
          epi_found=true;

        }
      }
    }
  }

  if (epi_found) {
    vcl_cout<<"epipole found in previous result file and used!! "<<epi_x_<<" "<<epi_y_<<vcl_endl;
    vcl_cerr<<"epipole found in previous result file and used!! "<<epi_x_<<" "<<epi_y_<<vcl_endl;
  }
  return epi_found;
}

vnl_vector <double> dbcri_whole_process::epi_constraint_on_the_plane(
  double epi_x, double epi_y, double search_range, double search_step_size)
{

  vcl_cout<<"dbcri_whole_process::epi_constraint_on_the_plane()"<<vcl_endl;
  vcl_cerr<<"dbcri_whole_process::epi_constraint_on_the_plane()"<<vcl_endl;
  vcl_cout<<"epi in epi const: "<<epi_x<<" "<<epi_y<<vcl_endl;
  vnl_double_3x4 WC=WC_;
  vnl_double_3 xep = WC.get_column(0);   
  double epi_x_x= (xep[0]/xep[2]); //point of infinity -x direction
  double epi_x_y= (xep[1]/xep[2]);


  vnl_double_3 yep = WC.get_column(1); 

  double epi_y_x= (yep[0]/yep[2]);
  double epi_y_y= (yep[1]/yep[2]);


  vgl_point_2d <double> epi_point_x(epi_x_x,epi_x_y);
  vgl_point_2d <double> epi_point_y(epi_y_x,epi_y_y);
  vgl_line_2d <double> l(epi_point_x, epi_point_y);

  double a=l.a(),b=l.b(),c=l.c();
  vcl_cout<<"epi line equation"<<l<<vcl_endl;
  vcl_vector <vnl_double_2> epi_list;
  for (double x=-search_range+epi_x;x<=search_range+epi_x;x+=search_step_size) {
    //ax+by+c=0;
    //y=-1/b(ax+c)

    double y=-1/b*(a*x+c);
    vnl_double_2 epi(x,y);
    epi_list.push_back(epi);
    vcl_cout<<epi<<vcl_endl;

  }

  //return epi_list;





  vnl_vector <double> tnew_selected;
  double sum;double max_sum=-100000000000.0;
  vnl_vector <double> tnew_max;
  double max_epi_x,max_epi_y;
  for (unsigned i=0;i<epi_list.size();i++) {
    epi_x_=epi_list[i][0];epi_y_=epi_list[i][1];
    if (output_data_.size() <7)
      sum=epi_search(tnew_selected,0,0,25.0);// 25.0 means nothing
    else
      sum=epi_search_lite(tnew_selected,0,0,25.0);//100 50 50  4-22-07 old values

    if (sum>max_sum) {

      max_sum=sum;

      tnew_max=tnew_selected;

      max_epi_x=epi_x_;
      max_epi_y=epi_y_;

    }
  }
  tnew_selected=tnew_max;
  init_cameras(tnew_selected);
  epi_x_=max_epi_x;
  epi_y_=max_epi_y;


  vcl_cout<<"epi plane constraint"<<vcl_endl;
  vcl_cout<<"linear search max val epi is : "<<epi_x_<<" "<<epi_y_<<vcl_endl;


  return tnew_selected;
}



float dbcri_whole_process::solve_z(vcl_vector <vgl_point_2d <double> >pts_upper,vcl_vector <vgl_point_2d <double> >pts_lower)
{

  double z_ave_1=0;
  double z_ave_2=0;

  vcl_cout<<"---solve Z from tangent line---"<<vcl_endl;
  vnl_double_3x4 A=WC_;
  double Y1=mean_y_1_;double Y2=mean_y_2_;
  A[0][1]*=mean_y_1_;
  A[1][1]*=mean_y_1_;
  A[2][1]*=mean_y_1_;
  //vgl_line_2d <double> line_upper=reg_upper.get_line();
  //vgl_line_2d <double> line_lower=reg_lower.get_line();

  //line_upper_=line_upper;
  //line_lower_=line_lower;

  vcl_cout<<line_upper_<<" "<<line_lower_<<vcl_endl;


  //double epi_x=(a*d-b*c)/(c-a);
  //double epi_y=(b-d)/(c-a);

  int counter1=0;int counter2=0;


  for (unsigned i=0;i<pts_upper.size();i++) 
    for (float j=-30.0;j<50;j+=5.0){  //around tangential point..
      double x_im=pts_upper[i].x()+j;
      double y_im=(-line_upper_.a() * x_im -line_upper_.c())/line_upper_.b();

      //X=(A[0][1]-A[2][1]*x_im +A[0][3]-A[2][3]*x_im +(A[0][2]-A[2][2]*x_im) * Z)/(A[2][0]*x_im-A[0][0]) ;
      //X=(A[1][1]-A[2][1]*y_im +A[1][3]-A[2][3]*y_im +(A[1][2]-A[2][2]*y_im) * Z)/(A[2][0]*y_im-A[1][0]) ;

      double C1=(A[0][1]-A[2][1]*x_im +A[0][3]-A[2][3]*x_im)/(A[2][0]*x_im-A[0][0]); 
      double C2=(A[0][2]-A[2][2]*x_im)/(A[2][0]*x_im-A[0][0]);

      double D1=(A[1][1]-A[2][1]*y_im +A[1][3]-A[2][3]*y_im)/(A[2][0]*y_im-A[1][0]); 
      double D2=(A[1][2]-A[2][2]*y_im)/(A[2][0]*y_im-A[1][0]);
      double Z=-(C1-D1)/(C2-D2);
      double X=C1+C2*Z;
      double Y=mean_y_1_;
      vcl_cout<<i<<": X Y Z= "<<X<<" "<<Y<<" "<<Z<<vcl_endl;
      z_ave_1+=Z;counter1++;
    }



    A=WC_;
    Y1=mean_y_1_;Y2=mean_y_2_;
    A[0][1]*=mean_y_2_;
    A[1][1]*=mean_y_2_;
    A[2][1]*=mean_y_2_;
    //vgl_line_2d <double> line_upper=reg_upper.get_line();
    //vgl_line_2d <double> line_lower=reg_lower.get_line();

    //line_upper_=line_upper;
    //line_lower_=line_lower;

    vcl_cout<<line_upper_<<" "<<line_lower_<<vcl_endl;


    //double epi_x=(a*d-b*c)/(c-a);
    //double epi_y=(b-d)/(c-a);


    for (unsigned i=0;i<pts_upper.size();i++) 
      for (float j=-30.0;j<50;j+=5.0){  //around tangential point.. pixel

        double x_im=pts_upper[i].x()+j;
        double y_im=(-line_upper_.a() * x_im -line_upper_.c())/line_upper_.b();

        //X=(A[0][1]-A[2][1]*x_im +A[0][3]-A[2][3]*x_im +(A[0][2]-A[2][2]*x_im) * Z)/(A[2][0]*x_im-A[0][0]) ;
        //X=(A[1][1]-A[2][1]*y_im +A[1][3]-A[2][3]*y_im +(A[1][2]-A[2][2]*y_im) * Z)/(A[2][0]*y_im-A[1][0]) ;

        double C1=(A[0][1]-A[2][1]*x_im +A[0][3]-A[2][3]*x_im)/(A[2][0]*x_im-A[0][0]); 
        double C2=(A[0][2]-A[2][2]*x_im)/(A[2][0]*x_im-A[0][0]);

        double D1=(A[1][1]-A[2][1]*y_im +A[1][3]-A[2][3]*y_im)/(A[2][0]*y_im-A[1][0]); 
        double D2=(A[1][2]-A[2][2]*y_im)/(A[2][0]*y_im-A[1][0]);
        double Z=-(C1-D1)/(C2-D2);
        double X=C1+C2*Z;
        double Y=mean_y_2_;
        vcl_cout<<i<<": X Y Z= "<<X<<" "<<Y<<" "<<Z<<vcl_endl;
        z_ave_2+=Z;counter2++;
      }

      //deal with when a==0 (ax+by=c)
      if (line_upper_.a()==0.0) {


        // epi_y=-line_upper.c()/line_upper.b();
        // epi_x=1/(line_lower.a())*
        //   (line_lower.b()*line_upper.c()/line_upper.b()-line_lower.c());

      }

      // epi_x_=epi_x;
      // epi_y_=epi_y;


      solve_z_.clear();
      z_ave_1/=counter1;
      z_ave_2/=counter2;
      vcl_cout<<z_ave_1<<" "<<z_ave_2<<vcl_endl;
      solve_z_.set_size(2);
      solve_z_[0]=(z_ave_1);
      solve_z_[1]=(z_ave_2);
      return 1.0;
}



float dbcri_whole_process::solve_front_windshield(vcl_vector <vgl_point_2d <double> >pts_upper,vcl_vector <vgl_point_2d <double> >pts_lower)
{



  /* vcl_cout<<"*&* "<<-theta_pos_<<" "<<-theta_neg_;
  theta_pos_=-vcl_atan((top_p.y()-epi_y_)/(top_p.x()-epi_x_));
  theta_neg_=-vcl_atan((bot_p.y()-epi_y_)/(bot_p.x()-epi_x_));

  double angle_temp=(-theta_neg_-(-theta_pos_))/200.0; */
  // for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=ang_ /*was .002*/) {
  ////angle goes clockwise..



  double z_ave_1=0;
  double z_ave_2=0;

  vcl_cout<<"---solve_front_windshield---"<<vcl_endl;


  // vcl_cout<<line_upper_<<" "<<line_lower_<<vcl_endl;


  /* for (unsigned con_i=0;con_i<veh_cons_.size();con_i++)
  {
  int N=veh_cons_[con_i][0].size();
  double *px,*py;
  px = (double *)malloc(sizeof(double)*N);
  py = (double *)malloc(sizeof(double)*N);
  for (unsigned int s = 0; s < veh_cons_[con_i].num_sheets(); ++s)
  for (unsigned int p = 0; p < veh_cons_[con_i][s].size(); ++p)
  {
  //do_something(polygon[s][p].x(), polygon[s][p].y());
  px[p]=veh_cons_[con_i][s][p].x();
  py[p]=veh_cons_[con_i][s][p].y();
  */
  int counter1=0;int counter2=0;

  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_1; 
  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_2; 
  vcl_vector <vgl_point_3d<double> > pts_3d_here;
  //vcl_vector <vgl_point_3d<double> > pts_3d_here_2;

  int s=0;
  for (unsigned con_i=0;con_i<veh_cons_.size();con_i++) //front...so max x...
  {

    vnl_double_3x4 A=cameras_[con_i];
    double Y1=mean_y_1_;double Y2=mean_y_2_;
    A[0][1]*=mean_y_1_; ///mean_y_1_ is front
    A[1][1]*=mean_y_1_;
    A[2][1]*=mean_y_1_;

    pts_3d_here.clear();
    int N=veh_cons_[con_i][0].size();
    for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=ang_ /*was .002*/) {
      double x_1=2000.0;
      double y_1=epi_y_+vcl_tan(ang)*(x_1-epi_x_);
      //      double cont2[] = { epi_x_,epi_y_, x_2,y_2 ,x_2,y_2+2000, epi_x_,epi_y_+2000};
      //      vgl_polygon<double> poly2(cont2, 4);
      double cont1[] = { epi_x_,epi_y_, x_1,y_1 , x_1+.0000001,y_1+.0000001 ,epi_x_+.0000001,epi_y_+.0000001};
      vgl_polygon<double> poly1(cont1, 4);

      vgl_polygon<double> result = vgl_clip( veh_cons_[con_i], poly1, vgl_clip_type_intersect );


      vcl_cout<<"result: "<<result;//<<vcl_endl;
      if (result.num_sheets()!=1||result.num_vertices() == 0) continue;

      if (result[0].size()!=4) vcl_cout<<"result polygon size wrong !!!"<<vcl_endl;
      int im=-1;
      double xm=10000000;
      //first, find minimum x 
      for (unsigned i=0;i<4;i++)
      {
        if (xm>result[0][i].x()) { xm=result[0][i].x();im=i;}
      }
      //second, find max x 
      double xmx=-10000000;
      int imx=-1;
      for (unsigned i=0;i<4;i++)
      {
        if (xmx<result[0][i].x()) { xmx=result[0][i].x();imx=i;}
      }

      double x_im=result[0][imx].x();
      double y_im=result[0][imx].y();
      //double x_im=veh_cons_[con_i][s][p].x();
      //double y_im=veh_cons_[con_i][s][p].y();

      double C1=(A[0][1]-A[2][1]*x_im +A[0][3]-A[2][3]*x_im)/(A[2][0]*x_im-A[0][0]); 
      double C2=(A[0][2]-A[2][2]*x_im)/(A[2][0]*x_im-A[0][0]);

      double D1=(A[1][1]-A[2][1]*y_im +A[1][3]-A[2][3]*y_im)/(A[2][0]*y_im-A[1][0]); 
      double D2=(A[1][2]-A[2][2]*y_im)/(A[2][0]*y_im-A[1][0]);
      double Z=-(C1-D1)/(C2-D2);
      double X=C1+C2*Z;
      double Y=mean_y_1_;

      vgl_point_3d <double> pt(X,Y,Z);
      pts_3d_here.push_back(pt);
      vcl_cout<<con_i<< "1 "<<ang<<": X Y Z= "<<X<<" "<<Y<<" "<<Z<<vcl_endl<<vcl_endl;
      z_ave_1+=Z;counter1++;
    }
    pts_3d_here_list_1.push_back(pts_3d_here);
  }


  //A=WC_;





  for (unsigned con_i=0;con_i<veh_cons_.size();con_i++)
  {
    vnl_double_3x4 A=cameras_[con_i];
    double Y1=mean_y_1_;double Y2=mean_y_2_;
    A[0][1]*=mean_y_2_;
    A[1][1]*=mean_y_2_;
    A[2][1]*=mean_y_2_;

    pts_3d_here.clear();
    int N=veh_cons_[con_i][0].size();
    for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=ang_ /*was .002*/) {
      double x_2=2000.0;
      double y_2=epi_y_+vcl_tan(ang)*(x_2-epi_x_);
      // double cont2[] = { epi_x_,epi_y_, x_1,y_1 ,x_1,y_1+2000, epi_x_,epi_y_+2000};
      // vgl_polygon<double> poly2(cont2, 4);
      double cont2[] = { epi_x_,epi_y_, x_2,y_2 , x_2+.0000001, y_2+.0000001,epi_x_+.0000001,epi_y_+.0000001};
      vgl_polygon<double> poly2(cont2, 4);
      vcl_cout<<x_2<<" "<<y_2<<vcl_endl;
      vgl_polygon<double> result = vgl_clip( veh_cons_[con_i], poly2, vgl_clip_type_intersect );


      vcl_cout<<"result: "<<result;
      if (result.num_sheets()!=1||result.num_vertices() == 0) continue;

      if (result[0].size()!=4) vcl_cout<<"result polygon size wrong !!!"<<vcl_endl;
      int im=-1;
      double xm=10000000;
      //first, find minimum x 
      for (unsigned i=0;i<4;i++)
      {
        if (xm>result[0][i].x()) { xm=result[0][i].x();im=i;}
      }
      //second, find max x 
      double xmx=-10000000;
      int imx=-1;
      for (unsigned i=0;i<4;i++)
      {
        if (xmx<result[0][i].x()) { xmx=result[0][i].x();imx=i;}
      }




      double x_im=result[0][im].x();  // min x necessary
      double y_im=result[0][im].y(); //min 


      //6-18-07// double x_im=pts_upper[i].x()+j;
      //6-18-07// double y_im=(-line_upper_.a() * x_im -line_upper_.c())/line_upper_.b();
      //      double x_im=veh_cons_[con_i][s][p].x();
      //      double y_im=veh_cons_[con_i][s][p].y();


      double C1=(A[0][1]-A[2][1]*x_im +A[0][3]-A[2][3]*x_im)/(A[2][0]*x_im-A[0][0]); 
      double C2=(A[0][2]-A[2][2]*x_im)/(A[2][0]*x_im-A[0][0]);

      double D1=(A[1][1]-A[2][1]*y_im +A[1][3]-A[2][3]*y_im)/(A[2][0]*y_im-A[1][0]); 
      double D2=(A[1][2]-A[2][2]*y_im)/(A[2][0]*y_im-A[1][0]);
      double Z=-(C1-D1)/(C2-D2);
      double X=C1+C2*Z;
      double Y=mean_y_2_;
      vcl_cout<<con_i<< "2 "<<ang<<": X Y Z= "<<X<<" "<<Y<<" "<<Z<<vcl_endl<<vcl_endl;
      z_ave_2+=Z;counter2++;


      vgl_point_3d <double> pt(X,Y,Z);
      pts_3d_here.push_back(pt);
    }

    pts_3d_here_list_2.push_back(pts_3d_here);
  }

  //deal with when a==0 (ax+by=c)
  if (line_upper_.a()==0.0) {




  }






  vcl_ofstream vrml1("test1.wrl");
  vcl_ofstream vrml2("test2.wrl");
  world_color_flag_=true;

  float m=pts_3d_here_list_1.size();
  for (unsigned i=0;i<  pts_3d_here_list_1.size();i++) {
    r_world_=1-i/m;g_world_=.5;b_world_=i/m;

    write_vrml_2(vrml1,pts_3d_here_list_1[i]);
  }
  for (unsigned i=0;i<  pts_3d_here_list_2.size();i++) {
    r_world_=1-i/m;g_world_=.5;b_world_=i/m;

    write_vrml_2(vrml2,pts_3d_here_list_2[i]);
  }
  world_color_flag_=false;
  vrml1.close();
  vrml2.close();

  /* solve_z_.clear();
  z_ave_1/=counter1;
  z_ave_2/=counter2;
  vcl_cout<<z_ave_1<<" "<<z_ave_2<<vcl_endl;
  solve_z_.set_size(2);
  solve_z_[0]=(z_ave_1);
  solve_z_[1]=(z_ave_2);*/
  return 1.0;//aaaa

}



float dbcri_whole_process::solve_front_windshield()
{ ////angle goes clockwise..


  double z_ave_1=0;
  double z_ave_2=0;

  vcl_cout<<"----------------------------------------"<<vcl_endl;
  vcl_cout<<"---solve_front_windshield-2-subtraction-"<<vcl_endl;
  vcl_cout<<"----------------------------------------"<<vcl_endl;


  int counter1=0;int counter2=0;

  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_1; 
  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_2; 
  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_1_before_rot; 
  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_2_before_rot; 
  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_sub; 
  vcl_vector <vgl_point_3d<double> > pts_3d_here_1;
  vcl_vector <vgl_point_3d<double> > pts_3d_here_2;
  vcl_vector <vgl_point_3d<double> > pts_3d_here_1_before_rot;
  vcl_vector <vgl_point_3d<double> > pts_3d_here_2_before_rot;
  vcl_vector <vgl_point_3d<double> > pts_3d_here_sub;

  int s=0;
  double cabin_length_x_average=0;
  double cabin_width_y_average=0;
  double cabin_height_z_average=0;
  double z1=0,z2=0,y1=0,y2=0;
  double x1=0;
  double x2=0;

  //11-15-07
  double con_p0x_all=0.0, 
    con_p7x_all=0.0;


  for (unsigned con_i=0;con_i<veh_cons_.size();con_i++) //front...so max x...
  {

    vnl_double_3x4 A1=cameras_[con_i];

    A1[0][1]*=mean_y_1_; ///mean_y_1_ is front
    A1[1][1]*=mean_y_1_;
    A1[2][1]*=mean_y_1_;

    vnl_double_3x4 A2=cameras_[con_i];

    A2[0][1]*=mean_y_2_;
    A2[1][1]*=mean_y_2_;
    A2[2][1]*=mean_y_2_;

    pts_3d_here_1.clear(); pts_3d_here_1_before_rot.clear();
    pts_3d_here_2.clear(); pts_3d_here_2_before_rot.clear();
    pts_3d_here_sub.clear();
    int N=veh_cons_[con_i][0].size();

    double wedge=-theta_neg_-(-theta_pos_);

    double cabin_length_x=0;
    double cabin_width_y=0;
    double cabin_height_z=0;

    vnl_double_3 cabin_1_front(0,0,0);
    vnl_double_3 cabin_2_rear(0,0,0);
    int count_cabin=0;


    //11-15-2007 con method for p0
    int count_con_p0=0;
    vnl_double_3 con_p0(0,0,0); 
    vnl_double_3 con_p7(0,0,0); 

    for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=ang_ /*was .002*/) {
      double x_1=2000.0;
      double y_1=epi_y_+vcl_tan(ang)*(x_1-epi_x_);
      //      double cont2[] = { epi_x_,epi_y_, x_2,y_2 ,x_2,y_2+2000, epi_x_,epi_y_+2000};
      //      vgl_polygon<double> poly2(cont2, 4);
      double cont1[] = { epi_x_,epi_y_, x_1,y_1 , x_1+.0000001,y_1+.0000001 ,epi_x_+.0000001,epi_y_+.0000001};
      vgl_polygon<double> poly1(cont1, 4);

      vgl_polygon<double> result = vgl_clip( veh_cons_[con_i], poly1, vgl_clip_type_intersect );


      vcl_cout<<"result: "<<result;//<<vcl_endl;
      if (result.num_sheets()!=1||result.num_vertices() == 0) {
        vcl_cout<<"-----"<<vcl_endl;
        continue;
      }
      if (result[0].size()!=4) {
        vcl_cout<<"result polygon size wrong !!!"<<vcl_endl;
        vcl_cout<<"-_-_-_-_-"<<vcl_endl;
        continue;
      }
      int im=-1;
      double xm=10000000;
      //first, find minimum x 
      for (unsigned i=0;i<4;i++)
      {
        if (xm>result[0][i].x()) { xm=result[0][i].x();im=i;}
      }
      //second, find max x 
      double xmx=-10000000;
      int imx=-1;
      for (unsigned i=0;i<4;i++)
      {
        if (xmx<result[0][i].x()) { xmx=result[0][i].x();imx=i;}
      }

      double x_im=result[0][imx].x();
      double y_im=result[0][imx].y();
      //double x_im=veh_cons_[con_i][s][p].x();
      //double y_im=veh_cons_[con_i][s][p].y();

      double C1=(A1[0][1]-A1[2][1]*x_im +A1[0][3]-A1[2][3]*x_im)/(A1[2][0]*x_im-A1[0][0]); 
      double C2=(A1[0][2]-A1[2][2]*x_im)/(A1[2][0]*x_im-A1[0][0]);

      double D1=(A1[1][1]-A1[2][1]*y_im +A1[1][3]-A1[2][3]*y_im)/(A1[2][0]*y_im-A1[1][0]); 
      double D2=(A1[1][2]-A1[2][2]*y_im)/(A1[2][0]*y_im-A1[1][0]);
      double Z=-(C1-D1)/(C2-D2);
      double X=C1+C2*Z;
      double Y=mean_y_1_;
      vcl_cout<<con_i<< "1 "<<ang<<": X Y Z= "<<X<<" "<<Y<<" "<<Z<<vcl_endl;
      z_ave_1+=Z;counter1++;

      vgl_point_3d <double> pt1_before_rot(X,Y,Z);


      //rotation==> real length  and make it shorter..
      vnl_double_3 p10(X,Y,Z);
      vnl_double_3 p11=rot_*p10;




      vgl_point_3d <double> pt1(p11[0],p11[1],p11[2]);

      vcl_cout<<"   "<<pt1<<vcl_endl;




      x_im=result[0][im].x();  // min x necessary
      y_im=result[0][im].y(); //min 
      C1=(A2[0][1]-A2[2][1]*x_im +A2[0][3]-A2[2][3]*x_im)/(A2[2][0]*x_im-A2[0][0]); 
      C2=(A2[0][2]-A2[2][2]*x_im)/(A2[2][0]*x_im-A2[0][0]);

      D1=(A2[1][1]-A2[2][1]*y_im +A2[1][3]-A2[2][3]*y_im)/(A2[2][0]*y_im-A2[1][0]); 
      D2=(A2[1][2]-A2[2][2]*y_im)/(A2[2][0]*y_im-A2[1][0]);
      Z=-(C1-D1)/(C2-D2);
      X=C1+C2*Z;
      Y=mean_y_2_;
      vcl_cout<<con_i<< "2 "<<ang<<": X Y Z= "<<X<<" "<<Y<<" "<<Z<<vcl_endl;
      vgl_point_3d <double> pt2_before_rot(X,Y,Z);

      //rotation==> real length  and make it shorter..
      vnl_double_3 p20(X,Y,Z);
      vnl_double_3 p21=rot_*p20;

      z_ave_2+=Z;counter2++;
      vgl_point_3d <double> pt2(p21[0],p21[1],p21[2]);

      vcl_cout<<"   "<<pt2<<vcl_endl;


      vgl_point_3d <double> pt_sub(pt1.x()-pt2.x(),pt1.y()-pt2.y(),(pt1.z()+pt2.z())/2.0);
      pts_3d_here_sub.push_back(pt_sub);
      pts_3d_here_1.push_back(pt1);
      pts_3d_here_2.push_back(pt2);
      pts_3d_here_1_before_rot.push_back(pt1_before_rot);
      pts_3d_here_2_before_rot.push_back(pt2_before_rot);

      //if (ang>-theta_pos_+.05*wedge && ang<-theta_pos_+.2*wedge)////////?????????????????/////good number????
      if (ang>-theta_pos_+.1*wedge && ang<-theta_pos_+.25*wedge)////////?????????????????/////good number????
      {

        //vcl_cout<<"<--** top .05-.20 wedge changed to .1-.25"<<vcl_endl;
        cabin_length_x+=pt1.x()-pt2.x();

        cabin_1_front(0)+=pt1.x();
        cabin_2_rear(0)+=pt2.x();
        count_cabin++;

        // since not addition, last(lowest z) will be stored at final.
        cabin_1_front(1)=pt1.y();
        cabin_2_rear(1)=pt2.y();
        cabin_1_front(2)=pt1.z();
        cabin_2_rear(2)=pt2.z();

        cabin_width_y=vcl_abs(pt1.y()-pt2.y());
        cabin_height_z=(pt1.z()+pt2.z())/2.0;
        vcl_cout<<"cabin counting--"<<count_cabin<<" "<< ang<< " "<<cabin_2_rear(0)<< " "<< (float)(cabin_2_rear(0)/count_cabin)<<vcl_endl;
      }

      //11-15-2007 rear of vehicle..
      // for probe 0 use contour..

      if (ang>-theta_pos_+.5*wedge && ang<-theta_pos_+.9*wedge)////////?????????????????/////good number????
      {
        vcl_cout<<"*rear end--contour method.."<<vcl_endl;
        // cabin_length_x+=pt1.x()-pt2.x();

        // cabin_1_front(0)+=pt1.x();
        // cabin_2_rear(0)+=pt2.x();
        // count_cabin++;

        // since not addition, last(lowest z) will be stored at final.
        //cabin_1_front(1)=pt1.y();
        //cabin_2_rear(1)=pt2.y();
        //cabin_1_front(2)=pt1.z();
        //cabin_2_rear(2)=pt2.z();

        //cabin_width_y=vcl_abs(pt1.y()-pt2.y());
        //cabin_height_z=(pt1.z()+pt2.z())/2.0;


        count_con_p0++;

        con_p7(0)+=pt1.x();
        con_p0(0)+=pt2.x();
        con_p7(1)=pt1.y();
        con_p0(1)=pt2.y();
        con_p7(2)=pt1.z();
        con_p0(2)=pt2.z();
        vcl_cout<<"con p0 counting--"<<count_con_p0<<" "<< ang<< " "<<con_p0<<vcl_endl;
      }



      vcl_cout<<"----------------------------------------"<<vcl_endl;


    }

    //11-15-07 con_p0 and con_p7
    if ( count_con_p0>0)
    {
      con_p0x_all+=con_p0(0)/(count_con_p0*veh_cons_.size());
      con_p7x_all+=con_p7(0)/(count_con_p0*veh_cons_.size());
    }

    //

    if (count_cabin==0) continue;//7-1-2007 to avoid 0 case
    cabin_length_x_average+=cabin_length_x/( count_cabin*veh_cons_.size() );
    cabin_width_y_average+=cabin_width_y/( veh_cons_.size() );
    cabin_height_z_average+=cabin_height_z/( veh_cons_.size() );

    z1+=cabin_1_front(2)/( veh_cons_.size() );
    z2+=cabin_2_rear(2)/( veh_cons_.size() );
    y1+=cabin_1_front(1)/( veh_cons_.size() );
    y2+=cabin_2_rear(1)/( veh_cons_.size() );


    x1+=cabin_1_front(0)/( count_cabin*veh_cons_.size() );
    x2+=cabin_2_rear(0)/( count_cabin*veh_cons_.size() );

    vcl_cout<<con_i<<"'th cabin length "<< cabin_length_x/count_cabin <<" "<<count_cabin<<vcl_endl;
    vcl_cout<<con_i<<"'th cabin  front "<< cabin_1_front(0)/count_cabin<<vcl_endl;
    vcl_cout<<con_i<<"'th cabin  rear  "<< cabin_2_rear(0)/count_cabin<<vcl_endl;

    pts_3d_here_list_1.push_back(pts_3d_here_1);
    pts_3d_here_list_2.push_back(pts_3d_here_2);
    pts_3d_here_list_1_before_rot.push_back(pts_3d_here_1_before_rot);
    pts_3d_here_list_2_before_rot.push_back(pts_3d_here_2_before_rot);
    pts_3d_here_list_sub.push_back(pts_3d_here_sub);
  }


  //11-15-07
  p0_con_[0]=con_p0x_all;
  p7_con_[0]=con_p7x_all;


  p2_[0]=x2;p2_[1]=y2;p2_[2]=z2;
  p8_[0]=x1;p8_[1]=y1;p8_[2]=z1;

  p_cabin_length_[0]=cabin_length_x_average;
  p_cabin_length_[1]=cabin_width_y_average;
  p_cabin_length_[2]=cabin_height_z_average;


  {
    vcl_ofstream vrml1("test1.wrl");
    vcl_ofstream vrml2("test2.wrl");
    world_color_flag_=true;

    float m=pts_3d_here_list_1.size();
    for (unsigned i=0;i<  pts_3d_here_list_1.size();i++) {
      r_world_=1-i/m;g_world_=.5;b_world_=i/m;

      write_vrml_2(vrml1,pts_3d_here_list_1[i]);
    }
    for (unsigned i=0;i<  pts_3d_here_list_2.size();i++) {
      r_world_=1-i/m;g_world_=.5;b_world_=i/m;

      write_vrml_2(vrml2,pts_3d_here_list_2[i]);
    }
    world_color_flag_=false;
    vrml1.close();
    vrml2.close();
  }

  //before ratation
  {
    vcl_ofstream vrml1("test1_before_rot.wrl");
    vcl_ofstream vrml2("test2_before_rot.wrl");
    world_color_flag_=true;

    float m=pts_3d_here_list_1.size();
    for (unsigned i=0;i<  pts_3d_here_list_1_before_rot.size();i++) {
      r_world_=1-i/m;g_world_=.5;b_world_=i/m;

      write_vrml_2(vrml1,pts_3d_here_list_1_before_rot[i]);
    }
    for (unsigned i=0;i<  pts_3d_here_list_2_before_rot.size();i++) {
      r_world_=1-i/m;g_world_=.5;b_world_=i/m;

      write_vrml_2(vrml2,pts_3d_here_list_2_before_rot[i]);
    }
    world_color_flag_=false;
    vrml1.close();
    vrml2.close();
  }
  vcl_ofstream vrml3("test3.wrl");
  //vcl_ofstream vrml2("test2.wrl");
  world_color_flag_=true;

  float m=pts_3d_here_list_sub.size();
  for (unsigned i=0;i<  pts_3d_here_list_sub.size();i++) {
    r_world_=1-i/m;g_world_=.5;b_world_=i/m;

    write_vrml_2(vrml3,pts_3d_here_list_sub[i]);
  }
  /*for (unsigned i=0;i<  pts_3d_here_list_2.size();i++) {
  r_world_=1-i/m;g_world_=.5;b_world_=i/m;

  write_vrml_2(vrml2,pts_3d_here_list_2[i]);
  }*/
  world_color_flag_=false;
  vrml3.close();


  for (unsigned k=0;k<pts_3d_here_list_1.size();k++) 
  {
    vcl_cout<<k<<"----pt1 asas---"<<k<<vcl_endl;
    for (unsigned i=0;i<  pts_3d_here_list_1[k].size();i++) {
      vcl_cout<<pts_3d_here_list_1[k][i].x()<<" "<<pts_3d_here_list_2[k][i].x()<<" "<<(pts_3d_here_list_1[k][i].z()+pts_3d_here_list_2[k][i].z())/2.0<<vcl_endl;
    }
    vcl_cout<<"----pt1 asasend---"<<vcl_endl;
  }

  vcl_cout<<"----------------------"<<vcl_endl;
  vcl_cout<<"----------------------"<<vcl_endl;
  vcl_cout<<"----------------------"<<vcl_endl;

  for (unsigned k=0;k<pts_3d_here_list_sub.size();k++) 
  {
    vcl_cout<<k<<"----asas---"<<k<<vcl_endl;
    for (unsigned i=0;i<  pts_3d_here_list_sub[k].size();i++) {
      vcl_cout<<pts_3d_here_list_sub[k][i].x()<<" "<<pts_3d_here_list_sub[k][i].y()<<" "<<pts_3d_here_list_sub[k][i].z()<<vcl_endl;
    }
    vcl_cout<<"----asasend---"<<vcl_endl;
  }


  return 1.0;//aaaa

}


// contour points reconstruction
float dbcri_whole_process::solve_front_windshield1()
{ ////angle goes clockwise..

  double z_ave_1=0;
  double z_ave_2=0;

  vcl_cout<<"----------------------------------------"<<vcl_endl;
  vcl_cout<<"---solve_front_windshield-3-contour-"<<vcl_endl;
  vcl_cout<<"----------------------------------------"<<vcl_endl;

  int counter1=0;int counter2=0;

  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_1; 
  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_2; 
  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_sub; 
  vcl_vector <vgl_point_3d<double> > pts_3d_here_1;
  vcl_vector <vgl_point_3d<double> > pts_3d_here_2;
  vcl_vector <vgl_point_3d<double> > pts_3d_here_sub;

  int s=0;
  for (unsigned con_i=0;con_i<veh_cons_.size();con_i++) //front...so max x...
  {


    vnl_double_3x4 A1=cameras_[con_i];

    A1[0][1]*=mean_y_1_; ///mean_y_1_ is front
    A1[1][1]*=mean_y_1_;
    A1[2][1]*=mean_y_1_;

    vnl_double_3x4 A2=cameras_[con_i];

    A2[0][1]*=mean_y_2_;
    A2[1][1]*=mean_y_2_;
    A2[2][1]*=mean_y_2_;

    pts_3d_here_1.clear();
    pts_3d_here_2.clear();
    pts_3d_here_sub.clear();

    int N=veh_cons_[con_i][0].size();

    bool clockwise=false;
    if( vgl_area_signed(veh_cons_[con_i])>0)//clockwise ->positive..
      true;
    else clockwise=false;


    // 1. find top and bottom point..
    vnl_double_2 top_pt_1,top_pt_2,bot_pt_1,bot_pt_2;//top pt 1,2 and bottom pt 1,2

    bool first=true;
    for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=ang_ /*was .002*/) {
      double x_1=2000.0;
      double y_1=epi_y_+vcl_tan(ang)*(x_1-epi_x_);

      double cont1[] = { epi_x_,epi_y_, x_1,y_1 , x_1+.0000001,y_1+.0000001 ,epi_x_+.0000001,epi_y_+.0000001};
      vgl_polygon<double> poly1(cont1, 4);

      vgl_polygon<double> result = vgl_clip( veh_cons_[con_i], poly1, vgl_clip_type_intersect );


      vcl_cout<<"result: "<<result;//<<vcl_endl;
      if (result.num_sheets()!=1||result.num_vertices() == 0) continue;

      if (result[0].size()!=4) vcl_cout<<"result polygon size wrong !!!"<<vcl_endl;
      int im=-1;
      double xm=10000000;
      //first, find minimum x 
      for (unsigned i=0;i<4;i++)
      {
        if (xm>result[0][i].x()) { xm=result[0][i].x();im=i;}
      }
      //second, find max x 
      double xmx=-10000000;
      int imx=-1;
      for (unsigned i=0;i<4;i++)
      {
        if (xmx<result[0][i].x()) { xmx=result[0][i].x();imx=i;}
      }

      double x_im=result[0][imx].x();
      double y_im=result[0][imx].y();

      //double x_im=veh_cons_[con_i][s][p].x();
      //double y_im=veh_cons_[con_i][s][p].y();

      if (first) {
        top_pt_1[0]=result[0][imx].x();//1 is front of vehicle.
        top_pt_1[1]=result[0][imx].y();
        top_pt_2[0]=result[0][im].x();
        top_pt_2[1]=result[0][im].y();
        first=false;
      }
      bot_pt_1[0]=result[0][imx].x();//1 is front of vehicle.
      bot_pt_1[1]=result[0][imx].y();
      bot_pt_2[0]=result[0][im].x();
      bot_pt_2[1]=result[0][im].y();
    }

    vcl_cout<<top_pt_1<<" "<<top_pt_2<<" "<<bot_pt_1<<" "<<bot_pt_2<<vcl_endl;

    //2. generate subsequence of right(1) and left(2) .
    double *px,*py;
    px = (double *)malloc(sizeof(double)*N);
    py = (double *)malloc(sizeof(double)*N);
    for (unsigned int s = 0; s < veh_cons_[con_i].num_sheets(); ++s) {
      for (unsigned int p = 0; p < veh_cons_[con_i][s].size(); ++p)
      {
        //do_something(polygon[s][p].x(), polygon[s][p].y());
        px[p]=veh_cons_[con_i][s][p].x();
        py[p]=veh_cons_[con_i][s][p].y();
      }
    }
    int top_pt_idx1= vgl_closest_point_to_closed_polygon(top_pt_1[0], top_pt_1[1], px, py, N,  top_pt_1[0], top_pt_1[1]);//cccc
    int top_pt_idx2= vgl_closest_point_to_closed_polygon(top_pt_2[0], top_pt_2[1], px, py, N,  top_pt_2[0], top_pt_2[1]);//cccc
    int bot_pt_idx1= vgl_closest_point_to_closed_polygon(bot_pt_1[0], bot_pt_1[1], px, py, N,  bot_pt_1[0], bot_pt_1[1]);//cccc
    int bot_pt_idx2= vgl_closest_point_to_closed_polygon(bot_pt_2[0], bot_pt_2[1], px, py, N,  bot_pt_2[0], bot_pt_2[1]);//cccc

    vcl_cout<<top_pt_1<<" "<<top_pt_2<<" "<<bot_pt_1<<" "<<bot_pt_2<<vcl_endl;
    vcl_cout<<"idx: "<<top_pt_idx1<<" "<<top_pt_idx2<<" "<<bot_pt_idx1<<" "<<bot_pt_idx2<<vcl_endl;
    free(px);free(py);


    /* for (;;)
    {
    double C1=(A1[0][1]-A1[2][1]*x_im +A1[0][3]-A1[2][3]*x_im)/(A1[2][0]*x_im-A1[0][0]); 
    double C2=(A1[0][2]-A1[2][2]*x_im)/(A1[2][0]*x_im-A1[0][0]);

    double D1=(A1[1][1]-A1[2][1]*y_im +A1[1][3]-A1[2][3]*y_im)/(A1[2][0]*y_im-A1[1][0]); 
    double D2=(A1[1][2]-A1[2][2]*y_im)/(A1[2][0]*y_im-A1[1][0]);
    double Z=-(C1-D1)/(C2-D2);
    double X=C1+C2*Z;
    double Y=mean_y_1_;
    vcl_cout<<con_i<< "1 "<<ang<<": X Y Z= "<<X<<" "<<Y<<" "<<Z<<vcl_endl;
    z_ave_1+=Z;counter1++;
    vgl_point_3d <double> pt1(X,Y,Z);


    x_im=result[0][im].x();  // min x necessary
    y_im=result[0][im].y(); //min 
    C1=(A2[0][1]-A2[2][1]*x_im +A2[0][3]-A2[2][3]*x_im)/(A2[2][0]*x_im-A2[0][0]); 
    C2=(A2[0][2]-A2[2][2]*x_im)/(A2[2][0]*x_im-A2[0][0]);

    D1=(A2[1][1]-A2[2][1]*y_im +A2[1][3]-A2[2][3]*y_im)/(A2[2][0]*y_im-A2[1][0]); 
    D2=(A2[1][2]-A2[2][2]*y_im)/(A2[2][0]*y_im-A2[1][0]);
    Z=-(C1-D1)/(C2-D2);
    X=C1+C2*Z;
    Y=mean_y_2_;
    vcl_cout<<con_i<< "2 "<<ang<<": X Y Z= "<<X<<" "<<Y<<" "<<Z<<vcl_endl;
    vcl_cout<<"----------------------------------------"<<vcl_endl;

    z_ave_2+=Z;counter2++;
    vgl_point_3d <double> pt2(X,Y,Z);



    vgl_point_3d <double> pt_sub(pt1.x()-pt2.x(),pt1.y()-pt2.y(),(pt1.z()+pt2.z())/2.0);
    pts_3d_here_sub.push_back(pt_sub);
    pts_3d_here_1.push_back(pt1);
    pts_3d_here_2.push_back(pt2);


    }*/


    pts_3d_here_list_1.push_back(pts_3d_here_1);
    pts_3d_here_list_2.push_back(pts_3d_here_2);
    pts_3d_here_list_sub.push_back(pts_3d_here_sub);
  }

  /*

  vcl_ofstream vrml3("test3.wrl");
  //vcl_ofstream vrml2("test2.wrl");
  world_color_flag_=true;

  float m=pts_3d_here_list_1.size();
  for (unsigned i=0;i<  pts_3d_here_list_1.size();i++) {
  r_world_=1-i/m;g_world_=.5;b_world_=i/m;

  write_vrml_2(vrml3,pts_3d_here_list_1[i]);
  }

  world_color_flag_=false;
  vrml3.close();


  for (unsigned k=0;k<pts_3d_here_list_1.size();k++) 
  {
  vcl_cout<<k<<"----pt1 asas---"<<k<<vcl_endl;
  for (unsigned i=0;i<  pts_3d_here_list_1[k].size();i++) {
  vcl_cout<<pts_3d_here_list_1[k][i].x()<<" "<<pts_3d_here_list_2[k][i].x()<<" "<<(pts_3d_here_list_1[k][i].z()+pts_3d_here_list_2[k][i].z())/2.0<<vcl_endl;
  }
  vcl_cout<<"----pt1 asasend---"<<vcl_endl;
  }

  vcl_cout<<"----------------------"<<vcl_endl;
  vcl_cout<<"----------------------"<<vcl_endl;
  vcl_cout<<"----------------------"<<vcl_endl;

  for (unsigned k=0;k<pts_3d_here_list_sub.size();k++) 
  {
  vcl_cout<<k<<"----asas---"<<k<<vcl_endl;
  for (unsigned i=0;i<  pts_3d_here_list_sub[k].size();i++) {
  vcl_cout<<pts_3d_here_list_sub[k][i].x()<<" "<<pts_3d_here_list_sub[k][i].y()<<" "<<pts_3d_here_list_sub[k][i].z()<<vcl_endl;
  }
  vcl_cout<<"----asasend---"<<vcl_endl;
  }
  */

  return 1.0;//aaaa

}






float dbcri_whole_process::solve_pickup()
{ ////angle goes clockwise..


  double z_ave_1=0;
  double z_ave_2=0;

  vcl_cout<<"----------------------------------------"<<vcl_endl;
  vcl_cout<<"---solve_pickup-"<<vcl_endl;
  vcl_cout<<"----------------------------------------"<<vcl_endl;


  //dcl_edge_.push_back(dclist);    <-- contour distance edges

  int counter1=0;int counter2=0;

  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_1; 
  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_2; 
  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_1_before_rot; 
  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_2_before_rot; 
  vcl_vector <vcl_vector <vgl_point_3d<double> > >pts_3d_here_list_sub; 
  vcl_vector <vgl_point_3d<double> > pts_3d_here_1;
  vcl_vector <vgl_point_3d<double> > pts_3d_here_2;
  vcl_vector <vgl_point_3d<double> > pts_3d_here_1_before_rot;
  vcl_vector <vgl_point_3d<double> > pts_3d_here_2_before_rot;
  vcl_vector <vgl_point_3d<double> > pts_3d_here_sub;

  int s=0;
  double cabin_length_x_average=0;
  double cabin_width_y_average=0;
  double cabin_height_z_average=0;
  double z1=0,z2=0,y1=0,y2=0;
  double x1=0;
  double x2=0;
  for (unsigned con_i=0;con_i<veh_cons_.size();con_i++) //front...so max x...
  {

    vnl_double_3x4 A1=cameras_[con_i];

    A1[0][1]*=mean_y_1_; ///mean_y_1_ is front
    A1[1][1]*=mean_y_1_;
    A1[2][1]*=mean_y_1_;

    vnl_double_3x4 A2=cameras_[con_i];

    A2[0][1]*=mean_y_2_;
    A2[1][1]*=mean_y_2_;
    A2[2][1]*=mean_y_2_;

    pts_3d_here_1.clear(); pts_3d_here_1_before_rot.clear();
    pts_3d_here_2.clear(); pts_3d_here_2_before_rot.clear();
    pts_3d_here_sub.clear();
    int N=veh_cons_[con_i][0].size();

    double wedge=-theta_neg_-(-theta_pos_);

    double cabin_length_x=0;
    double cabin_width_y=0;
    double cabin_height_z=0;

    vnl_double_3 cabin_1_front(0,0,0);
    vnl_double_3 cabin_2_rear(0,0,0);
    int count_cabin=0;

    for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=ang_ /*was .002*/) {
      double x_1=2000.0;
      double y_1=epi_y_+vcl_tan(ang)*(x_1-epi_x_);
      //      double cont2[] = { epi_x_,epi_y_, x_2,y_2 ,x_2,y_2+2000, epi_x_,epi_y_+2000};
      //      vgl_polygon<double> poly2(cont2, 4);
      double cont1[] = { epi_x_,epi_y_, x_1,y_1 , x_1+.0000001,y_1+.0000001 ,epi_x_+.0000001,epi_y_+.0000001};
      vgl_polygon<double> poly1(cont1, 4);

      vgl_polygon<double> result = vgl_clip( veh_cons_[con_i], poly1, vgl_clip_type_intersect );


      vcl_cout<<"result: "<<result;//<<vcl_endl;
      if (result.num_sheets()!=1||result.num_vertices() == 0) {
        vcl_cout<<"-----"<<vcl_endl;
        continue;
      }
      if (result[0].size()!=4) {
        vcl_cout<<"result polygon size wrong !!!"<<vcl_endl;
        vcl_cout<<"-_-_-_-_-"<<vcl_endl;
        continue;
      }
      int im=-1;
      double xm=10000000;
      //first, find minimum x 
      for (unsigned i=0;i<4;i++)
      {
        if (xm>result[0][i].x()) { xm=result[0][i].x();im=i;}
      }
      //second, find max x 
      double xmx=-10000000;
      int imx=-1;
      for (unsigned i=0;i<4;i++)
      {
        if (xmx<result[0][i].x()) { xmx=result[0][i].x();imx=i;}
      }

      double x_im=result[0][imx].x();
      double y_im=result[0][imx].y();
      //double x_im=veh_cons_[con_i][s][p].x();
      //double y_im=veh_cons_[con_i][s][p].y();

      double C1=(A1[0][1]-A1[2][1]*x_im +A1[0][3]-A1[2][3]*x_im)/(A1[2][0]*x_im-A1[0][0]); 
      double C2=(A1[0][2]-A1[2][2]*x_im)/(A1[2][0]*x_im-A1[0][0]);

      double D1=(A1[1][1]-A1[2][1]*y_im +A1[1][3]-A1[2][3]*y_im)/(A1[2][0]*y_im-A1[1][0]); 
      double D2=(A1[1][2]-A1[2][2]*y_im)/(A1[2][0]*y_im-A1[1][0]);
      double Z=-(C1-D1)/(C2-D2);
      double X=C1+C2*Z;
      double Y=mean_y_1_;
      vcl_cout<<con_i<< "1 "<<ang<<": X Y Z= "<<X<<" "<<Y<<" "<<Z<<vcl_endl;
      z_ave_1+=Z;counter1++;

      vgl_point_3d <double> pt1_before_rot(X,Y,Z);


      //rotation==> real length  and make it shorter..
      vnl_double_3 p10(X,Y,Z);
      vnl_double_3 p11=rot_*p10;




      vgl_point_3d <double> pt1(p11[0],p11[1],p11[2]);

      vcl_cout<<"   "<<pt1<<vcl_endl;




      x_im=result[0][im].x();  // min x necessary
      y_im=result[0][im].y(); //min 
      C1=(A2[0][1]-A2[2][1]*x_im +A2[0][3]-A2[2][3]*x_im)/(A2[2][0]*x_im-A2[0][0]); 
      C2=(A2[0][2]-A2[2][2]*x_im)/(A2[2][0]*x_im-A2[0][0]);

      D1=(A2[1][1]-A2[2][1]*y_im +A2[1][3]-A2[2][3]*y_im)/(A2[2][0]*y_im-A2[1][0]); 
      D2=(A2[1][2]-A2[2][2]*y_im)/(A2[2][0]*y_im-A2[1][0]);
      Z=-(C1-D1)/(C2-D2);
      X=C1+C2*Z;
      Y=mean_y_2_;
      vcl_cout<<con_i<< "2 "<<ang<<": X Y Z= "<<X<<" "<<Y<<" "<<Z<<vcl_endl;
      vgl_point_3d <double> pt2_before_rot(X,Y,Z);

      //rotation==> real length  and make it shorter..
      vnl_double_3 p20(X,Y,Z);
      vnl_double_3 p21=rot_*p20;

      z_ave_2+=Z;counter2++;
      vgl_point_3d <double> pt2(p21[0],p21[1],p21[2]);

      vcl_cout<<"   "<<pt2<<vcl_endl;


      vgl_point_3d <double> pt_sub(pt1.x()-pt2.x(),pt1.y()-pt2.y(),(pt1.z()+pt2.z())/2.0);
      pts_3d_here_sub.push_back(pt_sub);
      pts_3d_here_1.push_back(pt1);
      pts_3d_here_2.push_back(pt2);
      pts_3d_here_1_before_rot.push_back(pt1_before_rot);
      pts_3d_here_2_before_rot.push_back(pt2_before_rot);

      //if (ang>-theta_pos_+.05*wedge && ang<-theta_pos_+.2*wedge)////////?????????????????/////good number????
      if (ang>-theta_pos_+.1*wedge && ang<-theta_pos_+.25*wedge)////////?????????????????/////good number????
      {
        vcl_cout<<"<--** top .05-.20 wedge changed to .1-.25"<<vcl_endl;
        cabin_length_x+=pt1.x()-pt2.x();

        cabin_1_front(0)+=pt1.x();
        cabin_2_rear(0)+=pt2.x();
        count_cabin++;

        // since not addition, last(lowest z) will be stored at final.
        cabin_1_front(1)=pt1.y();
        cabin_2_rear(1)=pt2.y();
        cabin_1_front(2)=pt1.z();
        cabin_2_rear(2)=pt2.z();

        cabin_width_y=vcl_abs(pt1.y()-pt2.y());
        cabin_height_z=(pt1.z()+pt2.z())/2.0;

      }
      vcl_cout<<"----------------------------------------"<<vcl_endl;


    }

    if (count_cabin==0) continue;//7-1-2007 to avoid 0 case
    cabin_length_x_average+=cabin_length_x/( count_cabin*veh_cons_.size() );
    cabin_width_y_average+=cabin_width_y/( veh_cons_.size() );
    cabin_height_z_average+=cabin_height_z/( veh_cons_.size() );

    z1+=cabin_1_front(2)/( veh_cons_.size() );
    z2+=cabin_2_rear(2)/( veh_cons_.size() );
    y1+=cabin_1_front(1)/( veh_cons_.size() );
    y2+=cabin_2_rear(1)/( veh_cons_.size() );


    x1+=cabin_1_front(0)/( count_cabin*veh_cons_.size() );
    x2+=cabin_2_rear(0)/( count_cabin*veh_cons_.size() );

    vcl_cout<<con_i<<"'th cabin length "<< cabin_length_x/count_cabin <<" "<<count_cabin<<vcl_endl;
    vcl_cout<<con_i<<"'th cabin  front "<< cabin_1_front(0)/count_cabin<<vcl_endl;
    vcl_cout<<con_i<<"'th cabin  rear  "<< cabin_2_rear(0)/count_cabin<<vcl_endl;

    pts_3d_here_list_1.push_back(pts_3d_here_1);
    pts_3d_here_list_2.push_back(pts_3d_here_2);
    pts_3d_here_list_1_before_rot.push_back(pts_3d_here_1_before_rot);
    pts_3d_here_list_2_before_rot.push_back(pts_3d_here_2_before_rot);
    pts_3d_here_list_sub.push_back(pts_3d_here_sub);
  }

  p2_[0]=x2;p2_[1]=y2;p2_[2]=z2;
  p8_[0]=x1;p8_[1]=y1;p8_[2]=z1;

  p_cabin_length_[0]=cabin_length_x_average;
  p_cabin_length_[1]=cabin_width_y_average;
  p_cabin_length_[2]=cabin_height_z_average;


  {
    vcl_ofstream vrml1("test1.wrl");
    vcl_ofstream vrml2("test2.wrl");
    world_color_flag_=true;

    float m=pts_3d_here_list_1.size();
    for (unsigned i=0;i<  pts_3d_here_list_1.size();i++) {
      r_world_=1-i/m;g_world_=.5;b_world_=i/m;

      write_vrml_2(vrml1,pts_3d_here_list_1[i]);
    }
    for (unsigned i=0;i<  pts_3d_here_list_2.size();i++) {
      r_world_=1-i/m;g_world_=.5;b_world_=i/m;

      write_vrml_2(vrml2,pts_3d_here_list_2[i]);
    }
    world_color_flag_=false;
    vrml1.close();
    vrml2.close();
  }

  //before ratation
  {
    vcl_ofstream vrml1("test1_before_rot.wrl");
    vcl_ofstream vrml2("test2_before_rot.wrl");
    world_color_flag_=true;

    float m=pts_3d_here_list_1.size();
    for (unsigned i=0;i<  pts_3d_here_list_1_before_rot.size();i++) {
      r_world_=1-i/m;g_world_=.5;b_world_=i/m;

      write_vrml_2(vrml1,pts_3d_here_list_1_before_rot[i]);
    }
    for (unsigned i=0;i<  pts_3d_here_list_2_before_rot.size();i++) {
      r_world_=1-i/m;g_world_=.5;b_world_=i/m;

      write_vrml_2(vrml2,pts_3d_here_list_2_before_rot[i]);
    }
    world_color_flag_=false;
    vrml1.close();
    vrml2.close();
  }
  vcl_ofstream vrml3("test3.wrl");
  //vcl_ofstream vrml2("test2.wrl");
  world_color_flag_=true;

  float m=pts_3d_here_list_sub.size();
  for (unsigned i=0;i<  pts_3d_here_list_sub.size();i++) {
    r_world_=1-i/m;g_world_=.5;b_world_=i/m;

    write_vrml_2(vrml3,pts_3d_here_list_sub[i]);
  }
  /*for (unsigned i=0;i<  pts_3d_here_list_2.size();i++) {
  r_world_=1-i/m;g_world_=.5;b_world_=i/m;

  write_vrml_2(vrml2,pts_3d_here_list_2[i]);
  }*/
  world_color_flag_=false;
  vrml3.close();


  for (unsigned k=0;k<pts_3d_here_list_1.size();k++) 
  {
    vcl_cout<<k<<"----pt1 asas---"<<k<<vcl_endl;
    for (unsigned i=0;i<  pts_3d_here_list_1[k].size();i++) {
      vcl_cout<<pts_3d_here_list_1[k][i].x()<<" "<<pts_3d_here_list_2[k][i].x()<<" "<<(pts_3d_here_list_1[k][i].z()+pts_3d_here_list_2[k][i].z())/2.0<<vcl_endl;
    }
    vcl_cout<<"----pt1 asasend---"<<vcl_endl;
  }

  vcl_cout<<"----------------------"<<vcl_endl;
  vcl_cout<<"----------------------"<<vcl_endl;
  vcl_cout<<"----------------------"<<vcl_endl;

  for (unsigned k=0;k<pts_3d_here_list_sub.size();k++) 
  {
    vcl_cout<<k<<"----asas---"<<k<<vcl_endl;
    for (unsigned i=0;i<  pts_3d_here_list_sub[k].size();i++) {
      vcl_cout<<pts_3d_here_list_sub[k][i].x()<<" "<<pts_3d_here_list_sub[k][i].y()<<" "<<pts_3d_here_list_sub[k][i].z()<<vcl_endl;
    }
    vcl_cout<<"----asasend---"<<vcl_endl;
  }


  return 1.0;//aaaa

}



//not used yet 6-22-07
void dbcri_whole_process::spacial_filtering_x_with_z(double &r1,double &r2, vcl_vector <vgl_point_3d <double> > pts) {

  vnl_double_3 min_point(min_point_);
  vnl_double_3 max_point(max_point_);

  vnl_double_3 diag_vector = max_point - min_point;
  vnl_vector_fixed<double,3> x_axis(0.0), y_axis(0.0), z_axis(0.0);
  x_axis[0] = diag_vector[0];
  y_axis[1] = diag_vector[1];
  z_axis[2] = diag_vector[2];

  double max_y=max_point(1);
  double min_y=min_point(1);
  // vcl_cout<<min_point<<vcl_endl;
  double max_z0,min_z0;

  double max_x=max_point(0);
  double min_x=min_point(0);
  vcl_cout<<"min_point:"<<min_point<<vcl_endl;


  max_y=-vcl_numeric_limits<double>::infinity();

  min_y= vcl_numeric_limits<double>::infinity();
  max_x=-vcl_numeric_limits<double>::infinity();

  min_x= vcl_numeric_limits<double>::infinity();

  vcl_cout<<"spacial filtering x pts.size: "<<pts.size()<<vcl_endl;
  //normalization of y
  for (unsigned i=0;i<pts.size();i++) {
    //pts[i].set(pts[i].x(),(pts[i].y()-min_y)*(max_y-min_y),pts[i].z());
    if (pts[i].y()>max_y) max_y=pts[i].y();
    if (pts[i].y()<min_y) min_y=pts[i].y();

    if (pts[i].z()>max_z0) max_z0=pts[i].z();
    if (pts[i].z()<min_z0) min_z0=pts[i].z();
    //  vcl_cout<<pts[i]<<vcl_endl;
  }


  /*  for (unsigned i=0;i<point_3d_list_f.size();i++) {
  vnl_double_3 p(point_3d_list_f[i].x(),point_3d_list_f[i].y(),point_3d_list_f[i].z());

  pts_z.push_back(rot*p);
  vcl_cout<<rot*p<<vcl_endl;
  }*/


  vcl_vector<vnl_double_3> pts_x;
  vcl_vector<vnl_double_3> pts_temp;
  vcl_vector<vnl_double_3> pts_z;
  vcl_vector<vnl_double_3> pts_y;
  for (unsigned i=0;i<pts.size();i++) {
    vnl_double_3 p0(pts[i].x(),pts[i].y(),pts[i].z());
    vnl_double_3 p1=rot_*p0;

    //vnl_double_3 p(p1[0],(p1[1]-min_y)/(max_y-min_y),p1[2]);
    vnl_double_3 p(p1[0],p1[1],p1[2]);
    pts_temp.push_back(p);

  }


  //find max x and min x after rotation...
  for (unsigned i=0;i<pts_temp.size();i++) {
    //pts[i].set(pts[i].x(),(pts[i].y()-min_y)*(max_y-min_y),pts[i].z());
    if (pts_temp[i][0]>max_x) max_x=pts_temp[i][0];
    if (pts_temp[i][0]<min_x) min_x=pts_temp[i][0];

    //vcl_cout<<pts[i]<<vcl_endl;
  }


  //x scaling
  for (unsigned i=0;i<pts_temp.size();i++) {
    vnl_double_3 p((pts_temp[i][0]-min_x)/(max_x-min_x),pts_temp[i][1],pts_temp[i][2]);


    //vnl_double_3 p(p1[0],(p1[1]-min_y)/(max_y-min_y),p1[2]);
    //vnl_double_3 p(p1[0],p1[0],p1[2]);
    pts_x.push_back(p);

  }

  //vcl_sort(pts_z.begin(), pts_z.end(), dbcri_accu_edge_cmp_z);
  //vcl_vector<vnl_double_3> pts_x;

  // only consider points in x and y above 0.5 feet  //i don't use it..
  for ( vcl_vector<vnl_double_3>::const_iterator itr = pts_z.begin();
    itr != pts_z.end();  ++itr)
  {
    //if ((*itr)[2] > 0.5) { pts_x.push_back(*itr); }
    pts_y.push_back(*itr); 
  }

  //vcl_vector<vnl_double_3> pts_y = pts_x;

  // vcl_sort(pts_x.begin(), pts_x.end(), dbcri_accu_edge_cmp_x);
  vcl_sort(pts_x.begin(), pts_x.end(), dbcri_accu_edge_cmp_x);



  //#mothod 1... find out lier
  vcl_vector<int> bin_1(BIN_SIZE_,0);
  for (unsigned i=0;i<vcl_floor(pts_x.size()/5.0); i++) {// 6-22-07 if there is outlier, problem!
    // i,e. if x={-100, 0, 1,1, 2, 3, 4, 5, 6,6,6,7.1,10,11}
    // x=-100 makes problem.... since it search only 1/5 portion..
    double r=(pts_x[i])[0];
    if (vcl_floor(BIN_SIZE_*r)>=0&&vcl_floor(BIN_SIZE_*r)<=99)
    {
      if ( (int) vcl_floor(BIN_SIZE_*r-1)>=0)
        bin_1[(int) vcl_floor(BIN_SIZE_*r)-1]++;

      bin_1[(int) vcl_floor(BIN_SIZE_*r)]++;bin_1[(int) vcl_floor(BIN_SIZE_*r)]++;
      if ((int) vcl_floor(BIN_SIZE_*r+1)<=99)
        bin_1[(int) vcl_floor(BIN_SIZE_*r)+1]++;
    }
  }

  vcl_vector<int> bin_2(BIN_SIZE_,0);
  for (unsigned i=vcl_floor(pts_x.size()*4/5.0);i<pts_x.size(); i++) {
    double r=(pts_x[i])[0];
    if (vcl_floor(BIN_SIZE_*r)>=0&&vcl_floor(BIN_SIZE_*r)<=99)
    {
      if ((int) vcl_floor(BIN_SIZE_*r-1)>=0)
        bin_2[(int) vcl_floor(BIN_SIZE_*r)-1]++;

      bin_2[(int) vcl_floor(BIN_SIZE_*r)]++;bin_2[(int) vcl_floor(BIN_SIZE_*r)]++;
      if ((int) vcl_floor(BIN_SIZE_*r+1)<=99)
        bin_2[(int) vcl_floor(BIN_SIZE_*r)+1]++;
    }

  }


  // find max bin
  int max_bin_1=-100;
  int max_bin_id_1=-100;
  int max_bin_2=-100;
  int max_bin_id_2=-100;
  double mean_r=0.0;
  int counter=0; 

  //NOTICE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //***************// BIN_SIZE_/2 avoid driver side ...**********
  for (unsigned i=0;i<(int)(BIN_SIZE_/2.0);i++) {

    vcl_cout<<bin_1[i]<<vcl_endl;
    if (bin_1[i]>max_bin_1) {
      max_bin_1=bin_1[i];
      max_bin_id_1=i;
    }
  }


  vcl_cout<<"====front and top ==================="<<vcl_endl;
  //NOTICE !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
  //***************// BIN_SIZE_/2 avoid rear side ...**********
  for (unsigned i=(int)(BIN_SIZE_/2.0);i<BIN_SIZE_;i++) {

    vcl_cout<<bin_2[i]<<vcl_endl;
    if (bin_2[i]>max_bin_2) {
      max_bin_2=bin_2[i];
      max_bin_id_2=i;
    }
  }


  //vcl_cout<<mean_r/(double)counter<<vcl_endl;

  // check !!! fix later..// min_point and min_y is different
  r1=max_bin_id_1*(max_x-min_x)/100.0+min_x-min_point(0);// min_point and min_y is different
  r2=max_bin_id_2*(max_x-min_x)/100.0+min_x-min_point(0);// min_point and min_y is different
  // // min_point and min_y is different

  //display_0_frame_=false; // display sample needed for 0 frame once..


  double m_x_1=0.0,m_y_1=0.0,m_z_1=0.0;
  double m_x_2=0.0,m_y_2=0.0,m_z_2=0.0;
  double count1=0,count2=0;

  double max_z=-100.0;
  for (unsigned i=0;i<pts_x.size();i++) {




    double r=(pts_x[i])[0];
    if ((int) vcl_floor(BIN_SIZE_*r)<max_bin_id_1+1&&(int) vcl_floor(BIN_SIZE_*r)>max_bin_id_1-1)
    {

      double temp_x=(max_x-min_x)*r+min_x;
      m_x_1+=temp_x;
      m_y_1+=pts_x[i][1];
      m_z_1+=pts_x[i][2];


      count1++;
    }


    if ((int) vcl_floor(BIN_SIZE_*r)<max_bin_id_2+1&&(int) vcl_floor(BIN_SIZE_*r)>max_bin_id_2-1)
    {

      double temp_x=(max_x-min_x)*r+min_x;
      m_x_2+=temp_x;
      m_y_2+=pts_x[i][1];
      m_z_2+=pts_x[i][2];
      if (max_z<pts_x[i][2]) max_z=pts_x[i][2];
      count2++;
    }

  }
  vnl_double_3x3 inv_rot = inv_rot_;

  vnl_double_3 pm1_temp(m_x_1/count1,m_y_1/count1,m_z_1/count1);

  vnl_double_3 pm1=inv_rot*pm1_temp;


  //vnl_double_3 pm2_temp(m_x_2/count2,m_y_2/count2,m_z_2/count2);
  vnl_double_3 pm2_temp(m_x_2/count2,m_y_2/count2,max_z); //use maximum z for probe 7'th z location

  vnl_double_3 pm2=inv_rot*pm2_temp;


  vcl_cout<<"front & rear points:"<<pm1<<" "<<pm2<<vcl_endl;
  vcl_cout<<"max z- probe 7'th z:"<<max_z-pm2(2)<<vcl_endl;




  p7_=pm1;// actually, this is probe 0
  p0_=pm2;// actually, this is probe 7


  //////////////////////////////////////////

  //



  return;




}





