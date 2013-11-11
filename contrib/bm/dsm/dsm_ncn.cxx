//this is /contrib/bm/dsm/dsm_ncn.cxx

#include"dsm_ncn.h"

dsm_ncn::dsm_ncn( vcl_string const& video_glob, vcl_vector<vgl_point_2d<unsigned> > const& targets,
						unsigned const& num_neighbors, unsigned const& num_pivot_pixels, unsigned const& num_particles):
video_valid_(true), video_glob_(video_glob), targets_valid_(true), neighborhood_valid_(false), entropy_valid_(false), candidate_pivot_valid_(false), 
	num_neighbors_(num_neighbors), num_pivot_pixels_(num_pivot_pixels), num_particles_(num_particles)
{

	if( this->video_stream_.is_open() )
		this->video_stream_.close();

	this->video_stream_.open( video_glob );
	video_stream_.seek_frame(0);
	unsigned ncols = video_stream_.width();
	unsigned nrows = video_stream_.height();
	unsigned npixels = ncols*nrows;

	(npixels > num_pivot_pixels ) ? this->num_pivot_pixels_ = num_pivot_pixels : this->num_pivot_pixels_ = unsigned( npixels*.75);

	(npixels > num_particles) ? this->num_particles_ = num_particles : this->num_particles_ = npixels;

	vcl_vector<vgl_point_2d<unsigned> >::const_iterator target_itr, target_end = targets.end();

	vcl_vector<vgl_point_2d<unsigned> > empty_vector;
	
	for( target_itr = targets.begin(); target_itr != target_end; ++target_itr )
		this->neighborhood_[*target_itr] = empty_vector;

}//end dsm_ncn::dsm_nc

dsm_ncn::dsm_ncn( vcl_string const& video_glob, vcl_string const& targets_xml_path,
					unsigned const& num_neighbors, unsigned const& num_pivot_pixels, unsigned const& num_particles):
targets_xml_path_(targets_xml_path), video_valid_(true), video_glob_(video_glob), targets_valid_(true), neighborhood_valid_(false), 
	entropy_valid_(false), candidate_pivot_valid_(false), num_neighbors_(num_neighbors), num_pivot_pixels_(num_pivot_pixels), 
		num_particles_(num_particles)
{
	if( this->video_stream_.is_open() )
		this->video_stream_.close();

	this->video_stream_.open( video_glob );
	video_stream_.seek_frame(0);
	unsigned ncols = video_stream_.width();
	unsigned nrows = video_stream_.height();
	unsigned npixels = ncols*nrows;

	(npixels > num_pivot_pixels ) ? this->num_pivot_pixels_ = num_pivot_pixels : this->num_pivot_pixels_ = unsigned( npixels*.75);

	(npixels > num_particles) ? this->num_particles_ = num_particles : this->num_particles_ = npixels;

	this->parse_target_xml(this->targets_xml_path_);

	
}//end dsm_ncn::dsm_ncn

vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >,dsm_vgl_point_2d_coord_compare<unsigned>  > dsm_ncn::neighborhood()
{
	if( !this->neighborhood_valid_ )
		this->build_ncn();

	return this->neighborhood_;
}//end dsm_ncn::neighborhood

bool dsm_ncn::build_ncn()
{
	if( this->video_valid_ && this->targets_valid_ )
	{
		if( !this->entropy_valid_ )
			this->calculate_temporal_entropy();
		if( !this->candidate_pivot_valid_ )
			this->sample_pivot_pixels();

		vcl_cout << "Extracting Neighborhoods..." << vcl_endl;

		vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >,dsm_vgl_point_2d_coord_compare<unsigned> >::iterator
			t_itr, t_end = this->neighborhood_.end();

		vcl_map<unsigned, vil_image_resource_sptr> img_seq;
		this->build_frame_map_(this->video_stream_,img_seq);
		vcl_map<unsigned,vil_image_resource_sptr>::const_iterator img_seq_itr, img_seq_end = img_seq.end();
		vcl_set<vgl_point_2d<unsigned>,dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator pivot_itr, pivot_end = this->pivot_pixel_candidates_.end();
		vil_image_view<vxl_byte> img_view;

		vcl_map<double, vgl_point_2d<unsigned> > mi_point_map;

		for( t_itr = this->neighborhood_.begin(); t_itr != t_end; ++t_itr )
		{
			vgl_point_2d<unsigned> curr_target = t_itr->first;
			vcl_cout << "\t Extracting Neighborhood for target: " << curr_target << vcl_endl;

			//1.(a) rank each pivot pixel candidate by joint entropy with the target.
			//      we do this by calculating the joint entropy at each target with each pivot pixel candidate
			//      then we associate the pivot pixel point with the mi value with a vcl_map.
			//      because vcl_maps internally store keys in ascention, we may pick off the top ranking
			//      candidates from the back of the map. The key will be the mi which will ensure ties are ignored 
			//      and also frees us from using a customized ordering predicate.

			mi_point_map.clear(); //clear from previous iteration
			for( pivot_itr = this->pivot_pixel_candidates_.begin(); pivot_itr != pivot_end; ++pivot_itr )
			{
				unsigned nbins = 16;
				bsta_joint_histogram<double> joint_histogram(255, nbins);

				//don't self include point in neighborhood( mi would be highest with itself).
				if( curr_target != *pivot_itr )
				{
					for( img_seq_itr = img_seq.begin(); img_seq_itr != img_seq_end; ++img_seq_itr )
					{
						img_view = img_seq_itr->second->get_view();
						joint_histogram.upcount(img_view(curr_target.x(),curr_target.y(),0),1,img_view(pivot_itr->x(),pivot_itr->y(),0),1);
					}//end image iteration
				}//end self include check

				mi_point_map[joint_histogram.mutual_information()] = *pivot_itr;

			}//end pivot pixel candidate iteration

			//2. Build the neighborhood by creating and filling in the appropriate data structures.
			vcl_map<double, vgl_point_2d<unsigned> >::const_iterator mi_point_itr = mi_point_map.end();
			// end() is one pas the last element so go to last element
			--mi_point_itr;

			vcl_vector<vgl_point_2d<unsigned> > neighborhood;
			for( unsigned i = 0; i < this->num_neighbors_; ++i, --mi_point_itr )
				neighborhood.push_back(mi_point_itr->second);

			t_itr->second = neighborhood;
		}//end target iteration

		this->neighborhood_valid_ = true;

		vcl_cout << "\t Done Extracting Neighborhoods..." << vcl_endl;
		return true;
	}//end if( this->video_valid_ && this->targets_valid_ )
	else
	{
		if(!this->video_valid_)
			vcl_cerr << "ERROR: dsm_ncn::build_ncn() : video_valid_ = false" << vcl_flush;
		if(!this->targets_valid_)
			vcl_cerr << "ERROR: dsm_ncn::build_ncn() : targets_valid = false" << vcl_flush;
		return false;
	}
	
}//end dsm_ncn::build_ncn

bool dsm_ncn::calculate_temporal_entropy(unsigned const& nbins)
{
	if( this->video_valid_ )
	{
		vcl_cout << "Calculating Temporal Entropy..." << vcl_endl;

		this->temporal_entropy_.clear();

		bsta_histogram<double> histogram(double(0),double(255),nbins);

		unsigned ni = this->video_stream_.width();
		unsigned nj = this->video_stream_.height();

		vcl_map<unsigned, vil_image_resource_sptr> img_seq;
		this->build_frame_map_(this->video_stream_, img_seq);
		vil_image_view<vxl_byte> img_view;

		this->temporal_entropy_.set_size(ni,nj,1);

		double entropy_sum = double(0.0); //cache for normalization

		for( unsigned i = 0; i < ni; ++i )
		{
			for( unsigned j = 0; j < nj; ++ j)
			{
				histogram.clear();
				for( unsigned t = 0; t < img_seq.size() ; ++t )
				{
					img_view = img_seq[t]->get_view();
					histogram.upcount(img_view(i,j,0),1);
				}
				this->temporal_entropy_(i,j,0) = histogram.entropy();
				entropy_sum+=this->temporal_entropy_(i,j,0);
				vcl_cout << '\t' << (float(nj*i + j)/float(ni*nj))*float(100) << "% complete " << vcl_endl;
			}
		}//end pixel iteration

		//normalize
		vcl_cout << "Normalizing Temporal Entropy..." << vcl_endl;
		for( unsigned i = 0; i < ni; ++i )
			for( unsigned  j = 0; j < nj; ++j )
				this->temporal_entropy_(i,j,0)/=entropy_sum;

		this->entropy_valid_ = true;
	}
	else
	{
		vcl_cerr << "ERROR: dsm_ncn::calculate_temporal_entropy no video provided." << vcl_flush;
		this->entropy_valid_ = false;
	}

	return this->entropy_valid_;
}//end dsm_ncn::calculate_temporal_entropy

bool dsm_ncn::sample_pivot_pixels()
{
	if( this->entropy_valid_ == true ) //the temporal entropy must be calcualted prior to this step
	{
		vcl_cout << "Sampling Pivot Pixels..." << vcl_endl;

		this->pivot_pixel_candidates_.clear();

		vnl_random rand;

		vcl_map<vgl_point_2d<unsigned>, double, dsm_vgl_point_2d_coord_compare<unsigned> > particle_map;
		vgl_point_2d<unsigned> point;

		//1. sample the temporal entropy to create a reduced particle map.
		double wsum = double(0.0);
		
		unsigned npixels = this->video_stream_.width() * this->video_stream_.height();
		unsigned nrows = this->video_stream_.height();

		if( npixels > this->num_particles_ )
		{
			while( particle_map.size() < this->num_particles_ )
			{
				unsigned rand_indx = vnl_math_rnd(rand.drand64(0,npixels-1));
				point.set(rand_indx/nrows, rand_indx%nrows);
				particle_map[point] = this->temporal_entropy_(point.x(),point.y());
				wsum+=this->temporal_entropy_(point.x(),point.y());
				vcl_cout << particle_map.size() << " out of " << this->num_particles_ << " chosen." << vcl_endl;
			}
		}
		else //the particle set is the whole image and no need for stochastic particle selection
		{
			unsigned ncols = this->video_stream_.width();
			for( unsigned col = 0; col < ncols; ++col )
				for( unsigned row = 0; row < nrows; ++row )
					particle_map[point] = this->temporal_entropy_(col,row,0);
		}

		//2. reweight each sample and simultaneously construct the cdf
		vcl_map<vgl_point_2d<unsigned>,double,dsm_vgl_point_2d_coord_compare<unsigned>  >::iterator pmit, pmend = particle_map.end();

		//use the stl container so that we can use stl search.
		//the find predicate should be > so we can find the position of the first element of the cdf which is less than
		//the target.
		vcl_vector<double> cdf;
		vcl_vector<vgl_point_2d<unsigned> > cdf_index;

		double cdf_tot = double(0.0);

		for( pmit = particle_map.begin(); pmit != pmend; ++pmit )
		{
			pmit->second = pmit->second/wsum; //normalizing particle map
			cdf_index.push_back(pmit->first);
			cdf_tot += pmit->second;
			cdf.push_back(cdf_tot);
		}
		cdf.push_back(1.0);//last element should be 1 for binary search to work

		//the binary search predicate will find the first element that is larget than the target

		//3. Sample a unique set of size num_piv_pixels via inverse cdf method
		vcl_pair<vcl_set<vgl_point_2d<unsigned>, dsm_vgl_point_2d_coord_compare<unsigned> >::iterator,bool> ret;

		//ADD ELEMENTS VIA INVERSE CDF METHOD
		while( this->pivot_pixel_candidates_.size() < this->num_pivot_pixels_ )
		{
			double u = rand.drand64();
			vcl_vector<double> target(1,u);
			vcl_vector<double>::iterator search_itr = vcl_search(cdf.begin(),cdf.end(),target.begin(), target.end(),&dsm_ncn::binary_search_predicate_);
			unsigned bin;
			if( search_itr != cdf.end() )
			{
				bin = search_itr - cdf.begin();

				if( bin == cdf_index.size() )
				{
					bin = bin - 1;
				}
			}
			else
			{
				vcl_cerr << "ERROR: dsm_ncn::sample_pivot_pixels bin not found in inverse cdf method." << vcl_flush;
				return false;
			}

			//insert point into set which corresponds to the bin
			point.set(cdf_index[bin].x(), cdf_index[bin].y());
			ret = this->pivot_pixel_candidates_.insert(point);
			if( ret.second == true )
				vcl_cout << vcl_setprecision(2) << vcl_fixed
				<< (float(this->pivot_pixel_candidates_.size())/float(this->num_pivot_pixels_)) * 100 
				<< "% pivot pixels sampled." << vcl_endl; 
		}//end while pivot pixel iteration

		this->candidate_pivot_valid_ = true;
		return true;

	}
	else
	{
		vcl_cerr << "ERROR: dsm_ncn::sample_pivot_pixels() : entropy_valid_ = false" << vcl_flush;
		return false;
	}
}//end dsm_ncn::sample_pivot_pixels

void dsm_ncn::build_frame_map_( vidl_image_list_istream const& video_stream, vcl_map<unsigned, vil_image_resource_sptr> & img_seq )
{
	img_seq.clear();
	
	unsigned nframes = this->video_stream_.num_frames();

	vil_image_resource_sptr image_sptr;
	for( unsigned t = 0; t < nframes; ++t )
	{
		vil_image_view<vxl_byte> grey_img, curr_img;
		this->video_stream_.seek_frame(t);
		vidl_convert_to_view(*this->video_stream_.current_frame(),curr_img);
		if(curr_img.nplanes() != 1 || curr_img.pixel_format() == VIL_PIXEL_FORMAT_RGB_BYTE)
		{
			vil_convert_planes_to_grey(curr_img, grey_img);
			image_sptr = vil_new_image_resource_of_view(grey_img);
		}
		else
			image_sptr = vil_new_image_resource_of_view(curr_img);
		
		img_seq[t] = image_sptr;		
	}//end frame iteration

}//end dsm_ncn::build_frame_map_

void dsm_ncn::load_entropy_bin( vcl_string const& filename )
{
	vsl_b_ifstream bis( filename.c_str() );
	vsl_b_read( bis, this->temporal_entropy_ );
	bis.close();
	this->entropy_valid_ = true;
}//end dsm_ncn::load_entropy_bin

bool dsm_ncn::save_entropy_dat( vcl_string const& filename )
{
	if( this->entropy_valid_ == true )
	{
		vcl_ofstream os( filename.c_str(), vcl_ios::out );

		unsigned ni = this->temporal_entropy_.ni();
		unsigned nj = this->temporal_entropy_.nj();

		for( unsigned j = 0; j < nj; ++j )
		{
			for( unsigned i = 0; i < ni; ++i )
				os << this->temporal_entropy_(i,j,0) << "\t";
			os<< '\n';
		}

		return true;
	}
	else
	{
		vcl_cerr << "ERROR: dsm_ncn::save_entropy_dat() : entropy_valid_ = false" << vcl_flush;
		return false;
	}
}//end dsm_ncn::save_entropy_dat

bool dsm_ncn::save_entropy_bin( vcl_string const& filename )
{
	if( this->entropy_valid_ == true )
	{
		vsl_b_ofstream bofs( filename.c_str() );
		vsl_b_write( bofs, this->temporal_entropy_ );
		bofs.close();
		return true;
	}
	else
	{
		vcl_cerr << "ERROR: dsm_ncn::save_entropy_bin() : entropy_valid_ = false" << vcl_flush;
		return false;
	}
}//end dsm_ncn::save_entropy_bin

bool dsm_ncn::write_neighborhood_mfile( vcl_string const& filename )
{
	if( this->neighborhood_valid_ == true )
	{
		vcl_ofstream os( filename.c_str(), vcl_ios::out );

		os << "neighborhoods = cell(" << this->neighborhood_.size() << ", 2);\n";

		vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >,dsm_vgl_point_2d_coord_compare<unsigned>  >::const_iterator 
								t_itr, t_end = this->neighborhood_.end();
		
		vcl_vector<vgl_point_2d<unsigned> >::const_iterator n_itr, n_end;

		//MATLAB INDEXING STARTS AT 1!
		unsigned cell_indx;
		for( t_itr = this->neighborhood_.begin(), cell_indx = 1; t_itr != t_end; ++t_itr )
		{
			os << "neighborhoods{" << cell_indx << ",1} = [" << t_itr->first.x() << '\t' << t_itr->first.y() << "];\n";

			n_end = t_itr->second.end();

			for( n_itr = t_itr->second.begin(); n_itr != n_end; ++n_itr )
				os << n_itr->x() << '\t' << n_itr->y()  << ";\n";

			os << "];\n";
			
		}//end target iteration

		return true;
	}
	else
	{
		vcl_cerr << "ERROR: dsm_ncn::write_neighborhood_mfile() : neighborhood_valid_ = false" << vcl_flush;
		return false;
	}
}//end dsm_ncn::write_neighborhood_mfile

void dsm_ncn::load_video( vcl_string const& filename )
{
	if( this->video_stream_.is_open() )
		this->video_stream_.close();

	this->video_valid_ = this->video_stream_.open( filename );
	video_stream_.seek_frame(0);
	
}//end dsm_ncn::load_video

void dsm_ncn::b_write(vsl_b_ostream &os) const
{
	const short version_no = 1;
	vsl_b_write(os, version_no);
	vsl_b_write(os, this->video_valid_);
	vsl_b_write(os, this->targets_valid_);
	vsl_b_write(os, this->neighborhood_valid_);
	vsl_b_write(os, this->entropy_valid_);
	vsl_b_write(os, this->candidate_pivot_valid_);
	vsl_b_write(os, this->num_pivot_pixels_);
	vsl_b_write(os, this->num_particles_);
	vsl_b_write(os, this->num_neighbors_);
	
	if( this->video_valid_ )
	{
		vcl_string video_glob = this->video_stream_.current_path();

		vsl_b_write(os, video_glob);
	}

	if(this->targets_valid_)
	{
		unsigned num_targets = this->neighborhood_.size();
		vsl_b_write(os, num_targets);

		vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator t_itr, t_end;
		t_end = this->neighborhood_.end();

		for( t_itr = this->neighborhood_.begin(); t_itr != t_end; ++t_itr )
		{
			vsl_b_write(os, t_itr->first);
			if(this->neighborhood_valid_)
			{
				vcl_vector<vgl_point_2d<unsigned> >::const_iterator n_itr, n_end;
				n_end = t_itr->second.end();
				for( n_itr = t_itr->second.begin(); n_itr != n_end; ++n_itr )
					vsl_b_write(os, *n_itr);
			}

		}
	}

	if( this->entropy_valid_)
		vsl_b_write(os, this->temporal_entropy_);

	if( this->candidate_pivot_valid_ )
	{
		vcl_set<vgl_point_2d<unsigned>, dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator pc_itr, pc_end = this->pivot_pixel_candidates_.end();

		for( pc_itr = this->pivot_pixel_candidates_.begin(); pc_itr != pc_end; ++pc_itr )
			vsl_b_write(os, *pc_itr);
	}

	return;
}//end dsm_ncn::b_write

void dsm_ncn::b_read(vsl_b_istream &is)
{
	if(!is) return;

	short v;
	vsl_b_read(is,v);

	switch(v)
	{
	case 1:
	{
		vsl_b_read(is, this->video_valid_);
		vsl_b_read(is, this->targets_valid_);
		vsl_b_read(is, this->neighborhood_valid_);
		vsl_b_read(is, this->entropy_valid_);
		vsl_b_read(is, this->candidate_pivot_valid_);
		vsl_b_read(is, this->num_pivot_pixels_);
		vsl_b_read(is, this->num_particles_);
		vsl_b_read(is, this->num_neighbors_);

		if( this->video_valid_ )
		{
			vcl_string video_glob;
			vsl_b_read(is, video_glob);
			this->load_video(video_glob);
		}//end if( this->video_valid_ )

		if( this->neighborhood_valid_ )
		{
			unsigned num_targets;
			vsl_b_read(is,num_targets);

			for( unsigned i = 0; i < num_targets; ++i )
			{
				vgl_point_2d<unsigned> curr_target;
				vsl_b_read(is,curr_target);
				
				vcl_vector<vgl_point_2d<unsigned> > neighborhood;
				for( unsigned j = 0; j < this->num_neighbors_; ++j )
				{
					vgl_point_2d<unsigned> neighbor;
					vsl_b_read(is,neighbor);
					neighborhood.push_back(neighbor);
				}//end neighborhood iteration

				this->neighborhood_[curr_target] = neighborhood;
			}//end target iteration
		}//end if( this->neighborhood_valid_ )

		
		if( this->entropy_valid_ )
			vsl_b_read(is, this->temporal_entropy_);

		if( this->candidate_pivot_valid_ )
		{
			for( unsigned i = 0; i < this->num_pivot_pixels_; ++i )
			{
				vgl_point_2d<unsigned> curr_pivot_pix;
				vsl_b_read(is, curr_pivot_pix);
				this->pivot_pixel_candidates_.insert(curr_pivot_pix);
			}
		}//end if(this->candidate_pivot_valid_)
		
		break;
	}
	default:
	{
		vcl_cerr << "I/O ERROR: vsl_b_read(vsl_b_istream&, dsm_ncn ncn)\n"
                     << "           Unknown version number "<< v << '\n';
		is.is().clear(vcl_ios::badbit); // Set an unrecoverable IO error on stream
		return;
	}
	}

	return;
}//end dsm_ncn::b_read


bool dsm_ncn::write_neighborhood_xml( vcl_string const& filename )
{
	vcl_ofstream os(filename.c_str(),vcl_ios::out);
	bxml_document doc;
	bxml_element* root = new bxml_element("dsmNeighborhood");
	doc.set_root_element(root);
	root->append_text("\n");
	root->set_attribute("num_targets",this->neighborhood_.size());
	root->set_attribute("num_neighbors", this->num_neighbors_);
	root->set_attribute("video_glob", this->video_glob_);
	
	vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dsm_vgl_point_2d_coord_compare<unsigned>  >::const_iterator 
			target_itr, target_end = this->neighborhood_.end();

	vcl_vector<vgl_point_2d<unsigned> >::const_iterator n_itr, n_end;

	for( target_itr = this->neighborhood_.begin(); target_itr != target_end; ++target_itr )
	{
		bxml_data_sptr target = new bxml_element("target");
		

		bxml_element* tp = static_cast<bxml_element*>(target.as_pointer());
		tp->set_attribute("x", target_itr->first.x());
		tp->set_attribute("y", target_itr->first.y());

		for(n_itr = target_itr->second.begin(), n_end = target_itr->second.end(); n_itr != n_end; ++n_itr)
		{
			bxml_data_sptr neighbor = new bxml_element("neighbor");
			bxml_element* np = static_cast<bxml_element*>( neighbor.as_pointer() );
			np->set_attribute("x",n_itr->x());
			np->set_attribute("y",n_itr->y());
			tp->append_text("\n");
			tp->append_text("\t");
			tp->append_data(neighbor);
		}
		tp->append_text("\n");
		root->append_text("\n");
		root->append_data(target);
		root->append_text("\t");
		root->append_text("\n");
	}//end target iteration

	bxml_write(os,doc);
	os.close();

	return true;

}//end dsm_ncn::write_neighborhood_xml

bool dsm_ncn::parse_target_xml( vcl_string const& targets_xml_path )
{
	//Open the XML document
	if( targets_xml_path.size() == 0 )
	{
		vcl_cerr << "dsm_ncn::parse_target_xml -- xml file path is not set" << vcl_flush;
		this->targets_valid_ = false;
		return false;
	}

	//Check if you can find the root xml node
	bxml_document xml_doc = bxml_read(targets_xml_path);
	if( !xml_doc.root_element() )
	{
		vcl_cerr << "dsm_ncn::parse_target_xml -- xml root not found" << vcl_flush;
		this->targets_valid_ = false;
		return false;
	}

	//Check the root is an element
	if( xml_doc.root_element()->type() != bxml_data::ELEMENT )
	{
		vcl_cerr << "dsm_ncn::parse_target_xml -- params root is not an ELEMENT" << vcl_flush;
		this->targets_valid_ = false;
		return false;
	}

	bxml_data_sptr root = xml_doc.root_element();
	if(!root)
	{
		vcl_cerr << "dsm_ncn::parse_target_xml -- root tag not found!" << vcl_flush;
		this->targets_valid_ = false;
		return false;
	}

	bxml_element* h_elm = static_cast<bxml_element*>(root.ptr());
	for( bxml_element::const_data_iterator i = h_elm->data_begin(); i != h_elm->data_end(); ++i )
	{
		bxml_data_sptr elm = *i;
		if( elm->type() == bxml_data::ELEMENT )
		{
			bxml_element* target = static_cast<bxml_element*>(elm.as_pointer());
			if( target )
			{
				vcl_string type;
				target->get_attribute("type", type);
				if( !type.compare("unsigned_2d") )
				{
					unsigned x, y;
					target->get_attribute("x",x);
					target->get_attribute("y",y);
					vgl_point_2d<unsigned> target(x,y);
					//create an empty neighborhood
					vcl_vector<vgl_point_2d<unsigned> > neighborhood;
					this->neighborhood_[target] = neighborhood;
				}//end if(!type.compare("unsigned_2d")

			}//end if(target
		}//end if(elm->type() == bxml_data::ELEMENT)
	}//end element iteration

	this->targets_valid_ = true;
	return this->targets_valid_;

}//end dsm_ncn::parse_target_xml

void dsm_ncn::set_video_glob( vcl_string const& video_glob )
{
	if( this->video_stream_.is_open() )
		this->video_stream_.close();

	this->video_stream_.open(video_glob);
	video_stream_.seek_frame(0);
	unsigned ncols = video_stream_.width();
	unsigned nrows = video_stream_.height();
	unsigned npixels = ncols*nrows;

	if(npixels < this->num_pivot_pixels_)
		this->num_pivot_pixels_ = .75*npixels;

	if(npixels < this->num_particles_)
		this->num_particles_ = npixels;

	this->video_valid_ = true;
}//end set_video_glob

void dsm_ncn::add_target( vgl_point_2d<unsigned> const& target )
{
	vcl_vector<vgl_point_2d<unsigned> > neighborhood;
	this->neighborhood_[target] = neighborhood;
	if(!this->targets_valid_){this->targets_valid_=true;}
}//end add_target

bool dsm_ncn::set_num_particles(unsigned const& np)
{
	if(this->video_valid_)
	{
		unsigned ncols = video_stream_.width();
		unsigned nrows = video_stream_.height();
		unsigned npixels = ncols*nrows;

		if(npixels < np)
		{
			vcl_cout << "dsm_ncn::set_num_particles ---- WARNING ----\n"
					 << "number of pixels per frame: " << npixels << "\n"
					 << "exceeds requested number of particles: " << np << "\n"
					 << "USING MAXIMUM NUMBER OF PIXELS, NOT RESETTING THE NUMBER OF PARTICLES." << vcl_endl;
			this->num_particles_ = npixels;
			return false;
		}
		else
		{
			this->num_particles_ = np;
			return true;
		}
	}
	else
		this->num_particles_ = np;

	return true;
}//end set_num_particles

bool dsm_ncn::set_num_pivot_pixels(unsigned const& npp)
{
	if(this->video_valid_)
	{
		unsigned ncols = video_stream_.width();
		unsigned nrows = video_stream_.height();
		unsigned npixels = ncols*nrows;

		if(npixels < npp)
		{
			vcl_cout << "dsm_ncn::set_num_pivot_pixels ---- WARNING ----\n"
					 << "number of pixels per frame: " << npixels << "\n"
					 << "exceeds requested number of pivot pixels: " << npp << "\n"
					 << "USING MAXIMUM NUMBER OF PIXELS NOT REQUESTED NUMBER OF PIVOT PIXELS." << vcl_endl;
			this->num_pivot_pixels_ = npixels;
			return false;
		}
		else
		{
			this->num_pivot_pixels_ = npp;
			return true;
		}
	}

	this->num_pivot_pixels_ = npp;
	return true;
}//end set_num_pivot_pixels

bool dsm_ncn::write_neighborhood_txt( vcl_string const& filename )
{
	if(this->neighborhood_valid_)
	{

		vcl_ofstream of( filename.c_str(), vcl_ios::out );

		vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dsm_vgl_point_2d_coord_compare<unsigned> >::iterator
			nitr, nend = this->neighborhood_.end();

		for( nitr = this->neighborhood_.begin(); nitr != nend; ++nitr )
		{
			of << nitr->first.x() << '\t';
			
			vcl_vector<vgl_point_2d<unsigned> >::const_iterator vitr, vend = nitr->second.end();

			for( vitr = nitr->second.begin(); vitr != vend; ++vitr )
			{
				of << vitr->x() << '\t';
			}//end neighborhood x-coordinate iteration

			of << '\n';

			of << nitr->first.y() << '\t';

			for( vitr = nitr->second.begin(); vitr != vend; ++vitr )
			{
				of << vitr->y() << '\t';
			}//end neighborhood y-coordinate iteration

			of << '\n' << '\n';
		}//end target iteration

		of.close();
		return true;
	}
	
	vcl_cerr << "COULD NOT WRITE NEIGHBORHOOD TXT FILE, THE NEIGHBORHOOD IS NOT VALID." << vcl_flush;
	return false;
}//end dsm_ncn::write_neighborhood_txt


bool dsm_ncn::write_pivot_pixel_candidates_txt( vcl_string const& filename )
{
	if( this->candidate_pivot_valid_ )
	{
		vcl_ofstream of( filename.c_str(), vcl_ios::out );

		vcl_set<vgl_point_2d<unsigned>, dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator pitr, pend = this->pivot_pixel_candidates_.end();

		for( pitr = this->pivot_pixel_candidates_.begin(); pitr != pend; ++pitr )
		{
			of << pitr->x() << '\t' << pitr->y() << '\n';
		}

		of.close();
		return true;
	}
	else
	{
		vcl_cerr << "---------------- WARNING ----------------" <<'\n'
				 << "dsm_ncn::write_pivot_pixel_candidates_txt( vcl_string const& " << filename << " )\n"
				 << "\t pivot pixel candidates not valid. Could not write txt file.\n" << vcl_flush;
		return false;
	}
}