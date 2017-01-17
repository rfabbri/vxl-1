// This is seg/dbsks/vis/dbsks_xshock_cost_tool.cxx
//:
// \file
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date Sep 24, 2008

#include "dbsks_xshock_cost_tool.h"

#include <bvis1/bvis1_manager.h>

//#include <vgui/vgui_dialog.h>
//#include <vidpro1/vidpro1_repository.h>
//#include <vidpro1/storage/vidpro1_image_storage.h>

#include <dbsksp/vis/dbsksp_soview_xshock.h>
#include <dbsksp/dbsksp_xshock_edge.h>
#include <dbsksp/dbsksp_xshock_node.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsks/dbdet_arc_patch.h>
#include <dbsksp/dbsksp_xshock_fragment.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_line_2d.h>

// -----------------------------------------------------------------------------
//: Constructor
dbsks_xshock_cost_tool::
dbsks_xshock_cost_tool() : 
  tableau_(0), 
  shock_storage_(0)
{
  // define the keyboard combination for the gestures
  this->gesture_display_info_ = vgui_event_condition(vgui_LEFT, vgui_MODIFIER_NULL, true);

}

// -----------------------------------------------------------------------------
//: Destructor
dbsks_xshock_cost_tool::
~dbsks_xshock_cost_tool()
{
}


// -----------------------------------------------------------------------------
//: set the tableau to work with
bool dbsks_xshock_cost_tool::
set_tableau ( const vgui_tableau_sptr& tableau )
{
  if(!tableau)
    return false;
  if( tableau->type_name() == "dbsksp_shock_tableau" ){
    if( !this->set_storage(bvis1_manager::instance()->storage_from_tableau(tableau)) )
      return false;
    this->tableau_.vertical_cast(tableau);
    return true;
  }
  tableau_ = 0;
  return false;
}



// -----------------------------------------------------------------------------
//: Set the storage class for the active tableau
bool dbsks_xshock_cost_tool::
set_storage (const bpro1_storage_sptr& storage_sptr)
{
  if (!storage_sptr)
    return false;
  
  //make sure its a vsol storage class
  if (storage_sptr->type() == "dbsksp_shock"){
    this->shock_storage_.vertical_cast(storage_sptr);
    return true;
  }
  this->shock_storage_ = 0;
  return false;
}


// ----------------------------------------------------------------------------
//: Return the name of this tool
vcl_string dbsks_xshock_cost_tool::
name() const 
{
  return "XShock cost";
}


// ----------------------------------------------------------------------------
//: Return the storage of the tool
dbsksp_shock_storage_sptr dbsks_xshock_cost_tool::
shock_storage() const
{
  return this->shock_storage_;
}



// ----------------------------------------------------------------------------
//: Return the tableau of the tool
dbsksp_shock_tableau_sptr dbsks_xshock_cost_tool::
tableau()
{
  return this->tableau_;
}



//// -----------------------------------------------------------------------------
////: Set the image storage
//bool dbsks_xshock_cost_tool::
//set_image_storage ( const bpro1_storage_sptr& image_storage)
//{
//  if (!image_storage)
//    return false;
//  
//  //make sure its a vsol storage class
//  if (image_storage->type() == "image")
//  {
//    this->image_storage_.vertical_cast(image_storage);
//    return true;
//  }
//  this->image_storage_.vertical_cast(0);
//  return false;
//}
//  






////: Set shapematch storage
//bool dbsks_xshock_cost_tool::
//set_shapematch_storage(const bpro1_storage_sptr& shapematch_storage)
//{
//  if (!shapematch_storage)
//    return false;
//  
//  //make sure its a vsol storage class
//  if (shapematch_storage->type() == "dbsks_shapematch")
//  {
//    this->shapematch_storage_.vertical_cast(shapematch_storage);
//    return true;
//  }
//  this->shapematch_storage_.vertical_cast(0);
//  return false;
//}


//: Get selected edge
dbsksp_xshock_edge_sptr dbsks_xshock_cost_tool::
selected_xedge() const
{
  return this->shock_storage()->active_xedge();
}

//: Set selected edge 
void dbsks_xshock_cost_tool::
set_selected_xedge(const dbsksp_xshock_edge_sptr& xe)
{
  this->shock_storage()->set_active_xedge(xe);
}

//: Get selected node
dbsksp_xshock_node_sptr dbsks_xshock_cost_tool::
selected_xnode() const
{
  return this->shock_storage()->active_xnode();
}

//: Set selected node
void dbsks_xshock_cost_tool::
set_selected_xnode(const dbsksp_xshock_node_sptr& xv)
{
  this->shock_storage()->set_active_xnode(xv);
}




// -----------------------------------------------------------------------------
//: Allow the tool to add to the popup menu as a tableau would
void dbsks_xshock_cost_tool::
get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  //
  menu.add("Arc patches for current fragment", 
    new dbsks_xshock_draw_arc_patch_command(this)); 
}


// ----------------------------------------------------------------------------
//: Handle events
bool dbsks_xshock_cost_tool::
handle( const vgui_event & e, const bvis1_view_tableau_sptr& view )
{
  // Display information of selected object
  if (this->gesture_display_info_(e))
  {
    return this->handle_display_info();
  }

  return false;
}




// ----------------------------------------------------------------------------
//: Handle displaying info of selected objects
bool dbsks_xshock_cost_tool::
handle_display_info()
{
  unsigned int highlighted_id = this->tableau()->get_highlighted();
  if (highlighted_id)
  {
    vgui_soview* so = vgui_soview::id_to_object(highlighted_id);
    if (so->type_name() == "dbsksp_soview_xshock_chord")
    {
      dbsksp_soview_xshock_chord* chord = 
        static_cast<dbsksp_soview_xshock_chord* >(so);
      dbsksp_xshock_edge_sptr xedge = chord->xedge();
      xedge->print(vcl_cout);
      this->set_selected_xedge(xedge);
      
      // reset the selected xnode to neighbor the newly selected xedge
      if (!xedge->is_vertex(this->selected_xnode()))
      {
        if (xedge->source()->degree()==1)
        {
          this->set_selected_xnode(xedge->target());
        }
        else
        {
          this->set_selected_xnode(xedge->source());
        }
      }


      bvis1_manager::instance()->display_current_frame();
      return true;
    }

    if (so->type_name() == "dbsksp_soview_xshock_node" )
    {
      dbsksp_soview_xshock_node* so_xnode = 
        static_cast<dbsksp_soview_xshock_node* >(so);
      dbsksp_xshock_node_sptr xnode = so_xnode->xnode();

      xnode->print(vcl_cout);
      this->set_selected_xnode(xnode);

      // set the selected_edge to neighbor the newly selected node
      if (!this->selected_xedge() || !this->selected_xedge()->is_vertex(xnode))
      {
        this->set_selected_xedge(*xnode->edges_begin());
      }

      bvis1_manager::instance()->display_current_frame();
      return true;
    }
  }
  return true;
}




// ============================================================================
// dbsks_xshock_draw_arc_patch_command
// ============================================================================

//: Main execution of the command
void dbsks_xshock_draw_arc_patch_command::
execute()
{
  dbsksp_xshock_edge_sptr xe = this->tool()->selected_xedge();

  // preliminary check
  if (!xe) 
  {
    vcl_cerr << "ERROR: No active edge found." << vcl_endl;
    return;
  }

  if (xe->is_terminal_edge())
  {
    vcl_cerr << "ERROR: Selected edge should be non-terminal." << vcl_endl;
    return;
  }

  // insert a new shock node inside the terminal edge
  dbsksp_xshock_graph_sptr xg = this->tool()->shock_storage()->xshock_graph();
  


  // draw a patch around the boundary of the current fragment
  dbsksp_xshock_node_descriptor xdesc0 = *xe->source()->descriptor(xe);
  dbsksp_xshock_node_descriptor xdesc1 = xe->target()->descriptor(xe)->opposite_xnode();

  // collect histogram of gradient along the boundary
  dbsksp_xshock_fragment xfrag(xdesc0, xdesc1);
  int num_segments = 4;
  double patch_width = 8;
  

  // left boundary
  {
    dbgl_biarc bnd_left = xfrag.bnd_left_as_biarc();
    double len_left = bnd_left.len();
    for (int k =0; k < num_segments; ++k)
    {
      vgl_point_2d<double > start = bnd_left.point_at( k*len_left / num_segments );
      vgl_vector_2d<double > start_tangent = bnd_left.tangent_at(k*len_left / num_segments );
      vgl_point_2d<double > end = bnd_left.point_at((k+1)*len_left / num_segments);
      vgl_vector_2d<double > end_tangent = bnd_left.tangent_at((k+1)*len_left / num_segments);

      vgl_point_2d<double > midpt = bnd_left.point_at( (k+0.5)*len_left / num_segments );
      
      // estimate this curve segment with a circular arc
      dbgl_circ_arc arc;
      arc.set_from(start, midpt, end);
      
      // contruct the arc patch
      dbdet_arc_patch patch(arc, patch_width);

      // compute world-coordinate and reference vector of grid points
      vnl_matrix<double > X, Y, Fx, Fy;
      double du = 2; // spacing along the central axis
      double dv = 2; // spacing orthorgonal to the central axis
      patch.compute_grid(du, dv, X, Y, Fx, Fy);

      // draw the points
      for (unsigned i =0; i < X.rows(); ++i)
      {
        for (unsigned j =0; j < X.cols(); ++j)
        {
          this->tool()->shock_storage()->add_vsol_object(new vsol_point_2d(X(i, j), Y(i, j)));

          vgl_vector_2d<double > t0(Fx(i, j), Fy(i, j));
          t0 = normalized(t0) * 1;
          vgl_point_2d<double > pt(X(i, j), Y(i, j));
          this->tool()->shock_storage()->add_vsol_object(new vsol_line_2d(pt - t0/2, pt + t0/2));
          
        }
      }

      // draw two line segments separating the fragment
      vgl_point_2d<double > p00 = arc.start() + arc.normal_at(0) * patch_width/2;
      vgl_point_2d<double > p01 = arc.start() - arc.normal_at(0) * patch_width/2;
      this->tool()->shock_storage()->add_vsol_object(new vsol_line_2d(p00, p01));

      // draw two line segments separating the fragment
      vgl_point_2d<double > p10 = arc.end() + arc.normal_at(1) * patch_width/2;
      vgl_point_2d<double > p11 = arc.end() - arc.normal_at(1) * patch_width/2;
      this->tool()->shock_storage()->add_vsol_object(new vsol_line_2d(p00, p01));
      
    }
  }



  // right boundary
  {
    dbgl_biarc bnd_right = xfrag.bnd_right_as_biarc();
    double len_right = bnd_right.len();
    for (int k =0; k < num_segments; ++k)
    {
      vgl_point_2d<double > start = bnd_right.point_at( k*len_right / num_segments );
      vgl_vector_2d<double > start_tangent = bnd_right.tangent_at(k*len_right / num_segments );
      vgl_point_2d<double > end = bnd_right.point_at((k+1)*len_right / num_segments);
      vgl_vector_2d<double > end_tangent = bnd_right.tangent_at((k+1)*len_right / num_segments);

      vgl_point_2d<double > midpt = bnd_right.point_at( (k+0.5)*len_right / num_segments );
      
      // estimate this curve segment with a circular arc
      dbgl_circ_arc arc;
      arc.set_from(start, midpt, end);
      
      // contruct the arc patch
      dbdet_arc_patch patch(arc, patch_width);

      // compute world-coordinate and reference vector of grid points
      vnl_matrix<double > X, Y, Fx, Fy;
      double du = 2; // spacing along the central axis
      double dv = 2; // spacing orthorgonal to the central axis
      patch.compute_grid(du, dv, X, Y, Fx, Fy);

      // draw the points
      for (unsigned i =0; i < X.rows(); ++i)
      {
        for (unsigned j =0; j < X.cols(); ++j)
        {
          this->tool()->shock_storage()->add_vsol_object(new vsol_point_2d(X(i, j), Y(i, j)));

          vgl_vector_2d<double > t0(Fx(i, j), Fy(i, j));
          t0 = normalized(t0) * 1;
          vgl_point_2d<double > pt(X(i, j), Y(i, j));
          this->tool()->shock_storage()->add_vsol_object(new vsol_line_2d(pt - t0/2, pt + t0/2));
        }
      }


      // draw two line segments separating the fragment
      vgl_point_2d<double > p00 = arc.start() + arc.normal_at(0) * patch_width/2;
      vgl_point_2d<double > p01 = arc.start() - arc.normal_at(0) * patch_width/2;
      this->tool()->shock_storage()->add_vsol_object(new vsol_line_2d(p00, p01));

      // draw two line segments separating the fragment
      vgl_point_2d<double > p10 = arc.end() + arc.normal_at(1) * patch_width/2;
      vgl_point_2d<double > p11 = arc.end() - arc.normal_at(1) * patch_width/2;
      this->tool()->shock_storage()->add_vsol_object(new vsol_line_2d(p00, p01));
      
    }
  }


  bvis1_manager::instance()->display_current_frame();

  return;
}


