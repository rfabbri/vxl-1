//: dbsk3d_fs_algo.cxx
//: MingChing Chang
//  May 05, 2005

#include <vcl_algorithm.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_vector_fixed.h>
#include <rsdl/rsdl_kd_tree.h>

#include <dbgl/dbgl_dist.h>
#include <dbgl/dbgl_barycentric.h>
#include <dbgl/algo/dbgl_curve_smoothing.h>
#include <dbnl/dbnl_min_max_3.h>

#include <dbmsh3d/algo/dbmsh3d_pt_mesh_dist.h>

#include <dbsk3d/algo/dbsk3d_fs_algo.h>
#include <dbsk3d/algo/dbsk3d_fs_xform.h>

void get_FF_min_max_time (dbsk3d_fs_face* FF, float& min_time, float& max_time)
{
  FF->get_min_max_V_time (min_time, max_time);
  
  //Also check if the foot point of generator is inside the sheet and use it as min_time.
  vgl_point_3d<double> G;
  float min_dist = (float) dbmsh3d_pt_face_dist_3d (FF->genes(0)->pt(), FF, G);

  if (min_dist < min_time)
    min_time = min_dist;
}

//: Go through each fs_face and compute flow type.
void compute_fs_patches_flow_type (dbsk3d_fs_mesh* fs_mesh)
{  
  vcl_map<int, dbmsh3d_face*>::iterator fit = fs_mesh->facemap().begin();
  for (; fit != fs_mesh->facemap().end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    FF->detect_flow_type();
  }

#if DBMSH3D_DEBUG > 1
  //Brute-forcely verify A12-2 sheets by definition.
  fit = fs_mesh->facemap().begin();
  for (; fit != fs_mesh->facemap().end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;

    bool result = brute_force_verify_A122_P (FF, fs_mesh->bnd_mesh());
    if (FF->flow_type() == FF_FT_I_A12_2)      
      assert (result);
    else
      assert (result == false);
  }  
#endif
}

bool brute_force_verify_A122_P (dbsk3d_fs_face* FF, dbmsh3d_mesh* M)
{
  double midpt_time = FF->mid_pt_time();
  vgl_point_3d<double> midpt = FF->mid_pt();

  //Go through each generator of fs_mesh.
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = M->vertexmap().begin();
  for (; vit != M->vertexmap().end(); vit++) {
    dbmsh3d_vertex* G = (*vit).second;
    double d = vgl_distance (G->pt(), midpt);
    if (d < midpt_time)
      return false;
  }
  return true;
}

void compute_fs_links_flow_type (dbsk3d_fs_mesh* fs_mesh)
{
  vcl_map<int, dbmsh3d_edge*>::iterator eit = fs_mesh->edgemap().begin();
  for (; eit != fs_mesh->edgemap().end(); eit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*eit).second;
    FE->detect_flow_type();
  }
}

void compute_fs_nodes_flow_type (dbsk3d_fs_mesh* fs_mesh)
{
  vul_printf (vcl_cout, "\ncompute_fs_nodes_flow_type(): %d fs_vertices.\n", 
              fs_mesh->vertexmap().size());

  compute_fs_patches_flow_type (fs_mesh);
  compute_fs_links_flow_type (fs_mesh);

  vcl_map<int, dbmsh3d_vertex*>::iterator vit = fs_mesh->vertexmap().begin();
  for (; vit != fs_mesh->vertexmap().end(); vit++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) (*vit).second;
    FV->detect_flow_type();
  }
  vul_printf (vcl_cout, "\tdone.\n");
}

//######################################################################
//  Boundary mesh labeling 
//######################################################################

//Label the A12-2 shock sheets with mesh edge as visited.
void label_edge_A12_2_sheets (dbsk3d_fs_mesh* fs_mesh)
{
  vul_printf (vcl_cout, "label_edge_A12_2_sheets(): %u sheets.\n",
               fs_mesh->facemap().size()); 

  vcl_map<int, dbmsh3d_face*>::iterator fit = fs_mesh->facemap().begin();
  for (; fit != fs_mesh->facemap().end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;

    if (E_sharing_2V (FF->genes(0), FF->genes(1)))
      FF->set_visited (true);
  }
}

//Label the A13-2 shock links with mesh face as visited.
void label_face_A13_links (dbsk3d_fs_mesh* fs_mesh)
{
  vul_printf (vcl_cout, "label_face_A13_links(): %u links.\n",
               fs_mesh->edgemap().size());

  vcl_map<int, dbmsh3d_edge*>::iterator eit = fs_mesh->edgemap().begin();
  for (; eit != fs_mesh->edgemap().end(); eit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*eit).second;
    vcl_vector<dbmsh3d_vertex*> genes;
    bool result = FE->get_ordered_Gs_via_FF (genes);
    assert (result);

    if (find_F_sharing_Vs (genes))
      FE->set_visited (true);
  }
}

//######################################################################
//  Output shock elements to files
//######################################################################

//: Output A12-2 shock sheets according to option:
//    - option -1: all shock sheets.
//    - option 1: visited shock sheets (with a valid A12-2 source).
//    - option 2: unvisited shock sheets.
void output_A12_2_file (dbsk3d_fs_mesh* fs_mesh, const int option)
{
  vul_printf (vcl_cout, "output_A12_2_file(): totally %u sheets.\n", fs_mesh->facemap().size());
  vcl_string filename;
  if (option == 1)
    filename = "a12time_e.txt";
  else
    filename = "a12time_ne.txt";  

  FILE  *fp;
  if ((fp = fopen(filename.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open output file %s.\n", filename.c_str());
    return; 
  }

  unsigned int count = 0;
  vcl_map<int, dbmsh3d_face*>::iterator fit = fs_mesh->facemap().begin();
  for (; fit != fs_mesh->facemap().end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;

    if (option==1 && FF->b_visited()==false)
      continue; //Skip the non-visited shock sheets.
    else if (option==2 && FF->b_visited())
      continue;

    double time = FF->mid_pt_time();
    vcl_fprintf (fp, "%.16f\n", time);
    count++;
  }

  vul_printf (vcl_cout, "\t %u sheets output to %s.\n", count, filename.c_str());
  fclose (fp);  
}

//: Output A13-2 shock links:
//  A13-2 links with mesh faces: (correct) a13data_f.txt
//  A13-2 links without mesh faces: (false-alarm) a13data_nf.txt
//  non-A13-2 links with mesh faces: (miss-detection) a13data_2f.txt.
//  non-A13-2 links without mesh faces: (wrong) a13data_2.txt. 

void output_A13_file (dbsk3d_fs_mesh* fs_mesh)
{  
  vul_printf (vcl_cout, "output_A13_file(): totally %u links.\n", fs_mesh->edgemap().size());

  FILE *fp_f, *fp_nf, *fp_2f, *fp_2;
  if ((fp_f = fopen ("a13data_f.txt", "w")) == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open output file a13data_f.txt.\n");
    return; 
  }
  if ((fp_nf = fopen ("a13data_nf.txt", "w")) == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open output file a13data_nf.txt.\n");
    return; 
  }
  if ((fp_2f = fopen ("a13data_2f.txt", "w")) == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open output file a13data_2f.txt.\n");
    return; 
  }
  if ((fp_2 = fopen ("a13data_2.txt", "w")) == NULL) {
    vul_printf (vcl_cout, "ERROR: Can't open output file a13data_2.txt.\n");
    return; 
  }

  unsigned int inf = 0;
  unsigned int a132_face = 0;
  unsigned int a132_noface = 0;
  unsigned int non_a13_2_face = 0;
  unsigned int non_a13_2_noface = 0;
  vcl_map<int, dbmsh3d_edge*>::iterator eit = fs_mesh->edgemap().begin();
  for (; eit != fs_mesh->edgemap().end(); eit++) {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) (*eit).second;
    if (FE->b_inf()) {
      inf++;
      continue; //Skip inf shocks
    }

    //Compute the surface triangle parameters:
    double max_side, perimeter;
    unsigned int nG;
    double Side[3];
    dbmsh3d_vertex* Gene[3];
    bool result = FE->get_tri_params (max_side, perimeter, nG, Side, Gene);
    assert (result);
    
    vgl_point_3d<double> C = circum_center_3pts (Gene[0]->pt(), Gene[1]->pt(), Gene[2]->pt());
    const double SC = vgl_distance (FE->s_FV()->pt(), C);
    const double EC = vgl_distance (FE->e_FV()->pt(), C);
    const double SE = vgl_distance (FE->s_FV()->pt(), FE->e_FV()->pt());

    double size = vgl_distance (Gene[0]->pt(), C);
    double Angle[3];
    compute_tri_angles (Gene[0]->pt(), Gene[1]->pt(), Gene[2]->pt(), Angle);
    double max_angle = vcl_max (vcl_max (Angle[0], Angle[1]), Angle[2]);    
    double min_angle = vcl_min (vcl_min (Angle[0], Angle[1]), Angle[2]);

    double compactness = FE->compute_tri_compactness (nG, Side);

    double alpha, beta, gamma;
    get_barycentric (Gene[0]->pt(), Gene[1]->pt(), Gene[2]->pt(),
                     alpha, beta, gamma);

    double min_bary = dbnl_min3 (alpha, beta, gamma);
    double max_bary = dbnl_max3 (alpha, beta, gamma);

    if (dbgl_leq_m (SC, SE) == false || dbgl_leq_m (EC, SE) == false) { //Non-A13-2 links:
      if (FE->b_visited()) { //with mesh face:
        vcl_fprintf (fp_2f, "%.16f %.16f %.16f %.16f %.16f %.16f\n", 
                     size, compactness, max_angle, min_angle, max_bary, min_bary);
        non_a13_2_face++;
      }
      else {
        vcl_fprintf (fp_2, "%.16f %.16f %.16f %.16f %.16f %.16f\n", 
                     size, compactness, max_angle, min_angle, max_bary, min_bary);
        non_a13_2_noface++;
      }
    }
    else { //A13-2 shock links:
      if (FE->b_visited()) { //with mesh face:
        //(size, compactness, max_angle, min_angle)
        vcl_fprintf (fp_f, "%.16f %.16f %.16f %.16f %.16f %.16f\n", 
                     size, compactness, max_angle, min_angle, max_bary, min_bary);
        a132_face++;
      }
      else {
        vcl_fprintf (fp_nf, "%.16f %.16f %.16f %.16f %.16f %.16f\n", 
                     size, compactness, max_angle, min_angle, max_bary, min_bary);
        a132_noface++;
      }
    }
  }

  vul_printf (vcl_cout, "\t %u fs_edges at infinity.\n", inf);
  vul_printf (vcl_cout, "\t %u A13-2 fs_edges with mesh face output to a13data_f.txt.\n", a132_face);
  vul_printf (vcl_cout, "\t %u A13-2 fs_edges without mesh face output to a13data_nf.txt.\n", a132_noface);
  vul_printf (vcl_cout, "\t %u non-A13-2 fs_edges with mesh face output to a13data_2f.txt.\n", non_a13_2_face);
  vul_printf (vcl_cout, "\t %u non-A13-2 fs_edges without mesh face output to a13data_2.txt.\n", non_a13_2_noface);
  fclose (fp_f); 
  fclose (fp_nf); 
  fclose (fp_2f); 
  fclose (fp_2); 
}

//######################################################################
//  Surface Pruning Functions
//######################################################################

void shock_pruning_bnd_FF_compactness (dbsk3d_fs_sheet_set* fs_ss, const int iter, const float c_th)
{
  vul_printf (vcl_cout, "\nshock_pruning_bnd_FF_compactness(): iter = %d, c_th = %f.\n", iter, c_th);
  if (fs_ss->sheetmap().size() == 0)
    return;
  vcl_vector<dbsk3d_fs_face*> P_to_prune;
  int n_P_pruned = 0;

  for (int i=0; i<iter; i++) {
    //Go through each shock sheet, put fs_faces to be pruned into a list.
    vcl_map<int, dbsk3d_fs_sheet*>::iterator it = fs_ss->sheetmap().begin();
    for (; it != fs_ss->sheetmap().end(); it++) {
      dbsk3d_fs_sheet* FS = (*it).second;

      dbmsh3d_ptr_node* prev = NULL;
      dbmsh3d_ptr_node* cur = FS->FF_list(); 
      while (cur != NULL) {
        dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();

        //Prune very elongated FF's on boundary, which should very possibly be removed.
        bool b_elongated = FF->is_elongate (c_th);
        bool b_on_bnd = FF->is_on_shock_bnd ();
        if (b_elongated==false || b_on_bnd==false) {
          //Skip if all FF is not elongated or not on the boundary rib.
          prev = cur;
          cur = cur->next(); 
          continue;      
        }
        //Prune fs_face: Remove FF from its sheet component. 
        if (prev == NULL) { //cur is list head.
          dbmsh3d_ptr_node* tmp = cur;
          cur = cur->next();
          FS->set_FF_list (cur);
          delete tmp;
        }
        else { //cur is in middle of list.
          dbmsh3d_ptr_node* tmp = cur;
          cur = cur->next();
          prev->set_next (cur);
          delete tmp;
        }

        P_to_prune.push_back (FF);
      }

      if (FS->type() == FS_TYPE_UNBOUNDED)
        FS->set_type (FS_TYPE_TAB);
    }

    vul_printf (vcl_cout, "\t  iter %d: %u fs_faces pruned.\n", i, P_to_prune.size());

    //Go through P_to_prune and perform pruning.
    for (unsigned int j=0; j<P_to_prune.size(); j++) {
      dbsk3d_fs_face* FF = P_to_prune[j];      
      FF_prune_pass_Gs (FF); //Assign generators to remaining shocks.
      n_P_pruned++;
    }
    P_to_prune.clear();

  } //end iter i.
  
  //Remove the empty sheets (possible after the bounding-box pruning).
  unsigned int n_empty_sheets = fs_ss->remove_empty_sheets ();

  vul_printf (vcl_cout, "\tOut of a total of %u fs_faces:\n", fs_ss->fs_mesh()->facemap().size());
  vul_printf (vcl_cout, "\t%u boundary elongated fs_faces removed.\n", n_P_pruned);
  vul_printf (vcl_cout, "\t%u empty sheet components are removed.\n", n_empty_sheets);
}

//A Temporary solution to prune outside shocks.
//In theory we shouldn't do this because it prunes out some valid outside shocks, too.
//
void shock_pruning_box (dbsk3d_fs_sheet_set* fs_ss, const float box_ratio)
{
  vul_printf (vcl_cout, "\nshock_pruning_box(): prune_box_ratio = %.2f.\n", box_ratio);
  if (fs_ss->sheetmap().size() == 0)
    return;
  int n_P_out = 0;

  vgl_box_3d<double> box;
  detect_bounding_box (fs_ss->fs_mesh()->bnd_mesh(), box);
  vgl_box_3d<double> pruning_box = dbgl_extend_box_by_factor (box, box_ratio);

  //Go through each shock sheet, prune patch-elms outside the extended bounding box
  unsigned int count_elongated_bnd = 0;
  vcl_map<int, dbsk3d_fs_sheet*>::iterator it = fs_ss->sheetmap().begin();
  for (; it != fs_ss->sheetmap().end(); it++) {
    dbsk3d_fs_sheet* FS = (*it).second;

    dbmsh3d_ptr_node* prev = NULL;
    dbmsh3d_ptr_node* cur = FS->FF_list(); 
    while (cur != NULL) {
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) cur->ptr();
      if (FF->is_inside_box(pruning_box)) {        
        //Skip of all FV's of this FF are inside bounding box.
        prev = cur;
        cur = cur->next(); 
        continue;      
      }
      
      //Prune shock-patch-elm FF: Remove FF from its sheet component. 
      if (prev == NULL) { //cur is list head.
        dbmsh3d_ptr_node* tmp = cur;
        cur = cur->next();
        FS->set_FF_list (cur);
        delete tmp;
      }
      else { //cur is in middle of list.
        dbmsh3d_ptr_node* tmp = cur;
        cur = cur->next();
        prev->set_next (cur);
        delete tmp;
      }

      //Assign generators to remaining shocks.
      FF_prune_pass_Gs (FF);
      n_P_out++;
    }

    if (FS->type() == FS_TYPE_UNBOUNDED)
      FS->set_type (FS_TYPE_TAB);
  }
  
  //Remove the empty sheets (possible after the bounding-box pruning).
  unsigned int n_empty_sheets = fs_ss->remove_empty_sheets ();

  vul_printf (vcl_cout, "\tOut of a total of %u sheet_elms:\n", fs_ss->fs_mesh()->facemap().size());
  vul_printf (vcl_cout, "\t%6d are out of input bnd_mesh's extended bounding box,\n", n_P_out);
  vul_printf (vcl_cout, "\t%u empty sheets are removed.\n", n_empty_sheets);
}

//#############################################################################

void rmin_trim_a122 (dbsk3d_fs_sheet_set* fs_ss, const float rmin_th)
{
  vul_printf (vcl_cout, "rmin_trim_a122(): rmin_th = %f, %u sheets (%u elements).\n", 
              rmin_th, fs_ss->sheetmap().size(), fs_ss->fs_mesh()->facemap().size());

}

//#############################################################################

//: Sorts in ASCENDING order.
// Return whether first element is greater than the second
bool _FF_lesser (dbsk3d_fs_face* P1, dbsk3d_fs_face* P2)
{
   return P1->max_time() < P2->max_time();
}

//: Trim all fs_faces that are too close to the boundary generators.
//    - Remove all fs_faces whose A12-2 points are too close.
//
//    - Remove all fs_faces sharing FV's that are too close.
//      Perform the fs_face pruning in sequence of FF's max_time.
//
void rmin_trim_xforms (dbsk3d_fs_sheet_set* fs_ss, const float rmin_th)
{  
  vul_printf (vcl_cout, "rmin_trim_xforms(): rmin_th = %f, %u sheets (%u elements).\n\t", 
              rmin_th, fs_ss->sheetmap().size(), fs_ss->fs_mesh()->facemap().size());

  //Store all valid shock-sheet-elms in a map sorted by min_time.
  vcl_multimap<float, dbsk3d_fs_face*> FF_mmap;

  vcl_map<int, dbmsh3d_face*>::iterator fit = fs_ss->fs_mesh()->facemap().begin();
  for (; fit != fs_ss->fs_mesh()->facemap().end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    if (FF->b_valid() == false)
      continue;

    float min_time, max_time;
    ///FF->get_min_max_V_time (min_time, max_time);
    get_FF_min_max_time (FF, min_time, max_time);
    FF_mmap.insert (vcl_pair<float, dbsk3d_fs_face*> (min_time, FF));
  }

  //Perform trim xforms.
  perform_rmin_trim_xforms (FF_mmap, rmin_th);
  vul_printf (vcl_cout, "\n");
  FF_mmap.clear();
} 

void perform_rmin_trim_xforms (vcl_multimap<float, dbsk3d_fs_face*>& FF_mmap,
                 const float rmin_th)
{
  //Perform the shock pruning in sequence of max_time.
  //Prune the FF if its min_time < rmin_th.
  int n_valid_P = 0;
  int n_trimmed_P = 0;
  vcl_multimap<float, dbsk3d_fs_face*>::iterator pit = FF_mmap.begin();
  for (; pit != FF_mmap.end(); pit++) {
    dbsk3d_fs_face* FF = (*pit).second;
    assert (FF->b_valid());
    float min_time, max_time;
    get_FF_min_max_time (FF, min_time, max_time);

    //-rmin: if the Sheet's min_time is too close to generators (surface), prune it.
    if (min_time < rmin_th) {
      //Add FF's associated generators to remaining adjacent A3_rib_elms.
      FF_prune_pass_Gs (FF);
      n_trimmed_P++;
      continue; 
    }

    //The remaining ones are valid
    n_valid_P++;
  }

  vul_printf (vcl_cout, "%d trimmed, %d remains. ", n_trimmed_P, n_valid_P); 
}


//: Trim all fs_faces that are too close to the boundary generators.
//    - Remove all fs_faces whose A12-2 points are too far.
//
//    - Remove all fs_faces sharing FV's that are too far.
//      Perform the fs_face pruning in sequence of FF's max_time.
//
void rmax_trim_xforms (dbsk3d_fs_sheet_set* fs_ss, const float rmax_th)
{  
  vul_printf (vcl_cout, "rmax_trim_xforms(): rmax_th = %f, %u sheets (%u elements).\n\t", 
              rmax_th, fs_ss->sheetmap().size(), fs_ss->fs_mesh()->facemap().size());

  //Store all valid shock-sheet-elms in a map sorted by -max_time.
  vcl_multimap<float, dbsk3d_fs_face*> FF_mmap;

  vcl_map<int, dbmsh3d_face*>::iterator fit = fs_ss->fs_mesh()->facemap().begin();
  for (; fit != fs_ss->fs_mesh()->facemap().end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    if (FF->b_valid() == false)
      continue;

    float min_time, max_time;
    get_FF_min_max_time (FF, min_time, max_time);
    FF_mmap.insert (vcl_pair<float, dbsk3d_fs_face*> (-max_time, FF));
  }

  //Perform trim xforms.
  perform_rmax_trim_xforms (FF_mmap, rmax_th);
  vul_printf (vcl_cout, "\n");
  FF_mmap.clear();
} 


void perform_rmax_trim_xforms (vcl_multimap<float, dbsk3d_fs_face*>& FF_mmap,
                               const float rmax_th)
{
  //Perform the shock pruning in sequence of -max_time.
  //Prune the FF if its -max_time < -rmax_th.
  int n_valid_P = 0;
  int n_trimmed_P = 0;
  vcl_multimap<float, dbsk3d_fs_face*>::iterator pit = FF_mmap.begin();
  for (; pit != FF_mmap.end(); pit++) {
    dbsk3d_fs_face* FF = (*pit).second;
    assert (FF->b_valid());
    float min_time, max_time;
    get_FF_min_max_time (FF, min_time, max_time);

    //-rmin: if the Sheet's min_time is too close to generators (surface), prune it.
    if (-max_time < -rmax_th) {
      //Add FF's associated generators to remaining adjacent A3_rib_elms.
      FF_prune_pass_Gs (FF);
      n_trimmed_P++;
      continue; 
    }

    //The remaining ones are valid
    n_valid_P++;
  }

  vul_printf (vcl_cout, "%d trimmed, %d remains. ", n_trimmed_P, n_valid_P); 
}

//: Remove the shock of generator near boundary holes.
//  Skip removing the 'completely interior' fs_faces.
void prune_shocks_of_bnd_holes (dbmsh3d_mesh* bnd_mesh, dbsk3d_fs_mesh* fs_mesh)
{
  vul_printf (vcl_cout, "prune_shocks_of_bnd_holes().\n");
  unsigned int n_pruned_bnd_holes = 0;

  //Go through each shock patch element and draw it if it's valid and 
  //any of its G[2] is not on the 1-ring-topology, i.e., on mesh boundary.
  vcl_map<int, dbmsh3d_face*>::iterator fit = fs_mesh->facemap().begin();
  for (; fit != fs_mesh->facemap().end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    if (FF->b_valid() == false)
      continue; //skip the invalid sheet elements.

    //Check if the two generators of FF is of 1-ring topology or not.
    bool G0_not_1ring = FF->genes(0)->detect_vtopo_type() != VTOPO_2_MANIFOLD_1RING;
    bool G1_not_1ring = FF->genes(1)->detect_vtopo_type() != VTOPO_2_MANIFOLD_1RING;

    //Check if FF is complete interior or not.
    bool FF_on_bnd = FF->is_on_shock_bnd();
    
    if ((G0_not_1ring || G1_not_1ring) && FF_on_bnd) { // ||
      FF_prune_pass_Gs (FF); //Prune FF
      n_pruned_bnd_holes++;
    }
  }

  vul_printf (vcl_cout, "\t%d fs_faces pruned.\n\n", n_pruned_bnd_holes);
}

//#########################################################################

//: Re-assign 'lost' generators via finding the closest valid fs_vertex.
bool reasgn_lost_Gs_closest_FV (dbsk3d_fs_mesh* fs_mesh, vcl_vector<dbmsh3d_vertex*>& unasgn_genes)
{
  vul_printf (vcl_cout, "reasgn_lost_Gs_closest_FV(): %u lost genes.\n", unasgn_genes.size());

  //1) Put all valid shock nodes into a kd-tree.
  vul_printf (vcl_cout, "  putting all valid fs_vertices of %u fs_faces to a kd-tree.\n", 
              fs_mesh->facemap().size());

  //Reset all vertices' i_value.
  fs_mesh->reset_vertices_i_value (0);

  //Loop through all FF's and mark valid FV's  
  vcl_map<int, dbmsh3d_face*>::iterator fit = fs_mesh->facemap().begin();
  for (; fit != fs_mesh->facemap().end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    if (FF->b_valid() == false)
      continue;

    //Mark all FV's of FF as valid.    
    dbmsh3d_halfedge* HE = FF->halfedge();
    do {
      dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();
      //Mark both FE's s_FV and e_FV as valid (set i_value to 1).
      FE->s_FV()->set_valid (true);
      FE->e_FV()->set_valid (true);
      HE = HE->next();
    }
    while (HE != FF->halfedge());
  }

  //Loop through all N's and add valid ones to kd-tree. 
  unsigned int total = 0;
  vcl_map<int, dbmsh3d_vertex*>::iterator vit = fs_mesh->vertexmap().begin();
  for (; vit != fs_mesh->vertexmap().end(); vit++) {
    dbsk3d_fs_vertex* N = (dbsk3d_fs_vertex*) (*vit).second;
    if (N->is_valid() == false)
      continue;
    total++;
  }  
  //Store all the points in the kd-tree
  vcl_vector<rsdl_point> search_pts (total);
  const unsigned int nc = 3, na = 0;
  vit = fs_mesh->vertexmap().begin();
  for (unsigned int i=0; vit != fs_mesh->vertexmap().end(); vit++) {
    dbsk3d_fs_vertex* N = (dbsk3d_fs_vertex*) (*vit).second;
    if (N->is_valid() == false)
      continue;

    vnl_vector_fixed<double,3> FF (N->pt().x(), N->pt().y(), N->pt().z());
    search_pts[i].resize (nc, na);
    search_pts[i].set_cartesian (FF);
    i++;
  }

  //Unlikely but possible in the case that no valid fs_vertex remains.
  if (search_pts.size() == 0)
    return false; 

  rsdl_kd_tree* kd_tree = new rsdl_kd_tree (search_pts);
  vul_printf (vcl_cout, "  rsdl_kd_tree size: %u (valid shock nodes, out of total %u).\n", 
              total, fs_mesh->vertexmap().size());

  //Loop through the unasgn_genes and assign to closest fs_vertex.
  for (unsigned int i=0; i<unasgn_genes.size(); i++) {
    dbmsh3d_vertex* G = unasgn_genes[i];

    //Find the k closest points.
    int k = 1;
    vcl_vector<rsdl_point> near_neighbor_pts;
    vcl_vector<int> near_neighbor_indices;
    rsdl_point query_pt (3, 0);
    vnl_vector_fixed<double,3> P3 (G->pt().x(), G->pt().y(), G->pt().z());
    query_pt.set_cartesian (P3);
    kd_tree->n_nearest (query_pt, k, near_neighbor_pts, near_neighbor_indices);
    assert (near_neighbor_indices.size() == k);

    //Search in vertexmap for the j-th valid vertex, j = near_neighbor_indices[0].
    dbsk3d_fs_vertex* closestN = NULL;
    vit = fs_mesh->vertexmap().begin();    
    for (int j=0; vit != fs_mesh->vertexmap().end(); vit++) {
      dbsk3d_fs_vertex* N = (dbsk3d_fs_vertex*) (*vit).second;
      if (N->is_valid() == false)
        continue;
      if (j==near_neighbor_indices[0]) {
        closestN = N;
        break;
      }
      j++;
    }    

    //Assign G to closestN
    assert (closestN->is_valid_via_FF());
    closestN->add_asgn_G (G);

    //Only output 10 lines of command-line message.
    if (unasgn_genes.size() < 10)
      vul_printf (vcl_cout, "G %d assigned to closest N %d.\n", G->id(), closestN->id());
  } 
  return true;
}

//Recovery the association of 'lost' generators {G}.
//Search for nearby A3-shock-curve-elms that are closest to each G and associate with it.
//   1. Start with immediate (pruned) fs_faces pointing to G (available in the full medial scaffold).
//   2. Iteratively breadth-first-search (BFS) for adjacent shocks sheets until their valid shock curves are found.
//   3. Stop until # of candidate valid curves (fs_edges) >= th (5, or 10).
//   4. Choose the fs_edge closest to G for association.
//
bool reasgn_lost_Gs_via_FF (dbsk3d_fs_mesh* fs_mesh, vcl_vector<dbmsh3d_vertex*>& unasgn_genes)
{
  vul_printf (vcl_cout, "reasgn_lost_Gs_via_FF(): %d lost genes.\n", unasgn_genes.size());

  //Initialize a set unasgn_genes_set from the vector of unasgn_genes[] for fast search.
  vcl_set<dbmsh3d_vertex*> unasgn_genes_set;
  for (unsigned int i=0; i<unasgn_genes.size(); i++)
    unasgn_genes_set.insert (unasgn_genes[i]);
  assert (unasgn_genes.size() == unasgn_genes_set.size()); //no duplication.

  //For each unasso_gene G, initialize a set of fs_faces pointing to G.
  vcl_vector<vcl_vector<dbsk3d_fs_face*> > G_asgn_patches;
  G_asgn_patches.resize (unasgn_genes.size());

  //Loop through all fs_faces to search for the ones pointing to unasgn_genes.
  //Can use bucketing here to speed up. e.g. find the closest N shock nodes
  vcl_map<int, dbmsh3d_face*>::iterator fit = fs_mesh->facemap().begin();
  for (; fit != fs_mesh->facemap().end(); fit++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) (*fit).second;
    unsigned int idx;

    //Check if FF->genes(0) is in unasgn_genes.
    if (unasgn_genes_set.find (FF->genes(0)) != unasgn_genes_set.end()) {
      //Find the idx of FF->genes(0) in unasgn_genes[]
      for (idx = 0; idx < unasgn_genes.size(); idx++)
        if (unasgn_genes[idx] == FF->genes(0))
          break;

      //Put FF into G_asgn_patches[idx]'s set of fs_faces.
      G_asgn_patches[idx].push_back (FF);
    }

    //Check if FF->genes(1) is in unasgn_genes.
    if (unasgn_genes_set.find (FF->genes(1)) != unasgn_genes_set.end()) {
      //Find the idx of FF->genes(0) in unasgn_genes[]
      for (idx = 0; idx < unasgn_genes.size(); idx++)
        if (unasgn_genes[idx] == FF->genes(1))
          break;

      //Put FF into G_asgn_patches[idx]'s set of fs_faces.
      G_asgn_patches[idx].push_back (FF);
    }
  }

  //Reset the mesh face traverse flag.
  fs_mesh->reset_traverse_f ();

  //Loop through all unasgn_genes again and recover the shock for each G.
  for (unsigned int i=0; i < unasgn_genes.size(); i++) {
    bool success = asgn_lost_gene (fs_mesh, unasgn_genes[i], G_asgn_patches[i]);
    assert (success);
  }
  vul_printf (vcl_cout, "\tDone.\n\n");  
  G_asgn_patches.clear();
  unasgn_genes.clear();
  return true;
}

//Recovery the association of the lost generator G, given the init. fs_faces.
//Search for nearby A3-shock-curve-elms that are closest to each G and associate with it.
//   1. Start with immediate (pruned) fs_faces pointing to G.
//   2. Iteratively breadth-first-search (BFS) for adjacent shocks sheets until their A3 shock curves are found.
//   3. Stop until # of candidate valid A3 curves (fs_edges) >= th (5, or 10).
//   4. Choose the fs_edge closest to G for association.

#define N_CAND_A3_LINKS 10

bool asgn_lost_gene (dbsk3d_fs_mesh* fs_mesh, const dbmsh3d_vertex* G,
                     vcl_vector<dbsk3d_fs_face*>& init_fs_faces)
{
  #if DBMSH3D_DEBUG > 3
  vul_printf (vcl_cout, "  asgn_lost_gene (): G %d (%d init_fs_faces), ", 
              G->id(), init_fs_faces.size());
  #endif

  //Queue of adjacent fs_faces for breadth-first-search (BFS).
  vcl_queue<dbsk3d_fs_face*> BFS_patch_queue;

  //Put all init_fs_faces into the queue.
  vcl_vector<dbsk3d_fs_face*>::iterator ipit = init_fs_faces.begin();
  for (; ipit != init_fs_faces.end(); ipit++)
    BFS_patch_queue.push (*ipit);

  //A set of cand_A3_links to store candidate valid A3 fs_edges.
  vcl_set<dbsk3d_fs_edge*> cand_A3_links;

  //Main loop of breadth-first searching for valid A3 fs_edges.
  while (BFS_patch_queue.size() > 0 && cand_A3_links.size() < N_CAND_A3_LINKS) {
    dbsk3d_fs_face* frontFF = BFS_patch_queue.front();
    BFS_patch_queue.pop();

    //Search frontFF and unvisited neighbors for candidate valid A3 fs_edges.
    prop_BFS_on_FF (fs_mesh, frontFF, BFS_patch_queue, cand_A3_links);
  }

  //Search the set of candidate A3 shock links (or nodes) closest to G.
  double min_dist = DBL_MAX;
  dbsk3d_fs_edge* closestL = NULL;
  vcl_set<dbsk3d_fs_edge*>::iterator lit = cand_A3_links.begin();
  for (; lit != cand_A3_links.end(); lit++) {
    double dist = vgl_distance ((*lit)->mid_pt(), G->pt());
    if (dist < min_dist) {
      min_dist = dist;
      closestL = (*lit);
    }
  }

  if (closestL) {
    //Assign G to closestL
    closestL->add_asgn_G (G);
    #if DBMSH3D_DEBUG > 3
    vul_printf (vcl_cout, "assigned to FE %d.\n", closestL->id());
    #endif
  }

  return true;
}

//Search frontFF and unvisited neighbors for candidate valid A3 fs_edges.
void prop_BFS_on_FF (dbsk3d_fs_mesh* fs_mesh, dbsk3d_fs_face* frontFF, 
                     vcl_queue<dbsk3d_fs_face*>& BFS_patch_queue, 
                     vcl_set<dbsk3d_fs_edge*>& cand_A3_links)
{
  if (frontFF->is_visited(fs_mesh->i_traverse_flag()))
    return;
  frontFF->set_i_visited (fs_mesh->i_traverse_flag());

  //Check each incident fs_edge if it's valid A3.
  //Put all unvisited adjacent fs_faces (via the fs_edges) of frontFF to queue.
  dbmsh3d_halfedge* HE = frontFF->halfedge();
  do {
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();

    //Add FE to cand_A3_links if FE's valid A3.
    unsigned int n = FE->count_valid_Fs();
    if (n == 1)
      cand_A3_links.insert (FE);

    //Add for all other unvisited FF's of FE to the BFS_patch_queue.
    dbmsh3d_halfedge* LHE = FE->halfedge();
    do {
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) LHE->face();
      if (FF->is_visited(fs_mesh->i_traverse_flag()) == false)
        BFS_patch_queue.push (FF);
      LHE = LHE->pair();
    }
    while (LHE != FE->halfedge() && LHE != NULL);

    HE = HE->next();
  }
  while (HE != frontFF->halfedge());

}

//###### Smooth Rib Curves ######

dbmsh3d_edge* trim_FF (dbsk3d_fs_face* FF, dbmsh3d_vertex* keepV, const dbmsh3d_edge* keepE, 
                       const dbmsh3d_vertex* trimV, const bool pass_gene,
                       vcl_vector<dbmsh3d_edge*>& E_to_del, vcl_vector<dbmsh3d_vertex*>& V_to_del)
{
  E_to_del.clear();
  V_to_del.clear();
  assert (keepE->is_V_incident (keepV));
  assert (keepE->is_V_incident (trimV) == false);

  //Make FF.he points to keepFE.
  bool r = FF->point_halfedge_to_E (keepE);
  assert (r);

  //Make the orientation of FF.he from keepV to keepE.
  dbmsh3d_edge* nE = FF->halfedge()->next()->edge();
  bool loop2;
  dbmsh3d_vertex* V = Es_sharing_V_check (keepE, nE, loop2);
  assert (loop2 == false);
  if (V == keepV)
    FF->_reverse_bnd_HEs_chain (); //reverse FF.he chain.

  //Now FF.he is in right order. Traverse FF.he until trimV is reached. 
  //Find prevHE and prepare the vector of candidate HE's to delete.
  dbmsh3d_halfedge* HE = FF->halfedge();
  dbmsh3d_halfedge* prevHE = NULL;
  vcl_vector<dbmsh3d_halfedge*> HE_to_del;
  do {
    if (prevHE)
      HE_to_del.push_back (HE);
    if (prevHE == NULL && HE->edge()->is_V_incident (trimV))
      prevHE = HE;
    HE = HE->next();
  }
  while (HE != FF->halfedge());
  
  assert (HE_to_del.size() > 0);

  //For the case of HE_to_del[] with only one edge, return this edge as lastE.
  if (HE_to_del.size() == 1) {
    HE_to_del[0]->edge()->set_e_type (E_TYPE_RIB);
    return HE_to_del[0]->edge();
  }

  //Pick the first HE in HE_to_del[] to keep.
  dbsk3d_fs_edge* FE_keep = (dbsk3d_fs_edge*) HE_to_del[0]->edge();

  //Go through other HE_to_del[].
  // 1) For each vertex, check to put in V_to_del[] and pass generator to FE_keep or not.
  // 2) For each edge, check the topology of each HE in HE_to_del[] and decide 
  //    whether to put in E_to_del[] and pass generator to FE_keep or not.
  for (unsigned int i=1; i<HE_to_del.size(); i++) {
    HE = HE_to_del[i];
    dbmsh3d_halfedge* pHE = HE_to_del[i-1];
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) HE->edge();    
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) Es_sharing_V (pHE->edge(), FE);
    assert (FV);

    //1) If both edges of the FV are on a rib, delete it.
    if (HE->pair() == NULL && pHE->pair() == NULL) {
      V_to_del.push_back (FV);

      //Pass generator from FV to FE_keep    
      for (dbmsh3d_ptr_node* cur = FV->asgn_G_list(); cur != NULL; cur = cur->next()) {
        dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
        FE_keep->check_add_asgn_G (G);
      }
      FV->clear_asgn_G_list ();
    }

    //2) If FE is on a rib, delete it.
    if (HE->pair() == NULL) { 
      E_to_del.push_back (FE);
      
      //Pass generator from FE to FE_keep    
      for (dbmsh3d_ptr_node* cur = FE->asgn_G_list(); cur != NULL; cur = cur->next()) {
        dbmsh3d_vertex* G = (dbmsh3d_vertex*) cur->ptr();
        FE_keep->check_add_asgn_G (G);
      }
      FE->clear_asgn_G_list ();
    }
  }
  
  //Disconnect and delete the HE in HE_to_del[1..n-1].
  for (unsigned int i=1; i<HE_to_del.size(); i++) {
    HE = HE_to_del[i];
    dbmsh3d_edge* E = HE->edge();  
    FF->_disconnect_bnd_HE (HE);
    E->_disconnect_HE (HE);
    E->compute_e_type ();
    delete HE;
  }

  //Close FF's next chain.
  HE_to_del[0]->set_next (FF->halfedge());
  //Disconnect FE_keep from other vertex of trimV.
  dbmsh3d_vertex* otV = FE_keep->other_V (trimV);
  int idx = FE_keep->_disconnect_V (otV);
  assert (idx != -1);
  //Set FE_keep to point to keepV.
  FE_keep->connect_V (idx, keepV);
  FE_keep->set_e_type (E_TYPE_RIB);

  return FE_keep;
}

bool FF_smooth_rib_curve (dbsk3d_fs_face* FF, const float psi, const int nsteps)
{
  //Put all consecutive rib elements into the bnd_E_chains.
  vcl_vector<vcl_vector<dbmsh3d_edge*> > bnd_E_chains;
  FF->get_bnd_E_chains (bnd_E_chains);
  assert (bnd_E_chains.size() > 0);

  //Go through the bnd_E_chains and identify each E_chain.
  for (unsigned int i=0; i<bnd_E_chains.size(); i++) {    
    if (bnd_E_chains[i].size() < 2)
      continue; //no need to smooth

    //Identify all end points to apply DCS.
    vcl_vector<vgl_point_3d<double> > curve;
    get_digi_curve_E_chain (bnd_E_chains[i], curve);

    //DCS smooth the curve.
    dbgl_curve_shorten_3d (curve, psi, nsteps);

    //Update the dbmsh3d_vertices in bndE_chains[i].
    update_digi_curve_E_chain (bnd_E_chains[i], curve);
  }

  return true;
}

