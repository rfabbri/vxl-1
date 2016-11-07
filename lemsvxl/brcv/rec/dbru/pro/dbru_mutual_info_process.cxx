#include "dbru_mutual_info_process.h"

#include <vcl_ctime.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_cstdio.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_region_2d.h>
#include <vsol/vsol_polygon_2d.h>

#include <vidpro1/storage/vidpro1_image_storage.h>
#include <vidpro1/storage/vidpro1_image_storage_sptr.h>

#include <dbru/dbru_rcor.h>
#include <dbru/dbru_rcor_sptr.h>
#include <dbru/algo/dbru_object_matcher.h>

#include <dbinfo/dbinfo_observation.h>
#include <dbinfo/dbinfo_observation_sptr.h>
#include <dbinfo/dbinfo_observation_matcher.h>

#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/algo/dbsk2d_compute_shocks.h>

#include <dbskr/dbskr_tree.h>
#include <dbskr/dbskr_tree_sptr.h>

#include <bpro1/bpro1_process_sptr.h>
#include <bpro1/bpro1_parameters.h>
#include <vidpro1/vidpro1_process_manager.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage_sptr.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <vidpro1/vidpro1_repository.h>
#include <vidpro1/vidpro1_repository_sptr.h>

#define MARGIN  (10)

dbru_mutual_info_process::dbru_mutual_info_process()
{  
  if (
      !parameters()->add( "Smoothing sigma" , "-sigma" , 1.0f )  ||
      !parameters()->add( "R: " , "-R" , 10.0f ) ||
      !parameters()->add( "Shock matching: ", "-shock", false) ||
      !parameters()->add( "Thomas curve matching: " , "-thomas" , true) ||
      !parameters()->add( "Even grid curve matching): " , "-even" , false) ||
      !parameters()->add( "If even grid matching, geno interpolation (otherwise linear): " , "-linear" , false) ||
      !parameters()->add( "increment: " , "-increment" , 20) ||
      !parameters()->add( "Camera image pixel range (in bits): " , "-imagebits" , 8) ||
      !parameters()->add( "If using shock matching for region correspondence, elastic splice cost? :" , "-elastic" , true )  ||
      !parameters()->add( "Use line intersections for region correspondence? (if not shock matching):" , "-line" , true )  ||
      !parameters()->add( "Use distance transform region correspondence? (if not shock matching):" , "-disttrans" , false )  ||
      !parameters()->add( "If none of the three algs, then rigid alignment, dx:" , "-dx" , 1.0f )  ||
      !parameters()->add( "If none of the three algs, then rigid alignment, dr:" , "-dr" , 0.0f )  ||
      !parameters()->add( "If none of the three algs, then rigid alignment, ds:" , "-ds" , 0.0f )  ||
      !parameters()->add( "If none of the three algs, then rigid alignment, ratio:" , "-ratio" , 0.1f )  ||
      !parameters()->add( "If none of the three algs, Number random iterations for initialization:" , "-Nob" , 10 )  ||
      !parameters()->add( "prune threshold for shock extractions: " , "-lambda" , 1.0f ) 
      ) {
    vcl_cerr << "ERROR: Adding parameters in dbru_mutual_info_process::dbru_mutual_info_process()" << vcl_endl;
  }

  total_info_ = 0;
}


//: Clone the process
bpro1_process*
dbru_mutual_info_process::clone() const
{
  return new dbru_mutual_info_process(*this);
}

bool dbru_mutual_info_process::execute()
{
  float sigma;
  parameters()->get_value( "-sigma" , sigma );
  float R=0;
  parameters()->get_value( "-R" , R );
  bool shock_matching=false;
  parameters()->get_value( "-shock" , shock_matching );
  bool line_intersections=false;
  parameters()->get_value( "-line" , line_intersections );

  bool elastic_splice_cost=true;
  parameters()->get_value( "-elastic" , elastic_splice_cost );

  bool distance_transform=false;
  parameters()->get_value( "-disttrans" , distance_transform );
  bool thomas_matching;
  parameters()->get_value( "-thomas" , thomas_matching );
  bool even_matching;
  parameters()->get_value( "-even" , even_matching );
  bool geno_interpolation;
  parameters()->get_value( "-linear" , geno_interpolation );
  // if none of shock, dt or line intersections then rigidly align the observations
  float rigid_dx=0, rigid_dr=0, rigid_ds=0, rigid_ratio=0; int Nob=0;
  parameters()->get_value( "-dx", rigid_dx);
  parameters()->get_value( "-dr", rigid_dr);
  parameters()->get_value( "-ds", rigid_ds);
  parameters()->get_value( "-ratio", rigid_ratio);
  parameters()->get_value( "-Nob", Nob);
 
  //: CAUTION PRUNE THRESHOLD IS EXTREMELY IMPORTANT, it should be dynamically set, or input set
  //  should be examined to determine a good preset threshold
  //  If this threshold is high to much smoothing accours, the boundary loses detail
  //  otherwise too many branches stay and the shock matching takes very long time
  float prune_t=0;
  parameters()->get_value("-lambda", prune_t);   // the old lambda was for dt but it scales the regions dymanically so no need for such a variable

  int increment=0;
  parameters()->get_value("-increment", increment);
  //vcl_cout << "increment value is: " << increment << vcl_endl;

  int image_bits=0;
  parameters()->get_value("-imagebits", image_bits);
  float max_value = float(vcl_pow(double(2.0), double(image_bits))-1);

  clear_output();

  //: get input vsol 
  vidpro1_vsol2D_storage_sptr input_vsol1;
  input_vsol1.vertical_cast(input_data_[0][0]);
  vidpro1_vsol2D_storage_sptr input_vsol2;
  input_vsol2.vertical_cast(input_data_[0][2]);

  vcl_vector<vsol_point_2d_sptr> inp1, inp2;

  // The contour can either be a polyline producing an open contour 
  // or a polygon producing a close contour
  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list = input_vsol1->all_data();
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    if( vsol_list[b]->cast_to_curve())
    {
      if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
      {
        for (unsigned int i=0; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = new vsol_point_2d(vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i)->get_p());
          inp1.push_back(pt);
    } } }

    if( vsol_list[b]->cast_to_region())
    {
      if( vsol_list[b]->cast_to_region()->cast_to_polygon() )
      {
        for (unsigned int i=0; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
        {
          vsol_point_2d_sptr pt = new vsol_point_2d(vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i)->get_p());
          inp1.push_back(pt);
  } } } }

  vsol_list = input_vsol2->all_data();
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ )
  {
    if( vsol_list[b]->cast_to_curve())
    {
      if( vsol_list[b]->cast_to_curve()->cast_to_polyline() )
      {
        for (unsigned int i=0; i<vsol_list[b]->cast_to_curve()->cast_to_polyline()->size();i++)
        {
          vsol_point_2d_sptr pt = new vsol_point_2d(vsol_list[b]->cast_to_curve()->cast_to_polyline()->vertex(i)->get_p());
          inp2.push_back(pt);
    } } }

    if( vsol_list[b]->cast_to_region())
    {
      if( vsol_list[b]->cast_to_region()->cast_to_polygon() )
      {
        for (unsigned int i=0; i<vsol_list[b]->cast_to_region()->cast_to_polygon()->size();i++)
        {
          vsol_point_2d_sptr pt = new vsol_point_2d(vsol_list[b]->cast_to_region()->cast_to_polygon()->vertex(i)->get_p());
          inp2.push_back(pt);
  } } } }
  
  vsol_polygon_2d_sptr poly1 = new vsol_polygon_2d(inp1);
  vsol_polygon_2d_sptr poly2 = new vsol_polygon_2d(inp2);

  //: get input images
  vidpro1_image_storage_sptr frame_image1;
  frame_image1.vertical_cast(input_data_[0][1]);
  vil_image_resource_sptr image1_sptr = frame_image1->get_image();

  vidpro1_image_storage_sptr frame_image2;
  frame_image2.vertical_cast(input_data_[0][3]);
  vil_image_resource_sptr image2_sptr = frame_image2->get_image();

  //----------------------------------
  // create the observations
  //----------------------------------
  dbinfo_observation_sptr obs1 = new dbinfo_observation(0, image1_sptr, poly1, true, true, false);
  dbinfo_observation_sptr obs2 = new dbinfo_observation(0, image2_sptr, poly2, true, true, false);

  dbsk2d_shock_graph_sptr sg1;
  dbsk2d_shock_graph_sptr sg2;

  if (shock_matching) {
  
    //: CAUTION PRUNE THRESHOLD IS EXTREMELY IMPORTANT, it should be dynamically set, or input set
    //  should be examined to determine a good preset threshold
    //  If this threshold is high to much smoothing accours, the boundary loses detail
    //  otherwise too many branches stay and the shock matching takes very long time
    sg1 = dbsk2d_compute_shocks(poly1, prune_t);
    sg2 = dbsk2d_compute_shocks(poly2, prune_t);
  
    vcl_cout << " sg1 number of vertices: " << sg1->number_of_vertices() << " ";
    vcl_cout << " sg2 number of vertices: " << sg2->number_of_vertices() << vcl_endl;

    if (sg1->number_of_vertices() == 0) {
      vcl_cout << "First shock: ZERO number of vertices, skipping\n";
      total_info_ = 0;
      return 0;
    }
    if (sg2->number_of_vertices() == 0) {
      vcl_cout << "Second shock: ZERO number of vertices, skipping\n";
      total_info_ = 0;
      return 0;
    }
  }
  if (shock_matching && (!sg1 || !sg2))
  {
    vcl_cout << "Problems in getting shock graphs!\n";
    return 0;
  }
 
  //----------------------------------
  // compute re-mapped images
  //----------------------------------
  dbru_rcor_sptr output_rcor;

  // CAUTION: even curve matching is not supported anymore, the following algorithms use
  // THOMAS's original curve matching algorithm that aligns the samples of the input curves, after original curves are coarsened
  // via line fitting
  float rms = 0.05f;  // root mean square error for line fitting algorithm
  float restricted_cvmatch_ratio = 0.25f;  // this allows for curve alignments 
                                           // which are with (starting point + 1/4L) and (starting point - 1/4L) of the first curve
                                           // this restricts starting point invariance to prevent flipping of the contour
                                           // in vehicle recognition applications where vehicles that are matched are assumed to be
                                           // oriented similarly, hence the starting points on the two curves aligns roughly

  vil_image_resource_sptr output_sptr, output_sptr2, output_sptr3;
  if (shock_matching) {
    dbskr_tree_sptr tree1 = new dbskr_tree();
    dbskr_tree_sptr tree2 = new dbskr_tree();
    tree1->acquire(sg1, elastic_splice_cost, true, false);  // construct_circular_ends is true since matching closed curves in this application
                                                            // dpmatch_combined is false using interval cost computations as given in original edit distance algo of Sebastian et al PAMI 06
    tree2->acquire(sg2, elastic_splice_cost, true, false);  // construct_circular_ends is true since matching closed curves in this application
                                                            // dpmatch_combined is false using interval cost computations as given in original edit distance algo of Sebastian et al PAMI 06
    output_rcor = dbru_object_matcher::generate_rcor_shock_matching(obs1, obs2, tree1, tree2, false);  // verbose
  }   
  else if (distance_transform) {    
    output_rcor = dbru_object_matcher::generate_rcor_curve_matching_dt(obs1, obs2, R, rms, restricted_cvmatch_ratio, false);  // verbose
  } 
  else if (line_intersections) {
    output_rcor = dbru_object_matcher::generate_rcor_curve_matching_line(obs1, obs2,R, rms, restricted_cvmatch_ratio, increment, false);  // verbose
  } else {
    vcl_cout << "CAUTION WHEN INPUT IMAGE IS 16 bit, max value is not used in this algorithm\n";
    total_info_ = dbru_object_matcher::minfo_rigid_alignment_rand(obs1, obs2, rigid_dx, rigid_dr, rigid_ds, rigid_ratio, Nob, output_sptr2, output_sptr3, output_sptr, true);
  }

  if (shock_matching || distance_transform || line_intersections) {
    total_info_ = dbinfo_observation_matcher::minfo(obs1, obs2, output_rcor->get_correspondences(), false, max_value);
    output_sptr = output_rcor->get_appearance2_on_pixels1();
  }

  vcl_cout << "total_info: " << total_info_ << vcl_endl;

  //----------------------------------
  // create the output storage class
  //----------------------------------
  vidpro1_image_storage_sptr output_storage = vidpro1_image_storage_new();
  output_storage->set_image(output_sptr);
  output_data_[0].push_back(output_storage);

  if (!output_sptr2 && !output_sptr3) {
    vidpro1_image_storage_sptr output_storage2 = vidpro1_image_storage_new();
    output_storage2->set_image(output_sptr2);
    output_data_[0].push_back(output_storage2);

    vidpro1_image_storage_sptr output_storage3 = vidpro1_image_storage_new();
    output_storage3->set_image(output_sptr3);
    output_data_[0].push_back(output_storage3);
  }
  
  return true;
}

