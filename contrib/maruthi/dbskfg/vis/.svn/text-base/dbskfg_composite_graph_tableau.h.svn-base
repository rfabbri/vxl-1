// This is brcv/shp/dbskfg/vis/dbskfg_composite_graph_tableau.h
#ifndef dbskfg_composite_graph_tableau_h_
#define dbskfg_composite_graph_tableau_h_
//:
// \file
// \brief  Tableau for dbskfg_composite_graph
// \author Maruthi Narayanan (mn@lems.brown.edu)
// \date   July 07, 2010



#include <dbskfg/vis/dbskfg_composite_graph_tableau_sptr.h>
#include <dbskfg/algo/dbskfg_detect_transforms_sptr.h>
#include <dbskfg/algo/dbskfg_transform_descriptor_sptr.h>
#include <dbskfg/dbskfg_composite_node_sptr.h>
#include <dbskfg/dbskfg_composite_link_sptr.h>
#include <dbskfg/dbskfg_composite_graph_sptr.h>
#include <dbskfg/dbskfg_rag_graph_sptr.h>
#include <dbskfg/dbskfg_rag_node_sptr.h>
#include <vgui/vgui_tableau.h>
#include <vgui/vgui_style.h>
#include <bgui/bgui_vsol2D_tableau.h>
#include <dbskfg/vis/dbskfg_soview_composite_link.h>


class dbskfg_soview_transform;
class dbskfg_soview_composite_node;
class dbskfg_soview_rag_node;

//: A tableau to display shock objects
class dbskfg_composite_graph_tableau : public bgui_vsol2D_tableau
{
 
public:
    
    // Constructors / Destructors ----------------------------------------------

    //: Constructor - don't use this, use dbsksp_xgraph_tableau_new.
    // The child tableau is added later using add_child. 
    dbskfg_composite_graph_tableau(const char* n="unnamed" );
      
    dbskfg_composite_graph_tableau(vgui_image_tableau_sptr const& it,
    const char* n="unnamed") : bgui_vsol2D_tableau(it, n)
    {}
  
    dbskfg_composite_graph_tableau(
        vgui_tableau_sptr const& t,
        const char* n="unnamed") : bgui_vsol2D_tableau(t, n){};
        
    // Destructor
    /* virtual */ 
    ~dbskfg_composite_graph_tableau(){};

    // Data access--------------------------------------------------------------
  
    //: Return the composite graph
    dbskfg_composite_graph_sptr composite_graph() const
    { return this->composite_graph_; }

    //: Set the composite graph smartpointer
    void set_composite_graph(
        const dbskfg_composite_graph_sptr & composite_graph)
    {this->composite_graph_ = composite_graph; }

    //: Set the rag graph smartpointer
    void set_rag_graph(
        const dbskfg_rag_graph_sptr & rag_graph)
    {this->rag_graph_ = rag_graph; }

    //: Set the transforms smartpointer
    void set_transforms( dbskfg_detect_transforms_sptr new_transforms ) 
    { transforms_ = new_transforms; }

    //: Returns the type of tableau ('dbskfg_composite_graph_tableau').
    vcl_string type_name() const { return "dbskfg_composite_graph_tableau"; }


    // GUI----------------------------------------------------------------------
  
    //: Add given menu to the tableau popup menu
    void add_popup(vgui_menu& menu);

  
    // Utilities----------------------------------------------------------------

    //: Handle all events sent to this tableau.
    bool handle(const vgui_event& e);

    //: Display for composite graph
    bool add_composite_graph_elms_to_display();
    
    //: Display transforms
    bool add_transforms_to_display();

 protected:


    // Member methods

    //: Display for composite graph shock/contour nodes
    dbskfg_soview_composite_node* add_composite_node(
        const dbskfg_composite_node_sptr& xv,
        const vgui_style_sptr& style,
        vcl_string grouping);

    //: Display for composite contour links
    dbskfg_soview_contour_link* add_contour_link(
        dbskfg_contour_link* link,
        const vgui_style_sptr& style,
        vcl_string grouping);

    //: Display for composite shock links
    dbskfg_soview_shock_link* add_shock_link(
        dbskfg_shock_link* link,
        const vgui_style_sptr& style,
        vcl_string grouping);


    //: Display for composite shock rays
    dbskfg_soview_shock_ray* add_shock_ray(
        dbskfg_shock_link* link,
        const vgui_style_sptr& style,
        vcl_string grouping);


    //: Display for Rag Nodes
    dbskfg_soview_rag_node* add_rag_node(
        const dbskfg_rag_node_sptr& xv,
        const vgui_style_sptr& style,
        vcl_string grouping);

    //: Display for Transforms
    dbskfg_soview_transform* add_transform(
        const dbskfg_transform_descriptor_sptr& xv,
        const vgui_style_sptr style,
        vcl_string grouping);

    // Member variables


    //: composite graph and region adjancency graph
    dbskfg_composite_graph_sptr composite_graph_;
    dbskfg_rag_graph_sptr rag_graph_;
    dbskfg_detect_transforms_sptr transforms_;

    //: display selections
    bool display_composite_contour_nodes_;
    bool display_composite_shock_nodes_;
    bool display_composite_contour_links_;
    bool display_composite_shock_rays_;
    bool display_composite_shock_links_;
    bool display_rag_graph_;
    bool display_transforms_;

    //: drawing styles
    vgui_style_sptr composite_node_contour_curve_style_; 
    vgui_style_sptr composite_node_shock_curve_style_;
    vgui_style_sptr composite_link_shock_ray_curve_style_;
    vgui_style_sptr composite_link_shock_edge_curve_style_;
    vgui_style_sptr composite_link_contour_curve_style_;
    vgui_style_sptr rag_node_curve_style_;
    vgui_style_sptr transform_curve_style_;
    
};

//: Create a smart-pointer to a dbskfg_composite_graph_tableau.
struct dbskfg_composite_graph_tableau_new : 
public dbskfg_composite_graph_tableau_sptr 
{

  typedef dbskfg_composite_graph_tableau_sptr base;

  //
  dbskfg_composite_graph_tableau_new(const char* n="unnamed") : 
  base(new dbskfg_composite_graph_tableau(n)) { }
  
  dbskfg_composite_graph_tableau_new(vgui_image_tableau_sptr const& it,
                                     const char* n="unnamed") : 
  base(new dbskfg_composite_graph_tableau(it,n)) { }

  dbskfg_composite_graph_tableau_new(vgui_tableau_sptr const& t, 
                                     const char* n="unnamed") : 
  base(new dbskfg_composite_graph_tableau(t, n)) { }
  
  operator vgui_easy2D_tableau_sptr () const 
  { vgui_easy2D_tableau_sptr tt; tt.vertical_cast(*this); return tt; }

};

#endif // dbskfg_composite_graph_tableau_h_

