// This is brcv/shp/dbsk2d/dbsk2d_rich_map.cxx

//:
// \file

#include <dbsk2d/dbsk2d_rich_map.h>
#include <vcl_limits.h>
#include <vgl/vgl_polygon.h>
#include <vgl/vgl_polygon_scan_iterator.h>

//: Constructor
dbsk2d_rich_map::dbsk2d_rich_map(const dbsk2d_shock_graph_sptr shock_graph,
                                 unsigned int width, unsigned int height,
                                 int x_offset, int y_offset) :
  x_offset_(x_offset), y_offset_(y_offset),
  width_(width), height_(height),
  distance_map_(height, width, vcl_numeric_limits<float >::max()),
  curve_map_(height, width, curve_map_element(NULL,0)),
  shock_map_(height, width, shock_map_element())
{
  //assuming that the shock graph and boundary exists at construction time
  fill_the_map(shock_graph);
}

//: Constructor: the best grid size is determined from the shock graph
dbsk2d_rich_map::dbsk2d_rich_map(const dbsk2d_shock_graph_sptr shock_graph)
{
  //determine the bounding box of the boundary class and create a grid that
  //can contain it

  //can we do this entirely in the initialization list ??
}

//: Destructor
dbsk2d_rich_map::~dbsk2d_rich_map() 
{
  
}

//: Return the approximate distance to a curve at (x,y)
float dbsk2d_rich_map::distance(int x, int y) const
{
  if( x < x_offset_ || x >= int(width_+x_offset_) ||
      y < y_offset_ || y >= int(height_+y_offset_) )
    return vcl_numeric_limits<float>::infinity();

  return distance_map_[y-y_offset_][x-x_offset_];
}

//: Return a pointer to the closest curve at (x,y)
dbsk2d_bnd_contour_sptr 
dbsk2d_rich_map::closest_curve(int x, int y) const
{
  if( x < x_offset_ || x >= int(width_+x_offset_) ||
      y < y_offset_ || y >= int(height_+y_offset_) )
    return NULL;

  return curve_map_[y-y_offset_][x-x_offset_].contour;
}

//: Return a pointer to the shock_fragment at (x,y)
dbsk2d_shock_fragment_sptr 
dbsk2d_rich_map::shock_fragment(int x, int y) const
{
  if( x < x_offset_ || x >= int(width_+x_offset_) ||
      y < y_offset_ || y >= int(height_+y_offset_) )
    return NULL;

  return shock_map_[y-y_offset_][x-x_offset_].fragment;
}

//: traverse the shock graph to fill the map
void dbsk2d_rich_map::fill_the_map(const dbsk2d_shock_graph_sptr shock_graph)
{

  //go through the edges fragments first
  for ( dbsk2d_shock_graph::edge_iterator curE = shock_graph->edges_begin();
        curE != shock_graph->edges_end();
        curE++ ) 
  {
    dbsk2d_shock_edge_sptr sedge = (*curE);

    if (sedge->shock_fragment())
      fill_a_shock_fragment(sedge->shock_fragment());
  }

  //then draw the node fragments
  
  for ( dbsk2d_shock_graph::vertex_iterator curN = shock_graph->vertices_begin();
      curN != shock_graph->vertices_end();
      curN++ ) 
  {
    dbsk2d_shock_node_sptr snode = (*curN);

    //traverse the descriptor list and draw the shock fragments for the 
    //degenerate descriptors
    vcl_list<dbsk2d_shock_node_descriptor>::iterator p_itr = snode->descriptor_list().begin();
    for (; p_itr != snode->descriptor_list().end(); ++ p_itr){
      dbsk2d_shock_node_descriptor cur_descriptor = (*p_itr);
 
      if (cur_descriptor.fragment)
        fill_a_shock_fragment(cur_descriptor.fragment);
    }
  }

}

void dbsk2d_rich_map::fill_a_shock_fragment(dbsk2d_shock_fragment_sptr fragment)
{
  //1) Create a vgl_polygon from the bounding points of the fragment
  vgl_polygon<double> poly;
  poly.new_sheet();
  for( unsigned i = 0 ; i < fragment->ex_pts().size() ; i++ ) {
    poly.push_back(fragment->ex_pts()[i]);
  }

  //2) Use a polygon scan-iterator to pick out the image points that belong to it
  vgl_polygon_scan_iterator<double> psi(poly, false);
  for (psi.reset(); psi.next(); ) {
    int y = psi.scany();
    for (int x = psi.startx(); x <= psi.endx(); ++x)
    {
      //current internal grid point is (x,y)
      //make sure this is valid
      if( x < x_offset_ || x >= int(width_+x_offset_) ||
          y < y_offset_ || y >= int(height_+y_offset_) )
        continue;

      ////from this point find the closest point on the contour(s)
      //double s1=0, s2=0;
      //double d1 = vcl_numeric_limits<float >::max();
      //double d2 = vcl_numeric_limits<float >::max();

      //if (fragment->contour1){
      //  s1 = fragment->contour1->closest_point(x,y);
      //  d1 = vgl_distance(fragment->contour1->point_at(s1), vgl_point_2d<double>(x,y));
      //}
      //if (fragment->contour2){
      //  s2 = fragment->contour2->closest_point(x,y);
      //  d2 = vgl_distance(fragment->contour2->point_at(s2), vgl_point_2d<double>(x,y));
      //}

      //if (d1<d2) {
      //  //assign this distance to the distance map
      //  distance_map_[y-y_offset_][x-x_offset_] = d1;

      //  //assign this curve to the curve_map
      //  curve_map_[y-y_offset_][x-x_offset_] = curve_map_element(fragment->contour1, s1);
      //}
      //else {
      //  //assign this distance to the distance map
      //  distance_map_[y-y_offset_][x-x_offset_] = d2;

      //  //assign this curve to the curve_map
      //  curve_map_[y-y_offset_][x-x_offset_] = curve_map_element(fragment->contour2, s2);
      //}

      //Now find the intrinsic shock coordinates of this point

      //this will involve:
      //1) finding the bnd_edge at s
      //2) finding the eta the bnd element
      //3) finding the shock at eta
      //4) finding the tau on the shock corresponding to that eta
      //5) finding psi corresponding to that tau on the gropued shock edge
      //6) finding t = radius - distance to contour

      double psi=0;
      double t = 0;

      //assign these intrinsic coordinate to 
      shock_map_[y-y_offset_][x-x_offset_] = shock_map_element(fragment, psi, t); 
      
    }
  }

}
