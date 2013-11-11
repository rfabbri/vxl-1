// This is shp/dbsksp/vis/dbsksp_xgraph_tableau.h
#ifndef dbsksp_xgraph_tableau_h_
#define dbsksp_xgraph_tableau_h_
//:
// \file
// \brief  Tableau for dbsksp_xshock_graph
// \author Nhon Trinh (ntrinh@lems.brown.edu)
// \date   Oct 21, 2009



#include "dbsksp_xgraph_tableau_sptr.h"

#include <vgui/vgui_tableau.h>
#include <vgui/vgui_style.h>
#include <bgui/bgui_vsol2D_tableau.h>

#include <dbsksp/dbsksp_xshock_graph_sptr.h>
#include <dbsksp/dbsksp_xshock_edge_sptr.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_fragment_sptr.h>
#include <dbsksp/dbsksp_xshock_node_descriptor.h>

class dbsksp_soview_xfrag;
class dbsksp_soview_xshock_node;
class dbsksp_soview_xshock_chord;
class dbsksp_soview_xshock_contact;
class dbsksp_soview_xshock_curve;
class dbsksp_soview_xsample;

//: A tableau to display shock objects
class dbsksp_xgraph_tableau : public bgui_vsol2D_tableau
{
 
public:
  // Constructors / Destructors ------------------------------------------------

  //: Constructor - don't use this, use dbsksp_xgraph_tableau_new.
  // The child tableau is added later using add_child. 
  dbsksp_xgraph_tableau(const char* n="unnamed" );
      
  
  dbsksp_xgraph_tableau(vgui_image_tableau_sptr const& it,
    const char* n="unnamed") : bgui_vsol2D_tableau(it, n)
  {}
  
  dbsksp_xgraph_tableau(vgui_tableau_sptr const& t,
    const char* n="unnamed") : bgui_vsol2D_tableau(t, n){};

  // Destructor
  virtual ~dbsksp_xgraph_tableau(){};

  // Data access----------------------------------------------------------------
  
  //: Return the shock graph
  dbsksp_xshock_graph_sptr shock_graph() const
  { return this->shock_graph_; }

  //: Set the shockshape smartpointer
  void set_shock_graph(const dbsksp_xshock_graph_sptr & shock_graph)
  {this->shock_graph_ = shock_graph; }

  //: Returns the type of tableau ('dbsksp_xgraph_tableau').
  vcl_string type_name() const { return "dbsksp_xgraph_tableau"; }


  ////: Get child image tableau
  //vgui_tableau_sptr get_child() const;
  //

  // GUI------------------------------------------------------------------------
  
  //: Add given menu to the tableau popup menu
  void add_popup(vgui_menu& menu);
  


  // Utilities------------------------------------------------------------------

  //: Handle all events sent to this tableau.
  bool handle(const vgui_event& e);

  //: Add elements of xgraph to display list
  bool add_xgraph_elms_to_display();
 
  //: Display for xshock node
  dbsksp_soview_xshock_node* add_xshock_node(const dbsksp_xshock_node_sptr& xv,
    const vgui_style_sptr& style = 0);

  //: Display for contact xshock
  dbsksp_soview_xshock_contact* add_xshock_contact(const dbsksp_xshock_node_sptr& xv,
    const vgui_style_sptr& style = 0);

  //: Display chord of an extrinsic shock edge
  dbsksp_soview_xshock_chord* add_xshock_chord(const dbsksp_xshock_edge_sptr& xe,
    const vgui_style_sptr& style = 0);

  //: Display for the geometry of a shock edge (approximated with biarc)
  dbsksp_soview_xshock_curve* add_xshock_curve(const dbsksp_xshock_edge_sptr& xe, 
    const vgui_style_sptr& style = 0);


  //: Display boundary of an extrinsic shock edge
  bool add_xshock_bnd(const dbsksp_xshock_edge_sptr& xe, 
    const vgui_style_sptr& style = 0);

  //: Display an extrinsic shape fragment
  dbsksp_soview_xfrag* add_xfrag(const dbsksp_xshock_fragment_sptr& xfrag);

  //: Display an extrinsic shock sample
  dbsksp_soview_xsample* add_xsample(const dbsksp_xshock_node_descriptor& xsample,
    const vgui_style_sptr& style = 0);



  // display selections
  static bool display_shock_graph_elms_;
  static bool display_shock_node_;
  static bool display_shock_edge_chord_;
  static bool display_shock_edge_curve_;
  static bool display_shock_bnd_;
  static bool display_contact_shock_;
  static bool display_others_;

   
  // Member variables-----------------------------------------------------------
 protected:
  
  //: the shock graph
  dbsksp_xshock_graph_sptr shock_graph_;

  //: drawing styles
  vgui_style_sptr shock_edge_curve_style_;
  vgui_style_sptr shock_edge_chord_style_;
  vgui_style_sptr contact_shock_style_;
  vgui_style_sptr shock_node_style_;
  vgui_style_sptr shock_bnd_style_;
  vgui_style_sptr shock_fragment_style_;
  vgui_style_sptr shock_sample_style_;

};

//: Create a smart-pointer to a dbsksp_xgraph_tableau.
struct dbsksp_xgraph_tableau_new : public dbsksp_xgraph_tableau_sptr {
  typedef dbsksp_xgraph_tableau_sptr base;

  //
  dbsksp_xgraph_tableau_new(const char* n="unnamed") : 
  base(new dbsksp_xgraph_tableau(n)) { }
  
  dbsksp_xgraph_tableau_new(vgui_image_tableau_sptr const& it,
    const char* n="unnamed") : 
  base(new dbsksp_xgraph_tableau(it,n)) { }

  dbsksp_xgraph_tableau_new(vgui_tableau_sptr const& t, const char* n="unnamed") : 
  base(new dbsksp_xgraph_tableau(t, n)) { }

  operator vgui_easy2D_tableau_sptr () const 
  { vgui_easy2D_tableau_sptr tt; tt.vertical_cast(*this); return tt; }
};

#endif // dbsksp_xgraph_tableau_h_

