#include "dbsk2d_hor_flip_shock_graph.h"
#include <vsol/vsol_box_2d_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>
#include <dbsk2d/dbsk2d_xshock_edge_sptr.h>
#include <dbsk2d/dbsk2d_xshock_sample_sptr.h>
#include <vnl/vnl_math.h>
#include <dbsk2d/algo/dbsk2d_compute_bounding_box.h>

bool dbsk2d_hor_flip_shock_graph(dbsk2d_shock_graph_sptr sg)
{
    if(!dbsk2d_compute_bounding_box(sg))
    {
        vcl_cout << "Could not compute the bounding box for the shock graph" << vcl_endl;
        vcl_cout << "Flip operation failed" << vcl_endl;
        return false;
    }

    // Get the bounding box for the shock graph
    vsol_box_2d_sptr bbox = sg->get_bounding_box();

    // Get the width for the bounding box
    double width = bbox->width();

    // Reverse the order of edges for all nodes
    dbsk2d_shock_graph::vertex_iterator v_it = sg->vertices_begin();

    for( ; v_it != sg->vertices_end(); ++v_it)
    {
        dbsk2d_shock_node_sptr cur_node = (*v_it);
        cur_node->reverse_in_edges();
        cur_node->reverse_out_edges();
    }

    // Go over all the edges and 
    // 1) flip the x coordinates for all samples
    // 2) flip the x coordinates for all boundary points
    // 3) flip the tangents for all samples
    // 4) flip the tangents for all boundary points
    dbsk2d_shock_graph::edge_iterator e_it = sg->edges_begin();
    
    for( ; e_it != sg->edges_end(); ++e_it)
    {
        dbsk2d_xshock_edge* cur_edge = (dbsk2d_xshock_edge*)((*e_it).ptr());
        cur_edge->clear_ex_pts();

        for(unsigned int i=0; i<cur_edge->num_samples(); ++i)
        {
            dbsk2d_xshock_sample_sptr cur_sample = cur_edge->sample(i);
            dbsk2d_xshock_sample_sptr new_sample = cur_sample;

            vgl_point_2d<double> cur_point = cur_sample->pt;
            vgl_point_2d<double> new_point(width-cur_point.x(),cur_point.y());
            new_sample->pt = new_point;
            
            vgl_point_2d<double> cur_right_bnd_pt = cur_sample->right_bnd_pt;
            vgl_point_2d<double> new_left_bnd_pt(width-cur_right_bnd_pt.x(),cur_right_bnd_pt.y());
            vgl_point_2d<double> cur_left_bnd_pt = cur_sample->left_bnd_pt;
            vgl_point_2d<double> new_right_bnd_pt(width-cur_left_bnd_pt.x(),cur_left_bnd_pt.y());
            new_sample->left_bnd_pt = new_left_bnd_pt;
            new_sample->right_bnd_pt = new_right_bnd_pt;

            double new_theta = vnl_math::pi - cur_sample->theta;
            new_sample->theta = new_theta;

            double new_left_bnd_tangent = vnl_math::pi - cur_sample->right_bnd_tangent;
            double new_right_bnd_tangent = vnl_math::pi - cur_sample->left_bnd_tangent;
            new_sample->left_bnd_tangent = new_left_bnd_tangent;
            new_sample->right_bnd_tangent = new_right_bnd_tangent;

            cur_edge->set_sample(i,new_sample);
        }

        cur_edge->compute_extrinsic_locus();
    }

    return true;
}
