// This is file seg/dbsks/dbsks_wcm.cxx

//:
// \file

#include "dbsks_wcm.h"
#include <vnl/vnl_math.h>
#include <vsol/vsol_point_2d.h>
#include <vcl_set.h>
#include <vgl/vgl_distance.h>

// ============================================================================
// dbsks_wcm
// ============================================================================

// -----------------------------------------------------------------------------
//: Set the edge label image from a set of polylines
void dbsks_wcm::
set_edge_labels(const vcl_vector<vsol_polyline_2d_sptr >& contours)
{
  this->edge_label_.set_size(this->edge_strength_.ni(), this->edge_strength_.nj());
  this->edge_label_.fill(0);

  unsigned label = 0;
  for (unsigned i =0; i < contours.size(); ++i)
  {
    ++label;
    vsol_polyline_2d_sptr contour = contours[i];
    this->map_label2contour_.insert(vcl_make_pair(label, contour));

    // write to "edge_label" image
    for (unsigned m =0; m < contour->size(); ++m)
    {
      vsol_point_2d_sptr pt = contour->vertex(m);
      int ix = vnl_math_rnd(pt->x());
      int iy = vnl_math_rnd(pt->y());
      if (this->edge_label_.in_range(ix, iy))
      {
        this->edge_label_(ix, iy) =label;
      }
    }
  }

  return;
}







// -----------------------------------------------------------------------------
//: Image cost of a contour (an ordered set of oriented points) given the shape's 
// complete boundary
float dbsks_wcm::
f_whole_contour_v1(const vcl_vector<int >& x, const vcl_vector<int >& y, 
                const vcl_vector<int >& orient_channel, 
                double contour_length, const vgl_polygon<double >& shape_boundary)
{
  // Find labels of edge points corresponding to the given contour
  vcl_set<unsigned > unique_labels;
  for (unsigned i =0; i < x.size(); ++i)
  {
    if (!this->edge_strength_.in_range(x[i], y[i]))
    {
      continue;
    }

    vnl_vector<int >& edge_ix = this->matched_edge_ix_(x[i], y[i]);
    vnl_vector<int >& edge_iy = this->matched_edge_iy_(x[i], y[i]);

    // Move on if there is no corresponding edge
    if (edge_ix.empty())
      continue;

    // Retrieve the edge position
    int ex = edge_ix[orient_channel[i]];
    int ey = edge_iy[orient_channel[i]];

    // Retrieve the label of the edge
    if (this->edge_label_.in_range(ex, ey) && this->edge_label_(ex, ey) != 0)
    {
      unique_labels.insert(this->edge_label_(ex, ey));
    }
  }

  // count the number of edge points which are too far away from the shape and 
  // have labels "labels"
  int num_edges = 0;
  int num_edges_too_far = 0;
  for (vcl_set<unsigned >::iterator iter = unique_labels.begin(); 
    iter != unique_labels.end(); ++iter)
  {
    // retrieve the contour associated with the given label
    unsigned label = *iter;
    vcl_map<unsigned, vsol_polyline_2d_sptr >::iterator mit = 
      this->map_label2contour_.find(label);
    if (mit == this->map_label2contour_.end())
    {
      continue;
    }

    // iterate thru the points in the contour, count the number of points
    // that are too far away
    vsol_polyline_2d_sptr image_contour = mit->second;
    num_edges += image_contour->size();

    for (unsigned i =0; i < image_contour->size(); ++i)
    {
      vsol_point_2d_sptr pt = image_contour->vertex(i);
      double dist = vgl_distance(pt->get_p(), shape_boundary);
      num_edges_too_far += (dist > (this->tol_near_zero_+this->distance_threshold_)) ? 1 : 0;
    }
  }

  // ratio between the number of edges too far away and total number of edges
  //float unmatched_cost = float (num_edges_too_far / (contour_length + num_edges_too_far)); // num_edges;

  float unmatched_cost = float (num_edges_too_far / contour_length); // num_edges;

  // clip the cost at 2
  //unmatched_cost = vnl_math_min(unmatched_cost, 1.0f);

  // cost without considering the unmatched edges
  float cost1 = this->f(x, y, orient_channel);

  // Final cost
  float final_cost = cost1 + this->weight_unmatched() * unmatched_cost;

  //num_edges_unmatched = num_edges_too_far;
  return final_cost;
}


// -----------------------------------------------------------------------------
//: Image cost of a contour (an ordered set of oriented points) given the shape's 
// complete boundary
float dbsks_wcm::
f_whole_contour(const vcl_vector<int >& x, const vcl_vector<int >& y, 
                const vcl_vector<int >& orient_channel, 
                double contour_length, const vgl_polygon<double >& shape_boundary)
{
  if (x.size() < 5)
  {
    return 1.0f;
  }

  // cost of unmatched edges
  float unmatched_cost = this->cost_unmatched_edges(x, y, orient_channel, contour_length, shape_boundary);
  
  // cost of switching contours
  float switching_contour_cost = this->cost_switching_linked_contours(x, y, orient_channel);
   
  // cost without considering the unmatched edges
  float ccm_cost = this->f(x, y, orient_channel);


  // Final cost
  float final_cost = ccm_cost + 
    this->weight_switching_contours() * switching_contour_cost + 
    this->weight_unmatched() * unmatched_cost;
  return final_cost;
}



// -----------------------------------------------------------------------------
//: Cost of unmatched edges
float dbsks_wcm::
cost_unmatched_edges(const vcl_vector<int >& x, const vcl_vector<int >& y, 
                     const vcl_vector<int >& orient_channel, double contour_length,
                     const vgl_polygon<double >& shape_boundary)
{
  vcl_vector<vgl_point_2d<double > >query_pts;
  query_pts.reserve(x.size());
  for (unsigned i =0; i < x.size(); ++i)
  {
    query_pts.push_back(vgl_point_2d<double >(x[i], y[i]));
  }
  vgl_polygon<double > query_polygon(query_pts);


  // Find labels of edge points corresponding to the given contour
  vcl_set<unsigned > unique_labels;
  for (unsigned i =0; i < x.size(); ++i)
  {
    // if the point is out of range, move on
    if (!this->edge_strength_.in_range(x[i], y[i]))
    {
      continue;
    }
    vnl_vector<int >& edge_ix = this->matched_edge_ix_(x[i], y[i]);
    vnl_vector<int >& edge_iy = this->matched_edge_iy_(x[i], y[i]);

    // Move on if there is no corresponding edge
    if (edge_ix.empty())
      continue;

    // Retrieve the edge position
    int ex = edge_ix[orient_channel[i]];
    int ey = edge_iy[orient_channel[i]];

    // Retrieve the label of the edge
    if (this->edge_label_.in_range(ex, ey) && this->edge_label_(ex, ey) != 0)
    {
      unique_labels.insert(this->edge_label_(ex, ey));
    }
  }

  // Compute the number of mismatched points for each edge contour matched to the query contour
  double total_num_unmatched = 0;
  for (vcl_set<unsigned >::iterator iter = unique_labels.begin(); 
    iter != unique_labels.end(); ++iter)
  {
    // retrieve the contour associated with the given label
    unsigned label = *iter;

    vcl_map<unsigned, vsol_polyline_2d_sptr >::iterator mit =
      this->map_label2contour_.find(label);
    if (mit == this->map_label2contour_.end())
    {
      continue;
    }
    vsol_polyline_2d_sptr image_contour = mit->second;

    // Count the number of points too far, near, and matched
    int num_near = 0;
    int num_far = 0;
    int num_matched = 0;
    double near_distance = this->tol_near_zero_+this->distance_threshold_;

    for (unsigned i =0; i < image_contour->size(); ++i)
    {
      vsol_point_2d_sptr pt = image_contour->vertex(i);
      double dist_to_boundary = vgl_distance(pt->get_p(), shape_boundary);
      if (dist_to_boundary > near_distance)
      {
        ++num_far;
      }
      else
      {
        ++num_near;
        double dist_to_query = vgl_distance(pt->get_p(), query_polygon, false);

        if (dist_to_query <= (dist_to_boundary+1)) // this edge is matched to the query contour
        {
          ++num_matched;
        }
      }
    }

    // mismatched points are assigned proportionally to the query contour by length
    double num_unmatched = (num_near > 0) ? (1.0*num_far*num_matched/num_near) : num_far;
    total_num_unmatched += num_unmatched;
  }
  float unmatched_cost = float (total_num_unmatched / contour_length);

  return unmatched_cost;
}





// -----------------------------------------------------------------------------
//: Cost due to switching between linked contours (to avoid spurious edges)
float dbsks_wcm::
cost_switching_linked_contours(const vcl_vector<int >& x, const vcl_vector<int >& y, 
                               const vcl_vector<int >& orient_channel)
{
  // no contour switching cost when too few points
  if (x.size() < 5)
    return 0.0f;

  //1) collect the edge labels for every point
  static vcl_vector<unsigned > edge_labels;
  edge_labels.resize(x.size(), 0); //>> default = 0: no edge

  int npts = x.size();
  for (int m =0; m < npts; ++m)
  {
    int ix = x[m];
    int iy = y[m];
    int ichannel = orient_channel[m];

    // assign edge label when it is available
    if (this->in_image_range(ix, iy))
    {
      if (!this->matched_edge_ix_(ix, iy).empty() && 
        !this->matched_edge_iy_(ix, iy).empty())
      {
        int edge_x = this->matched_edge_ix_(ix, iy)[ichannel];
        int edge_y = this->matched_edge_iy_(ix, iy)[ichannel];

        // if this is a real edge then extract the label
        if (this->in_image_range(edge_x, edge_y))
        {
          edge_labels[m] = this->edge_label_(edge_x, edge_y);
        }
      }
    }
  }

  //2) Count number of times consecutive points switch edge label
  int num_label_switches = 0;
  unsigned prev_label = edge_labels[0];
  for (unsigned m =1; m < edge_labels.size(); ++m)
  {
    num_label_switches += (edge_labels[m] == prev_label) ? 0 : 1;
    prev_label = edge_labels[m];
  }

  float label_switching_cost = float(num_label_switches) / (edge_labels.size()-1);
  return label_switching_cost;
}











