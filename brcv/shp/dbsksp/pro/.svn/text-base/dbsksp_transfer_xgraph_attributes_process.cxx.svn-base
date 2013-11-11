// This is /lemsvxl/brcv/shp/dbsksp/pro/dbsksp_transfer_xgraph_attributes_process.cxx.

//:
// \file
// \author Firat Kalaycilar (firat@lems.brown.edu)
// \date Sep 13, 2010
//

#include "dbsksp_transfer_xgraph_attributes_process.h"

#include <bpro1/bpro1_parameters.h>
#include<dbsksp/pro/dbsksp_xgraph_storage.h>
#include<dbsksp/pro/dbsksp_xgraph_storage_sptr.h>



// ----------------------------------------------------------------------------
//: Constructor
dbsksp_transfer_xgraph_attributes_process::
dbsksp_transfer_xgraph_attributes_process()
{
    vcl_vector<vcl_string > transfer_options_;
    transfer_options_.push_back("Scale");
    transfer_options_.push_back("Translate");
    transfer_options_.push_back("Transfer node attributes");

    if( !parameters()->add("Transfer type: " , "-transfer_type" , transfer_options_, 0) ||
            !parameters()->add("Source xnode id: " , "-source_node_id", unsigned(0)) ||
            !parameters()->add("Target xnode id: " , "-dest_node_id", unsigned(0))
    )
    {
        vcl_cerr << "ERROR: Adding parameters in " __FILE__ << vcl_endl;
    }
}


// ----------------------------------------------------------------------------
//: Destructor
dbsksp_transfer_xgraph_attributes_process::
~dbsksp_transfer_xgraph_attributes_process()
{

}


// ----------------------------------------------------------------------------
//: Clone the process
bpro1_process* dbsksp_transfer_xgraph_attributes_process::
clone() const
{
    return new dbsksp_transfer_xgraph_attributes_process(*this);
}


// ----------------------------------------------------------------------------
//: Returns the name of this process
vcl_string dbsksp_transfer_xgraph_attributes_process::
name()
{
    return "Transfer xgraph attributes";
}


// ----------------------------------------------------------------------------
//: Provide a vector of required input types
vcl_vector< vcl_string > dbsksp_transfer_xgraph_attributes_process::
get_input_type()
{
    vcl_vector< vcl_string > to_return;
    to_return.push_back("dbsksp_xgraph");
    to_return.push_back("dbsksp_xgraph");
    return to_return;
}



// ----------------------------------------------------------------------------
//: Provide a vector of output types
vcl_vector< vcl_string > dbsksp_transfer_xgraph_attributes_process::
get_output_type()
{
    vcl_vector<vcl_string > to_return;
    return to_return;
}



// ----------------------------------------------------------------------------
//: Return the number of input frames for this process
int dbsksp_transfer_xgraph_attributes_process::
input_frames()
{
    return 1;
}


// ----------------------------------------------------------------------------
//: Return the number of output frames for this process
int dbsksp_transfer_xgraph_attributes_process::
output_frames()
{
    return 1;
}

//: Execute this process
bool dbsksp_transfer_xgraph_attributes_process::
execute()
{
    // Preliminary check for inputs

    unsigned int transfer_type = 0;
    parameters()->get_value( "-transfer_type" , transfer_type);
    unsigned source_id = 0;
    parameters()->get_value( "-source_node_id" , source_id);
    unsigned target_id = 0;
    parameters()->get_value( "-dest_node_id" , target_id);
    dbsksp_xgraph_storage_sptr input_storage1;
    input_storage1.vertical_cast(input_data_[0][0]);
    dbsksp_xgraph_storage_sptr input_storage2;
    input_storage2.vertical_cast(input_data_[0][1]);
    dbsksp_xshock_graph_sptr xg1 = input_storage1->xgraph();
    dbsksp_xshock_graph_sptr xg2 = input_storage2->xgraph();
    vcl_cout << "Source node: " << source_id << vcl_endl;
    vcl_cout << "Target node: " << target_id << vcl_endl;


    if(transfer_type == 0) // scale up the target xgraph so that both xgraphs have the same area
    {
        vcl_cout << "SCALE" << vcl_endl;
        vgl_point_2d<double> pt2 = xg2->node_from_id(target_id)->pt();
        double area1 = xg1->area();
        double area2 = xg2->area();
        xg2->scale_up(pt2.x(), pt2.y(), vcl_sqrt(area1/area2));
    }
    else if(transfer_type == 1) // translate the target xgraph so that it coincides with the selected source xgraph node
    {
        vcl_cout << "TRANSLATE" << vcl_endl;
        vgl_point_2d<double> pt1 = xg1->node_from_id(source_id)->pt();
        vgl_point_2d<double> pt2 = xg2->node_from_id(target_id)->pt();
        double dx = pt1.x() - pt2.x();
        double dy = pt1.y() - pt2.y();
        xg2->translate(dx, dy);
    }
    else if(transfer_type == 2) // transfer all attributes of a selected source node to a target node
    {
        vcl_cout << "TRANSFER" << vcl_endl;
        dbsksp_xshock_node_sptr source_node = xg1->node_from_id(source_id);
        dbsksp_xshock_node_sptr target_node = xg2->node_from_id(target_id);
        vcl_list<dbsksp_xshock_edge_sptr>::const_iterator source_edgelist_iterator = source_node->edge_list().begin();
        vcl_list<dbsksp_xshock_edge_sptr>::const_iterator target_edgelist_iterator = target_node->edge_list().begin();
        dbsksp_xshock_node_sptr source_terminal_node = 0;
        dbsksp_xshock_node_sptr target_terminal_node = 0;
        for(;source_edgelist_iterator != source_node->edge_list().end(); ++source_edgelist_iterator,++target_edgelist_iterator)
        {
            dbsksp_xshock_node_sptr source_neighbor_node = (*source_edgelist_iterator)->opposite(source_node);
            if(source_neighbor_node->degree() == 1)
            {
                vcl_cout << source_neighbor_node->id() << " is a terminal node of source xgraph!" << vcl_endl;
                source_terminal_node = source_neighbor_node;
            }

            dbsksp_xshock_node_sptr target_neighbor_node = (*target_edgelist_iterator)->opposite(target_node);
            if(target_neighbor_node->degree() == 1)
            {
                vcl_cout << target_neighbor_node->id() << " is a terminal node of target xgraph!" << vcl_endl;
                target_terminal_node = target_neighbor_node;
            }
            dbsksp_xshock_node_descriptor* source_node_descriptor = source_node->descriptor(*source_edgelist_iterator);
            dbsksp_xshock_node_descriptor* target_node_descriptor = target_node->descriptor(*target_edgelist_iterator);
            target_node->set_radius(source_node_descriptor->radius_);
            target_node->set_pt(source_node_descriptor->pt_);
            target_node_descriptor->set_phi(source_node_descriptor->phi_);
            target_node_descriptor->set_shock_tangent(source_node_descriptor->psi_);
        }
        if(source_terminal_node != 0 && target_terminal_node != 0)
        {
            dbsksp_xshock_node_descriptor* source_node_descriptor = source_terminal_node->descriptor(*source_terminal_node->edges_begin());
            dbsksp_xshock_node_descriptor* target_node_descriptor = target_terminal_node->descriptor(*target_terminal_node->edges_begin());
            target_terminal_node->set_radius(source_node_descriptor->radius_);
            target_terminal_node->set_pt(source_node_descriptor->pt_);
            target_node_descriptor->set_phi(source_node_descriptor->phi_);
            target_node_descriptor->set_shock_tangent(source_node_descriptor->psi_);
        }

    }
    return true;
}

bool dbsksp_transfer_xgraph_attributes_process::
finish()
{
    return true;
}





