#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <dbdet/pro/dbdet_mask_edges_process.h>
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
//#include <vil/vil_print.h>

dbdet_mask_edges_process::dbdet_mask_edges_process() : bpro1_process()
{
  if(!parameters()->add( "Use image? (otherwise use rectangular polygon (vsol2D))" , "-image" ,  false ) ||
     !parameters()->add( "(if use rectangle) Add the rectangle to the output vsol?" , "-addrect" ,  true ) 
     )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
  
}


//: Clone the process
bpro1_process*
dbdet_mask_edges_process::clone() const
{
  return new dbdet_mask_edges_process(*this);
}


vcl_vector< vcl_string > dbdet_mask_edges_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;

  to_return.push_back( "vsol2D" );
  to_return.push_back( "vsol2D" );
  to_return.push_back( "image" );
  return to_return;
}

vcl_vector< vcl_string > dbdet_mask_edges_process::get_output_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}

bool dbdet_mask_edges_process::valid_point(vsol_point_2d* point) {
  return valid_point(float(point->x()), float(point->y()));
}
bool dbdet_mask_edges_process::valid_point(vsol_point_2d_sptr point) {
  return valid_point(float(point->x()), float(point->y()));
}
bool dbdet_mask_edges_process::valid_point(float yy, float xx) {
  int x = int(vcl_floor(xx+0.5));
  int y = int(vcl_floor(yy+0.5));
  // vxl_byte val = mask(y,x);
  if (!mask(y,x) || (y > 0 && !mask(y-1,x)) || 
    (x > 0 && !mask(y,x-1)) || 
    (x > 0 && y > 0 && !mask(y-1, x-1)) ||
    (y+1 < ni && !mask(y+1,x)) || 
    (x+1 < nj && !mask(y,x+1)) || 
    (y+1 < ni && x+1 < nj && !mask(y+1, x+1)) ||
    (y > 1 && !mask(y-2,x)) || 
    (x > 1 && !mask(y,x-2)) || 
    (x > 1 && y > 1 && !mask(y-2, x-2)) ||
    (y+2 < ni && !mask(y+2,x)) || 
    (x+2 < nj && !mask(y,x+2)) || 
    (y+2 < ni && x+2 < nj && !mask(y+2, x+2)))
    return true;
  else
    return false;
}

bool dbdet_mask_edges_process::execute()
{
  // create the input storage class
  vidpro1_vsol2D_storage_sptr input_vsol;
  input_vsol.vertical_cast(input_data_[0][0]);

  bool use_image;
  parameters()->get_value("-image", use_image);
  bool add_rect;
  parameters()->get_value("-addrect", add_rect);

  // new vector to store the points
  vcl_vector< vsol_spatial_object_2d_sptr > contours;

  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol->all_data();
  
  if (use_image) {
  //  vil_image_view<vxl_byte> mask; int ni, nj;
    vidpro1_image_storage_sptr frame_image;
    frame_image.vertical_cast(input_data_[0][2]);
    vil_image_resource_sptr image_sptr = frame_image->get_image();
    mask = vil_convert_to_grey_using_average(image_sptr->get_view());
    ni = int(mask.ni());
    nj = int(mask.nj());  
    
    for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) 
    {
      //POINT
      if( vsol_list[b]->cast_to_point() ) {
        if (valid_point(vsol_list[b]->cast_to_point()))
          contours.push_back(vsol_list[b]);
      }
      else if( vsol_list[b]->cast_to_curve())
      {
        //LINE
        if( vsol_list[b]->cast_to_curve()->cast_to_line() )
        {
          vsol_point_2d_sptr p0 = vsol_list[b]->cast_to_curve()->cast_to_line()->p0();
          vsol_point_2d_sptr p1 = vsol_list[b]->cast_to_curve()->cast_to_line()->p1();
          if (valid_point(p0) || valid_point(p1))
            contours.push_back(vsol_list[b]);
        }
        //POLYLINE
        else if( vsol_list[b]->cast_to_curve()->cast_to_polyline() ) {
          vsol_polyline_2d* pline = vsol_list[b]->cast_to_curve()->cast_to_polyline();
          for (unsigned int i = 0; i<pline->size(); i++) 
            if (valid_point(pline->vertex(i))) {
              contours.push_back(vsol_list[b]);
              break;
            }
        }
        // CIRCULAR ARC
        else if (vsol_list[b]->cast_to_curve()->cast_to_conic())
        {
          vsol_conic_2d* conic = vsol_list[b]->cast_to_curve()->cast_to_conic();
          if (conic->is_real_circle())
          {
            if (valid_point(conic->p0()) || 
                valid_point(conic->p1()) ||
                valid_point(conic->midpoint())) 
                contours.push_back(vsol_list[b]);
          }
        }
      }
      else if( vsol_list[b]->cast_to_region())
      {
        //POLYGON
        if( vsol_list[b]->cast_to_region()->cast_to_polygon() ) {
          vsol_polygon_2d* poly = vsol_list[b]->cast_to_region()->cast_to_polygon();
          for (unsigned int i = 0; i<poly->size(); i++)
            if (valid_point(poly->vertex(i))) {
              contours.push_back(vsol_list[b]);
              break;
            }
        }
      }
    }
  } else {

    vidpro1_vsol2D_storage_sptr input_vsol2;
    input_vsol2.vertical_cast(input_data_[0][1]);
    vcl_vector< vsol_spatial_object_2d_sptr > rect = input_vsol2->all_data();
    if (rect.size() != 1) {
      vcl_cout << "input vsol contains more than one contours!! Which one is the rectangle!! Please create a vsol storage which contains a single rectangular polygon that signifies the region of interest!\n";
      return false;
    }
    
    if (!rect[0]->cast_to_region() || !(rect[0]->cast_to_region()->cast_to_polygon()) ) {
      vcl_cout << "input vsol does not contain a polygon (rectangle)!! Please create a vsol storage which contains a single rectangular polygon that signifies the region of interest!\n";
      return false;
    }

    vsol_polygon_2d* poly = rect[0]->cast_to_region()->cast_to_polygon();
    if (add_rect)
      contours.push_back(rect[0]);

    vgl_polygon<double> pp = bsol_algs::vgl_from_poly(poly);

    for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) 
    {
      //POINT
      if( vsol_list[b]->cast_to_point() ) {
        if (pp.contains(vsol_list[b]->cast_to_point()->get_p()))
          contours.push_back(vsol_list[b]);
      }
      else if( vsol_list[b]->cast_to_curve())
      {
        //LINE
        if( vsol_list[b]->cast_to_curve()->cast_to_line() )
        {
          vsol_point_2d_sptr p0 = vsol_list[b]->cast_to_curve()->cast_to_line()->p0();
          vsol_point_2d_sptr p1 = vsol_list[b]->cast_to_curve()->cast_to_line()->p1();
          if (pp.contains(p0->get_p()) && pp.contains(p1->get_p()))
            contours.push_back(vsol_list[b]);
        }
        //POLYLINE
        else if( vsol_list[b]->cast_to_curve()->cast_to_polyline() ) {
          vsol_polyline_2d* pline = vsol_list[b]->cast_to_curve()->cast_to_polyline();
          bool contains = true;
          for (unsigned int i = 0; i<pline->size(); i++) 
            if (!pp.contains(pline->vertex(i)->get_p())) {
              contains = false;
              break;
            }
          if (contains)
            contours.push_back(vsol_list[b]);
        }
        // CIRCULAR ARC
        else if (vsol_list[b]->cast_to_curve()->cast_to_conic())
        {
          vsol_conic_2d* conic = vsol_list[b]->cast_to_curve()->cast_to_conic();
          if (conic->is_real_circle())
          {
            if (pp.contains(conic->p0()->get_p()) && 
                pp.contains(conic->p1()->get_p()) &&
                pp.contains(conic->midpoint()->get_p()) ) 
                contours.push_back(vsol_list[b]);
          }
        }
      }
      else if( vsol_list[b]->cast_to_region())
      {
        //POLYGON
        if( vsol_list[b]->cast_to_region()->cast_to_polygon() ) {
          vsol_polygon_2d* poly = vsol_list[b]->cast_to_region()->cast_to_polygon();
          bool contains = true;
          for (unsigned int i = 0; i<poly->size(); i++)
            if (pp.contains(poly->vertex(i)->get_p())) {
              contains = false;
              break;
            }
          if (contains)
            contours.push_back(vsol_list[b]);
        }
      }
    }


  }

  vcl_cout << "Pruned " << input_vsol->all_data().size()-contours.size() << " edges out of " << input_vsol->all_data().size() << " edges\n"; 
  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(contours, "pruned");
  output_data_[0].push_back(output_vsol);

  return true;
}
