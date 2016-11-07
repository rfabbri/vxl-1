// This is /lemsvxl/contrib/firat/dbsksp_object_warehouse/dbsksp_object_warehouse.cxx.

// \file
// \author Firat Kalaycilar (firat_kalaycilar@brown.edu)
// \date Apr 8, 2011

#include "dbsksp_object_warehouse.h"
#include <dbsks/xio/dbsks_xio_xgraph_geom_model.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>
#include <dbsksp/dbsksp_xshock_graph.h>
#include <dbsksp/algo/dbsksp_xgraph_boundary_algos.h>
#include <dbsksp/xio/dbsksp_xio_xshock_graph.h>

#include <vcl_sstream.h>

#include <vcl_cstdlib.h>
#include <vcl_ctime.h>

#include <dbul/dbul_parse_simple_file.h>


dbsksp_object_warehouse::dbsksp_object_warehouse(const vcl_string& exemplar_xgraph_folder, const vcl_string& exemplar_xgraph_list_file)
{
	vcl_vector<vcl_string > xml_filenames;
	dbul_parse_string_list(exemplar_xgraph_list_file, xml_filenames);
	for(int i = 0; i < xml_filenames.size(); i++)
	{
		vcl_string xml_file = exemplar_xgraph_folder + "/" + xml_filenames[i];
		dbsksp_xshock_graph_sptr xg;
		if(!x_read(xml_file, xg))
		{
			vcl_cout << "Reading exemplar file failed!";
			continue;
		}
		dbsksp_xshock_graph::vertex_iterator vit = xg->vertices_begin();
		dbsksp_xshock_node_sptr root_node = xg->node_from_id((*vit)->id());
		xg->similarity_transform(root_node->pt(), 0, 0, 0, 100 / vcl_sqrt(xg->area()));
		this->exemplar_xgraphs_.push_back(xg);
		this->child_xgraphs_.push_back(new vcl_vector<dbsksp_xshock_graph_sptr>);
	}
	if(this->exemplar_xgraphs_.size() > 0)
	{
		this->active_ = true;
		this->random_engine_ = vnl_random(vcl_time(NULL));
	}
	else
	{
		this->active_ = false;
	}
}

dbsksp_object_warehouse::~dbsksp_object_warehouse()
{
	for(int i = 0; i < this->child_xgraphs_.size(); i++)
	{
		delete this->child_xgraphs_[i];
	}
}

bool dbsksp_object_warehouse::perturb_exemplar(int exemplar_id, int num_perturbations)
{
	if(this->active_)
	{
		if(exemplar_id < 0 || exemplar_id >= this->exemplar_xgraphs_.size())
		{
			return false;
		}
		for(int i = 0; i < num_perturbations; i++)
		{
			dbsksp_xshock_graph_sptr xgraph = new dbsksp_xshock_graph(*this->exemplar_xgraphs_[exemplar_id]);
			dbsksp_xshock_graph::vertex_iterator vit = xgraph->vertices_begin();
			dbsksp_xshock_node_sptr root_node = xgraph->node_from_id((*vit)->id());
			do
			{
				this->perturb_xgraph_(xgraph, root_node, -1);
			}
			while(dbsksp_test_boundary_self_intersection(xgraph, 10));
			xgraph->similarity_transform(root_node->pt(), 0, 0, 0, 100 / vcl_sqrt(xgraph->area()));
			this->child_xgraphs_[exemplar_id]->push_back(xgraph);
		}
	}
	return true;
}



bool dbsksp_object_warehouse::perturb_all_exemplars(int num_perturbations)
{
	return false;
}

dbsksp_xshock_graph_sptr dbsksp_object_warehouse::get_object(int exemplar_id, int child_id)
{
	return 0;
}

bool dbsksp_object_warehouse::save(const vcl_string& folder)
{
	for(int i = 0; i < this->exemplar_xgraphs_.size(); i++)
	{
		vcl_stringstream ss1 (vcl_stringstream::in | vcl_stringstream::out);
		ss1 << folder << "/exemplar_" << i+1 << ".xml";
		vcl_string exemplar_file_path;
		ss1 >> exemplar_file_path;
		vcl_cout << "Saving:" << exemplar_file_path << vcl_endl;
		x_write(exemplar_file_path, this->exemplar_xgraphs_[i]);
		vcl_vector<vsol_point_2d_sptr> point_list =  dbsksp_get_boundary_point_list(this->exemplar_xgraphs_[i], 400);
		vcl_string exemplar_contour_file = exemplar_file_path;
		exemplar_contour_file.replace(exemplar_contour_file.length()-3, 3, "txt");
		vcl_cout << "Saving:" << exemplar_contour_file << vcl_endl;
		this->save_contour_file_(point_list, exemplar_contour_file);
		for(int j = 0; j < this->child_xgraphs_[i]->size(); j++)
		{
			vcl_stringstream ss2 (vcl_stringstream::in | vcl_stringstream::out);
			ss2 << folder << "/child_" << i+1 << "_" << j+1 << ".xml";
			vcl_string child_file_path;
			ss2 >> child_file_path;
			vcl_cout << "Saving:" << child_file_path << vcl_endl;
			x_write(child_file_path, (*this->child_xgraphs_[i])[j]);
		}
	}
	return true;
}

dbsksp_object_warehouse* dbsksp_object_warehouse::load(const vcl_string& folder)
{
	return 0;
}

void dbsksp_object_warehouse::perturb_xgraph_(dbsksp_xshock_graph_sptr xgraph, dbsksp_xshock_node_sptr root_node, int parent_edge)
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
			root_node->set_radius(root_node->radius() + this->random_engine_.normal()*2);
			root_descriptor->set_phi(root_descriptor->phi_ + this->random_engine_.normal()*0.1);
			root_descriptor->set_shock_tangent(root_descriptor->psi_ + this->random_engine_.normal()*0.1);
		}

		double new_radius;
		vgl_point_2d<double> new_pt;
		double new_phi;
		double new_psi;
		if(child_node->degree() > 1)
		{
			new_radius = orig_child_descriptor->radius_+ this->random_engine_.normal()*2;
			new_pt = orig_child_descriptor->pt_;
			new_pt.set(orig_child_descriptor->pt_.x() + this->random_engine_.normal()*0.1,
					orig_child_descriptor->pt_.y() + this->random_engine_.normal()*0.1
			);
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

		this->perturb_xgraph_(xgraph, child_node, edge->id());
	}
}

void dbsksp_object_warehouse::save_contour_file_(vcl_vector<vsol_point_2d_sptr>& point_list, const vcl_string& filename)
{
	vcl_ofstream ofs(filename.c_str());
	ofs << point_list.size() << " 0" << vcl_endl;
	for(int i = 0; i < point_list.size(); i++)
	{
		ofs << point_list[i]->x() << " " << point_list[i]->y() << vcl_endl;
	}
	ofs.close();
}
