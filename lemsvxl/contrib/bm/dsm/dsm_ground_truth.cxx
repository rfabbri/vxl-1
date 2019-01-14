//this is /contrib/bm/dsm/dsm_ground_truth.cxx
#include"dsm_ground_truth.h"

void dsm_ground_truth::build_change_maps(unsigned const& ni, unsigned const& nj)
{
	if( this->frame_polygon_map.empty() )
	{
		std::cerr << "---ERROR--- dsm_ground_truth::build_change_maps()\n"
			     << "\t Error building change maps: gt polygons not yet loaded." << std::flush;
		return; 
	}

	if( this->frame_polygon_map.size() != this->frame_change_type_map.size() )
	{
		std::cerr << "---ERROR--- dsm_ground_truth::build_change_maps()\n"
			     << "\t Error building change maps: frame_polygon_map.size() != frame_change_type_map.size()" << std::flush;
		return;
	}

	std::map<unsigned, std::vector<vsol_polygon_2d_sptr> >::const_iterator
		p_itr, p_end = this->frame_polygon_map.end();

	std::map<vgl_point_2d<int>, std::vector<std::string>, dsm_vgl_point_2d_coord_compare<int> > location_change_types;

	unsigned built;
	for( p_itr = this->frame_polygon_map.begin(), built = 1; p_itr != p_end; ++p_itr, ++built )
	{
		unsigned curr_frame = p_itr->first;

		std::vector<vgl_point_2d<double> >changes;

		vil_image_view<vxl_byte> curr_change_view(ni,nj,1);

		curr_change_view.fill(vxl_byte(0));

		std::cout << "Building frame " << built << " out of " << this->frame_polygon_map.size() << std::endl;
		
		std::map<unsigned, std::vector<std::string> >::const_iterator c_itr = this->frame_change_type_map.find(curr_frame);

		if( c_itr == this->frame_change_type_map.end() )
		{
			std::cerr << "---ERROR--- dsm_ground_truth::build_change_maps()\n"
			         << "\t Error cannot find the change type vector for frame " << curr_frame << std::flush;
			return;
		}

		if( p_itr->second.size() != c_itr->second.size() )
		{
			std::cerr << "---ERROR--- dsm_ground_truth::build_change_maps()\n"
			         << "\t Error buildling change maps: There isn't a 1-1 correspondence between polygons and change types for frame: " << curr_frame << std::flush;
			return;
		}

		for( unsigned poly_idx = 0; poly_idx < p_itr->second.size(); ++poly_idx )
		{
			std::string curr_change_type = c_itr->second[poly_idx];
			std::cout << "\tProcessing Polygon: " << poly_idx + 1 << " out of " << p_itr->second.size() << std::endl;
			std::cout << "\t\tCurrent Change Type is: " << curr_change_type << std::endl;

			vgl_polygon<double> vgl_poly = bsol_algs::vgl_from_poly(p_itr->second[poly_idx]);
			
			vgl_polygon_scan_iterator<double> psi(vgl_poly,false);

			for( psi.reset(); psi.next(); )
			{
				int y = psi.scany();

				for( int x = psi.startx(); x <= psi.endx(); ++x )
				{
					// check the pixel index is within the image
					if( x < 0 || y < 0 )
						continue;
					if( x >= ni || y >= nj )
						continue;

					vgl_point_2d<int> pt(x,y);

					//check if the change type had previously been observed at the location
					std::map< vgl_point_2d<int>, std::vector<std::string>,
						dsm_vgl_point_2d_coord_compare<int> >::iterator lc_itr = location_change_types.find(pt);

					if( lc_itr != location_change_types.end() )
					{
						std::vector<std::string>::iterator s_itr = std::find(lc_itr->second.begin(), lc_itr->second.end(), curr_change_type);

						if( s_itr == lc_itr->second.end() )
						{
							curr_change_view(pt.x(),pt.y(),0) = vxl_byte(255);
							lc_itr->second.push_back(curr_change_type);
						}
					}
					else
					{
						curr_change_view(pt.x(),pt.y(),0)= vxl_byte(255);
						location_change_types[pt].push_back(curr_change_type);
					}
				}//end x dimension scan
			}//end polygon scan iteration
		}//end polygon list iteration
		this->change_maps[curr_frame] = curr_change_view;
	}//end frame iteration
}//end build_change_maps

std::map<unsigned, vil_image_view<vxl_byte> > dsm_ground_truth::build_change_map_classical(unsigned const& ni, unsigned const& nj) const
{
	if( this->frame_polygon_map.empty() )
	{
		std::cerr << "---ERROR--- dsm_ground_truth::build_change_map_classical()\n"
			     << "\t Error building change maps: gt polygons not yet loaded." << std::flush;
		exit(1); 
	}

	if( this->frame_polygon_map.size() != this->frame_change_type_map.size() )
	{
		std::cerr << "---ERROR--- dsm_ground_truth::build_change_map_classical()\n"
			     << "\t Error building change maps: frame_polygon_map.size() != frame_change_type_map.size()" << std::flush;
		exit(1);
	}

	std::map<unsigned, std::vector< vsol_polygon_2d_sptr> >::const_iterator
		p_itr, p_end = this->frame_polygon_map.end();

	std::map<unsigned, vil_image_view<vxl_byte> > change_map;

	unsigned built;
	for( p_itr = this->frame_polygon_map.begin(), built = 1; p_itr != p_end; ++p_itr, ++built )
	{
		unsigned curr_frame = p_itr->first;

		vil_image_view<vxl_byte> curr_change_view(ni,nj,1);

		curr_change_view.fill(vxl_byte(0));

		std::cout << "Building frame " << built << " out of " << this->frame_polygon_map.size() << std::endl;

		std::map<unsigned, std::vector<std::string> >::const_iterator c_itr = this->frame_change_type_map.find(curr_frame);

		if( c_itr == this->frame_change_type_map.end() )
		{
			std::cerr << "---ERROR--- dsm_ground_truth::build_change_maps()\n"
			         << "\t Error cannot find the change type vector for frame " << curr_frame << std::flush;
			exit(1);
		}

		if( p_itr->second.size() != c_itr->second.size() )
		{
			std::cerr << "---ERROR--- dsm_ground_truth::build_change_maps()\n"
			         << "\t Error buildling change maps: There isn't a 1-1 correspondence"
					 << " between polygons and change types for frame: " << curr_frame << std::flush;
			exit(1);
		}

		for( unsigned poly_idx = 0; poly_idx < p_itr->second.size(); ++poly_idx )
		{
			if( c_itr->second[poly_idx].compare("no change") != 0 )
			{
				std::cout << "\tProcessing Polygon: " << poly_idx + 1 
					     << " out of " << p_itr->second.size() << std::endl;
				std::cout << "\t\tCurrent Change Type is: " << c_itr->second[poly_idx] << std::endl;

				vgl_polygon<double> vgl_poly = bsol_algs::vgl_from_poly(p_itr->second[poly_idx]);

				vgl_polygon_scan_iterator<double> psi(vgl_poly,false);

				for( psi.reset(); psi.next(); )
				{
					int y = psi.scany();

					for( int x = psi.startx(); x <= psi.endx(); ++x )
					{
						//check if pixel index is within the image
						if( x < 0 || y < 0 || x >=ni || y >= nj )
							continue;

						curr_change_view(x,y,0) = vxl_byte(255);		
					}//end x iteration
				}//end polygon scan iteration (y)
			}//if the polygon represents a change
		}//end polygon iteration
		change_map[curr_frame] = curr_change_view;
	}//end frame iteration

	return change_map;
}//end build_change_map_classical

void dsm_ground_truth::
	save_change_maps_classical_tiff(unsigned const& ni, 
		unsigned const& nj, std::string const& result_dir) const
{
	std::map<unsigned, vil_image_view<vxl_byte> > 
		change_maps = this->build_change_map_classical(ni,nj);

	std::map<unsigned, vil_image_view<vxl_byte> >::const_iterator 
		c_itr, c_end = change_maps.end();

	if(!vul_file::is_directory(result_dir))
		vul_file::make_directory(result_dir);

	for( c_itr = change_maps.begin(); c_itr != c_end; ++c_itr )
	{
		std::stringstream filename;

		filename << result_dir << "/classical_change_map_"
			     << std::setfill('0') << std::setw(8)
				 << c_itr->first << ".tiff";

		std::cout << "\tSaving classical change map:\n"
			     << "\t\t" << filename.str() << std::endl;

		vil_save(c_itr->second, filename.str().c_str());
	}//end frame iteration

}//end save_change_maps_classical_tiff

void dsm_ground_truth::save_change_maps_tiff( std::string const& result_dir )
{
	std::map<unsigned, vil_image_view<vxl_byte> >::const_iterator
		cm_itr, cm_end = this->change_maps.end();

	if( !vul_file::is_directory(result_dir) )
		vul_file::make_directory(result_dir);

	unsigned idx;
	for( cm_itr = this->change_maps.begin(), idx = 0; cm_itr != cm_end; ++cm_itr, ++idx )
	{
		std::cout << "Writing Frame: " << idx << " out of " << this->change_maps.size() << std::endl;
		std::stringstream filename;

		filename << result_dir << "/change_map_" << std::setfill('0') << std::setw(8) << cm_itr->first << ".tiff";

		vil_save(cm_itr->second, filename.str().c_str());
	}//end change map iteration
}//end save_change_maps_tiff

void dsm_ground_truth::b_write_bwm_gt(std::string const& filename) const
{
	vsl_b_ofstream os(filename.c_str(), std::ios::out|std::ios::binary);

	if( frame_polygon_map.size() != this->frame_change_type_map.size() )
	{
		std::cout << "---ERROR--- dsm_ground_truth::b_write_bwm_gt( vsl_b_ostream& os)\n"
			     << "\t Error writing binary files: frame_polygon_map.size() != frame_change_type_map.size()";
		return;
	}

	unsigned nframes = frame_polygon_map.size();

	std::map<unsigned, std::vector<vsol_polygon_2d_sptr> >::const_iterator 
		f_itr, f_end = this->frame_polygon_map.end();

	//write the number of frames
	vsl_b_write(os, nframes);

	for( f_itr = this->frame_polygon_map.begin(); f_itr != f_end; ++f_itr )
	{
		unsigned nchanges = f_itr->second.size();

		//write the frame number
		unsigned fnumber=f_itr->first;
		vsl_b_write(os,f_itr->first);

		//write the number of changes
		vsl_b_write(os, nchanges);

		for( unsigned i = 0; i < f_itr->second.size(); ++i )
		{	
			//write the polygon
			vsl_b_write(os,f_itr->second[i].as_pointer());

			std::map<unsigned, std::vector<std::string> >::const_iterator
				c_itr = this->frame_change_type_map.find( fnumber );
			if( c_itr != this->frame_change_type_map.end() )
			{
			//write the change type
			vsl_b_write(os, c_itr->second[i]);
			}
			else
			{
				std::cout << "---ERROR--- dsm_ground_truth::b_write( vsl_b_ostream& os)\n"
						 << "\t Error writing binary files: this->frame_change_type_map.find(fnumber) == this->frame_change_type_map.end()";
				return;
			}
		}//end polygon/changetype iteration

	}//end frame iteration

	os.close();
	
}//end b_write


void dsm_ground_truth::b_read_bwm_gt(std::string const& filename)
{

	vsl_b_ifstream is(filename.c_str(), std::ios::in|std::ios::binary);

	unsigned nframes = 0; 

	vsl_b_read(is, nframes);

	if( nframes < 1 )
	{
		std::cout << "---ERROR--- dsm_ground_truth::b_read( vsl_b_istream& is )\n"
			     << "\t Error reading binary file: nframes < 1" << std::flush;
		return;
	}

	this->frame_polygon_map.clear();
	this->frame_change_type_map.clear();
	this->change_maps.clear();

	for( unsigned frame_idx = 0; frame_idx < nframes; ++frame_idx )
	{
		//read the frame number
		unsigned frame;
		vsl_b_read(is,frame);

		//read the number of changes
		unsigned nchanges;
		vsl_b_read(is, nchanges);

		std::vector<vsol_polygon_2d_sptr> polygon_vect;
		std::vector<std::string> change_type_vect;
		for( unsigned i = 0; i < nchanges; ++i )
		{
			//read the polygon
			vsol_polygon_2d* polygon_ptr = new vsol_polygon_2d;
			vsl_b_read(is, polygon_ptr);
			vsol_polygon_2d_sptr polygon_sptr(polygon_ptr);
			//this->frame_polygon_map[frame].push_back(polygon_sptr);
			polygon_vect.push_back(polygon_sptr);

			//read the change type
			std::string change_type;
			vsl_b_read(is,change_type);
			//this->frame_change_type_map[frame].push_back(change_type);
			change_type_vect.push_back(change_type);
		}//end change iteration
		this->frame_polygon_map[frame] = polygon_vect;
		this->frame_change_type_map[frame] = change_type_vect;
	}//end frame iteration

	is.close();

}//end b_read

void dsm_ground_truth::b_write( vsl_b_ostream& os ) const
{

	const short version_no = 1;
	vsl_b_write(os, version_no);

	std::map<unsigned, std::vector<vsol_polygon_2d_sptr> >::const_iterator
		p_itr, p_end = this->frame_polygon_map.end();

	//write the size of the frame_polygon_map
	vsl_b_write(os, this->frame_polygon_map.size());

	for( p_itr = this->frame_polygon_map.begin(); p_itr != p_end; ++p_itr )
	{
		//write the frame number
		vsl_b_write(os, p_itr->first);

		//write the number of polygons
		vsl_b_write(os, p_itr->second.size());

		for(unsigned i = 0; i < p_itr->second.size(); ++i)
			vsl_b_write(os, p_itr->second[i].as_pointer());
	}//end polygon map iteration

	std::map<unsigned, std::vector<std::string> >::const_iterator
		c_itr, c_end = this->frame_change_type_map.end();

	//write the size of the frame change type map
	vsl_b_write(os, frame_change_type_map.size());

	for( c_itr = this->frame_change_type_map.begin(); c_itr != c_end; ++c_itr )
	{
		//write the frame number
		vsl_b_write(os, c_itr->first);

		//write the number of change types
		vsl_b_write(os, c_itr->second.size());

		for(unsigned i = 0; i < c_itr->second.size(); ++i)
			vsl_b_write(os, c_itr->second[i]);
	}//end change type map iteration

	std::map<unsigned, vil_image_view<vxl_byte> >::const_iterator
		cm_itr, cm_end = this->change_maps.end();

	//write the size of the change_map
	vsl_b_write(os, this->change_maps.size());

	for( cm_itr = this->change_maps.begin(); cm_itr != cm_end; ++cm_itr )
	{
		//write the frame number
		vsl_b_write(os, cm_itr->first);

		//write the image view
		vsl_b_write(os, cm_itr->second);
	}//end change map iteration
}//end b_write

void dsm_ground_truth::b_read( vsl_b_istream& is )
{
	if(!is) return;

	short v;
	vsl_b_read(is,v);

	switch(v)
	{
	case 1:
		{
			//read the size of the polygon map
			unsigned poly_map_size = 0;
			vsl_b_read(is, poly_map_size);

			for(unsigned i = 0; i < poly_map_size; ++i)
			{
				//read the frame number
				unsigned frame = 0;
				vsl_b_read(is, frame);

				//read the number of polygons
				unsigned n_polys = 0;
				vsl_b_read(is,n_polys);

				std::vector<vsol_polygon_2d_sptr> poly_vect;
				for( unsigned j = 0; j < n_polys; ++j )
				{
					vsol_polygon_2d* poly_ptr = new vsol_polygon_2d;
					vsl_b_read(is,poly_ptr);
					vsol_polygon_2d_sptr poly_sptr(poly_ptr);
					poly_vect.push_back(poly_sptr);
				}
				this->frame_polygon_map[frame] = poly_vect;
			}//end polygon iteration

			unsigned change_type_map_size = 0;
			vsl_b_read(is, change_type_map_size);

			for(unsigned i = 0; i < change_type_map_size; ++change_type_map_size)
			{
				//read the frame number
				unsigned frame = 0;
				vsl_b_read(is,frame);

				//read the number of change types
				unsigned n_changes = 0;
				vsl_b_read(is, n_changes);

				std::vector<std::string> change_type_vect;
				for(unsigned j = 0; j < n_changes; ++j)
				{
					std::string change_type = "";
					//read the change type
					vsl_b_read(is, change_type);
					change_type_vect.push_back(change_type);
				}//end change type vector iteration
				this->frame_change_type_map[frame] = change_type_vect;
			}//end change type iteration

			//read the number of change maps
			unsigned n_change_maps = 0;
			vsl_b_read(is, n_change_maps);

			for(unsigned i = 0; i < n_change_maps; ++i)
			{
				//read the frame number
				unsigned frame = 0;
				vsl_b_read(is, frame);

				//read the image view
				vil_image_view<vxl_byte> img_view;
				vsl_b_read(is, img_view);

				this->change_maps[frame] = img_view;
			}//end change map iteration

		}//end case 
	default:
		{
			std::cerr << "----I/O ERROR: dsm_ground_truth::b_read ----\n"
				     << "	 UNKNOWN VERSION NUMBER " << v << "\n";
			is.is().clear(std::ios::badbit); //set an unrecoverable IO error on stream
			return;
		}//end default
	}//end switch(v)
}
