//---------------------------------------------------------------------
// This is brcv/rec/dbasnh/dbasnh_hypg_algos.cxx
//:
// \file
// \brief  The dbasn_hypg algorithms.
//
// \author
//  Ming-Ching Chang - Jan 6, 2008
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
#include <dbasn/dbasnh_hypg_algos.h>

//: Contruct a random generated graph with # of nodes 'nN' and connectivity 'con'.
void generate_random_hypg (dbasnh_hypg* G, const float corner_con)
{ 
  const int nN = G->n_nodes();
  //Allocate space for corners.
  G->alloc_corners_angle ();
  //Count the max number of corners.
  int max_corners = G->count_max_corners ();

  //Setup the corner table by the parameter corner_con.
  vnl_random mzr;
  int nC = vcl_floor (max_corners * corner_con);
  int i, j, k;

  for (int n = 0; n<nC; n++) {
    while (1) {
      i = mzr.lrand32(0, nN-1);
      j = (i + mzr.lrand32(1, nN-1)) % nN;
      k = (j + mzr.lrand32(1, nN-1)) % nN;

      //use the corner whenever link[i,j] and link[j,k] exists.
      if (G->links(i,j)==0 || G->links(j,k)==0)
        continue;

      if (G->corners(i,j,k) == 0) 
        break;
    }

    //corners_[][][] take [0,1] real values.
    G->add_corner_angle (i, j, k, mzr.drand32(0.0, 1.0));
  }
}

//: Generates a subgrah of the current graph by deleting the given percentage 
//  of nodes. labels will hold the correct matching.
void generate_subhypg (dbasnh_hypg* g, dbasnh_hypg* inputG, int nN, int *labels)
{
  assert (0);
}

// if (links) add unit normal noise to link weights
// if (attrbs) add unit normal noise to attrbs of nodes
// range is the range of uniform noise added
void generate_noisy_subhypg (dbasnh_hypg* g, dbasnh_hypg* 
                             inputG, int *labels, double dNoise,
                             float percent_remove_C)
{
  vnl_random mzr;
  int nN = g->n_nodes();
  assert (nN != 0);
  assert (g->corners() == NULL);
  g->alloc_corners_angle ();

  //Keep the corners of inputG but add noise.
  for (int i=0; i<nN; i++) {
    for (int j=0; j<nN; j++) {
      for (int k=0; k<nN; k++) {
        float cost = inputG->corners (labels[i], labels[j], labels[k]);
        if (cost == 0)
          continue;
        if (g->links(i,j)==0 || g->links(j,k)==0)
          continue;

        double percent = mzr.drand32 (0, 1);
        if (percent < percent_remove_C)
          continue; //skip this corner.

        if (g->corners(i,j,k)==0) {
          double noise = mzr.drand32(0.0, dNoise);
          cost += noise;
          g->add_corner_angle (i, j, k, cost);
        }
      }
    }
  }
  assert (g->n_corners() <= inputG->n_corners());
}

int  load_GA_hypg_file (dbasnh_hypg* G, const vcl_string& filename)
{
  return 0;
}

void save_GA_hypg_file (dbasnh_hypg* G, const vcl_string& filename)
{
}



