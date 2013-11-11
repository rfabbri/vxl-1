#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <algorithm>

#include "csa.h"
#include "kofn.h"
#include "Array.h"
#include "match_edge_maps.h"

#include <dbdet/edge/dbdet_edgemap_sptr.h>
#include <dbdet/edge/dbdet_edgemap.h>


//rint 
inline double rint (double x) {
  double low = floor(x);
  if (fmod(x,low) >= (double)0.5)
    return(ceil(x));
  else
    return(low);
}

struct Edge {
  int i,j;      // node ids, 0-based
  double w;     // distance between pixels
};

// CSA code needs integer weights.  Use this multiplier to convert
// floating-point weights to integers.
static const int multiplier = 100;

// The degree of outlier connections.
static const int degree = 6;

double matchEdgeMaps ( dbdet_edgemap_sptr edgemap1, dbdet_edgemap_sptr edgemap2,
                      double maxDist, double outlierCost,
                      std::vector<int> &matched1, std::vector<int> &matched2)
{

  // Check global constants.
  assert (degree > 0);
  assert (multiplier > 0);

  // Check arguments.
  assert (maxDist >= 0);
  assert (outlierCost > maxDist);

  const int num_edges1 = edgemap1->num_edgels();
  const int num_edges2 = edgemap2->num_edgels();

  // Radius of search window.
  const int r = (int) ceil (maxDist);   

  // Figure out which nodes are matchable, i.e. within maxDist
  // of another node.
  std::vector<bool> matchable1 (num_edges1, false);
  std::vector<bool> matchable2 (num_edges2, false);

  for (int i = 0; i < num_edges1; i++) 
  {
    double x1 = edgemap1->edgels[i]->pt.x();
    double y1 = edgemap1->edgels[i]->pt.y();
    int ix1 = (int) rint(x1);
    int iy1 = (int) rint(y1);

    for (int v = -r; v <= r; v++) {
      for (int u = -r; u <= r; u++) {
        const int x2 = ix1 + u;
        const int y2 = iy1 + v;

        if (x2 < 0 || x2 >= edgemap1->width()) { continue; }
        if (y2 < 0 || y2 >= edgemap1->height()) { continue; }

        if (edgemap2->cell(x2,y2).size()==0) continue;
        dbdet_edgel* e2 = edgemap2->cell(x2,y2).front();

        const double d2 = (e2->pt.x()-x1)*(e2->pt.x()-x1) + (e2->pt.y()-y1)*(e2->pt.y()-y1); //dist from edgel to edgel
        if (d2 > maxDist*maxDist) { continue; }

        matchable1[i] = true;
        matchable2[e2->id] = true;
      }
    }
  }

  // Count the number of nodes on each side of the match.
  // Construct nodeID->pixel and pixel->nodeID maps.
  // Node IDs range from [0,n1) and [0,n2).
  int n1=0, n2=0;
  std::vector<int> nodeToPix1; //nodeID->index on edge_list
  std::vector<int> nodeToPix2;
  std::vector<int> pixToNode1 (num_edges1);
  std::vector<int> pixToNode2 (num_edges2);

  for (int i = 0; i < num_edges1; i++){
    pixToNode1[i] = -1;
    if (matchable1[i]) {
      pixToNode1[i] = n1;
      nodeToPix1.push_back(i);
      n1++;
    }
  }

  for (int i = 0; i < num_edges2; i++){
    pixToNode2[i] = -1;
    if (matchable2[i]) {
      pixToNode2[i] = n2;
      nodeToPix2.push_back(i);
      n2++;
    }
  }

  // Construct the list of edges between edgels within maxDist.
  std::vector<Edge> edges;
  for (int i = 0; i < num_edges1; i++) 
  {
    if (!matchable1[i]) { continue; }

    double x1 = edgemap1->edgels[i]->pt.x();
    double y1 = edgemap1->edgels[i]->pt.y();
    int ix1 = (int) rint(x1);
    int iy1 = (int) rint(y1);

    for (int v = -r; v <= r; v++) {      
      for (int u = -r; u <= r; u++) {
        const int x2 = ix1 + u;
        const int y2 = iy1 + v;

        if (x2 < 0 || x2 >= edgemap1->width()) { continue; }
        if (y2 < 0 || y2 >= edgemap1->height()) { continue; }

        if (edgemap2->cell(x2,y2).size()==0) continue;
        dbdet_edgel* e2 = edgemap2->cell(x2,y2).front();

        const double d2 = (e2->pt.x()-x1)*(e2->pt.x()-x1) + (e2->pt.y()-y1)*(e2->pt.y()-y1); //dist from edgel to edgel
        if (d2 > maxDist*maxDist) { continue; }

        if (!matchable2[e2->id]) { continue; }

        Edge e; 
        e.i = pixToNode1[i];
        e.j = pixToNode2[e2->id];
        e.w = sqrt(d2);
        assert (e.i >= 0 && e.i < n1);
        assert (e.j >= 0 && e.j < n2);
        assert (e.w < outlierCost);
        edges.push_back(e);
      }
    }
  }

  // The cardinality of the match is n.
  const int n = n1 + n2;
  const int nmin = std::min(n1,n2);
  const int nmax = std::max(n1,n2);

  // Compute the degree of various outlier connections.
  const int d1 = std::max(0, std::min(degree,n1-1)); // from map1
  const int d2 = std::max(0, std::min(degree,n2-1)); // from map2
  const int d3 = std::min(degree, std::min(n1,n2)); // between outliers
  const int dmax = std::max(d1, std::max(d2,d3));

  assert (n1 == 0 || (d1 >= 0 && d1 < n1));
  assert (n2 == 0 || (d2 >= 0 && d2 < n2));
  assert (d3 >= 0 && d3 <= nmin);

  // Count the number of edges.
  int m = 0;
  m += edges.size();    // real connections
  m += d1 * n1; // outlier connections
  m += d2 * n2; // outlier connections
  m += d3 * nmax;       // outlier-outlier connections
  m += n;               // high-cost perfect match overlay

  // If the graph is empty, then there's nothing to do.
  if (m == 0) {
    return 0;
  }

  // Weight of outlier connections.
  const int ow = (int) ceil (outlierCost * multiplier);

  // Scratch array for outlier edges.
  Array1D<int> outliers (dmax);

  // Construct the input graph for the assignment problem.
  Array2D<int> igraph (m,3);
  int count = 0;
  // real edges
  for (int a = 0; a < (int)edges.size(); a++) {
    int i = edges[a].i;
    int j = edges[a].j;
    assert (i >= 0 && i < n1);
    assert (j >= 0 && j < n2);
    igraph(count,0) = i;
    igraph(count,1) = j;
    igraph(count,2) = (int) rint (edges[a].w * multiplier);
    count++;
  }
  // outliers edges for map1, exclude diagonal
  for (int i = 0; i < n1; i++) {
    kOfN(d1,n1-1,outliers.data());
    for (int a = 0; a < d1; a++) {
      int j = outliers(a);
      if (j >= i) { j++; }
      assert (i != j);
      assert (j >= 0 && j < n1);
      igraph(count,0) = i;
      igraph(count,1) = n2 + j;
      igraph(count,2) = ow;
      count++;
    }
  }
  // outliers edges for map2, exclude diagonal
  for (int j = 0; j < n2; j++) {
    kOfN(d2,n2-1,outliers.data());
    for (int a = 0; a < d2; a++) {
      int i = outliers(a);
      if (i >= j) { i++; }
      assert (i != j);
      assert (i >= 0 && i < n2);
      igraph(count,0) = n1 + i;
      igraph(count,1) = j;
      igraph(count,2) = ow;
      count++;
    }
  }
  // outlier-to-outlier edges
  for (int i = 0; i < nmax; i++) {
    kOfN(d3,nmin,outliers.data());
    for (int a = 0; a < d3; a++) {
      const int j = outliers(a);
      assert (j >= 0 && j < nmin);
      if (n1 < n2) {
        assert (i >= 0 && i < n2);
        assert (j >= 0 && j < n1);
        igraph(count,0) = n1 + i;
        igraph(count,1) = n2 + j;
      } else {
        assert (i >= 0 && i < n1);
        assert (j >= 0 && j < n2);
        igraph(count,0) = n1 + j;
        igraph(count,1) = n2 + i;
      }
      igraph(count,2) = ow;
      count++;
    }
  }
  // perfect match overlay (diagonal)
  for (int i = 0; i < n1; i++) {
    igraph(count,0) = i;
    igraph(count,1) = n2 + i;
    igraph(count,2) = ow * multiplier;
    count++;
  }
  for (int i = 0; i < n2; i++) {
    igraph(count,0) = n1 + i;
    igraph(count,1) = i;
    igraph(count,2) = ow * multiplier;
    count++;
  }
  assert (count == m);

  // Check all the edges, and set the values up for CSA.
  for (int i = 0; i < m; i++) {
    assert(igraph(i,0) >= 0 && igraph(i,0) < n);
    assert(igraph(i,1) >= 0 && igraph(i,1) < n);
    igraph(i,0) += 1;
    igraph(i,1) += 1+n;
  }

  // Solve the assignment problem.
  CSA csa(2*n,m,igraph.data());
  assert(csa.edges()==n);

  Array2D<int> ograph (n,3);
  for (int i = 0; i < n; i++) {
    int a,b,c;
    csa.edge(i,a,b,c);
    ograph(i,0)=a-1; ograph(i,1)=b-1-n; ograph(i,2)=c;
  }

  // Check the solution.
  // Count the number of high-cost edges from the perfect match
  // overlay that were used in the match.
  int overlayCount = 0;
  for (int a = 0; a < n; a++) {
    const int i = ograph(a,0);
    const int j = ograph(a,1);
    const int c = ograph(a,2);
    assert (i >= 0 && i < n);
    assert (j >= 0 && j < n);
    assert (c >= 0);
    // edge from high-cost perfect match overlay
    if (c == ow * multiplier) { overlayCount++; }
    // skip outlier edges
    if (i >= n1) { continue; }
    if (j >= n2) { continue; }

    // for edges between real nodes, check the edge weight
    const vgl_point_2d<double> pix1 = edgemap1->edgels[nodeToPix1[i]]->pt;
    const vgl_point_2d<double> pix2 = edgemap2->edgels[nodeToPix2[j]]->pt;

    const double dx = pix1.x() - pix2.x();
    const double dy = pix1.y() - pix2.y();
    const int w = (int) rint (sqrt(dx*dx+dy*dy)*multiplier);
    assert (w == c);
  }

  // Print a warning if any of the edges from the perfect match overlay
  // were used.  This should happen rarely.  If it happens frequently,
  // then the outlier connectivity should be increased.
  if (overlayCount > 5) {
    fprintf (stderr, "%s:%d: WARNING: The match includes %d "
      "outlier(s) from the perfect match overlay.\n",
      __FILE__, __LINE__, overlayCount);
  }

  // construct the match maps
  for (int a = 0; a < n; a++) {
    // node ids
    const int i = ograph(a,0);
    const int j = ograph(a,1);
    // skip outlier edges
    if (i >= n1) { continue; }
    if (j >= n2) { continue; }

    // map node ids to edgel ids and record matching edges
    matched1[nodeToPix1[i]] = nodeToPix2[j];
    matched2[nodeToPix2[j]] = nodeToPix1[i];
  }

  // Compute the match cost.
  double cost = 0;
  for (int i = 0; i < num_edges1; i++) 
  {
    if (matched1[i] == -1){
      cost += outlierCost;
    }
    else {
      const double dx = edgemap1->edgels[i]->pt.x() - edgemap2->edgels[matched1[i]]->pt.x();
      const double dy = edgemap1->edgels[i]->pt.y() - edgemap2->edgels[matched1[i]]->pt.y();
      cost += 0.5 * sqrt (dx*dx + dy*dy);
    }
  }

  for (int i = 0; i < num_edges2; i++) 
  {
    if (matched2[i] == -1){
      cost += outlierCost;
    }
    else {
      const double dx = edgemap2->edgels[i]->pt.x() - edgemap1->edgels[matched2[i]]->pt.x();
      const double dy = edgemap2->edgels[i]->pt.y() - edgemap1->edgels[matched2[i]]->pt.y();
      cost += 0.5 * sqrt (dx*dx + dy*dy);
    }
  }

  // Return the match cost.
  return cost;
}
