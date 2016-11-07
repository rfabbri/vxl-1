//this is /contrib/bm/dsm/dsm_pixel_time_series_map.h
#include"dsm_pixel_time_series_map.h"

dsm_pixel_time_series_map::dsm_pixel_time_series_map( unsigned const& x, unsigned const& y, 
	dsm_time_series_sptr time_series_sptr, unsigned const& feature_dim):feature_dimension(feature_dim)
{
	this->pixel_time_series_map[vgl_point_2d<unsigned>(x,y)] = time_series_sptr;
}

void dsm_pixel_time_series_map::insert( unsigned const& x, unsigned const& y, dsm_time_series_sptr ts_sptr)
{
	vgl_point_2d<unsigned> pt(x,y);

	pixel_time_series_map_type::iterator itr;

	itr = pixel_time_series_map.find(pt);

	//erase the entry if it exists
	if( itr != pixel_time_series_map.end() )
		pixel_time_series_map.erase(itr);

	pixel_time_series_map[pt] = ts_sptr;
}

void dsm_pixel_time_series_map::insert( vgl_point_2d<unsigned> const& pt, dsm_time_series_sptr ts_sptr)
{
	pixel_time_series_map_type::iterator itr;

	itr = pixel_time_series_map.find(pt);

	//erase entry if it exists
	if( itr != pixel_time_series_map.end() )
		pixel_time_series_map.erase(itr);

	pixel_time_series_map[pt] = ts_sptr;
}

dsm_pixel_time_series_map::pixel_time_series_map_type::iterator dsm_pixel_time_series_map::time_series( vgl_point_2d<unsigned> const& pt )
{
	return this->pixel_time_series_map.find(pt);
}

dsm_pixel_time_series_map::pixel_time_series_map_type::iterator dsm_pixel_time_series_map::time_series( unsigned const& x, unsigned const& y)
{
	return this->pixel_time_series_map.find(vgl_point_2d<unsigned>(x,y));
}

void dsm_pixel_time_series_map::b_write(vsl_b_ostream& os) const
{
	const short version_no = 1;
	vsl_b_write(os, version_no);
	vsl_b_write(os, this->feature_dimension);
	vsl_b_write(os, this->pixel_time_series_map.size());//size of the map

	pixel_time_series_map_type::const_iterator itr, end_itr = this->pixel_time_series_map.end();

	for( itr = this->pixel_time_series_map.begin(); itr != end_itr; ++itr ) 
	{
		vsl_b_write(os, itr->first);//write the point
		vsl_b_write(os, itr->second);//write the dsm_time_series_sptr
	}

}

void dsm_pixel_time_series_map::b_read(vsl_b_istream& is)
{
	if(!is) return;

    short v;
    vsl_b_read(is,v);
    switch(v)
    {
    case 1:
        {
			unsigned feature_dim;
			vsl_b_read(is,feature_dim);
			this->feature_dimension = feature_dim;
            unsigned map_size;
			vsl_b_read(is, map_size);

			for( unsigned i = 0; i < map_size; ++i )
			{
				vgl_point_2d<unsigned> pt;
				vsl_b_read(is,pt);//read the point
				dsm_time_series_sptr ts_sptr = new dsm_time_series();
				vsl_b_read(is, ts_sptr);//read the time series sptr
				this->pixel_time_series_map[pt] = ts_sptr;
			}//end map iteration
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
}

bool dsm_pixel_time_series_map::write_txt( vcl_string const& filename ) const
{
	
	if( !vul_file::is_directory(filename) )
	{
		vcl_cerr << "ERROR ---- dsm_pixel_time_series_map::write_txt ----\n"
			     << "\t MUST PROVIDE A VALID DIRECTORY.";
		return false;
	}

	pixel_time_series_map_type::const_iterator itr, map_end = this->pixel_time_series_map.end();
	
	for( itr = this->pixel_time_series_map.begin(); itr != map_end; ++itr )
	{
		vcl_stringstream ss;
		ss << filename << "/" << itr->first.x() << "_" << itr->first.y() << "_" << this->feature_dimension <<  ".txt";

		itr->second->write_txt(ss.str());
	}//end pixel time series map iteration

	return true;
}

void dsm_pixel_time_series_map::insert( vgl_point_2d<unsigned> const& pt, unsigned const& time, dsm_feature_sptr feature_sptr )
{
	if( !this->pixel_time_series_map.count(pt) )
	{
		dsm_time_series_sptr ts_sptr = new dsm_time_series(time,feature_sptr);
		ts_sptr->insert(time,feature_sptr);
		this->pixel_time_series_map[pt] = ts_sptr;
	}
	else
		this->pixel_time_series_map[pt]->insert(time,feature_sptr);
}

void dsm_pixel_time_series_map::insert( unsigned const& x, unsigned const& y, unsigned const& time, dsm_feature_sptr feature_sptr )
{
	this->insert(vgl_point_2d<unsigned>(x,y),time,feature_sptr);
}

bool dsm_pixel_time_series_map::reduce_dims( unsigned const& ndims2keep )
{
	pixel_time_series_map_type::iterator pts_itr, pts_end = this->pixel_time_series_map.end();

	unsigned npixels = this->pixel_time_series_map.size();
	for( pts_itr = this->pixel_time_series_map.begin(); pts_itr != pts_end; ++pts_itr )
	{
		vcl_cout << "Reducing Time Series Dimension " << npixels - vcl_distance(pts_itr,pts_end) + 1 << " of " << npixels << vcl_endl;

		if( pts_itr->second->ndims() <= ndims2keep )
		{
			vcl_cerr << "----Error: dsm_pixel_time_series_map::reduce_dims----\n"
				<< "\t ndims2keep (" << ndims2keep << " ) less than or equal to ndims (" << pts_itr->second->ndims() << ")\n" << vcl_flush;
			return false;
		}

		if(!pts_itr->second->reduce_feature_dims(ndims2keep))
			return false;
	}

	return true;
}//end dsm_pixel_time_series_map::reduce_dims