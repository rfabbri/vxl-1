//this is /contrib/bm/ncn/ncn_factory.cxx
#include "ncn_factory.h"


ncn_factory::ncn_factory(vcl_string const& video_list_glob, target_pixel_type const& target_pixels,unsigned const& n_neighbors):
				num_pivot_pixels_(2000),n_particles_(10000)
{
	video_stream_.open(video_list_glob.c_str());
	if(video_stream_.is_seekable())
		video_stream_.seek_frame(0);
	else
		vcl_cerr << "\n Error ncn_factory constructor: need to supply a seekable video stream." << vcl_flush;

	
	neighborhood_sptr_ = new ncn_neighborhood;
	this->calculate_temporal_entropy();
	neighborhood_sptr_->set_pivot_pixel_candidates(this->sample_pivot_pixels());
	neighborhood_sptr_->set_targets(target_pixels);
	neighborhood_sptr_->set_n_neighbors(n_neighbors);
	this->build_neighborhood();
	
}//end ncn_factory(video_list_glob,target_pixel_list)


ncn_factory::ncn_factory(vcl_string const& video_list_glob)
{
	video_stream_.open(video_list_glob.c_str());
	if(video_stream_.is_seekable())
		video_stream_.seek_frame(0);
	else
		vcl_cerr << "\n Error ncn_factory constructor: need to supply a seekable video stream." << vcl_flush;

	vil_image_view<double> curr_img;
	vidl_convert_to_view(*video_stream_.current_frame(),curr_img);

	neighborhood_sptr_ = new ncn_neighborhood;
	neighborhood_sptr_->set_pivot_pixel_candidates(this->sample_pivot_pixels());

	//set the target_pixel_list to the whole image.
	vgl_point_2d<unsigned> top_left(0,0), bottom_right(curr_img.ni()-1,curr_img.nj()-1);
	target_pixel_type target_pixel_list;
	for(unsigned y = top_left.y(); y <= bottom_right.y(); ++y)
		for(unsigned x = top_left.x(); x <= bottom_right.x(); ++x)
		{
			vgl_point_2d<unsigned> point(x,y);
			target_pixel_list.push_back(point);
		}

	neighborhood_sptr_->set_targets(target_pixel_list);
	num_pivot_pixels_ = 2000;
	n_particles_ = 10000;
	neighborhood_sptr_->set_n_neighbors(10);

	this->build_neighborhood();


}//end ncn_factory

void ncn_factory::calculate_temporal_entropy()
{
	vcl_cout << "Calculating Temporal Entropy..." << vcl_endl;	
	video_stream_.seek_frame(0);
	vil_image_view<vxl_byte> curr_img;
	vil_image_view<vxl_byte> grey_img;
	//vidl_convert_to_view(*video_stream_.current_frame(),curr_img);
	vidl_convert_to_view(*video_stream_.current_frame(),curr_img);
	
	unsigned nbins = 16;
	unsigned ncols = curr_img.ni();
	unsigned nrows = curr_img.nj();
	unsigned num_frames = video_stream_.num_frames();
	entropy_view_.set_size(nrows,ncols);

	bsta_histogram<double> histogram(double(0), double(255), nbins);

	entropy_view_.set_size(ncols,nrows);

	//load all images into memory for speed.
	vcl_map<unsigned, vil_image_view<vxl_byte> > img_seq;
	for(unsigned t = 0; t < num_frames; ++t)
	{
		video_stream_.seek_frame(t);
		vidl_convert_to_view(*video_stream_.current_frame(),curr_img);
		vil_convert_planes_to_grey<vxl_byte,vxl_byte>(curr_img,grey_img);
		img_seq[t] = grey_img;
	}
	video_stream_.seek_frame(0);

	unsigned imi = img_seq[0].ni();
	unsigned imj = img_seq[0].nj();
	unsigned ei = entropy_view_.ni();
	unsigned ej = entropy_view_.nj();

	for(unsigned row = 0; row < nrows; ++row)
	{
		for(unsigned col = 0; col < ncols; ++col)
		{
			histogram.clear();
			//vcl_cout << "col = " << col << vcl_endl;
			for(unsigned t = 0; t < num_frames; ++t)
			{
				histogram.upcount(img_seq[t](col,row,0),1);				
			}
			entropy_view_(col,row) = histogram.entropy();		
			//vcl_cout << float(col*nrows + row)/float(row) << "% temporal entropy completed. " << vcl_endl;
			//vcl_cout << "row = " << row << vcl_endl;
			vcl_cout << float(row*ncols+col)/float(nrows*ncols) << "% temporal entropy completed. " << vcl_endl;
		}
	}

	//write out bin file
	vcl_string entropy_view_bin_file = "C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\results\\westin1\\entropy_view.bin";
	vsl_b_ofstream bofs(entropy_view_bin_file.c_str());
	vsl_b_write(bofs,entropy_view_);

	//write out matlab file
	vcl_string entropy_view_dat_file = "C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\results\\westin1\\entropy_view.dat";
	vcl_ofstream os_dat(entropy_view_dat_file.c_str());
	for(unsigned i = 0; i < entropy_view_.ni(); ++i)
	{
		for(unsigned j = 0; j < entropy_view_.nj(); ++j)
			os_dat << entropy_view_(i,j,0) << '\t';
		vcl_cout << "\n";
	}

	
	
}//end ncn_factory::calculate_temporal_entropy

//main function to build and return the neighborhood
void ncn_factory::build_neighborhood()
{
	vcl_cout << "Building Neighborhood..." << vcl_endl;
	neighborhood_type neighborhood;
	unsigned nbins = 16;
	vcl_map<double,vcl_set<vgl_point_2d<unsigned>,ncn_vgl_point_2d_less_than>::const_iterator > mi_point_map;
	bsta_joint_histogram<double> joint_histogram(255,nbins);

	pivot_pixel_candidate_type pivot_pixel_candidates;
	pivot_pixel_candidates = neighborhood_sptr_->pivot_pixel_candidates();
	pivot_pixel_candidate_type::const_iterator pivot_pixel_candidates_itr;
	pivot_pixel_candidate_type::const_iterator pivot_pixel_candidates_end = pivot_pixel_candidates.end();

	target_pixel_type target_pixels;
	target_pixels = neighborhood_sptr_->target_pixels();
	target_pixel_type::const_iterator target_pixel_itr;
	target_pixel_type::const_iterator target_pixel_end = target_pixels.end();

	unsigned nframes = video_stream_.num_frames();
	//form neighborhoods for every point in the target_list
	
	unsigned n_targets = target_pixels.size();

	unsigned n_neighbors = neighborhood_sptr_->n_neighbors();

	//load all images to memory to speed up
	vil_image_view<vxl_byte> grey_img, curr_img;
	vcl_map<unsigned, vil_image_view<vxl_byte> > image_sequence;
	for(unsigned t = 0; t < nframes; ++t)
	{
		vidl_convert_to_view(*video_stream_.current_frame(),curr_img);
		vil_convert_planes_to_grey(curr_img,grey_img);
		image_sequence[t] = grey_img;
	}

	vcl_map<unsigned, vil_image_view<vxl_byte> >::const_iterator image_sequence_itr;
	vcl_map<unsigned, vil_image_view<vxl_byte> >::const_iterator image_sequence_end = image_sequence.end();

	unsigned counts = 0;
	for(target_pixel_itr = target_pixels.begin(); target_pixel_itr != target_pixel_end; ++target_pixel_itr)
	{
		mi_point_map.clear();
		for(pivot_pixel_candidates_itr = pivot_pixel_candidates.begin();pivot_pixel_candidates_itr!=pivot_pixel_candidates_end;++pivot_pixel_candidates_itr)
		{
			joint_histogram.clear();
			if(*target_pixel_itr != *pivot_pixel_candidates_itr)
			{
				//parse the temporal information at the target and pivot location to calculate mutual information
				for(image_sequence_itr=image_sequence.begin();image_sequence_itr!=image_sequence.end();++image_sequence_itr)
				{
					double target_intensity = image_sequence_itr->second(target_pixel_itr->x(),target_pixel_itr->y());
					double pivot_intensity = image_sequence_itr->second(pivot_pixel_candidates_itr->x(),pivot_pixel_candidates_itr->y());
					joint_histogram.upcount(target_intensity,1,pivot_intensity,1);
				}
				double mi = joint_histogram.mutual_information();
				mi_point_map[mi] = pivot_pixel_candidates_itr;
			}//end equality if
		}

		//create the neighborhood vector
		unsigned  i = 0;
		vcl_map<double,vcl_set<vgl_point_2d<unsigned>,ncn_vgl_point_2d_less_than>::const_iterator >::const_iterator mi_point_map_itr = mi_point_map.end();
		--mi_point_map_itr;
		
		vcl_vector<pivot_pixel_candidate_type::const_iterator> nbrhd;
		for(;i<n_neighbors; --mi_point_map_itr)
			nbrhd.push_back(mi_point_map_itr->second);

		//associate the point and the neighborhood
		vcl_pair<target_pixel_type::const_iterator,vcl_vector<pivot_pixel_candidate_type::const_iterator> > ni(target_pixel_itr,nbrhd);
		neighborhood.insert(ni);
		vcl_cout << counts << " out of " << target_pixels.size() << " neighbors build." << vcl_endl;
		++counts;
	}



	
}//end build_neighborhood

pivot_pixel_candidate_type ncn_factory::sample_pivot_pixels()
{
	vcl_cout << "Sampling Pivot Pixels..." << vcl_endl;
	pivot_pixel_candidate_type pivot_pixel_candidates;

	vnl_random rand;
	unsigned npix = entropy_view_.size();
	unsigned nrows = entropy_view_.nj();
	vcl_map<vgl_point_2d<unsigned>,double,ncn_vgl_point_2d_less_than> particle_map;
	vgl_point_2d<unsigned> point;

	

    //1. Select a random subset of unique pixels and associate with the corresponding probability.
    // Also cache the sum of the particle weights...we'll need these later for normalization.
	double wsum = 0;
	while(particle_map.size() < n_particles_)
	{
		unsigned rand_indx = vnl_math_rnd(rand.drand64(0,npix));
		point.set(rand_indx/nrows,rand_indx%nrows);
		particle_map[point] = entropy_view_(point.x(),point.y());
	}

	//2. Reweight each sample and simultaneously construct the cdf
	vcl_map<vgl_point_2d<unsigned>, double, ncn_vgl_point_2d_less_than >::iterator pmit;
	vcl_map<vgl_point_2d<unsigned>, double, ncn_vgl_point_2d_less_than >::iterator pmend = particle_map.end();

	vbl_array_1d<double> cdf;

	vcl_vector<vgl_point_2d<unsigned> > particle_vector; //to ease with indexing
	double cdf_tot = 0.0;
	for(pmit = particle_map.begin(); pmit != pmend; ++pmit)
	{
		pmit->second = pmit->second/wsum;
		particle_vector.push_back(pmit->first);
		cdf_tot += pmit->second;
		cdf.push_back(cdf_tot);
	}

	//3. Sample a unique set of size num_piv_pix via inverse cdf method.
	vcl_pair<vcl_set<vgl_point_2d<unsigned>,ncn_vgl_point_2d_less_than>::iterator,bool> ret;

	pmit = particle_map.begin();
	while(pivot_pixel_candidates.size() < num_pivot_pixels_)
	{
		double u = rand.drand64();
		unsigned bin = ncn_factory::find_bin(cdf,u);
		point.set(particle_vector[bin].x(),particle_vector[bin].y());
		ret = pivot_pixel_candidates.insert(point);
		if(ret.second == true)
			vcl_cout << vcl_setprecision(2) << vcl_fixed 
                     << (float(pivot_pixel_candidates.size())/float(num_pivot_pixels_)) * 100 
                     << "% pivot pixels sampled." << vcl_endl;
	}//end sample loop

	return pivot_pixel_candidates;
}//end sample_pivot_pixels

unsigned ncn_factory::find_bin(vbl_array_1d<double> const& cdf, double const& target)
{
	
	unsigned first = 0;
    unsigned last = cdf.size()-1;
    bool bin_found = false;
    unsigned mid;

    if( target < cdf[0] )
    {
        bin_found = true;
        mid = 0;
    }
    else if(target > cdf[cdf.size()-1])
    {
        bin_found = true;
        mid = cdf.size()-1;
    }
    else
    {
        //DIVIDE AND CONQUER!!!
        while(!bin_found && first<=last)
        {
            mid = (first+last)/2;

            if(cdf[mid-1] <= target && target <= cdf[mid])
                bin_found = true;
            else
                if(cdf[mid] > target)
                    last = mid - 1;
                else
                    first = mid + 1;

        }//end DIVIDE AND CONQUER!!! loop
    }

    if( !bin_found )
    {
        vcl_cerr << "Error: ncn_utilities::find_bin bin was not found." << vcl_flush;
    }

    return mid;

}//end ncn_factory::find_bin