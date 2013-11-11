//this is /contrib/bm/dbcl/dbcl_ncn_extractor.cxx
#include"dbcl_ncn_extractor.h"

dbcl_ncn_extractor::dbcl_ncn_extractor( vcl_string const& video_glob, vcl_vector<vgl_point_2d<unsigned> > const& target_points, 
						unsigned const& num_pivot_pixels, unsigned const& num_particles, unsigned const& num_neighbors )
						:neighborhood_valid_(false),entropy_valid_(false),pivot_set_valid_(false)
{
	this->build_frame_map(video_glob);

	vcl_vector<vgl_point_2d<unsigned> >::const_iterator target_itr, target_end;
	target_end = target_points.end();

	//create dummy neighborhood so we can store the targets in the neighborhood map.
	//the neighborhood_valid_ flag will still read false, indicating that the neighborhood hasn't yet
	//been extracted
	for( target_itr = target_points.begin(); target_itr != target_end; ++target_itr )
	{
		vcl_vector<vgl_point_2d<unsigned> > dummy_neighborhood;
		this->target_ncn_map_[*target_itr] = dummy_neighborhood;
	}//end target iteration
	
	unsigned ncols = this->img_seq_[0]->ni();
	unsigned nrows = this->img_seq_[0]->nj();
	unsigned npixels = ncols*nrows;

	if( npixels > num_pivot_pixels )
		this->num_pivot_pixels_ = num_pivot_pixels;
	else
		this->num_pivot_pixels_ = unsigned( npixels*.75);
	
	if( npixels > num_particles )
		this->num_particles_ = num_particles;
	else
		num_particles_ = npixels;

}//end dbcl_ncn_extractor::dbcl_ncn_extractor

bool dbcl_ncn_extractor::build_ncn()
{
	this->calculate_temporal_entropy();
	this->sample_pivot_pixels();
	return this->extract_neighbors();
}//end dbcl_ncn_extractor::build_ncn()

bool dbcl_ncn_extractor::build_ncn_no_entropy()
{
	if(this->entropy_valid_ == true)
	{
		this->sample_pivot_pixels();
		return this->extract_neighbors();
	}
	else
	{
		vcl_cerr << "ERROR: dbcl_ncn_extractor::build_no_entropy() : entropy_valid_ = false" << vcl_flush;
		return false;
	}
}//end dbcl_ncn_extractor::build_ncn_no_entropy()

bool dbcl_ncn_extractor::load_video(vcl_string const& video_glob)
{
	this->build_frame_map(video_glob);
	this->calculate_temporal_entropy();
	this->sample_pivot_pixels();
	return this->extract_neighbors();
}//end dbcl_ncn_extractor::load_video

void dbcl_ncn_extractor::calculate_temporal_entropy(unsigned const& nbins)
{

	vcl_cout << "Calculating Temproal Entropy..." << vcl_endl;

	this->temporal_entropy_.clear();

	bsta_histogram<double> histogram(double(0),double(255),nbins);

	unsigned ni = this->img_seq_[0]->ni();
	unsigned nj = this->img_seq_[0]->nj();

	this->temporal_entropy_.set_size(ni,nj,1);

	double entropy_sum = double(0.0); //cache for normalization

	vil_image_view<vxl_byte> img_view;

	for(unsigned i = 0; i < ni; ++i)
	{
		for(unsigned j = 0; j < nj; ++j)
		{
			histogram.clear();
			for(unsigned t = 0; t < this->img_seq_.size(); ++t)
			{
				img_view = this->img_seq_[t]->get_view();
				histogram.upcount(img_view(i,j,0),1);
			}//end frame iteration
			this->temporal_entropy_(i,j,0) = histogram.entropy();
			entropy_sum+=temporal_entropy_(i,j,0);
			vcl_cout << (float(nj*i + j)/float(ni*nj))*float(100) << "% complete " << vcl_endl;
		}
	}//end pixel iteration

	//normalize
	vcl_cout << "Normalizeing Temporal Entropy..." << vcl_endl;
	for( unsigned i = 0; i < ni; ++i )
		for( unsigned j = 0; j < nj; ++j )
			this->temporal_entropy_(i,j,0)/=entropy_sum;

	this->entropy_valid_ = true;

}//end dbcl_ncn_extractor::calculate_temporal_entropy

bool dbcl_ncn_extractor::sample_pivot_pixels()
{
	if( this->entropy_valid_ == true) //the temporal entropy needs to be calculated by this step
	{
		vcl_cout << "Sampling Pivot Pixels..." << vcl_endl;

		pivot_pixel_set_type pivot_pixel_set;

		vnl_random rand;
		unsigned num_pixels = this->temporal_entropy_.size();
		unsigned num_cols = this->temporal_entropy_.ni();
		unsigned num_rows = this->temporal_entropy_.nj();

		vcl_map<vgl_point_2d<unsigned>, double, dbcl_vgl_point_2d_dist_compare> particle_map;
		vgl_point_2d<unsigned> point;

		//1. sample the temporal entropy to create a reduced particle map.
		double wsum = double(0.0);

		if( num_pixels > this->num_particles_ )
		{
			while( particle_map.size() < this->num_particles_ )
			{
				unsigned rand_indx = vnl_math_rnd(rand.drand64(0,num_pixels-1));
				point.set(rand_indx / num_rows, rand_indx % num_rows);
				particle_map[point] = this->temporal_entropy_(point.x(),point.y());
				wsum += this->temporal_entropy_(point.x(),point.y());
				vcl_cout << particle_map.size() << " out of " << this->num_particles_ << " chosen." << vcl_endl;
			}
		}
		else//the particle set is the whole image no need for stochastic particle selection.
		{
			for( unsigned col = 0; col < num_cols; ++col )
				for( unsigned row = 0; row < num_rows; ++row )
					particle_map[point] = this->temporal_entropy_(col,row);

		}


		//2. reweight each sample and simultaneously construct the cdf
		vcl_map<vgl_point_2d<unsigned>,double,dbcl_vgl_point_2d_dist_compare >::iterator pmit;
		vcl_map<vgl_point_2d<unsigned>,double,dbcl_vgl_point_2d_dist_compare >::iterator pmend = particle_map.end();

		//use the stl container so that we may use stl search.
		//the find predicate should be > so that we find the position of the first element of the cdf which is less than
		//the target
		vcl_vector<double> cdf;
		vcl_vector<vgl_point_2d<unsigned> > cdf_index;

		double cdf_tot = double(0.0);
		for( pmit = particle_map.begin(); pmit != pmend; ++pmit )
		{
			pmit->second = pmit->second/wsum; //normalizing the particle map value
			cdf_index.push_back(pmit->first);
			cdf_tot += pmit->second;
			cdf.push_back(cdf_tot);
		}//end building the cdf
		cdf.push_back(1.0);//last element should be 1 for binary search to work

		//the binary search predicate will find the first element that is larget than the target

		//3. Sample a unique set of size num_piv_pixels via inverse cdf method.
		vcl_pair<vcl_set<vgl_point_2d<unsigned>,dbcl_vgl_point_2d_dist_compare>::iterator,bool> ret;

		//ADD ELEMENTS VIA INVERSE CDF METHOD
		while( pivot_pixel_set.size() < this->num_pivot_pixels_ )
		{
			double u = rand.drand64();
			vcl_vector<double> target(1,u);
			vcl_vector<double>::iterator search_itr = vcl_search(cdf.begin(),cdf.end(),target.begin(),target.end(),&dbcl_ncn_extractor::binary_search_predicate);
			unsigned bin;
			if( search_itr != cdf.end() )
			{          
				bin = search_itr - cdf.begin();

				if( bin == cdf_index.size() )
				{
					bin = bin - 1;
					//vcl_cout << "bin = " << bin << vcl_endl;
				}
			}
			else
			{
				vcl_cerr << "ERROR: dncn_factory::sample_pivot_pixels bin not found in inverse cdf method." << vcl_flush;
				return false;
			}

			//insert point into set which corresponds to the bin
			point.set(cdf_index[bin].x(),cdf_index[bin].y());
			ret = pivot_pixel_set.insert(point);
			if( ret.second == true )
				vcl_cout << vcl_setprecision(2) << vcl_fixed
				<< (float(pivot_pixel_set.size())/float(num_pivot_pixels_)) * 100 
				<< "% pivot pixels sampled." << vcl_endl; 
		}//end pivot_pixel_set iteration

		this->pivot_pixel_set_ = pivot_pixel_set;
		this->pivot_set_valid_ = true;
		return true;
	}
	else
	{
		vcl_cerr << "ERROR: dbcl_ncn_extractor::sample_pivot_pixels() : entropy_valid_ = false" << vcl_flush;
		return false;
	}

}//end dbcl_ncn_extractor::sample_pivot_pixels

bool dbcl_ncn_extractor::extract_neighbors()
{
	if( this->pivot_set_valid_ == true )
	{
		vcl_cout << "Extracting Neighborhoods..." << vcl_endl;

		unsigned num_frames = this->img_seq_.size();

		target_ncn_map_type::iterator target_itr, target_end = this->target_ncn_map_.end();
		vcl_map<unsigned, vil_image_resource_sptr>::const_iterator img_seq_itr, img_seq_end = this->img_seq_.end();
		pivot_pixel_set_type::const_iterator pivot_itr, pivot_end = this->pivot_pixel_set_.end();
		vil_image_view<vxl_byte> img_view;

		vcl_map<double, vgl_point_2d<unsigned> > mi_point_map; //declare outside of the loop so memory is preallocated
		for( target_itr = this->target_ncn_map_.begin(); target_itr != target_end; ++target_itr )
		{
			vgl_point_2d<unsigned> curr_target = target_itr->first;
			vcl_cout << "\t Extracting Neighborhood for target: " << curr_target << vcl_endl;

			//1.(a) rank each pivot pixel candidate by joint entropy with the target.
			//      we do this by calculating the joint entropy at each target with each pivot pixel candidate
			//      then we associate the pivot pixel point with the mi value with a vcl_map.
			//      because vcl_maps internally store keys in ascention, we may pick off the top ranking
			//      candidates from the back of the map. The key will be the mi which will ensure ties are ignored 
			//      and also frees us from using a customized ordering predicate.

			mi_point_map.clear(); //clear from previous iteration
			for( pivot_itr = this->pivot_pixel_set_.begin(); pivot_itr != pivot_end; ++pivot_itr )
			{

				unsigned nbins = 16;
				bsta_joint_histogram<double> joint_histogram(255, nbins);

				//don't self include point in neighborhood (mi would be highest with itself).
				if( curr_target != *pivot_itr )
				{
					for( img_seq_itr = this->img_seq_.begin(); img_seq_itr != img_seq_end; ++img_seq_itr )
					{	
						img_view = img_seq_itr->second->get_view();
						joint_histogram.upcount(img_view(curr_target.x(),curr_target.y(),0),1,img_view(pivot_itr->x(), pivot_itr->y(),0),1);
					}//end image sequence iteration
				}//end self include check

				mi_point_map[joint_histogram.mutual_information()] = *pivot_itr;
			}//end pivot pixel iteration

			//2. Build the neighborhood by creating and filling in the appropriate data structures.
			vcl_map<double, vgl_point_2d<unsigned> >::const_iterator mi_point_itr = mi_point_map.end();
			// end() is one pas the last element so go to last element
			--mi_point_itr;

			vcl_vector<vgl_point_2d<unsigned> > neighborhood;
			for( unsigned i = 0; i < this->num_neighbors_; ++i, --mi_point_itr )
				neighborhood.push_back(mi_point_itr->second);

			target_itr->second = neighborhood;
		}//end target iteration

		this->neighborhood_valid_ = true;

		vcl_cout << "\t Done Extracting Neighborhoods..." << vcl_endl;

		return true;
	}
	else
	{
		vcl_cerr << "ERROR: dbcl_ncn_extractor::extract_neighborhors() : pixel_set_valid_ = false" << vcl_flush;
		return false;
	}

}//end dbcl_ncn_extractor::extract_neighbors()

void dbcl_ncn_extractor::load_entropy_bin(vcl_string const& filename)
{
    vsl_b_ifstream bis( filename.c_str() );
    vsl_b_read( bis,this->temporal_entropy_ );
    bis.close();
	this->entropy_valid_ = true;
	this->sample_pivot_pixels();
	this->extract_neighbors();
}//end dbcl_ncn_extractor::load_entropy_bin

bool dbcl_ncn_extractor::save_entropy_dat(vcl_string const& filename)
{
	if( this->entropy_valid_ == true )
	{
		vcl_ofstream os(filename.c_str(), vcl_ios::out);

		unsigned ni = this->temporal_entropy_.ni();
		unsigned nj = this->temporal_entropy_.nj();

		for( unsigned j = 0; j < nj; ++j )
		{
			for( unsigned i = 0; i < ni; ++i )
				os << this->temporal_entropy_(i,j,0) << '\t';
			os << '\n';
		}

		os.close();
		return true;
	}
	else
	{
		vcl_cerr << "ERROR: dbcl_ncn_extractor::save_entropy_dat() : entropy_valid_ = false" << vcl_flush;
		return false;
	}
}//end dbcl_ncn_extractor::save_entropy_dat

bool dbcl_ncn_extractor::save_entropy_bin(vcl_string const& filename)
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
		vcl_cerr << "ERROR: dbcl_ncn_extractor::save_entropy_bin() : entropy_valid_ = false" << vcl_flush;
		return false;
	}
}//end dbcl_ncn_extractor::save_entropy_bin

bool dbcl_ncn_extractor::write_neighborhood_mfile(vcl_string const& filename)
{
	if( this->neighborhood_valid_ == true )
	{
		vcl_ofstream os(filename.c_str(), vcl_ios::out);

		os << "neighborhoods = cell(" << this->target_ncn_map_.size() << ", 2);\n";

		target_ncn_map_type::const_iterator target_itr, target_end = this->target_ncn_map_.end();
		vcl_vector<vgl_point_2d<unsigned> >::const_iterator neighborhood_itr, neighborhood_end;

		//MATLAB INDEXING STARTS FROM 1!
		unsigned cell_indx;
		for( target_itr = this->target_ncn_map_.begin(), cell_indx = 1; target_itr != target_end; ++target_itr, ++cell_indx )
		{
			os << "neighborhoods{" << cell_indx << ",1} = [" << target_itr->first.x() << '\t' << target_itr->first.y() << "];\n";

			neighborhood_end = target_itr->second.end();
			for(neighborhood_itr = target_itr->second.begin(); target_itr != target_end; ++target_itr )
				os << neighborhood_itr->x() << '\t' << neighborhood_itr->y() << ";\n";

			os << "];\n";
		}//end target iteration
		return true;
	}
	else
	{
		vcl_cerr << "ERROR: dbcl_ncn_extractor::write_neighborhood_mfile() : neighborhood_valid_ = false" << vcl_flush;
		return false;
	}
}//end dbcl_ncn_extractor::write_neighborhood_mfile

//PROTECTED MEMBER FUNCTIONS
void dbcl_ncn_extractor::build_frame_map(vcl_string const& filename)
{
	this->img_seq_.clear();
	vidl_image_list_istream video_stream(filename);

	unsigned num_frames = video_stream.num_frames();
	vcl_map<unsigned, vil_image_resource_sptr> img_seq;

	for(unsigned t = 0 ; t < num_frames; ++t)
	{
		vil_image_view<vxl_byte> grey_img, current_img;
		video_stream.seek_frame(t);
		vidl_convert_to_view(*video_stream.current_frame(),current_img);
		vil_convert_planes_to_grey(current_img,grey_img);
		vil_image_resource_sptr image_sptr = vil_new_image_resource_of_view(grey_img);
		this->img_seq_[t] = image_sptr;
	}//end frame iteration
}//end dbcl_ncn_extractor::build_frame_map
