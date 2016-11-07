#include <dbsk2d/dbsk2d_shock_graph_sptr.h>
#include <dbsk2d/dbsk2d_shock_graph.h>
#include <dbsk2d/dbsk2d_shock_edge_sptr.h>
#include <dbsk2d/dbsk2d_shock_edge.h>
#include <dbsk2d/dbsk2d_shock_node_sptr.h>
#include <dbsk2d/dbsk2d_shock_node.h>
#include <dbsk2d/dbsk2d_xshock_edge_sptr.h>
#include <dbsk2d/dbsk2d_xshock_edge.h>
#include <dbsk2d/algo/dbsk2d_compute_bounding_box.h>
#include <dbsk2d/dbsk2d_utils.h>
#include <dbsk2d/algo/dbsk2d_hor_flip_shock_graph.h>

bool dbsk2d_hor_flip_shock_graph(dbsk2d_shock_graph_sptr shock)
{

    // Compute bounding box for the given shock file
    if(!dbsk2d_compute_bounding_box(shock))
    {
        vcl_cout << "Cannot compute bounding box for the given shock graph!" << vcl_endl;
        vcl_cout << "Exiting" << vcl_endl;
        return false;
    }

    // Save the coordinates of the bounding box for modifying x coordinates
    vsol_box_2d_sptr bbox = shock->get_bounding_box();
    double min_x = bbox->get_min_x();
    double max_x = bbox->get_max_x();

    vcl_cout << "min x: " << min_x << vcl_endl;
    vcl_cout << "max x: " << max_x << vcl_endl;

    //Iterate through all edges
    dbsk2d_shock_graph::edge_iterator e_it = shock->edges_begin();
    for(; e_it != shock->edges_end(); ++e_it)
    {
        dbsk2d_shock_edge_sptr cur_edge = (*e_it);
        dbsk2d_xshock_edge_sptr cur_xedge = static_cast<dbsk2d_xshock_edge*>(cur_edge.ptr());

        int cur_num_samples = cur_xedge->num_samples();

        // Clear the previously-stored extrinsic points
        cur_xedge->clear_ex_pts();


        // Iterate through all samples and...
        for(int i=0; i<cur_num_samples; ++i)
        {
            dbsk2d_xshock_sample_sptr flipped_sample = cur_xedge->sample(i);

            // ...flip the x coordinates of the shock point...
            vgl_point_2d<double> new_point = flipped_sample->pt;
            new_point.set(min_x+max_x-new_point.x(), new_point.y());
            flipped_sample->pt = new_point;

            // ...and the boundary points...
            // (it is normal for them to be flipped!)
            vgl_point_2d<double> new_left_bnd_pt = flipped_sample->right_bnd_pt;
            vgl_point_2d<double> new_right_bnd_pt = flipped_sample->left_bnd_pt;
            new_left_bnd_pt.set(min_x+max_x-new_left_bnd_pt.x(), new_left_bnd_pt.y());
            new_right_bnd_pt.set(min_x+max_x-new_right_bnd_pt.x(), new_right_bnd_pt.y());
            flipped_sample->left_bnd_pt = new_left_bnd_pt;
            flipped_sample->right_bnd_pt = new_right_bnd_pt;

            // ...and set the tangent
            double new_theta = vnl_math::pi - flipped_sample->theta;
            new_theta = angle0To2Pi(new_theta);
            flipped_sample->theta = new_theta;
            
            
            cur_xedge->set_sample(i, flipped_sample);
        }

        // Re-build the locus and replace the extrinsic points
        cur_xedge->compute_extrinsic_locus();

    }

    dbsk2d_shock_graph::vertex_iterator v_it = shock->vertices_begin();
    for(; v_it!=shock->vertices_end(); ++v_it)
    {
        dbsk2d_shock_node_sptr cur_node = *(v_it);
        cur_node->reverse_in_edges();
        cur_node->reverse_out_edges();
    }


    return true;
}


