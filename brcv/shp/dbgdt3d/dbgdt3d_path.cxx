//: Aug 19, 2005 MingChing Chang
//  

#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbgdt3d/dbgdt3d_path.h>
#include <dbgdt3d/dbgdt3d_mesh.h>

//: Given a query_dist, return the <i, s> representation
//  i: the i-th interval in the back-tracking
//  s: the local arc-length from i-th interval to (i+1) th interval.
int gdt_path::iter_from_query_dist (double query_dist, double& local_s)
{
  //: start from the beginning of back-tracking
  gdt_interval* I = I_tau_pairs_[0].first;
  double tau = I_tau_pairs_[0].second;

  return I->I_iter_to_source (tau, query_dist, local_s);
}

//: given an I_iter in back-tracking the gdt_path,
//  find the next face which contains interval of I_iter and interval of I_iter+1.
//  Be careful that one of them may be a pseudo-source of the gdt_path.
//
dbmsh3d_face* gdt_path::get_face_from_I_iter (int i)
{
  assert (i+1 < (int) I_tau_pairs_.size());

  //: the cur_edge
  gdt_interval* prev_I = I_tau_pairs_[i].first;
  double cur_tau = I_tau_pairs_[i].second;
  dbmsh3d_gdt_edge* cur_edge = (dbmsh3d_gdt_edge*) prev_I->edge ();

  //: if cur_edge is a pseudo-source of the gdt_path, locate the vertex1
  dbmsh3d_vertex* vertex1 = NULL;
  if (cur_tau == 0)
    vertex1 = cur_edge->sV ();
  else if (cur_tau == cur_edge->len())
    vertex1 = cur_edge->eV ();

  //: the next_edge
  gdt_interval* next_I = I_tau_pairs_[i+1].first;
  double next_tau = I_tau_pairs_[i+1].second;
  dbmsh3d_gdt_edge* next_edge = (dbmsh3d_gdt_edge*) next_I->edge ();

  //: if next_edge degenerate, locate the vertex2
  dbmsh3d_vertex* vertex2 = NULL;
  if (next_tau == 0)
    vertex2 = next_edge->sV ();
  else if (next_tau == next_edge->len())
    vertex2 = next_edge->eV ();

  //: determine the cur_face
  dbmsh3d_face* cur_face;
  if (vertex1 == NULL) {
    if (vertex2 == NULL)
      cur_face = cur_edge->incident_F_given_E (next_edge);
    else
      cur_face = cur_edge->incident_F_given_V (vertex2);
  }
  else {
    if (vertex2 == NULL)
      cur_face = next_edge->incident_F_given_V (vertex1);
    else {
      //: the double-degeneracy
      //  just return one face incident to both vertices
      //  find the common edge
      assert (vertex1 != vertex2);
      dbmsh3d_edge* edge = E_sharing_2V (vertex1, vertex2);
      cur_face = edge->halfedge()->face();
      ///return cur_edge->halfedge()->face ();
    }
  }
  assert (cur_face);

  return cur_face;
}

vgl_point_3d<double> gdt_path::get_query_point (int i, double local_s)
{
  assert (i+1 < (int) I_tau_pairs_.size());

  //: the prev_pt
  vgl_point_3d<double> prev_pt = get_point (i);

  //: the cur_pt
  vgl_point_3d<double> cur_pt = get_point (i+1);

  //linear_interpolation to get the query point
  vgl_vector_3d<double> v = cur_pt - prev_pt;
  double l = v.length();
  double r = local_s / l;

  //: handle numerical error here.
  if (_eqT (r, 1)) //eqT
    r = 1;

  v = v * r;

  vgl_point_3d<double> query_point = prev_pt + v;

  return query_point;
}

//: given the iteration number, return the extrinsic point coord.
vgl_point_3d<double> gdt_path::get_point (unsigned int i)
{
  assert (i < I_tau_pairs_.size());

  gdt_interval* I = I_tau_pairs_[i].first;
  vgl_point_3d<double> pt = I->_point_from_tau (I_tau_pairs_[i].second);

  return pt;
}

void gdt_path::print_path ()
{
  vul_printf (vcl_cout, "Geodesic path: length %lf, to source %d\n", 
               get_length(), get_closest_source()->id());

  double cumu_len = 0;
  //: loop through the geodesic_path and print it
  for (unsigned int i=0; i<I_tau_pairs_.size(); i++) {
    gdt_interval* I = I_tau_pairs_[i].first;
    double tau = I_tau_pairs_[i].second;

    vgl_point_3d<double> cur_pt = get_point (i);

    //: print the path between [i-1] to [i]
    if (i>0) {
      vgl_point_3d<double> prev_pt = get_point (i-1);
      assert (cur_pt != prev_pt);
      double path_len = vgl_distance (prev_pt, cur_pt);
      cumu_len += path_len;
      vul_printf (vcl_cout, "       len (%d-%d): %lf, cumulative len: %lf\n", i-1, i, path_len, cumu_len);
    }

    //: print the [i]-th point
    vul_printf (vcl_cout, "     Point[%d] on edge %d with tau %lf, (%.2f, %.2f, %.2f)\n", 
                 i, I->edge()->id(), tau, 
                 cur_pt.x(), cur_pt.y(), cur_pt.z());
  }
}







