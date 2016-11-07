//this is contrib/bm/ncn1/ncn1_factory.cxx

#include "ncn1_factory.h"

ncn1_factory::ncn1_factory(vcl_string const& video_glob, unsigned num_pivot_pixels, unsigned n_particles, unsigned n_neighbors)
{
    //num_pivot_pixels_ = num_pivot_pixels;
    //n_particles_ = n_particles;
    //n_neighbors_ = n_neighbors;

    if(this->video_stream_.is_open())
        this->video_stream_.close();
    
    video_stream_.open(video_glob);

    video_stream_.seek_frame(0);

    unsigned ncols = video_stream_.width();
    unsigned nrows = video_stream_.height();
    unsigned npix = ncols*nrows;

    n_neighbors_ = n_neighbors;

    neighborhood_sptr_ = new ncn1_neighborhood;
    feature_sptr_ = new ncn1_feature(n_neighbors+1);
    

    if( npix > num_pivot_pixels )
        num_pivot_pixels_ = num_pivot_pixels;
    else
        num_pivot_pixels = unsigned( npix *.75 );

    if( npix > n_particles ) 
        n_particles_ = n_particles;
    else
        n_particles_ = npix;

    neighborhood_sptr_->set_num_neighbors(this->n_neighbors_);

}//end ncn1_factory::ncn1_factory

pivot_pixel_list_type ncn1_factory::sample_pivot_pixel_candidates()
{
    vcl_cout << "Sampling Pivot Pixels..." << vcl_endl;
    vcl_set<vgl_point_2d<unsigned>,ncn1_vgl_point_2d_less_than> pivot_pixel_set;
    pivot_pixel_list_type pivot_pixel_candidates;

    vnl_random rand;
    unsigned npix = this->temporal_entropy_.size();
    unsigned ncol = this->temporal_entropy_.ni();
    unsigned nrow = this->temporal_entropy_.nj();
    vcl_map<vgl_point_2d<unsigned>,double,ncn1_vgl_point_2d_less_than> particle_map;
    vgl_point_2d<unsigned> point;

    
    //the image sequence we are using is small enough to use the whole image pixel set as pivot candidates.
    if( npix <= this->n_particles_ ) 
    {
        for( unsigned c = 0; c < ncol; ++c )
            for( unsigned r = 0; r < nrow; ++r )
            {
                point.set(c,r);
                pivot_pixel_candidates.push_back(point);
            }
    }
    else
    {
        double wsum = 0;
        while( particle_map.size() < this->n_particles_ )
        {  
            unsigned rand_indx = vnl_math_rnd(rand.drand64(0,npix));
            point.set(rand_indx/nrow,rand_indx%nrow);
            particle_map[point] = this->temporal_entropy_(point.x(),point.y());
            wsum += this->temporal_entropy_(point.x(),point.y());
        }
        //2. Reweight each sample and simultaneously construct the cdf
        vcl_map<vgl_point_2d<unsigned>,double, ncn1_vgl_point_2d_less_than >::iterator pmit;
        vcl_map<vgl_point_2d<unsigned>,double, ncn1_vgl_point_2d_less_than >::iterator pmend = particle_map.end();

        vbl_array_1d<double> cdf;

        vcl_vector<vgl_point_2d<unsigned> > particle_vector; //to ease indexing
        double cdf_tot = 0.0;
        for( pmit = particle_map.begin(); pmit != pmend; ++pmit )
        {
            pmit->second = pmit->second/wsum;
            particle_vector.push_back(pmit->first);
            cdf_tot += pmit->second;
            cdf.push_back(cdf_tot);
        }

        //3. Sample a unique set of size num_piv_pix via inverse cdf method.
        vcl_pair<vcl_set<vgl_point_2d<unsigned>,ncn1_vgl_point_2d_less_than>::iterator,bool> ret;

        //pmit = particle_map.begin();
        //use the set to check for unique points in the candidate set
        while( pivot_pixel_set.size() < this->num_pivot_pixels_ )
        {
            double u = rand.drand64();
            //vcl_cout << "u = " << u << vcl_endl;
            unsigned bin = this->find_bin(cdf,u);
            //vcl_cout << "bin = " << bin << vcl_endl;
            //vcl_cout << "cdf.size() = " << cdf.size() << vcl_endl;
            point.set(particle_vector[bin].x(),particle_vector[bin].y());
            ret = pivot_pixel_set.insert(point);
            if(ret.second == true)
                vcl_cout << vcl_setprecision(2) << vcl_fixed 
                << (float(pivot_pixel_set.size())/float(num_pivot_pixels_)) * 100 
                << "% pivot pixels sampled." << vcl_endl;
            
        }  

        //put the pivot pixel candidates in the right container
        vcl_set<vgl_point_2d<unsigned>,ncn1_vgl_point_2d_less_than>::iterator psitr;
        vcl_set<vgl_point_2d<unsigned>,ncn1_vgl_point_2d_less_than>::iterator psend = pivot_pixel_set.end();
        for(psitr = pivot_pixel_set.begin(); psitr != psend; ++psitr)
            pivot_pixel_candidates.push_back(*psitr);
    }

    return pivot_pixel_candidates;

}//end sample_pivot_pixel_candidates

unsigned ncn1_factory::find_bin(vbl_array_1d<double> const& cdf, double const& target)
{
	unsigned first = 0;
    unsigned last = cdf.size()-1;
    bool bin_found = false;
    unsigned mid;

    if( target <= cdf[0] )
    {
        bin_found = true;
        mid = 0;
        //vcl_cout << "In first Condition. " << vcl_endl;
    }
	else if(target >= cdf.back())
    {
        bin_found = true;
        mid = cdf.size()-1;
        //vcl_cout << "In Second condition. " << vcl_endl;
    }
    else if( target >= cdf[0] && target <= cdf[1] )
    {
        bin_found = true;
        mid = 1;
    }
    else
    {
        //vcl_cout << "In divide and conquer. " << vcl_endl;
        //DIVIDE AND CONQUER!!!
        while(!bin_found && first<=last)
        {

            mid = (first+last)/2;

            //vcl_cout << "target = " << vcl_setprecision(10) << target << vcl_endl;
            //vcl_cout << "cdf[0] = " << vcl_setprecision(10) << cdf[0] << vcl_endl;
            //vcl_cout << "cdf[1] = " << vcl_setprecision(10) << cdf[1] << vcl_endl;
            //vcl_cout << "cdf.back() = " << cdf.back() << vcl_endl;
            //vcl_cout << "First = " << first << vcl_endl;
            //vcl_cout << "Last = " << last << vcl_endl;
            //vcl_cout << "Mid = " << mid << vcl_endl;

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
}//end ncn1_factory::find_bin

void ncn1_factory::calculate_temporal_entropy()
{
    this->calculate_temporal_entropy(this->video_stream_);
}// end ncn1_factory::calculate_temporal_entropy()

void ncn1_factory::calculate_temporal_entropy(vidl_image_list_istream& video_stream)
{
	//1. load images from vidl_image_list_istream into memory for faster access.
	//vil_image_view<unsigned> curr_img;
	//vil_image_view<unsigned> grey_img;
	vcl_map<unsigned,vil_image_view<unsigned> > img_seq;

	unsigned n_frames = video_stream.num_frames();
	for(unsigned t = 0; t < n_frames; ++t)
	{
        vil_image_view<unsigned> curr_img;
        vil_image_view<unsigned> grey_img;
		video_stream.seek_frame(t);
		vidl_convert_to_view(*video_stream.current_frame(),curr_img);
		vil_convert_planes_to_grey(curr_img,grey_img);
		img_seq[t] = grey_img;
	}

	//2. calculate temporal entropy
	temporal_entropy_ = this->calculate_temporal_entropy(img_seq);

}//end ncn1_factory::calculate_temporal_entropy

void ncn1_factory::calculate_temporal_entropy(vgl_box_2d<unsigned> const& roi)
{
    vgl_point_2d<unsigned> top_left = roi.min_point();
    vgl_point_2d<unsigned> bottom_right = roi.max_point();

    vcl_map<unsigned, vil_image_view<unsigned> > img_seq;
    
    unsigned ni = roi.min_point().x() - roi.max_point().x();
    unsigned nj = roi.min_point().y() - roi.min_point().y();

    unsigned nframes = video_stream_.num_frames();

    for( unsigned t = 0; t < nframes; ++t )
    {
        vil_image_view<unsigned> full_img,grey_img,cropped_img;
        video_stream_.seek_frame(t);
        vidl_convert_to_view(*video_stream_.current_frame(),full_img);
        vil_convert_planes_to_grey(full_img,grey_img);
        cropped_img = vil_crop(grey_img,roi.min_point().x(),ni,roi.min_point().y(),nj);
        img_seq[t] = cropped_img;
    }

    video_stream_.seek_frame(0);

    this->calculate_temporal_entropy(img_seq);
}//end ncn1::calculate_temporal_entropy

vil_image_view<double> ncn1_factory::calculate_temporal_entropy(vcl_map<unsigned, vil_image_view<unsigned> >& img_seq)
{
	vcl_cout << "Calculating Entropy..." << vcl_endl;
	unsigned nbins = 16;
	vil_image_view<double> temporal_entropy;
	bsta_histogram<double> histogram(double(0),double(255),nbins);

	unsigned ncols = img_seq[0].ni();
	unsigned nrows = img_seq[0].nj();
    
    //DEBUG
    //for( unsigned t = 0; t < 35; t = t + 5 )
    //{
    //    vcl_cout << "Image " << t << vcl_endl;
    //    for( unsigned i = 100; i < 115; ++i )
    //    {
    //        for( unsigned j = 100; j < 115; ++j )
    //        {
    //            vcl_cout << img_seq[t](i,j) << '\t';
    //        }
    //        vcl_cout << vcl_endl;
    //    }
    //    vcl_cout << '\n' << '\n';
    //}

	temporal_entropy.set_size(ncols,nrows,1);

	unsigned nframes = img_seq.size();

    double entropy_sum = 0.0;

	for( unsigned c = 0; c < ncols; ++c )
	{
		for( unsigned r = 0; r < nrows; ++r )
		{
			histogram.clear();
			for( unsigned t = 0; t < nframes; ++t)
			{
                //vcl_cout << img_seq[t](c,r,0) << vcl_endl;
				histogram.upcount(img_seq[t](c,r,0),1);
			}
			temporal_entropy(c,r,0) = histogram.entropy();
            entropy_sum += temporal_entropy(c,r,0);
			vcl_cout << (float(nrows*c + r)/float(ncols*nrows))*float(100) << "% complete " << vcl_endl;
            //vcl_cout << "Temporal_entropy = " << temporal_entropy(r,c,0) << vcl_endl;
		}		
	}

    //normalize
    vcl_cout << "Normalizing Temporal Entropy View..." << vcl_endl;
    for( unsigned c = 0; c < ncols; ++c )
        for( unsigned r = 0; r < nrows; ++r )
            temporal_entropy(c,r,0) = temporal_entropy(c,r,0)/entropy_sum;

	return temporal_entropy;
	
}// end ncn1_factory::calculate_temporal_entropy

void ncn1_factory::build_neighborhood()
{
    vcl_cout << "Building Neighborhood..." << vcl_endl;
    neighborhood_type neighborhood;
    unsigned nbins = 16;
    vcl_map<double,vgl_point_2d<unsigned> > mi_point_map;
    bsta_joint_histogram<double> joint_histogram(255,nbins);

    pivot_pixel_list_type pivot_pixel_candidates;
    pivot_pixel_candidates = neighborhood_sptr_->pivot_pixel_candidates();

    pivot_pixel_list_type::const_iterator pivot_pixel_candidates_itr;
    pivot_pixel_list_type::const_iterator pivot_pixel_candidates_end = pivot_pixel_candidates.end();

    target_pixel_list_type target_pixels;
    target_pixels = neighborhood_sptr_->target_pixel_list();

    target_pixel_list_type::const_iterator target_pixel_itr;
    target_pixel_list_type::const_iterator target_pixel_end = target_pixels.end();

    unsigned n_frames = video_stream_.num_frames();
    unsigned n_targets = target_pixels.size();
    unsigned n_neighbors = this->neighborhood_sptr_->n_neighbors();

    //load all images to memory to speed up
    
    vcl_map<unsigned, vil_image_view<vxl_byte> > img_seq;
    for( unsigned t = 0; t < n_frames; ++t)
    {
        vil_image_view<vxl_byte> grey_img, curr_img;
        this->video_stream_.seek_frame(t);
        vidl_convert_to_view(*video_stream_.current_frame(),curr_img);
        vil_convert_planes_to_grey(curr_img,grey_img);
        img_seq[t] = grey_img;
    }
    
    //iteraters to traverse the sequence
    vcl_map<unsigned, vil_image_view<vxl_byte> >::const_iterator img_seq_itr;
    vcl_map<unsigned, vil_image_view<vxl_byte> >::const_iterator img_seq_end = img_seq.end();

    unsigned counts = 0;
    for(target_pixel_itr = target_pixels.begin(); target_pixel_itr != target_pixel_end; ++target_pixel_itr)
    {
        mi_point_map.clear();
        for(pivot_pixel_candidates_itr = pivot_pixel_candidates.begin(); pivot_pixel_candidates_itr != pivot_pixel_candidates_end; ++pivot_pixel_candidates_itr)
        {
            joint_histogram.clear();
            if(*target_pixel_itr != *pivot_pixel_candidates_itr)//if we're not looking at the same point 
            {
                unsigned ti = target_pixel_itr->x();
                unsigned tj = target_pixel_itr->y();
                unsigned pivi = pivot_pixel_candidates_itr->x();
                unsigned pivj = pivot_pixel_candidates_itr->y();

                //vcl_cout << "ti = " << ti << vcl_endl;
                //vcl_cout << "tj = " << tj << vcl_endl;
                //vcl_cout << "pivi = " << pivi << vcl_endl;
                //vcl_cout << "pivj = " << pivj << vcl_endl;
                for( img_seq_itr = img_seq.begin(); img_seq_itr != img_seq_end; ++img_seq_itr )
                {
                    double target_intensity = img_seq_itr->second(ti,tj);
                    double pivot_intensity = img_seq_itr->second(pivi,pivj);
                    //vcl_cout << "target_intensity = " << target_intensity << vcl_endl;
                    //vcl_cout << "pivot_intensity = " << pivot_intensity << vcl_endl;
                    //vcl_cout << "frame number = " << img_seq_itr->first << vcl_endl;
                    joint_histogram.upcount(target_intensity,1,pivot_intensity,1);
                }             
            }
            double mi = joint_histogram.mutual_information();
            mi_point_map[mi] = *pivot_pixel_candidates_itr;
        } //end pivot pixel loop

        //build the neighborhood
        //we need to start at the end of the mi_point map because the map sorts in ascending order then.
        //the end point points to one element after the last hence we must decrement once prior to dereferencing the neighboring pivots
        unsigned i = 0;
        vcl_cout << "n_neighbors = " << n_neighbors << vcl_endl;
        vcl_cout << "mi_point_map.size() = " << mi_point_map.size() << vcl_endl;
        vcl_map<double,vgl_point_2d<unsigned> >::iterator mi_point_map_itr = mi_point_map.end();
        //vcl_map<double,vgl_point_2d<unsigned> >::iterator mi_point_map_end = mi_point_map.end();
        mi_point_map_itr--;

        pivot_pixel_list_type nbrhd;
        
        for(; i < n_neighbors; --mi_point_map_itr, ++i)
            nbrhd.push_back(mi_point_map_itr->second);

        //associate the target and neighborhood vector
        neighborhood[*target_pixel_itr] = nbrhd;
        vcl_cout << '\t' << counts + 1 << " out of " << target_pixels.size() << " neighbors built." << vcl_endl;
        ++counts;
    }//end target pixel loop

    this->neighborhood_sptr_->set_neighborhood(neighborhood);

    this->neighborhood_sptr_->set_neighborhood_valid(true);

}//end ncn1_factory::build_neighborhood


void ncn1_factory::extract_features()
{

    vcl_cout << "Extracting Features... " << vcl_endl;
    unsigned nframes = video_stream_.num_frames();
    unsigned n_neighbors = this->neighborhood_sptr_->n_neighbors();
    unsigned feat_vect_dim = 1 + n_neighbors;

    neighborhood_type nbrhd = this->neighborhood_sptr_->neighborhood();

    neighborhood_type::const_iterator nbrhd_itr;
    neighborhood_type::const_iterator nbrhd_end = nbrhd.end();

    feature_map_type feature_map;
    
    for( unsigned t = 0; t < nframes; ++t )
    {
        //load the image
        vil_image_view<vxl_byte> curr_img, grey_img;
        this->video_stream_.seek_frame(t);
        vidl_convert_to_view(*video_stream_.current_frame(),curr_img);
        vil_convert_planes_to_grey(curr_img,grey_img);

        

        //create temporary target/feature_vector map
        feature_vector_type target_intensity_map;
        
      
        for( nbrhd_itr = nbrhd.begin(); nbrhd_itr != nbrhd_end; ++nbrhd_itr )
        {
            //create temporary feature vector
            vnl_vector<vxl_byte> intensity_vector(feat_vect_dim);

            
            //get the target's intensity and insert into feature vector
            vgl_point_2d<unsigned> target = nbrhd_itr->first;
            intensity_vector[0] = grey_img(target.x(),target.y());
            //intensity_vector.push_back(grey_img(target.x(),target.y()));

            //iterate through the neighbors of the particular target
            pivot_pixel_list_type::const_iterator piv_itr = nbrhd_itr->second.begin();
            pivot_pixel_list_type::const_iterator piv_end = nbrhd_itr->second.end();

            //insert the neighbors' intensity into the feature vector
            for( unsigned frame_indx = 1; piv_itr != piv_end; ++frame_indx, ++piv_itr )               
                intensity_vector[frame_indx] = grey_img(piv_itr->x(),piv_itr->y());
            
            target_intensity_map[target] = intensity_vector;

        }//end target loop

       feature_map[t] = target_intensity_map;
       
       vcl_cout << float(t+1) / float(nframes) * 100 << "% complete. " << vcl_endl;
    }//end frame loop

    this->feature_sptr_->set_feature_map(feature_map);
    //this->neighborhood_sptr_->set_feature_map(feature_map);
}

void ncn1_factory::save_binary_temporal_entropy(vcl_string const& filename, vil_image_view<double> const& temporal_entropy)
{
	vsl_b_ofstream bofs(filename.c_str());
	vsl_b_write(bofs,temporal_entropy);
	bofs.close();
}//end ncn1_factory::save_binary_temporal_entropy

void ncn1_factory::save_binary_temporal_entropy(vcl_string const& filename)
{
    this->save_binary_temporal_entropy(filename,this->temporal_entropy_);
}//end ncn1_factory::save_binary_temporal_entropy

void ncn1_factory::load_binary_temporal_entropy(vcl_string const& filename, vil_image_view<double>& temporal_entropy)
{
	vsl_b_ifstream bis(filename.c_str());
	vsl_b_read(bis,temporal_entropy);
	bis.close();
}//end ncn1_factory::load_binary_temporal_entropy

void ncn1_factory::load_binary_temporal_entropy(vcl_string const& filename)
{
    this->load_binary_temporal_entropy(filename,this->temporal_entropy_);
}//end ncn1_factory::load_binary_temporal_entropy

void ncn1_factory::save_matlab_temporal_entropy(vcl_string const& filename)
{
    vcl_ofstream os(filename.c_str(),vcl_ios::out);

    unsigned ni = this->temporal_entropy_.ni();
    unsigned nj = this->temporal_entropy_.nj();

    //for( unsigned i = 0; i < ni; ++i )
    //{
    //    for( unsigned j = 0; j < nj; ++j )
    //    {
    //        os << temporal_entropy_(i,j,0) << '\t';
    //    }
    //    os << '\n';
    //}

    for( unsigned j = 0; j < nj; ++j )
    {
        for( unsigned i = 0; i < ni; ++i )
            os << temporal_entropy_(i,j,0) << '\t';
        os << '\n';
    }
        
    os.close();
}//end ncn1_factory::save_matlab_temporal_entropy


