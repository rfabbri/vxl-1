#include <vcl_iostream.h>
#include <vgl/vgl_distance.h>
#include <vul/vul_printf.h>

#include <rsdl/rsdl_point.h>
#include <rsdl/rsdl_kd_tree.h>

#include <dbgl/dbgl_dist.h>
#include <dbgl/dbgl_rigid_align.h>
#include <dbmsh3d/algo/dbmsh3d_pt_mesh_dist.h>
#include <dbmsh3dr/dbmsh3dr_match.h>

//: Run the point-point ICP registration.
//  Return the final homogeneous transformation matrix.
//
bool dbmsh3dr_pp_icp_regstr (const vcl_vector<vgl_point_3d<double> >& fixPS, 
                             vcl_vector<vgl_point_3d<double> >& movPS, 
                             const int max_iter, const float conv_th, const double dist_th,
                             vgl_h_matrix_3d<double>& H)
{
  #if DBMSH3D_DEBUG > 2
  vul_printf (vcl_cout, "\ndbmsh3dr_pp_icp_regstr(): fixPS %d, movPS %d.\n", fixPS.size(), movPS.size());
  vul_printf (vcl_cout, "  max_iter %d, conv_th %f\n", max_iter, conv_th);   
  #endif

  //Reset H to identity.
  //H should be the init. xform.
  ///H.set_identity();

  //The registration parameters.
  vnl_matrix_fixed<double,3,3> R;
  vnl_vector_fixed<double,3> fixC;
  vnl_vector_fixed<double,3> movC;

  //The corresponding point sets.
  vcl_vector<vgl_point_3d<double> > cor_fixPS;
  vcl_vector<vgl_point_3d<double> > cor_movPS;

  //:1) Initilization: Build a kd-tree for the fixed set of points.
  rsdl_kd_tree* kdtree = dbmsh3d_build_kdtree_pts (fixPS);

  //:2) In each iteration, for each point on moving point set movPS, 
  //    find the closest point on the fixed point set fixedPS as correspondence,
  //    if the correspondence dist is < dist_th.
  int nIteration = 0;

START_ICP_ITERATION:

  //initializing
  cor_fixPS.clear();
  cor_movPS.clear();

  nIteration++;
  #if DBMSH3D_DEBUG > 2
  vul_printf (vcl_cout, "\nPt-Pt ICP Iteration: %d\n", nIteration);
  #endif
  
  //Go through each point in the moving set movPS.
  for (unsigned int i=0; i<movPS.size(); i++) {
    //Find closest point on fixed set fixPS as correspondence.
    int top_n = 1;
    vcl_vector<rsdl_point> near_neighbor_pts;
    vcl_vector<int> near_neighbor_indices;
    rsdl_point query_pt (3, 0);
    vnl_vector_fixed<double,3> P3 (movPS[i].x(), movPS[i].y(), movPS[i].z());
    query_pt.set_cartesian (P3);
    kdtree->n_nearest (query_pt, top_n, near_neighbor_pts, near_neighbor_indices);
    assert (near_neighbor_indices.size() == top_n);
    int idx = near_neighbor_indices[0];

    //Compute the closest_dist
    double dist = vgl_distance (movPS[i], fixPS[idx]);

    //Put the closest point on fixPS to the corresponding fixed set cor_fixPS,
    //if their dist < dist_th (generating a pulling force).
    if (dist < dist_th) {           
      cor_movPS.push_back (movPS[i]); //cor_movPS = movPS in every iteration.
      cor_fixPS.push_back (fixPS[idx]);
    }
  }

  #if DBMSH3D_DEBUG > 2
  vul_printf (vcl_cout, "  Skip %u points out of dist_th.\n", movPS.size() - cor_movPS.size());
  #endif
  assert (cor_movPS.size() != 0);

  //:3) Find the xform for this iteration.
  dbgl_get_rigid_alignment (cor_fixPS, cor_movPS, R, fixC, movC);

  //:4) Rigidly transform movPS to approach fixPS.
  for (unsigned int i=0; i<movPS.size(); i++) {
    double x = movPS[i].x();
    double y = movPS[i].y();
    double z = movPS[i].z();
    transform_point_3d (R, fixC, movC, x, y, z);
    movPS[i].set (x, y, z);
  }

  //Add the current xform to H.
  vgl_h_matrix_3d<double> Hc = set_h_matrix (R, fixC, movC);
  H = Hc*H;

  //:5) Compute convergence and test if iteration is enough.
  //    Stop iteration if # iter > max_iter or H3x3 converges.
  double H33RMS = estimate_ICP_conv (R);
  #if DBMSH3D_DEBUG > 2
  vul_printf (vcl_cout, "  Pt-Pt ICP convergence value: %f.\n", H33RMS);
  #endif
  if (nIteration < max_iter && H33RMS > conv_th)
    goto START_ICP_ITERATION;
  
  #if DBMSH3D_DEBUG > 2
  if (H33RMS <= conv_th)
    vul_printf (vcl_cout, "\n  Pt-Pt ICP converges.\n\n");
  else
    vul_printf (vcl_cout, "\n  Pt-Pt ICP does not converge!\n\n");
  vcl_cout << "    Registration result hmatrix: " << vcl_endl << H;
  #endif

  delete kdtree;
  return H33RMS <= conv_th; //return ICP converge or not
}


//: Run the point-point ICP registration.
//  Return the final homogeneous transformation matrix.
//
bool dbmsh3dr_pf_icp_regstr (dbmsh3d_mesh* fixM, dbmsh3d_mesh* movM, 
                             const int max_iter, const float conv_th, const double dist_th,
                             vgl_h_matrix_3d<double>& H)
{
  vul_printf (vcl_cout, "dbmsh3dr_pf_icp_regstr():\n");
  vul_printf (vcl_cout, "  fixM (V: %u, F: %u), movM (V: %d, F: %u).\n", 
              fixM->vertexmap().size(), fixM->facemap().size(), 
              movM->vertexmap().size(), movM->facemap().size());
  vul_printf (vcl_cout, "  max_iter %d, conv_th %f\n", max_iter, conv_th);   

  //The registration parameters.
  vnl_matrix_fixed<double,3,3> R;
  vnl_vector_fixed<double,3> fixC;
  vnl_vector_fixed<double,3> movC;

  //The corresponding point sets.
  vcl_vector<vgl_point_3d<double> > cor_movPS;
  vcl_vector<vgl_point_3d<double> > cor_fixPS;

  //:1) Initilization: Build a kd-tree for the fixed set of points.
  rsdl_kd_tree* kdtree = dbmsh3d_build_kdtree_vertices (fixM);

  //:2) In each iteration, for each vertex on moving point set movM, 
  //    find the closest point on the fixed mesh fixM as correspondence.
  int nIteration = 0;
  H.set_identity();

START_ICP_ITERATION:

  //initializing
  cor_movPS.clear();
  cor_fixPS.clear();

  nIteration++;
  vul_printf (vcl_cout, "\nPt-Plane ICP Iteration: %d\n", nIteration);
  
  //Go through each vertices in the moving set movM.
  vcl_map<int, dbmsh3d_vertex*>::iterator it = movM->vertexmap().begin();
  for (; it != movM->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;

    //Find closest point on fixed mesh fixM as correspondence.
    int top_n = 1;
    vgl_point_3d<double> G; //the closest point.
    double dist = dbmsh3d_pt_mesh_dist (V->pt(), fixM, kdtree, top_n, G);
    
    //Put the closest point on fixPS to the corresponding fixed set cor_fixPS,
    //if their dist < dist_th (generating a pulling force).
    if (dist < dist_th) {
      cor_movPS.push_back (V->pt()); //cor_movPS = movPS in every iteration.    
      cor_fixPS.push_back (G); //add the closest point G on fixM.
    }
  }

  vul_printf (vcl_cout, "  Skip %u points out of dist_th.\n", movM->vertexmap().size() - cor_movPS.size());
  assert (cor_movPS.size() != 0);

  //:3) Find the xform for this iteration.
  dbgl_get_rigid_alignment (cor_fixPS, cor_movPS, R, fixC, movC);

  //:4) Rigidly transform movM to approach fixM.
  it = movM->vertexmap().begin();
  for (; it != movM->vertexmap().end(); it++) {
    dbmsh3d_vertex* V = (*it).second;
    double x = V->pt().x();
    double y = V->pt().y();
    double z = V->pt().z();
    transform_point_3d (R, fixC, movC, x, y, z);
    V->get_pt().set (x, y, z);
  }

  //Add the current xform to H.
  vgl_h_matrix_3d<double> Hc = set_h_matrix (R, fixC, movC);
  H = Hc*H;

  //:5) Compute convergence and test if iteration is enough.
  //    Stop iteration if # iter > max_iter or H3x3 converges.
  double H33RMS = estimate_ICP_conv (R);
  vul_printf (vcl_cout, "  Pt-Mesh ICP convergence value: %f.\n", H33RMS);
  if (nIteration < max_iter && H33RMS > conv_th)
    goto START_ICP_ITERATION;

  if (H33RMS <= conv_th)
    vul_printf (vcl_cout, "\n  Pt-Mesh ICP converges.\n\n");
  else
    vul_printf (vcl_cout, "\n  Pt-Mesh ICP does not converge!\n\n");
  vcl_cout << "    Registration result hmatrix: " << vcl_endl << H;

  delete kdtree;
  return H33RMS <= conv_th; //return ICP converge or not
}

vgl_h_matrix_3d<double> set_h_matrix (const vnl_matrix_fixed<double,3,3>& R, 
                                      const vnl_vector_fixed<double,3>& fixC,
                                      const vnl_vector_fixed<double,3>& movC)
{
  // R is the upper-left 3x3 of H.
  vnl_matrix_fixed<double,4,4> H44;
  unsigned int r, c;
  for (r=0; r<3; r++) 
    for (c=0; c<3; c++) 
      H44(r,c) = R(r,c);

  // T = Cen1-R*Cen2
  vnl_vector_fixed<double,3> T = fixC;
  T = T - R*movC;

  for (r=0; r<3; r++) 
    H44(r,3) = T(r);
  for (c=0; c<3; c++) 
    H44(3,c) = 0;
  H44(3,3) = 1;
  vgl_h_matrix_3d<double> H;
  H.set (H44);
  return H;
}

//: Compare R to I and estimate RMS.
double estimate_ICP_conv (const vnl_matrix_fixed<double,3,3>& R)
{
  double sum = 0;
  for (unsigned int r=0; r<3; r++) {
    for (unsigned int c=0; c<3; c++) {
      if (r==c) 
        sum += (R(r,c)-1)*(R(r,c)-1);
      else
        sum += R(r,c)*R(r,c);
    }
  }

  sum /= 9;
  sum = vcl_sqrt (sum);
  return sum;
}

