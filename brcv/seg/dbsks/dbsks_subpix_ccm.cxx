// This is file seg/dbsks/dbsks_subpix_ccm.cxx

//:
// \file

#include "dbsks_subpix_ccm.h"
#include <dbdet/edge/dbdet_edgemap.h>

#include <vnl/vnl_math.h>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_distance.h>




// ============================================================================
//  dbsks_subpix_ccm
// ============================================================================


////: Clear the memory
//void dbsks_subpix_ccm::
//clear()
//{
//  this->edge_strength_.clear();
//  this->edge_orient_x_.clear();
//  this->edge_orient_y_.clear();
//  this->edge_angle_.clear();
//
//  
//  this->matched_edge_ix_.clear();
//  this->matched_edge_iy_.clear();
//
//  this->closest_edge_orient_x_.clear();
//  this->closest_edge_orient_y_.clear();
//
//  // Cost computation window
//  this->roi_.empty();
//  
//  // Distance (Chamfer) cost
//  this->chamfer_cost_.clear();
//
//  // edge orientation cost
//  this->edge_orient_cost_.clear();
//
//  // contour orientation cost
//  this->contour_orient_cost_.clear();
//
//  return;
//}

// -----------------------------------------------------------------------------
//: Set edge map and clean up existing pre-computed costs
void dbsks_subpix_ccm::
set_edgemap(const dbdet_edgemap_sptr& edgemap)
{
  if (!edgemap || edgemap == this->edgemap_)
    return;

  this->edgemap_ = edgemap;

  // Prepare storage
  int ni = this->edgemap()->ncols();
  int nj = this->edgemap()->nrows();
  
  this->cost_update_to_date_.resize(ni, nj);
  this->matched_edgel_.resize(ni, nj);
  this->chamfer_cost_.resize(ni, nj);
  this->edge_orient_cost_.resize(ni, nj);
  this->contour_orient_cost_.resize(ni, nj);

  // deallocate memory outside the roi
  for (int i =0; i < ni; ++i)
  {
    for (int j =0; j < nj; ++j)
    {
      this->cost_update_to_date_(i, j) = false;
      this->matched_edgel_(i, j).clear();
      this->chamfer_cost_(i, j).clear();
      this->edge_orient_cost_(i, j).clear();
      this->contour_orient_cost_(i, j).clear();
    }
  }
  return;
}


// -----------------------------------------------------------------------------
//: Radians per angle bin
double dbsks_subpix_ccm::
radians_per_bin() const
{
  return vnl_math::pi / this->nbins_0topi_;
}


//-------------------------------------------------------------------------------
//: Compute bin number of an angle (in radian)
int dbsks_subpix_ccm::
orient_bin(double angle)
{
  int idx = vnl_math::rnd(angle * this->nbins_0topi() / vnl_math::pi);
  idx %= (2*this->nbins_0topi());
  return (idx > 0) ? idx : (idx + 2*this->nbins_0topi()); // force positve index
}


//-------------------------------------------------------------------------------
//: Compute cost components for the whole image
void dbsks_subpix_ccm::
compute(edge_correspondence_method method, bool precompute_contour_orient_cost)
{
  if (!this->edgemap())
    return;

  vgl_box_2d<int > roi(0, this->edgemap()->ncols()-1, 0, this->edgemap()->nrows()-1);
  this->compute(roi, method, precompute_contour_orient_cost);
}




// ------------------------------------------------------------------------------
//: Compute all cost components (internal data) for a given window
void dbsks_subpix_ccm::
compute(const vgl_box_2d<int >& roi, edge_correspondence_method method, 
        bool precompute_contour_orient_cost)
{
  this->roi_ = roi;
  this->precompute_contour_orient_cost_ = precompute_contour_orient_cost;

  // Prepare storage
  int ni = this->edgemap()->ncols();
  int nj = this->edgemap()->nrows();
  
  this->matched_edgel_.resize(ni, nj);
  this->chamfer_cost_.resize(ni, nj);
  this->edge_orient_cost_.resize(ni, nj);
  this->contour_orient_cost_.resize(ni, nj);

  // \todo for now, we recompute costs at every point
  // in the future we may want to re-use costs computed in previous steps.
  this->cost_update_to_date_.fill(false);

  // deallocate memory outside the roi
  for (int i =0; i < ni; ++i)
  {
    for (int j =0; j < nj; ++j)
    {
      
      if (!this->roi_.contains(i, j))
      {
        this->matched_edgel_(i, j).clear();
        this->chamfer_cost_(i, j).clear();
        this->edge_orient_cost_(i, j).clear();
        this->contour_orient_cost_(i, j).clear();
      }
    }
  }

  
  if (method = dbsks_subpix_ccm::USE_CLOSEST_ORIENTED_EDGE)
  {
    // Correspondence considers both distance and orientation difference
    this->compute_ccm_cost_using_closest_oriented_edge();
    this->compute_ortho_dist_from_matched_edgels();
  }
  else
  {
    vcl_cout << "\nERROR: unknown edge correspondence method.\n";
  }
  return;
}



// -----------------------------------------------------------------------------
//: average cost of an ordered set of oriented points, each represented by an
// index triplet (i_x, i_y, i_orient), where 0 <= i_orient < 2*nbins_0topi
float dbsks_subpix_ccm::
f(const vcl_vector<int >& xs, const vcl_vector<int >& ys, const vcl_vector<int >& orient_bins) const
{
  assert(xs.size() == ys.size() && ys.size() == orient_bins.size());

  // need to have sufficient number of sample points to compute cost
  if (xs.size() < 3)
    return 1.0f;

  // Chamfer and edge orientation costs
  float total_chamfer = 0;
  float total_edge_orient = 0;
  
  unsigned npts = xs.size();
  for (unsigned k =0; k < npts; ++k)
  {
    int ix = xs[k];
    int iy = ys[k];
    int iorient = orient_bins[k];

    float chamfer_cost = 1.0f;  // default to max value
    float edge_orient_cost = 1.0f; // default to max value

    if (this->roi_.contains(ix, iy))
    {
      // chamfer cost
      chamfer_cost =  this->chamfer_cost_(ix, iy).empty() ?
        chamfer_cost : this->chamfer_cost_(ix, iy)[iorient];

      // edge orient cost
      edge_orient_cost = this->edge_orient_cost_(ix, iy).empty() ?
        edge_orient_cost : this->edge_orient_cost_(ix, iy)[iorient];
    }
    total_chamfer += chamfer_cost;
    total_edge_orient += edge_orient_cost;
  } // k

  float avg_chamfer = total_chamfer / npts;
  float avg_edge_orient = total_edge_orient / npts;


  // Contour-orientation cost
  // retrieve pre-computed costs if they're readily available
  float avg_contour_orient = 1.0f;
  if (this->precompute_contour_orient_cost_)
  {
    float total_contour_orient = 0;
    for (unsigned k =0; k < (npts-1); ++k)
    {
      int ix = xs[k];
      int iy = ys[k];
      int iorient = orient_bins[k];

      float contour_orient_cost = this->roi_.contains(ix, iy) && !(this->contour_orient_cost_(ix, iy).empty()) ?
        this->contour_orient_cost_(ix, iy)[iorient] : 1.0f;
      
      total_contour_orient += contour_orient_cost;
    } // k
    avg_contour_orient = total_contour_orient / (npts-1);
  }
  // explicitly compute contour orientation cost
  else
  {
    // compute contour-orientation cost directly from given pts
    float total_contour_orient = 0;
    for (unsigned k =0; k < (npts-2); ++k)
    {
      // take a jump of two sample points for stability
      // \todo it is necessary?
      total_contour_orient += this->contour_orient_cost(xs[k], ys[k], orient_bins[k],
        xs[k+2], ys[k+2], orient_bins[k+2]);
    }
    avg_contour_orient = total_contour_orient / (npts-2);
  }

  // final cost is a linear combination of the three components
  float ccm_cost = this->weight_chamfer() * avg_chamfer  +
    this->weight_edge_orient() * avg_edge_orient + 
    this->weight_contour_orient() * avg_contour_orient;

  return ccm_cost;
}










// -----------------------------------------------------------------------------
//: Convert distance between query point and edgel to Chamfer cost [0, 1]
float dbsks_subpix_ccm::
chamfer_cost(double distance) const
{
  double cost = vnl_math::max(0.0, distance-this->distance_tol_near_zero_) / this->distance_threshold_;
  return float(vnl_math::min(cost, 1.0)); // upper-bound = 1
}



// -----------------------------------------------------------------------------
//: Convert angle between query point and edgel to "edge orientation cost" [0, 1]
float dbsks_subpix_ccm::
edge_orient_cost(double signed_angle_diff) const
{
  // modulo the angle difference to [0, pi]
  double diff = vcl_fmod(signed_angle_diff, vnl_math::pi);
  if (diff < 0)
    diff += vnl_math::pi;

  // now convert this angle difference to [0, pi/2]
  diff = vnl_math::min(diff, vnl_math::pi - diff);
           
  // due to discretization, we allow a "tolerance" zone near zero
  double cost = vnl_math::max(diff- this->orient_tol_near_zero_, 0.0) / this->orient_threshold_;

  // clip the cost at 2.0
  return float(vnl_math::min(cost, 1.0));
}




// -----------------------------------------------------------------------------
//: Contour orientation cost
float dbsks_subpix_ccm::
contour_orient_cost(int cur_x, int cur_y, int cur_orient_bin,
    int next_x, int next_y, int next_orient_bin) const
{
  // Best-matched edge of current point
  dbdet_edgel* cur_edgel = 0;
  if (!this->matched_edgel_(cur_x, cur_y).empty())
  {
    cur_edgel = this->matched_edgel_(cur_x, cur_y)[cur_orient_bin];
  }
   
  if ( !cur_edgel )
    return 1.0f;

  // if this point at the ROI border, shouldn't impose contour cost
  if (!this->roi_.contains(next_x, next_y))
  {
    return 0.0f;
  }

  // Best-matched edge of next point
  dbdet_edgel* next_edgel = 0;
  if (!this->matched_edgel_(next_x, next_y).empty())
  {
    next_edgel = this->matched_edgel_(next_x, next_y)[next_orient_bin];
  }

  if (!next_edgel)
  {
    return 1.0f;
  }

  // Compute angle difference between query contour and "image" contour
  double angle_diff = 0;
  vgl_vector_2d<double > v_query(next_x-cur_x, next_y-cur_y);
  if (cur_edgel == next_edgel)
  {
    // use the edge orientation as the contour orientation
    double edge_contour_angle = cur_edgel->tangent;
    vgl_vector_2d<double > v_edge(vcl_cos(edge_contour_angle), vcl_sin(edge_contour_angle));
    
    angle_diff = vnl_math::abs(signed_angle(v_query, v_edge));

    // since edge angle has the ambiguity of +-pi, the angle difference cannot be more than pi/2
    // there are two possible values, differeing by pi
    angle_diff = vnl_math::min(angle_diff, vnl_math::pi - angle_diff);
  }
  else
  {
    // Compute orientation of "contour" along the edges
    vgl_vector_2d<double > v_edge = next_edgel->pt -cur_edgel->pt;
    angle_diff = vnl_math::abs(signed_angle(v_query, v_edge));
  }

  assert(angle_diff >= 0); // make sure nothing weird happens
           
  // due to discretization, we allow a "tolerance" zone equal
  angle_diff = vnl_math::max(angle_diff-this->orient_tol_near_zero_, 0.0);
  double angle_cost = angle_diff / this->orient_threshold_;

  // clip the cost at 1.0 // was 2.0
  return float(vnl_math::min(angle_cost, 1.0));
}


// -----------------------------------------------------------------------------
//: Compute CCM cost as cost to edge closest in both distance and orientation
// assuming memory for internal variables have been properly allocated
bool dbsks_subpix_ccm::
compute_ccm_cost_using_closest_oriented_edge()
{
  this->compute_matched_edgels_using_ocm_cost();
  this->compute_cost_components_from_matched_edgels();
  return true;
}



// -----------------------------------------------------------------------------
//: Matching edgels to image points using Oriented Chamfer Matching cost
// Output is saved to a member variable (matched_edgels_)
bool dbsks_subpix_ccm::
compute_matched_edgels_using_ocm_cost()
{
  double half_width = this->local_window_width()/2;
  int radius = (int) (this->distance_threshold_ + this->distance_tol_near_zero_);
  
  
  int ni = this->edgemap_->ncols();
  int nj = this->edgemap_->nrows();
  int norient = 2*this->nbins_0topi();

  // Pre-computed direction vector for each angle bin
  vcl_vector<vgl_vector_2d<double > > precomputed_tangents(norient);
  for (int i=0; i < norient; ++i)
  {
    double angle = i * this->radians_per_bin();
    precomputed_tangents[i].set(vcl_cos(angle), vcl_sin(angle));
  }
    
  // iterate thru every point in the image and locate the best matched edges
  for (int i =this->roi_.min_x(); i < this->roi_.max_x(); ++i)
  {
    // x-dimension of local window
    int wmin_x = vnl_math::max(i-radius, 0);
    int wmax_x = vnl_math::min(i+radius, ni-1);
    
    for (int j =this->roi_.min_y(); j <this->roi_.max_y(); ++j)
    {
      // y-dimension of local window
      int wmin_y = vnl_math::max(j-radius, 0);
      int wmax_y = vnl_math::min(j+radius, nj-1);

      //a) Check-up: if cost is up-to-date, don't bother to recompute it
      if (this->cost_update_to_date_(i, j))
        continue;

      // coordinate of the query point
      vgl_point_2d<double > query_pt(i, j);

      //b) Collect locations of all the edges in the neighborhood
      vcl_vector<dbdet_edgel* > neighbor_edgels;
      neighbor_edgels.reserve((wmax_x-wmin_x+1)*(wmax_y-wmin_y+1));
      for (int wx = wmin_x; wx <= wmax_x; ++wx)
      {
        for (int wy = wmin_y; wy <= wmax_y; ++wy)
        {
          if (vnl_math::hypot(wx-i, wy-j) > radius)
            continue;

          const vcl_vector<dbdet_edgel* >& edgels = this->edgemap()->cell(wx, wy);
          neighbor_edgels.insert(neighbor_edgels.end(), edgels.begin(), edgels.end());          
        }
      }


      //c) Find best-matched edgel for each orientation of the query point 
      
      // if there is no edge in the neighborhood then there is no corresponding edges
      if (neighbor_edgels.empty())
      {
        this->matched_edgel_(i, j).clear();
      }
      else
      {
        // prepare space to store location of corresponding edges
        this->matched_edgel_(i, j).resize(norient, 0); // 0: non-existing edges

        // vector to store cost of each edgel in the neigborhood
        vnl_vector<float > ocm_cost(neighbor_edgels.size(), 2.0f); // cost is always <= 1.0f

        // Iterate thru all orientations of query point and all edgels
        for (int p = 0; p < norient; ++p)
        {
          double query_angle = p * this->radians_per_bin();
          vgl_vector_2d<double > query_tangent = precomputed_tangents[p];
          
          // Seach among all edges in the neighborhood and minimize Oriented Chamfer Matching cost
          ocm_cost.fill(vnl_numeric_traits<float >::maxval);
          for (unsigned i_edge = 0; i_edge < neighbor_edgels.size(); ++i_edge)
          {
            dbdet_edgel* edgel = neighbor_edgels[i_edge];


            ///////////////////////////////////////////////////////////////
            ////\todo -- REMOVE THIS --- This is a hack to get bypass OCM
            //{
            //  // cost components
            //  float chamfer_cost = this->chamfer_cost(vgl_distance(query_pt, edgel->pt));

            //  // we avoid all edges that are too far away or too different in orientation
            //  if (chamfer_cost >= 1.0)
            //    continue;

            //  // total oriented chamfer cost
            //  ocm_cost[i_edge] = this->weight_chamfer() * chamfer_cost + 0;
            //}
            //continue;
            /////////////////////////////////////////////////////////////////



            // limit the search strictly to edges that are along the "vertical" direction
            // wrt to the local coordinate system of the query (oriented) point
            double local_dx = dot_product(query_tangent, edgel->pt - query_pt);

            if (vnl_math::abs(local_dx) > half_width)
            {
              continue;
            }

            // cost components
            float chamfer_cost = this->chamfer_cost(vgl_distance(query_pt, edgel->pt));
            float edge_orient_cost = this->edge_orient_cost(query_angle - edgel->tangent);

            // we avoid all edges that are too far away or too different in orientation
            if (chamfer_cost >= 1.0 || edge_orient_cost >= 1.0f)
              continue;

            // total oriented chamfer cost
            ocm_cost[i_edge] = this->weight_chamfer() * chamfer_cost + 
              this->weight_edge_orient() * edge_orient_cost;
          }

          unsigned argmin_ocm_cost = ocm_cost.arg_min();
          float min_ocm_cost = ocm_cost[argmin_ocm_cost];

          // if a corresponding edge is found, assign it
          if (min_ocm_cost < vnl_numeric_traits<float >::maxval )
          {
            this->matched_edgel_(i, j)[p] = neighbor_edgels[argmin_ocm_cost];
          }
        } // p
      } // if there are some edges in the neighborhood
    } // j
  } // i

  return true;
}







// -----------------------------------------------------------------------------
//: Compute the 3 components (chamfer, edge orient, contour orient) from matched_edgels
// Results are saved in 3 member variables
bool dbsks_subpix_ccm::
compute_cost_components_from_matched_edgels()
{
  // Distance parameter, used to estimate contour orientation cost
  float neighborhood_radius = this->sampling_step_for_contour_orient_cost_; //3.0f; // \todo should be set to sampling rate of contour
  

  for (int i =this->roi_.min_x(); i < this->roi_.max_x(); ++i)
  {
    for (int j =this->roi_.min_y(); j <this->roi_.max_y(); ++j)
    {
      // no need to compute if the cost is up-to-date
      if (this->cost_update_to_date_(i, j))
        continue;

      // cost vectors when query point is at (i, j)
      vnl_vector<float >& f_chamfer = this->chamfer_cost_(i, j);
      vnl_vector<float >& f_edge_orient = this->edge_orient_cost_(i, j);
      vnl_vector<float >& f_contour_orient = this->contour_orient_cost_(i, j);

      vcl_vector<dbdet_edgel* >& matched_edgels = this->matched_edgel_(i, j);


      // a) if there is no corresponding edges then all costs for all orientation are maximum
      if (matched_edgels.empty())
      {
        f_chamfer.clear();
        f_edge_orient.clear();
        f_contour_orient.clear();
        continue;
      }

      // b) Compute costs based on matched edgels
      int num_orients = matched_edgels.size();

      // compute chamfer and edge orient cost
      f_chamfer.set_size(num_orients);
      f_chamfer.fill(1.0f);

      f_edge_orient.set_size(num_orients);
      f_edge_orient.fill(1.0f);
      
      vgl_point_2d<double > query_pt(i, j);
      for (int p =0; p < num_orients; ++p)
      {
        double query_angle = p * this->radians_per_bin();
        dbdet_edgel* edgel = matched_edgels[p];

        if (!edgel)
          continue;

        // Chamfer cost : normalized distance
        f_chamfer[p] = this->chamfer_cost(vgl_distance(query_pt, edgel->pt));

        // edge orientation cost : normalized orientation difference  
        f_edge_orient[p] = this->edge_orient_cost(query_angle - edgel->tangent);
      } // p


      // Only compute contour orientation cost if flag is on to save memory and time
      if (this->precompute_contour_orient_cost_)
      {
        // compute contour orientation cost
        f_contour_orient.set_size(num_orients);
        f_contour_orient.fill(1.0f);

        for (int p =0; p < num_orients; ++p)
        {
          double query_angle = p * this->radians_per_bin();

          // Coordinate of next point along contour's tangent
          int next_point_x = vnl_math::rnd(i + vcl_cos(query_angle) * neighborhood_radius);
          int next_point_y = vnl_math::rnd(j + vcl_sin(query_angle) * neighborhood_radius);

          // assuming orientation doesn't change
          // \todo alternatively, we can average the cost for various next_point_p,
          // e.g., p, p-1, p+1. For simplicity, we are NOT doing it here.
          int next_point_p = p; 

          f_contour_orient[p] = this->contour_orient_cost(i, j, p, 
            next_point_x, next_point_y, next_point_p);
        } // p
      } // if precompute_contour_orient_cost
    } // j
  } // i
  return true;
}







//------------------------------------------------------------------------------
//: Compute orthgonal distance to matched edgels
bool dbsks_subpix_ccm::
compute_ortho_dist_from_matched_edgels()
{
  this->ortho_dist_to_matched_edgel_.resize(this->matched_edgel_.rows(), this->matched_edgel_.cols());
  this->ortho_dist_to_matched_edgel_.fill(vnl_vector<float >());

  for (int i =this->roi_.min_x(); i < this->roi_.max_x(); ++i)
  {
    for (int j =this->roi_.min_y(); j <this->roi_.max_y(); ++j)
    {
      // cost vectors when query point is at (i, j)
      vnl_vector<float >& d_ortho = this->ortho_dist_to_matched_edgel_(i, j);
      
      // list of matched edgels for each orientation
      vcl_vector<dbdet_edgel* >& matched_edgels = this->matched_edgel_(i, j);

      // a) if there is no corresponding edges then all costs for all orientation are maximum
      if (matched_edgels.empty())
      {
        d_ortho.clear();
        continue;
      }

      // b) Compute orthogonal distance to matched edgels
      int num_orients = matched_edgels.size();

      // set default value
      d_ortho.set_size(num_orients);
      d_ortho.fill(vnl_numeric_traits<float >::maxval);

      vgl_point_2d<double > query_pt(i, j);
      for (int p =0; p < num_orients; ++p)
      {
        double query_angle = p * this->radians_per_bin();
        vgl_vector_2d<double > query_normal(vcl_cos(query_angle+vnl_math::pi_over_2), 
                                            vcl_sin(query_angle+vnl_math::pi_over_2));
        dbdet_edgel* edgel = matched_edgels[p];

        if (!edgel)
          continue;

        // orthogonal distance
        d_ortho[p] = float(inner_product(edgel->pt - query_pt, query_normal));        
      } // p
    } // j
  } // i
  return true;
}















//------------------------------------------------------------------------------
//: Estimate lower bound value of ccm cost for the specified ROI
float dbsks_subpix_ccm::
compute_f_lower_bound() const
{
  // Estimate lower bound of the cost by computing only chamfer and edge orientation
  // cost, ignoring the contour-orientation cost componenent
  float fmin = 1.0f;

  int num_orients = this->nbins_0topi()*2;
  for (int i =this->roi_.min_x(); i < this->roi_.max_x(); ++i)
  {
    for (int j =this->roi_.min_y(); j <this->roi_.max_y(); ++j)
    {
      // cost vectors when query point is at (i, j)
      const vnl_vector<float >& f_chamfer = this->chamfer_cost_(i, j);
      const vnl_vector<float >& f_edge_orient = this->edge_orient_cost_(i, j);
      assert(f_chamfer.size() == f_edge_orient.size());

      // Oriented-Chamfer-Matching is a lower bound of of CCM
      // because it has only two of the three components of CCM
      vnl_vector<float > f_ocm = this->weight_chamfer()*f_chamfer + 
        this->weight_edge_orient() * f_edge_orient;
      if (f_ocm.empty())
        continue;
      fmin = vnl_math::min(fmin, f_ocm.min_value());
    } // j
  } // i

  return fmin;
}





//------------------------------------------------------------------------------
//: Estimate upper bound valu of ccm cost for the specified ROI
float dbsks_subpix_ccm::
compute_f_upper_bound() const
{
  // Estimate lower bound of the cost by computing only chamfer and edge orientation
  // cost, ignoring the contour-orientation cost componenent
  float fmax = 0.0f;

  int num_orients = this->nbins_0topi()*2;
  for (int i =this->roi_.min_x(); i < this->roi_.max_x(); ++i)
  {
    for (int j =this->roi_.min_y(); j <this->roi_.max_y(); ++j)
    {
      // cost vectors when query point is at (i, j)
      const vnl_vector<float >& f_chamfer = this->chamfer_cost_(i, j);
      const vnl_vector<float >& f_edge_orient = this->edge_orient_cost_(i, j);
      assert(f_chamfer.size() == f_edge_orient.size());

      // upper-bounding ccm cost by making contour_orient cost = 1.0f
      vnl_vector<float > f_ccm = this->weight_chamfer()*f_chamfer + 
          this->weight_edge_orient() * f_edge_orient + 
          this->weight_contour_orient() * 1.0f; // max f_contour_orient = 1.0f;

      if (f_ccm.empty())
        continue;

      fmax = vnl_math::max(fmax, f_ccm.max_value());
    } // j
  } // i

  return fmax;
}






//------------------------------------------------------------------------------
//: Retrieve match edgels given position and orientation of contour piont
dbdet_edgel* dbsks_subpix_ccm::
matched_edgel(int ix, int iy, int ibin) const
{
  if (!this->roi_.contains(ix, iy))
    return 0;

  const vcl_vector<dbdet_edgel* >& matched_edgels = this->matched_edgel_(ix, iy);

  if (matched_edgels.empty())
  {
    return 0;
  }

  if (ibin < 0 || ibin >= int(matched_edgels.size()) )
    return 0;

  return matched_edgels[ibin];
}















