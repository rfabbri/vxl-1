// This is shp/dbsksp/vis/dbsksp_shock_tableau.h
#ifndef dbsksp_shock_tableau_h_
#define dbsksp_shock_tableau_h_
//:
// \file
// \brief  Tableau for shape modelling
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date   Sep 28, 2006



#include "dbsksp_shock_tableau_sptr.h"

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_style.h>
#include <bgui/bgui_vsol2D_tableau.h>

#include <dbsksp/dbsksp_shock_graph_sptr.h>
#include <dbsksp/dbsksp_shock_node_sptr.h>
#include <dbsksp/dbsksp_shock_edge_sptr.h>
#include <dbsksp/dbsksp_bnd_arc_sptr.h>
#include <dbsksp/dbsksp_shapelet_sptr.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_fragment_sptr.h>

class dbsksp_soview_shock_node;
class dbsksp_soview_shock_edge_chord;
class dbsksp_soview_shock_geom;
class dbsksp_soview_contact_shock;
class dbsksp_soview_bnd_arc;
class dbsksp_soview_shapelet;
class dbsksp_soview_xfrag;
class dbsksp_soview_xshock_node;
class dbsksp_soview_xshock_chord;
class dbsksp_soview_xshock_contact;

//: A tableau to display shock objects
class dbsksp_shock_tableau : public bgui_vsol2D_tableau
{
 
public:
  //: Constructor - don't use this, use dbsksp_shock_tableau_new.
  //  The child tableau is added later using add_child. 
  dbsksp_shock_tableau(const char* n="unnamed" ): bgui_vsol2D_tableau(n), 
    ////
    //shock_geom_style_(vgui_style::new_style(1.0f, 0.0f, 0.0f, 2.0f, 2.0f)),
    //shock_chord_style_(vgui_style::new_style(1.0f, 0.0f, 1.0f, 2.0f, 2.0f)),
    //contact_shock_style_(vgui_style::new_style(0.5f, 0, 1.0f, 1.0f, 1.0f)),
    //bnd_style_(vgui_style::new_style(0.1f, 0.1f, 1.0f, 2.0f, 2.0f)),
    //shapelet_style_(vgui_style::new_style(0.1f, 1.0f, 0.1f, 2.0f, 2.0f))

    shock_geom_style_(vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f, 3.0f)),
    shock_chord_style_(vgui_style::new_style(0.5f, 0.9f, 1.0f, 3.0f, 3.0f)),
    contact_shock_style_(vgui_style::new_style(0.1f, 1.0f, 0.5f, 3.0f, 3.0f)),
    bnd_style_(vgui_style::new_style(0.1f, 0.1f, 1.0f, 3.0f, 3.0f)),
    shapelet_style_(vgui_style::new_style(0.1f, 1.0f, 0.1f, 3.0f, 3.0f)),
    shock_node_style_(vgui_style::new_style(0.0f, 1.0f, 0.0f, 6.0f, 1.0f)),

    //xshock_chord_style_(vgui_style::new_style(0.5f, 0.9f, 1.0f, 3.0f, 3.0f)),
    //xshock_node_style_(vgui_style::new_style(0.0f, 1.0f, 0.0f, 6.0f, 1.0f))
    xshock_chord_style_(vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f, 3.0f)),
    xshock_node_style_(vgui_style::new_style(0.0f, 0.0f, 1.0f, 6.0f, 1.0f))
  {
  }
      
  
  dbsksp_shock_tableau(vgui_image_tableau_sptr const& it,
    const char* n="unnamed") : bgui_vsol2D_tableau(it, n),
    contact_shock_style_(vgui_style::new_style(0.5f, 0,1, 1.0, 1.0))
  {}
  
  dbsksp_shock_tableau(vgui_tableau_sptr const& t,
    const char* n="unnamed") : bgui_vsol2D_tableau(t, n),
  contact_shock_style_(vgui_style::new_style(0.5f, 0,1, 1.0, 1.0)){};

  // Destructor
  virtual ~dbsksp_shock_tableau(){};

  // ======== Data access ===============================
  
  //: Return the shock graph
  dbsksp_shock_graph_sptr shock_graph() const
  { return this->shock_graph_; }

  //: Set the shockshape smartpointer
  void set_shock_graph(const dbsksp_shock_graph_sptr & shock_graph)
  {this->shock_graph_ = shock_graph; }

  //: Returns the type of tableau ('dbsksp_shock_tableau').
  vcl_string type_name() const { return "dbsksp_shock_tableau"; }

  // ============ GUI ========================================
  
  //: Add given menu to the tableau popup menu
  void add_popup(vgui_menu& menu);
  


  // ========= UTILITY =====================================

  //: Handle all events sent to this tableau.
  bool handle(const vgui_event& e);

  //: Display for shock node
  dbsksp_soview_shock_node* add_shock_node(const dbsksp_shock_node_sptr& node,
    const vgui_style_sptr& style = 0);

  //: Display for the chord of a shock edge
  dbsksp_soview_shock_edge_chord* add_shock_edge_chord(
    const dbsksp_shock_edge_sptr& e, const vgui_style_sptr& style = 0);

  //: Display for the geometry (a conic) of a shock edge
  dbsksp_soview_shock_geom* add_shock_geom(
    const dbsksp_shock_edge_sptr& e, const vgui_style_sptr& style = 0);


  //: Display for contact shocks around a node
  dbsksp_soview_contact_shock* add_contact_shock(
    const dbsksp_shock_node_sptr& node, const vgui_style_sptr& style = 0);

  //: Display for a boundary circular arc
  dbsksp_soview_bnd_arc* add_bnd_arc(
    const dbsksp_bnd_arc_sptr& bnd, const vgui_style_sptr& style = 0);


  //: Display for a shapelet
  dbsksp_soview_shapelet* add_shapelet(
    const dbsksp_shapelet_sptr& shapelet, const vgui_style_sptr& style = 0);

  dbsksp_soview_shapelet* add_shapelet(
    const dbsksp_shapelet_sptr& shapelet,
    const vgui_style_sptr& shock_chord_style,
    const vgui_style_sptr& shock_geom_style, 
    const vgui_style_sptr& contact_shock_style, 
    const vgui_style_sptr& bnd_style);

  //: Display an extrinsic shape fragment
  dbsksp_soview_xfrag* add_xfrag(const dbsksp_xshock_fragment_sptr& xfrag);

  //: Display for xshock node
  dbsksp_soview_xshock_node* add_xshock_node(const dbsksp_xshock_node_sptr& xv,
    const vgui_style_sptr& style = 0);


  //: Display for contact xshock
  dbsksp_soview_xshock_contact* add_xshock_contact(const dbsksp_xshock_node_sptr& xv,
    const vgui_style_sptr& style = 0);

  //: Display chord of an extrinsic shock edge
  dbsksp_soview_xshock_chord* add_xshock_chord(const dbsksp_xshock_edge_sptr& xe,
    const vgui_style_sptr& style = 0);


  //: Display boundary of an extrinsic shock edge
  bool add_xshock_bnd(const dbsksp_xshock_edge_sptr& xe, 
    const vgui_style_sptr& style = 0);


    // ========= MEMBER VARIABLES =========================
 protected:
  
  //: the shock graph
  dbsksp_shock_graph_sptr shock_graph_;

  // display selections
  static bool display_shock_graph_elms_;
  static bool display_shock_node_;
  static bool display_shock_edge_chord_;
  static bool display_shock_geom_;
  static bool display_contact_shock_;
  static bool display_bnd_arc_;
  static bool display_shapelet_;
  static bool display_others_;

  //: drawing styles
  // intrinsic shock
  vgui_style_sptr shock_geom_style_;
  vgui_style_sptr shock_chord_style_;
  vgui_style_sptr contact_shock_style_;
  vgui_style_sptr shock_node_style_;

  // extrinsic shock
  vgui_style_sptr xshock_chord_style_;
  vgui_style_sptr xshock_node_style_;

  vgui_style_sptr bnd_style_;
  vgui_style_sptr shapelet_style_;

};

//: Create a smart-pointer to a dbsksp_shock_tableau.
struct dbsksp_shock_tableau_new : public dbsksp_shock_tableau_sptr {
  typedef dbsksp_shock_tableau_sptr base;

  dbsksp_shock_tableau_new(const char* n="unnamed") : 
  base(new dbsksp_shock_tableau(n)) { }
  
  dbsksp_shock_tableau_new(vgui_image_tableau_sptr const& it,
    const char* n="unnamed") : 
  base(new dbsksp_shock_tableau(it,n)) { }

  dbsksp_shock_tableau_new(vgui_tableau_sptr const& t, const char* n="unnamed") : 
  base(new dbsksp_shock_tableau(t, n)) { }

  operator vgui_easy2D_tableau_sptr () const 
  { vgui_easy2D_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // dbsksp_shock_tableau_h_

