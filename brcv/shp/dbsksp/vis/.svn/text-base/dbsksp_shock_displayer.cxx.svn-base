// This is contrib/ntrinh/v2_gui/bvis1_dbsksp_shock_displayer.cxx

//:
// \file
// \author Nhon Trinh (ntrinh@lems.brown.edu
// \date 1/25/2005


#include "dbsksp_shock_displayer.h"


#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_line_2d.h>
#include <vgui/vgui_style.h>
#include <vil/vil_color_table.h>

#include <dbsksp/dbsksp_shock_graph.h>
#include <dbsksp/dbsksp_shock_fragment.h>

#include <dbsksp/pro/dbsksp_shock_storage.h>
#include <dbsksp/vis/dbsksp_shock_tableau.h>
#include <dbsksp/vis/dbsksp_soview_shock.h>

#include <vcl_cstdlib.h>





//: Create a tableau if the storage object is of type dbsksp_skgraph
vgui_tableau_sptr
dbsksp_shock_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return 0 tableau if the types don't match
  if( storage->type() != this->type())
    return 0;

  // Cast the storage object into a dbsksp_skgraph storage object
  dbsksp_shock_storage_sptr shock_storage;
  shock_storage.vertical_cast(storage);

  // create a new dbsksp_skgraph tableau
  dbsksp_shock_tableau_new shock_tab;
  shock_tab->set_shock_graph(shock_storage->shock_graph());

  dbsksp_shock_graph_sptr shock_graph = shock_storage->shock_graph();
  if (shock_graph)
  {
    vcl_vector<vgui_style_sptr> style_vector;
    style_vector.push_back(vgui_style::new_style(1,0,0, 3.0, 3.0));
    style_vector.push_back(vgui_style::new_style(0,1,0, 3.0, 3.0));
    style_vector.push_back(vgui_style::new_style(0,0,1, 3.0, 3.0));
    style_vector.push_back(vgui_style::new_style(0.5f,0,1, 1.0, 1.0));

    // add the visual boundary
    for (dbsksp_shock_graph::edge_iterator eit = shock_graph->edges_begin();
      eit != shock_graph->edges_end(); ++eit)
    {
      assert(*eit);
      dbsksp_shock_edge_sptr edge = (*eit);

      //////////////////////////////////////
      // add the chord
      shock_tab->add_shock_edge_chord(edge);
      //////////////////////////////////////

      //////////////////////////////////////
      // add the shock curve
      shock_tab->add_shock_geom(edge);
      //////////////////////////////////////


      dbsksp_shock_fragment_sptr fragment = edge->fragment();
      if (!fragment) continue;

      ////////////////////////////////////////////
      // add boundary
      shock_tab->add_bnd_arc(fragment->left_bnd());
      shock_tab->add_bnd_arc(fragment->right_bnd());
      ////////////////////////////////////////////
    }


    // add the nodes
    for (dbsksp_shock_graph::vertex_iterator vit = shock_graph->vertices_begin();
      vit != shock_graph->vertices_end(); ++vit)
    {
      // ignore degenerate nodes
      if ((*vit)->degree() == 1) continue;

      shock_tab->add_shock_node(*vit);
      shock_tab->add_contact_shock(*vit);
    }

    // add the reference node and reference edge
    // reference node
    vgl_point_2d<double > ref_pt = shock_graph->ref_node()->pt();
    shock_tab->set_current_grouping("dbsksp_soview_reference");
    vgui_soview2D_circle* circle = shock_tab->add_circle(ref_pt.x(), ref_pt.y(), shock_graph->ref_node_radius());
    circle->set_style(vgui_style::new_style(1.0f, 1.0f, 0.0, 1.0, 1.0));
    circle->set_selectable(false);

    // reference line
    vgl_point_2d<double > ref_line_start = shock_graph->ref_edge()->source()->pt();
    vgl_point_2d<double > ref_line_end = shock_graph->ref_edge()->target()->pt();
    vgui_soview2D_lineseg* line = shock_tab->add_line(float(ref_line_start.x()),
      float(ref_line_start.y()), float(ref_line_end.x()), float(ref_line_end.y()));
    line->set_style(vgui_style::new_style(1.0f, 1.0f, 0.0, 8.0, 8.0));
    line->set_selectable(false);

    // add the active shapelet
    if (shock_storage->active_shapelet())
    {
      shock_tab->add_shapelet(shock_storage->active_shapelet());
    }

    // display all the shapelets
    vcl_vector<dbsksp_shapelet_sptr > shapelet_list = shock_storage->shapelet_list();
    for (unsigned i=0; i<shapelet_list.size(); ++i)
    {
      int index = (5*i) % 256;
      vil_rgb<vxl_byte > color = color_value(HSV1, index);
      
      shock_tab->add_shapelet(shapelet_list[i], vgui_style::new_style(
        float(color.R())/255, float(color.G())/255, float(color.B())/255, 3.0f, 3.0f));
    }

    // display the active node, active edge, e0, e1
    if (shock_storage->active_node())
    {
      vgui_style_sptr active_node_style = vgui_style::new_style(1.0f, 0.0f, 1.0f, 8.0f, 8.0f);
      vgui_soview2D* so = shock_tab->add_shock_node(shock_storage->active_node(),
        active_node_style);
      so->set_selectable(false);
    }

    if (shock_storage->active_edge())
    {
      vgui_style_sptr active_edge_style = vgui_style::new_style(1.0f, 0.0f, 1.0f, 8.0f, 8.0f);
      vgui_soview2D* so = shock_tab->add_shock_edge_chord(shock_storage->active_edge(), 
        active_edge_style);
      so->set_selectable(false);
    }

    // display the target point
    if (shock_storage->target_point())
    {
      shock_tab->add_vsol_point_2d(shock_storage->target_point());
    }
  }

  // DISPLAY EXTRINSIC SHOCK GRAPH
  
  dbsksp_xshock_graph_sptr xgraph = shock_storage->xshock_graph();
  if (xgraph)
  {
    shock_tab->set_current_grouping("dbsksp_xshock");
    for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin();
      eit != xgraph->edges_end(); ++eit)
    {
      dbsksp_xshock_edge_sptr xe = *eit;
      //dbsksp_soview_xfrag* frag = shock_tab->add_xfrag(xe);  

      // extrinsic chord
      shock_tab->add_xshock_chord(xe);

      // extrinsic boundary
      shock_tab->add_xshock_bnd(xe);
    }

    // add the nodes
    for (dbsksp_xshock_graph::vertex_iterator vit = xgraph->vertices_begin();
      vit != xgraph->vertices_end(); ++vit)
    {
      // ignore degenerate nodes
      if ((*vit)->degree() == 1) continue;

      shock_tab->add_xshock_node(*vit);
      shock_tab->add_xshock_contact(*vit);
    }

    // active edge
    
    if (shock_storage->active_xedge())
    {
      dbsksp_xshock_edge_sptr xe = shock_storage->active_xedge();
      
      // reference line
      vgl_point_2d<double > seg_start = xe->source()->pt();
      vgl_point_2d<double > seg_end = xe->target()->pt();
      vgui_soview2D_lineseg* line = shock_tab->add_line(float(seg_start.x()),
        float(seg_start.y()), float(seg_end.x()), float(seg_end.y()));
      line->set_style(vgui_style::new_style(1.0f, 1.0f, 0.0, 6.0, 6.0));
      line->set_selectable(false);
    }

    // active node
    if (shock_storage->active_xnode())
    {
      dbsksp_xshock_node_sptr xv = shock_storage->active_xnode();
      vgl_point_2d<double > pt = xv->pt();
      vgui_soview2D_circle* circle = shock_tab->add_circle(pt.x(), pt.y(), xv->radius());
      circle->set_style(vgui_style::new_style(1.0f, 0.5f, 0.0f, 2.0f, 2.0f));
      circle->set_selectable(false);

      vgui_soview2D_point* center = shock_tab->add_point(pt.x(), pt.y());
      center->set_style(vgui_style::new_style(1.0f, 0.5f, 0.0f, 9.0f, 9.0f));
      center->set_selectable(false);
    }
  }

  // DISPLAY VSOL objects
  for (unsigned i =0; i < shock_storage->vsol_list().size(); ++i)
  {
    vsol_spatial_object_2d_sptr obj = shock_storage->vsol_list()[i];
    shock_tab->add_spatial_object(obj);
  }
  
  
  

        
  shock_tab->set_current_grouping( "default" );
  return shock_tab;
}

