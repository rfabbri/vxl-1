#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <dbdet/pro/dbdet_prune_curves_process.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_point_2d.h>

#include <bbas/bsol/bsol_algs.h>

#include <vil/vil_convert.h>
#include <bil/algo/bil_color_conversions.h>
#include <dbsol/algo/dbsol_img_curve_algs.h>
#include <dbsol/algo/dbsol_curve_algs.h>
#include <dbsol/dbsol_interp_curve_2d.h>


dbdet_prune_curves_process::dbdet_prune_curves_process() : bpro1_process()
{
  if(!parameters()->add( "color threshold: " , "-color_thres" ,  0.4f ) ||
     !parameters()->add( "color gamma: " , "-color_gamma" ,  14.0f ) ||
     !parameters()->add( "intensity gamma: " , "-intensity_gamma" ,  15.0f ) ||
     !parameters()->add( "region width (pixels): " , "-region_width" ,  5.0f ) 
     )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
  
}


//: Clone the process
bpro1_process*
dbdet_prune_curves_process::clone() const
{
  return new dbdet_prune_curves_process(*this);
}


vcl_vector< vcl_string > dbdet_prune_curves_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;

  to_return.push_back( "vsol2D" );
  to_return.push_back( "image" );
  return to_return;
}

vcl_vector< vcl_string > dbdet_prune_curves_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}

bool dbdet_prune_curves_process::execute()
{
  // create the input storage class
  vidpro1_vsol2D_storage_sptr input_vsol;
  input_vsol.vertical_cast(input_data_[0][0]);

  float color_threshold, color_gamma, region_width, intensity_gamma;
  parameters()->get_value("-color_thres", color_threshold);
  parameters()->get_value("-color_gamma", color_gamma);
  parameters()->get_value("-intensity_gamma", intensity_gamma);
  parameters()->get_value("-region_width", region_width);

  // new vector to store the points
  vcl_vector< vsol_spatial_object_2d_sptr > contours;

  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol->all_data();
  
  vidpro1_image_storage_sptr frame_image;
  frame_image.vertical_cast(input_data_[0][1]);
  vil_image_resource_sptr image_sptr = frame_image->get_image();
  vil_image_view<vxl_byte> img = image_sptr->get_view();
  bool LAB = false;
  if( img.nplanes() != 3 ) {
    vcl_cout << "image is not colored, using intensity differences!!!\n"; 
  } else {
    LAB = true;
    convert_RGB_to_Lab(img, L_, A_, B_);
  }
  
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) 
  {
    vcl_vector<vsol_point_2d_sptr> pts;
    bool closed = false;
    //POINT
    if( vsol_list[b]->cast_to_point() ) {
      continue;   // remove the points!!
    }
    else if( vsol_list[b]->cast_to_curve())
    {
      //LINE
      if( vsol_list[b]->cast_to_curve()->cast_to_line() )
      {
        vsol_point_2d_sptr p0 = vsol_list[b]->cast_to_curve()->cast_to_line()->p0();
        vsol_point_2d_sptr p1 = vsol_list[b]->cast_to_curve()->cast_to_line()->p1();
        pts.push_back(p0);
        pts.push_back(p1);
      }
      //POLYLINE
      else if( vsol_list[b]->cast_to_curve()->cast_to_polyline() ) {
        vsol_polyline_2d* pline = vsol_list[b]->cast_to_curve()->cast_to_polyline();
        for (unsigned int i = 0; i<pline->size(); i++) 
          pts.push_back(pline->vertex(i));
      }
      // CIRCULAR ARC
      else if (vsol_list[b]->cast_to_curve()->cast_to_conic())
      {
        vcl_cout << "CAUTION: This vsol member is a circular ARC and this process is NOT HANDLING circular arcs!!! Skipping it!\n";
        continue;
      }
    }
    else if( vsol_list[b]->cast_to_region())
    {
      //POLYGON
      if( vsol_list[b]->cast_to_region()->cast_to_polygon() ) {
        vsol_polygon_2d* poly = vsol_list[b]->cast_to_region()->cast_to_polygon();
        closed = true;
        for (unsigned int i = 0; i<poly->size(); i++)
          pts.push_back(poly->vertex(i));
      }
    }
    dbsol_interp_curve_2d_sptr c = new dbsol_interp_curve_2d();
    dbsol_curve_algs::interpolate_linear(c.ptr(), pts, closed);
    double dist = 1000000.0;
    if (LAB)
      dist = get_color_distance_of_curve_regions(c, region_width, L_, A_, B_, color_gamma);
    else
      dist = get_intensity_distance_of_curve_regions(c, region_width, img, intensity_gamma);
    if (dist < color_threshold)
      contours.push_back(vsol_list[b]);
  }

  vcl_cout << "Pruned " << input_vsol->all_data().size()-contours.size() << " curves out of " << input_vsol->all_data().size() << " curves\n"; 
  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(contours, "pruned");
  output_data_[0].push_back(output_vsol);

  return true;
}
