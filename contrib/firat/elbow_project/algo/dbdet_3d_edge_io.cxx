/*
 * dbdet_3d_edge_io.cxx
 *
 *  Created on: Oct 21, 2011
 *      Author: firat
 */

#include "dbdet_3d_edge_io.h"
#include <vcl_fstream.h>
#include <vcl_sstream.h>

bool dbdet_save_3d_edges(const vcl_vector<dbdet_3d_edge_sptr>& edgemap, const vcl_string& out_file, bool include_strength)
{
	vcl_ofstream ofs(out_file.c_str());
	if(ofs.good())
	{
		for(int i = 0; i < edgemap.size(); i++)
		{
			ofs << edgemap[i]->x << " " << edgemap[i]->y << " " << edgemap[i]->z << " "
					<< edgemap[i]->nx << " " << edgemap[i]->ny << " " << edgemap[i]->nz;
			if(include_strength)
			{
				ofs << " " << edgemap[i]->strength;
			}
			if(i < edgemap.size()-1)
			{
				ofs << vcl_endl;
			}
		}
		ofs.close();
		return true;
	}
	else
	{
		return false;
	}
}

bool dbdet_load_3d_edges(const vcl_string& in_file, vcl_vector<dbdet_3d_edge_sptr>& edgemap)
{
	vcl_ifstream ifs(in_file.c_str());
	if(!ifs.good())
	{
		return false;
	}
	vcl_string   line;
	while(vcl_getline(ifs, line))
	{
		vcl_stringstream lineStream(line);
		dbdet_3d_edge_sptr edg = new dbdet_3d_edge;
		lineStream >> edg->x;
		lineStream >> edg->y;
		lineStream >> edg->z;
		lineStream >> edg->nx;
		lineStream >> edg->ny;
		lineStream >> edg->nz;
		if(lineStream)
		{
			lineStream >> edg->strength;
		}
		edgemap.push_back(edg);
	}
	ifs.close();
	return true;
}


