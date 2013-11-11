// This is dbsks/pro/dbsks_local_match_process.cxx

//:
// \file

#include "dbsks_local_match_process.h"

#include <bpro1/bpro1_parameters.h>
//#include <vil/vil_image_resource.h>
#include <vidpro1/storage/vidpro1_vsol2D_storage.h>
#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsks/pro/dbsks_shapematch_storage.h>

#include <dbsks/dbsks_dp_match.h>
#include <dbsks/dbsks_utils.h>

#include <dbsks/dbsks_local_match.h>
#include <dbsks/dbsks_local_match_sptr.h>
#include <dbsksp/dbsksp_shapelet.h>

#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_polyline_2d_sptr.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_point_2d_sptr.h>



//: Constructor
dbsks_local_match_process::
dbsks_local_match_process()
{
  vcl_vector<vcl_string > opt_mode_desc;
  opt_mode_desc.push_back("The whole graph at once"); // 0
  opt_mode_desc.push_back("One node"); // 1
  opt_mode_desc.push_back("All nodes in a sequence"); // 2

  if ( !parameters()->add("Optimization mode: " , "opt_mode" , opt_mode_desc, 2) ||
    !parameters()->add("Lambda: " , "lambda", 0.1f) ||
    !parameters()->add("Id of node to optimize: " , "node_id", int(-1)) ||
    !parameters()->add("Max number of node visits: " , "max_node_visits", int(25)) ||
    !parameters()->add("Force start from DP match result", 
      "force_start_from_DP_match", false) ||
    !parameters()->add("Use sub-optimal DP solution (instead of the optimal)?", 
      "use_sub_optimal", false) ||
    !parameters()->add("Index of sub-optimal solution (min=0)" , 
      "sub_optimal_index", int(0))
    
    )
  {
    vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
  }
}


//: Destructor
dbsks_local_match_process::
~dbsks_local_match_process()
{
}


//: Clone the process
bpro1_process* dbsks_local_match_process::
clone() const
{
  return new dbsks_local_match_process(*this);
}

//: Returns the name of this process
vcl_string dbsks_local_match_process::
name()
{ 
  return "Local Match"; 
}

//: Provide a vector of required input types
vcl_vector< vcl_string > dbsks_local_match_process::
get_input_type()
{
  vcl_vector< vcl_string > to_return;
  to_return.push_back( "dbsks_shapematch" );
  return to_return;
}


//: Provide a vector of output types
vcl_vector< vcl_string > dbsks_local_match_process::
get_output_type()
{
  vcl_vector<vcl_string > to_return;
  to_return.push_back("dbsksp_shock");
  to_return.push_back("vsol2D");
  return to_return;
}

//: Return the number of input frames for this process
int dbsks_local_match_process::
input_frames()
{
  return 1;
}


//: Return the number of output frames for this process
int dbsks_local_match_process::
output_frames()
{
  return 1;
}


// -----------------------------------------------------------------------------
//: Execute this process
bool dbsks_local_match_process::
execute()
{
  if ( input_data_.size() != 1 )
  {
    vcl_cerr << "In dbsks_local_match_process::execute() - "
             << "not exactly one input images" << vcl_endl;
    return false;
  }

  // USER PARAMS --------------------------------------------------------------
  unsigned int opt_mode;
  this->parameters()->get_value("opt_mode", opt_mode);

  float lambda;
  this->parameters()->get_value("lambda", lambda);

  int node_id;
  this->parameters()->get_value("node_id", node_id);

  int max_node_visits;
  this->parameters()->get_value("max_node_visits", max_node_visits);

  bool force_start_from_DP_match = false;
  this->parameters()->get_value("force_start_from_DP_match", force_start_from_DP_match);


  bool use_sub_optimal = false;
  this->parameters()->get_value("use_sub_optimal", use_sub_optimal);

  int sub_optimal_index;
  this->parameters()->get_value("sub_optimal_index", sub_optimal_index);


  // STORAGE CLASSES ----------------------------------------------------------
  
  // DP shape matcher
  dbsks_shapematch_storage_sptr shapematch_storage;
  shapematch_storage.vertical_cast(input_data_[0][0]);

  // PROCESS DATA -------------------------------------------------------------
  // >> Set up the shape detector
  dbsks_dp_match_sptr dp_engine = shapematch_storage->dp_engine();
  dbsks_local_match_sptr lm_engine = 0;

  // create a new engine if one doesn't exist
  if (!shapematch_storage->lm_engine() || force_start_from_DP_match)
  {
    shapematch_storage->set_lm_engine(new dbsks_local_match());
    lm_engine = shapematch_storage->lm_engine();
    lm_engine->set_graph(dp_engine->graph());
    lm_engine->set_ref_shapelet_map(dp_engine->ref_shapelet_map_);

    // use globally optimal solution
    if (!use_sub_optimal)
    {
      lm_engine->set_xnode_states(dp_engine->opt_shapelet_map_);
    }

    // use a suboptimal solution
    else
    {
      vcl_map<dbsksp_shock_edge_sptr, dbsksp_shapelet_sptr > opt_shapelet_map =
        dp_engine->list_opt_shapelet_map_[sub_optimal_index];
      lm_engine->set_xnode_states(opt_shapelet_map);
    }
  }
  else
  {
    lm_engine = shapematch_storage->lm_engine();
  }

  

  if (opt_mode == 0)  /// NEED MAJOR FIX-UP
  {
    vcl_cout << "Currently not implemented.\n";
    return false;
    ////
    //dbsks_local_match_sptr lm_engine = new dbsks_local_match();
    //lm_engine->set_graph(dp_engine->graph());
    //lm_engine->set_cost_params(dp_engine->oriented_chamfer(), 
    //  float(dp_engine->ds_shapelet_),
    //  lambda);

    //lm_engine->set_init_states_of_edges(dp_engine->opt_shapelet_map_);

    ///////////////////////////////////////////////
    //lm_engine->optimize();
    ///////////////////////////////////////////////

    //// create the output storage class
    //dbsksp_shock_storage_sptr out_sksp_storage = dbsksp_shock_storage_new();
    //output_data_[0].push_back(out_sksp_storage);

    //// create the output storage class
    //vidpro1_vsol2D_storage_sptr out_vsol2D_storage = vidpro1_vsol2D_storage_new();
    //output_data_[0].push_back(out_vsol2D_storage);


    //// Result shock graph
    //dbsksp_shock_graph_sptr new_shock_graph = new dbsksp_shock_graph(
    //  *lm_engine->graph());
    //new_shock_graph->compute_all_dependent_params();

    //// Save to storage class
    //out_sksp_storage->set_shock_graph(new_shock_graph);

    //// contact shocks
    //vcl_vector<vsol_spatial_object_2d_sptr > contact_shock_list;
    //vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor > xnode_map;
    //lm_engine->get_final_xnodes(xnode_map);
    //for (vcl_map<dbsksp_shock_node_sptr, dbsksp_xshock_node_descriptor >::iterator
    //  it = xnode_map.begin(); it != xnode_map.end(); ++it)
    //{
    //  dbsksp_xshock_node_descriptor xnode = it->second;
    //  vcl_vector<vsol_point_2d_sptr > pts;
    //  pts.push_back(new vsol_point_2d(xnode.bnd_pt_left()));
    //  pts.push_back(new vsol_point_2d(xnode.pt_));
    //  pts.push_back(new vsol_point_2d(xnode.bnd_pt_right()));
    //  contact_shock_list.push_back(new vsol_polyline_2d(pts));
    //}
    //out_vsol2D_storage->add_objects(contact_shock_list);



    //// boundary
    //vcl_vector<dbgl_circ_arc > arc_list;
    //lm_engine->get_bnd_arc_list(arc_list);

    //
    //vcl_vector<vsol_spatial_object_2d_sptr > bnd_list;
    //for (unsigned i =0; i < arc_list.size(); ++i)
    //{
    //  dbgl_circ_arc arc = arc_list[i];

    //  vcl_vector<vsol_point_2d_sptr > pts;
    //  // sample the arc using 10 segments
    //  double ds = arc.length() / 10;
    //  for (unsigned i =0; i < 11; ++i)
    //  {
    //    pts.push_back(new vsol_point_2d(arc.point_at_length(i*ds)));
    //  }

    //  bnd_list.push_back(new vsol_polyline_2d(pts));
    //}
    //out_vsol2D_storage->add_objects(bnd_list);
  }
  else if (opt_mode == 1 || opt_mode == 2) // optimize the nodes invididually
  {
    if (opt_mode == 1)
    {
      if (node_id < 0)
      {
        vcl_cout << "ERROR: node id must be >= 0.\n";
        return false;
      }


      // set up parameters for the local match engine
      lm_engine->set_cost_params(dp_engine->oriented_chamfer(), 
        float(dp_engine->ds_shapelet_),
        lambda);

      // Make sure tolerance near zero of lm_engine is zero
      
      
      ///////////////////////////////////////////////
      dbsksp_shock_node_sptr target_node = lm_engine->graph()->node_from_id(node_id);
      lm_engine->optimize(target_node);
      ///////////////////////////////////////////////
    }
    else if (opt_mode == 2)
    {
      lm_engine->set_cost_params(dp_engine->oriented_chamfer(), 
        float(dp_engine->ds_shapelet_),
        lambda);

      ///////////////////////////////////////////////
      lm_engine->optimize_nodes_in_sequence(max_node_visits);
      ///////////////////////////////////////////////

      // print out summary of the process
      lm_engine->print_summary(vcl_cout);
    }

    // create the output storage class
    dbsksp_shock_storage_sptr out_sksp_storage = dbsksp_shock_storage_new();
    output_data_[0].push_back(out_sksp_storage);

    // create the output storage class
    vidpro1_vsol2D_storage_sptr out_vsol2D_storage = vidpro1_vsol2D_storage_new();
    output_data_[0].push_back(out_vsol2D_storage);


    // Result shock graph
    dbsksp_shock_graph_sptr new_shock_graph = new dbsksp_shock_graph(
      *lm_engine->graph());
    new_shock_graph->compute_all_dependent_params();

    // Save to storage class
    out_sksp_storage->set_shock_graph(new_shock_graph);

    // contact shocks
    vcl_vector<vsol_spatial_object_2d_sptr > contact_shock_list =
      dbsks_trace_contact_shocks(lm_engine->graph(), 
      lm_engine->get_cur_xnode_map());
    
    out_vsol2D_storage->add_objects(contact_shock_list);

    // boundary
    vcl_vector<vsol_spatial_object_2d_sptr > bnd_list = 
      dbsks_trace_boundary(lm_engine->graph(), lm_engine->get_cur_xnode_map());
    out_vsol2D_storage->add_objects(bnd_list);
    return true;
  }
  else
  {
    return false;
  }

  return true;
}


// ----------------------------------------------------------------------------
bool dbsks_local_match_process::
finish()
{
  return true;
}
