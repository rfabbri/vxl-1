// This is pro/dbshadow_process.cxx

//:
// \file
#define DEBUG 0
#define WORLD_CAM true
#define FRAME_OFFSET 2;
//#define ERROR 1000

///  if all pts across frames are good use it or not (in cross ratio sense)
#define ANGLE_FLAG false 
///
#include "dbshadow_process.h"
//#include "dbecl_episeg_storage.h"
#include <bpro/bpro_parameters.h>


#include <vidpro/process/vidpro_VD_edge_process.h>

#include <vul/vul_timer.h>
#include <vul/vul_get_timestamp.h>


#include <bpro/bpro_parameters.h>

#include <vidpro/storage/vidpro_image_storage.h>
#include <vidpro/storage/vidpro_image_storage_sptr.h>
#include <vidpro/storage/vidpro_vtol_storage.h>
#include <vidpro/storage/vidpro_vtol_storage_sptr.h>


#include <vidpro/storage/vidpro_vtol_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage_sptr.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
#include <vidpro/storage/vidpro_image_storage.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_convert.h>
#include <vil/vil_new.h>
#include <vil/algo/vil_gauss_filter.h>


#include <vil/vil_convert.h>

//required because sdet still uses vil1
#include <vil1/vil1_vil.h>
#include <vil1/vil1_memory_image_of.h>


#include <vtol/vtol_edge.h>
#include <vtol/vtol_edge_2d.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vdgl/vdgl_interpolator.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vsol/vsol_point_2d.h>

#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_spatial_object_2d_sptr.h> 
#include <vsol/vsol_polygon_2d.h>
#include <brip/brip_vil_float_ops.h>

#include <dbecl/dbecl_episeg_from_curve_converter.h>
#include <vsol/vsol_digital_curve_2d.h>

//#include <vgl/algo/vgl_fit_lines_2d.h> //line segment
#include <vgl/algo/vgl_line_2d_regression.h>
#include <vgl/algo/vgl_convex_hull_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>

#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>


#include <dbecl/dbecl_episeg.h>
#include <dbecl/dbecl_episeg_point.h>
#include <dbecl/dbecl_episeg_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_inverse.h>
#include <vnl/vnl_double_2.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_double_4.h>
#include <vnl/vnl_double_3x4.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_rotation_matrix.h>
#include <vnl/vnl_identity_3x3.h>
#include <vnl/algo/vnl_svd.h>

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_algorithm.h>
#include <vcl_utility.h>
#include <vcl_limits.h>

#include <vcl_cstring.h>
#include <vcl_string.h>
//#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <vul/vul_file_iterator.h>
#include <vul/vul_reg_exp.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_point_2d.h>



#include <bmvl/brct/brct_algos.h>


#include <vnl/vnl_math.h> // for vnl_math_isnan()


#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_convex_hull_2d.h>


#include <bsol/bsol_algs.h>
#include <sdet/sdet_detector.h>
#include <sdet/sdet_detector_params.h>



bool cmp_grey(const vnl_vector_fixed <double, 6 > & rhs, const vnl_vector_fixed <double, 6 >& lhs)
{
  return rhs[2] < lhs[2];
}


//: Constructor
dbshadow_process::dbshadow_process()
: bpro_process()
{

  if(
    //!parameters()->add( "world camera <filename...>" , "-cam_filename", bpro_filepath("","*") )||
    //!parameters()->add( "use epipole from world camera?", "-epi_flag",  (bool) false)||
    //!parameters()->add( "manual epipole?", "-man_epi_flag",  (bool) false)||
    //!parameters()->add( "epi_x?", "-epi_x",  (float) 0.0f) ||
    //!parameters()->add( "epi_y?", "-epi_y",  (float) 0.0f) ||
    //!parameters()->add( "epi_search_range_x", "-e_range_x",  (float) 200.0f) ||
    //!parameters()->add( "epi_search_range_y", "-e_range_y",  (float) 50.0f) ||
    //!parameters()->add( "epi_search_interval", "-epi_search_interval",  (float) 50.0f) ||

    //!parameters()->add( "3d rec con only 0/near edge 2/both 1/all edge 3(con+near+messy)/?", "-recon_flag",  (int) 0)||
    //!parameters()->add( "draw_only?", "-draw_flag",  (bool) false)||
    //!parameters()->add( "use weight?", "-weight_flag",  (bool) true)||
    //!parameters()->add( "use clustering?", "-cluster_flag",  (bool) true)||
    //!parameters()->add( "histogram normalization?", "-histo_normal_flag",  (bool) false)||
    //!parameters()->add( "wedge search margin?", "-MARGIN",  0.0f)||
    /*!parameters()->add( "random number threshold?", "-rand_th",  0.2f)||
    !parameters()->add( "frame Number?", "-N",  (int) 7)||
    !parameters()->add( "combination number?", "-Comb",  (int)15)||
    !parameters()->add( "bin minimum?", "-bin_min",  (int)5)||*/


    //!parameters()->add( "draw_only?", "-draw_flag",  (bool) false)||
    //!parameters()->add( "use default world camera?", "-cam_flag",  (bool) false) ||
    //!parameters()->add( "contour neighbor only?", "-edge_distance",  (bool) true) ||
    //!parameters()->add( "contour distance?", "-edge_distance2",  (float) 5.0f) ||
    //!parameters()->add( "convex hull shift?", "-auto_shift",  (bool) true) ||
    //!parameters()->add( "3d recon error tolerance", "-d3_error",  (float) 10.0f)  ||
    !parameters()->add( "edge detection?", "-sedge_detection",(bool) true) ||
    !parameters()->add( "grey-level threshold?", "-sgrey_th",(float) .1f) ||
    !parameters()->add( "draw bottom line?", "-sdraw_bline",(bool) true)

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
dbshadow_process::
dbshadow_process(const dbshadow_process& other)
: bpro_process(other)
{
}


//: Destructor
dbshadow_process::~dbshadow_process()
{
}


//: Clone the process
bpro_process* 
dbshadow_process::clone() const
{
  return new dbshadow_process(*this);
}


//: Return the name of the process
vcl_string
dbshadow_process::name()
{
  return "dbshadow";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbshadow_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;

  to_return.push_back( "vsol2D" );
  to_return.push_back( "image" );
  to_return.push_back( "vtol2D" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbshadow_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;

  to_return.push_back( "vtol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vtol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );

  to_return.push_back( "vtol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );

  to_return.push_back( "vtol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );



  return to_return;
}


//: Returns the number of input frames to this process
int
dbshadow_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbshadow_process::output_frames()
{
  //return nframes_;
  return 1;
}




//: Run the process on the current frame
bool
dbshadow_process::execute()
{


  vcl_cout<<  input_data_.size() <<vcl_endl;
  if ( input_data_.size() != 1 ){
    vcl_cout<<  input_data_.size() <<vcl_endl;
    vcl_cout << "In dbshadow_process::execute() - "
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

  bool edge=false;
  parameters()->get_value( "-sedge_detection" ,  edge);
  if (!edge) return true;
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
dbshadow_process::finish()
{

  if (!initialize_camera_N_string_scan()) {
    vcl_cout<<"bbox_cam_my file problem"<<vcl_endl;
    return false;
  }
  map3d_2d();//need for count_edge


  //probe_plane();
  vnl_double_4 s0, e0;
  //probe_bottom(s0,e0);
  // probe_front_plane(s0,e0);

  shadow_histo();
  for (double dz=-0;dz<=0;dz+=.2) {
    vnl_double_4 p0(-7.20569, 12.9386, 0.1+dz,1);
    vnl_double_4 p7(14.9867, 14.819, 0.1+dz,1);//a3820

    vnl_double_4 p0_A3811( 4.35624, 5.23431, 0.1+dz,1);
    vnl_double_4 p7_A3811(20.5832, 9.06676, 0.1+dz,1);

      vnl_double_4 p0_4477( 5.80811, 4.46354, .5+dz,1);
    vnl_double_4 p7_4477(19.8571, 8.97397, .5+dz,1);

    vcl_cout<<"z: "<<0.1+dz<<vcl_endl;
    shadow_histo(p0_4477,  p7_4477);
  }

 return true;

  if (0) {
    for (double dx=0;dx<=0;dx+=.3) {
      vnl_double_4 s(9.2243+dx,15.3703,2.97796,1);
      vnl_double_4 e(9.2243+dx,11.3703,2.97796,1);
      double ans=count_edge( s,  e);
      vcl_cout<<s<<" "<<ans<<vcl_endl;
    }

    vcl_cout<<vcl_endl;
    for (double dx=0;dx<=0;dx+=.3) {
      vnl_double_4 s(9.2243+dx,15.3703,2.97796,1);
      vnl_double_4 e(6.76132+dx,11.232,2.66335,1);
      double ans=count_edge( s,  e);
      vcl_cout<<s<<" "<<ans<<vcl_endl;
    }
  }
  // 9.2243,15.3703,2.97796

  //    6.76132,11.232,2.66335
  return true;

}

bool dbshadow_process::initialize_camera_N_string_scan()
{ 

  bpro_filepath cam_path;
  bool default_cam_flag;
  parameters()->get_value( "-cam_filename" , cam_path );
  parameters()->get_value( "-cam_flag" , default_cam_flag );
  //    vcl_cout<<vul_file::dirname(cam_path);
  vcl_string cam_filename = cam_path.path;
  //first, check the con file names to find correct frmae # to use.
  vul_reg_exp r0("00");

  vul_file_iterator fn_con=vul_file::dirname(cam_path.path)+"/*.con";

  int cam_num;
  vcl_vector <int> cam_num_list;
  for ( ; fn_con; ++fn_con) 
  {
    vcl_string input_file = fn_con();

    if (r0.find(input_file.c_str()))
    {
      //vul_string_c_trim(cstr,">");
      vcl_string a=vul_file::strip_extension(vul_file::strip_directory(input_file));
      vcl_cout<<atoi(a.c_str())<<vcl_endl;
      cam_num_list.push_back(atoi(a.c_str()));
      //vnl_double_
    }
  }


  vul_reg_exp r1("bbox_cam_my.txt");
  vul_reg_exp r11("cam");
  vul_reg_exp r2("txt");

  vul_file_iterator fn=vul_file::dirname(cam_path.path)+"/*.txt";


  bool bbox_cam_my_file=false;
  for ( ; fn; ++fn) 
  {
    vcl_string input_file = fn();

    if (r1.find(input_file.c_str())&&r2.find(input_file.c_str())||r11.find(input_file.c_str())&&r2.find(input_file.c_str()) )
    {
      bbox_cam_my_file=true;
      vcl_ifstream fp(input_file.c_str());
      if (!fp) {
        vcl_cout<<" Unable to Open "<< cam_filename <<vcl_endl;
        return false;
      }
      vnl_double_3x4 C;
      double cam_element;
      double temp=0;
      vnl_double_3x4 Camera(0.0);
      vnl_double_3x4 Camera_Null(-1.0);
      Cam_List_.clear();
      vcl_cout<<"reading camera file "<< input_file.c_str()<<vcl_endl;
      vcl_string hhh="";
      while (hhh!="Transform") {
        fp>> hhh;
        // vcl_cout<< hhh <<vcl_endl;
      }
      vnl_double_4x4 RT(0.0);
      for (unsigned i=0;i<4;i++) {
        for (unsigned j=0;j<4;j++) {
          fp >> temp;
          RT[i][j]=temp;
        }
      }
      vcl_cout<<RT<<vcl_endl;
      BB_RT_matt_=RT;
      fp>>hhh;
      int count=0; // for BRMF 2.7
      int empty_camera_number=0;       // for BRMF 2.7
      while (hhh=="Camera") {
        Camera.fill(0.0);
        fp>>hhh; cam_num=atoi(hhh.c_str());
        if (count==0) {                 // for BRMF 2.7
          char a[10];                   // for BRMF 2.7
          //vcl_sprintf(a,"%s",hhh);    // for BRMF 2.7
          empty_camera_number= atoi(hhh.c_str());// for BRMF 2.7
        }                               // for BRMF 2.7
        count++;                        // for BRMF 2.7

        vcl_cout<<hhh<<vcl_endl;
        for (unsigned j=0;j<3; j++) {
          for (unsigned k=0;k<4;k++) {
            fp >> temp;
            Camera[j][k]=temp;
          }
        }

        for (unsigned i=0;i<cam_num_list.size();i++)
          if (cam_num_list[i]==cam_num) {
            Cam_List_.push_back(Camera);
            vcl_cout<<Camera<<vcl_endl;
          }
          fp>>hhh;
      }
    }
  }
  return bbox_cam_my_file;
}



void dbshadow_process::map3d_2d()
{


  int frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

    vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();

    vcl_vector< vsol_spatial_object_2d_sptr > sh_pts;
    vidpro_vtol_storage_sptr frame_vtol;
    ////frame_image.vertical_cast(input_data_[frame][0]);
    vcl_cout<<output_data_.size()<<vcl_endl;
    vcl_cout<<output_data_[frame].size()<<vcl_endl;
    vcl_cout<<input_data_[frame][0]<<vcl_endl;
    frame_vtol.vertical_cast(input_data_[frame][2]);// 1 is ocntour?
    vcl_cout<<frame_vtol<<vcl_endl;
    vcl_vector <vsol_digital_curve_2d_sptr>  dclist;
    vcl_vector<vtol_edge_2d_sptr> edges;



    //     for ( vcl_vector<vtol_edge_2d_sptr>::iterator e_itr = edges->begin();
    //   e_itr != edges->end();  ++e_itr )

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
              if (ec->size()<4) continue;
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

                bool distance_measure;
                parameters()->get_value( "-edge_distance" , distance_measure );
                float distance_measure2;
                parameters()->get_value( "-edge_distance2" , distance_measure2 );


                dc->add_vertex(new vsol_point_2d(ec->edgel(i).get_pt()));
                // vcl_cout<< (*ec)[i].get_x()<<" "<<(*ec)[i].get_y()<<vcl_endl;

                sh_pts.push_back(new vsol_point_2d(ec->edgel(i).get_pt()));
              }


              if (dc->size()>3)
                dclist.push_back(dc);

            }
          }
        }
      }
      dcl_edge_.push_back(dclist);
      output_vsol->add_objects(sh_pts);
      output_data_[frame].push_back(output_vsol);
  }


  //for image in the polygon


  blob_image_list_.clear();

  frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){


    vcl_vector <vnl_vector_fixed <double, 6 > > blob_image;

    //vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
    vcl_vector< vgl_point_2d <double> > image_pts;
    //vcl_vector< vsol_spatial_object_2d_sptr > sh_pts;
    // get image from the storage class
    vidpro_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[frame][1]);// 1 is image

    vil_image_resource_sptr image_sptr = frame_image->get_image();
    vil_image_view<vxl_byte> grey_view = vil_convert_stretch_range(vxl_byte(),
      vil_convert_to_grey_using_rgb_weighting(image_sptr->get_view()) );
    vil_image_view<vil_rgb<vxl_byte> > color_view = image_sptr->get_view();




    for  (unsigned i=0;i<grey_view.ni();i++)
      for (unsigned j=0;j<grey_view.nj();j++)
      {

        if ( !veh_cons_[frame].contains(i,j) ) continue;
        vnl_vector_fixed <double, 6 >  blob_point(0.0);
        blob_point[0]=i;
        blob_point[1]=j;
        blob_point[2]=(double)grey_view(i,j);
        blob_point[3]=(double)color_view(i,j).r;
        blob_point[4]=(double)color_view(i,j).g;
        blob_point[5]=(double)color_view(i,j).b;

        blob_image.push_back(blob_point);
        //        if ( veh_cons_[frame].contains((*ec)[i].get_x(),(*ec)[i].get_y()) )
        // vnl_double_4(i,j,blob_image
        //         continue;




      }
      blob_image_list_.push_back(blob_image);
  }//frame

  
}

void dbshadow_process::probe_plane()
{


  int frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

    vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
    vcl_vector< vsol_spatial_object_2d_sptr > sh_pts;
    vidpro_vtol_storage_sptr frame_vtol;

    for (double di=-1;di<=1;di+=0.1)
      for (double dj=0;dj<=3;dj+=0.1) {

        vnl_double_4 a0( 16.9871+di, 9.54367, 0.115178+dj, 1);

        vnl_double_3 p0=Cam_List_[input_data_.size()-frame-1]*a0;
        //16.901 9.65016 0.115819

        //14.2597 14.8963 0.81754
        sh_pts.push_back(new vsol_point_2d(p0[0]/p0[2],p0[1]/p0[2]));
      }
      //    sh_pts.push_back(new vsol_point_2d(ec->edgel(i).get_pt()));
      output_vsol->add_objects(sh_pts);
      output_data_[frame].push_back(output_vsol);

  }

}



void dbshadow_process::probe_bottom(vnl_double_4 s0, vnl_double_4 e0)
{




  int frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

    vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
    vcl_vector< vgl_point_2d <double> > image_pts;
    vcl_vector< vsol_spatial_object_2d_sptr > sh_pts;
    // get image from the storage class
    vidpro_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[frame][1]);

    vil_image_resource_sptr image_sptr = frame_image->get_image();
    vil_image_view<vxl_byte> grey_view = vil_convert_stretch_range(vxl_byte(),
      vil_convert_to_grey_using_rgb_weighting(image_sptr->get_view()) );

    //vil_image_view<vil_rgb<vxl_byte> >color_view =vil_convert_stretch_range(vxl_byte(),image_sptr->get_view() );

    vil_image_view<vil_rgb<vxl_byte> > color_view = image_sptr->get_view();

    bool flag=false;//to see first point

    vil_image_view<vil_rgb<vxl_byte> >  img = vil_convert_to_component_order(  image_sptr->get_view());
    for (double dx=-7.2;dx<14.9867; dx+=.1)
      for (double dz=0;dz<.5;dz+=.1)
      {
        vnl_double_4 a0( dx, 12.9386, dz, 1);
        vnl_double_3 p0=Cam_List_[input_data_.size()-frame-1]*a0;
        //16.901 9.65016 0.115819

        //14.2597 14.8963 0.81754
        double v0=p0[0]/p0[2];double w0=p0[1]/p0[2];
        image_pts.push_back( vgl_point_2d<double >(v0,w0));
        // vcl_cout<<(int)(p0[0]/p0[2])<<" "<<(p0[1]/p0[2])<<" "<<color_view((int)(p0[0]/p0[2]),(int)(p0[1]/p0[2]),0)<<vcl_endl;
        // vcl_cout<<(int)(p0[0]/p0[2])<<" "<<(p0[1]/p0[2])<<" "<<(int)img((int)(p0[0]/p0[2]),(int)(p0[1]/p0[2])).r<<vcl_endl;
        vcl_cout<<(int)(p0[0]/p0[2])<<" "<<(p0[1]/p0[2])<<" "
          <<(int)img((int)(p0[0]/p0[2]),(int)(p0[1]/p0[2])).r<<" "
          <<(int)img((int)(p0[0]/p0[2]),(int)(p0[1]/p0[2])).g<<" "
          <<(int)img((int)(p0[0]/p0[2]),(int)(p0[1]/p0[2])).b<<" "
          <<vcl_endl;
        if (flag)
          sh_pts.push_back(new vsol_point_2d(p0[0]/p0[2],p0[1]/p0[2]));
        flag=true;
      }

      vcl_cout<<"=== "<<frame<<vcl_endl;
      vcl_cout<<"=== "<<frame<<vcl_endl;
      vcl_cout<<"=== "<<frame<<vcl_endl;
      vcl_cout<<"=== "<<frame<<vcl_endl;
      vcl_cout<<"=== "<<frame<<vcl_endl;
      vcl_cout<<" "<<vcl_endl;


      if(grey_view.nplanes() != 1) {
        vcl_cerr << "Returning false. nplanes(): " << grey_view.nplanes() << vcl_endl;
        return ;
      }



      output_vsol->add_objects(sh_pts);
      output_data_[frame].push_back(output_vsol);



  }


}



void dbshadow_process::probe_front_plane(vnl_double_4 s0, vnl_double_4 e0)
{


  int frame=0;
  /*for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

  vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();

  output_data_[frame].push_back(output_vsol);
  }*/

  for (double dx=-5; dx<3;dx+=.5) {
    vcl_vector <vcl_vector< double > >image_pts_frames;
    frame=0;
    for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

      vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
      vcl_vector<  double > image_pts;
      vcl_vector< vsol_spatial_object_2d_sptr > sh_pts;
      // get image from the storage class
      vidpro_image_storage_sptr frame_image;
      frame_image.vertical_cast(input_data_[frame][1]);

      vil_image_resource_sptr image_sptr = frame_image->get_image();
      vil_image_view<vxl_byte> grey_view = vil_convert_stretch_range(vxl_byte(),
        vil_convert_to_grey_using_rgb_weighting(image_sptr->get_view()) );

      //vil_image_view<vil_rgb<vxl_byte> >color_view =vil_convert_stretch_range(vxl_byte(),image_sptr->get_view() );

      vil_image_view<vil_rgb<vxl_byte> > color_view = image_sptr->get_view();

      bool flag=false;//to see first point

      vil_image_view<vil_rgb<vxl_byte> >  img = vil_convert_to_component_order(  image_sptr->get_view());


      //for (double dx=-3;dx<2;dx+=.3)
      for (double dy=10.1;dy<14.1; dy+=.1)
        for (double dz=0;dz<2.5;dz+=.1)
        {
          vnl_double_4 a0( 14+dx, dy, dz, 1);
          vnl_double_3 p0=Cam_List_[input_data_.size()-frame-1]*a0;
          //16.901 9.65016 0.115819

          //14.2597 14.8963 0.81754
          double v0=p0[0]/p0[2];double w0=p0[1]/p0[2];
          image_pts.push_back( (double)grey_view(v0,w0));
          // vcl_cout<<(double)grey_view(v0,w0)<<vcl_endl;
          //vcl_cout<<(int)(p0[0]/p0[2])<<" "<<(p0[1]/p0[2])<<" "
          //  <<(int)img((int)(p0[0]/p0[2]),(int)(p0[1]/p0[2])).r<<" "
          //  <<(int)img((int)(p0[0]/p0[2]),(int)(p0[1]/p0[2])).g<<" "
          //  <<(int)img((int)(p0[0]/p0[2]),(int)(p0[1]/p0[2])).b<<" "
          //  <<vcl_endl;
          if (flag)  //flag not to draw first point.
            sh_pts.push_back(new vsol_point_2d(p0[0]/p0[2],p0[1]/p0[2]));
          flag=true;
        }


        //vcl_cout<<"=== "<<frame<<vcl_endl;
        //vcl_cout<<" "<<vcl_endl;


        if(grey_view.nplanes() != 1) {
          vcl_cerr << "Returning false. nplanes(): " << grey_view.nplanes() << vcl_endl;
          return ;
        }
        output_vsol->add_objects(sh_pts);
        output_data_[frame].push_back(output_vsol);

        //        output_data_[frame][0]->

        image_pts_frames.push_back(image_pts);


    }

    vcl_vector <double > mean_list;
    for (unsigned j=0;j< image_pts_frames[0].size();j++)//# of pixel in rectangle
    {
      double s=0;
      for (unsigned i=0;i < input_data_.size();i++) //across the frames
      {
        s+=image_pts_frames[i][j];
      }
      double mean=s/image_pts_frames[0].size();
      mean_list.push_back(mean);
    }

    //error calculation
    double all_error=0.0;
    for (unsigned j=0;j< image_pts_frames[0].size();j++)//# of pixel in rectangle
    {
      double error=0;
      for (unsigned i=0;i < input_data_.size();i++) //across the frames
      {
        error+=(image_pts_frames[i][j]-mean_list[j])*(image_pts_frames[i][j]-mean_list[j]);
      }
      all_error+=error/input_data_.size();
      //mean_list.push_back(mean);
    }

    vcl_cout<<dx<<" "<<all_error<<vcl_endl;
  }//dx
}







void dbshadow_process::probe_windshield(vnl_double_4 s0, vnl_double_4 e0)
{

  int frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

    vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
    vcl_vector< vsol_spatial_object_2d_sptr > sh_pts;
    vidpro_vtol_storage_sptr frame_vtol;

    ////frame_image.vertical_cast(input_data_[frame][0]);
    vcl_cout<<output_data_.size()<<vcl_endl;
    vcl_cout<<output_data_[frame].size()<<vcl_endl;
    vcl_cout<<input_data_[frame][0]<<vcl_endl;
    frame_vtol.vertical_cast(input_data_[frame][2]);// 1 is ocntour?
    vcl_cout<<frame_vtol<<vcl_endl;
  }

}


// from start point and end point draw a elongated rectagle and count the edge in it
double dbshadow_process::count_edge(vnl_double_4 s0, vnl_double_4 e0)
{

  double edge_N=0.0;

  double width=.03;
  vnl_double_4 s0u(s0),s0d(s0); // up down
  s0u(2)+=width/2;s0d(2)-=width/2;
  vnl_double_4 e0u(e0),e0d(e0); // up down
  e0u(2)+=width/2;e0d(2)-=width/2;
  vcl_vector <vnl_double_4> pts;
  pts.push_back(s0d);pts.push_back(s0u);pts.push_back(e0u);pts.push_back(e0d);

  int *count= new int[input_data_.size()];
  int frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
    count[frame]=0;



    vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
    vcl_vector< vsol_spatial_object_2d_sptr > sh_pts;

    // generate probe on 2d
    vcl_vector <vgl_point_2d<double> > image_pts;
    for (unsigned ki=0;ki<pts.size();ki++) {
      vnl_double_3 p0=Cam_List_[input_data_.size()-frame-1]*pts[ki];
      double v0=p0[0]/p0[2];double w0=p0[1]/p0[2];
      image_pts.push_back( vgl_point_2d<double >(v0,w0));
    }
    vgl_polygon<double> pol(image_pts);


    //check if the edge is in the probe
    vcl_vector <vsol_digital_curve_2d_sptr> curve_edge_list = dcl_edge_[frame];
    //vcl_cout<<"curve_edge_list size:"<<curve_edge_list.size()<<vcl_endl;
    for (unsigned i=0;i<curve_edge_list.size();i++) {
      vsol_digital_curve_2d_sptr dc_edge=curve_edge_list[i];
      //vcl_cout<<dc_edge->size()<<vcl_endl;
      for(int j=0; j<dc_edge->size(); j++){
        vsol_point_2d_sptr pt = dc_edge->point(j);
        //pt->set_x(pt->x()+x);
        //pt->set_y(pt->y()+y);
        pt->x();
        pt->y();
        if (pol.contains(pt->x(),pt->y())) {
          count[frame] ++;
          sh_pts.push_back(new vsol_point_2d(pt->x(),pt->y()));
        }
      }
    }


    output_vsol->add_objects(sh_pts);
    output_data_[frame].push_back(output_vsol);

  }//frame




  double sum=0;
  for (unsigned i=0;i<input_data_.size();i++)
    sum+=count[i];
  delete[] count;
  edge_N=sum;
  return edge_N;

}




double dbshadow_process::shadow_histo(vnl_double_4 p0, vnl_double_4 p7)
{


  vnl_double_4 pt3d_0(p0),pt3d_7(p7); // 
  vnl_double_4 pt3d_6(pt3d_7(0),pt3d_0(1),(pt3d_0(2)+pt3d_7(2))/2.0,1);
  pt3d_0(3)=pt3d_6(3);pt3d_7(3)=pt3d_6(3);

  
  //band of shadow..
  double band=1;
  vnl_double_4 pt3d_0_down(pt3d_0),pt3d_6_down(pt3d_6),pt3d_7_down(pt3d_7); // band strp down
  pt3d_0_down(2)-=band;pt3d_6_down(2)-=band;pt3d_7_down(2)-=band;

  vnl_double_4 pt3d_0_up(pt3d_0),pt3d_6_up(pt3d_6),pt3d_7_up(pt3d_7); // band strp up
  pt3d_0_up(2)+=band;pt3d_6_up(2)+=band;pt3d_7_up(2)+=band;
  
  bool draw_bottom_line=true;
  parameters()->get_value( "-sdraw_bline" ,   draw_bottom_line);
  

  
  int *shadow_count= new int[input_data_.size()];
  double *r_count= new double[input_data_.size()];
  double *g_count= new double[input_data_.size()];
  double *b_count= new double[input_data_.size()];
  double *grey_count= new double[input_data_.size()];

  int *body_count= new int[input_data_.size()];
  double *body_r_count= new double[input_data_.size()];
  double *body_g_count= new double[input_data_.size()];
  double *body_b_count= new double[input_data_.size()];
  double *body_grey_count= new double[input_data_.size()];

  int frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
    shadow_count[frame]=0;
    r_count[frame]=0;
    g_count[frame]=0;
    b_count[frame]=0;
    grey_count[frame]=0;

    body_count[frame]=0;
    body_r_count[frame]=0;
    body_g_count[frame]=0;
    body_b_count[frame]=0;
    body_grey_count[frame]=0;

    vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
    vcl_vector< vgl_point_2d <double> > image_pts;
    vcl_vector< vsol_spatial_object_2d_sptr > sh_pts;

    vnl_double_3 pt2d_0=Cam_List_[input_data_.size()-frame-1]*pt3d_0;
    vnl_double_3 pt2d_6=Cam_List_[input_data_.size()-frame-1]*pt3d_6;
    vnl_double_3 pt2d_7=Cam_List_[input_data_.size()-frame-1]*pt3d_7;
    vgl_point_2d <double> p2d_0(pt2d_0[0]/pt2d_0[2],pt2d_0[1]/pt2d_0[2]);
    vgl_point_2d <double> p2d_6(pt2d_6[0]/pt2d_6[2],pt2d_6[1]/pt2d_6[2]);
    vgl_point_2d <double> p2d_7(pt2d_7[0]/pt2d_7[2],pt2d_7[1]/pt2d_7[2]);
    vgl_line_2d <double> l1(p2d_0,p2d_6);
    vgl_line_2d <double> l2(p2d_6,p2d_7);


    vnl_double_3 pt2d_0_down=Cam_List_[input_data_.size()-frame-1]*pt3d_0_down;
    vnl_double_3 pt2d_6_down=Cam_List_[input_data_.size()-frame-1]*pt3d_6_down;
    vnl_double_3 pt2d_7_down=Cam_List_[input_data_.size()-frame-1]*pt3d_7_down;
    vgl_point_2d <double> p2d_0_down(pt2d_0_down[0]/pt2d_0_down[2],pt2d_0_down[1]/pt2d_0_down[2]);
    vgl_point_2d <double> p2d_6_down(pt2d_6_down[0]/pt2d_6_down[2],pt2d_6_down[1]/pt2d_6_down[2]);
    vgl_point_2d <double> p2d_7_down(pt2d_7_down[0]/pt2d_7_down[2],pt2d_7_down[1]/pt2d_7_down[2]);
    vgl_line_2d <double> l1_down(p2d_0_down,p2d_6_down);
    vgl_line_2d <double> l2_down(p2d_6_down,p2d_7_down);

    vnl_double_3 pt2d_0_up=Cam_List_[input_data_.size()-frame-1]*pt3d_0_up;
    vnl_double_3 pt2d_6_up=Cam_List_[input_data_.size()-frame-1]*pt3d_6_up;
    vnl_double_3 pt2d_7_up=Cam_List_[input_data_.size()-frame-1]*pt3d_7_up;
    vgl_point_2d <double> p2d_0_up(pt2d_0_up[0]/pt2d_0_up[2],pt2d_0_up[1]/pt2d_0_up[2]);
    vgl_point_2d <double> p2d_6_up(pt2d_6_up[0]/pt2d_6_up[2],pt2d_6_up[1]/pt2d_6_up[2]);
    vgl_point_2d <double> p2d_7_up(pt2d_7_up[0]/pt2d_7_up[2],pt2d_7_up[1]/pt2d_7_up[2]);
    vgl_line_2d <double> l1_up(p2d_0_up,p2d_6_up);
    vgl_line_2d <double> l2_up(p2d_6_up,p2d_7_up);

    
    vsol_line_2d_sptr a= new vsol_line_2d(p2d_0,p2d_6);
    sh_pts.push_back(a->cast_to_spatial_object());
    vsol_line_2d_sptr b= new vsol_line_2d(p2d_6,p2d_7);
    sh_pts.push_back(b->cast_to_spatial_object());


    vsol_line_2d_sptr a_down= new vsol_line_2d(p2d_0_down,p2d_6_down);
    sh_pts.push_back(a_down->cast_to_spatial_object());
    vsol_line_2d_sptr b_down= new vsol_line_2d(p2d_6_down,p2d_7_down);
    sh_pts.push_back(b_down->cast_to_spatial_object());


    vsol_line_2d_sptr a_up= new vsol_line_2d(p2d_0_up,p2d_6_up);
    sh_pts.push_back(a_up->cast_to_spatial_object());
    vsol_line_2d_sptr b_up= new vsol_line_2d(p2d_6_up,p2d_7_up);
    sh_pts.push_back(b_up->cast_to_spatial_object());
    //sh_pts.push_back(vsol_line_2d(l2));

    //    sh_pts.push_back(new vsol_point_2d(ec->edgel(i).get_pt()));
    output_vsol->add_objects(sh_pts);
   if (draw_bottom_line) output_data_[frame].push_back(output_vsol);

   
    for (unsigned i=0;i<blob_image_list_[frame].size();i++) {

      double x=blob_image_list_[frame][i][0];
      double y=blob_image_list_[frame][i][1];

      //shadow
      if (
        (l1.a()*x+l1.b()*y+l1.c()>0||l2.a()*x+l2.b()*y+l2.c()>0)   &&
        l1_down.a()*x+l1_down.b()*y+l1_down.c()<0  &&  
        l2_down.a()*x+l2_down.b()*y+l2_down.c()<0) 
      
      {
        shadow_count[frame]++;
        grey_count[frame]+=blob_image_list_[frame][i][2];
        r_count[frame]+=blob_image_list_[frame][i][3];
        g_count[frame]+=blob_image_list_[frame][i][4];
        b_count[frame]+=blob_image_list_[frame][i][5];

       // vcl_cout<<blob_image_list_[frame][i]<<vcl_endl;
      }

      else if (
        (l1.a()*x+l1.b()*y+l1.c()<0&&l2.a()*x+l2.b()*y+l2.c()<0)   &&
        (l1_up.a()*x+l1_up.b()*y+l1_up.c()>0  ||  
        l2_up.a()*x+l2_up.b()*y+l2_up.c()>0) ) 
      
      {
        body_count[frame]++;
        body_grey_count[frame]+=blob_image_list_[frame][i][2];
        body_r_count[frame]+=blob_image_list_[frame][i][3];
        body_g_count[frame]+=blob_image_list_[frame][i][4];
        body_b_count[frame]+=blob_image_list_[frame][i][5];

        //vcl_cout<<blob_image_list_[frame][i]<<vcl_endl;
      }


    } //image list

  }  //frame

  if (0) {
    frame=0;
    for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
      vcl_cout<<"shadow # "<<frame<<" : "<<shadow_count[frame]<<vcl_endl;
      vcl_cout<<"a # "<<frame<<" : "<<grey_count[frame]<<vcl_endl;
      vcl_cout<<"r # "<<frame<<" : "<<r_count[frame]<<vcl_endl;
      vcl_cout<<"g # "<<frame<<" : "<<g_count[frame]<<vcl_endl;
      vcl_cout<<"b # "<<frame<<" : "<<b_count[frame]<<vcl_endl;
    }

    frame=0;
    for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
      vcl_cout<<"body # "<<frame<<" : "<<body_count[frame]<<vcl_endl;
      vcl_cout<<"a # "<<frame<<" : "<<body_grey_count[frame]<<vcl_endl;
      vcl_cout<<"r # "<<frame<<" : "<<body_r_count[frame]<<vcl_endl;
      vcl_cout<<"g # "<<frame<<" : "<<body_g_count[frame]<<vcl_endl;
      vcl_cout<<"b # "<<frame<<" : "<<body_b_count[frame]<<vcl_endl;
    }

  }

  frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
    vcl_cout<<grey_count[frame]/shadow_count[frame]<<" ";
  }
  vcl_cout<<vcl_endl;
  frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){
    vcl_cout<<body_grey_count[frame]/body_count[frame]<<" ";
  }
  vcl_cout<<vcl_endl;
  vcl_cout<<vcl_endl;

  delete[] shadow_count;
  delete[] r_count;
  delete[] g_count;
  delete[] b_count;
  delete[] grey_count;
  delete[] body_count;
  delete[] body_r_count;
  delete[] body_g_count;
  delete[] body_b_count;
  delete[] body_grey_count;
  return 0.0;
}




// mark shadow region by percentage
void dbshadow_process::shadow_histo()
{
  float grey_level_threshold=.1;
  parameters()->get_value( "-sgrey_th" ,   grey_level_threshold);
  int frame=0;

  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame)
  {
    //blob_image_list_[frame][i][2];
     //vcl_sort(pts_z.begin(), pts_z.end(), dbcri_accu_edge_cmp_z);
    vcl_sort(blob_image_list_[frame].begin(), blob_image_list_[frame].end(), cmp_grey);
  }

  frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame)
  {
    vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
    vcl_vector< vsol_spatial_object_2d_sptr > sh_pts;
    for (unsigned i=0;i<grey_level_threshold*blob_image_list_[frame].size();i++)
    {
      sh_pts.push_back(new vsol_point_2d(blob_image_list_[frame][i][0],blob_image_list_[frame][i][1]));
    }
    output_vsol->add_objects(sh_pts);
    output_data_[frame].push_back(output_vsol);
  }
}



