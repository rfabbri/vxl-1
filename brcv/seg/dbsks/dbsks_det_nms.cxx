// This is dbsks/pro/dbsks_det_nms.cxx

//:
// \file

#include "dbsks_det_nms.h"


#include <dbsks/dbsks_det_desc_xgraph.h>

#include <dbsksp/algo/dbsksp_trace_boundary.h>

#include <bsol/bsol_algs.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <vcl_algorithm.h>
#include <vgl/vgl_polygon_scan_iterator.h>
#include <vgl/vgl_clip.h>
#include <vgl/vgl_area.h>






// -----------------------------------------------------------------------------
//: Non-max supression on list of detection based the polygon of the detected xgraphs
// Output is a list of detections that survive, sorted in descending confidence level.
bool dbsks_det_nms_using_polygon(const vcl_vector<dbsks_det_desc_xgraph_sptr >& input_det_list,
                                 vcl_vector<dbsks_det_desc_xgraph_sptr >& output_det_list,
                                 double min_overlap_ratio)
{
  // first thing first
  output_det_list.clear();

  // Create a sorted copy of the input detection list
  vcl_vector<dbsks_det_desc_xgraph_sptr > sorted_det_list = input_det_list;
  vcl_sort(sorted_det_list.begin(), sorted_det_list.end(), dbsks_decreasing_confidence);

  ///////////////////////////////////
  //// \debug - no nnms - pass everything
  //output_det_list = sorted_det_list;
  //return true;
  ///////////////////////////////////


  // Start with the detection with highest confidence and move down the list.
  // Remove all detections whose (polygonal) boundary overlap with another
  // detection with higher confidence level

  output_det_list.reserve(sorted_det_list.size());
  // \debug text
  vcl_cout << "\nProcessing detection ";
  for (unsigned i =0; i < sorted_det_list.size(); ++i)
  {
    vcl_cout << " " << i;

    dbsks_det_desc_xgraph_sptr query_det = sorted_det_list[i];

    // boundary as a polygon
    double approx_ds = 1.0;
    vsol_polygon_2d_sptr query_poly = dbsksp_trace_xgraph_boundary_as_polygon(query_det->xgraph(), approx_ds);
    
    // retrace the boundary to roughly 50 points
    query_poly = dbsksp_trace_xgraph_boundary_as_polygon(query_det->xgraph(), 
      double(query_poly->size()*approx_ds) / 50);
    vgl_polygon<double > query_vgl_poly = bsol_algs::vgl_from_poly(query_poly);

    // Compare with detection that survive the detection
    bool pass_nonmax_test = true;
    for (unsigned j =0; j < output_det_list.size(); ++j)
    {
      dbsks_det_desc_xgraph_sptr saved_det = output_det_list[j];
      vsol_polygon_2d_sptr saved_poly = dbsksp_trace_xgraph_boundary_as_polygon(saved_det->xgraph(), approx_ds);
      
      // retrace the boundary to roughly 50 points
      saved_poly = dbsksp_trace_xgraph_boundary_as_polygon(saved_det->xgraph(), 
        double(saved_poly->size()*approx_ds) / 50);

      // Find overlapping ratio between the polygon boundary of the two shapes
      
      // use vgl_clip to find intersection of two polygon
      vgl_polygon<double > saved_vgl_poly = bsol_algs::vgl_from_poly(saved_poly);
      vgl_polygon<double > intersect_vgl_poly = vgl_clip(query_vgl_poly, saved_vgl_poly, vgl_clip_type_intersect);


      // overlap is the ratio between intersection area and union area
      double overlap = 0;
      double area_intersect = vgl_area(intersect_vgl_poly);
      if (area_intersect > 0)
      {
        double area_union = query_poly->area()+saved_poly->area() - area_intersect;
        
        //// Nhon, 2/4/09: maybe it is better to divide by the query area rather than union
        //double area_union = query_poly->area();

        // avoid problem of real-value union-area vs. interger-value intersection-count
        if (area_union < area_intersect) 
          area_union = area_intersect;

        overlap = area_intersect / area_union;
      }

      if (overlap >= min_overlap_ratio)
      {
        pass_nonmax_test = false;
        break;
      }
    }

    // Only add a detection if it passes the nonmax-test, i.e., it does not
    // overlap with any other detection with higher confidence level.
    if (pass_nonmax_test)
    {
      output_det_list.push_back(query_det);
    }

    // \debug
    if (pass_nonmax_test)
    {
      vcl_cout << "[Y]";
    }
    else
    {
      vcl_cout << "[N]";
    }
  }
  vcl_cout << vcl_endl;
  return true;
}


