// This is brcv/shp/dbsk2d/algo/dbsk2d_compute_bounding_box.cxx

//:
// \file

#include <dbsk2d/algo/dbsk2d_compute_bounding_box.h>
#include <vsol/vsol_box_2d.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_xshock_edge_sptr.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>
#include <dbsk2d/dbsk2d_xshock_node.h>

//: take a shock graph, check if its intrinsic and set its bounding box if so
//  return true if successfull
bool 
dbsk2d_compute_bounding_box(dbsk2d_shock_graph_sptr shock)
{
  dbsk2d_shock_graph::edge_iterator first_it = shock->edges_begin();
  dbsk2d_xshock_edge* first = dynamic_cast<dbsk2d_xshock_edge*>((*first_it).ptr());
  if (!first)
    return false;

  vsol_box_2d_sptr box = new vsol_box_2d;

  //then iterate over the nodes
  //draw the edges first
  for ( dbsk2d_shock_graph::edge_iterator curE = shock->edges_begin();
        curE != shock->edges_end();
        curE++ ) 
  {
    dbsk2d_shock_edge* selm = (*curE).ptr();
    for( unsigned int i = 0 ; i < selm->ex_pts().size() ; i++ ) {
      box->add_point( selm->ex_pts()[i].x() , selm->ex_pts()[i].y() );
    }
  }

  shock->set_bounding_box(box);
  return true;
}

double
dbsk2d_compute_total_width (dbsk2d_shock_graph_sptr shock)
{

  double total_width = 0;
  for ( dbsk2d_shock_graph::edge_iterator curE = shock->edges_begin();
        curE != shock->edges_end();
        curE++ ) 
  {
    dbsk2d_shock_edge_sptr selm = (*curE);

    //draw the shock samples if this is an extrinsic shock graph
    dbsk2d_xshock_edge* xedge = dynamic_cast<dbsk2d_xshock_edge*>(selm.ptr());
    if (xedge)
    {
      for (int i=0; i<xedge->num_samples(); i++){
        dbsk2d_xshock_sample* xsample = xedge->sample(i).ptr();
        total_width += xsample->radius*2;
      }
    }
  }
  
  for ( dbsk2d_shock_graph::vertex_iterator curN = shock->vertices_begin();
      curN != shock->vertices_end();
      curN++ ) 
  {
    dbsk2d_shock_node_sptr snode = (*curN);

    //draw the shock samples if this is an extrinsic shock graph
    dbsk2d_xshock_node* xnode = dynamic_cast<dbsk2d_xshock_node*>(snode.ptr());
    if (xnode)
    {
      for (int i=0; i<xnode->num_samples(); i++){
        dbsk2d_xshock_sample* xsample = xnode->sample(i).ptr();
        total_width += xsample->radius*2;
      }
    }
  }

  return total_width;
}
