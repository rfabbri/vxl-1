// This is brcv/shp/dbsk2d/algo/dbsk2d_bnd_preprocess_common.cxx

//:
// \file



#include "dbsk2d_bnd_preprocess.h"

#include <vcl_algorithm.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_distance.h>

#include <vtol/vtol_list_functions.h>

#include <dbgl/algo/dbgl_circ_arc.h>
#include <dbgl/algo/dbgl_closest_point.h>
#include <dbsk2d/dbsk2d_bnd_utils.h>
#include <dbsk2d/dbsk2d_ishock_barc.h>

// ================= COMMON ======================


//-----------------------------------------------------------------------------
//: Remove "unlinked" edges in an edge list
void dbsk2d_bnd_preprocess::
remove_unlinked_edges(vcl_list<dbsk2d_bnd_edge_sptr >& edges)
{
  bnd_edge_list::iterator eit = edges.begin();
  while (eit != edges.end())
  {
    dbsk2d_bnd_edge_sptr e = *eit;
    if (e->numinf()==0 && e->numsup()==0)
      eit = edges.erase(eit);
    else
      ++eit;
  }
}



//-----------------------------------------------------------------------------
//: Separate out the edges into three groups - points, lines, and arcs
void dbsk2d_bnd_preprocess::
classify_edges(vcl_list<dbsk2d_bnd_edge_sptr >& edges,
  vcl_list<dbsk2d_bnd_edge_sptr >* bnd_pts, 
  vcl_list<dbsk2d_bnd_edge_sptr >* bnd_lines, 
  vcl_list<dbsk2d_bnd_edge_sptr >* bnd_arcs)
{
  // clean up
  if (bnd_pts) bnd_pts->clear();
  if (bnd_lines) bnd_lines->clear();
  if (bnd_arcs) bnd_arcs->clear();

  for (bnd_edge_list::iterator eit = edges.begin(); 
    eit != edges.end(); ++eit)
  {
    dbsk2d_bnd_edge_sptr e = *eit;
    if (e->is_a_point())
    {
      if (bnd_pts) bnd_pts->push_back(e);
    }
    else if (e->left_bcurve()->is_a_line())
    {
      if (bnd_lines) bnd_lines->push_back(e);
    }
    else if ( e->left_bcurve()->is_an_arc())
    {
      if (bnd_arcs) bnd_arcs->push_back(e);
    }

    // if reached here, something is wrong. Some unknown edge type is present
    else
      dbsk2d_assert(false);
  }
  edges.clear();
  return;
}





//------------------------------------------------------------------------
//: Convert too short lines and arcs into points
// If the points is not stand-alone, the remove it
void dbsk2d_bnd_preprocess::
remove_short_curves(vcl_list<dbsk2d_bnd_edge_sptr >& edges)
{
  bnd_edge_list::iterator eit = edges.begin();
  while (eit != edges.end())
  {
    dbsk2d_bnd_edge_sptr e = (*eit);

    if (!e->is_a_point())
    {    
      // do nothing if distance between endpoints are above threshold
      double d = (e->point2()-e->point1()).length();
      if (d > dbsk2d_bnd_preprocess::distance_tol)
      {
        ++eit;
        continue;
      }

      // when distance is below threshold, merge the two end points
      // and delete the internal bcurves of edge
      e->bnd_v1()->merge_with(e->bnd_v2());
    }
    e->clear_bcurves();

    // check if this is really a stand-alone point
    edge_list elist;
    e->v1()->edges(elist);
    if (elist.size() > 1)
    {
      e->unlink_from_cells();
      e->unlink();
      eit = edges.erase(eit);
    }
    else
    {
      ++eit;
    }
  }
  return;
}



//----------------------------------------------------------------------------
//: Merge vertices that are geometrically close
void dbsk2d_bnd_preprocess::
merge_close_vertices(vcl_list<dbsk2d_bnd_vertex_sptr >* affected_vertices,
                     vcl_list<dbsk2d_bnd_vertex_sptr >* vertex_set1,
                     vcl_list<dbsk2d_bnd_vertex_sptr >* vertex_set2)
{
  // clear old stuffs
  affected_vertices->clear();

  bnd_vertex_list::iterator vit1;
  bnd_vertex_list::iterator vit2;

  dbsk2d_bnd_vertex_sptr v1 = 0;
  dbsk2d_bnd_vertex_sptr v2 = 0;

  bnd_vertex_list* vertex_set2b = (vertex_set2) ? vertex_set2 : vertex_set1;

  // loop through all pair of vertices and check distance between them
  for (vit1 = vertex_set1->begin(); vit1 != vertex_set1->end(); ++vit1)
  {
    v1 = (*vit1);
    if (vertex_set2)
    {
      vit2 = vertex_set2->begin();
    }
    // if vit1 and vit2 of the same list, start from the next vertex
    else
    {
      vit2 = vit1;
      ++vit2;
    }
    
    while (vit2 != vertex_set2b->end())
    {
      v2 = (*vit2);
      if (v2 == v1)
      {
        vit2 = vertex_set2b->erase(vit2);
        continue;
      }

      // if the two vertices have the same geometry, transfer all the links
      // from one vertice to the other and delete one of them
      // this should not delete any bnd_edge, hence the structure of the contours
      // is preserved. No need to modify the contour

      double d = (v2->point()-v1->point()).length();
      if (d > dbsk2d_bnd_preprocess::distance_tol)
      {
        ++vit2;
      }
      else
      {
        v1->merge_with(v2);
        vit2 = vertex_set2b->erase(vit2);
        affected_vertices->push_back(v1);
      }
    }
  }
}



//-----------------------------------------------------------------------------
//: Insert new vertices to the middle of the edges
// The overall edge list will also be updated
void dbsk2d_bnd_preprocess::
insert_new_vertices(const vkey_vertex_map & new_vertex_map,
                    vcl_list<dbsk2d_bnd_edge_sptr >& all_edges,
                    vcl_list<dbsk2d_bnd_vertex_sptr >& affected_vertices)
{

  // clear old stuffs
  affected_vertices.clear();

  vkey_vertex_map::const_iterator vit = new_vertex_map.begin();
  while (vit != new_vertex_map.end())
  {
    dbsk2d_bnd_edge_sptr cur_edge = (*vit).first.first;
    
    // list of vertices along `cur_edge'
    bnd_vertex_list vertices_along_edge;
    vertices_along_edge.push_back(cur_edge->bnd_v1());

    vkey_vertex_map::const_iterator vit2 = vit;
    for (; vit2 != new_vertex_map.end() && (*vit2).first.first == cur_edge;
        ++vit2)  
    {
      vertices_along_edge.push_back((*vit2).second);
    }
    vertices_along_edge.push_back(cur_edge->bnd_v2());

    // update vit
    vit = vit2;


    // vector of edges (lines or arc) that will be used to replace `cur_edge'
    vcl_vector<dbsk2d_bnd_edge_sptr > new_edges;
    vcl_vector<signed char > directions;
  
    // Treat each of of belm curves (line, arc) differently  
    switch (cur_edge->left_bcurve()->type())
    {
    case(BLINE):
      {
        // form a vector of line edges equivalent to the old edge
        bnd_vertex_list::iterator vit3 = vertices_along_edge.begin();
        dbsk2d_bnd_vertex_sptr last_vertex = *vit3;
        ++vit3;
        
        for (; vit3 != vertices_along_edge.end(); ++vit3)
        {
          dbsk2d_bnd_vertex_sptr new_vertex = (*vit3);
          new_edges.push_back(dbsk2d_bnd_utils::new_line_between(last_vertex, 
            new_vertex, this->boundary()));
          directions.push_back(1);
          last_vertex = new_vertex;
        }
        break;
      }
    case(BARC):
      {
        dbsk2d_ishock_barc* barc = 
          static_cast<dbsk2d_ishock_barc*>(cur_edge->left_bcurve());
        double arc_k = barc->curvature();

        // form a vector of line edges equivalent to the old edge
        bnd_vertex_list::iterator vit3 = vertices_along_edge.begin();
        dbsk2d_bnd_vertex_sptr last_vertex = *vit3;
        ++vit3;
        
        for (; vit3 != vertices_along_edge.end(); ++vit3)
        {
          dbsk2d_bnd_vertex_sptr new_vertex = (*vit3);
          new_edges.push_back(dbsk2d_bnd_utils::new_arc_between(last_vertex, 
            new_vertex, arc_k, this->boundary()));
          directions.push_back(1);
          last_vertex = new_vertex;
        }
        break;
      }
    default:
      {
        vcl_cerr << "dbsk2d_bnd_preprocess::insert_new_vertices()\n" <<
          "Can't handle this type of edge\n";
        dbsk2d_assert(false);
      }
    }

    // replace `cur_edge' with the new vector of edges

    // containers for new_edges with reverse order and reverse directions
    // in case they are needed.
    vcl_vector<dbsk2d_bnd_edge_sptr > reverse_new_edges;
    vcl_vector<signed char > reverse_directions;


    // replace `cur_edge' with new edges, topologically
    while (cur_edge->numsup()>0)
    {
      vtol_topology_object* t = cur_edge->superiors_list()->front();
      dbsk2d_bnd_contour_sptr contour = static_cast<dbsk2d_bnd_contour* >(t);
      signed char dir = contour->direction(*cur_edge);
      // if direction of `cur_edge' in `contour' is -1 then we
      // need to reverse the order of new edges and sign of new directions
      if (dir == 1)
      {
        contour->replace_edges(new_edges, directions, cur_edge);  
      }
      else
      {
        // only reverse copy from `new_edges' to `reverse_new_edges' once
        if (reverse_new_edges.size() == 0)
        {
          reverse_new_edges.reserve(new_edges.size());
          vcl_reverse_copy(new_edges.begin(), new_edges.end(), 
            reverse_new_edges.begin());
          reverse_directions.assign(directions.size(), -1);
        }

        // replace the edge with new_edges whose orders are reversed
        contour->replace_edges(reverse_new_edges, reverse_directions, cur_edge);
      }
    }

    // replace `cur_edge' with new edges, geographically
    vcl_list<dbsk2d_bnd_cell* > cells = cur_edge->cells();
    cur_edge->unlink_from_cells();

    // add back the replacement of `cur_edge'
    for (vcl_list<dbsk2d_bnd_cell* >::iterator cit = cells.begin();
      cit != cells.end(); ++cit)
    {
      dbsk2d_bnd_cell_sptr cell = *cit;
  
      for (unsigned int i=0; i<new_edges.size(); ++i)
      {
        

        // ???? not consistent with new allocation method
        if (new_edges[i]->intersect_cell(cell, dbsk2d_bnd_preprocess::distance_tol))
        {
          cell->add_bnd_edge(new_edges[i]);
        }
      }
    }


    cur_edge->unlink();

    // update the overall edge list
    all_edges.remove(cur_edge);
    for (unsigned int i = 0; i < new_edges.size(); ++i)
    {
      all_edges.push_back(new_edges[i]);
    }

    affected_vertices.splice(affected_vertices.end(), vertices_along_edge);
  }

  tagged_union<dbsk2d_bnd_vertex_sptr >(&affected_vertices);
  return;
}


//: Dissolve end vertices into curves(lines, arcs) when they are too close
// Require: the vertex list is unique
void dbsk2d_bnd_preprocess::
dissolve_vertices_into_curves(vcl_list<dbsk2d_bnd_edge_sptr >& tainted_edges, 
                           vcl_list<dbsk2d_bnd_edge_sptr >& bnd_curves,
                           const vcl_list<dbsk2d_bnd_vertex_sptr >& vertices)
{
  // A list of modification to the edges to be made at the end 
  vkey_vertex_map modifications;

  // scan through all pairs of (vertex, line edge)
  for (bnd_vertex_list::const_iterator vit = vertices.begin();
    vit != vertices.end(); ++vit)
  {
    dbsk2d_bnd_vertex_sptr v = *vit;
  
    // list of coordinates that the vertex may be moved to (because
    // of curves it is close to)
    // these coordinates will be averaged to determine final position of vertex
    vcl_vector<vgl_point_2d<double > > target_pts;

    // `weak link' edges, i.e. epsilon < distance < 2*epsilon
    // these edges will be reconsidered after the vertex is moved to its
    // final position.
    bnd_edge_vector weak_link_curves;

    // loop through all edges and compute distance to the vertex
    for (bnd_edge_list::iterator eit = bnd_curves.begin(); 
      eit != bnd_curves.end(); ++eit)
    {
      dbsk2d_bnd_edge_sptr e = *eit;
      // ignore edges that have the considered vertex as end-point
      if (e->is_endpoint(v.ptr())) continue;

      switch (e->left_bcurve()->type())
      {
      case(BLINE):
        {
          // distance from the vertex to the line edge
          vgl_point_2d<double > pv = v->point();
          vgl_point_2d<double > p1 = e->point1();
          vgl_point_2d<double > p2 = e->point2();

          double d = vgl_distance_to_linesegment(p1.x(), p1.y(), 
            p2.x(), p2.y(), pv.x(), pv.y());

          // No link : do nothing
          if ( d > 2*dbsk2d_bnd_preprocess::distance_tol) continue;

          // Weak link : add the edge to `weak link' edge (to reconsider later)
          if ( d > dbsk2d_bnd_preprocess::distance_tol)
          {
            weak_link_curves.push_back(e);
            continue;
          }

          // Strong link: merge the point into the line

          // find location on line that is closest to the vertex
          double xp, yp;
          vgl_closest_point_to_linesegment<double >(xp, yp, 
            p1.x(), p1.y(), p2.x(), p2.y(), pv.x(), pv.y());
          vgl_point_2d<double > foot_pt(xp, yp);

          // add the point (xp, yp) to the target list
          target_pts.push_back(foot_pt);

          // mark vertex to add to the line at the end
          double ratio_pt = ratio<double >(p1, p2, foot_pt);
          vkey key(e, ratio_pt);
          modifications.insert(vkey_vertex_pair(key, v));
          break;
        }
      case (BARC):
        {
          // distance from the vertex to the arc edge
          vgl_point_2d<double > pv = v->point();
          vgl_point_2d<double > p1 = e->point1();
          vgl_point_2d<double > p2 = e->point2();
          dbsk2d_ishock_barc* barc = 
            static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());
          double arc_k = barc->curvature();

          double arc_ratio;
          double d = dbgl_closest_point::point_to_circular_arc(pv, p1, p2, 
            arc_k, arc_ratio);

          
          // No link : do nothing
          if ( d > 2*dbsk2d_bnd_preprocess::distance_tol) continue;

          // Weak link : add the edge to `weak link' edge (to reconsider later)
          if ( d > dbsk2d_bnd_preprocess::distance_tol)
          {
            weak_link_curves.push_back(e);
            continue;
          }

          // Strong link: merge the point into the line

          // find location on arc that is closest to the vertex
          dbgl_circ_arc arc(p1, p2, arc_k);
          vgl_point_2d<double > pt = arc.point_at_length(arc_ratio*arc.len());
        
          // add to the target list
          target_pts.push_back(pt);

          // mark vertex to add to the arc at the end
          vkey key(e, arc_ratio);
          modifications.insert(vkey_vertex_pair(key, v));
          break;
        
        }
      default:
        {
          vcl_cerr << "dbsk2d_bnd_preprocess::insert_new_vertices()\n" <<
            "Can't handle this type of edge\n";
          dbsk2d_assert(false);
        }
      }
    }

    // move the vertex to the new position and set the vertex
    if (!target_pts.empty())
    {
      // find centroid of all target points
      double centroid_x = 0; 
      double centroid_y = 0;
      for (unsigned int i=0; i<target_pts.size(); ++i)
      {
        centroid_x += target_pts[i].x();
        centroid_y += target_pts[i].y();
      }
      centroid_x = centroid_x / target_pts.size();
      centroid_y = centroid_y / target_pts.size();
      v->bpoint()->set_pt(centroid_x, centroid_y);
    }

    // reconsider the edges in the `weak-link' list
    // if any distance to the vertex has moved to the "epsilon" range
    // the edge will be broken but the coordinate of the vertex
    // will remain unchanged

    // loop through all `weak-link' edges and recompute distance
    for (bnd_edge_vector::iterator eit = weak_link_curves.begin();
      eit != weak_link_curves.end(); ++eit)
    {
      dbsk2d_bnd_edge_sptr e = *eit;
      switch (e->left_bcurve()->type())
      {
      case(BLINE):
        {
          // recompute distance from the vertex to the line edge
          vgl_point_2d<double > pv = v->point();
          vgl_point_2d<double > p1 = e->point1();
          vgl_point_2d<double > p2 = e->point2();

          double d = vgl_distance_to_linesegment(p1.x(), p1.y(), 
            p2.x(), p2.y(), pv.x(), pv.y());

          // do nothing when distance is above threshold
          if ( d > dbsk2d_bnd_preprocess::distance_tol) continue;

          // When distance is below threshold, merge vertex into the line
          // find location on line that is closest to the vertex
          double xp, yp;
          vgl_closest_point_to_linesegment<double >(xp, yp, 
            p1.x(), p1.y(), p2.x(), p2.y(), pv.x(), pv.y());
          vgl_point_2d<double > foot_pt(xp, yp);

          double ratio_pt = ratio<double >(p1, p2, foot_pt);
          vkey key(e, ratio_pt);
          modifications.insert(vkey_vertex_pair(key, v));
        }
      case (BARC):
        {
        
        // recompute distance from the vertex to the arc edge
          vgl_point_2d<double > pv = v->point();
          vgl_point_2d<double > p1 = e->point1();
          vgl_point_2d<double > p2 = e->point2();
          dbsk2d_ishock_barc* barc = 
            static_cast<dbsk2d_ishock_barc* >(e->left_bcurve());
          double arc_k = barc->curvature();

          double arc_ratio;
          double d = dbgl_closest_point::point_to_circular_arc(pv, p1, p2, 
            arc_k, arc_ratio);

          
          // do nothing when distance is above threshold
          if ( d > dbsk2d_bnd_preprocess::distance_tol) continue;

          // When distance is below threshold, merge vertex into the arc

          // find location on arc that is closest to the vertex
          dbgl_circ_arc arc(p1, p2, arc_k);
          vgl_point_2d<double > pt = arc.point_at_length(arc_ratio*arc.len());
        
          // mark vertex to add to the arc at the end
          vkey key(e, arc_ratio);
          modifications.insert(vkey_vertex_pair(key, v));
          break;
        }
      default:
        break;
      }
    }
  }
  bnd_vertex_list affected_vertices;
  this->insert_new_vertices(modifications, bnd_curves, affected_vertices);
  dbsk2d_bnd_utils::extract_edge_list(affected_vertices, tainted_edges); 
  return;
}





//-------------------------------------------------------------------------
//: Form bnd_contours from edges
// require: the list of edges must be preprocessed

void dbsk2d_bnd_preprocess::
form_contours_from_edges(const vcl_list<dbsk2d_bnd_edge_sptr >& edges,
    vcl_list<dbsk2d_bnd_contour_sptr >& new_contours)
{
  //this->update_vertex_list();
  bnd_vertex_list vertices;
  dbsk2d_bnd_utils::extract_vertex_list(edges, vertices);
  
  // sort the vertices so that vertices with order 2 are at the end of the list
  vcl_list<dbsk2d_bnd_vertex_sptr > order_2_vertices;
  
  for (vcl_list<dbsk2d_bnd_vertex_sptr >::iterator 
    vit = vertices.begin(); vit != vertices.end();)
  {
    // check order of this vertex
    edge_list elist;
    (*vit)->edges(elist);

    // separate out edges with order != 2
    if (elist.size() == 2)
    {
      order_2_vertices.push_back((*vit));
      vit = vertices.erase(vit);
    }
    else
    {
      ++vit;
    }
  }

  // merge two lists together to have a complete list with  
  // order-2 vertices at the end
  vertices.splice(vertices.end(), order_2_vertices);

  // reset flags on edges and vertices indicating they have been visited
  for (vcl_list<dbsk2d_bnd_edge_sptr >::const_iterator eit = edges.begin(); 
    eit != edges.end(); ++eit)
  {
    (*eit)->unset_user_flag(VSOL_FLAG1);
  }

  for(vcl_list<dbsk2d_bnd_vertex_sptr >::iterator vit = vertices.begin();
    vit != vertices.end(); ++vit)
  {
    (*vit)->unset_user_flag(VSOL_FLAG1);
  }


  // The algorithm:
  // For each vertex v0, check if it is a starting vertex of an edge,
  // i.e. vertices with degree different from 2
  // For each edge that v0 is a starting vertex of
  // trace along the edge till hitting an end, i.e. vertex with order different from 2
  // Form a contour from the edges and marked them as traversed.
  // Repeat this for ever vertex
  for (vcl_list<dbsk2d_bnd_vertex_sptr >::iterator vit_s = 
    vertices.begin(); vit_s != vertices.end(); ++vit_s)
  {
    // obtain the list of edges connected to this vertex
    edge_list cur_elist;
    (*vit_s)->edges(cur_elist);
    
    //// vertex with degree 2 is not an end-vertex
    //if (cur_elist.size() == 2) continue;
    
    (*vit_s)->set_user_flag(VSOL_FLAG1);
    // loop through all direction to form contour
    
    

    for (edge_list::iterator 
      eit = cur_elist.begin(); eit != cur_elist.end(); ++eit)
    {
      // skip edges that have been visited
      if ((*eit)->get_user_flag(VSOL_FLAG1)) continue;
     
      dbsk2d_bnd_vertex_sptr tracing_vertex = (*vit_s);
      dbsk2d_bnd_edge_sptr tracing_edge = 
        static_cast<dbsk2d_bnd_edge* >((*eit).ptr());

       //vector to hold edges that will be used to form a contour
      vcl_vector< dbsk2d_bnd_edge_sptr > connected_edges;
      bool contour_stopped = false;
      
      while (!contour_stopped)
      {
        //safety check
        if (! tracing_edge->is_class("dbsk2d_bnd_edge"))
        {
          vcl_cerr << "Edge is not of class <dbsk2d_bnd_edge>\n";
          break;
        };

        // reverse edge direction if necessary
        // \TODO - just set the directions_ to -1
        // avoid physically reverse the direction of the edge. it is expensive.
        if (tracing_edge->v1() != tracing_vertex.ptr() ) 
          tracing_edge->reverse_direction();

        // safety check
        dbsk2d_assert (tracing_edge->v1() == tracing_vertex.ptr());

        // add the edge to the to-form-contour edge list
        connected_edges.push_back( tracing_edge);

        // safety check - make sure this edge was not visited previously
        //\TODO - deal with this, run-time error is bad
        dbsk2d_assert( ! tracing_edge->get_user_flag(VSOL_FLAG1));

        tracing_edge->set_user_flag(VSOL_FLAG1);

        // update next vertex and check if it is end-vertex of the contour
        tracing_vertex = tracing_edge->bnd_v2();
        tracing_vertex->set_user_flag(VSOL_FLAG1);

        if (tracing_vertex == (*vit_s))
        {
          // this is an end-vertex of a close-loop contour
          contour_stopped = true;
          continue;
        };

        edge_list temp_elist;
        tracing_vertex->edges(temp_elist);

        if (temp_elist.size() != 2) 
        {
          // this is an end-vertex of a contour
          contour_stopped = true;
          continue;
        }
        
        // the contour is not ended yet. Find the next edge
        vtol_edge_sptr temp_edge = (temp_elist.front() == tracing_edge.ptr()) ?
          temp_elist.back() : temp_elist.front();
        tracing_edge = static_cast<dbsk2d_bnd_edge* >(temp_edge.ptr());
      }
      // save the new contour to the preprocessed contour list
      if (contour_stopped){
        dbsk2d_bnd_contour_sptr new_con = new dbsk2d_bnd_contour(connected_edges,
          this->boundary()->nextAvailableID());
        // vcl_cout << "Number of edges of new_con = " << new_con->num_edges() << vcl_endl;
        if (new_con->num_edges() > 0)
          //this->preproc_contours_.push_back( new_con);
          new_contours.push_back( new_con);
      }
    }
  }

}




