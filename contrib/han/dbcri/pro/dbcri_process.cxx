// This is pro/dbcri_process.cxx

//:
// \file
#define DEBUG 0
#define WORLD_CAM true
#define FRAME_OFFSET 2;
//#define ERROR 1000

///  if all pts across frames are good use it or not (in cross ratio sense)
#define ANGLE_FLAG false 
///
#include "dbcri_process.h"
//#include "dbecl_episeg_storage.h"
#include <bpro/bpro_parameters.h>

#include <vidpro/storage/vidpro_vtol_storage.h>
#include <vidpro/storage/vidpro_vsol2D_storage_sptr.h>
#include <vidpro/storage/vidpro_vsol2D_storage.h>
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

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_point_2d.h>



#include <bmvl/brct/brct_algos.h>


#include <vnl/vnl_math.h> // for vnl_math_isnan()


#include <vgl/vgl_polygon.h>
#include <vgl/algo/vgl_convex_hull_2d.h>


#include <bsol/bsol_algs.h>

//: Constructor
dbcri_process::dbcri_process()
: bpro_process()
{

  if(!parameters()->add( "world camera <filename...>" , "-wc_filename", bpro_filepath("","*") )||
    !parameters()->add( "use epipole from world camera?", "-epi_flag",  (bool) false)||
    !parameters()->add( "draw_only?", "-draw_flag",  (bool) false)||
    !parameters()->add( "use default world camera?", "-wc_flag",  (bool) false) ) 
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  } 
}


//: Copy Constructor
dbcri_process::
dbcri_process(const dbcri_process& other)
: bpro_process(other)
{
}


//: Destructor
dbcri_process::~dbcri_process()
{
}


//: Clone the process
bpro_process* 
dbcri_process::clone() const
{
  return new dbcri_process(*this);
}


//: Return the name of the process
vcl_string
dbcri_process::name()
{
  return "Cross Ratio Inv Calibration";
}


//: Returns a vector of strings describing the input types to this process
vcl_vector< vcl_string > dbcri_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Returns a vector of strings describing the output types of this process
vcl_vector< vcl_string > dbcri_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
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
dbcri_process::input_frames()
{
  return 1;
}


//: Returns the number of output frames from this process
int
dbcri_process::output_frames()
{
  //return nframes_;
  return 1;
}




//: Run the process on the current frame
bool
dbcri_process::execute()
{
  bpro_filepath wc_path;
  bool default_wc_flag;


  parameters()->get_value( "-wc_filename" , wc_path );
  parameters()->get_value( "-wc_flag" , default_wc_flag );


  if (!default_wc_flag&&wc_path.path=="") return 0;
  if (!default_wc_flag) {
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
  else {

    read_WC();
  }
  if(vnl_det(vnl_double_3x3(WC_.extract(3,3))) < 0) {
      WC_ *= -1;
      vcl_cout<<"WC_ *= -1 ***** right hand camera *****"<<vcl_endl;
      vcl_cout<<WC_<<vcl_endl;
  }


  return true;
 
  
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

  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

    // get image from the storage class

    ////vidpro_image_storage_sptr frame_image;
    vidpro_vsol2D_storage_sptr frame_vsol=vidpro_vsol2D_storage_new();

    ////frame_image.vertical_cast(input_data_[frame][0]);
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
            }
          }
        }

      }


    }

    vgl_point_2d <double> p_upper(max_x,max_y);
    vgl_point_2d <double> p_lower(min_x,min_y);
    vcl_cout<<p_upper<<p_lower<<vcl_endl;
    pts_upper.push_back(p_upper);
    pts_lower.push_back(p_lower);


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

  //find the cross section..

  vgl_line_2d <double> line_upper=reg_upper.get_line();
  vgl_line_2d <double> line_lower=reg_lower.get_line();

  line_upper_=line_upper;
  line_lower_=line_lower;

  double a,b,c,d;
  a=line_upper.b()/line_upper.a(); b=line_upper.c()/line_upper.a();
  c=line_lower.b()/line_lower.a(); d=line_lower.c()/line_lower.a();

  double epi_x=(a*d-b*c)/(c-a);
  double epi_y=(b-d)/(c-a);

  epi_x_=epi_x;
  epi_y_=epi_y;





  ////done
  nframes_=frame;
  //for (unsigned frame = 0; frame<input_data_.size(); frame++) {
  frame = 0;
  for(; frame < input_data_.size(); ++frame)
    if(!input_data_[frame].empty() && input_data_[frame][0])
      break;

  if(frame >= input_data_.size())
    return false;

  if (0)
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

    vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
    vgl_point_2d<double> epipole(epi_x_,epi_y);
    vgl_point_2d <double> uppoint(1000.0,(-1000.0f-line_upper_.c())/line_upper_.b());
    vgl_point_2d <double> lowpoint(1000.0,(-1000.0f-line_lower_.c())/line_lower_.b());
    vsol_line_2d_sptr newhline_upper= new vsol_line_2d(epipole,uppoint);
    vsol_line_2d_sptr newhline_lower= new vsol_line_2d(epipole,lowpoint);
    //add to the storage class
    output_vsol->add_object( newhline_upper->cast_to_spatial_object() ,"output_upperline" );
    output_vsol->add_object( newhline_lower->cast_to_spatial_object() ,"output_lowerline" );
    output_data_[frame].push_back(output_vsol);
  }


  
  return true;
}





static bool approx_equal(double a, double b, double thresh=1e-12)
{
  return vcl_abs(b-a) < thresh;
}


//: Finish
bool
dbcri_process::finish()
{

 // print_bb_and_camera();
 // return 1;

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

    vgl_point_2d <double> left_pt(left_x,left_y);
    vgl_point_2d <double> right_pt(right_x,right_y);
    left_pts_.push_back(left_pt);
    right_pts_.push_back(right_pt);





    dcl_.push_back(dc1);

    vgl_polygon<double> veh_contour(x1,y1,N);
    veh_cons_.push_back(veh_contour);    
    free(x1);
    free(y1);
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

  //find the cross section..

  vgl_line_2d <double> line_upper=reg_upper.get_line();
  vgl_line_2d <double> line_lower=reg_lower.get_line();

  line_upper_=line_upper;
  line_lower_=line_lower;

  vcl_cout<<line_upper_<<" "<<line_lower_<<vcl_endl;
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

  parameters()->get_value( "-epi_flag" , epi_flag );
   parameters()->get_value( "-draw_flag" , draw_only );
 // if (epi_x_<-400.0&&epi_flag) {
   if (epi_flag) {

    vnl_double_3x4 WC=WC_;
    vnl_double_3 nep = WC.get_column(0);   
    epi_x_= float(nep[0]/nep[2]);
    epi_y_= float(nep[1]/nep[2]);

    vcl_cout<<"epipole from camera used"<<vcl_endl;
    vcl_cout<< epi_x_<<" "<<epi_y_<<vcl_endl;


  }

  /////////////////
  /////////////////
  ///////////////// epipole from camera ///
/*vnl_double_3x4 WC;
 WC[0][0]= 2.71954;    WC[0][1]= 12.1025;    WC[0][2]=0;         WC[0][3]=63.9305;
  WC[1][0]=-1.00509;    WC[1][1]= 0.757923;   WC[1][2]=-11.6824;  WC[1][3]= 159.883;
  WC[2][0]=-0.00516136; WC[2][1]= 0.0026911;  WC[2][2]= 0;        WC[2][3]= 0.33245;

  vnl_double_3 nep = WC.get_column(0);   
      epi_x_= float(nep[0]/nep[2]);
      epi_y_= float(nep[1]/nep[2]);

      vcl_cout<<"epipole from camera"<<vcl_endl;
      vcl_cout<< epi_x_<<" "<<epi_y_<<vcl_endl;*/
  /////////////////
  /////////////////
  /////////////////


 // vcl_vector < vcl_vector <dbecl_episeg_sptr> >episeglist;

  ////done
  nframes_=frame;
  //for (unsigned frame = 0; frame<input_data_.size(); frame++) {
  frame = 0;
  for(; frame < input_data_.size(); ++frame)
    if(!input_data_[frame].empty() && input_data_[frame][0])
      break;

  if(frame >= input_data_.size())
    return false;


  // get line to display

  vgl_point_2d<double> epipole(epi_x_,epi_y_);
  vcl_cout<<epipole<<vcl_endl;
  vgl_point_2d <double> uppoint(1000.0,(-1000.0f*line_upper_.a()-line_upper_.c())/line_upper_.b());
  vgl_point_2d <double> lowpoint(1000.0,(-1000.0f*line_lower_.a()-line_lower_.c())/line_lower_.b());
  vsol_line_2d_sptr newhline_upper= new vsol_line_2d(epipole,uppoint);
  //newhline_upper->vgl_hline_2d();
  vsol_line_2d_sptr newhline_lower= new vsol_line_2d(epipole,lowpoint);

  //get angle..

  vcl_cout<<uppoint<<" "<<lowpoint<<vcl_endl;
  theta_pos_=-vcl_atan((uppoint.y()-epi_y_)/(1000.0f-epi_x_));
  theta_neg_=-vcl_atan((lowpoint.y()-epi_y_)/(1000.0f-epi_x_));




  /// generate angle_flag;
  angle_f_flag_.clear();
  angle_r_flag_.clear();
  for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=.002) { 
     bool angle_flag=true;

     angle_f_flag_.push_back(angle_flag);
     angle_r_flag_.push_back(angle_flag);
  }


  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){

    vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();
 

    //add to the storage class
    output_vsol->add_object( newhline_upper->cast_to_spatial_object() ,"output_upperline" );
    output_vsol->add_object( newhline_lower->cast_to_spatial_object() ,"output_lowerline" );
    output_data_[frame].push_back(output_vsol);

    dbecl_epipole_sptr epipole1 = new dbecl_epipole(epi_x_,epi_y_);
    // make an episeg
    //dbecl_episeg_sptr episeg = new dbecl_episeg(epipole1, dcl_[frame], 0.0, double(dcl_[frame]->size()-1));


    //////////////
    //////////////
    //////////////


    // _epi_segs.clear();
    // vcl_vector<dbecl_episeg_sptr> episegs;
    //dbecl_episeg_from_curve_converter factory(p->epipole());
    dbecl_episeg_from_curve_converter factory(epipole1);
    //for(int i = 0; i < _curves.size(); i++) {
    vsol_digital_curve_2d_sptr curve = dcl_[frame];
    vcl_vector<dbecl_episeg_sptr> episegs = factory.convert_curve(curve);
    //_epi_segs.insert(_epi_segs.end(), episegs.begin(), episegs.end());
    //}
    //_curves.clear();
    //discretize();
    //////////////
    //////////////
    //////////////

    // frame 마다 에피세그먼트를 가지고 잇어야 한다.
    episeglist.push_back(episegs);

  }


  dbecl_epipole_sptr epipole2 = new dbecl_epipole(epi_x_,epi_y_);

  if (draw_only) return true;

  // double r=cameras(0,1,2,dcl_,episeglist,epipole2);

  vcl_cout<<"\n---------------------------------------"<<vcl_endl;
  int C=episeglist.size();
  int combination=C*(C-1)*(C-2);
  combination/=6;
  vnl_matrix <double> B((int)combination,C-1,0.0);


  vnl_matrix <double> BB((int)combination,C-2,0.0);

  display_0_frame_=true;
  //display_n_frame_=true;
  int counter=0;
  for (unsigned i=0; i<episeglist.size();i++) {
    for (unsigned j=i+1; j<episeglist.size();j++) {
      for (unsigned k=j+1; k<episeglist.size();k++) {

        if (k==episeglist.size()-1) display_n_frame_=true;
        else display_n_frame_=false;
        if (i==0&&j==1) display_0_frame_=true;
        else display_0_frame_=false;
        

        //generate vector 

        double r=cameras(i,j,k,dcl_,episeglist,epipole2);
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


  //vnl_vector <double>  x_;
  //vnl_vector <double> t=vnl_inverse(BB.transpose()*BB)*BB.transpose()*bb;
  
 
  
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
  init_cameras(tnew);
  //


  frame = 0;
  for(; frame < input_data_.size(); ++frame)
    if(!input_data_[frame].empty() && input_data_[frame][0])
      break;

  if(frame >= input_data_.size())
    return false;

  int frame_counter=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame){


    vidpro_vsol2D_storage_sptr output_vsol = vidpro_vsol2D_storage_new();

    if (frame==0||frame==input_data_.size()-1)
    {
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

      vcl_cout<<vcl_sqrt(x_dist*x_dist+y_dist*y_dist)<<" "<<c<<vcl_endl;
      
      double temp_r=0;
      
      for (unsigned tr=0;tr<frame;tr++) temp_r+=tnew[tr];

      double r=(total_t-temp_r)/total_t;
      //double r=temp_r/total_t;
      double r_prime=r*(c-a)/c;
      double b=a/(1-r_prime);


      vsol_point_2d_sptr point_mid=new vsol_point_2d(epipole.x()+b/c*x_dist,epipole.y()+b/c*y_dist);

      vcl_cout<<(epipole.x()+b/c*x_dist)<<" "<<epipole.y()+b/c*y_dist<<vcl_endl;
      //output_vsol->add_object( line_mid->cast_to_spatial_object() ,"output_midline" );
      output_vsol->add_object( point_mid->cast_to_spatial_object() ,"output_midpoint" );
     
    }
    output_data_[frame].push_back(output_vsol);
    //rear display


  }




  // 3d reconstruction


  //d3_build();


  vcl_vector <vgl_point_3d<double> > point_3d_mean_pts;
  d3_build_points(10000000000000.0, point_3d_mean_pts);
  
  ///////////get bb and orentation and scale 
  
  delete_contour();
  pts_3d_.clear();
  int point_counter=0;
  for (unsigned i=0;i <pts_3d_a_.size();i++) {
    
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
  }vcl_cout<<"points used counter:"<<point_counter<<vcl_endl;
  get_BB_and_scale(pts_3d_,pts_3d_);

  vcl_cout<<scale_<<vcl_endl; //this is real scale...

  //get_BB_and_scale(point_3d_mean_pts,point_3d_mean_pts);

  
  // redo it with new scale
  init_cameras(tnew,scale_);

   vcl_vector <vgl_point_3d<double> > point_3d_mean_pts_with_scale;
  d3_build_points(10.0, point_3d_mean_pts_with_scale);


  delete_contour(.5);//delete 1 feet high too.
  pts_3d_.clear(); point_counter=0;
  for (unsigned i=0;i <pts_3d_a_.size();i++) {//d3_build_points  clear pts_3_a_
    
    //vcl_cout<<point_3d_mean_pts_with_scale[i]<<vcl_endl;

    
    if (0){ 
    vcl_cout<<pts_3d_a_[i].pt3d<<vcl_endl;
    vcl_cout<<pts_3d_a_[i].position<<vcl_endl;
    vcl_cout<<pts_3d_a_[i].angle<<vcl_endl;
    vcl_cout<<pts_3d_a_[i].weight<<vcl_endl;
    vcl_cout<<vcl_endl;

    }


    ////////////
    if (pts_3d_a_[i].usage) {
      pts_3d_.push_back(pts_3d_a_[i].pt3d); 
      point_counter++;
     
    }
  } vcl_cout<<"points used counter:"<<point_counter<<vcl_endl;
  get_BB_and_scale(pts_3d_,pts_3d_);



  vcl_string vrml_file="del.wrl";
  
  vcl_ofstream vrml(vrml_file.c_str());
  write_vrml_2(vrml,pts_3d_);
  //get_BB_and_scale(pts_3d_,pts_3d_);
  write_vrml_bbox(vrml, bb_xform_);
  
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  /////////////////////////////////////////////

  Estimation_BB_shift_using_convex_hull_in_2D();// reduce the BB size
  
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  double r=.1;
  spacial_filtering(r,pts_3d_);
  /*Estimation_BB_and_bb_using_convex_hull_in2D(0);
  Estimation_BB_and_bb_using_convex_hull_in2D(1);
  Estimation_BB_and_bb_using_convex_hull_in2D(2);
  generate_mean_contour_model(0);
  generate_mean_contour_model(1);
  generate_mean_contour_model(2);
*/
  /////////////////////////////////////////////
  /////////////////////////////////////////////
  /////////////////////////////////////////////

  write_vrml_bbox(vrml, bb_xform_);
  vrml.close();
  print_bb_and_camera();
// redo it with new scale
 // init_cameras(tnew,scale_);
  

  /////////// vrml generation
    
 vcl_ofstream out("out.wrl");
  
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

  for (unsigned i=0;i <point_3d_mean_pts.size();i++) {
    //point_3d_mean_pts[i].set(scale_*point_3d_mean_pts[i].x(),scale_*point_3d_mean_pts[i].y(),scale_*point_3d_mean_pts[i].z());
    //vcl_cout<<point_3d_mean_pts[i]<<vcl_endl;






   

     out<<"Transform {  \n";
          out<<"translation "<<point_3d_mean_pts_with_scale[i].x()<<" "<<point_3d_mean_pts_with_scale[i].y()<<" "<<point_3d_mean_pts_with_scale[i].z()<< "\n";  
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

  }vcl_cout<<"#### end ####"<<vcl_endl;

  

 vcl_cout<<"epipole used was_ "<< epi_x_<<" "<<epi_y_<<vcl_endl;
 return true;

}






///// cross ratio calculetion

double dbcri_process::cameras(int fm1, int fm2, int fm3,
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
  int angle_index=-1;
  for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=.002) { 

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


    vgl_point_3d<double> point_3d = brct_algos::triangulate_3d_point(x0pt.pt(), P1, x2pt.pt(), P3);

    //vcl_cout<<point_3d<<vcl_endl;
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
  for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=.002) { 
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

//3 frames given, compute cross ratio
double dbcri_process::cross_ratio(dbecl_epipole_sptr &epipole,dbecl_episeg_sptr & epi_segl1,
                                  dbecl_episeg_sptr & epi_segl2,dbecl_episeg_sptr & epi_segl3) 
{



  
  return 0.0;
}



//: Write cameras and points to a file in VRML 2.0 for debugging
void 
dbcri_process::write_vrml(const vcl_string& filename,
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


void dbcri_process::write_vrml_2(vcl_ofstream& out,//const vcl_string& filename,
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

          out<<" material Material { diffuseColor "<<red <<" "<<green <<" "<<blue <<"}\n";


          out<<" }   \n";
          out<<"  geometry  Sphere{  \n"; 
          //out<<" radius "<< radius*(j+5)/7 << "}  \n"; 
          out<<" radius "<< radius*scale_ << "}  \n"; 
          out<<" } \n"; 
          out<<"} \n"; 

  }


  vcl_cout<<"#### write_vrml_2 end ####"<<vcl_endl;
}

double 
dbcri_process::histogram_method_for_finding_cross_ratio(vcl_vector <ratios >& ratios_front_list, vcl_vector < ratios >& ratios_rear_list) 
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

      
      if (counter%4==0&&display_0_frame_&&display_n_frame_)
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


      if (counter%9==0&&display_0_frame_&&display_n_frame_)
      display_sample_rear_.push_back(ratios_rear_list[i]);
    }

  }

  //vcl_cout<<mean_r/(double)counter<<vcl_endl;


  //display_0_frame_=false; // display sample needed for 0 frame once..
  return mean_r/counter;

}


void dbcri_process::init_cameras(vnl_vector <double> t,double scale)
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

  C[0][0]= 2.71954;    C[0][1]= 12.1025;    C[0][2]=0;         C[0][3]=63.9305;
  C[1][0]=-1.00509;    C[1][1]= 0.757923;   C[1][2]=-11.6824;  C[1][3]= 159.883;
  C[2][0]=-0.00516136; C[2][1]= 0.0026911;  C[2][2]= 0;        C[2][3]= 0.33245;
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


  vcl_cout<<t<<T<<vcl_endl;
  ///////////////reverse camera


  //vcl_reverse(cameras_.begin(), cameras_.end());
  //for (unsigned i=0;i<cameras_.size();i++)
  //vcl_cout<<cameras_[i]<<vcl_endl;
  ///////////////
}



void dbcri_process::d3_build(vcl_vector <vsol_digital_curve_2d_sptr> &dcl_,
                             vcl_vector< vcl_vector <dbecl_episeg_sptr> >&episegl)

{
}

void dbcri_process::d3_build()

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

    /*vgl_point_2d <double> p_upper(max_x,max_y);
    vgl_point_2d <double> p_lower(min_x,min_y);
    vcl_cout<<p_upper<<p_lower<<vcl_endl;
    pts_upper.push_back(p_upper);
    pts_lower.push_back(p_lower);*/

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



bool dbcri_process::is_inwedge(vgl_point_2d<double> p, int frame) 
{

  // vgl_line_2d <double> line_upper=reg_upper.get_line();
  //vgl_line_2d <double> line_lower=reg_lower.get_line();

  //line_upper_=line_upper;
  //line_lower_=line_lower;

 // vcl_cout<<line_upper_<<" "<<line_lower_<<vcl_endl;
  
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



void dbcri_process::d3_build_points(double err, vcl_vector <vgl_point_3d<double> >& point_3d_mean_pts) 
{
pts_3d_a_.clear();

pts_3d_attribute p_att;
p_att.usage=true;
p_att.error=0.0;
p_att.count=0;

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
  for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=.002) { 

    p_att.angle=ang;
    p_att.position=true; //riught side(front)
    p_att.weight=(ang+theta_pos_)/(-theta_neg_+theta_pos_);



    
    angle_index++;
    if (ANGLE_FLAG)
    if (angle_f_flag_[angle_index]==false) continue;


    

    for (unsigned fm1=0;fm1<nframes_;fm1++)
      for (unsigned fm2=fm1+3;fm2<nframes_;fm2++) {

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
      vcl_cout<<(error/count)<<" "<<count<<vcl_endl;

      if (error/count< err&& count>0) {   


        //find mean



        /*
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
          */

          //only selected points
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
  for (double ang=-theta_pos_ ; ang<-theta_neg_ ; ang+=.002) { 

    p_att.angle=ang;
    p_att.position=false; //left side(rear)
    p_att.weight=(ang+theta_pos_)/(-theta_neg_+theta_pos_);


     angle_index++;
     if (ANGLE_FLAG)
    if (angle_r_flag_[angle_index]==false) continue;

    for (unsigned fm1=0;fm1<nframes_;fm1++)
      for (unsigned fm2=fm1+3;fm2<nframes_;fm2++) {

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

        ratios_rear_list.push_back(temp_ratios);*/

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
      vcl_cout<<(error/count)<<" "<<count<<vcl_endl;

      if (error/count< err&& count>0) {   


        //find mean


        /*
          out<<"Transform {  \n";
          out<<"translation "<<xx<<" "<<yy<<" "<<zz<< "\n";  
          out<<"Shape { \n";
          out<<"appearance Appearance { \n";
          //out<<" material Material { diffuseColor "<<r*(j+1)/15 <<" "<<g*(13-j)/13 <<" "<<b*(12+j)/24 <<"}\n";

          out<<" material Material { diffuseColor "<<r <<" "<<g <<" "<<1 <<"}\n";


          out<<" }   \n";
          out<<"  geometry  Sphere{  \n"; 
          //out<<" radius "<< radius*(j+5)/7 << "}  \n"; 
          out<<" radius "<< radius << "}  \n"; 
          out<<" } \n"; 
          out<<"} \n";     
          */
          
          //only selected points
          vgl_point_3d <double> mean_point_3d(xx,yy,zz);
          point_3d_mean_pts.push_back(mean_point_3d);

          p_att.error=error;
          p_att.count=count;

          p_att.pt3d=mean_point_3d;
          pts_3d_a_.push_back(p_att);
        }
     





        point_3d_rear_list.clear();

  }


  /*
 ///////////get bb and orentation and scale 
  get_BB_and_scale(point_3d_mean_pts,point_3d_mean_pts);


  
  for (unsigned i=0;i <point_3d_mean_pts.size();i++) {
    
    vcl_cout<<point_3d_mean_pts[i]<<vcl_endl;
  }vcl_cout<<"#### end ####"<<vcl_endl;



  for (unsigned i=0;i <point_3d_mean_pts.size();i++) {
    point_3d_mean_pts[i].set(scale_*point_3d_mean_pts[i].x(),scale_*point_3d_mean_pts[i].y(),scale_*point_3d_mean_pts[i].z());
    vcl_cout<<point_3d_mean_pts[i]<<vcl_endl;



     out<<"Transform {  \n";
          out<<"translation "<<point_3d_mean_pts[i].x()<<" "<<point_3d_mean_pts[i].y()<<" "<<point_3d_mean_pts[i].z()<< "\n";  
          out<<"Shape { \n";
          out<<"appearance Appearance { \n";
          //out<<" material Material { diffuseColor "<<r*(j+1)/15 <<" "<<g*(13-j)/13 <<" "<<b*(12+j)/24 <<"}\n";

          out<<" material Material { diffuseColor "<<r <<" "<<g <<" "<<1 <<"}\n";


          out<<" }   \n";
          out<<"  geometry  Sphere{  \n"; 
          //out<<" radius "<< radius*(j+5)/7 << "}  \n"; 
          out<<" radius "<< radius*scale_ << "}  \n"; 
          out<<" } \n"; 
          out<<"} \n"; 

  }vcl_cout<<"#### end ####"<<vcl_endl;

  */
//???
 // get_BB_and_scale(point_3d_mean_pts,point_3d_mean_pts);
  return;



  ////////////////////////////////////////////////////////////////////////////////////////
  /////  left_right_line_bb///////////////////////////////////////////////////////////////
  ////////////////////////////////////////////////////////////////////////////////////////

/*  vcl_vector <vgl_point_3d<double> > point_3d_left_list, point_3d_right_list;
  left_right_line_bb(point_3d_left_list, point_3d_right_list);

  r=0;g=0;b=1;
  for (unsigned i=0;i<point_3d_left_list.size();i++ ) {
    xx=point_3d_left_list[i].x();
    yy=point_3d_left_list[i].y();
    zz=point_3d_left_list[i].z();


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

  r=0;g=1;b=1;
    for (unsigned i=0;i<point_3d_right_list.size();i++ ) {
    xx=point_3d_right_list[i].x();
    yy=point_3d_right_list[i].y();
    zz=point_3d_right_list[i].z();


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
*/
}

void  dbcri_process::outliers(vcl_vector <vgl_point_3d<double> > point_3d_list) {

}




bool dbcri_sort(const double& rhs, const double& lhs)
{
  return rhs < lhs;
}
bool dbcri_cmp_x(const vnl_double_3& rhs, const vnl_double_3& lhs)
{
  return rhs[0] < lhs[0];
}

bool dbcri_cmp_y(const vnl_double_3& rhs, const vnl_double_3& lhs)
{
  return rhs[1] < lhs[1];
}

bool dbcri_cmp_z(const vnl_double_3& rhs, const vnl_double_3& lhs)
{
  return rhs[2] < lhs[2];
}



void dbcri_process::get_BB_and_scale(vcl_vector <vgl_point_3d<double> > point_3d_list_f,vcl_vector <vgl_point_3d<double> > point_3d_list_r) {

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
    double ang = angle(xz_proj, base_axis);
    rot_axis *= ang;
    vnl_double_3x3 rot_y = vnl_rotation_matrix(rot_axis);

    rot_axis = vnl_double_3(0.0, 0.0, 1.0);
    vnl_double_3 xy_proj = rot_y * vnl_double_3(direction.x(), direction.y(), direction.z());
    xy_proj.normalize();
    ang = angle(xy_proj, base_axis);
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


    vcl_cout<<rot<<vcl_endl;
    vcl_vector<vnl_double_3> pts_z;
    for (unsigned i=0;i<point_3d_list_f.size();i++) {
      vnl_double_3 p(point_3d_list_f[i].x(),point_3d_list_f[i].y(),point_3d_list_f[i].z());

      pts_z.push_back(rot*p);
      vcl_cout<<rot*p<<vcl_endl;
    }



 vcl_cout<<"checking pts # delivered to get bb: "<<point_3d_list_f.size()<<vcl_endl;
if (point_3d_list_f.size()==0) return;
  vcl_sort(pts_z.begin(), pts_z.end(), dbcri_cmp_z);
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

  vcl_sort(pts_x.begin(), pts_x.end(), dbcri_cmp_x);
  vcl_sort(pts_y.begin(), pts_y.end(), dbcri_cmp_y);

  vcl_cout<<"### pts_y start ###"<<vcl_endl;
  for (vcl_vector<vnl_double_3>::const_iterator itr = pts_y.begin();
        itr != pts_y.end();  ++itr)
  vcl_cout<<(*itr)<<vcl_endl;
  vcl_cout<<"### pts_y end ###"<<vcl_endl;

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

  vcl_cout<<"min_point:"<<min_point<<" "<<"max_point:"<< max_point<<vcl_endl;

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


void dbcri_process::left_right_line_bb(vcl_vector <vgl_point_3d<double> > &point_3d_left_list,vcl_vector <vgl_point_3d<double> > &point_3d_right_list) {


   


  
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




void dbcri_process::contour() {

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

void dbcri_process::delete_contour(double thresh) 
{
 
 // pts_3d_.clear();
  vcl_vector <vgl_point_3d  <double> > pts_3d;
  vgl_point_3d <double> p;
  for (unsigned i=0;i<pts_3d_a_.size();i++) {

    //pts_3d_a_[i].usage=true;
    if (pts_3d_a_[i].position&& pts_3d_a_[i].weight>.5)// use lower wedge points only
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
    vcl_cout<<pts_3d_a_[j].pt3d;
   
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

void dbcri_process::write_vrml_bbox( vcl_ofstream& str,
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

void dbcri_process::print_bb_and_camera()
{

  //from vidpro/process/vidpro_load_con_process.cxx

  //vul_file::is_directory(input_file_path))
  
  //vul_file_iterator fn=input_file_path+"/*.con";
  
  vcl_vector<vnl_double_3x4> cameras;
  vcl_vector <int> nums_list;
int counter=0;
    
    //start_number=empty_camera_number;//1-12-2005
    
                int fram_start;
    int fram_end;
    //int fram3 = start_number + frame_3 ;
    //  vcl_vector <bsol_intrinsic_curve_2d_sptr> curve_2dl;
    //curve_2dl_.clear();
    char tf[20];char f11[22]; 
    //fore_name.clear();
    //itoa(fram1,tf,10);
    // sprintf(f11,"%03s",tf);
    //vcl_sprintf(f11,"%03d",fram1);
                
                //if (BATCH_) f1=batch_dir_ + "./00" + f11+".png";
    //else f1=str_filename + "./00" + f11+".png";
    //vcl_cout<<f1<<vcl_endl;

    vcl_string input_file_path=".";
    vul_file_iterator fn=input_file_path+"/*.con";//nframes_=5;
    for ( ; fn; ++fn) 
    {
      vcl_string input_file = fn();
      vcl_cout<<input_file<<vcl_endl;
    /*  strcpy(f11,input_file.c_str());
      //sprintf(tf,"%02s",f11);
      vcl_string nums=" ";
      // vcl_cout<<nums<<vcl_endl;
      nums=nums+f11[14];
      // vcl_cout<<nums<<vcl_endl;
      nums=nums+f11[15];
    */
      
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
  vcl_ofstream bbout(vcl_string("bbox_cam_my.txt").c_str());
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
  bbout.close();



return;
}

void dbcri_process::read_WC()
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



void dbcri_process::Estimation_BB_shift_using_convex_hull_in_2D() {

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


vnl_double_4x4 dbcri_process::bb_reform(double x0, double y0, double z0, double x1, double y1, double z1)
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

int dbcri_process::points_inside_of_the_box(int index, vnl_double_4x4 bb_xform) {

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

void dbcri_process::statitical_filtering(double &r, vcl_vector <vgl_point_3d <double> > pts) {


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



  //vcl_sort(pts_z.begin(), pts_z.end(), dbcri_cmp_z);
  //vcl_vector<vnl_double_3> pts_x;

  // only consider points in x and y above 0.5 feet  //i don't use it..
  for ( vcl_vector<vnl_double_3>::const_iterator itr = pts_z.begin();
        itr != pts_z.end();  ++itr)
  {
    //if ((*itr)[2] > 0.5) { pts_x.push_back(*itr); }
    pts_y.push_back(*itr); 
  }
  
  //vcl_vector<vnl_double_3> pts_y = pts_x;

 // vcl_sort(pts_x.begin(), pts_x.end(), dbcri_cmp_x);
  vcl_sort(pts_y.begin(), pts_y.end(), dbcri_cmp_y);



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


void dbcri_process::spacial_filtering(double &r, vcl_vector <vgl_point_3d <double> > pts) {


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



  //vcl_sort(pts_z.begin(), pts_z.end(), dbcri_cmp_z);
  //vcl_vector<vnl_double_3> pts_x;

  // only consider points in x and y above 0.5 feet  //i don't use it..
  for ( vcl_vector<vnl_double_3>::const_iterator itr = pts_z.begin();
        itr != pts_z.end();  ++itr)
  {
    //if ((*itr)[2] > 0.5) { pts_x.push_back(*itr); }
    pts_y.push_back(*itr); 
  }
  
  //vcl_vector<vnl_double_3> pts_y = pts_x;

 // vcl_sort(pts_x.begin(), pts_x.end(), dbcri_cmp_x);
  vcl_sort(pts_y.begin(), pts_y.end(), dbcri_cmp_y);



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

   vcl_sort(sel_pts_y.begin(), sel_pts_y.end(), dbcri_cmp_x);

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



 // return;

}

double dbcri_process::bb_contour() {

 


return 1.0;
}



void dbcri_process::read_bb_and_probe(float scale, double &cube_x, double &cube_y, double &cube_z,
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

void dbcri_process::read_bb_and_probe_test(float scale, double &cube_x, double &cube_y, double &cube_z,  //test for same cube_size
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


double dbcri_process::Estimation_BB_and_bb_using_convex_hull_in2D(int some_index) {
  
  
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


double dbcri_process::bb_box_contour_measure(vcl_vector<vgl_point_3d<double> > points) {

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

double dbcri_process::val_list_check(vcl_vector <double> val_list)
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
  

      vcl_sort(prob_list.begin(), prob_list.end(), dbcri_sort);

  /////return mean/var;
      vcl_cout<<prob_list[0]-prob_list[4]<<vcl_endl;
      return (prob_list[0]-prob_list[4]);
}

void dbcri_process::draw_vsol(vcl_vector <vnl_matrix <double> >p8_max_list) {

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



void dbcri_process::rotate_bb( vnl_double_3x3 R_in, vnl_double_3x3 & R_out,double theta_x,double theta_y,double theta_z ) {

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



double dbcri_process::generate_mean_contour_model(int class_index) {



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



double dbcri_process::bb_box_contour_measure(vcl_vector< vcl_vector<vgl_point_3d<double> > >points) {

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


double dbcri_process::bb_box_contour_measure_clip(vcl_vector<vgl_point_3d<double> > points) {

  //polygon generation
  /*int frame=0;
  for(; (frame < input_data_.size()) && input_data_[frame][0]; ++frame) {
    int N=polys_[frame]->size();
  float*  x1 = (float *)malloc(sizeof(float)*N);
  float* y1 = (float *)malloc(sizeof(float)*N);


  }*/

  





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

