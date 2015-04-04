//:
// \file

#include "mw_project_polyline_process.h"

#include <vcl_vector.h>
#include <vcl_string.h>
#include <dbdif/algo/dbdif_data.h>
#include <vpgl/vpgl_perspective_camera.h>


#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dvpgl/pro/dvpgl_camera_storage.h>
#include <dvpgl/io/dvpgl_io_cameras.h>

#include <vsol/vsol_point_3d.h>
#include <vsol/vsol_point_3d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <mw/mw_util.h>

//: Constructor
mw_project_polyline_process::mw_project_polyline_process()
{
}


//: Destructor
mw_project_polyline_process::~mw_project_polyline_process()
{
}


//: Clone the process
bpro1_process*
mw_project_polyline_process::clone() const
{
  return new mw_project_polyline_process(*this);
}


//: Return the name of this process
vcl_string
mw_project_polyline_process::name()
{
  return "Project Polyline";
}


//: Return the number of input frame for this process
int
mw_project_polyline_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
mw_project_polyline_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > mw_project_polyline_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vpgl camera" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > mw_project_polyline_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Execute the process
bool
mw_project_polyline_process::execute()
{
  clear_output();

  // get camera matrix

  dvpgl_camera_storage_sptr cam_storage;

  cam_storage.vertical_cast(input_data_[0][0]);

  const vpgl_perspective_camera<double> *pcam = 
    dvpgl_cast_to_perspective_camera(cam_storage->get_camera());
  if(!pcam) {
    vcl_cerr << "Error: process requires a perspective camera" << vcl_endl;
    return false;
  }

  vcl_cout << "NAME: " << cam_storage->name() << vcl_endl;
  vcl_cout << "Camera: \n" << pcam->get_matrix();

  const vpgl_perspective_camera<double> &cam = *pcam;

  // Read the file from matlab

  vcl_vector<vgl_point_3d<double> > pts3d;

  // Documentation: 
  //
  // - Note that there are pre-stored my3dcurves for different datasets inside
  // the dat directory.
  //
  // - One way to get the 3D curve is to run the tracer, press 'y', and copy or
  // link the 3D curve over to the name my3dcurve.dat
  //
  // - sample matlab session to generate long curves into my3dcurve.dat inside
  // mw/app from a curve sketch reconstruction opened with a file->open
  //
  // Example curve sketches at /vision/video/rfabbri/capitol/manual-results
  //
  // get_recs_from_fig
  // for i=1:270; mylen(i)=max(size(recs{i})); end
  // mylen
  // max(mylen)
  // find(mylen >= 300)
  // cplot(recs{88},'r');
  // cplot(recs{89},'r');
  // find(mylen >= 200)
  // cplot(recs{6},'r');
  // cplot(recs{88},'r');
  // cplot(recs{153},'r');
  // b=recs{153}';
  // mywrite('my3dcurve.dat',b(:));
  // pwd
  // cd ..
  // pwd
  // mywrite('my3dcurve.dat',b(:));
  // cplot(recs{157},'r');
  // cplot(recs{204},'r');
  // cplot(recs{205},'r');
  // find(mylen >= 150 && mylen < 200)
  // find(mylen >= 150 & mylen < 200)
  // cplot(recs{73},'g');
  // cplot(recs{87},'g');
  // cplot(recs{149},'g');
  // b=recs{149}';
  // mywrite('my3dcurve.dat',b(:));
  // cplot(recs{156},'g');
  // cplot(recs{177},'g');
  // b=recs{177}';
  // mywrite('my3dcurve.dat',b(:));
  // b=recs{203}';
  // cplot(recs{203},'g');
  // find(mylen >= 100 & mylen < 150)
  // figure
  // hist(mylen)
  // hist(mylen,20)
  // cplot(recs{61},'ro');
  // cplot(recs{63},'ro');
  // cplot(recs{77},'ro');
  // cplot(recs{99},'ro');
  // cplot(recs{195},'ro');
  // b=recs{195}';
  // mywrite('my3dcurve.dat',b(:));
  // cplot(recs{196},'ro');
  // b=recs{196}';
  // mywrite('my3dcurve.dat',b(:));
  // cplot(recs{201},'go');
  // cplot(recs{262},'go');

  bool retval= myreadv("my3dcurve.dat", pts3d);
  if (!retval)
  {
    vcl_cerr << "Problem: couldn't find file my3dcurve.dat in the current directory\n";
    return false;
  }

  vcl_vector<vsol_point_2d_sptr> pts(pts3d.size());
  for (unsigned i=0; i < pts3d.size(); ++i)
    pts[i] = new vsol_point_2d(vgl_point_2d<double>( cam.project(pts3d[i]) ));

  vcl_vector<vsol_spatial_object_2d_sptr> poly;
  poly.push_back(new vsol_polyline_2d (pts));

  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(poly, "polylines");
  output_data_[0].push_back(output_vsol);

  return true;
}

bool
mw_project_polyline_process::finish()
{
  return true;
}

