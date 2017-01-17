/*************************************************************************
 *    NAME: MingChing
 *    USER: mcchang
 *    FILE: dbcvr_curveaverage_process.cxx
 *    DATE: Tue Oct 29 10:49:27 2003
 *************************************************************************/
#include "dbcvr_curveaverage_process.h"

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>

/*************************************************************************
 * Function Name: dbcvr_curveaverage_process::dbcvr_curveaverage_process
 * Parameters: 
 * Effects: 
 *************************************************************************/

dbcvr_curveaverage_process::dbcvr_curveaverage_process()
{
  //The default is to average 2 curves.
  nframes_ = 2;

/*  if (!parameters()->add_parameter( "Output file < filename.cm >" , "-curvematchoutput" ,
                            bpro1_parameters::FILE_STRING , (vcl_string)"output.cm" ) ||
      !parameters()->add_parameter(  "Input file1 <filename...>" , "-curvematchinput1" ,
                            bpro1_parameters::FILE_STRING , (vcl_string)"input1.con" )  ||
      !parameters()->add_parameter(  "Input file2 <filename...>" , "-curvematchinput2" ,
                            bpro1_parameters::FILE_STRING , (vcl_string)"input2.con" ) ) {
    vcl_cerr << "ERROR: Adding parameters in dbcvr_curveaverage_process::dbcvr_curveaverage_process()" << vcl_endl;
  }*/
}

dbcvr_curveaverage_process::dbcvr_curveaverage_process (int nframes)
{
  //average curves in n frames.
  nframes_ = nframes;
}


//: Clone the process
bpro1_process*
dbcvr_curveaverage_process::clone() const
{
  return new dbcvr_curveaverage_process(*this);
}

/*************************************************************************
 * Function Name: dbcvr_curveaverage_process::execute
 * Parameters:
 * Returns: bool
 * Effects:
 *************************************************************************/
//Ming: curve1 in input_spatial_object_[0], build curve from polyline (digital curve)
//Ming: curve2 in input_spatial_object_[1]
bool dbcvr_curveaverage_process::execute()
{
  vcl_string input_file;
  parameters()->get_value( "-curvematchinput" , input_file );

  clear_output();

  dbcvr_cvmatch* curveMatch = new dbcvr_cvmatch();

  //loop through all storage curves ... and match current to the previous frame
/*    dbcvr_curve_storage_sptr input_curves;
  input_curves.vertical_cast(input_data_[0][0]);
  bsol_intrinsic_curve_2d_sptr curve1 = input_curves->get_curve();

  input_curves.vertical_cast(input_data_[1][0]);
  bsol_intrinsic_curve_2d_sptr curve2 = input_curves->get_curve();
*/
  vidpro1_vsol2D_storage_sptr input_vsol1;
  input_vsol1.vertical_cast(input_data_[0][0]);

  vidpro1_vsol2D_storage_sptr input_vsol2;
  input_vsol2.vertical_cast(input_data_[1][0]);

  // TODO: no open-close curve notion in vsol2D
  bsol_intrinsic_curve_2d_sptr curve1 = new bsol_intrinsic_curve_2d();
  bsol_intrinsic_curve_2d_sptr curve2 = new bsol_intrinsic_curve_2d();

  // The contour can either be a polyline producing an open contour 
  // or a polygon producing a close contour
  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol1->all_data();
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    if( vsol_list[b]->cast_to_curve())
    {
      if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
      {
        curve1->setOpen(true);
        for (unsigned int i=0; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i);
          curve1->add_vertex(pt->x(), pt->y());
    } } }

    if( vsol_list[b]->cast_to_region())
    {
      if( vsol_list[b]->cast_to_region()->cast_to_polygon() )
      {
        curve1->setOpen(false);
        for (unsigned int i=0; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
        {
          vsol_point_2d_sptr pt = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i);
          curve1->add_vertex(pt->x(), pt->y());
  } } } }

  vsol_list = input_vsol2->all_data();
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    if( vsol_list[b]->cast_to_curve())
    {
      if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
      {
        curve2->setOpen(true);
        for (unsigned int i=0; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i);
          curve2->add_vertex(pt->x(), pt->y());
    } } }

    if( vsol_list[b]->cast_to_region())
    {
      if( vsol_list[b]->cast_to_region()->cast_to_polygon() )
      {
        curve2->setOpen(false);
        for (unsigned int i=0; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
        {
          vsol_point_2d_sptr pt = vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i);
          curve2->add_vertex(pt->x(), pt->y());
  } } } }


  curve1->computeProperties();
  curve2->computeProperties();

  curveMatch->setCurve1 (curve1);
  curveMatch->setCurve2 (curve2);
  curveMatch->Match ();

  vcl_cout<< vcl_endl<<"curve_2d matching cost: "<< curveMatch->finalCost() <<vcl_endl;

  // create the output storage class
  dbcvr_curvematch_storage_sptr output_curvematch = dbcvr_curvematch_storage_new();
  output_data_[0].push_back (output_curvematch);
  output_curvematch->set_curvematch (curveMatch);

  return true;
}
