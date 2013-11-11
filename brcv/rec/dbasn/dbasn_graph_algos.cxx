//---------------------------------------------------------------------
// This is brcv/rec/dbasn/dbasn_graph_algos.h
//:
// \file
// \brief  Some simple graph algorithms.
//
// \author
//  Ming-Ching Chang - Aug, 29, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_cmath.h>
#include <vcl_fstream.h>
#include <vul/vul_printf.h>
#include <vnl/vnl_random.h>
#include <dbasn/dbasn_graph_algos.h>
#include <vcl_cstring.h>
#include <vcl_cstdlib.h>

//: Contruct a random generated graph with # of nodes 'nN' and connectivity 'con'.
void generate_random_graph (dbasn_graph* G, const int nN, const float con)
{
  //1) Generate random nodes.  
  vnl_random mzr;
  for (int i = 0; i<nN; i++)
    G->_add_node (new dbasn_node (i, mzr.drand32(0.0, 1.0)));

  //2) Allocate space for links.
  G->alloc_links ();

  //3) Setup links by parameter 'con'
  int nL = vcl_floor (nN * nN * con);
  int i, j;
  for (int n = 0; n<nL; n++) {
    while (1) {
      i = mzr.lrand32(0, nN-1);
      j = (i + mzr.lrand32(1, nN-1)) % nN;
      if (G->links(i,j) == 0) 
        break;
    }

    //links_[][] take [0,1] real values.
    G->add_link (i, j, mzr.drand32(0.0, 1.0));
  }
}

dbasn_graph* generate_subgraph (dbasn_graph* inputG, int nN, int *labels) 
{
  vnl_random mzr;

  //1)Create a sub-graph, allocate links.
  dbasn_graph* g = new dbasn_graph ();
  g->alloc_links ();

  //2)randomly select nN nodes from current graph
  int n;
  for (int i = 0; i<nN; i++) {
    while(1) {
      n = mzr.lrand32(0, inputG->nodes().size()-1);
      int tag = 0;
      for (int j = 0; j<i; j++)  // assure that each node appears only once
        if (labels[j] == n) {
          tag = 1;
          break;
        }
      if (!tag) 
        break;
    }
    labels[i] = n;
  }

  for (int i = 0; i<nN; i++) {
    g->_add_node (new dbasn_node (i, inputG->nodes(labels[i])->cost()));
  }

  for (int i = 0; i<nN; i++)
    for (int j = i+1; j<nN; j++)
      g->add_link (i, j, inputG->links (labels[i], labels[j]));
  
  return g;
}

// if (links_) add unit normal noise to link weights
// if (attrbs) add unit normal noise to attrbs of nodes
// dNoise is the dNoise of uniform noise added
void generate_noisy_subgraph (dbasn_graph* g, dbasn_graph* inputG, int nN, int *labels, 
                              bool add_noise_to_L, bool add_noise_to_N, double dNoise,
                              float percent_remove_L) 
{
  vnl_random mzr;

  //randomly select nN nodes from current graph
  int n;
  for (int i = 0; i<nN; i++) {
    while(1) {
      n = mzr.lrand32(0, inputG->nodes().size()-1);
      int tag = 0;
      for (int j = 0; j<i; j++)  // assure that each node appears only once
        if (labels[j] == n) {
          tag = 1;
          break;
        }
      if (!tag) break;
    }
    labels[i] = n;
  }

  for (int i = 0; i<nN; i++) {
    double cost = inputG->nodes(labels[i])->cost();
    if (add_noise_to_N) { //Add noise such that the node cost is always within [0 ~ 1].      
      do
        cost += mzr.drand32 (-dNoise, dNoise); //0.0
      while (cost < 0 || cost > 1);
    }
    g->_add_node (new dbasn_node(i, cost));
  }

  g->alloc_links ();

  //keep the edges corresponding to the selected nodes
  for (int i = 0; i<nN; i++) {
    for (int j = i+1; j<nN; j++) {
      float cost = inputG->links (labels[i], labels[j]);
      if (cost == 0)
        continue;

      double percent = mzr.drand32 (0, 1);
      if (percent < percent_remove_L)
        continue; //skip this link.

      if (!add_noise_to_L)
        g->add_link (i, j, cost);
      else {
        double n_v = mzr.drand32(0.0, dNoise);
        g->add_link (i, j, cost + n_v);
      }
    }
  }
}


int load_GA_graph_file (dbasn_graph* G, const vcl_string& filename)
{
  char buffer[1024];
  int nN = 0, nL = 0;
 
  vcl_ifstream infp(filename.c_str(), vcl_ios::in);
  if (!infp) {
    vcl_cout << " Error opening file!" << vcl_endl;
    vcl_exit(-1); 
  }

  vcl_vector<int> links_id;
  vcl_vector<int> links_id2;
  vcl_vector<double> links_attr;

  while (infp.getline(buffer,1024)) {    
    //ignore comment lines and empty lines
    if (strlen(buffer)<2 || buffer[0]=='#')
      continue;

    if (!vcl_strncmp(buffer, "[BEGIN GRAPH]", sizeof("[END GRAPH]")-1))
      continue;
    if (!vcl_strncmp(buffer, "[END GRAPH]", sizeof("[END GRAPH]")-1))
      continue;
 
    //read the line with the contour count info
    if (!vcl_strncmp(buffer, "NODE_COUNT=", sizeof("NODE_COUNT=")-1)){
      sscanf (buffer,"NODE_COUNT=%d", &nN);
      continue;
    }
 
    //read the beginning of a node block
    if (!vcl_strncmp(buffer, "[BEGIN NODE]", sizeof("[BEGIN NODE]")-1)){
      
      //each [BEGIN NODE] -- [END NODE] block is a node 
      infp.getline (buffer, 1024);
      int id;
      sscanf (buffer, "NODE_NO=%d", &id);

      infp.getline (buffer, 1024);
      double attr;
      sscanf (buffer, "NODE_ATTR=%lf", &attr);

      dbasn_node* N = new dbasn_node (id, attr);

      infp.getline (buffer, 1024);
      int nLinksForThisNode;
      sscanf(buffer,"LINK_COUNT=%d",&nLinksForThisNode);
      nL += nLinksForThisNode;

      for (int j=0; j<nLinksForThisNode; j++){
        //the rest should have data that goes into the current contour
        infp.getline (buffer, 1024);
        int id2;
        sscanf(buffer,"%d %lf", &id2, &attr);

        ///G->add_link (id, id2, attr);
        ///N->_add_conn_nid (id2);
        ///G->links()[id][id2] = attr;
        ///G->links()[id2][id] = attr;
        links_id.push_back (id);
        links_id2.push_back (id2);
        links_attr.push_back (attr);
      }
   
      infp.getline (buffer, 1024);
      if (vcl_strncmp(buffer, "[END NODE]", sizeof("[END NODE]")-1))
        assert(false);

      G->_add_node (N);
    } 
  }

  //Now put the links in.  
  G->alloc_links ();

  assert (links_id.size() == links_id2.size());
  assert (links_id.size() == links_attr.size());
  for (unsigned int i=0; i<links_id.size(); i++) {
    int id = links_id[i];
    int id2 = links_id2[i];
    double attr = links_attr[i];
    if (G->links(id, id2)==0)
      G->add_link (id, id2, float(attr));
  }

  infp.close();
  return nL;
}

void save_GA_graph_file (dbasn_graph* G, const vcl_string& filename)
{ 
  FILE* fp;
  if ((fp = fopen(filename.c_str(), "w")) == NULL) {
    vul_printf (vcl_cout, "ERROR(SaveGAGraphFile): Can't open output dbasn_graph file %s.\n", filename.c_str());
    return; 
  }

  vcl_fprintf (fp, "[BEGIN GRAPH]\n");
  vcl_fprintf (fp, "NODE_COUNT=%d\n", G->nodes().size());
  for (unsigned int i=0; i<G->nodes().size(); i++) {
    const dbasn_node* N = G->nodes (i);
    vcl_fprintf (fp, "[BEGIN NODE]\n");
    vcl_fprintf (fp, "NODE_NO=%d\n",  N->nid());
    vcl_fprintf (fp, "NODE_ATTR=%lf\n", N->cost());
    vcl_fprintf (fp, "LINK_COUNT=%d\n",  N->n_conn_nids());
    for (int j=0; j<N->n_conn_nids(); j++) {
      int j_id = N->conn_nids (j);
      vcl_fprintf (fp, "  %d  %lf\n", j_id, G->links()[i][j_id]);
    }
    vcl_fprintf (fp, "[END NODE]\n");
  }
  fclose (fp);
}


