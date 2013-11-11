// This is brcv/shp/dbsk2d/algo/dbsk2d_compute_shocks.cxx

//:
// \file

#include <dbsk2d/algo/dbsk2d_compute_shocks.h>

#include <vul/vul_timer.h>

#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_conic_2d.h>
#include <vsol/vsol_conic_2d_sptr.h>

#include <dbsk2d/dbsk2d_boundary.h>
#include <dbsk2d/dbsk2d_ishock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>

#include <dbsk2d/algo/dbsk2d_bnd_preprocess.h>
#include <dbsk2d/algo/dbsk2d_ishock_detector.h>
#include <dbsk2d/algo/dbsk2d_prune_ishock.h>
#include <dbsk2d/algo/dbsk2d_sample_ishock.h>

dbsk2d_boundary_sptr 
dbsk2d_create_boundary( vcl_vector< vsol_spatial_object_2d_sptr > vsol_list,
                        bool override_default_partitioning,
                        float xmin, float ymin,
                        int num_rows, int num_cols, 
                        float cell_width, float cell_height,
                        bool preprocess_boundary,
                        bool break_long_lines)
{
  // timer to measure time for each step
  vul_timer timer1;

  //----------------------------------------------------------------------
  //1) instantiate a boundary class
  dbsk2d_boundary_sptr boundary = new dbsk2d_boundary();

  //----------------------------------------------------------------------
  //2) VSOL -> dbsk2d_boundary
  //   parse through all the vsol classes and add to the boundary

  //timer1.mark(); // start timer
  //vcl_cout << "\nPutting VSOL objects into boundary ... ";
  for (unsigned int b = 0 ; b < vsol_list.size() ; b++ ) 
  {
    if( vsol_list[b]->cast_to_point() ) //POINT
      boundary->add_a_point(vsol_list[b]->cast_to_point()); 
    else if( vsol_list[b]->cast_to_curve()) {
      if (vsol_list[b]->cast_to_curve()->cast_to_line())  //LINE
        boundary->add_a_line(vsol_list[b]->cast_to_curve()->cast_to_line());
      else if( vsol_list[b]->cast_to_curve()->cast_to_polyline()) //POLYLINE
        boundary->add_a_polyline(vsol_list[b]->cast_to_curve()->cast_to_polyline()); 
      else if (vsol_list[b]->cast_to_curve()->cast_to_conic()) // CIRCULAR ARC
      {
        vsol_conic_2d_sptr conic = vsol_list[b]->cast_to_curve()->cast_to_conic();
        if (conic->is_real_circle())
        {
          vgl_point_2d<double > arc_p1 = conic->p0()->get_p();
          vgl_point_2d<double > arc_p2 = conic->p1()->get_p();
          vgl_point_2d<double > arc_center = conic->centre();
          double arc_k = 1/(arc_center-arc_p1).length();
          // determine sign of curvature
          if (cross_product<double >(arc_p1-arc_center, arc_p2-arc_center) <0)
          {
            arc_k = -arc_k;
          }
          boundary->add_an_arc(arc_p1, arc_p2, arc_k);
        }
      }
    }
    else if( vsol_list[b]->cast_to_region()) {
      if (vsol_list[b]->cast_to_region()->cast_to_polygon()) //POLYGON
        boundary->add_a_polygon(vsol_list[b]->cast_to_region()->cast_to_polygon());
    }
  }

  //double vsol2boundary = (float)(timer1.real())/1000;
  //vcl_cout << "done. Time taken= " << vsol2boundary << "seconds\n";
  
  //----------------------------------------------------------------------
  //3) Partition the boundary elements into cells
  //timer1.mark();
  //vcl_cout << "Partitioning boundary into cells...";

  if (!override_default_partitioning)
  {
    vsol_box_2d_sptr box = boundary->get_bounding_box();
    // box->print_summary(vcl_cout);
    cell_width = (float)( box->width()/(num_cols-0.5) );
    cell_height = (float)( box->height()/(num_rows-0.5) );
    xmin = (float)(box->get_min_x() - cell_width/4);
    ymin = (float)(box->get_min_y() - cell_height/4);
  }

  // set partitioning parameters
  boundary->set_partition_params(xmin, ymin, num_rows, num_cols, cell_height, cell_width);
  //boundary->print_partition_summary();

  // Partition the boundary
  boundary->partition_into_cells(true, false, dbsk2d_bnd_preprocess::distance_tol);

  //double partition_time = (float)(timer1.real())/1000;
  //vcl_cout << "done. Time taken= " << partition_time << "seconds\n";
  
  //----------------------------------------------------------------------
  //4) Preprocess the boundary to build topology
  if (preprocess_boundary){
    timer1.mark();
    //vcl_cout << "Preprocessing boundary ... ";

    dbsk2d_bnd_preprocess bnd_preprocessor;
    bnd_preprocessor.preprocess(boundary, false);

    //double preprocess_time = (float)(timer1.real())/1000;
    //vcl_cout << "done. Time taken= " << preprocess_time << "seconds\n";

    //// validate boundary preprocessing 
    //timer1.mark();
    //vcl_cout << "Verifying boundary preprocessing .. " ;
    //
    //bool need_preprocess = bnd_preprocessor.need_preprocessing(boundary);

    ////double verify_time = (float)(timer1.real())/1000;
    ////vcl_cout << "done. Time taken= " << verify_time << "seconds\n";

    //vcl_string result = (need_preprocess) ? "Yes" : "No";
    //vcl_cout <<"Boundary needs preprocessing " << result << vcl_endl;

    //if (need_preprocess) 
    //  return false;
    
    //Break long lines that extend beyond one cell into small segments
    if (break_long_lines)
    {
      timer1.mark();
      boundary->break_long_line_edges(dbsk2d_bnd_preprocess::distance_tol);

      //double time_to_break_long_lines = (float)(timer1.real())/1000;
      //vcl_cout << "done. Time taken= " << time_to_break_long_lines << "seconds\n";
    }
  }

  //after everything is converted, compile the belm_list
  boundary->update_belm_list();
  //vcl_cout << "Number of edges = " << boundary->all_edges().size() << vcl_endl;
  //vcl_cout << "Number of belms = " << boundary->num_belms() << vcl_endl;
  //boundary->print_belm_list(vcl_cout);

  return boundary;
}

//: Compute intrinsic shocks from a boundary 
//  \relates dbsk2d_ishock_detector 
dbsk2d_ishock_graph_sptr 
dbsk2d_compute_ishocks (dbsk2d_boundary_sptr boundary)
{
  //1) Detect Shocks from this boundary
  dbsk2d_ishock_detector sh_det(boundary);

  sh_det.detect_shocks();
  //validate the shock computation
  bool shock_computation_valid = sh_det.validate_shocks(); 
  //update the extrinsic points so that they are displayed properly
  sh_det.ishock_graph()->update_shocks();

  if (!shock_computation_valid)
    return 0;
  else
    return sh_det.ishock_graph();
}


//: \relates dbsk2d_ishock_detector 
//  \relates dbsk2d_prune_ishock
dbsk2d_shock_graph_sptr 
dbsk2d_compute_shocks (dbsk2d_boundary_sptr boundary, 
                       float prune_threshold,
                       bool prune_shocks)
{
  //1) Detect Shocks from this boundary
  dbsk2d_ishock_detector sh_det(boundary);

  sh_det.detect_shocks();
  //validate the shock computation
  bool shock_computation_valid = sh_det.validate_shocks(); 
  //update the extrinsic points so that they are displayed properly
  sh_det.ishock_graph()->update_shocks();

  //2) Prune the shock graph
  //instantiate an empty coarse shock graph at this time
  //this will be properly defined once the shock is pruned
  dbsk2d_shock_graph_sptr coarse_ishock = new dbsk2d_shock_graph();

  if (prune_shocks && shock_computation_valid){
    //prune this shock graph and output a coarse shock graph 
    //corresponding to the remaining shock edges
    dbsk2d_prune_ishock ishock_pruner(sh_det.ishock_graph(), coarse_ishock);
    ishock_pruner.prune(prune_threshold);
    ishock_pruner.compile_coarse_shock_graph();
  }

  return coarse_ishock;
}

dbsk2d_shock_graph_sptr 
dbsk2d_compute_shocks (vcl_vector< vsol_spatial_object_2d_sptr > vsol_list,
                       float prune_threshold,
                       bool override_default_partitioning,
                       float xmin, float ymin,
                       int num_rows, int num_cols, 
                       float cell_width, float cell_height,
                       bool preprocess_boundary,
                       bool break_long_lines,
                       bool prune_shocks)
{
  //1) Form the boundary class from the VSOL and preprocess
  dbsk2d_boundary_sptr boundary = dbsk2d_create_boundary( vsol_list, 
                                                          override_default_partitioning,
                                                          xmin, ymin, num_rows, num_cols, 
                                                          cell_width, cell_height,
                                                          preprocess_boundary,
                                                          break_long_lines);

  //2) Detect Shocks from this boundary and return the pruned shock graph
  return dbsk2d_compute_shocks (boundary, prune_threshold, prune_shocks);
}

dbsk2d_shock_graph_sptr 
dbsk2d_compute_shocks (vsol_polygon_2d_sptr & polygon,
                       float prune_threshold)
{
  //----------------------------------------------------------------------
  //1) Form the boundary class from the vsol_polygon_2d
  vcl_vector< vsol_spatial_object_2d_sptr > vsol_list;
  vsol_list.push_back(polygon->cast_to_spatial_object());
  dbsk2d_boundary_sptr boundary = dbsk2d_create_boundary(vsol_list);

  //----------------------------------------------------------------------
  //2) Detect Shocks from this boundary
  dbsk2d_ishock_detector sh_det(boundary);

  sh_det.detect_shocks();
  //validate the shock computation
  bool shock_computation_valid = sh_det.validate_shocks(); 
  //update the extrinsic points so that they are displayed properly
  sh_det.ishock_graph()->update_shocks();

  //----------------------------------------------------------------------
  //3) Prune the shock graph
  //instantiate an empty coarse shock graph at this time
  //this will be properly defined once the shock is pruned
  dbsk2d_shock_graph_sptr coarse_ishock = new dbsk2d_shock_graph();

  if (shock_computation_valid){
    //prune this shock graph and output a coarse shock graph 
    //corresponding to the remaining shock edges
    dbsk2d_prune_ishock ishock_pruner(sh_det.ishock_graph(), coarse_ishock);
    ishock_pruner.prune(prune_threshold);
    ishock_pruner.compile_coarse_shock_graph();
  }
  else
    return NULL;

  return coarse_ishock;
}

dbsk2d_shock_graph_sptr 
dbsk2d_compute_xshocks (vsol_polygon_2d_sptr & polygon,
                       float prune_threshold, 
                       float sampling_resoluton)
{
  //first compute the intrinsic shock graph then sample it to get the extrinsic shock graph
  dbsk2d_shock_graph_sptr pruned_ishock_graph = dbsk2d_compute_shocks(polygon, prune_threshold);

  //sample the intrinsic shock graph into an extrinsic shock graph
  dbsk2d_sample_ishock ishock_sampler(pruned_ishock_graph);
  ishock_sampler.sample(sampling_resoluton);

  return ishock_sampler.extrinsic_shock_graph();
}


//: test xshock computation
bool test_xshock_graph(dbsk2d_shock_graph_sptr sg)
{
  if (!sg) { 
    vcl_cout << "In  test_xshock_graph() -- invalid pointer!\n";
    return false;
  }

  if (!sg->number_of_edges()) {
    vcl_cout << "In  test_xshock_graph() -- zero edges!\n";
    return false;
  }

  if (!sg->number_of_vertices()) {
    vcl_cout << "In  test_xshock_graph() -- zero vertices!\n";
    return false;
  }

  //: check samples
  for ( dbsk2d_shock_graph::vertex_iterator v = sg->vertices_begin(); v != sg->vertices_end(); v++ ) {
    if (!(*v)->ex_pts().size())
      return false;
  }

  //: check samples
  for ( dbsk2d_shock_graph::edge_iterator e = sg->edges_begin(); e != sg->edges_end(); e++ ) {
    if (!(*e)->ex_pts().size())
      return false;
  }

  return true;
}


