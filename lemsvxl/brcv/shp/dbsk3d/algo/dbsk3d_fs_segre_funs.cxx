//: dbsk3d_fs_segre_funs.cxx
//: MingChing Chang
//  Dec 14, 2006

#include <dbsk3d/algo/dbsk3d_fs_segre.h>


double compute_bending_angle (const dbsk3d_fs_edge* inputL,
                              const dbsk3d_fs_face* FF,
                              const dbsk3d_fs_edge* FE)
{
  //compute by dot_product of the two shock link vectors.
  //locally orient the vector direction (sign) using G0, G1 of shock patch.
  const dbmsh3d_vertex* G0 = FF->genes (0);
  const dbmsh3d_vertex* G1 = FF->genes (1);
  const dbmsh3d_vertex* G2 = inputL->A13_opposite_G (FF);
  const dbmsh3d_vertex* G3 = FE->A13_opposite_G (FF);

  const vgl_vector_3d<double> G0G1 = G1->pt() - G0->pt();
  const vgl_vector_3d<double> G1G2 = G2->pt() - G1->pt();
  const vgl_vector_3d<double> G0G3 = G3->pt() - G0->pt();

  const vgl_vector_3d<double> input_link_vector = cross_product (G0G1, G1G2);
  const vgl_vector_3d<double> link_vector = cross_product (-G0G1, G0G3);

  double cos_theta = dot_product (input_link_vector, link_vector);
  cos_theta /= (input_link_vector.length() * link_vector.length());

  double theta;
  if (dbgl_eq_m (cos_theta, 1))
    theta = 0;
  else
    theta = vcl_acos (cos_theta);
  return theta;
}

//: return true if the surface interpolant of the input link
//  fills a hole on the reconstructed surface.
//  condition: all incident sheet_elms are one side meshed.
bool L_check_fill_hole (dbsk3d_fs_edge* FE)
{
  dbmsh3d_halfedge* he = FE->halfedge();
  dbsk3d_fs_face* FF = (dbsk3d_fs_face*) he->face();
  dbmsh3d_vertex* startG = FF->genes(0);  
  dbmsh3d_vertex* G = startG;

  do {
    if (FF->one_side_meshed() == false)
      return false;
    
    //the next FF that connected to G
    G = FF->other_G (G);
    FF = FE->other_FF_of_G (FF, G);
  }
  while (G != startG && FF != NULL);

  return true;
}

