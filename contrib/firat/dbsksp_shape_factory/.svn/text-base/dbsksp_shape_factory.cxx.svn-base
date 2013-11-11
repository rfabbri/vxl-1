// This is /lemsvxl/contrib/firat/shape_factory_v1/dbsksp_shape_factory.cxx.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Jan 19, 2011
//

#include "dbsksp_shape_factory.h"
#include <dbsks/xio/dbsks_xio_xgraph_geom_model.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsks/dbsks_xgraph_geom_model.h>
#include <dbsksp/algo/dbsksp_xgraph_boundary_algos.h>
#include <dbsks/dbsks_xfrag_geom_model.h>

#include <vcl_cstdlib.h>
#include <vcl_ctime.h>

#include <dbul/dbul_parse_simple_file.h>


void dbsksp_shape_factory::generate_xgraph_(dbsksp_xshock_graph_sptr xgraph, dbsksp_xshock_node_sptr root_node, int parent_edge, double graph_size)
{
    vcl_list<dbsksp_xshock_edge_sptr>::const_iterator edgelist_iterator = root_node->edge_list().begin();


    for(;edgelist_iterator != root_node->edge_list().end(); ++edgelist_iterator)
    {
        dbsksp_xshock_edge_sptr edge = *edgelist_iterator;
        vcl_cout << "node " << root_node->id() << " / edge " << edge->id() << vcl_endl;
        if(edge->id() == parent_edge)
            continue;

        dbsksp_xshock_node_sptr child_node = edge->opposite(root_node);
        dbsksp_xshock_node_descriptor* root_descriptor = root_node->descriptor(edge);
        dbsksp_xshock_node_descriptor* orig_child_descriptor = child_node->descriptor(edge);

        if(parent_edge == -1)
        {
            root_descriptor->set_phi(root_descriptor->phi_ + this->random_engine_.normal()*0.05);
            root_descriptor->set_shock_tangent(root_descriptor->psi_ + this->random_engine_.normal()*0.05);
        }

        double new_radius;
        vgl_point_2d<double> new_pt;
        double new_phi;
        double new_psi;
        if(child_node->degree() > 1)
        {
            vcl_map<unsigned, dbsks_xfrag_geom_model_sptr > map_edge2geom = this->xgeom_->map_edge2geom();
            dbsks_xfrag_geom_model_sptr frag_model = map_edge2geom[edge->id()];

            vcl_vector<dbsksp_xshock_node_descriptor> sampled_node_descriptors;

            frag_model->sample_legal_end_given_start_using_model_minmax_range(*root_node->descriptor(edge), graph_size, 1000, sampled_node_descriptors);

            if(sampled_node_descriptors.size() > 0)
            {
                dbsksp_xshock_node_descriptor child_descriptor = sampled_node_descriptors[this->random_engine_.lrand32(0,sampled_node_descriptors.size()-1)];
                new_radius = child_descriptor.radius_;
                new_pt = child_descriptor.pt_;
            }
            else
            {
                new_radius = orig_child_descriptor->radius_;
                new_pt = orig_child_descriptor->pt_;

            }
            new_phi = orig_child_descriptor->phi_+ this->random_engine_.normal()*0.1;
            new_psi = orig_child_descriptor->psi_+ this->random_engine_.normal()*0.1;
        }
        else
        {
            new_radius = root_descriptor->radius_;
            new_pt = root_descriptor->pt_;
            new_phi = 0;
            new_psi = root_descriptor->psi_;
        }
        child_node->set_radius(new_radius);
        child_node->set_pt(new_pt);
        child_node->descriptor(edge)->set_phi(new_phi);
        child_node->descriptor(edge)->set_shock_tangent(new_psi);

        this->generate_xgraph_(xgraph, child_node, edge->id(), graph_size);
    }

}



dbsksp_shape_factory::dbsksp_shape_factory(const vcl_string& xgraph_geom_filename, const vcl_string& prototype_xgraph_filename)
{
    if(!x_read(xgraph_geom_filename, this->xgeom_))
    {
        vcl_cout << "Reading geometric model file failed!";
        this->active_ = false;
        return;
    }
    dbsksp_xshock_graph_sptr xg;
    if(!x_read(prototype_xgraph_filename, xg))
    {
        vcl_cout << "Reading prototype file failed!";
        this->active_ = false;
        return;
    }
    this->prototype_xgraphs_.push_back(xg);
    this->active_ = true;
    this->random_engine_ = vnl_random(vcl_time(NULL));
}

dbsksp_xshock_graph_sptr dbsksp_shape_factory::generate_random_shape(int xgraph_id)
{
    if(this->active_)
    {
        if(xgraph_id < 0)
        {
        	xgraph_id = this->random_engine_.lrand32(0, this->prototype_xgraphs_.size()-1);
        }
        dbsksp_xshock_graph_sptr xgraph = new dbsksp_xshock_graph(*this->prototype_xgraphs_[xgraph_id]);
        dbsksp_xshock_node_sptr root_node = xgraph->node_from_id(this->xgeom_->root_vid());
        do
        {
            this->generate_xgraph_(xgraph, root_node, -1, vcl_sqrt(xgraph->area()));
        }
        while(dbsksp_test_boundary_self_intersection(xgraph, 10));
        xgraph->similarity_transform(root_node->pt(), 0, 0, 0, 100 / vcl_sqrt(xgraph->area()));
        return xgraph;
    }
    return NULL;
}

dbsksp_shape_factory::dbsksp_shape_factory(const vcl_string& xgraph_geom_filename, const vcl_string& prototype_xgraph_folder, const vcl_string& prototype_xgraph_list_file)
{
    if(!x_read(xgraph_geom_filename, this->xgeom_))
    {
        vcl_cout << "Reading geometric model file failed!";
        this->active_ = false;
        return;
    }
    vcl_vector<vcl_string > xml_filenames;
    dbul_parse_string_list(prototype_xgraph_list_file, xml_filenames);
    for(int i = 0; i < xml_filenames.size(); i++)
    {
        vcl_string xml_file = prototype_xgraph_folder + "/" + xml_filenames[i];
        dbsksp_xshock_graph_sptr xg;
        if(!x_read(xml_file, xg))
        {
            vcl_cout << "Reading prototype file failed!";
            continue;
        }
        this->prototype_xgraphs_.push_back(xg);
    }
    if(this->prototype_xgraphs_.size() > 0)
    {
        this->active_ = true;
        this->random_engine_ = vnl_random(vcl_time(NULL));
    }
    else
    {
        this->active_ = false;
    }
}

vcl_vector<dbsksp_xshock_graph_sptr> dbsksp_shape_factory::generate_random_shapes(int num, int xgraph_id)
{
    vcl_vector<dbsksp_xshock_graph_sptr> shapes;
    for(int i = 0; i < num; i++)
    {
        shapes.push_back(this->generate_random_shape(xgraph_id));
    }
    return shapes;
}

vcl_vector<dbsksp_xshock_graph_sptr> dbsksp_shape_factory::generate_all_shapes()
{

}
