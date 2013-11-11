#include "dbdet_generic_linker.h"

#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_cassert.h>
#include <vcl_deque.h>
#include <vcl_algorithm.h>
#include <vnl/vnl_math.h>
#include <pdf1d/pdf1d_calc_mean_var.h>

#include <dbdet/edge/dbdet_edgemap.h>

//: Constructor
dbdet_generic_linker::dbdet_generic_linker(dbdet_edgemap_sptr edgemap, 
                                           dbdet_curve_fragment_graph& curve_frag_graph,
                                           vbl_array_2d<bool> & unlinked, 
                                           double threshold) :
  edgemap_(edgemap), 
  occupied_(edgemap->occupancy), 
  unlinked_(unlinked), 
  curve_frag_graph_(curve_frag_graph),
  nrows_(edgemap->height()), ncols_(edgemap->width()), 
  threshold_(threshold),
  req_app_consistency_(true), app_thresh_(0.2), 
  req_orient_consistency_(true),
  req_smooth_continuation_(false),
  req_low_curvatures_(false)
{
  //update the unlinked map
  unlinked_.resize(nrows_, ncols_);
  occupied_.resize(nrows_, ncols_); //this needs to be done by the edgemap

  for (unsigned row=0; row<nrows_; row++){
    for (unsigned col=0; col<ncols_; col++)
    {
      if (edgemap_->edge_cells(row, col).size()>0){
        if (edgemap_->edge_cells(row, col).front()->strength > threshold_){
          unlinked_(row,col) = true;
          occupied_(row,col) = true;
        }
      }
    }
  }
}

//:destructor
dbdet_generic_linker::~dbdet_generic_linker()
{
}

//: extract image contours by tracing on the image grid
void dbdet_generic_linker::extract_image_contours_by_tracing()
{ 
  //first remove any existing contours from the contour fragment graph
  curve_frag_graph_.clear();

  //form a new contour fragment graph
  curve_frag_graph_.resize(edgemap_->num_edgels());

  //clear unlinked_ map
  unlinked_.fill(false);

  //update the unlinked map
  for (unsigned row=0; row<nrows_; row++){
    for (unsigned col=0; col<ncols_; col++){
      if (edgemap_->edge_cells(row, col).size()>0){
        if (edgemap_->edge_cells(row, col).front()->strength > threshold_){ //legal edgel
          occupied_(row,col) = true; //valid edgel exists
          unlinked_(row,col) = true; //edgel is not linked yet
        }
      }
    }
  }

  vcl_cout << "Before linking, # of unlinked edgels = " << count_unlinked_edgels() << vcl_endl;

  //trace contours from unlinked points in both directions
  trace_contours_in_both_directions();

  //first trace chains from end points
  //trace_contours_from_end_points();
  //next trace all the chains starting at junctions
  //trace_contours_from_junctions();

  vcl_cout << "After linking, # of unlinked edgels = " << count_unlinked_edgels() << vcl_endl;
}

void dbdet_generic_linker::trace_contours_in_both_directions()
{
  dbdet_edgel_chain *temp_chain; 
  int neighbor_row[8], neighbor_col[8];

  // Loop over all pixels in the image to see if chains can be started from them
  for (unsigned i = 0; i < nrows_; i++){
    for (unsigned j = 0; j < ncols_; j++){

      if (!occupied_(i,j) || !unlinked_(i,j)) //either linked or no edgel at this pixel
        continue;

      //Rules: 
      //    (a) start from an edgel that is locally legal
      //    (b) trace in both directions until an illegal edgel is reached

      // Get the # of 8-neighbors of this edgel 
      int num_of_neigh = Find_unlinked_8_Neighbors(i, j, neighbor_row, neighbor_col);

      if (num_of_neigh>0){
        //start a new chain
        temp_chain = new dbdet_edgel_chain();
        temp_chain->push_back(edgemap_->edge_cells(i,j).front());

        //trace as far as possible
        Grow_Seed_Edge (j, i, temp_chain, 0.0, 0.0, true);

        //now start tracing in the other direction (if possible)
        double dx=0.0, dy=0.0;
        if (temp_chain->edgels.size()>1){
          dx = temp_chain->edgels[0]->pt.x()-temp_chain->edgels[1]->pt.x();
          dy = temp_chain->edgels[0]->pt.y()-temp_chain->edgels[1]->pt.y();
          
          double norm = vcl_sqrt(dx*dx + dy*dy);
          dx /= norm;
          dy /= norm;
        }
        Grow_Seed_Edge (j, i, temp_chain, dx, dy, false);

        if (temp_chain->edgels.size()<2)
          delete temp_chain;
        else
          curve_frag_graph_.insert_fragment(temp_chain);//save this chain on the curve_fragment graph
      }

    }
  }

}

void dbdet_generic_linker::trace_contours_from_end_points()
{
  dbdet_edgel_chain *temp_chain; 
  int neighbor_row[8], neighbor_col[8];

  // Loop over all pixels in the image to see if chains can be started from them
  for (unsigned i = 0; i < nrows_; i++){
    for (unsigned j = 0; j < ncols_; j++){

      if (!occupied_(i,j) || !unlinked_(i,j)) //either linked or no edgel at this pixel
        continue;

      // Get the # of 8-neighbors of this edgel 
      int num_of_neigh = Find_8_Neighbors(i, j, neighbor_row, neighbor_col);

      // if this pixel contains an unlinked edgel and it has a single unlinked neighbor
      // or two unlinked neighbors that are themseleves neighbors of each other, 
      // this edgel is a good candidate to start tracing a chain from.

      if (num_of_neigh == 1 || num_of_neigh ==2 )
      {
        if (num_of_neigh == 2){
          // Make sure it is an 8-connect link (if the 2 neighbors are themselves 4-connected)
          if (!Is_4_Neighbor(neighbor_col[0], neighbor_row[0], neighbor_col[1], neighbor_row[1]))
            continue;
        }

        //start a new chain
        temp_chain = new dbdet_edgel_chain();
        temp_chain->push_back(edgemap_->edge_cells(i,j).front());

        Grow_Seed_Edge (j, i, temp_chain, 0, 0, true);

        if (temp_chain->edgels.size()<2)
          delete temp_chain;
        else
          curve_frag_graph_.insert_fragment(temp_chain);//save this chain on the curve_fragment graph
      }
    } 
  }
}

void dbdet_generic_linker::trace_contours_from_junctions()
{
  dbdet_edgel_chain *temp_chain; 
  //static int d_row[8] = {0, -1, -1, -1, 0, 1, 1, 1};
  //static int d_col[8] = {1, 1, 0, -1, -1, -1, 0, 1};

  // Having traced all possible contours from all terminal edgels, we have to now handle 
  // the BRANCH POINTS

  for (unsigned i = 0; i < nrows_; i++){
    for (unsigned j = 0; j < ncols_; j++)
    {  
      if (!unlinked_(i,j)) continue;

      //if an unlinked edgel still has more than one unlinked neighbors, this must be a junction
      if (Num_of_unlinked_8_Neighbors(i,j) >= 2 )
      {
        // Mark this edgel as linked
        unlinked_(i, j) = false;

        // flag the first neighbor
        bool firstNeighbor = true;

        // go over all its 8 neighbors
        for (int ii = -1; ii < 2; ii++){
          for (int jj = -1; jj < 2; jj++){

            if (ii==0 && jj==0) continue;
            if (!Is_Inside_Image (i+ii, j+jj)) continue;

            // If either of its neighbors are (1) unlinked and 2) has 1 or 2 neighbors
            // we can start a chain from there
            if (!unlinked_(i+ii, j+jj)) continue;

            int neighbor_row[8], neighbor_col[8];
            int num_of_neigh = Find_unlinked_8_Neighbors(i+ii, j+jj, neighbor_row, neighbor_col);

            if (num_of_neigh == 1 || num_of_neigh ==2 )
            {
              if (num_of_neigh == 2){
                // Make sure it is an 8-connect link (if the 2 neighbors are themselves 4-connected)
                if (!Is_4_Neighbor(neighbor_col[0], neighbor_row[0], neighbor_col[1], neighbor_row[1]))
                  continue;
              }

              temp_chain = new dbdet_edgel_chain();

              // add the junction edgel to the chain
              temp_chain->push_back(edgemap_->edge_cells(i, j).front());
              firstNeighbor = false; //junction formed

              //start tracing from this edgel
              vgl_point_2d<int> ept = Grow_Seed_Edge(j, i, temp_chain, 0, 0, true);

              //check to see if the end of the chain connects to the start of the chain
              //if so add the first element again, to close the loop
              if (Is_8_Neighbor(j, i, ept.x(), ept.y()) && temp_chain->edgels.size()>3)
                temp_chain->push_back(edgemap_->edge_cells(i, j).front());

              //save this chain on the curve_fragment graph
              curve_frag_graph_.insert_fragment(temp_chain);
              
            } 
          } 
        }

        if (firstNeighbor) //this edgel did not link to any other edgel
          unlinked_(i, j) = true;
      } 
    } 
  }

}

//: Grow the chain recursively from the current edgel (assume 8-connectivity)
//  Assumes that the current pixel has already been added to the current chain
//  but hasn't been marked off as linked yet.
// (dx1, dx2) record the direction of the last segment added to this chain
// Assume that appearance is constantly varying in a short segment of the current contour.
// keep a moving window of this information and use it to terminate the contour
vgl_point_2d<int> dbdet_generic_linker::Grow_Seed_Edge(int x, int y, dbdet_edgel_chain* cur_chain, 
                                                 double dx1, double dy1, 
                                                 bool forward_trace)
{
  int neighbor_row[8], neighbor_col[8];
  
  //find the number of unlinked neighbors that are connected to this edgel
  int num_of_neigh = Find_unlinked_8_Neighbors(y, x, neighbor_row, neighbor_col);

  //TODO: However, if there are linked edgels in its neighborhood, then it should 
  //      try to form a junction
  if (num_of_neigh==0)//no more edgels to link, terminate this chain
  {
    // no obvious neighbors, terminate the chain
    if (cur_chain->edgels.size()>1)
      unlinked_(y, x) = false; // Mark the terminal edge as linked

    return vgl_point_2d<int>(x,y);
  }

  //the last edgel in the chain
  dbdet_edgel* eC;
  if (forward_trace)
    eC = cur_chain->edgels.back();
  else
    eC = cur_chain->edgels.front();

  //determine the closest edgel to the current one and link to it
  vcl_multimap<double, int> dist;
  //double dist[8];
  
  for (int i=0; i<num_of_neigh; i++){
    dbdet_edgel* eN = edgemap_->edge_cells(neighbor_row[i], neighbor_col[i]).front();
    dist.insert(vcl_pair<double, int>(vgl_distance(eC->pt, eN->pt), i));
  }

  //connect to the closest one
  vcl_multimap<double, int>::iterator nit = dist.begin();
  for (; nit != dist.end(); nit++){
    //double d = nit->first;
    int min = nit->second;

    dbdet_edgel* eN = edgemap_->edge_cells(neighbor_row[min], neighbor_col[min]).front();

    //compute line segment vector
    double dy2 = eN->pt.y() - eC->pt.y();
    double dx2 = eN->pt.x() - eC->pt.x();

    double norm = vcl_sqrt(dx2*dx2 + dy2*dy2);
    dx2 /= norm;
    dy2 /= norm;

    //test for (a) orientation of edgel consistent with new segment
    //         (b) zigzag/bending (thresh = cos(pi/8))
    //         (c) appearance consistency
    //         (d) smooth continuation
    //         (e) low curvatures
    bool orientation_consistent, no_zigzag, app_consistent, smooth_continuation, curvature_low;

    if (cur_chain->edgels.size()==1)
      no_zigzag = true;  //ignore zigzag test for first link
    else
      no_zigzag = (dx1*dx2+dy1*dy2)>=0.7;

    double dp1 = dx2*vcl_cos(eC->tangent)+dy2*vcl_sin(eC->tangent);
    double dp2 = dx2*vcl_cos(eN->tangent)+dy2*vcl_sin(eN->tangent);

    bool eC_aligned = dp1>0;
    bool eN_aligned = dp2>0;

    if (!req_orient_consistency_)
      orientation_consistent = true;
    else {
      //check orientation wrt new segment (tracing direction does not matter)
      orientation_consistent = (eC_aligned == eN_aligned);
    }

    //perform the appearance consistency check
    if (req_app_consistency_){
      double Lc_app = eC_aligned ? eC->left_app->value()  : eC->right_app->value();
      double Rc_app = eC_aligned ? eC->right_app->value() : eC->left_app->value();
      double Ln_app = eN_aligned ? eN->left_app->value()  : eN->right_app->value();
      double Rn_app = eN_aligned ? eN->right_app->value() : eN->left_app->value();

      app_consistent = (vcl_fabs(Lc_app-Ln_app) < app_thresh_ &&
                             vcl_fabs(Rc_app-Rn_app) < app_thresh_);
    }
    else
      app_consistent = true;

    //perform the smoothness test
    if (req_smooth_continuation_){
      //only link if the edgels are locally relatable (tangents do not deviate too much from the curve)
      double vec = vcl_atan2(dy2, dx2);
      double th1 = vcl_min(dbdet_CCW(vec, eC->tangent), dbdet_CCW(vec, eC->tangent+vnl_math::pi));
      double th2 = vcl_min(dbdet_CCW(vec, eN->tangent), dbdet_CCW(vec, eN->tangent+vnl_math::pi));
    
      smooth_continuation = vcl_fabs(vnl_math::pi - th1+th2)<0.25;
    }
    else
      smooth_continuation = true;
    
    //perform curvature test
    if (req_low_curvatures_){
      //k = 2*sin(th)/d (this estimate can change with slight perturbations, need to be conservative)
      //curvature_low = vcl_fabs(2*vcl_sin(th1)/d)<0.5 && vcl_fabs(2*vcl_sin(th2)/d)<0.5;

      //alternative low curvature test
      curvature_low = (vcl_fabs(dp1)>0.9 && vcl_fabs(dp2)>0.9);
    }
    else
      curvature_low = true;

    if (orientation_consistent && no_zigzag && app_consistent && curvature_low && smooth_continuation)
    {
      // Mark the current edge as linked
      unlinked_(y, x) = false;

      //update the cur_edgel pointer to the neighboring edgel
      x = neighbor_col[min]; y = neighbor_row[min];

      //add the neighbor to the chain
      if (forward_trace)
        cur_chain->push_back(edgemap_->edge_cells(y, x).front());
      else
        cur_chain->push_front(edgemap_->edge_cells(y, x).front());

      //grow the chain from this neighbor
      return Grow_Seed_Edge (x, y, cur_chain, dx2, dy2, forward_trace);
    }
  }

  // no obvious neighbors, despite possibilities, terminate the chain 
  if (cur_chain->edgels.size()>1)
    unlinked_(y, x) = false;

  return vgl_point_2d<int>(x, y);
}

int dbdet_generic_linker::count_unlinked_edgels()
{
  //count the number of unlinked edgels before the linking begins
  int cnt=0;

  for (unsigned i=0; i<nrows_; i++){
    for(unsigned j=0; j<ncols_; j++){
      if (unlinked_(i,j))
        cnt++;
    }
  }
  return cnt;
}

bool dbdet_generic_linker::Is_Inside_Image (int row, int col)
{
  return ((row >= 0) && (row < (int)nrows_) &&
          (col >= 0) && (col < (int)ncols_));
}

bool dbdet_generic_linker::Is_8_Neighbor (int x1, int y1, int x2, int y2)
{
  //* Two points are 8-connected neighbors
  //* IF delta_x <= 1 AND delta_y <= 1

  int delta_x = vnl_math_abs (x2 - x1);
  int delta_y = vnl_math_abs (y2 - y1);

  return ((delta_x <= 1) && (delta_y <= 1));
}

bool dbdet_generic_linker::Is_4_Neighbor (int x1, int y1, int x2, int y2)
{
  //* Two points are 4-connected neighbors
  //* IF delta_x + delta_y = 1

  int delta_x = vnl_math_abs (x2 - x1);
  int delta_y = vnl_math_abs (y2 - y1);

  return ((delta_x+delta_y) == 1);
}

//: returns the number of 8 connected neighbors to the current edgel.
int dbdet_generic_linker::Num_of_8_Neighbors (int row, int col)
{
  int num_of_neighbors = 0;
  static int d_row[8] = {0, -1, -1, -1, 0, 1, 1, 1};
  static int d_col[8] = {1, 1, 0, -1, -1, -1, 0, 1};

  // go over all its 8 neighbors
  int neigh_row, neigh_col;
  for (int neigh = 0; neigh < 8; neigh++)
  {
    neigh_row = row+d_row[neigh];
    neigh_col = col+d_col[neigh];

    if (Is_Inside_Image (neigh_row, neigh_col) && occupied_(neigh_row, neigh_col))
      num_of_neighbors++;
  }
  return num_of_neighbors;
}

//: returns the number of 8 connected neighbors to the current edgel.
int dbdet_generic_linker::Num_of_unlinked_8_Neighbors(int row, int col)
{
  int num_of_neighbors = 0;
  static int d_row[8] = {0, -1, -1, -1, 0, 1, 1, 1};
  static int d_col[8] = {1, 1, 0, -1, -1, -1, 0, 1};

  // go over all its 8 neighbors
  int neigh_row, neigh_col;
  for (int neigh = 0; neigh < 8; neigh++)
  {
    neigh_row = row+d_row[neigh];
    neigh_col = col+d_col[neigh];

    if (Is_Inside_Image (neigh_row, neigh_col) && unlinked_(neigh_row, neigh_col))
      num_of_neighbors++;
  }
  return num_of_neighbors;
}

//: Get a list of neighboring edgels 
int dbdet_generic_linker::Find_8_Neighbors (int row, int col, int *neighbor_row, int *neighbor_col)
{
  int neighbor_ct = 0;
  static int d_row[8] = {0, -1, -1, -1, 0, 1, 1, 1};
  static int d_col[8] = {1, 1, 0, -1, -1, -1, 0, 1};

  int neigh_row, neigh_col;
  
  //go over the 8-neighbors of this edgel and determine 
  //if there are any unlinked edgels available
  for (int neigh_lp = 0; neigh_lp < 8; neigh_lp++)
  {
    neigh_row = row + d_row[neigh_lp];
    neigh_col = col + d_col[neigh_lp];

    //make sure the neighboring edgels are inside the image
    if (Is_Inside_Image (neigh_row, neigh_col) && occupied_(neigh_row, neigh_col))
    {
      neighbor_row[neighbor_ct] = neigh_row;
      neighbor_col[neighbor_ct] = neigh_col;

      neighbor_ct++;
    }
  }

  return neighbor_ct;
}

//: Get a list of neighboring unlinked edgels 
int dbdet_generic_linker::Find_unlinked_8_Neighbors (int row, int col, int *neighbor_row, int *neighbor_col)
{
  int neighbor_ct = 0;
  static int d_row[8] = {0, -1, -1, -1, 0, 1, 1, 1};
  static int d_col[8] = {1, 1, 0, -1, -1, -1, 0, 1};

  int neigh_row, neigh_col;
  
  //go over the 8-neighbors of this edgel and determine 
  //if there are any unlinked edgels available
  for (int neigh_lp = 0; neigh_lp < 8; neigh_lp++)
  {
    neigh_row = row + d_row[neigh_lp];
    neigh_col = col + d_col[neigh_lp];

    //make sure the neighboring edgels are inside the image
    if (Is_Inside_Image (neigh_row, neigh_col) && unlinked_(neigh_row, neigh_col))
    {
      neighbor_row[neighbor_ct] = neigh_row;
      neighbor_col[neighbor_ct] = neigh_col;

      neighbor_ct++;
    }
  }

  return neighbor_ct;
}

void dbdet_generic_linker::form_junctions()
{
}

void dbdet_generic_linker::report_stats()
{
  vcl_cout << "======================================" << vcl_endl;
  vcl_cout << "Edge Linking Summary\n";
  vcl_cout << "======================================" << vcl_endl;

  
}


