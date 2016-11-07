
//:
// \file

#include "dbmcs_nearest_edgels_process.h"
#include <dbdif/dbdif_analytic.h>
#include <dbdif/algo/dbdif_data.h>

#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>

#include <dbdif/pro/dbdif_3rd_order_geometry_storage.h>

#include <vcl_vector.h>
#include <vcl_string.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <dbdet/edge/dbdet_edgemap.h>
#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/pro/dbdet_edgemap_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>
#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vil/vil_image_view.h>
#include <vil/vil_image_resource.h>
#include <vcl_limits.h>

struct mynearest {

  mynearest(
    const dbdet_edgemap_sptr &em, 
    const vil_image_view<vxl_uint_32> &dt,
    const vil_image_view<unsigned> &label
      )
    :
      em_(em),
      dt_(dt),
      label_(label)
  {}


  dbdet_edgemap_sptr em_; 
  vil_image_view<vxl_uint_32> dt_;
  vil_image_view<unsigned> label_;

  bool
  lookup_nearest(double px, double py, unsigned *id)
  {
    unsigned p_i = static_cast<unsigned>(px+0.5);
    unsigned p_j = static_cast<unsigned>(py+0.5);

    if (!label_.in_range(p_i, p_j)) {
      return false;
    }

    double min_d = vcl_numeric_limits<double>::infinity();
    unsigned l = label_(p_i, p_j);
    const vcl_vector<dbdet_edgel*> &ev = em_->edge_cells.begin()[l];
    for (unsigned i=0; i < ev.size(); ++i) {
      // form a vector d betwen pt and ev
      // project this vcl_vector onto the normal; this is our residual
      double dx = px - ev[i]->pt.x();
      double dy = py - ev[i]->pt.y();

      if (dx*dx + dy*dy < min_d) {
        min_d = dx*dx + dy*dy;
        *id = ev[i]->id;
      }
    }
    return true;
  }

  void 
  nearest_edgels(
    const vcl_vector< vcl_vector<vsol_point_2d_sptr> > &contours,
    vcl_vector<unsigned> *ptr_nearest)
  {
    vcl_vector<unsigned> &nearest = *ptr_nearest;

    for (unsigned i=0; i < contours.size(); ++i)
      for (unsigned k=0; k < contours[i].size(); ++k) {
        unsigned id;
        bool valid = lookup_nearest(contours[i][k]->x(), contours[i][k]->y(), &id);
        if (valid) 
          nearest.push_back(id);
      }
  }
};

//: Constructor
dbmcs_nearest_edgels_process::dbmcs_nearest_edgels_process()
{
//  if( 
//      !parameters()->add( "   perturb tangents (deg)" , "-dtan"     , 10.0)
//      ) {
//    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
//  }
}


//: Destructor
dbmcs_nearest_edgels_process::~dbmcs_nearest_edgels_process()
{
}


//: Clone the process
bpro1_process*
dbmcs_nearest_edgels_process::clone() const
{
  return new dbmcs_nearest_edgels_process(*this);
}


//: Return the name of this process
vcl_string
dbmcs_nearest_edgels_process::name()
{
  return "Nearest Edgels to Polyline";
}


//: Return the number of input frame for this process
int
dbmcs_nearest_edgels_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbmcs_nearest_edgels_process::output_frames()
{
  return 1;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbmcs_nearest_edgels_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" ); // curves
  to_return.push_back( "edge_map" );
  to_return.push_back( "image" ); // DT
  to_return.push_back( "image" ); // Label
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbmcs_nearest_edgels_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" ); // edgels
  return to_return;
}


//: Execute the process
bool
dbmcs_nearest_edgels_process::execute()
{
  clear_output();


  // Load polylines.
  // new vector to store the resulting contours
  vcl_vector< vcl_vector<vsol_point_2d_sptr> > contours;

  // cast the storage classes
  vidpro1_vsol2D_storage_sptr input_vsol;
  input_vsol.vertical_cast(input_data_[0][0]);

  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol->all_data();

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
    contours.push_back(pts);
  }

  // Load edgemap
  dbdet_edgemap_storage_sptr input_edgels;
  input_edgels.vertical_cast(input_data_[0][1]);

  // Load distmap & label
  vidpro1_image_storage_sptr input_dt;
  input_dt.vertical_cast(input_data_[0][2]);

  vidpro1_image_storage_sptr input_label;
  input_label.vertical_cast(input_data_[0][3]);

  // Locate nearest edgels and add them to edgels list

  vcl_vector<unsigned> nearest;
  mynearest NE(input_edgels->get_edgemap(), input_dt->get_image()->get_view(), input_label->get_image()->get_view());
  NE.nearest_edgels(contours, &nearest);

  // transform edgels into lines.

  vcl_vector< vsol_spatial_object_2d_sptr > edgel_lines;
//  const double scale=0.5;

  for (unsigned i=0; i < nearest.size(); ++i ) {
    dbdet_edgel *e = input_edgels->get_edgemap()->edgels[nearest[i]];

    vsol_line_2d_sptr newLine = new vsol_line_2d(vgl_vector_2d<double>(vcl_cos(e->tangent), vcl_sin(e->tangent)), 
        e->pt);

//    newLine->set_length(newLine->length()*scale);
    edgel_lines.push_back(newLine->cast_to_spatial_object());
  }

  // create the output storage class
  vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
  output_vsol->add_objects(edgel_lines, "edgels");
  output_data_[0].push_back(output_vsol);

  return true;
}

bool
dbmcs_nearest_edgels_process::finish()
{
  return true;
}

