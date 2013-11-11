// This is shp/dbsksp/vis/dbsksp_shock_tableau.cxx
//:
// \file



#include "dbsksp_shock_tableau.h"
#include <dbsksp/vis/dbsksp_soview_shock.h>
#include <dbsksp/vis/dbsksp_soview_xshock.h>
#include <dbsksp/dbsksp_shock_edge.h>
#include <dbsksp/dbsksp_shock_node.h>

#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>


class dbsksp_shock_tableau_toggle_command : public vgui_command
{
 public:
  dbsksp_shock_tableau_toggle_command(dbsksp_shock_tableau* tab, const void* boolref) : 
       shock_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    shock_tableau->post_redraw(); 
  }

  dbsksp_shock_tableau *shock_tableau;
  bool* bref;
};







// display selections
bool dbsksp_shock_tableau::display_shock_graph_elms_ = true;;
bool dbsksp_shock_tableau::display_shock_node_ = true;
bool dbsksp_shock_tableau::display_shock_edge_chord_ = true;
bool dbsksp_shock_tableau::display_shock_geom_ = true;
bool dbsksp_shock_tableau::display_contact_shock_ = true;
bool dbsksp_shock_tableau::display_bnd_arc_ = true;
bool dbsksp_shock_tableau::display_shapelet_ = true;
bool dbsksp_shock_tableau::display_others_ = true;






//: Add given menu to the tableau popup menu
void dbsksp_shock_tableau::
add_popup(vgui_menu& menu)
{
  vgui_menu submenu;
  vcl_string on = "[x] ", off = "[ ] ";

  submenu.add( ((this->display_shock_graph_elms_)?on:off) + "Display Shock Graph Elements", 
    new dbsksp_shock_tableau_toggle_command(this, &(this->display_shock_graph_elms_)) );
  submenu.add( ((this->display_shock_node_)?on:off) + "Display Shock Nodes", 
    new dbsksp_shock_tableau_toggle_command(this, &(this->display_shock_node_)) );
  submenu.add( ((this->display_shock_edge_chord_)?on:off) + "Display Shock Chords", 
    new dbsksp_shock_tableau_toggle_command(this, &(this->display_shock_edge_chord_)) );
  submenu.add( ((this->display_shock_geom_)?on:off) + "Display Shock Curves", 
    new dbsksp_shock_tableau_toggle_command(this, &(this->display_shock_geom_)) );
  submenu.add( ((this->display_contact_shock_)?on:off) + "Display Contact Shocks", 
    new dbsksp_shock_tableau_toggle_command(this, &(this->display_contact_shock_)) );
  submenu.add( ((this->display_bnd_arc_)?on:off) + "Display Shock Boundary", 
    new dbsksp_shock_tableau_toggle_command(this, &(this->display_bnd_arc_)) );
  submenu.add( ((this->display_shapelet_)?on:off) + "Display Shapelets", 
    new dbsksp_shock_tableau_toggle_command(this, &(this->display_shapelet_)) );
  submenu.add( ((this->display_others_)?on:off) + "Display Other Elements", 
    new dbsksp_shock_tableau_toggle_command(this, &(this->display_others_)) );


  

  //add this submenu to the popup menu
  menu.add("Shock Tableau Options", submenu);
}





//-----------------------------------------------------------------------------
//: Handle all events sent to this tableau.
bool dbsksp_shock_tableau::
handle(const vgui_event &e)
{
  // set the display (hide/show) flags of the soviews

  for (vcl_map< vcl_string , vgui_displaybase_tableau_grouping >::iterator it 
    = groupings.begin(); it != groupings.end(); it++ )
  {
    vcl_string group_name = it->first;
    
    
    if (group_name == "dbsksp_soview_shock_node")
      it->second.hide = !(display_shock_node_ && display_shock_graph_elms_);
    else if (group_name == "dbsksp_soview_shock_edge_chord")
      it->second.hide = !(display_shock_edge_chord_ && display_shock_graph_elms_);
    else if (group_name == "dbsksp_soview_shock_geom")
      it->second.hide = !(display_shock_geom_ && display_shock_graph_elms_);
    else if (group_name == "dbsksp_soview_contact_shock")
      it->second.hide = !(display_contact_shock_ && display_shock_graph_elms_);
    else if (group_name == "dbsksp_soview_bnd_arc")
      it->second.hide = !(display_bnd_arc_ && display_shock_graph_elms_);
    else if (group_name == "dbsksp_soview_shapelet")
      it->second.hide = !display_shapelet_;
    else
      it->second.hide = !display_others_;
  }


  return bgui_vsol2D_tableau::handle(e);
}

//: Display for shock node
dbsksp_soview_shock_node* dbsksp_shock_tableau::
add_shock_node(const dbsksp_shock_node_sptr& node,
               const vgui_style_sptr& style)
{
  dbsksp_soview_shock_node* obj = new dbsksp_soview_shock_node(node);
  
  this->set_current_grouping(obj->type_name());
  this->add(obj);
  
  if (style)
    obj->set_style(style);
  else
    obj->set_style(this->shock_node_style_);
  return obj;
}




//: Display for the chord of a shock edge
dbsksp_soview_shock_edge_chord* dbsksp_shock_tableau::
add_shock_edge_chord(const dbsksp_shock_edge_sptr& e, const vgui_style_sptr& style)
{
  dbsksp_soview_shock_edge_chord* obj =
    new dbsksp_soview_shock_edge_chord(e);

  this->set_current_grouping(obj->type_name());
  this->add(obj);
  
  if (style)
    obj->set_style( style );
  else
    obj->set_style( this->shock_chord_style_);
  return obj;
}





//: Display for the geometry (a conic) of a shock edge
dbsksp_soview_shock_geom* dbsksp_shock_tableau::
add_shock_geom(const dbsksp_shock_edge_sptr& e, const vgui_style_sptr& style)
{
  dbsksp_soview_shock_geom* obj =
    new dbsksp_soview_shock_geom(e);

  this->set_current_grouping(obj->type_name());
  this->add(obj);

  if (style)
    obj->set_style( style );
  else
    obj->set_style(shock_geom_style_);
  return obj;
}




// -----------------------------------------------------------------------------
//: Display for contact shocks around a node
dbsksp_soview_contact_shock* dbsksp_shock_tableau::
add_contact_shock(const dbsksp_shock_node_sptr& node, 
                  const vgui_style_sptr& style)
{
  dbsksp_soview_contact_shock* obj = new dbsksp_soview_contact_shock(node);
  
  this->set_current_grouping(obj->type_name());
  this->add(obj);
  
  if (style)
    obj->set_style(style);
  else
    obj->set_style(this->contact_shock_style_);
  return obj;
}



// -----------------------------------------------------------------------------
//: Display for a boundary circular arc
dbsksp_soview_bnd_arc* dbsksp_shock_tableau::
add_bnd_arc(const dbsksp_bnd_arc_sptr& bnd, 
            const vgui_style_sptr& style)
{
  dbsksp_soview_bnd_arc* obj = new dbsksp_soview_bnd_arc(bnd);
  
  this->set_current_grouping(obj->type_name());
  this->add(obj);
  
  if (style)
    obj->set_style(style);
  else
    obj->set_style(this->bnd_style_);
  return obj;
}


// -----------------------------------------------------------------------------
//: Dispaly a shapelet
dbsksp_soview_shapelet* dbsksp_shock_tableau::
add_shapelet(const dbsksp_shapelet_sptr& shapelet, const vgui_style_sptr& style)
{
  if (style)
    return this->add_shapelet(shapelet, style, style, style, style);
  else
    return this->add_shapelet(shapelet, this->shock_chord_style_,
    this->shock_geom_style_,
    this->contact_shock_style_,
    this->bnd_style_);
}



// -----------------------------------------------------------------------------
//: Display a shapelet
dbsksp_soview_shapelet* dbsksp_shock_tableau::
add_shapelet(const dbsksp_shapelet_sptr& shapelet,
             const vgui_style_sptr& shock_chord_style,
             const vgui_style_sptr& shock_geom_style, 
             const vgui_style_sptr& contact_shock_style, 
             const vgui_style_sptr& bnd_style)
{
  dbsksp_soview_shapelet* obj = new dbsksp_soview_shapelet(shapelet);
  
  this->set_current_grouping(obj->type_name());
  this->add(obj);
  
  obj->set_style(shock_geom_style);
  obj->set_shapelet_styles(shock_chord_style, 
    shock_geom_style, 
    contact_shock_style,
    bnd_style);
  return obj;
}



// -----------------------------------------------------------------------------
//: Display an extrinsic shape fragment
dbsksp_soview_xfrag* dbsksp_shock_tableau::
add_xfrag(const dbsksp_xshock_fragment_sptr& xfrag)
{
  dbsksp_soview_xfrag* obj = new dbsksp_soview_xfrag(xfrag);
  this->set_current_grouping(obj->type_name());
  this->add(obj);
  
  obj->set_style(this->shock_geom_style_);
  obj->set_xfrag_styles(this->shock_chord_style_, this->contact_shock_style_, 
    this->bnd_style_);
  return obj;
}


// -----------------------------------------------------------------------------
//: Display for xshock node
dbsksp_soview_xshock_node* dbsksp_shock_tableau::
add_xshock_node(const dbsksp_xshock_node_sptr& xv, const vgui_style_sptr& style)
{
  dbsksp_soview_xshock_node* obj = new dbsksp_soview_xshock_node(xv);
  
  this->set_current_grouping(obj->type_name());
  this->add(obj);
  
  if (style)
    obj->set_style(style);
  else
    obj->set_style(this->xshock_node_style_);
  return obj;
}



// -----------------------------------------------------------------------------
//: Display for contact xshock
dbsksp_soview_xshock_contact* dbsksp_shock_tableau::
add_xshock_contact(const dbsksp_xshock_node_sptr& xv, 
                   const vgui_style_sptr& style)
{
  dbsksp_soview_xshock_contact* obj = new dbsksp_soview_xshock_contact(xv);
  obj->set_selectable(false);
  this->set_current_grouping(obj->type_name());
  this->add(obj);
  
  if (style)
    obj->set_style(style);
  else
    obj->set_style(this->contact_shock_style_);
  return obj;
}


// -----------------------------------------------------------------------------
//: Display chord of an extrinsic shock edge
dbsksp_soview_xshock_chord* dbsksp_shock_tableau::
add_xshock_chord(const dbsksp_xshock_edge_sptr& xe, const vgui_style_sptr& style)
{
  // add object to the drawing list
  dbsksp_soview_xshock_chord* obj = new dbsksp_soview_xshock_chord(xe);
  this->set_current_grouping(obj->type_name());
  this->add(obj);
  
  // set drawing style
  if (style)
    obj->set_style( style );
  else
    obj->set_style( this->xshock_chord_style_);
  return obj;
}



// -----------------------------------------------------------------------------
//: Display boundary of an extrinsic shock edge
bool dbsksp_shock_tableau::
add_xshock_bnd(const dbsksp_xshock_edge_sptr& xe, const vgui_style_sptr& style)
{
  // left boundary
  // add object to the drawing list
  dbsksp_soview_xshock_bnd* left_bnd = new dbsksp_soview_xshock_bnd(xe, true);
  this->set_current_grouping(left_bnd->type_name());
  this->add(left_bnd);
  
  // set drawing style
  if (style)
    left_bnd->set_style( style );
  else
    left_bnd->set_style( this->bnd_style_);
  
  // right boundary
  // add object to the drawing list
  dbsksp_soview_xshock_bnd* right_bnd = new dbsksp_soview_xshock_bnd(xe, false);
  this->set_current_grouping(right_bnd->type_name());
  this->add(right_bnd);
  
  // set drawing style
  if (style)
    right_bnd->set_style( style );
  else
    right_bnd->set_style( this->bnd_style_);

  return true;
}


