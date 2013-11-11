// This is brcv/seg/dbkpr/pro/dbdet_combine_curves_process.cxx

//:
// \file

#include "dbdet_combine_curves_process.h"

#include <bpro1/bpro1_parameters.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
//#include <dbdet/pro/dbdet_edgemap_storage.h>
//#include <dbdet/pro/dbdet_edgemap_storage_sptr.h>

#include <vsol/vsol_line_2d_sptr.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>





//: Constructor
dbdet_combine_curves_process::dbdet_combine_curves_process()
{
}


//: Destructor
dbdet_combine_curves_process::~dbdet_combine_curves_process()
{
}


//: Clone the process
bpro1_process*
dbdet_combine_curves_process::clone() const
{
  return new dbdet_combine_curves_process(*this);
}


//: Return the name of this process
vcl_string
dbdet_combine_curves_process::name()
{
  return "Combine VSOL from Frames";
}


//: Return the number of input frame for this process
int
dbdet_combine_curves_process::input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int
dbdet_combine_curves_process::output_frames()
{
  return 0;
}


//: Provide a vector of required input types
vcl_vector< vcl_string > dbdet_combine_curves_process::get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "vsol2D" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbdet_combine_curves_process::get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back( "vsol2D" );


  return to_return;
}


//: Execute the process
bool
dbdet_combine_curves_process::execute()
{
  return true;
}

bool
dbdet_combine_curves_process::finish()
{

  // new vector to store the resulting contours
  vcl_vector< vsol_spatial_object_2d_sptr > contours;



  // cast the storage classes
  for(unsigned int i=0; i<input_data_.size(); ++i) {
    vidpro1_vsol2D_storage_sptr input_vsol;

    input_vsol.vertical_cast(input_data_[i][0]);

    vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol->all_data();

    for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) 
    {
      vcl_vector<vsol_point_2d_sptr> pts; //:< not really used right now
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
      contours.push_back(vsol_list[b]);
    }
  }


  for (unsigned i=0; i < input_data_.size(); ++i) {
    // create the output storage class
    vidpro1_vsol2D_storage_sptr output_vsol = vidpro1_vsol2D_storage_new();
    output_vsol->add_objects(contours, "combined");
    output_data_[i].push_back(output_vsol);
  }


  return true;
}
