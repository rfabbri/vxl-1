
#include "dbru_show_tps_process.h"

#include<bpro1/bpro1_parameters.h>
#include<vsol/vsol_point_2d.h>
#include<vsol/vsol_polyline_2d_sptr.h>
#include<vsol/vsol_polyline_2d.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include<vidpro1/storage/vidpro1_vsol2D_storage.h>

//#include <vil/vil_image_view.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>

#include <vil/vil_image_resource_sptr.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_new.h>
#include <vil/vil_image_view.h>
#include <vil/vil_convert.h>
#include <vul/vul_sprintf.h>

#include <vgl/vgl_distance.h>

#include <mbl/mbl_thin_plate_spline_2d.h>

//: Constructor
dbru_show_tps_process::dbru_show_tps_process(void): bpro1_process()
    {

    if( //!parameters()->add( "Initial temperature " , "-initT" , (float)1) ||
        !parameters()->add( "grid delta_s" , "-delta" , (float) 1.0f )||
        !parameters()->add( "grid margin" , "-margin" , (float) 200.0f )||
        !parameters()->add( "sampling delta_s" , "-sdelta" , (float) 0.1f )||
        !parameters()->add( "Input correspondence points file <filename...>" , 
        "-corfile" , bpro1_filepath("","*.out") ) 
        ) 
        {
            vcl_cerr << "ERROR: Adding parameters in dbru_show_tps_process::dbru_show_tps_process()" << vcl_endl;
        }
    
    }
//: Destructor
dbru_show_tps_process::~dbru_show_tps_process()
    {
    }
//: Return the name of this process
vcl_string
dbru_show_tps_process::name()
    {
        return "Show TPS";
    }
//: Return the number of input frame for this process
int
dbru_show_tps_process::input_frames()
    {
        return 1;
    }
//: Return the number of output frames for this process
int
dbru_show_tps_process::output_frames()
    {
        return 1;
    }
//: Provide a vector of required input types
vcl_vector< vcl_string > dbru_show_tps_process::get_input_type()
    {
    vcl_vector< vcl_string > to_return;
    //to_return.push_back( "dbrl_id_point_2d" );
    return to_return;
    }


//: Provide a vector of output types
vcl_vector< vcl_string > dbru_show_tps_process::get_output_type()
    {  
    vcl_vector<vcl_string > to_return;
    //to_return.push_back( "image" );
    //to_return.push_back( "dbrl_match_set" );
    //to_return.push_back( "dbrl_match_set" );
    to_return.push_back( "vsol2D" );  // template
    to_return.push_back( "vsol2D" );  // target
    to_return.push_back( "vsol2D" );  // grid
    //to_return.push_back( "vsol2D" );
    return to_return;
    }


//: Execute the process
bool
dbru_show_tps_process::execute()
{

  bpro1_filepath input_path;
  parameters()->get_value( "-corfile" , input_path);
  vcl_string cor_file = input_path.path;

  float delta=0, delta_s=0, margin=0;
  parameters()->get_value( "-delta" , delta);
  parameters()->get_value( "-margin" , margin);
  parameters()->get_value( "-sdelta" , delta_s);
  
  vcl_ifstream ifs(cor_file.c_str());
  if (!ifs) {
    vcl_cout << "file not found!\n";
    return false;
  }
  // read points
  vcl_vector<vsol_spatial_object_2d_sptr> points1, points2, grid2;
  
  int size1, size2;
  ifs >> size1;
  float minx = 1000000, miny = 1000000, maxx = 0, maxy = 0;
  float minx2 = 1000000, miny2 = 1000000, maxx2 = 0, maxy2 = 0;
  for (int i = 0; i<size1; i++) {
    float x, y;
    ifs >> x; ifs >> y;
    if (x < minx)
      minx = x;
    if (y < miny)
      miny = y;
    if (x > maxx)
      maxx = x;
    if (y > maxy)
      maxy = y;

    vsol_point_2d_sptr pt = new vsol_point_2d(double(x), double(y));
    points1.push_back(pt->cast_to_spatial_object());
  }
  ifs >> size2;
  assert(size1 == size2);
  for (int i = 0; i<size2; i++) {
    float x, y;
    ifs >> x; ifs >> y;

    if (x < minx2)
      minx2 = x;
    if (y < miny2)
      miny2 = y;
    if (x > maxx2)
      maxx2 = x;
    if (y > maxy2)
      maxy2 = y;

    vsol_point_2d_sptr pt = new vsol_point_2d(double(x), double(y));
    points2.push_back(pt->cast_to_spatial_object());
  }
  ifs.close();
  float xrange = maxx-minx+2*margin;
  float yrange = maxy-miny+2*margin;
  float xrange2 = maxx2-minx2+2*margin;
  float yrange2 = maxy2-miny2+2*margin;

  float minxr = minx-margin;
  float minyr = miny-margin;
  float minxr2 = minx2-margin;
  float minyr2 = miny2-margin;
  vcl_vector<vgl_point_2d<double> > src_pts(size1), dest_pts(size2);
  for (int i = 0; i<size1; i++) {
    double x = (points1[i]->cast_to_point()->x()-minxr)/xrange;
    double y = (points1[i]->cast_to_point()->y()-minyr)/yrange;
    vgl_point_2d<double> p(x,y);
    vcl_cout << "src " << i << ": " << p << vcl_endl;
    src_pts.push_back(p);
  }
  for (int i = 0; i<size2; i++) {
    double x = (points2[i]->cast_to_point()->x()-minxr2)/xrange2;
    double y = (points2[i]->cast_to_point()->y()-minyr2)/yrange2;
    vgl_point_2d<double> p(x,y);
    vcl_cout << "src " << i << ": " << p << vcl_endl;
    dest_pts.push_back(p);
  }

  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(points1, "template");

  vidpro1_vsol2D_storage_sptr output_vsol2 = vidpro1_vsol2D_storage_new();
  output_vsol2->add_objects(points2, "target");

  output_data_[0].push_back(output_vsol);
  output_data_[0].push_back(output_vsol2);

  // find the transformation
  mbl_thin_plate_spline_2d tps;
  tps.build(src_pts,dest_pts);

  // validate this warp
  for (unsigned i = 0 ; i < src_pts.size(); i++) {
    vgl_point_2d<double> p = tps(src_pts[i]);
    double dist = vgl_distance(p,dest_pts[i]);
    vcl_cout << "src: " << src_pts[i] << " dest: " << dest_pts[i] << " warped dest: " << p << " dist: " << dist << vcl_endl;
    if (vcl_abs(dist) > 1e-2)
      vcl_cout << "!!!!!!!!!!!!!!!!DIST IS NOT OK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n";
    else 
      vcl_cout << " dist is ok!\n";
  }

  // create grid1 and grid2
  //double minx_n = minx - margin;
  //double miny_n = miny - margin;
  //double maxx_n = maxx + margin;
  //double maxy_n = maxy + margin;

  for (double y = minyr; y < minyr+yrange; y+=delta) {
    vsol_polyline_2d_sptr line = new vsol_polyline_2d();
    for (double x = minxr; x < minxr+xrange; x+=delta_s) {
      //vcl_cout << "x: " << x << ", y: " << y;
      vgl_point_2d<double> p = tps((x-minxr)/xrange,(y-minyr)/yrange);
      //vcl_cout << " tps x: " << p.x() << ", y: " << p.y() << vcl_endl;
      vsol_point_2d_sptr new_p = new vsol_point_2d( (p.x()*xrange+minxr), (p.y()*yrange+minyr));
      line->add_vertex(new_p);
    }
    grid2.push_back(line->cast_to_spatial_object());
  }

  for (double x = minxr; x <minxr+xrange; x+=delta) {
    vsol_polyline_2d_sptr line = new vsol_polyline_2d();
    for (double y = minyr; y <minyr+yrange; y+=delta_s) {
      //vcl_cout << "x: " << x << ", y: " << y;
      vgl_point_2d<double> p = tps((x-minxr)/xrange,(y-minyr)/yrange);
      //vcl_cout << " tps x: " << p.x() << ", y: " << p.y() << vcl_endl;
      vsol_point_2d_sptr new_p = new vsol_point_2d( (p.x()*xrange+minxr), (p.y()*yrange+minyr));
      line->add_vertex(new_p);
    }
    grid2.push_back(line->cast_to_spatial_object());
  }
  
  vidpro1_vsol2D_storage_sptr output_vsol3 = vidpro1_vsol2D_storage_new();
  output_vsol2->add_objects(grid2, "target");

  output_data_[0].push_back(output_vsol3);
  

  return true;
}
//: Clone the process
bpro1_process*
dbru_show_tps_process::clone() const
    {
    return new dbru_show_tps_process(*this);
    }

bool
dbru_show_tps_process::finish()
{
  return true;
}
