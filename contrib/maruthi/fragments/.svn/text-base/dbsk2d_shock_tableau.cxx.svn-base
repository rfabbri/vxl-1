// This is brcv/shp/dbsk2d/vis/dbsk2d_shock_tableau.cxx

//:
// \file

#include <float.h>
#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <dbsk2d/dbsk2d_ishock_edge.h>
#include <dbsk2d/dbsk2d_shock_grouped_ishock_edge.h>
#include <dbsk2d/dbsk2d_shock_grouped_ishock_edge_sptr.h>
#include <vgui/vgui.h>
#include <vgui/vgui_command.h>
#include <vgui/vgui_menu.h>
#include <vgui/vgui_utils.h>
#include <vgui/vgui_projection_inspector.h>
#include <vgl/vgl_distance.h>

#include "dbsk2d_shock_tableau.h"

#include "dbsk2d_bpoint_view.h"
#include "dbsk2d_bline_view.h"

#include <dbsk2d/dbsk2d_ishock_graph.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbsk2d/dbsk2d_xshock_node.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>
#include <dbsk2d/dbsk2d_xshock_sample.h>
#include <dbsk2d/dbsk2d_shock_fragment_sptr.h>
#include <dbsk2d/dbsk2d_shock_fragment.h>
#include <dbsk2d/dbsk2d_rich_map.h>
#include <vul/vul_sprintf.h>
#include <dbsk2d/dbsk2d_shock_ishock_node.h>

//#include <dbsk2d/dbsk2d_ishock_bucket_manager.h>

class dbsk2d_shock_tableau_toggle_command : public vgui_command
{
 public:
  dbsk2d_shock_tableau_toggle_command(dbsk2d_shock_tableau* tab, const void* boolref) : 
       shock_tableau(tab), bref((bool*) boolref) {}

  void execute() 
  { 
    *bref = !(*bref);
    shock_tableau->post_redraw(); 
  }

  dbsk2d_shock_tableau *shock_tableau;
  bool* bref;
};

//: Constructor
dbsk2d_shock_tableau::dbsk2d_shock_tableau():
  current(0), boundary_(0), ishock_graph_(0), shock_graph_(0), rich_map_(0)
{
  gl_mode = GL_RENDER;
  current = NULL;

  display_boundary_ = true;
  display_boundary_cells_ = false;
  display_rich_map_grid_ = false;
  display_boundary_burnt_regions_ = false;
  display_contact_shocks_ = true;
  display_shock_nodes_ = true;
  display_intrinsic_shocks_ = true;
  display_coarse_shocks_ = true;
  display_shock_fragments_ = false;
  display_xshock_samples_ = false;
  display_xshock_node_samples_ = true;
  display_xshock_edge_samples_ = true;

  //fill in the randomized color table
  for (int i=0; i<100; i++){
    for (int j=0; j<3;j++)
      rnd_colormap[i][j] = (rand() % 256)/256.0;
  }
}

bool dbsk2d_shock_tableau::handle( const vgui_event & e )
{
  if( e.type == vgui_DRAW ) {
    if (gl_mode == GL_SELECT)
    {
      draw_rich_map_select();
      if (display_boundary_)
        draw_boundary_select();
      if (display_intrinsic_shocks_)
        draw_ishock_select();
      if (display_coarse_shocks_)
        draw_shock_select();
    }
    else 
    {
      draw_rich_map_render();
      if (display_shock_fragments_)
        draw_shock_fragments_render();
      if (display_intrinsic_shocks_)
        draw_ishock_render();
      if (display_coarse_shocks_)
        draw_shock_render();
      if (display_boundary_)
        draw_boundary_render();
      
    }
    return true;
  }

  return false;
}

dbsk2d_shock_tableau::~dbsk2d_shock_tableau()
{
}

dbsk2d_base_gui_geometry* dbsk2d_shock_tableau::select( int ax , int ay )
{
  // select objects within 20 pixels of the mouse
  GLuint *ptr = vgui_utils::enter_pick_mode(ax,ay,20);

  this->gl_mode = GL_SELECT;
  this->handle(vgui_event(vgui_DRAW));
  this->gl_mode = GL_RENDER;

  int num_hits = vgui_utils::leave_pick_mode();

  // get all hits
  vcl_vector<vcl_vector<unsigned> > hits;
  vgui_utils::process_hits(num_hits, ptr, hits);

  // for each hit get the name of the soview if it is
  // being managed by this vcl_list
  //
  // Each hit from a display list has two entries. The first is the id
  // of the display list, and the second is the id of the soview. See
  // vgui_displaybase_tableau::draw_soviews_select(). Thus, an object
  // belongs to this display list iff the first hit number is this
  // list's id.

  vcl_vector<GLuint> my_hits;

  for (vcl_vector<vcl_vector<unsigned> >::iterator i=hits.begin();
    i != hits.end(); ++i)
  {
    vcl_vector<unsigned> const& names = *i;
    dbsk2d_assert( names.size() == 1 );
    my_hits.push_back( names[0] );
  }

  //vcl_cout << "my_hits.size() = " << my_hits.size() << vcl_endl; 

  if( my_hits.size() == 1 ) 
  {
    return (dbsk2d_base_gui_geometry* ) (long) my_hits[0];
  } 
  else if( my_hits.size() > 1 ) 
  {
    vgui_projection_inspector pi;
    float ix, iy, dx, dy;
    pi.window_to_image_coordinates(ax,ay, ix,iy);

    // to compute a threshold in distance at the current zoom that 
    // is equivalent to 2 pixels on the screen
    pi.window_to_image_coordinates(0, 2, dx,dy);

    // vcl_cout << "getting closest" << vcl_endl; 

    float smallest = FLT_MAX;
    unsigned int smallest_i = 0;

    for( unsigned int i = 0 ; i < my_hits.size() ; i++ ) {

      dbsk2d_base_gui_geometry * temp = (dbsk2d_base_gui_geometry *) (long) my_hits[ i ];
      int expts_size = temp->ex_pts().size();

      if( expts_size == 1 ) 
      {
        float tempx_distance = temp->ex_pts()[0].x() - ix;
        float tempy_distance = temp->ex_pts()[0].y() - iy;
        float dist_squared = tempx_distance * tempx_distance +
          tempy_distance * tempy_distance;
        if( (dist_squared - dy) < smallest ) { //favoring points (epsilon ball of 2 pixels)
          smallest = dist_squared;
          smallest_i = i;
        }
      } 
      else if( expts_size == 2 ) 
      {

        float dist_squared = 
          vgl_distance2_to_linesegment( temp->ex_pts()[0].x(), 
                                        temp->ex_pts()[0].y(), 
                                        temp->ex_pts()[1].x(),
                                        temp->ex_pts()[1].y(),
                                        double(ix), 
                                        double(iy));

        if( dist_squared < smallest ) {
          smallest = dist_squared;
          smallest_i = i;
        }
      } else if( expts_size > 2 ) {

        float *x = new float [ expts_size ];
        float *y = new float [ expts_size ];

        for( int a = 0 ; a < expts_size ; a++ ) {
          x[ a ] = temp->ex_pts()[a].x();
          y[ a ] = temp->ex_pts()[a].y();
        }

        float dist_squared = 
          vgl_distance_to_non_closed_polygon( x , y , expts_size , ix , iy );

        if( dist_squared < smallest ) {
          smallest = dist_squared;
          smallest_i = i;
        }

        delete []x;
        delete []y;

      }
      else {
        //vcl_cout << "Error" << vcl_endl;
      }

    }
    //vcl_cout<< "smallest_i: " << smallest_i << vcl_endl; 
    return (dbsk2d_base_gui_geometry *) (long) my_hits[ smallest_i ];
  }
  return NULL;
}

//: render the boundary class
void dbsk2d_shock_tableau::draw_boundary_select()
{
  if( boundary_.ptr() == 0 ) 
    return;

  glPushName( 0 ); // will be replaced by ids

  for (dbsk2d_boundary::belm_iterator it = boundary_->belm_begin();
       it != boundary_->belm_end(); ++it)
  {
    dbsk2d_ishock_belm* bit = *it;

    if (!bit->is_a_GUIelm()) //Amir: only draw the GUIelms
      continue;

    glLoadName((GLuint) (long)(bit));

    switch (bit->type())
    {
    case BPOINT:
      {
        dbsk2d_bpoint_view bp_view((dbsk2d_ishock_bpoint*)(bit));
        bp_view.draw(false, true);
      }
      break;
    case BLINE:
      {
        dbsk2d_bline_view bl_view((dbsk2d_ishock_bline*)(bit));
        bl_view.draw();
      }
      break;
    case BARC:
      {
        glColor3f( 0.0 , 0.0 , 1.0 );
        glLineWidth (1.0);
        glBegin( GL_LINE_STRIP );
        for( unsigned int i = 0 ; i < bit->ex_pts().size() ; i++ ) {
          glVertex2f( bit->ex_pts()[i].x() , bit->ex_pts()[i].y() );
        }
        glEnd();
      }
    }
  }

  glPopName();
}

void dbsk2d_shock_tableau::draw_boundary_render()
{
  if( boundary_.ptr() == 0 ) 
    return;

  if (display_boundary_cells_)
  {
    //draw the cells
    for (int i=0; i<boundary_->num_rows(); i++){
      for (int j=0; j<boundary_->num_cols(); j++){
        vgl_box_2d<double> box = boundary_->cell(i,j)->box();
        glColor3f( 0.2f , 0.2f , 0.2f );
        glBegin( GL_LINE_STRIP );
        glVertex2d(box.min_x(), box.min_y());
        glVertex2d(box.max_x(), box.min_y());      
        glVertex2d(box.max_x(), box.max_y());      
        glVertex2d(box.min_x(), box.max_y());
        glVertex2d(box.min_x(), box.min_y());
        glEnd();
      }
    }
  }

  for (dbsk2d_boundary::belm_iterator it = boundary_->belm_begin();
       it != boundary_->belm_end(); ++it)
  {
    dbsk2d_ishock_belm* bit = *it;

    if (!bit->is_a_GUIelm())  //Amir: only draw the GUIelms
      continue;

    switch (bit->type())
    {
    case BPOINT:
      {
        dbsk2d_bpoint_view bp_view((dbsk2d_ishock_bpoint*)bit);
        bp_view.draw(false, display_boundary_burnt_regions_);
      }
      break;
    case BLINE:
      {
        dbsk2d_bline_view bl_view((dbsk2d_ishock_bline*)bit);
        bl_view.draw(false, display_boundary_burnt_regions_);
      }
      break;
    case BARC:
      {
        glColor3f( 0.0 , 0.0 , 1.0 );
        glLineWidth (1.0);
        glBegin( GL_LINE_STRIP );
        for( unsigned int i = 0 ; i < bit->ex_pts().size() ; i++ ) {
          glVertex2f( bit->ex_pts()[i].x() , bit->ex_pts()[i].y() );
        }
        glEnd();
      }
    }
  }
}

void dbsk2d_shock_tableau::draw_ishock_select()
{
  if( ishock_graph_.ptr() == 0 ) 
    return;

  glPushName( 0 ); // will be replaced by ids

  //draw the edges first
  for ( dbsk2d_ishock_graph::edge_iterator curE = ishock_graph_->all_edges().begin();
        curE != ishock_graph_->all_edges().end();
        curE++ ) 
  {
    dbsk2d_ishock_edge* selm = (*curE);

    if (selm->is_a_contact() && !display_contact_shocks_)
      continue;
    
    glLoadName((GLuint)(long) selm);

    glLineWidth (1.0);
    glBegin( GL_LINE_STRIP );
    for( unsigned int i = 0 ; i < selm->ex_pts().size() ; i++ ) {
      glVertex2f( selm->ex_pts()[i].x() , selm->ex_pts()[i].y() );
    }
    glEnd();
  }

  //then draw the nodes
  if (display_shock_nodes_)
  {
    for ( dbsk2d_ishock_graph::vertex_iterator curN = ishock_graph_->all_nodes().begin();
          curN != ishock_graph_->all_nodes().end();
          curN++ ) 
    {
      dbsk2d_ishock_node* snode = (*curN);

      glLoadName((GLuint)(long) snode);

      glPointSize( 3.0 );
      glBegin( GL_POINTS );
      glVertex2f(snode->ex_pts()[0].x() , snode->ex_pts()[0].y() ); 
      glEnd();
    }
  }

  glPopName();
}

void dbsk2d_shock_tableau::draw_ishock_render()
{
  if( ishock_graph_.ptr() == 0 ) 
    return;

  //draw the edges first
  for ( dbsk2d_ishock_graph::edge_iterator curE = ishock_graph_->all_edges().begin();
        curE != ishock_graph_->all_edges().end();
        curE++ ) 
  {
    dbsk2d_ishock_edge* selm = (*curE);

    //No need to actually hide them
    //if (selm->isHidden())
    //  continue;

    if (selm->is_a_contact())
    {
      if (display_contact_shocks_)
        glColor3f( 0.5 , 0.5 , 0.5 );
      else
        continue;
    }
    else if (selm->isHidden()) 
      glColor3f( 0.5, 0.5, 0.5 );
    else
      glColor3f( 1.0 , 0.0 , 0.0 );

    if (!(selm->cSNode() || selm->cell_bnd()))
      glColor3f( 0.0, 1.0, 0.0 ); //bright green if there is an error in the shock topology

    glLineWidth (3.0);
    glBegin( GL_LINE_STRIP );
    for( unsigned int i = 0 ; i < selm->ex_pts().size() ; i++ ) {
      glVertex2f( selm->ex_pts()[i].x() , selm->ex_pts()[i].y() );
    }
    glEnd();
  }

  //then draw the nodes
  if (display_shock_nodes_)
  {
    for ( dbsk2d_ishock_graph::vertex_iterator curN = ishock_graph_->all_nodes().begin();
          curN != ishock_graph_->all_nodes().end();
          curN++ ) 
    {
      dbsk2d_ishock_node* snode = (*curN);

      //No need to actually hide them
      if (snode->isHidden())
        glColor3f( 0.5 , 0.5 , 1.0 );
      //  continue;
      else
        glColor3f( 0.5 , 0.5 , 1.0 );
      glPointSize( 5.0 );
      glBegin( GL_POINTS );
      glVertex2f(snode->ex_pts()[0].x() , snode->ex_pts()[0].y() ); 
      glEnd();
    }
  }

}

void dbsk2d_shock_tableau::draw_shock_select()
{
  if( shock_graph_.ptr() == 0 ) 
    return;
  
  glPushName( 0 ); // will be replaced by ids

  //draw the edges first
  for ( dbsk2d_shock_graph::edge_iterator curE = shock_graph_->edges_begin();
        curE != shock_graph_->edges_end();
        curE++ ) 
  {
    dbsk2d_shock_edge* selm = (*curE).ptr();
    
    glLoadName((GLuint)(long) selm);

    glLineWidth (2.0);
    glBegin( GL_LINE_STRIP );
    for( unsigned int i = 0 ; i < selm->ex_pts().size() ; i++ ) {
      glVertex2f( selm->ex_pts()[i].x() , selm->ex_pts()[i].y() );
    }
    glEnd();
  }

  //then draw the nodes
  for ( dbsk2d_shock_graph::vertex_iterator curN = shock_graph_->vertices_begin();
        curN != shock_graph_->vertices_end();
        curN++ ) 
  {
    dbsk2d_shock_node* snode = (*curN).ptr();
    glLoadName((GLuint)(long) snode);

    glPointSize( 5.0 );
    glBegin( GL_POINTS );
    glVertex2f(snode->ex_pts()[0].x() , snode->ex_pts()[0].y() ); 
    glEnd();
  } 
  glPopName();
}

void dbsk2d_shock_tableau::draw_shock_render()
{
  if( shock_graph_.ptr() == 0 ) 
    return;

  //draw the edges first
  for ( dbsk2d_shock_graph::edge_iterator curE = shock_graph_->edges_begin();
        curE != shock_graph_->edges_end();
        curE++ ) 
  {
    dbsk2d_shock_edge_sptr selm = (*curE);

    //draw the shock samples if this is an extrinsic shock graph
    dbsk2d_xshock_edge* xedge = dynamic_cast<dbsk2d_xshock_edge*>(selm.ptr());
    if (xedge && display_xshock_samples_ && display_xshock_edge_samples_)
    {
      for (int i=0; i<xedge->num_samples(); i++){
        dbsk2d_xshock_sample* xsample = xedge->sample(i).ptr();

        //draw the line representing this ray
        int col = selm->id() % 100;
        glColor3f(rnd_colormap[col][0], rnd_colormap[col][1], rnd_colormap[col][2]);
        glLineWidth (1.0);
        glBegin( GL_LINE_STRIP );
        vgl_point_2d<double> ex_pt1 = _translatePoint(xsample->pt, 
                                        xsample->theta+vcl_acos(-1/xsample->speed), 
                                        xsample->radius);
        vgl_point_2d<double> ex_pt2 = _translatePoint(xsample->pt, 
                                        xsample->theta-vcl_acos(-1/xsample->speed), 
                                        xsample->radius);

        glVertex2f(ex_pt1.x(), ex_pt1.y());
        glVertex2f(xsample->pt.x(), xsample->pt.y());
        glVertex2f(ex_pt2.x(), ex_pt2.y());

        //this is without proper reconstruction
        //glVertex2f(xsample->left_bnd_pt.x(), xsample->left_bnd_pt.y());
        //glVertex2f(xsample->pt.x(), xsample->pt.y());
        //glVertex2f(xsample->right_bnd_pt.x(), xsample->right_bnd_pt.y());
        glEnd();
      }
    }

    glColor3f( 0.0 , 1.0 , 0.0 );
    glLineWidth (5.0);
    glBegin( GL_LINE_STRIP );
    for( unsigned int i = 0 ; i < selm->ex_pts().size() ; i++ ) {
      glVertex2f( selm->ex_pts()[i].x() , selm->ex_pts()[i].y() );
    }
    glEnd();
  }

  //then draw the nodes
  
  for ( dbsk2d_shock_graph::vertex_iterator curN = shock_graph_->vertices_begin();
      curN != shock_graph_->vertices_end();
      curN++ ) 
  {
    dbsk2d_shock_node_sptr snode = (*curN);

    //draw the shock samples if this is an extrinsic shock graph
    dbsk2d_xshock_node* xnode = dynamic_cast<dbsk2d_xshock_node*>(snode.ptr());
    if (xnode && display_xshock_samples_ && display_xshock_node_samples_)
    {
      for (int i=0; i<xnode->num_samples(); i++){
        dbsk2d_xshock_sample* xsample = xnode->sample(i).ptr();

        //draw the line representing this ray
        int col = snode->id() % 100;
        glColor3f(rnd_colormap[col][0], rnd_colormap[col][1], rnd_colormap[col][2]);
        glLineWidth (1.0);
        glBegin( GL_LINE_STRIP );
        vgl_point_2d<double> ex_pt1 = _translatePoint(xsample->pt, 
                                          xsample->theta, 
                                          xsample->radius);

        glVertex2f(ex_pt1.x(), ex_pt1.y());
        glVertex2f(xsample->pt.x(), xsample->pt.y());
        
        //this is without proper reconstruction
        //glVertex2f(xsample->left_bnd_pt.x(), xsample->left_bnd_pt.y());
        //glVertex2f(xsample->pt.x(), xsample->pt.y());

        glEnd();
      }
    }

    if (display_shock_nodes_)
    {
      if (snode->id() == 1090409 || snode->id() == 1115109 || snode->id() == 1158209) {
        glColor3f( 0.0, 1.0, 0.0);
        glPointSize( 20.0 );
      } else {
        glColor3f( 1.0 , 1.0 , 1.0 );
        glPointSize( 5.0 );
      }
      glBegin( GL_POINTS );
      glVertex2f(snode->ex_pts()[0].x() , snode->ex_pts()[0].y() ); 
      glEnd();
    }
  }
}

vcl_pair<int,int>
dbsk2d_shock_tableau::getContourIdFromShockNode(dbsk2d_shock_node_sptr shockNode)
{

  // Grab pointer to group of shock edges
  dbsk2d_shock_ishock_node *linkedShocks = dynamic_cast<dbsk2d_shock_ishock_node*>(shockNode.ptr());

  // // Grab list of of parent shocks
  ishock_edge_list& parentShocks = linkedShocks->ishock_node()->pShocks();

  // // Grab first parent in list
  dbsk2d_ishock_edge *parentEdge = parentShocks.back();

  // dbsk2d_ishock_edge *childNonSourceEdge = linkedShocks->ishock_node()->cShock();
  // dbsk2d_ishock_edge *childSourceEdge = linkedShocks->ishock_node()->cShock2();
  // dbsk2d_ishock_edge *childEdge = ( childNonSourceEdge == 0 )? childSourceEdge: childNonSourceEdge;
  
  // if ( childNonSourceEdge==0 && childSourceEdge==0)
  //   {
  //     vcl_cout<<"Very Very bad"<<vcl_endl;

  //   } 
  // Initialize ids 
  int leftid(0);
  int rightid(0);

  if (parentEdge != 0 )
  {

      dbsk2d_ishock_belm *left_belem  = parentEdge->lBElement();
      dbsk2d_ishock_belm *right_belem = parentEdge->rBElement();

      // Check that boundary elements exist
      if ( left_belem!=0 && right_belem != 0 )
    {
      // --------------------  Process left boundary element

      // Check if it is a line
      if ( left_belem->is_a_line())
      {

    dbsk2d_ishock_bline *line = static_cast<dbsk2d_ishock_bline*>(left_belem);
    const vcl_list<vtol_topology_object*>* contour_list= line->bnd_edge()->superiors_list();
    leftid=contour_list->front()->get_id();

      }
      // If not a line assume it is a point
      else
      {
    // Grab point
    dbsk2d_ishock_bpoint *point = static_cast<dbsk2d_ishock_bpoint*>(left_belem);

    // Grab container class of point, vertex
    dbsk2d_bnd_vertex* vertex   = point->bnd_vertex();

    // Grab vertex superior
    const vcl_list<vtol_topology_object*>* zero_chain_sup_list = vertex->superiors_list();
    
    // Grab zero chain superior
    const vcl_list<vtol_topology_object*>* edge_chain_sup_list = zero_chain_sup_list->front()->superiors_list();

    // Grab edge superior list
    const vcl_list<vtol_topology_object*>* contour_sup_list    = edge_chain_sup_list->front()->superiors_list();

    // Grab id from edge superior, should be contour
    leftid = contour_sup_list->front()->get_id();

       
      }



      // --------------------  Process right boundary element

      // Check if it is a line
      if ( right_belem->is_a_line())
      {
    dbsk2d_ishock_bline *line = static_cast<dbsk2d_ishock_bline*>(right_belem);
    const vcl_list<vtol_topology_object*>* contour_list= line->bnd_edge()->superiors_list();
    rightid=contour_list->front()->get_id();
      }
      // If not a line assume it is a point
      else
      {
    // Grab point
    dbsk2d_ishock_bpoint *point = static_cast<dbsk2d_ishock_bpoint*>(right_belem);

    // Grab container class of point, vertex
    dbsk2d_bnd_vertex* vertex   = point->bnd_vertex();

    // Grab vertex superior
    const vcl_list<vtol_topology_object*>* zero_chain_sup_list = vertex->superiors_list();
    
    // Grab zero chain superior
    const vcl_list<vtol_topology_object*>* edge_chain_sup_list = zero_chain_sup_list->front()->superiors_list();

    // Grab edge superior list
    const vcl_list<vtol_topology_object*>* contour_sup_list    = edge_chain_sup_list->front()->superiors_list();

    // Grab id from edge superior, should be contour
    rightid = contour_sup_list->front()->get_id();
    
      }
    }
  }
  
  vcl_pair<int,int> key = vcl_make_pair(leftid,rightid);
  return key;


}
vcl_pair<int,int>
dbsk2d_shock_tableau::getContourIdFromShockEdge(dbsk2d_shock_edge_sptr shockEdge)
{

  // Grab pointer to group of shock edges
  dbsk2d_shock_grouped_ishock_edge *elly = dynamic_cast<dbsk2d_shock_grouped_ishock_edge*>(&(*shockEdge));
  
  // Grab a list of edges
  vcl_list<dbsk2d_ishock_edge*>& edgeList = elly->edges();

  // Initialize ids 
  int leftid(0);
  int rightid(0);

  // Iterator
  vcl_list<dbsk2d_ishock_edge*>::iterator i;  

  // Loop over edges and find their parents
  for(i=edgeList.begin(); i != edgeList.end(); ++i) 
  {

      dbsk2d_ishock_edge* elem = *i;

      // Check elem is not null
      if (elem==0)
    continue;

      dbsk2d_ishock_belm *left_belem  = elem->lBElement();
      dbsk2d_ishock_belm *right_belem = elem->rBElement();

      // Check that boundary elements exist
      if ( left_belem==0 || right_belem==0 )
    continue;

      // --------------------  Process left boundary element

      // Check if it is a line
      if ( left_belem->is_a_line())
      {

    dbsk2d_ishock_bline *line = static_cast<dbsk2d_ishock_bline*>(left_belem);
    const vcl_list<vtol_topology_object*>* contour_list= line->bnd_edge()->superiors_list();
    leftid=contour_list->front()->get_id();

      }
      // If not a line assume it is a point
      else
      {
    // Grab point
    dbsk2d_ishock_bpoint *point = static_cast<dbsk2d_ishock_bpoint*>(left_belem);

    // Grab container class of point, vertex
    dbsk2d_bnd_vertex* vertex   = point->bnd_vertex();

    // Grab vertex superior
    const vcl_list<vtol_topology_object*>* zero_chain_sup_list = vertex->superiors_list();
    
    // Grab zero chain superior
    const vcl_list<vtol_topology_object*>* edge_chain_sup_list = zero_chain_sup_list->front()->superiors_list();

    // Grab edge superior list
    const vcl_list<vtol_topology_object*>* contour_sup_list    = edge_chain_sup_list->front()->superiors_list();

    // Grab id from edge superior, should be contour
    leftid = contour_sup_list->front()->get_id();

       
      }



      // --------------------  Process right boundary element

      // Check if it is a line
      if ( right_belem->is_a_line())
      {
    dbsk2d_ishock_bline *line = static_cast<dbsk2d_ishock_bline*>(right_belem);
    const vcl_list<vtol_topology_object*>* contour_list= line->bnd_edge()->superiors_list();
    rightid=contour_list->front()->get_id();
      }
      // If not a line assume it is a point
      else
      {
    // Grab point
    dbsk2d_ishock_bpoint *point = static_cast<dbsk2d_ishock_bpoint*>(right_belem);

    // Grab container class of point, vertex
    dbsk2d_bnd_vertex* vertex   = point->bnd_vertex();

    // Grab vertex superior
    const vcl_list<vtol_topology_object*>* zero_chain_sup_list = vertex->superiors_list();
    
    // Grab zero chain superior
    const vcl_list<vtol_topology_object*>* edge_chain_sup_list = zero_chain_sup_list->front()->superiors_list();

    // Grab edge superior list
    const vcl_list<vtol_topology_object*>* contour_sup_list    = edge_chain_sup_list->front()->superiors_list();

    // Grab id from edge superior, should be contour
    rightid = contour_sup_list->front()->get_id();
    
      }

      // If we have gotten here then we know we have found a pair
      break;
    }

    // Create a vector of vcl_pairs
    vcl_pair<int,int> key = vcl_make_pair(leftid,rightid);

    return key;


}
void dbsk2d_shock_tableau::draw_shock_fragments_render()
{
  if( shock_graph_.ptr() == 0 ) 
    return;

  // Loop over all edge and determine contour pairs
  // Use contour pairs to index to color table
  vcl_map<vcl_pair<int, int>,vcl_vector<float> > visualFragColorMap;
  int index(0);

  for ( dbsk2d_shock_graph::edge_iterator curE = shock_graph_->edges_begin();
        curE != shock_graph_->edges_end();
        curE++ ) 
  {

    dbsk2d_shock_edge_sptr selm = (*curE);

    //return if no fragment has been computed 
    if (!selm->shock_fragment())
      continue;

    //Create a key to hold contour id
    vcl_pair<int,int> key1=getContourIdFromShockEdge(selm);
    vcl_pair<int,int> key2=vcl_make_pair(key1.second,key1.first);

    //create a vector to hold all three colors
    vcl_vector<float> colors;
    colors.push_back((rand() % 256)/256.0);
    colors.push_back((rand() % 256)/256.0);
    colors.push_back((rand() % 256)/256.0);
    
    visualFragColorMap[key1]=colors;
    visualFragColorMap[key2]=colors;
    
  }


  //--------------------------draw the edges fragments first
  for ( dbsk2d_shock_graph::edge_iterator curE = shock_graph_->edges_begin();
        curE != shock_graph_->edges_end();
        curE++ ) 
  {

    dbsk2d_shock_edge_sptr selm = (*curE);

    //return if no fragment has been computed 
    if (!selm->shock_fragment())
      continue;
  
    //Create a key to hold contour id
    vcl_pair<int,int> key=getContourIdFromShockEdge(selm);

    //create a vector to hold all three colors
    vcl_vector<float> colors=visualFragColorMap[key];

    //draw the fragment with a random color assigned wrt its id
    // int col = selm->id() % 100;
    glColor3f(colors[0], colors[1], colors[2]);

    glBegin( GL_POLYGON );
    //glLineWidth(1.0);
    //glBegin( GL_LINE_LOOP );
    for( unsigned int i = 0 ; i < selm->shock_fragment()->ex_pts().size() ; i++ ) {
      glVertex2f( selm->shock_fragment()->ex_pts()[i].x(), 
                  selm->shock_fragment()->ex_pts()[i].y() );
    }
    glEnd();

  }

  //then draw the node fragments
  
  for ( dbsk2d_shock_graph::vertex_iterator curN = shock_graph_->vertices_begin();
      curN != shock_graph_->vertices_end();
      curN++ ) 
  {
    dbsk2d_shock_node_sptr snode = (*curN);

    //traverse the descriptor list and draw the shock fragments for the 
    //degenerate descriptors
    vcl_list<dbsk2d_shock_node_descriptor>::iterator p_itr = snode->descriptor_list().begin();
    for (; p_itr != snode->descriptor_list().end(); ++ p_itr){
      dbsk2d_shock_node_descriptor cur_descriptor = (*p_itr);
 
      if (!cur_descriptor.fragment.ptr())
        continue;

      // //Create a key to hold contour id
      // vcl_pair<int,int> key=getContourIdFromShockNode(snode);

      // //create a vector to hold all three colors
      // vcl_vector<float> colors=visualFragColorMap[key];
     
      // //draw the fragment with a random color assigned wrt its id
      // // int col = selm->id() % 100;
      // if ( colors.size() != 0 )
      //     {
      //       glColor3f(colors[0], colors[1], colors[2]);
      //     }
      // else
      //     {
      //       glColor3f(0.0f,0.0f,0.0f);
      //     }
      //draw the fragment with a random color assigned wrt its id
      //int col = snode->id() % 100;
      //glColor3f(rnd_colormap[col][0], rnd_colormap[col][1], rnd_colormap[col][2]);
      glColor3f(0.0f,0.0f,0.0f);
      glBegin( GL_POLYGON );
      //glColor3f(0, 0, 0);
      //glLineWidth(1.0);
      //glBegin( GL_LINE_LOOP );
      for( unsigned int i = 0 ; i < cur_descriptor.fragment->ex_pts().size() ; i++ ) {
        glVertex2f( cur_descriptor.fragment->ex_pts()[i].x(), 
                    cur_descriptor.fragment->ex_pts()[i].y() );
      }
      glEnd();
    }
  }
}

//: render the rich map
void dbsk2d_shock_tableau::draw_rich_map_select()
{
}
void dbsk2d_shock_tableau::draw_rich_map_render()
{
  if (display_rich_map_grid_)
  {
    //draw the grid lines
    //horizontal lines
    for (unsigned int y=rich_map_->y_offset(); 
         y <= rich_map_->y_offset()+rich_map_->height(); y++)
    {
      glColor3f(0.5f, 0.5f, 0.5f);
      glLineWidth (1.0);
      glBegin( GL_LINE_STRIP );
      glVertex2f(rich_map_->x_offset(), y);
      glVertex2f(rich_map_->x_offset()+rich_map_->width(), y);
      glEnd();
    }

    //then vertical lines
    for (unsigned int x=rich_map_->x_offset(); 
         x <= rich_map_->x_offset()+rich_map_->width(); x++)
    {
      glColor3f(0.5f, 0.5f, 0.5f);
      glLineWidth (1.0);
      glBegin( GL_LINE_STRIP );
      glVertex2f(x, rich_map_->y_offset());
      glVertex2f(x, rich_map_->y_offset()+rich_map_->height());
      glEnd();
    }
  }
}

void 
dbsk2d_shock_tableau::get_popup(const vgui_popup_params& params, vgui_menu &menu)
{
  vgui_menu submenu;
  vcl_string on = "[x] ", off = "[ ] ";

  submenu.add( ((display_boundary_)?on:off)+"Draw Boundary", 
               new dbsk2d_shock_tableau_toggle_command(this, &display_boundary_));

  submenu.add( ((display_boundary_cells_)?on:off)+"Draw Bnd Cells", 
               new dbsk2d_shock_tableau_toggle_command(this, &display_boundary_cells_));

  submenu.add( ((display_rich_map_grid_)?on:off)+"Draw Grid", 
               new dbsk2d_shock_tableau_toggle_command(this, &display_rich_map_grid_));

  submenu.add( ((display_contact_shocks_)?on:off)+"Draw Contact Shocks", 
               new dbsk2d_shock_tableau_toggle_command(this, &display_contact_shocks_));

  submenu.add( ((display_shock_nodes_)?on:off)+"Draw Shock Nodes",  
               new dbsk2d_shock_tableau_toggle_command(this, &display_shock_nodes_));

  submenu.add( ((display_intrinsic_shocks_)?on:off)+"Show Intrinsic Shock Graph",  
               new dbsk2d_shock_tableau_toggle_command(this, &display_intrinsic_shocks_));

  submenu.add( ((display_coarse_shocks_)?on:off)+"Draw Coarse Shock Graph",  
               new dbsk2d_shock_tableau_toggle_command(this, &display_coarse_shocks_));

  submenu.add( ((display_shock_fragments_)?on:off)+"Draw Shock Fragments", 
               new dbsk2d_shock_tableau_toggle_command(this, &display_shock_fragments_));

  submenu.add( ((display_boundary_burnt_regions_)?on:off)+"Draw Burnt Region", 
               new dbsk2d_shock_tableau_toggle_command(this, &display_boundary_burnt_regions_));

  submenu.add( ((display_xshock_samples_)?on:off)+"Draw Shock Samples", 
               new dbsk2d_shock_tableau_toggle_command(this, &display_xshock_samples_));

  submenu.add( ((display_xshock_node_samples_)?on:off)+"Draw Shock Node Samples", 
               new dbsk2d_shock_tableau_toggle_command(this, &display_xshock_node_samples_));

  submenu.add( ((display_xshock_edge_samples_)?on:off)+"Draw Shock Edge Samples", 
               new dbsk2d_shock_tableau_toggle_command(this, &display_xshock_edge_samples_));
  

  //add this submenu to the popup menu
  menu.add("Shock Tableau Options", submenu);
}
