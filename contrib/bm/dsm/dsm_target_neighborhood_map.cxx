//this is /contrib/bm/dsm/dsm_target_neighborhood_map.cxx
#include"dsm_target_neighborhood_map.h"

void dsm_target_neighborhood_map::b_write(vsl_b_ostream& os) const
{
	const short version_no = 1;
	vsl_b_write(os, version_no);

	//write the number of targets
	unsigned ntargets = this->target_neighborhood_map_.size();
	vsl_b_write(os,ntargets);

	dsm_target_neighborhood_map_type::const_iterator titr, tend=this->target_neighborhood_map_.end();

	for(titr=this->target_neighborhood_map_.begin(); titr!=tend; ++titr)
	{
		//write the target
		vsl_b_write(os,titr->first);

		//write the number of neighbors
		unsigned nneighbors = titr->second.size();
		vsl_b_write(os, nneighbors);

		vcl_vector<vgl_point_2d<unsigned> >::const_iterator nitr, nend = titr->second.end();
		for(nitr=titr->second.begin(); nitr!=nend; ++nitr)
			vsl_b_write(os, *nitr);
	}
}

void dsm_target_neighborhood_map::b_read(vsl_b_istream& is)
{
	if(!is) return;

	short v;
	vsl_b_read(is,v);

	switch(v)
	{
	case 1:
		{
			//read number of targets
			unsigned ntargets;
			vsl_b_read(is, ntargets);

			for( unsigned i = 0; i < ntargets; ++i )
			{
				vgl_point_2d<unsigned> target;
				vsl_b_read(is,target);

				//read the number of neighbors
				unsigned nneighbors;
				vsl_b_read(is,nneighbors);

				vcl_vector<vgl_point_2d<unsigned> > neighborhood;
				for(unsigned j = 0; j < nneighbors; ++j)
				{
					vgl_point_2d<unsigned> neighbor;
					vsl_b_read(is, neighbor);
					neighborhood.push_back(neighbor);
				}//end neighborhood iteration

				this->target_neighborhood_map_[target] = neighborhood;
			}//end target iteration

		}//end case 1
		break;
	default:
		{
			vcl_cerr << "----I/O ERROR: dsm_feature::b_read ----\n"
				     << "	 UNKNOWN VERSION NUMBER " << v << "\n";
			is.is().clear(vcl_ios::badbit); //set an unrecoverable IO error on stream
			return;
		}//end default
	}//end switch

}//end b_read()

bool dsm_target_neighborhood_map::write_txt(vcl_string const& filename) const
{
	if( this->target_neighborhood_map_.empty() )
	{
		vcl_cerr << "---------------- WARNING ----------------" <<'\n'
				 << "dsm_target_neighborhood_map::write_txt( vcl_string const& " << filename << " )\n"
				 << "\t The map is empty. Could not write txt file.\n" << vcl_flush;
		return false;
	}
	else
	{
		vcl_ofstream of( filename.c_str(), vcl_ios::out );

		dsm_target_neighborhood_map_type::const_iterator titr, tend = this->target_neighborhood_map_.end();

		for(titr=this->target_neighborhood_map_.begin(); titr!=tend; ++titr)
		{
			of << titr->first.x() << '\t';

			vcl_vector<vgl_point_2d<unsigned> >::const_iterator nitr, nend = titr->second.end();

			for( nitr = titr->second.begin(); nitr != nend; ++nitr)
			{
				of << nitr->x() << '\t';
			}//end neighborhood iteration 1

			of << '\n';

			of << titr->first.y() << '\t';

			for( nitr = titr->second.begin(); nitr != nend; ++nitr )
			{
				of << nitr->y() << '\t';
			}//end neighborhood iteration 2

			of << '\n' << '\n';
		}//end target iteration

		of.close();
		return true;
	}
}//end write_txt;


void dsm_target_neighborhood_map::insert( vgl_point_2d<unsigned> const& target, vcl_vector<vgl_point_2d<unsigned> > const& neighborhood )
{
	dsm_target_neighborhood_map_type::iterator itr;

	itr = this->target_neighborhood_map_.find(target);

	//erase the entry if it exists
	if( itr != this->target_neighborhood_map_.end() )
		this->target_neighborhood_map_.erase(itr);

	this->target_neighborhood_map_[target] = neighborhood;
}

unsigned dsm_target_neighborhood_map::num_neighbors() const
{
	if( this->target_neighborhood_map_.empty() )
	{
		vcl_cerr << "Warning: target_neighborhood_map is empty, cannot calculate the number of neighbors." << vcl_flush;
		return 0;
	}

	return this->target_neighborhood_map_.begin()->second.size();
}