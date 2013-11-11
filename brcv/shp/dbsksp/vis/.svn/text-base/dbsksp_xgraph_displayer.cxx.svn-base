// This is dbsksp/vis/dbsksp_xgraph_displayer.cxx

//:
// \file


#include "dbsksp_xgraph_displayer.h"

#include <dbsksp/pro/dbsksp_xgraph_storage.h>
#include <dbsksp/vis/dbsksp_xgraph_tableau.h>
#include <dbsksp/vis/dbsksp_soview_shock.h>



//------------------------------------------------------------------------------
//: Create a tableau if the storage object is of type dbsksp_skgraph
vgui_tableau_sptr
dbsksp_xgraph_displayer::make_tableau( bpro1_storage_sptr storage) const
{
  // Return 0 tableau if the types don't match
  if( storage->type() != this->type())
    return 0;

  // Cast the storage object into a dbsksp_skgraph storage object
  dbsksp_xgraph_storage_sptr xgraph_storage;
  xgraph_storage.vertical_cast(storage);

  // create a new dbsksp_skgraph tableau
  dbsksp_xgraph_tableau_sptr shock_tab = dbsksp_xgraph_tableau_new();
  if (this->update_xgraph_tableau(shock_tab, xgraph_storage))
  {
    return shock_tab;
  }
  return 0;
}








//------------------------------------------------------------------------------
//: Update a tableau with a the storage object if both are of type image
bool dbsksp_xgraph_displayer::
update_tableau(vgui_tableau_sptr tab, const bpro1_storage_sptr& storage ) const
{
  // Return a NULL tableau if the types don't match
  if( storage->type() != this->type() )
    return false;
  
  dbsksp_xgraph_tableau_sptr xgraph_tab = dynamic_cast<dbsksp_xgraph_tableau* >(tab.ptr());
  if(!xgraph_tab)
    return false;

  // Cast the storage object into an storage object
  dbsksp_xgraph_storage_sptr xgraph_storage;
  xgraph_storage.vertical_cast(storage);

  return this->update_xgraph_tableau(xgraph_tab, xgraph_storage);  
}
 

//------------------------------------------------------------------------------
//: Perform the update once types are known
bool dbsksp_xgraph_displayer::
update_xgraph_tableau(dbsksp_xgraph_tableau_sptr shock_tab, 
                      const dbsksp_xgraph_storage_sptr& xgraph_storage) const
{
  if (!shock_tab || !xgraph_storage)
    return false;

  shock_tab->clear();

  // set shock graph
  shock_tab->set_shock_graph(xgraph_storage->xgraph());

  // set image
  if (xgraph_storage->image())
  {
    shock_tab->set_child(vgui_image_tableau_new(*xgraph_storage->image()));
  }


  // Display elements of xshock graph
  shock_tab->add_xgraph_elms_to_display();

  // list of fragments
  shock_tab->set_current_grouping("xgraph_default");

  // active edge
  if (xgraph_storage->active_xedge())   
  {   
    dbsksp_xshock_edge_sptr xe = xgraph_storage->active_xedge();   

    // reference line   
    vgl_point_2d<double > seg_start = xe->source()->pt();   
    vgl_point_2d<double > seg_end = xe->target()->pt();   
    vgui_soview2D_lineseg* line = shock_tab->add_line(float(seg_start.x()),   
      float(seg_start.y()), float(seg_end.x()), float(seg_end.y()));   
    line->set_style(vgui_style::new_style(1.0f, 1.0f, 0.0, 6.0, 6.0));   
    line->set_selectable(false);   
  }   

  // active node   
  if (xgraph_storage->active_xnode())   
  {   
    dbsksp_xshock_node_sptr xv = xgraph_storage->active_xnode();   
    vgl_point_2d<double > pt = xv->pt();   
    vgui_soview2D_circle* circle = shock_tab->add_circle(pt.x(), pt.y(), xv->radius());   
    circle->set_style(vgui_style::new_style(1.0f, 0.5f, 0.0f, 2.0f, 2.0f));   
    circle->set_selectable(false);   

    vgui_soview2D_point* center = shock_tab->add_point(pt.x(), pt.y());   
    center->set_style(vgui_style::new_style(1.0f, 0.5f, 0.0f, 9.0f, 9.0f));   
    center->set_selectable(false);   
  } 


  // Add the shock fragments
  vcl_vector<dbsksp_xshock_fragment_sptr >& xfrag_list = xgraph_storage->xfrag_list();
  for (unsigned i =0; i < xfrag_list.size(); ++i)
  {
    shock_tab->add_xfrag(xfrag_list[i]);
  }


  // Add the extrinsic shock samples
  for (unsigned i =0; i < xgraph_storage->xsamples()->size(); ++i)
  {
    shock_tab->add_xsample(xgraph_storage->xsamples()->at(i));
  }
     
  shock_tab->set_current_grouping( "default" );
  return true;
}


