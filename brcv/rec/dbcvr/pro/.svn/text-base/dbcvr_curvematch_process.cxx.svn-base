/*************************************************************************
*    NAME: MingChing
*    USER: mcchang
*    FILE: dbcvr_curvematch_process.cxx
*    DATE: Tue Oct 29 10:49:27 2003
*************************************************************************/

/*************************************************************************
*    Modiefied to add closed curve matching
*    NAME: Ozge Can Ozcanli
*    USER: ozge@lems.brown.edu
*    FILE: dbcvr_curvematch_process.cxx
*    DATE: Tue Oct 12 2004
*************************************************************************/


#include "dbcvr_curvematch_process.h"

#include <vcl_ctime.h>
#include <vcl_algorithm.h>
#include <vcl_cstdio.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <dbsol/dbsol_file_io.h>

/*************************************************************************
* Function Name: dbcvr_curvematch_process::dbcvr_curvematch_process
* Parameters: 
* Effects: 
*************************************************************************/

dbcvr_curvematch_process::dbcvr_curvematch_process()
{  
  if (//!parameters()->add( "With cost: sqrt((ds1)^2+(ds2)^2-(ds1*ds2*cos|dtheta1-dtheta2|))?" , "-normstretch" , false )  ||    
    !parameters()->add( "Load curve graph 1 from con file?" , "-load1" , (bool) false ) ||
    !parameters()->add( "Input con file 1 <filename...>" , 
    "-con1" ,
    bpro1_filepath("","*.con") 
    ) ||
    !parameters()->add( "Load curve graph 2 from con file?" , "-load2" , (bool) false ) ||
    !parameters()->add( "Input con file 2 <filename...>" , 
    "-con2" ,
    bpro1_filepath("","*.con") 
    ) ||
    !parameters()->add( "Do closed curve matching" , "-closedversion" , false )  ||
    !parameters()->add( "Template size: " , "-template_size" , 3 )  ||
    !parameters()->add( "R:" , "-r1" , 10.0f ) ||
    !parameters()->add( "restricted curve match ratio: ", "-ret", 0.25f ) ||
    !parameters()->add( "rms for line fitting before matching: ", "-rms", 0.05f) ) {
      vcl_cerr << "ERROR: Adding parameters in dbcvr_curvematch_process::dbcvr_curvematch_process()" << vcl_endl;
  }
}


//: Clone the process
bpro1_process*
dbcvr_curvematch_process::clone() const
{
  return new dbcvr_curvematch_process(*this);
}

/*************************************************************************
* Function Name: dbcvr_curvematch_process::execute
* Parameters:
* Returns: bool
* Effects:
*************************************************************************/
//Ming: curve1 in input_spatial_object_[0], build curve from polyline (digital curve)
//Ming: curve2 in input_spatial_object_[1]
bool dbcvr_curvematch_process::execute()
{
  bool load1=false; parameters()->get_value( "-load1" , load1 );
  bool load2=false; parameters()->get_value( "-load2" , load2 );
  bool closed_version; parameters()->get_value( "-closedversion" , closed_version );
  int template_size=0; parameters()->get_value( "-template_size" , template_size );
  float r1=0; parameters()->get_value( "-r1" , r1 );
  // if restricted curve match ratio is very small its close to open curve matching, 
  // if its 1, its fully closed curve matching
  float restricted_cvmatch_ratio=0; parameters()->get_value( "-ret", restricted_cvmatch_ratio);
  float rms=0; parameters()->get_value( "-rms", rms);
  bpro1_filepath input_path; parameters()->get_value( "-con1" , input_path);
  vcl_string con_file1 = input_path.path;
  parameters()->get_value( "-con2" , input_path);
  vcl_string con_file2 = input_path.path;  
  clear_output();

  //----------------------------------
  // get input vsol (two polygons)
  //----------------------------------
  vsol_polygon_2d_sptr poly1;
  vidpro1_vsol2D_storage_sptr input_vsol1;
  vsol_spatial_object_2d_sptr out1;

  if(load1){
    out1 = dbsol_load_con_file(con_file1.c_str());

//    poly1 = out1->cast_to_region()->cast_to_polygon();
  }
  else{
    input_vsol1.vertical_cast(input_data_[0][0]);
    // The contour needs to be a polygon
    {
      const vcl_vector< vsol_spatial_object_2d_sptr >& vsol_list = input_vsol1->all_data();
      poly1 = vsol_list[0]->cast_to_region()->cast_to_polygon();
    }
  }
  //the second polygon
  vsol_polygon_2d_sptr poly2;
  vidpro1_vsol2D_storage_sptr input_vsol2;
  vsol_spatial_object_2d_sptr out2;
  if(load2){
    out2 = dbsol_load_con_file(con_file2.c_str());
//    poly2 = out2->cast_to_region()->cast_to_polygon();
  }
  else{
    input_vsol2.vertical_cast(input_data_[0][1]);
    // The contour needs to be a polygon
    {
      const vcl_vector< vsol_spatial_object_2d_sptr >& vsol_list = input_vsol2->all_data();
      poly2 = vsol_list[0]->cast_to_region()->cast_to_polygon();
    }
  }

  //if (!poly1 || !poly2) {
  //  vcl_cout << "one of the polygons is not valid.\n";
  //  return false;
  //}
  /*
  // get input storage class
  vidpro1_vsol2D_storage_sptr input_vsol1;
  input_vsol1.vertical_cast(input_data_[0][0]);

  vidpro1_vsol2D_storage_sptr input_vsol2;
  input_vsol2.vertical_cast(input_data_[1][0]);
  */
  // no open-close curve notion in vsol2D
  bsol_intrinsic_curve_2d_sptr curve1 = new bsol_intrinsic_curve_2d();
  bsol_intrinsic_curve_2d_sptr curve2 = new bsol_intrinsic_curve_2d();

  // The contour can either be a polyline producing an open contour 
  // or a polygon producing a close contour
  //vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol1->all_data();
  if (load1){
    if( out1->cast_to_curve())
    {
      if( out1->cast_to_curve()->cast_to_polyline() )
      {
        curve1->setOpen(true);
        for (unsigned int i=0; i<out1->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = out1->cast_to_curve()->cast_to_polyline()->vertex(i);
          curve1->add_vertex(pt->x(), pt->y());
        } } }
    if( out1->cast_to_region())
    {
      if( out1->cast_to_region()->cast_to_polygon() )
      {
        poly1=out1->cast_to_region()->cast_to_polygon();
        curve1->setOpen(false);
        for (unsigned int i=0; i<out1->cast_to_region()->cast_to_polygon()->size();i++)
        {
          vsol_point_2d_sptr pt = out1->cast_to_region()->cast_to_polygon()->vertex(i);
          curve1->add_vertex(pt->x(), pt->y());
        }
      }
    }
  }

  else {
    const vcl_vector< vsol_spatial_object_2d_sptr >& vsol_list = input_vsol1->all_data();
    
//    if(load1){
//    vsol_list[0]=out1;
//    }
//    else;

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
          }
        }
      }
    }
  }
  if (load2){
    if( out2->cast_to_curve())
    {
      if( out2->cast_to_curve()->cast_to_polyline() )
      {
        curve2->setOpen(true);
        for (unsigned int i=0; i<out2->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = out2->cast_to_curve()->cast_to_polyline()->vertex(i);
          curve2->add_vertex(pt->x(), pt->y());
        } } }
    if( out2->cast_to_region())
    {
      if( out2->cast_to_region()->cast_to_polygon() )
      {
        poly2=out2->cast_to_region()->cast_to_polygon();
        curve2->setOpen(false);
        for (unsigned int i=0; i<out2->cast_to_region()->cast_to_polygon()->size();i++)
        {
          vsol_point_2d_sptr pt = out2->cast_to_region()->cast_to_polygon()->vertex(i);
          curve2->add_vertex(pt->x(), pt->y());
        }
      }
    }
  }
  else {
    const vcl_vector< vsol_spatial_object_2d_sptr >& vsol_list = input_vsol2->all_data();

//    if(load2){
//    vsol_list[0]=out2;
//    }
//    else;

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
          }
        }
      }
    }
  }

  dbcvr_cvmatch* curveMatch = new dbcvr_cvmatch();   // template size is 3 in this case

  curveMatch->setCurve1 (curve1);
  curveMatch->setCurve2 (curve2);
  /*
  clock_t time1, time2;
  if (closed_version) {

  curve1->setOpen(false);
  curve2->setOpen(false);

  double length1=2*curve1->arcLength(curve1->size()-1);
  vcl_cout << "length of first curve: " << length1/2 << vcl_endl;
  double length2=curve2->arcLength(curve2->size()-1);
  vcl_cout << "length of second curve: " << length2 << vcl_endl;

  dbcvr_clsd_cvmatch_sptr d1 = new dbcvr_clsd_cvmatch(curve1,curve2,(double)r1, template_size);
  //d1->setStretchCostFlag(flag);
  d1->setStretchCostFlag(false);   // cost: |ds1-ds2| + R|d_theta1-d_theta2|


  time1 = clock();
  d1->Match();

  double minCost=1E10;
  int minIndex=0;
  for (int count=0;count<curve1->size();count++){
  if (minCost>d1->finalCost(count)){
  minCost=d1->finalCost(count);
  minIndex=count;
  }
  }

  // flip the second curve and match again 
  bsol_intrinsic_curve_2d_sptr curve2_flipped = new bsol_intrinsic_curve_2d();
  for (int i = curve2->size()-1; i>=0 ; i--)
  curve2_flipped->add_vertex(curve2->x(i), curve2->y(i));

  dbcvr_clsd_cvmatch_sptr d2 = new dbcvr_clsd_cvmatch(curve1,curve2_flipped,(double)r1, template_size);
  //d2->setStretchCostFlag(flag);
  d2->setStretchCostFlag(false);  // cost: |ds1-ds2| + R|d_theta1-d_theta2|
  d2->Match();

  double minCostFlip=1E10;
  int minIndexFlip=0;
  for (int count=0;count<curve1->size();count++){
  if (minCostFlip>d2->finalCost(count)){
  minCostFlip=d2->finalCost(count);
  minIndexFlip=count;
  }
  }

  double normCost;
  //Flipping does better
  if (minCostFlip<minCost){
  normCost=minCostFlip/(length1/2+length2);
  vcl_printf("%9.6f %9.6f %9.6f %9.6f\n",minCostFlip,normCost,minCost,(length1/2+length2));

  curveMatch->setFinalMap(d2->finalMap(minIndexFlip));
  int d2size_int = static_cast<int>(d2->finalMap(minIndexFlip).size());
  assert(d2size_int>=0);
  for (int i = 0; i<d2size_int; i++)
  curveMatch->setFinalMap(i, //d2->finalMap(minIndexFlip)[i].first, 
  d2->finalMap(minIndexFlip)[i].first%curve1->size(), 
  d2->finalMap(minIndexFlip)[i].second);
  vcl_cout << "Flipped cost is better!!!\n";
  }
  else{
  normCost=minCost/(length1/2+length2);
  vcl_printf("%9.6f %9.6f %9.6f %9.6f\n",minCost,normCost,minCostFlip,(length1/2+length2));

  curveMatch->setFinalMap(d1->finalMap(minIndex));
  int d1size_int = static_cast<int>(d2->finalMap(minIndexFlip).size());
  assert(d1size_int>=0);
  for (int i = 0; i<d1size_int; i++) {
  curveMatch->setFinalMap(i, //d1->finalMap(minIndex)[i].first, 
  d1->finalMap(minIndex)[i].first%curve1->size(), 
  d1->finalMap(minIndex)[i].second);
  }
  }
  time2 = clock();
  vcl_cout<< vcl_endl<<"closed curve_2d matching cost: "<< (minCostFlip<minCost?minCostFlip:minCost) <<vcl_endl;
  vcl_cout<< "matching time: "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. " <<vcl_endl;

  } else {

  double length1=2*curve1->arcLength(curve1->size()-1);
  vcl_cout << "length of first curve: " << length1/2 << vcl_endl;
  double length2=curve2->arcLength(curve2->size()-1);
  vcl_cout << "length of second curve: " << length2 << vcl_endl;

  curve1->setOpen(true);
  curve2->setOpen(true);
  curve1->computeProperties();
  curve2->computeProperties();

  //curveMatch->setStretchCostFlag(flag);
  curveMatch->setStretchCostFlag(false);  // cost: |ds1-ds2| + R|d_theta1-d_theta2|

  time1 = clock();
  curveMatch->Match ();
  time2 = clock();

  //      vcl_cout << "Final map size of curve match: " << curveMatch->finalMap()->size() << vcl_endl;
  vcl_cout<< vcl_endl<<"curve_2d matching cost: "<< curveMatch->finalCost() <<vcl_endl;
  vcl_cout<< "matching time: "<< ((double)(time2-time1))/CLOCKS_PER_SEC << " seconds. Final map:" <<vcl_endl;

  //for (int i = 0; i<curveMatch->finalMap()->size(); i++)
  //        cl_cout << "curve match Map " << i << ": " << (*curveMatch->finalMap())[i].first << " " << (*curveMatch->finalMap())[i].second << vcl_endl;
  //
  } 
  */
dbcvr_cv_cor_sptr sil_cor;
if(closed_version){
  dbcvr_clsd_cvmatch_sptr d1 = new dbcvr_clsd_cvmatch(poly1, poly2, r1, rms, template_size);
  d1->setStretchCostFlag(false);   // cost: |ds1-ds2| + R|d_theta1-d_theta2|
  d1->Match();

  int minIndex;
  double curve_matching_cost = d1->finalBestCostRestrictedStartingPoint(minIndex, restricted_cvmatch_ratio, true);
  vcl_printf("%9.6f\n",curve_matching_cost);
  sil_cor= d1->get_cv_cor(minIndex);
    curveMatch->setFinalMap(d1->finalMap(minIndex));
}
else{
  dbcvr_cvmatch_sptr d1 = new dbcvr_cvmatch(curve1, curve2);
  d1->setStretchCostFlag(false);   // cost: |ds1-ds2| + R|d_theta1-d_theta2|
  d1->Match();
  double curve_matching_cost = d1->finalCost();
  vcl_printf("%9.6f\n",curve_matching_cost);
  sil_cor= d1->get_cv_cor();
    curveMatch->setFinalMap(*(d1->finalMap()));
    curveMatch->setFinalMapCost(d1->finalMapCost());
}
  // we need to rewrite curve match displayer tableau to use dbcvr_cv_cor_sptr
  


  // create the output storage class
  dbcvr_curvematch_storage_sptr output_curvematch = dbcvr_curvematch_storage_new();
  output_data_[0].push_back (output_curvematch);
  output_curvematch->set_curvematch (curveMatch);
  output_curvematch->set_cv_cor (sil_cor);
  return true;
}


