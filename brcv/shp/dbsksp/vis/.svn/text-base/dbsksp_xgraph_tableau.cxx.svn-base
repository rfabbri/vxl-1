// This is shp/dbsksp/vis/dbsksp_xgraph_tableau.cxx
//:
// \file



#include "dbsksp_xgraph_tableau.h"
#include <dbsksp/vis/dbsksp_soview_xshock.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>


//==============================================================================
// dbsksp_xgraph_tableau_toggle_command
//==============================================================================
//:
class dbsksp_xgraph_tableau_toggle_command : public vgui_command
{
 public:
  dbsksp_xgraph_tableau_toggle_command(dbsksp_xgraph_tableau* tab, const void* boolref) : 
       shock_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    shock_tableau->post_redraw(); 
  }

  dbsksp_xgraph_tableau *shock_tableau;
  bool* bref;
};



//==============================================================================
// dbsksp_xgraph_tableau
//==============================================================================

// display selections
bool dbsksp_xgraph_tableau::display_shock_graph_elms_ = true;;
bool dbsksp_xgraph_tableau::display_shock_node_ = true;
bool dbsksp_xgraph_tableau::display_shock_edge_chord_ = true;
bool dbsksp_xgraph_tableau::display_shock_edge_curve_ = true;
bool dbsksp_xgraph_tableau::display_shock_bnd_ = true;
bool dbsksp_xgraph_tableau::display_contact_shock_ = true;
bool dbsksp_xgraph_tableau::display_others_ = true;






//------------------------------------------------------------------------------
//: Constructor - don't use this, use dbsksp_xgraph_tableau_new.
// The child tableau is added later using add_child. 
dbsksp_xgraph_tableau::
dbsksp_xgraph_tableau(const char* n ): bgui_vsol2D_tableau(n), 
shock_edge_curve_style_(vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f, 3.0f)),
shock_edge_chord_style_(vgui_style::new_style(0.6f, 0.15f, 0.05f, 3.0f, 3.0f)),
contact_shock_style_(vgui_style::new_style(0.1f, 1.0f, 0.5f, 3.0f, 3.0f)),
shock_bnd_style_(vgui_style::new_style(0.1f, 0.1f, 1.0f, 3.0f, 3.0f)),
shock_node_style_(vgui_style::new_style(0.0f, 1.0f, 0.0f, 6.0f, 1.0f)),
shock_fragment_style_(vgui_style::new_style(0.1f, 1.0f, 0.1f, 3.0f, 3.0f)),
shock_sample_style_(vgui_style::new_style(0.1f, 0.1f, 0.1f, 2.0f, 1.0f))
{
}



////------------------------------------------------------------------------------
////: Get child image tableau
//vgui_tableau_sptr dbsksp_xgraph_tableau::
//get_child() const
//{
//  return this->image_slot.child();
//}


//: Add given menu to the tableau popup menu
void dbsksp_xgraph_tableau::
add_popup(vgui_menu& menu)
{
  vgui_menu submenu;
  vcl_string on = "[x] ", off = "[ ] ";

  submenu.add( ((this->display_shock_graph_elms_)?on:off) + "Display Shock Graph Elements", 
    new dbsksp_xgraph_tableau_toggle_command(this, &(this->display_shock_graph_elms_)) );

  submenu.add( ((this->display_shock_node_)?on:off) + "Display Shock Nodes", 
    new dbsksp_xgraph_tableau_toggle_command(this, &(this->display_shock_node_)) );

  submenu.add( ((this->display_contact_shock_)?on:off) + "Display Contact Shocks", 
    new dbsksp_xgraph_tableau_toggle_command(this, &(this->display_contact_shock_)) );

  submenu.add( ((this->display_shock_edge_chord_)?on:off) + "Display Shock Chords", 
    new dbsksp_xgraph_tableau_toggle_command(this, &(this->display_shock_edge_chord_)) );

  submenu.add( ((this->display_shock_edge_curve_)?on:off) + "Display Shock Curves", 
    new dbsksp_xgraph_tableau_toggle_command(this, &(this->display_shock_edge_curve_)) );
  
  submenu.add( ((this->display_shock_bnd_)?on:off) + "Display Shock Boundary", 
    new dbsksp_xgraph_tableau_toggle_command(this, &(this->display_shock_bnd_)) );

  submenu.add( ((this->display_others_)?on:off) + "Display Other Elements", 
    new dbsksp_xgraph_tableau_toggle_command(this, &(this->display_others_)) );

  //add this submenu to the popup menu
  menu.add("Shock Tableau Options", submenu);
}





//-----------------------------------------------------------------------------
//: Handle all events sent to this tableau.
bool dbsksp_xgraph_tableau::
handle(const vgui_event &e)
{
  // set the display (hide/show) flags of the soviews
  for (vcl_map< vcl_string , vgui_displaybase_tableau_grouping >::iterator it 
    = groupings.begin(); it != groupings.end(); it++ )
  {
    vcl_string group_name = it->first;
    
    if (group_name == "shock_node")
      it->second.hide = !(display_shock_node_ && display_shock_graph_elms_);
    else if (group_name == "shock_edge_chord")
      it->second.hide = !(display_shock_edge_chord_ && display_shock_graph_elms_);
    else if (group_name == "shock_edge_curve")
      it->second.hide = !(display_shock_edge_curve_ && display_shock_graph_elms_);
    else if (group_name == "contact_shock")
      it->second.hide = !(display_contact_shock_ && display_shock_graph_elms_);
    else if (group_name == "shock_bnd")
      it->second.hide = !(display_shock_bnd_ && display_shock_graph_elms_);
    else
      it->second.hide = !display_others_;
  }


  return bgui_vsol2D_tableau::handle(e);
}



// -----------------------------------------------------------------------------
//: Display for xshock node
dbsksp_soview_xshock_node* dbsksp_xgraph_tableau::
add_xshock_node(const dbsksp_xshock_node_sptr& xv, const vgui_style_sptr& style)
{
  dbsksp_soview_xshock_node* obj = new dbsksp_soview_xshock_node(xv);
  obj->set_selectable(true);
  this->set_current_grouping("shock_node");
  this->add(obj);
  
  if (style)
    obj->set_style(style);
  else
    obj->set_style(this->shock_node_style_);
  return obj;
}



// -----------------------------------------------------------------------------
//: Display for contact xshock
dbsksp_soview_xshock_contact* dbsksp_xgraph_tableau::
add_xshock_contact(const dbsksp_xshock_node_sptr& xv, 
                   const vgui_style_sptr& style)
{
  dbsksp_soview_xshock_contact* obj = new dbsksp_soview_xshock_contact(xv);
  obj->set_selectable(false);
  this->set_current_grouping("contact_shock");
  this->add(obj);
  
  if (style)
    obj->set_style(style);
  else
    obj->set_style(this->contact_shock_style_);
  return obj;
}


//------------------------------------------------------------------------------
//: Display chord of an extrinsic shock edge
dbsksp_soview_xshock_chord* dbsksp_xgraph_tableau::
add_xshock_chord(const dbsksp_xshock_edge_sptr& xe, const vgui_style_sptr& style)
{
  // add object to the drawing list
  dbsksp_soview_xshock_chord* obj = new dbsksp_soview_xshock_chord(xe);
  obj->set_selectable( true );
  this->set_current_grouping("shock_edge_chord");
  this->add(obj);
  
  // set drawing style
  if (style)
    obj->set_style( style );
  else
    obj->set_style( this->shock_edge_chord_style_);
  return obj;
}


//------------------------------------------------------------------------------
//: Display for the geometry of a shock edge (approximated with biarc)
dbsksp_soview_xshock_curve* dbsksp_xgraph_tableau::
add_xshock_curve(const dbsksp_xshock_edge_sptr& xe, 
    const vgui_style_sptr& style)
{
  // add object to the drawing list
  dbsksp_soview_xshock_curve* obj = new dbsksp_soview_xshock_curve(xe);
  obj->set_selectable( false );
  this->set_current_grouping("shock_edge_curve");
  this->add(obj);
  
  // set drawing style
  if (style)
    obj->set_style( style );
  else
    obj->set_style( this->shock_edge_curve_style_);
  return obj;
}



// -----------------------------------------------------------------------------
//: Display boundary of an extrinsic shock edge
bool dbsksp_xgraph_tableau::
add_xshock_bnd(const dbsksp_xshock_edge_sptr& xe, const vgui_style_sptr& style)
{
  // left boundary
  // add object to the drawing list
  dbsksp_soview_xshock_bnd* left_bnd = new dbsksp_soview_xshock_bnd(xe, true);
  left_bnd->set_selectable( false );
  this->set_current_grouping("shock_bnd");
  this->add(left_bnd);

  // set drawing style
  if (style)
    left_bnd->set_style( style );
  else
    left_bnd->set_style( this->shock_bnd_style_);
  
  // right boundary
  // add object to the drawing list
  dbsksp_soview_xshock_bnd* right_bnd = new dbsksp_soview_xshock_bnd(xe, false);
  right_bnd->set_selectable( false );
  this->set_current_grouping("shock_bnd");
  this->add(right_bnd);
  
  // set drawing style
  if (style)
    right_bnd->set_style( style );
  else
    right_bnd->set_style( this->shock_bnd_style_);

  return true;
}




//------------------------------------------------------------------------------
//: Add elements of xgraph to display list
bool dbsksp_xgraph_tableau::
add_xgraph_elms_to_display()
{
  if (!this->shock_graph())
    return false;

  dbsksp_xshock_graph_sptr xgraph = this->shock_graph();

  this->set_current_grouping("xgraph_elms");
  for (dbsksp_xshock_graph::edge_iterator eit = xgraph->edges_begin();
    eit != xgraph->edges_end(); ++eit)
  {
    dbsksp_xshock_edge_sptr xe = *eit;

    // extrinsic chord
    this->add_xshock_chord(xe);

    // extrinsic shock curve
    this->add_xshock_curve(xe);

    // extrinsic boundary
    this->add_xshock_bnd(xe);
  }

  // add the nodes
  for (dbsksp_xshock_graph::vertex_iterator vit = xgraph->vertices_begin();
    vit != xgraph->vertices_end(); ++vit)
  {
    // ignore degenerate nodes
    if ((*vit)->degree() == 1) continue;

    this->add_xshock_node(*vit);
    this->add_xshock_contact(*vit);
  }

  return true;
}









//------------------------------------------------------------------------------
//: Display an extrinsic shape fragment
dbsksp_soview_xfrag* dbsksp_xgraph_tableau::
add_xfrag(const dbsksp_xshock_fragment_sptr& xfrag)
{
  dbsksp_soview_xfrag* obj = new dbsksp_soview_xfrag(xfrag);
  this->set_current_grouping("shock_fragment");
  obj->set_selectable( false );
  this->add(obj);
  
  obj->set_style(this->shock_edge_curve_style_);
  obj->set_xfrag_styles(this->shock_edge_chord_style_, this->contact_shock_style_, 
    this->shock_bnd_style_);
  return obj;
}

//------------------------------------------------------------------------------
//: Display an extrinsic shock sample
dbsksp_soview_xsample* dbsksp_xgraph_tableau::
add_xsample(const dbsksp_xshock_node_descriptor& xsample,
            const vgui_style_sptr& style)
{
  dbsksp_soview_xsample* obj = new dbsksp_soview_xsample(xsample);
  this->set_current_grouping("shock_sample");
  obj->set_selectable( true ); //false );
  this->add(obj);
    
  // set drawing style
  if (style)
    obj->set_style( style );
  else
    obj->set_style( this->shock_sample_style_ );
  return obj;
}

