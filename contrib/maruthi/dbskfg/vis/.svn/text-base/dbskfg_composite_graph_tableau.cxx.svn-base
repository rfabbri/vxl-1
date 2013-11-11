// This is brcv/shp/dbskfg/vis/dbskfg_composite_graph_tableau.cxx
//:
// \file


#include <dbskfg/algo/dbskfg_detect_transforms.h>
#include <dbskfg/vis/dbskfg_composite_graph_tableau.h>
#include <dbskfg/vis/dbskfg_soview_composite_node.h>
#include <dbskfg/vis/dbskfg_soview_rag_node.h>
#include <dbskfg/vis/dbskfg_soview_transform.h>
#include <dbskfg/dbskfg_composite_graph.h>
#include <dbskfg/dbskfg_rag_graph.h>
#include <dbskfg/dbskfg_shock_link.h>
#include <dbskfg/dbskfg_contour_link.h>

#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vcl_cstdlib.h>

//==============================================================================
// dbskfg_composite_graph_tableau_toggle_command
//==============================================================================
//:
class dbskfg_composite_graph_tableau_toggle_command : public vgui_command
{
 public:
  dbskfg_composite_graph_tableau_toggle_command
  (dbskfg_composite_graph_tableau* tab, const void* boolref) : 
       composite_graph_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
      *bref = !(*bref);
      composite_graph_tableau->post_redraw(); 
  }

  dbskfg_composite_graph_tableau *composite_graph_tableau;
  bool* bref;
};



//==============================================================================
// dbskfg_composite_graph_tableau
//==============================================================================

//------------------------------------------------------------------------------
//: Constructor - don't use this, use dbskfg_composite_graph_tableau_new.
// The child tableau is added later using add_child. 
dbskfg_composite_graph_tableau::
dbskfg_composite_graph_tableau(const char* n): 
    bgui_vsol2D_tableau(n),
    composite_graph_(0),
    rag_graph_(0),
    transforms_(0),
    display_composite_contour_nodes_(true),
    display_composite_shock_nodes_(true),
    display_composite_contour_links_(true),
    display_composite_shock_rays_(true),
    display_composite_shock_links_(true),
    display_rag_graph_(true),
    display_transforms_(true),
    composite_node_contour_curve_style_(
        vgui_style::new_style(1.0f, 0.0f, 0.0f, 6.0f,1.0f)),
    composite_node_shock_curve_style_(
        vgui_style::new_style(0.0f, 1.0f, 0.0f, 6.0f,1.0f)),
    composite_link_shock_ray_curve_style_(
        vgui_style::new_style(0.0f, 0.0f, 1.0f, 3.0f,3.0f)),
    composite_link_shock_edge_curve_style_(
        vgui_style::new_style(0.0f, 1.0f, 0.0f, 3.0f,3.0f)),
    composite_link_contour_curve_style_(
        vgui_style::new_style(1.0f, 0.0f, 0.0f, 3.0f,3.0f)),
    rag_node_curve_style_(
        vgui_style::new_style(0.6275f, 0.1255f, 0.9412f, 3.0f,4.0f)),
    transform_curve_style_(
        vgui_style::new_style(0, 1, 1, 3.0f,4.0f))
    
{

}


//: Add given menu to the tableau popup menu
void dbskfg_composite_graph_tableau::
add_popup(vgui_menu& menu)
{
  vgui_menu submenu;
  vcl_string on = "[x] ", off = "[ ] ";


  submenu.add( ((this->display_composite_contour_nodes_)?on:off) 
               + "Display Contour Nodes", 
    new dbskfg_composite_graph_tableau_toggle_command(
        this, &(this->display_composite_contour_nodes_)) );

  submenu.add( ((this->display_composite_shock_nodes_)?on:off) 
               + "Display Shock Nodes", 
    new dbskfg_composite_graph_tableau_toggle_command(
        this, &(this->display_composite_shock_nodes_)) );

  submenu.add( ((this->display_composite_shock_rays_)?on:off) 
               + "Display Shock Rays", 
    new dbskfg_composite_graph_tableau_toggle_command(
        this, &(this->display_composite_shock_rays_)) );

  submenu.add( ((this->display_composite_shock_links_)?on:off) 
               + "Display Shock Links", 
    new dbskfg_composite_graph_tableau_toggle_command(
        this, &(this->display_composite_shock_links_)) );

  submenu.add( ((this->display_composite_contour_links_)?on:off) 
               + "Display Contour Links", 
    new dbskfg_composite_graph_tableau_toggle_command(
        this, &(this->display_composite_contour_links_)) );

  submenu.add( ((this->display_rag_graph_)?on:off) 
               + "Display Rag Graph", 
    new dbskfg_composite_graph_tableau_toggle_command(
        this, &(this->display_rag_graph_)) );

  submenu.add( ((this->display_transforms_)?on:off) 
               + "Display Transforms", 
    new dbskfg_composite_graph_tableau_toggle_command(
        this, &(this->display_transforms_)) );

  //add this submenu to the popup menu
  menu.add("Composite Graph Tableau Options", submenu);

}


//-----------------------------------------------------------------------------
//: Handle all events sent to this tableau.
bool dbskfg_composite_graph_tableau::
handle(const vgui_event &e)
{
  // set the display (hide/show) flags of the soviews
  for (vcl_map< vcl_string , vgui_displaybase_tableau_grouping >::iterator it 
    = groupings.begin(); it != groupings.end(); it++ )
  {
    vcl_string group_name = it->first;
    
    if (group_name == "node_contour")
    {
        it->second.hide=!display_composite_contour_nodes_;
    }
    else if( group_name == "node_shock" )
    {
        it->second.hide=!display_composite_shock_nodes_;
    }
    else if( group_name == "link_shock_ray" )
    {
        it->second.hide=!display_composite_shock_rays_;
    }
    else if( group_name == "link_shock_link" )
    {
        it->second.hide=!display_composite_shock_links_;
    }
 
    else if ( group_name == "link_contour_link" )
    {
        it->second.hide=!display_composite_contour_links_;
    }
    else if ( group_name == "fragment" )
    {
        it->second.hide=!display_rag_graph_;
    }
    else if ( group_name == "transform" )
    {
        it->second.hide=!display_transforms_;
    }
  
  
  }


  return bgui_vsol2D_tableau::handle(e);
}



// -----------------------------------------------------------------------------
//: Display for composite graph contour/shock node
dbskfg_soview_composite_node* dbskfg_composite_graph_tableau::
add_composite_node(const dbskfg_composite_node_sptr& xv, 
                   const vgui_style_sptr& style,
                   vcl_string grouping)
{
    dbskfg_soview_composite_node* obj = new dbskfg_soview_composite_node(xv);
    obj->set_selectable(true);
    this->set_current_grouping(grouping);
    this->add(obj);
    obj->set_style(style);
    return obj;

}

// -----------------------------------------------------------------------------
//: Display for composite contour links
dbskfg_soview_contour_link* dbskfg_composite_graph_tableau::
add_contour_link(dbskfg_contour_link* xv, 
                 const vgui_style_sptr& style,
                 vcl_string grouping)
{

    dbskfg_soview_contour_link* obj = new dbskfg_soview_contour_link(xv);
    obj->set_selectable(true);
    this->set_current_grouping(grouping);
    this->add(obj);
    obj->set_style(style);
    return obj;

}


// -----------------------------------------------------------------------------
//: Display for composite shock links
dbskfg_soview_shock_link* dbskfg_composite_graph_tableau::
add_shock_link(dbskfg_shock_link* xv, 
               const vgui_style_sptr& style,
               vcl_string grouping)
{

    dbskfg_soview_shock_link* obj = new dbskfg_soview_shock_link(xv);
    obj->set_selectable(true);
    this->set_current_grouping(grouping);
    this->add(obj);
    obj->set_style(style);
    return obj;

}


// -----------------------------------------------------------------------------
//: Display for shock rays
dbskfg_soview_shock_ray* dbskfg_composite_graph_tableau::
add_shock_ray(dbskfg_shock_link* xv, 
              const vgui_style_sptr& style,
              vcl_string grouping)
{

    dbskfg_soview_shock_ray* obj = new dbskfg_soview_shock_ray(xv);
    obj->set_selectable(true);
    this->set_current_grouping(grouping);
    this->add(obj);
    obj->set_style(style);
    return obj;

}

// -----------------------------------------------------------------------------
//: Display for rag node
dbskfg_soview_rag_node* dbskfg_composite_graph_tableau::
add_rag_node(const dbskfg_rag_node_sptr& xv, 
             const vgui_style_sptr& style,
             vcl_string grouping)
{

    dbskfg_soview_rag_node* obj = new dbskfg_soview_rag_node(xv);
    obj->set_selectable(true);
    this->set_current_grouping(grouping);
    this->add(obj);
    obj->set_style(style);
    float r =  (rand() % 256)/256.0;
    float g =  (rand() % 256)/256.0;
    float b =  (rand() % 256)/256.0;
    obj->set_colour(r,g,b);
    return obj;

}

// -----------------------------------------------------------------------------
//: Display for transform
dbskfg_soview_transform* dbskfg_composite_graph_tableau::
add_transform(const dbskfg_transform_descriptor_sptr& xv, 
              const vgui_style_sptr style,
              vcl_string grouping)
{

    dbskfg_soview_transform* obj = new dbskfg_soview_transform(xv);
    obj->set_selectable(true);
    this->set_current_grouping(grouping);
    this->add(obj);
    obj->set_style(style);
    float r =  (rand() % 256)/256.0;
    float g =  (rand() % 256)/256.0;
    float b =  (rand() % 256)/256.0;
    obj->set_colour(r,g,b);
    return obj;

}

//------------------------------------------------------------------------------
//: Add elements of composite_graph to display list
bool dbskfg_composite_graph_tableau::
add_composite_graph_elms_to_display()
{
    if (!composite_graph_)
    {
        return false;
    }


    for (dbskfg_composite_graph::edge_iterator eit =
             composite_graph_->edges_begin();
         eit != composite_graph_->edges_end(); ++eit)
    {
        dbskfg_composite_link_sptr link = *eit;
        if ( link->link_type() == dbskfg_composite_link::SHOCK_LINK )
        {
            dbskfg_shock_link* shock_link=
                dynamic_cast<dbskfg_shock_link*>(&(*link));

            if ( shock_link->shock_link_type() == dbskfg_shock_link::SHOCK_RAY )
            {
                add_shock_ray(shock_link,
                              composite_link_shock_ray_curve_style_,
                              "link_shock_ray");

            }
            else
            {
                if ( shock_link->ex_pts().size() > 0 )
                {

                    add_shock_link(shock_link,
                                   composite_link_shock_edge_curve_style_,
                                   "link_shock_link");
                }
            }
        }
        else
        {
            dbskfg_contour_link* contour_link=
                dynamic_cast<dbskfg_contour_link*>(&(*link));

            add_contour_link(contour_link,
                             composite_link_contour_curve_style_,
                             "link_contour_link");
    
        }
    } 

    for (dbskfg_composite_graph::vertex_iterator vit = 
             composite_graph_->vertices_begin(); 
         vit != composite_graph_->vertices_end(); ++vit)
    {
        dbskfg_composite_node_sptr node = *vit;
        if ( node->node_type() == dbskfg_composite_node::CONTOUR_NODE )
        {
            add_composite_node(node,
                               composite_node_contour_curve_style_,
                               "node_contour");
        }
        else
        {
            add_composite_node(node,
                               composite_node_shock_curve_style_,
                               "node_shock");

        }
    }

    if (rag_graph_)
    {
 

        for (dbskfg_rag_graph::vertex_iterator vit = 
                 rag_graph_->vertices_begin(); 
             vit != rag_graph_->vertices_end(); ++vit)
        {

                     
            dbskfg_rag_node_sptr node = *vit;
            add_rag_node(node,
                         rag_node_curve_style_,
                         "fragment");


        }
 


    }
 
    return true;
}

bool dbskfg_composite_graph_tableau::add_transforms_to_display()
{

    if (transforms_)
    {
        vcl_vector<dbskfg_transform_descriptor_sptr> results=
            transforms_->objects();
   
        for ( unsigned int i(0) ; i < results.size() ; ++i)
        {
            if ( !results[i]->processed_ )
            {
            
                dbskfg_soview_transform* obj=add_transform
                    (results[i],
                     transform_curve_style_,
                     "transform");
 

            }
        }
   
    }


    return true;
}






