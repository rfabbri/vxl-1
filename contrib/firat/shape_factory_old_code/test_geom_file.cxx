// This is /lemsvxl/contrib/firat/shape_factory/test_geom_file.cxx.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Aug 5, 2010
//
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_map.h>
#include <vcl_list.h>
#include <vcl_ctime.h>
#include <dbsks/xio/dbsks_xio_xgraph_geom_model.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_node_sptr.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsks/dbsks_xgraph_geom_model_sptr.h>
#include <dbsks/dbsks_xgraph_geom_model.h>
#include <dbsks/dbsks_xnode_geom_model.h>
#include <dbsks/dbsks_xfrag_geom_model.h>
#include <vsol/vsol_polygon_2d.h>
#include <vsol/vsol_polygon_2d_sptr.h>
#include <dbsksp/algo/dbsksp_trace_boundary.h>
#include <vgl/vgl_polygon.h>
#include <bsol/bsol_algs.h>

vnl_random random_engine2;

void traverse_xgraph(int parent_edge, dbsksp_xshock_node_sptr root_node)
{
    vcl_cout << "Node id: " << root_node->id() << vcl_endl;
    vcl_list<dbsksp_xshock_edge_sptr>::const_iterator edgelist_iterator = root_node->edge_list().begin();
    for(;edgelist_iterator != root_node->edge_list().end(); ++edgelist_iterator)
    {
        dbsksp_xshock_edge_sptr edge = *edgelist_iterator;
        vcl_cout << "edge " << edge->id() << vcl_endl;
        if(edge->id() == parent_edge)
            continue;
        dbsksp_xshock_node_sptr child_node = edge->opposite(root_node);
        traverse_xgraph(edge->id(), child_node);
    }
}

bool self_intersection(dbsksp_xshock_graph_sptr xgraph)
{
    // boundary as a polygon
        double approx_ds = 1.0;
        vsol_polygon_2d_sptr query_poly = dbsksp_trace_xgraph_boundary_as_polygon(xgraph, approx_ds);

        // retrace the boundary to roughly 50 points
        query_poly = dbsksp_trace_xgraph_boundary_as_polygon(xgraph, double(query_poly->size()*approx_ds) / 1000);
        vgl_polygon<double > query_vgl_poly = bsol_algs::vgl_from_poly(query_poly);
        vcl_vector<vcl_pair<unsigned,unsigned> > e1;
        vcl_vector<vcl_pair<unsigned,unsigned> > e2;
        vcl_vector<vgl_point_2d<double> > ip;
        vgl_selfintersections(query_vgl_poly, e1, e2, ip);
        return ip.size() > 0;
}

void generate_xgraph(int parent_edge, dbsksp_xshock_node_sptr root_node, dbsks_xgraph_geom_model_sptr xgeom, dbsksp_xshock_graph_sptr xgraph, double graph_size)
{
    vcl_list<dbsksp_xshock_edge_sptr>::const_iterator edgelist_iterator = root_node->edge_list().begin();

    for(;edgelist_iterator != root_node->edge_list().end(); ++edgelist_iterator)
    {
        dbsksp_xshock_edge_sptr edge = *edgelist_iterator;
        vcl_cout << "node " << root_node->id() << " / edge " << edge->id() << vcl_endl;
        if(edge->id() == parent_edge)
            continue;
        dbsksp_xshock_node_sptr child_node = edge->opposite(root_node);

        vcl_map<unsigned, dbsks_xfrag_geom_model_sptr > map_edge2geom = xgeom->map_edge2geom();
        dbsks_xfrag_geom_model_sptr frag_model = map_edge2geom[edge->id()];

        vcl_vector<dbsksp_xshock_node_descriptor> sampled_node_descriptors;
        do
        {
            frag_model->sample_legal_end_given_start_using_model_minmax_range(*root_node->descriptor(edge), graph_size, 1, sampled_node_descriptors);

            dbsksp_xshock_node_descriptor child_descriptor = sampled_node_descriptors[random_engine2.lrand32(0,99)];
            child_node->set_radius(child_descriptor.radius_);
            child_node->set_pt(child_descriptor.pt_);
            child_node->descriptor(edge)->set_phi(child_descriptor.phi_);
            child_node->descriptor(edge)->set_shock_tangent(child_descriptor.psi_);
        }
        while(self_intersection(xgraph));

        generate_xgraph(edge->id(), child_node, xgeom, xgraph, graph_size);
    }

}

int main()
{

    random_engine2 = vnl_random(vcl_time(NULL));
    vcl_string xgraph_geom_file = "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph-firat/applelogos/applelogos-geom-model-rv29-ppe30-mce31-09232010.xml";
    vcl_string xgraph_file = "/vision/projects/kimia/shockshape/symseg/results/ETHZ-dataset/xshock-graph/applelogos-xgraph/applelogos_another.xgraph.0.prototype1.xml";
    dbsks_xgraph_geom_model_sptr xgeom;
    x_read(xgraph_geom_file, xgeom);
    dbsksp_xshock_graph_sptr xgraph;
    x_read(xgraph_file, xgraph);
    if(xgeom->is_compatible(xgraph))
    {
        vcl_cout << "Compatible" << vcl_endl;
    }
    else
    {
        vcl_cout << "Incompatible" << vcl_endl;
    }
    dbsksp_xshock_node_sptr root_node = xgraph->node_from_id(xgeom->root_vid());
    generate_xgraph(-1, root_node, xgeom, xgraph, vcl_sqrt(xgraph->area()));
    if(xgeom->is_compatible(xgraph))
    {
        vcl_cout << "Compatible" << vcl_endl;
    }
    else
    {
        vcl_cout << "Incompatible" << vcl_endl;
    }
    x_write("/home/firat/Desktop/generated.xml", xgraph);


}
