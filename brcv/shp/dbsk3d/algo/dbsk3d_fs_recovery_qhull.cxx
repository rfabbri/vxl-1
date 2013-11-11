//: This is 3DShock_FullShock_QHull_PostProcess.cxx
//  MingChing Chang
//  Nov 03, 2004   Creation
//  Nov 08, 2004   Add to FullShock

#include <vcl_algorithm.h>
#include <vcl_map.h>
#include <vcl_utility.h>
#include <vcl_iostream.h>
#include <vnl/vnl_math.h>
#include <vul/vul_printf.h>

#include <dbmsh3d/algo/dbmsh3d_fileio.h>

#include <dbsk3d/dbsk3d_fs_face.h>
#include <dbsk3d/dbsk3d_fs_mesh.h>
#include <dbsk3d/algo/dbsk3d_fs_algo.h>

//#################################################################

void add_bndsphere_genes (dbmsh3d_mesh* bndset, const float radius_ratio, const int nsample_ratio)
{
  vul_printf (vcl_cout, "\nadd_bndsphere_genes():\n");

  vgl_box_3d<double> bbox;
  detect_bounding_box (bndset, bbox);

  const vgl_point_3d<double> center = bbox.centroid();

  double radius = bbox.width() * radius_ratio;

  //: Add points to the North and South pole
  dbmsh3d_vertex* vertex = bndset->_new_vertex ();
  vertex->get_pt().set (center.x(), center.y(), center.z() + radius);
  bndset->_add_vertex (vertex);

  vertex = bndset->_new_vertex ();
  vertex->get_pt().set (center.x(), center.y(), center.z() - radius);
  bndset->_add_vertex (vertex);

  unsigned int seed = 10000;
  srand(seed);

  unsigned int CONST_POINTS = 10; ///10;
  unsigned int CONST_LAYERS = 8; ///8;

  switch (nsample_ratio) {
  case 1:
    CONST_POINTS = 10;
    CONST_LAYERS = 8;
  break;
  case 2:
    CONST_POINTS = 100;
    CONST_LAYERS = 80;
  break;
  default:
  break;
  }

  int nTotalPoints = 0;

  for (unsigned int i=1; i<CONST_LAYERS; i++) {
    double phi = vnl_math::pi * i/CONST_LAYERS;
    double dPoints = (double) sin(phi) * CONST_POINTS;
    int    nPoints = (int) ceil (dPoints);

    //: add nPoints (random first theta)
    double theta = (double)rand() / RAND_MAX;
    theta *= vnl_math::pi*2;
    for (int j=0; j<nPoints; j++) {
      theta += 2.0 * vnl_math::pi * j / nPoints;
      double x = radius * vcl_sin(phi) * vcl_cos(theta) + center.x();
      double y = radius * vcl_sin(phi) * vcl_sin(theta) + center.y();
      double z = radius * vcl_cos(phi) + center.z();

      vertex = bndset->_new_vertex ();
      vertex->get_pt().set (x, y, z);
      bndset->_add_vertex (vertex);
    }

    nTotalPoints += nPoints;
  }

  vul_printf (vcl_cout, "\tTotally, %d point genes added.\n", nTotalPoints);
}


//: Note of Clique
//  clique of a graph is its maximal complete subgraph (Harary 1994, p. 20), 
//  although some authors define a clique as any complete subgraph and then refer to 
//  "maximum cliques" (Skiena 1990, p. 217). 
//  The problem of finding the size of a clique for a given graph is 
//   an NP-complete problem (Skiena 1997). 

//: Note of co-simplicial
//  A vertex v in a graph G is called simplicial if the neighborhood N(v) in G induces a clique; 
//  v is called co-simplicial if it is simplicial in the complement G of G.
//  Clearly, a vertex is simplicial if and only if it is not the midpoint of any P3. 
//  A vertexv in G is then called semi-simplicial if v is not a midpoint of any P4in G. 
//  A graph G is called brittle if, for each induced subgraph F of G, F or F has a semi-simplicial vertex.
//  In [11] it is proved that every HHD-free graph is brittle. 
//  Moreover,every brittle graph has a simplicial vertex or a co-simplicialvertex or a homogeneous set.

//: return number of voronoi vertices...
bool load_from_vor_file (dbsk3d_fs_mesh* fs_mesh, const char* pcVORFile)
{
  vul_printf (vcl_cout, "\n  load_from_vor_file(): Pass 1\n");

  // --- Start Scanning the Voronoi data (ASCII file from qvoronoi)
  FILE* fp = fopen (pcVORFile, "r");
  if (fp == NULL)
    return false;

  //: Dimension 3
  int iDim;
  fscanf (fp, "%d\n", &iDim);
  assert (iDim == 3);

  //: Number of Voronoi vertices
  int num_voronoi_nodes;
  fscanf (fp, "%d", &num_voronoi_nodes);   

  //: Number of input points
  int nG;
  fscanf (fp, "%d", &nG); 
 
  //: Read in Voronoi vertices.
  int n_non_co_simplical;
  fscanf(fp, "%d\n", &n_non_co_simplical);

  double x, y, z;
  for (int i=0; i<num_voronoi_nodes; i++) {
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) fs_mesh->_new_vertex ();
    FV->set_id (i);
    fscanf(fp, "%lf %lf %lf\n", &x, &y, &z);
    FV->get_pt().set (x, y, z);
    fs_mesh->_add_vertex (FV);
  }
  vul_printf (vcl_cout, "\tread in %d Voronoi nodes finished.\n", num_voronoi_nodes);

  //: Read in Voronoi regions (centering at each PointGene)
  //  !! Important !!
  //  This information is NOT used! i.e. can remove it by changing the QHull run options.
  int nN_G;
  int id;
  for (int i=0; i<nG; i++) {
    fscanf (fp, "%d", &nN_G);
    for (int j=0; j<nN_G; j++)
      fscanf (fp, " %d", &id);
    fscanf(fp, "\n");
  }
  vul_printf (vcl_cout, "\tread in Voronoi regions for %d genes finished.\n", nG);

  //: Read in Voronoi facets
  int num_voronoi_faces;
  fscanf (fp, "%d\n", &num_voronoi_faces);
  
  vul_printf (vcl_cout, "\tread in %d Voronoi faces...\n", num_voronoi_faces);
  unsigned int count = 0;
  for (int i = 0; i < num_voronoi_faces; i++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) fs_mesh->_new_face ();
    FF->set_id (i);

    int nN_P; //number of fs_vertex per fs_face
    fscanf(fp, "%d", &nN_P);
    nN_P -= 2;

    //read in but skip the generator info, will be processed in pass 2.
    int iGeneA, iGeneB;
    fscanf(fp, " %d", &iGeneA);
    fscanf(fp, " %d", &iGeneB);

    //: Get list of nodeElms bounding this FF
    //: Note that the first node might be the infinity-node
    int nid;
    for (int j=0; j<nN_P; j++) {
      fscanf (fp, " %d", &nid);
      //Use the dbmsh3d_face::vertices_ to store the indexed vertices
      dbmsh3d_vertex* V = fs_mesh->vertexmap (nid);
      FF->_ifs_add_bnd_V (V);
      V->add_F (FF);
      count++;
    }
    fscanf (fp, "\n");

    fs_mesh->_add_face (FF);
  }
  vul_printf (vcl_cout, "\tdone.\n", num_voronoi_faces);
  fclose(fp);
  return true;
}

bool load_from_vor_file_pass2 (dbsk3d_fs_mesh* fs_mesh, 
                               const char* pcVORFile, const int nOrigGene,
                               vcl_vector<vcl_vector<dbsk3d_fs_face*> >& G_S_asso)

{
  vul_printf (vcl_cout, "\n  load_from_vor_file_pass2(): Pass 2\n");

  // --- Start Scanning the Voronoi data (ASCII file from qvoronoi)
  FILE* fp = fopen (pcVORFile, "r");
  if (fp == NULL)
    return false;

  //: Dimension 3
  int iDim;
  fscanf (fp, "%d\n", &iDim);
  assert (iDim == 3);

  //Number of Voronoi vertices
  int num_voronoi_nodes;
  fscanf (fp, "%d", &num_voronoi_nodes);   

  //Number of input points
  int nG;
  fscanf (fp, "%d", &nG); 
 
  //Read in Voronoi nodes
  int n_non_co_simplical;
  fscanf(fp, "%d\n", &n_non_co_simplical);

  double x, y, z;
  for (int i=0; i<num_voronoi_nodes; i++)
    fscanf(fp, "%lf %lf %lf\n", &x, &y, &z);
  vul_printf (vcl_cout, "\tread in %d Voronoi nodes finished.\n", num_voronoi_nodes);

  //Read in Voronoi regions (centering at each PointGene)
  int nN_G, id;
  for (int i=0; i<nG; i++) {
    fscanf (fp, "%d", &nN_G);
    for (int j=0; j<nN_G; j++)
      fscanf (fp, " %d", &id);
    fscanf(fp, "\n");
  }
  vul_printf (vcl_cout, "\tread in Voronoi regions for %d genes finished.\n", nG);

  //: Read in Voronoi patches
  int num_voronoi_faces;
  fscanf (fp, "%d\n", &num_voronoi_faces);

  G_S_asso.resize (nG - nOrigGene);
  
  vul_printf (vcl_cout, "\tread in %d Voronoi faces...\n", num_voronoi_faces);
  for (int i = 0; i < num_voronoi_faces; i++) {
    dbsk3d_fs_face* FF = (dbsk3d_fs_face*) fs_mesh->facemap (i);

    int nN_P;
    fscanf(fp, "%d", &nN_P);
    nN_P -= 2;

    int iGeneA, iGeneB;
    fscanf(fp, " %d", &iGeneA);
    fscanf(fp, " %d", &iGeneB);

    dbmsh3d_vertex* Ga = fs_mesh->bnd_mesh()->vertexmap (iGeneA);
    FF->set_G (0, Ga);
    dbmsh3d_vertex* Gb = fs_mesh->bnd_mesh()->vertexmap (iGeneB);
    FF->set_G (1, Gb);

    //Only add the infGene to the multimap.
    if (iGeneA >= nOrigGene)
      G_S_asso[iGeneA-nOrigGene].push_back (FF);
    if (iGeneB >= nOrigGene)
      G_S_asso[iGeneB-nOrigGene].push_back (FF);

    //Get list of nodeElms bounding this FF
    //Note that the first node might be the infinity-node
    int nid;
    for (int j=0; j<nN_P; j++)
      fscanf (fp, " %d", &nid);
    fscanf (fp, "\n");
  }
  vul_printf (vcl_cout, "\t%u entries added to G_S_asso[][].\n", G_S_asso.size(), nG - nOrigGene);
  vul_printf (vcl_cout, "\tdone.\n");
  fclose(fp);
  return true;
}

//#################################################################

void rm_bndsphere_shock_recovery (dbsk3d_fs_mesh* fs_mesh, const int n_orig_gene,
                                  vcl_vector<vcl_vector<dbsk3d_fs_face*> >& G_S_asso)
{
  vul_printf (vcl_cout, "\nrm_bndsphere_shock_recovery(): G_S_asso size: %u\n", G_S_asso.size());
  vul_printf (vcl_cout, "\tTotal %u shock nodes, %u links, %u patches.\n", 
           fs_mesh->vertexmap().size(), fs_mesh->edgemap().size(), fs_mesh->facemap().size());
  
  //: The infinity shock elms are the shocks formed with the 'infinity' generators.
  //  Go through all bounding sphere of dbsk3d_point_genes, 
  //  put all nodeElms, linkElms, sheetElms with them into the to_delete set.
  vcl_set<int> inf_P_to_del;
  vcl_set<int> inf_L_to_del;
  vcl_set<int> inf_N_to_del;

  assert (n_orig_gene < int(fs_mesh->bnd_mesh()->vertexmap().size()));
  for (unsigned int i=n_orig_gene; i<fs_mesh->bnd_mesh()->vertexmap().size(); i++) {
    dbmsh3d_vertex* infGene = fs_mesh->bnd_mesh()->vertexmap(i);

    //Loop through each incident face for this infGene.
    int gsid = infGene->id() - n_orig_gene;
    vcl_vector<dbsk3d_fs_face*>::iterator it = G_S_asso[gsid].begin();
    for (; it != G_S_asso[gsid].end(); it++) {
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) *it;
  
      //Set each FF's connecting linkElms and nodeElms to INF type.
      dbmsh3d_halfedge* cur_he = FF->halfedge();
      do {
        dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) cur_he->edge();

        inf_L_to_del.insert (FE->id());
        inf_N_to_del.insert (FE->s_FV()->id());
        inf_N_to_del.insert (FE->e_FV()->id());

        cur_he = cur_he->next();
      }
      while (cur_he != FF->halfedge());

      //Put this FF to delete list.
      inf_P_to_del.insert (FF->id());
    }

    //Erase the entry of this infGene in G_S_asso[][].
    G_S_asso[gsid].clear();

  }
  G_S_asso.clear();
  
  // ###############################################################
  //: Go through all 'infinity' fs_edges (to delete) and check
  //  if it is incident to any other non-inf fs_face. If so, keep them.
  vcl_set<int>::iterator it = inf_L_to_del.begin();
  for (; it != inf_L_to_del.end(); it++) {
    int id = (*it);
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) fs_mesh->edgemap (id);

    //: The unbounded patchElms
    dbmsh3d_halfedge* HE = FE->halfedge();
    do {
      dbsk3d_fs_face* FF = (dbsk3d_fs_face*) HE->face();
      if (is_FF_inf (FF, n_orig_gene) == false) {        
        FF->set_finite (false); //set FF to be unbounded.
        FE->set_inf (); //Keep this infinity FE and set to be 'inf'.
        FE->s_FV()->set_inf ();
        FE->e_FV()->set_inf ();
      }

      HE = HE->pair();
    }
    while (HE != FE->halfedge());
  }

  // ###############################################################
  //Remove all bounding sphere PointGenes
  remove_bndsphere_genes (fs_mesh->bnd_mesh(), n_orig_gene);

  //Remove all INF shock patchElms, linkElms, and finally nodeElms.
  vul_printf (vcl_cout, "\tremoving %u shock sheet elements at infinity.\n", 
              inf_P_to_del.size());
  it = inf_P_to_del.begin();
  for (; it != inf_P_to_del.end(); it++) {
    int id = (*it);
    fs_mesh->remove_face (id);
  }
  inf_P_to_del.clear();

  vul_printf (vcl_cout, "\tremoving %u possible shock link elements at infinity.\n", 
              inf_L_to_del.size());
  it = inf_L_to_del.begin();
  for (; it != inf_L_to_del.end(); it++) {
    int id = (*it);
    dbsk3d_fs_edge* FE = (dbsk3d_fs_edge*) fs_mesh->edgemap (id);
    //remove the one without the mark of 'inf to keep'
    if (FE->b_inf()==false)
      fs_mesh->remove_edge (id);
  }
  inf_L_to_del.clear();

  vul_printf (vcl_cout, "\tremoving %u possible shock node elements at infinity.\n", 
              inf_N_to_del.size());
  it = inf_N_to_del.begin();
  for (; it != inf_N_to_del.end(); it++) {
    int id = (*it);
    dbsk3d_fs_vertex* FV = (dbsk3d_fs_vertex*) fs_mesh->vertexmap (id);
    //remove the one without the mark of 'inf to keep'
    if (FV->b_inf()==false)
      fs_mesh->remove_vertex (id);
  }
  inf_N_to_del.clear();

  vul_printf (vcl_cout, "\tRemaining: %u shock nodes, %u links, %u patches.\n", 
           fs_mesh->vertexmap().size(), fs_mesh->edgemap().size(), fs_mesh->facemap().size());
}

void remove_bndsphere_genes (dbmsh3d_mesh* bndset, const int n_orig_gene)
{
  vul_printf (vcl_cout, "  remove_bndsphere_genes()\n");

  //Remove all generators with id after n_orig_gene.
  int max_id = int(bndset->vertexmap().size()); 
  for (int i=n_orig_gene; i<max_id; i++) {
    vcl_map<int, dbmsh3d_vertex*>::iterator it = bndset->vertexmap().find (i);
    dbmsh3d_vertex* G = (*it).second;
    delete G;
    bndset->vertexmap().erase (it);
  }
  
  assert (bndset->vertexmap().size() == n_orig_gene);
  vul_printf (vcl_cout, "\tAfter removing bounding sphere, back to %u points.\n", n_orig_gene);
}




