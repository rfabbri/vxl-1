// This is dbmsh3d/algo/dbmsh3d_pt_set_correspondence.cxx

//:
// \file


#include "dbmsh3d_pt_set_correspondence.h"
#include <vcl_iostream.h>
#include <vcl_cmath.h>

#include <vsol/vsol_point_3d.h>
#include <bsol/bsol_point_index_3d.h>




// ==================== dbmsh3d_match_mesh_using_euclidean_distance ===========

dbmsh3d_match_mesh_using_euclidean_distance::
dbmsh3d_match_mesh_using_euclidean_distance( dbmsh3d_mesh* reference_mesh,
                                            dbmsh3d_mesh* data_mesh,
                                            double max_distance): 
  dbmsh3d_match_mesh()

{
  this->set_reference_mesh(reference_mesh);
  this->set_data_mesh(data_mesh);
  this->set_max_distance(max_distance);
  this->correspondence_map_.clear();
  this->distance_map_.clear();
  this->match();
}




// ----------------------------------------------------------------------------
//: Match the two meshes
void dbmsh3d_match_mesh_using_euclidean_distance::
match()
{
  // put the points of target_mesh in a grid to speed up search for closest point

  // >> convert the point set of reference mesh into a vector of vsol points
  dbmsh3d_mesh* ref_mesh = this->reference_mesh();
  vcl_vector<vsol_point_3d_sptr > ref_points;
  ref_points.reserve(ref_mesh->num_vertices());

  ref_mesh->reset_vertex_traversal();
  dbmsh3d_vertex* v0;
  while(ref_mesh->next_vertex(v0))
  {
    dbmsh3d_vertex* v = static_cast<dbmsh3d_vertex*>(v0);
    assert(v);

    vsol_point_3d_sptr pt = new vsol_point_3d(v->pt());
    pt->set_id(v->id());
    ref_points.push_back(pt);  
  }
  

  // approximate the optimal number of rows, columns, and slabs using the total
  // number of points

  // This number just comes from the sky, change it if found in efficient/////////
  int num_pts_per_cell = 50;
  /////////////////////////////////////////////////////


  int num_cells = ref_points.size() / num_pts_per_cell;
  int num_rows = (int)vcl_pow(num_cells, 1.0/3);
  int num_cols = num_rows;
  int num_slabs = num_rows;

  

  bsol_point_index_3d point_grid(num_rows, num_cols, num_slabs, ref_points);


  // >> For every point in the data mesh, find its closest point int the
  // reference mesh by searching in the point grid
  dbmsh3d_mesh* data_mesh = this->data_mesh();
  data_mesh->reset_vertex_traversal();

  int counter = 0;
  int ten_percent_points = data_mesh->num_vertices() / 10;
  while(data_mesh->next_vertex(v0))
  {
    ++counter;
    if (counter > ten_percent_points)
    {
      vcl_cout << "+10% done.\n";
      counter = 0;
    }


    dbmsh3d_vertex* v = static_cast<dbmsh3d_vertex*>(v0);
    assert(v);

    vsol_point_3d_sptr query = new vsol_point_3d(v->pt());
    query->set_id(v->id());

    vsol_point_3d_sptr closest_pt;
    point_grid.closest_in_radius(this->max_distance(), query, closest_pt);
    if (closest_pt)
    {

      this->correspondence_map_.insert(vcl_make_pair(query->get_id(), closest_pt->get_id()));
      this->distance_map_.insert(
        vcl_make_pair(query->get_id(), query->distance(closest_pt)));
    }
  }
}


