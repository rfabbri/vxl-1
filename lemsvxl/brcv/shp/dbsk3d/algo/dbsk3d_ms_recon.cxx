//: MingChing Chang
//  Mar 02, 2004  Creation
//  3D ShockGraph-Boundary Reconstruction

#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include <dbsk3d/algo/dbsk3d_ms_recon.h>


// ###########################################################################
//Ridge boundary curve detection

#define NUM_LOOK_BACK 10

bool _add_to_ridgebnd (vcl_vector<vgl_point_3d<double> >& ridgebnd_curve,
                       const vgl_point_3d<double> Ga)
{
  //Check if Pa already exists in ridgebnd_curve or not
  //by looking back N elements.
  int s = ridgebnd_curve.size();
  for (int i = s-1; i > s-NUM_LOOK_BACK && i>0; i--) {
    vgl_point_3d<double> p = ridgebnd_curve[i];
    if (p == Ga)
      return false;
  }

  ridgebnd_curve.push_back (Ga);
  return true;
}

void get_ridge_bnd_curves (dbsk3d_ms_curve* SCurve, 
                           vcl_vector<vgl_point_3d<double> >& ridgebnd_curve_a, 
                           vcl_vector<vgl_point_3d<double> >& ridgebnd_curve_b)
{
  assert (SCurve->c_type() == C_TYPE_RIB);
  ridgebnd_curve_a.clear();
  ridgebnd_curve_b.clear();

  dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) SCurve->E_vec(0);
  dbsk3d_fs_face* FF = (dbsk3d_fs_face*) FE->halfedge()->face();
    
  //Determine Ga and Gb
  vgl_point_3d<double> Gap = FF->genes(0)->pt();
  vgl_point_3d<double> Gbp = FF->genes(1)->pt();

  ridgebnd_curve_a.push_back (Gap);
  ridgebnd_curve_b.push_back (Gbp);
  dbsk3d_fs_edge* prevLink = FE;
  
  //Go through all linkElms of this A3Rib.    
  for (unsigned int j=1; j<SCurve->E_vec().size(); j++) {
    FE = (dbsk3d_fs_edge*) SCurve->E_vec(j);
    //There is only one adjacent shock-sheet-elm. 
    FF = (dbsk3d_fs_face*) FE->halfedge()->face();
    
    //Determine Ga and Gb using the travering direction and the sheet normal.
    vgl_point_3d<double> Ga = FF->genes(0)->pt();
    vgl_point_3d<double> Gb = FF->genes(1)->pt();

    vgl_point_3d<double> C = FF->compute_center_pt();
    vgl_vector_3d<double> CGa = Ga - C;
    vgl_vector_3d<double> CGb = Gb - C;

    dbsk3d_fs_vertex* s_FV = (dbsk3d_fs_vertex*) Es_sharing_V (prevLink, FE);
    vgl_vector_3d<double> shock_patch_N = compute_normal (C, FE, s_FV);

    if (dot_product (CGa, shock_patch_N) > 0) { //add Ga to ridgebnd_curve_a
      _add_to_ridgebnd (ridgebnd_curve_a, Ga);
      _add_to_ridgebnd (ridgebnd_curve_b, Gb);    
      Gap = Ga;
      Gbp = Gb;
    }
    else { //add Gb to ridgebnd_curve_a      
      assert (dot_product (CGb, shock_patch_N) >= 0);
      _add_to_ridgebnd (ridgebnd_curve_a, Gb);
      _add_to_ridgebnd (ridgebnd_curve_b, Ga);
      Gap = Gb;
      Gbp = Ga;
    }
    ///(Old not working): if (vgl_distance (Ga, Gap) < vgl_distance (Gb, Gap))

    prevLink = FE;
  }
}

// Go through each incident shock FF of this FE,
// return the one blongs to MS
dbsk3d_fs_face* get_F_of_E_MS (dbsk3d_fs_edge* FE, dbsk3d_ms_sheet* MS)
{  
  dbmsh3d_halfedge* HE = FE->halfedge();
  do {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
    if (MS->contain_F (FF->id()))
      return FF;
    HE = HE->pair();
  }
  while (HE != FE->halfedge() && HE != NULL);
  return NULL;
}

void get_curoff_patch_bnd_curves (dbsk3d_ms_curve* SCurve, dbsk3d_ms_sheet* MS,
                                  vcl_vector<vgl_point_3d<double> >& surfbnd_curve_a, 
                                  vcl_vector<vgl_point_3d<double> >& surfbnd_curve_b)
{
  assert (SCurve->c_type() == C_TYPE_AXIAL);
  surfbnd_curve_a.clear();
  surfbnd_curve_b.clear();

  dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) SCurve->E_vec(0);
  dbsk3d_fs_face* FF = (dbsk3d_fs_face*) FE->halfedge()->face();
    
  //Determine Ga and Gb
  vgl_point_3d<double> Gap = FF->genes(0)->pt();
  vgl_point_3d<double> Gbp = FF->genes(1)->pt();

  surfbnd_curve_a.push_back (Gap);
  surfbnd_curve_b.push_back (Gbp);
  dbsk3d_fs_edge* prevLink = FE;
  
  //Go through all linkElms of this A13Axial.    
  for (unsigned int j=1; j<SCurve->E_vec().size(); j++) {
    FE = (dbsk3d_fs_edge*) SCurve->E_vec(j);
    //Find the adjacent shock-sheet-elm that belongs to MS.
    FF = get_F_of_E_MS (FE, MS);
    assert (FF);
    
    //Determine Ga and Gb using the travering direction and the sheet normal.
    vgl_point_3d<double> Ga = FF->genes(0)->pt();
    vgl_point_3d<double> Gb = FF->genes(1)->pt();

    vgl_point_3d<double> C = FF->compute_center_pt();
    vgl_vector_3d<double> CGa = Ga - C;
    vgl_vector_3d<double> CGb = Gb - C;

    dbsk3d_fs_vertex* s_FV = (dbsk3d_fs_vertex*) Es_sharing_V (prevLink, FE);
    vgl_vector_3d<double> shock_patch_N = compute_normal (C, FE, s_FV);

    if (dot_product (CGa, shock_patch_N) > 0) { //add Ga to surfbnd_curve_a
      _add_to_ridgebnd (surfbnd_curve_a, Ga);
      _add_to_ridgebnd (surfbnd_curve_b, Gb);    
      Gap = Ga;
      Gbp = Gb;
    }
    else { //add Gb to surfbnd_curve_a      
      assert (dot_product (CGb, shock_patch_N) >= 0);
      _add_to_ridgebnd (surfbnd_curve_a, Gb);
      _add_to_ridgebnd (surfbnd_curve_b, Ga);
      Gap = Gb;
      Gbp = Ga;
    }
    ///(Old not working): if (vgl_distance (Ga, Gap) < vgl_distance (Gb, Gap))

    prevLink = FE;
  }
}

// ###########################################################################
//Ridge estimation

vgl_vector_3d<double> get_init_ridge_vector (const dbsk3d_fs_edge* A3RibElm,
                                             const dbsk3d_fs_face* FF,
                                             const vgl_point_3d<double>& C)
{
  vgl_vector_3d<double> ridge_V;
  return ridge_V;
}

vgl_vector_3d<double> get_ridge_vector (const dbsk3d_fs_edge* A3RibElm,
                                        const dbsk3d_fs_face* FF,
                                        const vgl_point_3d<double>& C,
                                        const dbsk3d_fs_edge* prevRibElm)
{
  vgl_point_3d<double> Ga = FF->genes(0)->pt();
  vgl_point_3d<double> Gb = FF->genes(1)->pt();

  vgl_vector_3d<double> CGa = Ga - C;
  vgl_vector_3d<double> CGb = Gb - C;

  dbsk3d_fs_vertex* s_FV = (dbsk3d_fs_vertex*) Es_sharing_V (prevRibElm, A3RibElm);
  vgl_vector_3d<double> shock_patch_N = compute_normal (C, A3RibElm, s_FV);

  //Determine fan_N using the travering direction and the sheet_normal.
  vgl_vector_3d<double> fan_N;
  if (dot_product (CGa, shock_patch_N) > 0) { //Ga is on ridgebnd_curve_a
    fan_N = cross_product (CGa, CGb);
  }
  else { //Gb is on ridgebnd_curve_a
    assert (dot_product (CGb, shock_patch_N) >= 0);
    fan_N = cross_product (CGb, CGa);
  }

  vgl_vector_3d<double> ridge_V = cross_product (fan_N, shock_patch_N);

  return ridge_V;
}

bool save_ms_bnd_link_g3d (dbmsh3d_graph* ms_graph, dbmsh3d_mesh* bndset, vcl_string filename)
{
  vul_printf (vcl_cout, "\nSaving the shock-boundary trace curves into .g3d.\n");

  float rib_col[3] = {0, 0, 1};
  float axial_col[3] = {1, 0, 0};

  FILE* fp;
  if ((fp = vcl_fopen(filename.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "Can't open input G3d file %s\n", filename.c_str());
    return false; 
  }
  vcl_fprintf (fp, "%d\n", 3);

  vcl_vector <vgl_point_3d<double> > rib_pts, axial_pts;
  get_A3Rib_bnd_trace_points (ms_graph, &rib_pts); 
  get_A13Axial_bnd_trace_points (ms_graph, &axial_pts); 

  vcl_fprintf (fp, "%u\n", rib_pts.size() + axial_pts.size());

  //Rib
  for (unsigned int i=0; i<rib_pts.size(); i++) {
    vgl_point_3d<double> p = rib_pts[i];
    vcl_fprintf (fp, "%.16f %.16f %.16f %f %f %f\n", 
                 p.x(), p.y(), p.z(), 
                 rib_col[0], rib_col[1], rib_col[2]);
  }
  //Axial
  for (unsigned int i=0; i<axial_pts.size(); i++) {
    vgl_point_3d<double> p = axial_pts[i];
    vcl_fprintf (fp, "%.16f %.16f %.16f %f %f %f\n", 
                 p.x(), p.y(), p.z(), 
                 axial_col[0], axial_col[1], axial_col[2]);
  }

  fclose (fp);
  return true;
}

void get_A3Rib_bnd_trace_points (dbmsh3d_graph* ms_graph, 
                                 vcl_vector <vgl_point_3d<double> >* pts)
{
  //Get all boundary points of A3 Ribs.
  vcl_map<int, dbmsh3d_edge*>::iterator SC_it = ms_graph->edgemap().begin();
  for (; SC_it != ms_graph->edgemap().end(); SC_it++) {
    dbsk3d_ms_curve* SCurve = (dbsk3d_ms_curve*) (*SC_it).second;
    if (SCurve->c_type() != C_TYPE_RIB) 
      continue;
    
    //Go through all V_vec of this A3Rib.
    for (unsigned int j=0; j<SCurve->V_vec().size(); j++) {
      dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) SCurve->V_vec(j);
      vcl_set<dbmsh3d_vertex*> Genes = FV->get_Gs_from_FFs ();

      //draw line from this FV to each gene.
      vcl_set<dbmsh3d_vertex*>::iterator git = Genes.begin();
      while (git != Genes.end()) {
        const dbmsh3d_vertex* g = (*git);
        pts->push_back (g->pt());

        git++;
      }
    }//end for j
  }//end for i
}

void get_A13Axial_bnd_trace_points (dbmsh3d_graph* ms_graph, 
                                    vcl_vector <vgl_point_3d<double> >* pts)
{
  //Get all boundary points of A13 Axials.
  vcl_map<int, dbmsh3d_edge*>::iterator SC_it = ms_graph->edgemap().begin();
  for (; SC_it != ms_graph->edgemap().end(); SC_it++) {
    dbsk3d_ms_curve* SCurve = (dbsk3d_ms_curve*) (*SC_it).second;
    if (SCurve->c_type() != C_TYPE_AXIAL) 
      continue;

    //Go through all V_vec of this A13Axial
    for (unsigned int j=0; j<SCurve->V_vec().size(); j++) {
      dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) SCurve->V_vec(j);
      vcl_set<dbmsh3d_vertex*> Genes = FV->get_Gs_from_FFs ();

      //draw line from this FV to each gene.
      vcl_set<dbmsh3d_vertex*>::iterator git = Genes.begin();
      while (git != Genes.end()) {
        const dbmsh3d_vertex* g = (*git);
        pts->push_back (g->pt());

        git++;
      }
    }//end for j
  }//end for i
}
