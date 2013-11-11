//this is contrib/bm/dncn/dncn_factory.cxx
#include"dncn_factory.h"

dncn_factory::dncn_factory( vcl_string& video_glob, point_vector_type& pv, unsigned num_pivot_pixels, unsigned num_particles, unsigned num_neighbors):
    num_neighbors_(num_neighbors)
{
    point_vector_ = pv;
    this->target_list_2d_sptr_ = new dncn_target_list_2d;

    

    if( this->video_stream_.is_open() )
        this->video_stream_.close();

    video_stream_.open(video_glob);

    video_stream_.seek_frame(0);
    unsigned ncols = video_stream_.width();
    target_list_2d_sptr_->set_cols(ncols);

    unsigned nrows = video_stream_.height();
    target_list_2d_sptr_->set_rows(nrows);

    target_list_2d_sptr_->set_nframes(video_stream_.num_frames());

    unsigned num_pixels = ncols*nrows;

    if( num_pixels > num_pivot_pixels )
        num_pivot_pixels_ = num_pivot_pixels;
    else
        num_pivot_pixels_ = unsigned( num_pixels * .75 );

    if( num_pixels > num_particles )
        num_particles_ = num_particles;
    else
        num_particles_ = num_pixels;

    this->build_target_list();

}//end dncn_factory::dncn_factory

// PROCESSES


// Iterate throught the client supplied point vector and create the appropriate data structures.

void dncn_factory::build_target_list()
{
    unsigned num_frames = this->video_stream_.num_frames();

    point_vector_type::iterator pvit;
    point_vector_type::iterator pvend = this->point_vector_.end();

    //iterate through the client interface
    for( pvit = this->point_vector_.begin(); pvit != pvend; ++pvit )
    {
        vcl_cout << "Target List " << (pvit-point_vector_.begin())/float(point_vector_.size()) * 100 << "% complete." << vcl_endl;
        //vgl_point_2d<unsigned> curr_target = pvit->img_point;

        //create the target 
        dncn_target_2d_sptr curr_target_sptr = new dncn_target_2d(pvit->img_point);

        //create the neighborhood
        dncn_neighborhood_2d_sptr neighborhood_sptr = new dncn_neighborhood_2d;

        //link the target and neighborhood structure
        curr_target_sptr->set_neighborhood_sptr(neighborhood_sptr);

        //feature_map_type == vcl_map<unsigned, dncn_feature_sptr>
        dncn_target_2d::feature_map_type curr_feature_map; 

		dncn_target_2d::feature_map_type curr_target_map;

        //There is a 1:1 correspondence between frames and features. For each frame create a feature which is/is not 
        //labeled as ground truth as specified by the client.
        for( unsigned t = 0; t < num_frames; ++t )
        {
            dncn_feature_sptr feature_sptr = new dncn_feature;
			dncn_feature_sptr target_feature_sptr = new dncn_feature;

            //if the label is not in the frame map supplied by the client it is not a gt
            //else it is gt and set the appropriate fields.
            vcl_map<unsigned,unsigned>::iterator it;
            it = pvit->frame_label.find(t);
         
            if( it != pvit->frame_label.end() )
            {       
                feature_sptr->set_frame(it->first);
                feature_sptr->set_label(it->second);
                feature_sptr->set_is_gt(true);
            }
            else
            {
                feature_sptr->set_frame(t);
            }
			target_feature_sptr->set_frame(t);


            //map each feature and frame to insert into the target smart pointer
            curr_feature_map.insert( vcl_make_pair<unsigned,dncn_feature_sptr>(feature_sptr->frame(),feature_sptr) );
			curr_target_map.insert( vcl_make_pair<unsigned, dncn_feature_sptr>(target_feature_sptr->frame(), target_feature_sptr) );

        }//end frame/feature iteration
        
        //set the current target's feature map.
        curr_target_sptr->set_feature_map(curr_feature_map);
		curr_target_sptr->set_target_map(curr_target_map);

        //insert the curr_target_sprt into the target_list_2d_sptr_ target list.
        this->target_list_2d_sptr_->add_target(curr_target_sptr);

    }//end client point vector iteration
}//end dncn_factory::build_target_list()

void dncn_factory::calculate_temporal_entropy()
{
    //1. load frames from vidl_image_list_istream into memory for faster access.
    vcl_map<unsigned, vil_image_view<unsigned> > img_seq;

    unsigned num_frames = this->video_stream_.num_frames();

    for( unsigned t = 0; t < num_frames; ++t )
    {
        vil_image_view<unsigned> curr_img;
        vil_image_view<unsigned> grey_img;
        video_stream_.seek_frame(t);
        vidl_convert_to_view(*video_stream_.current_frame(),curr_img);
        vil_convert_planes_to_grey(curr_img,grey_img);
        img_seq[t] = grey_img;
    }//end video stream iteration

    this->video_stream_.seek_frame(0);

    vcl_cout << "Calculating Temporal Entropy..." << vcl_endl;

    unsigned num_bins = 16;
    vil_image_view<double> temporal_entropy;
    bsta_histogram<double> histogram(double(0),double(255),num_bins);

    unsigned num_cols = img_seq[0].ni();
    unsigned num_rows = img_seq[0].nj();

    temporal_entropy.set_size(num_cols,num_rows,1);

    double entropy_sum = double(0.0); //for normalization

    for( unsigned c = 0; c < num_cols; ++c )
    {
        for( unsigned r = 0; r < num_rows; ++r )
        {
            histogram.clear();
            for( unsigned t = 0; t < num_frames; ++t )
            {
                histogram.upcount(img_seq[t](c,r,0),1);
            }
            temporal_entropy(c,r,0) = histogram.entropy();
            entropy_sum += temporal_entropy(c,r,0);
            vcl_cout << (float(num_rows*c + r)/float(num_cols*num_rows))*float(100) << "% complete " << vcl_endl;
        }
    }

    //normalize
    vcl_cout << "Normalizing Temporal Entropy..." << vcl_endl;
    for( unsigned c = 0; c < num_cols; ++c )
        for( unsigned r = 0; r < num_rows; ++r )
            temporal_entropy(c,r,0) = temporal_entropy(c,r,0)/entropy_sum;

    this->temporal_entropy_ = temporal_entropy;

}//end dncn_factory::calculate_temporal_entropy

void dncn_factory::sample_pivot_pixels()
{
    vcl_cout << "Sampling Pivot Pixels..." << vcl_endl;
    
    pivot_pixel_set_type pivot_pixel_set;

    vnl_random rand;
    unsigned num_pixels = this->temporal_entropy_.size();
    unsigned num_cols = this->temporal_entropy_.ni();
    unsigned num_rows = this->temporal_entropy_.nj();

    vcl_map<vgl_point_2d<unsigned>, double, dncn_less_than> particle_map;
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
    
    vcl_map<vgl_point_2d<unsigned>,double,dncn_less_than >::iterator pmit;
    vcl_map<vgl_point_2d<unsigned>,double,dncn_less_than >::iterator pmend = particle_map.end();

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
    vcl_pair<vcl_set<vgl_point_2d<unsigned>,dncn_less_than>::iterator,bool> ret;

    //ADD ELEMENTS VIA INVERSE CDF METHOD
    while( pivot_pixel_set.size() < this->num_pivot_pixels_ )
    {
        double u = rand.drand64();
        vcl_vector<double> target(1,u);
        vcl_vector<double>::iterator search_itr = vcl_search(cdf.begin(),cdf.end(),target.begin(),target.end(),&dncn_factory::binary_search_predicate);
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
            vcl_cerr << "ERROR: dncn_factory::sample_pivot_pixels bin not found in inverse cdf method." << vcl_flush;

        //insert point into set which corresponds to the bin
        point.set(cdf_index[bin].x(),cdf_index[bin].y());
        ret = pivot_pixel_set.insert(point);
        if( ret.second == true )
            vcl_cout << vcl_setprecision(2) << vcl_fixed
            << (float(pivot_pixel_set.size())/float(num_pivot_pixels_)) * 100 
            << "% pivot pixels sampled." << vcl_endl; 
    }//end pivot_pixel_set iteration

    this->pivot_pixel_set_ = pivot_pixel_set;

}//end void dncn_factory::sample_pivot_pixels()

void dncn_factory::extract_neighbors()
{
    vcl_cout << "Extracting Neighborhoods..." << vcl_endl;

    unsigned num_frames = this->video_stream_.num_frames();

    //1. Load all images to memory to speed up.
    vcl_map<unsigned, vil_image_view<vxl_byte> > img_seq = this->build_frame_map();

    //2. For all Specified targets:    
    dncn_target_list_2d::target_list_type::iterator target_list_itr;
    dncn_target_list_2d::target_list_type::iterator target_list_begin = this->target_list_2d_sptr_->target_list_begin();
    dncn_target_list_2d::target_list_type::iterator target_list_end = this->target_list_2d_sptr_->target_list_end();

    /*point_vector_type::iterator target_pixel_itr;
    point_vector_type::iterator target_pixel_end = this->point_vector_.end();*/

    for( target_list_itr = this->target_list_2d_sptr_->target_list_begin(); target_list_itr != target_list_end; ++target_list_itr )
    {
        dncn_target_2d_sptr curr_target_sptr = *target_list_itr;
        vgl_point_2d<unsigned> curr_target = curr_target_sptr->target();
        dncn_neighborhood_2d_sptr curr_neighborhood_sptr = curr_target_sptr->neighborhood_sptr();

        vcl_cout << "Extracting Neighborhood for image coordinate: " << curr_target << /*" (" 
            << (target_list_itr - target_list_begin) + 1 << " out of " << this->target_list_2d_sptr_->num_targets()
            << ")" <<*/ vcl_endl;

        //2.(a) rank each pivot pixel candidate by joint entropy with the target.
        //      we do this by calculating the joint entropy at each target with each pivot pixel candidate
        //      then we associate the pivot pixel point with the mi value with a vcl_map.
        //      because vcl_maps internally store keys in ascention, we may pick off the top ranking
        //      candidates from the back of the map. The key will be the mi which will ensure ties are ignored 
        //      and also frees us from using a customized ordering predicate.
        
        vcl_map<unsigned, vil_image_view<vxl_byte> >::const_iterator img_seq_itr;
        vcl_map<unsigned, vil_image_view<vxl_byte> >::const_iterator img_seq_end = img_seq.end();
        pivot_pixel_set_type::const_iterator pivot_candidate_itr;
        pivot_pixel_set_type::const_iterator pivot_candidate_end = this->pivot_pixel_set_.end();

        vcl_map<double, vgl_point_2d<unsigned> > mi_point_map;
        for( pivot_candidate_itr = this->pivot_pixel_set_.begin(); pivot_candidate_itr != pivot_candidate_end; ++pivot_candidate_itr )
        {
            unsigned num_bins = 16;
            bsta_joint_histogram<double> joint_histogram(255,num_bins);

            //so that we don't self include the point in the neighborhood as mi would surely be highest on itself.
            if( curr_target != *pivot_candidate_itr)
            {
                unsigned ti = curr_target.x();
                unsigned tj = curr_target.y();
                unsigned pivi = pivot_candidate_itr->x();
                unsigned pivj = pivot_candidate_itr->y();

                for( img_seq_itr = img_seq.begin(); img_seq_itr != img_seq_end; ++img_seq_itr )
                {
                    double target_intensity = img_seq_itr->second(ti,tj);
                    double pivot_intensity = img_seq_itr->second(pivi,pivj);
                    joint_histogram.upcount(target_intensity,1,pivot_intensity,1);
                }// end frame iteration
            }// end target = pivot candidate check.
            double mi = joint_histogram.mutual_information();
            mi_point_map[mi] = *pivot_candidate_itr;
        }// end pivot pixel iteration

        //3. Build the neighborhood by creating and filling in the appropriate data structures.
        //   the dncn_target_2d with dncn_neighborhood_2d_sptr's should have been created by now.
        dncn_neighborhood_2d::neighborhood_type neighborhood_vector;

        //remember the highest ranking mi value is at the end of the map so we should pick off the 
        //values working from the end backwards.
        vcl_map<double, vgl_point_2d<unsigned> >::const_iterator mi_point_itr = mi_point_map.end();

        // end() is one past the last element, go to the last element.
        --mi_point_itr;

        for( unsigned i = 0; i < this->num_neighbors_; ++i, --mi_point_itr )
            neighborhood_vector.push_back(mi_point_itr->second);

        curr_neighborhood_sptr->set_neighborhood(neighborhood_vector);

    }//end point vector iteration

    vcl_cout << "Done Extracting Neighbors." << vcl_endl;
}//end dncn_factory::extract_neighbors()

void dncn_factory::extract_features()
{

    //1. Load all images to memory to speed up.
    unsigned num_frames = this->video_stream_.num_frames();

    //load frames into memory for faster access.
    vcl_map<unsigned, vil_image_view<vxl_byte> > img_seq = this->build_frame_map();

    dncn_target_list_2d::target_list_type::iterator tl_itr;
    dncn_target_list_2d::target_list_type::iterator tl_end = this->target_list_2d_sptr_->target_list_end();

    for( tl_itr = this->target_list_2d_sptr_->target_list_begin(); tl_itr != tl_end; ++tl_itr )
    {
        dncn_target_2d_sptr curr_target_sptr = *tl_itr;
        
        vcl_map<unsigned, vil_image_view<vxl_byte> >::iterator img_seq_itr;
        vcl_map<unsigned, vil_image_view<vxl_byte> >::iterator img_seq_end = img_seq.end();

        dncn_neighborhood_2d::neighborhood_type curr_neighborhood = curr_target_sptr->neighborhood_sptr()->neighborhood();
        dncn_neighborhood_2d::neighborhood_type::iterator nbrhd_itr;
        dncn_neighborhood_2d::neighborhood_type::iterator nbrhd_end = curr_neighborhood.end();

        for( img_seq_itr = img_seq.begin(); img_seq_itr != img_seq_end; ++img_seq_itr )
        {
            dncn_feature_sptr curr_feature_sptr = curr_target_sptr->feature_sptr( img_seq_itr->first );

			dncn_feature_sptr curr_target_feature_sptr = curr_target_sptr->target_sptr( img_seq_itr->first );

            dncn_feature::feature_vector_type curr_feature_vector;
            
            curr_feature_vector.set_size(this->num_neighbors_ + 1 );

            double target_intensity = double( img_seq_itr->second( curr_target_sptr->target().x(), curr_target_sptr->target().y() ) );

            curr_feature_vector[0] = target_intensity;

            unsigned indx;
            for(indx = 1, nbrhd_itr = curr_neighborhood.begin(); nbrhd_itr != nbrhd_end; ++nbrhd_itr, ++indx )
            {
                //check if x,y and vil image view coords match.
                curr_feature_vector[indx] = double(img_seq_itr->second(nbrhd_itr->x(),nbrhd_itr->y()));       
            }//end neighborhood iteration

			

			//double curr_feature_vector_mean = curr_feature_vector.mean();

			//curr_feature_vector = curr_feature_vector - curr_feature_vector_mean;

			//double curr_feature_vector_rms = curr_feature_vector.rms();

			//curr_feature_vector = curr_feature_vector/curr_feature_vector_rms;

            curr_feature_sptr->set_feature_vector( curr_feature_vector );

			vnl_vector<double> target_feature_vector(curr_feature_vector.size(),target_intensity);

			//target_feature_vector = (target_feature_vector - curr_feature_vector_mean);// / curr_feature_vector_rms;

			curr_target_feature_sptr->set_feature_vector(target_feature_vector);

			//vcl_cout << "target intensity = " << target_intensity << vcl_endl;
			//vcl_cout << "curr_feature_vector_mean = " << curr_feature_vector_mean << vcl_endl;
			//
			//vnl_vector<double>::iterator cv_itr;
			//vnl_vector<double>::iterator cv_end = curr_feature_vector.end();
			//vcl_cout << "curr_feature_vector = [";
			//for( cv_itr = curr_feature_vector.begin(); cv_itr != cv_end; ++cv_itr )
			//	vcl_cout << *cv_itr << ' ';
			//vcl_cout << "];" << vcl_endl;

			//vnl_vector<double>::iterator tv_itr;
			//vnl_vector<double>::iterator tv_end = target_feature_vector.end();
			//vcl_cout << "curr_target_vector = [";
			//for( tv_itr = target_feature_vector.begin(); tv_itr != tv_end; ++tv_itr )
			//	vcl_cout << *tv_itr << ' ';
			//vcl_cout << "];\n";


			 
		}//end frame iteration

    }//end target_list_2d_sptr iteration

}//end dncn_factory::extract_features()

void dncn_factory::extract_sift_features( unsigned const& octave_size, unsigned const& num_oct )
{
	
    unsigned num_frames = this->video_stream_.num_frames();
    //load frames into memory for faster access.
    vcl_map<unsigned, vil_image_view<vxl_byte> > img_seq = this->build_frame_map();

    dncn_target_list_2d::target_list_type::iterator tl_itr;
    dncn_target_list_2d::target_list_type::iterator tl_end = this->target_list_2d_sptr_->target_list_end();

	vcl_cout << "Extracting Keypoints..." << vcl_endl;

	for( tl_itr = this->target_list_2d_sptr_->target_list_begin(); tl_itr != tl_end; ++tl_itr )
	{

		dncn_target_2d_sptr curr_target_sptr = *tl_itr;

		

		for( unsigned t = 0; t < img_seq.size(); ++t )//iterate through all images in the sequence
		{
			vcl_cout << "\t Dense Sift on Image: " << t << vcl_endl;

			//make an image resource sptr of the current view (the input required for dense sift)
			vil_image_resource_sptr image_res_sptr = vil_new_image_resource_of_view(img_seq[t]);

			//create the dense sift sptr instance
			bapl_dense_sift_sptr dense_sift_sptr = new bapl_dense_sift(image_res_sptr,octave_size,num_oct);

		}//end sequence iteration
	}//end target iteration
	vcl_cout << "Done Extracting Keypoints..." << vcl_endl;
}//end dncn_factory::extract_sift_features

//bapl_lowe_keypoint_sptr dncn_factory::make_keypoint(vgl_point_2d<unsigned> const& location, bapl_lowe_pyramid_set_sptr pyramid_set)
//{
//	vcl_cout << "Finding Maximal Scale of point (" << location.x() << ", " << location.y() << ")." << vcl_endl;
//	int num_oct = pyramid_set->num_octaves();
//	int oct_size = pyramid_set->octave_size();
//	int max_index = 1;
//	float max_val;
//	
//	for( int index = 1; index < (num_oct*oct_size) - 1; ++index)
//	{
//		const vil_image_view<float>& dog_image = pyramid_set->dog_pyramid(index/oct_size,index%oct_size);
//
//		float scale = vcl_pow(float(2.0),float((index/oct_size)-1));
//
//		vgl_point_2d<float> scaled_target(location.x()/scale,location.y()/scale);
//
//		if(index == 1)
//			max_val = dog_image(scaled_target.x(),scaled_target.y());
//		else
//		{
//			float temp = dog_image(scaled_target.x(),scaled_target.y());
//			if(temp > max_val)
//			{
//				max_val = temp;
//				max_index = index;
//			}
//		}
//	}//end scale iteration
//	float max_scale = vcl_pow(float(2.0),float(max_index/oct_size)-1);
//	float actual_scale;
//	const vil_image_view<float>& orient_img = pyramid_set->grad_orient_at(max_scale,&actual_scale);
//	const vil_image_view<float>& mag_img = pyramid_set->grad_mag_at(max_scale);
//	
//
//	//coordinates at the actual scale
//	float key_x = location.x()/actual_scale;
//	float key_y = location.y()/actual_scale;
//
//	vcl_vector<float> orientations;
//	bapl_lowe_orientation orientor(3.0,36); //same parameters that matt used.
//	orientor.orient_at(key_x,key_y,max_scale,orient_img,mag_img,orientations);
//
//	//for now use only the dominant gradient direction
//	bapl_lowe_keypoint_sptr kp = bapl_lowe_keypoint_new(pyramid_set,key_x,key_y,max_scale,orientations[0]);
//
//	pyramid_set->make_descriptor(dynamic_cast<bapl_lowe_keypoint*>(kp.as_pointer()));
//
//	return kp;
//
//
//}//end dncn_factor::make_keypoint

void dncn_factory::reduce_dimensionality( unsigned const& dimensions_to_retain )
{
    this->target_list_2d_sptr_->reduce_dimensionality(dimensions_to_retain);
}//end dncn_factory::reduce_dimensionality

// BUILD FUNCTIONS
bool dncn_factory::build( point_vector_type point_vector )
{
    this->point_vector_ = point_vector;
    this->build_target_list();
    return this->build();
}//dncn_factory::build

bool dncn_factory::build()
{
    this->calculate_temporal_entropy();
    return this->build_no_entropy();
}// end dncn_factory::build()


bool dncn_factory::build_no_entropy( point_vector_type point_vector )
{
    this->point_vector_ = point_vector;
    this->build_target_list();
    return this->build_no_entropy();
}//end dncn_factory::build_no_entropy

bool dncn_factory::build_no_entropy()
{
    this->sample_pivot_pixels();
    this->extract_neighbors();
    this->extract_features();
    this->reduce_dimensionality(2);

    return true;
}//end dncn_factory::build_no_entropy

bool dncn_factory::build_no_entropy_sift()
{
	this->sample_pivot_pixels();
	this->extract_neighbors();
	//this->extract_sift_features();

	return true;
}

// IO
void dncn_factory::save_entropy_bin( vcl_string const& filename )
{
    vsl_b_ofstream bofs( filename.c_str() );
    vsl_b_write( bofs, this->temporal_entropy_ );
    bofs.close();
}//end dncn_factory::save_entropy_bin

void dncn_factory::load_entropy_bin( vcl_string const& filename )
{
    vsl_b_ifstream bis( filename.c_str() );
    vsl_b_read( bis,this->temporal_entropy_ );
    bis.close();
}//end dncn_factory::load_entropy_bin

void dncn_factory::save_entropy_dat( vcl_string const& filename )
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
}//end dncn_factory::save_entropy_dat

void dncn_factory::save_pivot_pixels_mfile( vcl_string const& filename )
{
    pivot_pixel_set_type::iterator pitr;
    pivot_pixel_set_type::iterator pend = this->pivot_pixel_set_.end();

    vcl_ofstream os(filename.c_str(), vcl_ios::out);

    os << "pivot_pixels = [";
    for( pitr = this->pivot_pixel_set_.begin(); pitr != pend; ++pitr )
    {
        os << pitr->x() << '\t' << pitr->y() << ";\n";
    }//end pivot pixel set iteration
    os << "];\n";
}//end dncn_factory::save_pivot_pixels_dat

void dncn_factory::write_neighborhood_mfile( vcl_string const& filename )
{
    this->target_list_2d_sptr_->write_neighborhood_mfile( filename );
}//dncn_factory::write_neighborhood_mfile

void dncn_factory::save_factory_bin( vcl_string const& filename )
{
    vcl_ofstream of(filename.c_str(),vcl_ios::binary);
    
    of.write( (char*)&*this, sizeof(*this) );
}//end dncn_factory::save_factory_bin

void dncn_factory::read_factory_bin( vcl_string const& filename )
{
    vcl_ifstream ifs(filename.c_str(), vcl_ios::binary);

    ifs.read( (char*)&*this, sizeof(*this) );
}//end dncn_factory::read_factory_bin


// PROTECTED MEMBER FUNCTIONS
vcl_map<unsigned, vil_image_view<vxl_byte> > dncn_factory::build_frame_map()
{
    //1. Load all images to memory to speed up.
    unsigned num_frames = this->video_stream_.num_frames();
    vcl_map<unsigned, vil_image_view<vxl_byte> > img_seq;
    for( unsigned t = 0; t < num_frames; ++t )
    {
        vil_image_view<vxl_byte> grey_img,curr_img;
        this->video_stream_.seek_frame(t);
        vidl_convert_to_view(*video_stream_.current_frame(),curr_img);
        vil_convert_planes_to_grey(curr_img,grey_img);
        img_seq[t] = grey_img;
    }//end video stream iteration

    return img_seq;
}//end dncn_factory::build_frame_map()

