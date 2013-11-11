//---------------------------------------------------------------------
// This is brcv/shp/dbmsh3d/dbmsh3d_pt_normal.cxx
//-------------------------------------------------------------------------

#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/algo/vnl_scatter_3x3.h>

#include <rsdl/rsdl_kd_tree.h>

#include <dbmsh3d/algo/dbmsh3d_pt_normal.h>



//: Compute point normal and store in the vectors.
void compute_point_normal (vcl_vector<vcl_pair<vgl_point_3d<double>, vgl_vector_3d<double> > >& oripts)
{
  vcl_vector<vnl_vector_fixed<double,3> > pts;
  for (unsigned int i=0; i<oripts.size(); i++) {
    vnl_vector_fixed<double, 3> v;
    v.put (0, oripts[i].first.x());
    v.put (1, oripts[i].first.y());
    v.put (2, oripts[i].first.z());
    pts.push_back (v);
  }

  vcl_vector<vnl_vector_fixed<double,3> > normals;
  estimate_point_normal (pts, normals);

  assert (normals.size() == oripts.size());
  for (unsigned int i=0; i<normals.size(); i++) {
    vnl_vector_fixed<double, 3> v = normals[i];
    oripts[i].second.set (v.get(0), v.get(1), v.get(2));
  }

}

vnl_vector_fixed<double,3> approximate_normal (const vcl_vector<vnl_vector_fixed<double,3> >& points)
{
  // Compute the center of the points
  //
  vnl_vector_fixed<double,3> center(0.0, 0.0, 0.0);
  int num_pts = points.size();
  for ( int i=0; i<num_pts; i++)
    center += points[i];
  center /= num_pts;

  // Compute the scatter matrix
  //
  vnl_scatter_3x3<double> scatter_matrix;

  for ( int i=0; i<num_pts;i++) {
    vnl_vector_fixed<double,3> shifted_pt = points[i] - center;
    scatter_matrix.add_outer_product(shifted_pt);
  }

  // Using Lagrange Multiplier, the normal of the plane corresponds to
  // the eigenvector of the least eigenvalue
  //
  vnl_vector_fixed<double,3> normal = scatter_matrix.minimum_eigenvector();

  return normal;
}

//: Estimate the normal of a point set via the rgrl code using Lagrange Multiplier.
//  The normal of the fitted plane corresponds to the eigenvector of the least eigenvalue.
void estimate_point_normal (const vcl_vector<vnl_vector_fixed<double,3> >& pts,
                            vcl_vector<vnl_vector_fixed<double,3> >& normals)
{
  // Store all the points in the kd-tree
  //
  int total = pts.size();
  const unsigned int nc = 3, na = 0;
  vcl_vector<rsdl_point> search_pts (total);
  for (int i = 0; i<total; ++i) {
    search_pts[i].resize( nc, na );
    search_pts[i].set_cartesian (pts[i]);
  }
  rsdl_kd_tree kd_tree( search_pts );

  // For each data point, generate the normal to the plane. It first
  // collects a set of nearby points from the kd_tree. It then fits
  // the best plane using the set of points. The normal to the plane
  // is taken as the normal to the data point.
  //
  vcl_vector< vnl_vector_fixed<double,3> > near_neighbors;
  vcl_vector<rsdl_point> near_neighbor_pts;
  vcl_vector<int> near_neighbor_indices;
  int num_nb = 4;
  for (int i = 0; i<total; ++i) {
    rsdl_point query_pt (3, 0);
    query_pt.set_cartesian(pts[i]);
    kd_tree.n_nearest(query_pt, num_nb, near_neighbor_pts, near_neighbor_indices);
    near_neighbors.clear();
    for (int j = 0; j<num_nb; ++j)
      near_neighbors.push_back(pts[near_neighbor_indices[j]]);

    vnl_vector_fixed<double,3> normal = approximate_normal (near_neighbors);
    ///ostr<<points[i]<<' '<<normal<<vcl_endl;
    normals.push_back (normal);
  }
}


