// This is file seg/dbsks/dbsks_xshock_utils.cxx

//:
// \file

#include "dbsks_xshock_utils.h"
#include <dbsksp/dbsksp_xshock_graph.h>
#include <vsol/vsol_polyline_2d.h>
#include <vsol/vsol_point_2d.h>
#include <dbnl/dbnl_angle.h>
#include <vnl/vnl_math.h>
#include <vcl_utility.h>
#include <vcl_algorithm.h>
#include <vgl/vgl_distance.h>
//// -----------------------------------------------------------------------------
////: Update descriptors around a degree-2 node, given the descriptor of the child edge
//void dbsks_update_degree2_node(const dbsksp_xshock_node_sptr& xv, 
//                               const dbsksp_xshock_edge_sptr& xe_child, 
//                               const dbsksp_xshock_node_descriptor& xdesc)
//{
//  assert(xv->degree() == 2);
//  assert(xe_child->is_vertex(xv));
//
//  // get its descriptor
//  dbsksp_xshock_node_descriptor* xdesc_c = xv->descriptor(xe_child);
//  xdesc_c->phi_ = xdesc.phi_;
//  xdesc_c->psi_ = xdesc.psi_;
//
//  // the opposite edge (parent edge)
//  dbsksp_xshock_edge_sptr xe_p = *xv->edges_begin();
//  if (xe_p == xe_child)
//    xe_p = *(++xv->edges_begin());
//
//  dbsksp_xshock_node_descriptor* xdesc_p = xv->descriptor(xe_p);
//  xdesc_p->phi_ = vnl_math::pi - xdesc_c->phi_;
//  xdesc_p->psi_ = vnl_math::pi + xdesc_c->psi_;
//
//  // set location and radius (this affects all the descriptors)
//  xv->set_pt(xdesc.pt());
//  xv->set_radius(xdesc.radius_);
//
//  return;
//}









//// -----------------------------------------------------------------------------
////: Update descriptors around a degree-3 node
//void dbsks_update_degree3_node(const dbsksp_xshock_node_sptr& xv, 
//                               const dbsksp_xshock_edge_sptr& xe_parent,
//                               const dbsksp_xshock_edge_sptr& xe_child1,
//                               const dbsksp_xshock_edge_sptr& xe_child2,
//                               const dbsksp_xshock_node_descriptor& xdesc_parent,
//                               double phi_child1)
//{
//  assert(xv->degree() == 3);
//  assert(xe_parent->is_vertex(xv));
//  assert(xe_child1->is_vertex(xv));
//  assert(xe_child2->is_vertex(xv));
//
//  // set descriptor of parent
//  dbsksp_xshock_node_descriptor* xdesc_p = xv->descriptor(xe_parent);
//  xdesc_p->phi_ = xdesc_parent.phi_;
//  xdesc_p->psi_ = xdesc_parent.psi_;
//
//  // set descriptor of child1
//  dbsksp_xshock_node_descriptor* xdesc_c1 = xv->descriptor(xe_child1);
//  xdesc_c1->phi_ = phi_child1;
//  xdesc_c1->psi_ = xdesc_p->psi_ + xdesc_p->phi_ + xdesc_c1->phi_;
//
//  // set descriptor of child2
//  dbsksp_xshock_node_descriptor* xdesc_c2 = xv->descriptor(xe_child2);
//  xdesc_c2->phi_ = vnl_math::pi - xdesc_p->phi_ - xdesc_c1->phi_;
//  xdesc_c2->psi_ = xdesc_c1->psi_ + xdesc_c1->phi_ + xdesc_c2->phi_;
//
//  // set location and radius (this affects all the descriptors)
//  xv->set_pt(xdesc_parent.pt());
//  xv->set_radius(xdesc_parent.radius_);
//
//  return;
//}
//










// -----------------------------------------------------------------------------
//: Scale the edgemap, edgeorient, and contour map so that the xgraph scale is
// about the "standard scale"
bool dbsks_adjust_to_standard_scale(double standard_scale,
                               double cur_scale,
                               const vil_image_view<float >& cur_edgemap,
                               const vcl_vector<vsol_polyline_2d_sptr >& cur_polyline_list,
                               const vil_image_view<float >& cur_edge_angle,
                               double& scaled_up_factor,
                               vil_image_view<float >& new_edgemap,
                               vcl_vector<vsol_polyline_2d_sptr >& new_polyline_list,
                               vil_image_view<float >& new_edge_angle)
{

  return dbsks_adjust_to_standard_scale_dense(standard_scale, cur_scale, 
    cur_edgemap, cur_polyline_list, cur_edge_angle, scaled_up_factor, 
    new_edgemap, new_polyline_list, new_edge_angle);
}







// -----------------------------------------------------------------------------
//: Scale the edgemap, edgeorient, and contour map so that the xgraph scale is
// about the "standard scale"
bool dbsks_adjust_to_standard_scale_sparse(double standard_scale,
                               double cur_scale,
                               const vil_image_view<float >& cur_edgemap,
                               const vcl_vector<vsol_polyline_2d_sptr >& cur_polyline_list,
                               const vil_image_view<float >& cur_edge_angle,
                               double& scaled_up_factor,
                               vil_image_view<float >& new_edgemap,
                               vcl_vector<vsol_polyline_2d_sptr >& new_polyline_list,
                               vil_image_view<float >& new_edge_angle)
{
  int scaling_up = vnl_math::rnd(standard_scale / cur_scale);

  if (scaling_up <= 1)
  {
    scaling_up = 1;
    new_edgemap = cur_edgemap;
    new_edge_angle = cur_edge_angle;
    new_polyline_list = cur_polyline_list;
  }
  else 
  {
    // scale up the edgemap so that we don't have to scale down the graph
    int new_ni = cur_edgemap.ni() * scaling_up;
    int new_nj = cur_edgemap.nj() * scaling_up;

    new_edgemap.set_size(new_ni, new_nj);
    new_edgemap.fill(0.0f);

    new_edge_angle.set_size(new_ni, new_nj);
    new_edge_angle.fill(0.0f);

    new_polyline_list.reserve(cur_polyline_list.size());
    for (unsigned i =0; i < cur_polyline_list.size(); ++i)
    {
      vsol_polyline_2d_sptr poly = cur_polyline_list[i];
      vsol_polyline_2d_sptr new_poly = new vsol_polyline_2d(*poly);
      for (unsigned m =0; m < poly->size(); ++m)
      {
        vsol_point_2d_sptr pt = poly->vertex(m);
        int px = vnl_math::rnd(pt->x());
        int py = vnl_math::rnd(pt->y());

        int new_px = px * scaling_up;
        int new_py = py * scaling_up;

        new_edgemap(new_px, new_py) = cur_edgemap(px, py);
        new_edge_angle(new_px, new_py) = cur_edge_angle(px, py);
        new_poly->vertex(m)->set_x(new_px);
        new_poly->vertex(m)->set_y(new_py);
      }
      new_polyline_list.push_back(new_poly);
    }
  }

  scaled_up_factor = scaling_up;
  return true;
}





// -----------------------------------------------------------------------------
//: Scale the edgemap, edgeorient, and contour map so that the xgraph scale is
// about the "standard scale"
bool dbsks_adjust_to_standard_scale_dense(double standard_scale,
                               double cur_scale,
                               const vil_image_view<float >& cur_edgemap,
                               const vcl_vector<vsol_polyline_2d_sptr >& cur_polyline_list,
                               const vil_image_view<float >& cur_edge_angle,
                               double& scaled_up_factor,
                               vil_image_view<float >& new_edgemap,
                               vcl_vector<vsol_polyline_2d_sptr >& new_polyline_list,
                               vil_image_view<float >& new_edge_angle)
{
  // assumption:
  // all linked contours (in polyline_list) have pixel-accuracy only
  // the scaling factor will be integer

  int ratio = vnl_math::rnd(standard_scale / cur_scale);

  // for now, we keep everything the same for large scale objects
  // \todo handle this case properly (scale down the edgemaps)
  if (ratio <= 1)
  {
    ratio = 1;
    new_edgemap = cur_edgemap;
    new_edge_angle = cur_edge_angle;
    new_polyline_list = cur_polyline_list;
  }
  else 
  {
    // scale up the edgemap so that we don't have to scale down the graph
    int new_ni = cur_edgemap.ni() * ratio;
    int new_nj = cur_edgemap.nj() * ratio;

    new_edgemap.set_size(new_ni, new_nj);
    new_edgemap.fill(0.0f);

    new_edge_angle.set_size(new_ni, new_nj);
    new_edge_angle.fill(0.0f);

    new_polyline_list.reserve(cur_polyline_list.size());
    for (unsigned i =0; i < cur_polyline_list.size(); ++i)
    {
      vsol_polyline_2d_sptr poly = cur_polyline_list[i];
      vsol_polyline_2d_sptr new_poly = new vsol_polyline_2d();

      // first point
      int px0 = vnl_math::rnd(poly->vertex(0)->x());
      int py0 = vnl_math::rnd(poly->vertex(0)->y());
      int new_px0 = ratio * px0;
      int new_py0 = ratio * py0;
      new_poly->add_vertex(new vsol_point_2d(new_px0, new_py0));
      new_edgemap(new_px0, new_py0) = cur_edgemap(px0, py0);
      new_edge_angle(new_px0, new_py0) = cur_edge_angle(px0, py0);

      // the remaining points / line segments
      // we will scale each line segment between 2 vertices up and fill in the space in between
      // the number of points to fill in between the vertex points = (scaling up factor - 1)
      // e.g. (1, 1)--(2,3) ---(scaled up by 2)----> (2, 2), (3, 4), (4, 6)
      vgl_point_2d<int > prev_pt(px0, py0);
      
      for (unsigned m =1; m < poly->size(); ++m)
      {
        vsol_point_2d_sptr vertex = poly->vertex(m);
        vgl_point_2d<int > pt(vnl_math::rnd(vertex->x()), vnl_math::rnd(vertex->y()));
        float pixel_val = cur_edgemap(pt.x(), pt.y());
        float angle = cur_edge_angle(pt.x(), pt.y());
        
        // info of previous vertex
        vgl_point_2d<int > prev_pt_scaled(ratio * prev_pt.x(), ratio * prev_pt.y());
        float prev_pixel_val = cur_edgemap(prev_pt.x(), prev_pt.y());
        float prev_angle = cur_edge_angle(prev_pt.x(), prev_pt.y());
        
        // k = 0 has been computed in previous step
        vgl_vector_2d<int > v = pt - prev_pt;
        for (int k = 1; k <= ratio; ++k)
        {
          vgl_point_2d<int > new_pt = prev_pt_scaled + k * v;
          new_poly->add_vertex(new vsol_point_2d(new_pt.x(), new_pt.y()));
          
          float lambda = float(k) / ratio;
          float new_pixel_val = (1-lambda) * prev_pixel_val + lambda * pixel_val;
          float new_angle = (1-lambda) * prev_angle + lambda * angle;

          new_edgemap(new_pt.x(), new_pt.y()) = new_pixel_val;
          new_edge_angle(new_pt.x(), new_pt.y()) = new_angle;
        }

        // update location of previous point for next iteration
        prev_pt = pt;
      }
      new_polyline_list.push_back(new_poly);
    }
  }

  scaled_up_factor = ratio;
  return true;
}






// -----------------------------------------------------------------------------
//: Scale the edgemap and edgeorient so that the xgraph scale is
// about the "standard scale"
bool dbsks_adjust_to_standard_scale(double standard_scale,
                               double cur_scale,
                               const vil_image_view<float >& cur_edgemap,
                               const vil_image_view<float >& cur_edge_angle,
                               double& scaled_up_factor,
                               vil_image_view<float >& new_edgemap,
                               vil_image_view<float >& new_edge_angle)
{
  int scaling_up = vnl_math::rnd(standard_scale / cur_scale);

  if (scaling_up <= 1)
  {
    scaling_up = 1;
    new_edgemap = cur_edgemap;
    new_edge_angle = cur_edge_angle;
  }
  else 
  {
    // scale up the edgemap so that we don't have to scale down the graph
    int new_ni = cur_edgemap.ni() * scaling_up;
    int new_nj = cur_edgemap.nj() * scaling_up;

    new_edgemap.set_size(new_ni, new_nj);
    new_edgemap.fill(0.0f);

    new_edge_angle.set_size(new_ni, new_nj);
    new_edge_angle.fill(0.0f);

    for (unsigned i =0; i < cur_edgemap.ni(); ++i)
    {
      for (unsigned j =0; j < cur_edgemap.nj(); ++j)
      {
        int px = i;
        int py = j;

        int new_px = px * scaling_up;
        int new_py = py * scaling_up;

        new_edgemap(new_px, new_py) = cur_edgemap(px, py);
        new_edge_angle(new_px, new_py) = cur_edge_angle(px, py);
      }
    }
  }

  scaled_up_factor = scaling_up;
  return true;
}









//------------------------------------------------------------------------------
//: min-max of a set of angles, noting angles are circular
vnl_vector<double > dbsks_compute_angle_minmax(const vnl_vector<double >& angles,
                          double& min_angle, double& max_angle)
{
  // since angles are circular, we define min and as angles such that the range
  // between them covers all "angles" and is minimum.

  assert(!angles.empty());

  // create a copy
  vnl_vector<double > x = angles;

  // convert to 0 to 2pi
  for (unsigned i =0; i < x.size(); ++i)
  {
    x[i] = dbnl_angle_0to2pi(x[i]);
  }

  // sort the angles
  vcl_sort(x.begin(), x.end());

  // find the maximum difference between adjacent elements
  vnl_vector<double > diff(x.size());
  for (unsigned i = 1; i < x.size(); ++i)
  {
    diff[i] = x[i] - x[i-1];
  }
  assert(!x.empty());
  diff[0] = x[0] - (x[x.size()-1] - 2*vnl_math::pi);

  // position of maximum difference
  unsigned max_idx = diff.arg_max();
  double max_diff = diff[max_idx];
  
  // set min and max
  min_angle = x[max_idx];
  max_angle = min_angle + (2*vnl_math::pi - max_diff);

  // re-arrange x so that it starts with min and ends with max
  x[0] = min_angle;
  for (unsigned i = max_idx+1; i < x.size(); ++i)
  {
    int ii= i-max_idx;
    x[ii] = x[ii-1] + diff[i];
  }

  for (unsigned i = 1; i < x.size(); ++i)
  {
    int pos = (i + max_idx) % (x.size());
    x[i] = x[i-1] + diff[pos];
  }
  
  return x;
}

bool dbsks_fill_in_silhouette(const dbsksp_xshock_graph_sptr& xgraph, const vil_image_view<vxl_byte >& source_image, vcl_vector<vgl_point_2d<int > >& points, vil_image_view<vxl_byte >& screenshot_image)
{
	points.clear();
    screenshot_image.set_size(source_image.ni(),source_image.nj());
    screenshot_image.fill(0);

	// draw the shock graph on top of the black window
    dbsksp_screenshot_binary(xgraph, screenshot_image, 1, points);

	for (dbsksp_xshock_graph::vertex_iterator vit = xgraph->vertices_begin();
	vit != xgraph->vertices_end(); ++vit)
	{
		dbsksp_xshock_node_sptr xv = *vit;
		//dbsksp_xshock_node_sptr xv = xgraph->node_from_id(xgraph->root_vertex_id());

		vcl_vector<vil_chord> region;
		vil_flood_fill8(screenshot_image, xv->pt().x(), xv->pt().y(), vxl_byte(0), vxl_byte(255), region);

		for (int c = 0; c < region.size(); c++)
		{
			int xlo = region[c].ilo;
			int xhi = region[c].ihi;
			int y = region[c].j;

			for (int x = xlo; x<= xhi; x++)
			{
				vgl_point_2d<int> pt(x,y);
				points.push_back(pt);
			}
		}
	}
	return true;
}

vcl_vector<vnl_matrix<double> > dbsks_compute_appearance_id_matrix(vcl_vector<vgl_point_2d<int > >& points, const vil_image_view<vxl_byte >& source_image)
{
	vnl_matrix <double> appearance_id_matrix_1 (21, 21, 0);
	vnl_matrix <double> appearance_id_matrix_2 (21, 21, 0);

	vcl_vector<vnl_matrix<double> > two_matrix;
	//vcl_vector <double> dist_V;
	//vcl_vector <double> int_sum_V;
	//vcl_vector <double> int_sub_V;

	//vcl_cout << "\n compute appearce id matrix \n";
	for(int i = 0; i< points.size(); i++)
	{
		for(int j = i+1; j<points.size(); j++)
		{
			double dist = vgl_distance(points[i], points[j]);
			int dist_id = vnl_math::floor(dist/10);
			double int_sum = double(source_image(points[i].x(), points[i].y()) + source_image(points[j].x(), points[j].y()))/255;
			int int_sum_id = vnl_math::floor(int_sum/0.1);
			double int_diff = double(vnl_math::abs(source_image(points[i].x(), points[i].y()) - source_image(points[j].x(), points[j].y())))/255;
			int int_diff_id = vnl_math::floor(int_diff/0.1);

			appearance_id_matrix_1(dist_id, int_sum_id) +=1;
			appearance_id_matrix_2(dist_id, int_diff_id) +=1;
			//dist_V.push_back();
			//int_sum_V.push_back();
			//int_sub_V.push_back();
		}
	}

	appearance_id_matrix_1 /= appearance_id_matrix_1.absolute_value_sum();
	appearance_id_matrix_2 /= appearance_id_matrix_2.absolute_value_sum();


	two_matrix.push_back(appearance_id_matrix_1);
	two_matrix.push_back(appearance_id_matrix_2);
	return two_matrix;
}











