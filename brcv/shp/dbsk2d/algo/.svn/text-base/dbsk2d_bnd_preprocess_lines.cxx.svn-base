// This is brcv/shp/dbsk2d/algo/dbsk2d_bnd_preprocess_lines.cxx

//:
// \file



#include "dbsk2d_bnd_preprocess.h"

#include <vcl_algorithm.h>
#include <vgl/vgl_lineseg_test.h>
#include <vgl/vgl_closest_point.h>
#include <vgl/vgl_distance.h>

#include <dbgl/algo/dbgl_closest_point.h>
#include <dbsk2d/dbsk2d_bnd_utils.h>




//-------------------------------------------------------------------------
//: Detect and form all intersection between `lineset1' and `lineset2'
// if `lineset2' not given then the function intersect `lineset1' against 
//itself
// Return: `lineset1' contains all lines from both sets and `lineset2' will 
// be empty
// `tainted_lines' contains all lines affected by intersection and will need
// further processing
void dbsk2d_bnd_preprocess::
intersect_bnd_lines(vcl_list<dbsk2d_bnd_edge_sptr >* tainted_lines,
                    vcl_list<dbsk2d_bnd_edge_sptr >* lineset1, 
                    vcl_list<dbsk2d_bnd_edge_sptr >* lineset2)
{
  // clear old stuffs
  tainted_lines->clear();

  // For every pair of lines, form an X junction when they intersect
  // The junctions are first marked, then formed at the end.

  bnd_edge_list* lineset2b = (lineset2) ? lineset2 : lineset1;
  
  // A list of modification to the edges to be made at the end
  vkey_vertex_map toadd_vertices;

  // scan through all pair of lines
  for (bnd_edge_list::iterator eit1 = lineset1->begin(); 
    eit1 != lineset1->end(); ++eit1)
  {
    bnd_edge_list::iterator eit2;
    if (lineset2 && (lineset2 != lineset1))
    {
      eit2 = lineset2->begin();
    }
    // no need to start from begining when intersecting lineset1 against itself
    else
    {
      eit2 = eit1;
      ++eit2;
    }

    dbsk2d_bnd_edge_sptr e1 = *eit1;

    // endpoints and length of line 1
    vgl_point_2d<double > p11 = e1->point1();
    vgl_point_2d<double > p12 = e1->point2();

    while (eit2 != lineset2b->end())
    {
      dbsk2d_bnd_edge_sptr e2 = *eit2;
      if (e1 == e2)
      {
        eit2 = lineset2b->erase(eit2);
        continue;
      }
    
      // Ignore topologically connected pair of edges because their
      // intersection has already been recorded
      if ( e1->share_vertex_with(e2.ptr())) 
      {
        ++eit2;
        continue;
      }
      
      // Do nothing when the two lines don't intersect

      // endpoints and length of line 2
      vgl_point_2d<double > p21 = e2->point1();
      vgl_point_2d<double > p22 = e2->point2();

      if (!vgl_lineseg_test_lineseg<double>(p11.x(), p11.y(), p12.x(), p12.y(),
        p21.x(), p21.y(), p22.x(), p22.y())) 
      {
        ++eit2;
        continue;
      }

      // when the two lines intersect, find the intersection 
      // and modify the edges
      double ratio1, ratio2;
      double d = dbgl_closest_point::lineseg_lineseg(p11, p12, p21, p22, ratio1, ratio2);

      //////////////////////////////////////
      // safety check
      dbsk2d_assert(d==0);
      dbsk2d_assert(ratio1 >= 0 && ratio1 <= 1 && ratio2 >= 0 && ratio2 <= 1);
      ////////////////////////////////////////

      // ignore the cases where one of the end points is the intersection point
      // these cases will be taken care of later when end points are dissolved
      // into line segments close to it

      if (ratio1 == 0 || ratio1 == 1 || ratio2 == 0 || ratio2 == 1)
      {
        ++eit2;
        continue;
      }
      
      // intersection point
      vgl_point_2d<double > new_pt = midpoint<double >(p11, p12, ratio1);
      // create a new vertex for the intersecting point
      dbsk2d_bnd_vertex_sptr new_v = 
        dbsk2d_bnd_utils::new_vertex(new_pt,this->boundary());

      vkey key1(e1, ratio1);
      toadd_vertices.insert(vkey_vertex_pair(key1, new_v) );
      vkey key2(e2, ratio2);
      toadd_vertices.insert(vkey_vertex_pair(key2, new_v) );

      // move to next edge on the list
      ++eit2;
    }
  }

  // combine the two list and save to `lineset1'
  if (lineset2 && (lineset2 != lineset1))
    lineset1->splice(lineset1->end(), *lineset2);

  // get the list of line edges affected by this operation
  bnd_vertex_list affected_vertices;
  this->insert_new_vertices(toadd_vertices, *lineset1, affected_vertices);

  bnd_edge_list all_affected_edges;
  dbsk2d_bnd_utils::extract_edge_list(affected_vertices, all_affected_edges);
  this->classify_edges(all_affected_edges, 0, tainted_lines, 0);
  return;
}



//----------------------------------------------------------------
//: Remove (exact) duplicate lines - lines with same end vertices
void dbsk2d_bnd_preprocess::
remove_duplicate_lines(vcl_list<dbsk2d_bnd_edge_sptr >& bnd_lines)
{
  for (bnd_edge_list::iterator eit1 = bnd_lines.begin();
    eit1 != bnd_lines.end(); ++eit1)
  {
    dbsk2d_bnd_edge_sptr e1 = *eit1;

    // required
    dbsk2d_assert(e1->left_bcurve()->is_a_line());

    bnd_edge_list::iterator eit2 = eit1;
    ++eit2;
    while (eit2 != bnd_lines.end())
    {
      dbsk2d_bnd_edge_sptr e2 = *eit2;

      // required
      dbsk2d_assert (e2->left_bcurve()->is_a_line());

      // check if the line edges are exactly the same
      if (!(e1->is_endpoint(e2->v1()) && e1->is_endpoint(e2->v2())))
      {
        ++eit2;
        continue;
      }

      e1->merge_with(*e2);

      // erase e2 from the overall list
      eit2 = bnd_lines.erase(eit2);
    }
  }
  return;
}



