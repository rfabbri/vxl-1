//this is /contrib/bm/dsm/dsm_features.cxx
#include"dsm_features.h"

dsm_features::dsm_features(vcl_string const& neighborhood_xml_path):
	neighborhood_xml_path_(neighborhood_xml_path),  neighborhood_valid_(false), features_valid_(false), dim_reduced_(false),octave_size_(6),
		num_octaves_(1)
{
	this->parse_neighborhood_xml(neighborhood_xml_path);
}//end dsm_features constructor

bool dsm_features::parse_neighborhood_xml(vcl_string const& neighborhood_xml_path)
{
	//Open the XML document
	if( neighborhood_xml_path.size() == 0 )
	{
		vcl_cerr << "dsm_features::parse_neighborhood_xml -- xml file path not set." << vcl_flush;
		this->neighborhood_valid_ = false;
		return false;
	}

	//Check if you can find the root xml node
	bxml_document xml_doc = bxml_read(neighborhood_xml_path);
	if( !xml_doc.root_element() )
	{
		vcl_cerr << "dsm_features::parse_neighborhood_xml -- xml root not found" << vcl_flush;
		this->neighborhood_valid_ = false;
		return false;
	}

	//Check the root is an element
	if( xml_doc.root_element()->type() != bxml_data::ELEMENT )
	{
		vcl_cerr << "dsm_features::parse_neighborhood_xml -- paramse root is not an ELEMENT!" << vcl_flush;
		this->neighborhood_valid_ = false;
		return false;
	}

	bxml_data_sptr root = xml_doc.root_element();
	if(!root)
	{
		vcl_cerr << "dsm_features::parse_neighborhood_xml -- root tag not found!" << vcl_endl;
		this->neighborhood_valid_ = false;
		return false;
	}

	

	bxml_element* h_elm = static_cast<bxml_element*>(root.ptr());
	h_elm->get_attribute("num_targets", this->num_targets_);
	h_elm->get_attribute("num_neighbors", this->num_neighbors_);
	h_elm->get_attribute("video_glob",this->video_glob_);
	

	for( bxml_element::const_data_iterator i = h_elm->data_begin(); i != h_elm->data_end(); ++i )
	{
		bxml_data_sptr elm = *i;
		if( elm->type() == bxml_data::ELEMENT )
		{
			bxml_element* tp = static_cast<bxml_element*>(elm.as_pointer());
			unsigned x, y;
			tp->get_attribute("x",x);
			tp->get_attribute("y",y);
			vgl_point_2d<unsigned> target(x,y);
			
			vcl_vector<vgl_point_2d<unsigned> > neighbors;

			for( bxml_element::const_data_iterator j = tp->data_begin(); j != tp->data_end(); ++j )
			{
				bxml_data_sptr nelm = *j;
				if( nelm->type() == bxml_data::ELEMENT )
				{
					bxml_element* np = static_cast<bxml_element*>(nelm.ptr());
					unsigned nx, ny;
					np->get_attribute("x",nx);
					np->get_attribute("y",ny);
					vgl_point_2d<unsigned> neighbor(nx,ny);
					neighbors.push_back(neighbor);
				}//check if inner data is an element
			}//end inner data iteration (neighbors)

			this->neighborhood_[target] = neighbors;
		}//check if outter data is an element
	}//end outer data iteration (targets)

	this->neighborhood_valid_ = true;
	return true;
}//end dsm_features::parse_xml

bool dsm_features::extract_sift_features(unsigned const& octave_size, unsigned const& num_octaves)
{
	vidl_image_list_istream video_stream(this->video_glob_);


	this->octave_size_ = octave_size;
	this->num_octaves_ = num_octaves;
	unsigned nframes = video_stream.num_frames();

	//load sift objects into memory for speed.
	//vcl_cout << "Creating Dense sift objects..." << vcl_endl;
	for( unsigned t = 0; t < nframes; ++t )
	{
		//vcl_cout << '\t' << (t/nframes)*100 << "% complete..." << vcl_endl;
		vil_image_view<vxl_byte> grey_img, curr_img;
		video_stream.seek_frame(t);
		vidl_convert_to_view(*video_stream.current_frame(),curr_img);
		vil_convert_planes_to_grey(curr_img, grey_img);
		vil_image_resource_sptr img_sptr = vil_new_image_resource_of_view(grey_img);
		bapl_dense_sift_sptr dsift_sptr = new bapl_dense_sift(img_sptr, octave_size, num_octaves);

		vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator
			t_itr, t_end = this->neighborhood_.end();


		for( t_itr = this->neighborhood_.begin(); t_itr != t_end; ++t_itr )
		{

			vnl_vector<double> feature_vector(128*(t_itr->second.size()+1),0);
			bapl_lowe_keypoint_sptr target_keypoint;
			dsift_sptr->make_keypoint(target_keypoint, t_itr->first.x(), t_itr->first.y());
			//vnl_vector<double> feature_vector = target_keypoint->descriptor().as_vector();
			feature_vector.update(target_keypoint->descriptor().as_vector(),0);
			this->lowe_keypoint_map_[t_itr->first][t] = target_keypoint;
			vcl_vector<vgl_point_2d<unsigned> >::const_iterator n_itr, n_end = t_itr->second.end();

			unsigned tn_idx;
			for( n_itr = t_itr->second.begin(), tn_idx = 1; n_itr != n_end; ++n_itr, ++tn_idx )
			{
				bapl_lowe_keypoint_sptr neighbor_keypoint;
				dsift_sptr->make_keypoint(neighbor_keypoint, n_itr->x(), n_itr->y());
				feature_vector.update(neighbor_keypoint->descriptor().as_vector(), 128*tn_idx);
				this->lowe_keypoint_map_[t_itr->first][t] = neighbor_keypoint;
			}//end neighborhood iteration

			this->feature_map_[t_itr->first][t] = feature_vector;
		}//end target iteration
	}//end frame iteration

	this->features_valid_ = true;
	return true;
}//end dsm_features::extract_features()

bool dsm_features::extract_dsift_features( float const& orientation )
{
	vidl_image_list_istream video_stream(this->video_glob_);
	unsigned nframes = video_stream.num_frames();

	bapl_dsift_sptr dsift_sptr = new bapl_dsift();

	for( unsigned t = 0; t < nframes; ++t )
	{
		vcl_cout << '\t' << (float(t)/nframes)*100 << "% complete..." << vcl_endl;
		vil_image_view<vxl_byte> curr_img;
		video_stream.seek_frame(t);
		vidl_convert_to_view(*video_stream.current_frame(),curr_img);
		vil_image_view<float> grey_img;
		vil_convert_planes_to_grey(curr_img, grey_img);

		dsift_sptr->set_img(grey_img);

		vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator
			t_itr, t_end = this->neighborhood_.end();

		for( t_itr = this->neighborhood_.begin(); t_itr != t_end; ++t_itr )
		{
			vnl_vector<double> feature_vector = dsift_sptr->vnl_dsift(t_itr->first.x(), t_itr->first.y(), 0.0f);

			vcl_vector<vgl_point_2d<unsigned> >::const_iterator n_itr, n_end = t_itr->second.end();

			unsigned tn_idx;
			for( n_itr = t_itr->second.begin(),tn_idx = 1; n_itr != n_end; ++n_itr, ++tn_idx )
			{
				vnl_vector<double> neighborhood_descriptor = dsift_sptr->vnl_dsift(n_itr->x(), n_itr->y(), 0.0f);
				feature_vector.update(neighborhood_descriptor,128*tn_idx);
			}//end neighborhood iteration
			this->feature_map_[t_itr->first][t] = feature_vector;
		}//end target iteration
	}//end frame iteration

	this->features_valid_ = true;
	return true;
}//end dsm_features::extract_dsift

bool dsm_features::extract_intensity_ratio_features()
{
	//The first dimension is the intensity of the target divided by the mean of the neighbors.
	//the rest of the dimensions is the intensity of the target divided by the corresponding neighbor's intensity.
	vidl_image_list_istream video_stream(this->video_glob_);

	unsigned nframes = video_stream.num_frames();

	vcl_cout << "Extracting Target Intensity Ratio Features..." << vcl_endl;
	for(unsigned t = 0; t < nframes; ++t)
	{
		vcl_cout << '\t' << "Frame Iteration " << (double(t)/nframes)*100 << "% complete." << vcl_endl;

		vil_image_view<vxl_byte> grey_img, curr_img;
		video_stream.seek_frame(t);
		vidl_convert_to_view(*video_stream.current_frame(),curr_img);
		vil_convert_planes_to_grey(curr_img, grey_img);

		vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator
			t_itr, t_end = this->neighborhood_.end();

		for( t_itr = this->neighborhood_.begin(); t_itr != t_end; ++t_itr )
		{
			vnl_vector<double> feature_vector(t_itr->second.size()+1,0);
			
			vcl_vector<vgl_point_2d<unsigned> >::const_iterator n_itr, n_end = t_itr->second.end();

			feature_vector[0] = grey_img(t_itr->first.x(), t_itr->first.y());
			
			unsigned feature_vect_idx;
			for( n_itr = t_itr->second.begin(), feature_vect_idx = 1; n_itr != n_end; ++n_itr, ++feature_vect_idx )
			{
				feature_vector[feature_vect_idx] = double(grey_img(n_itr->x(), n_itr->y()));									
			}//end neighborhood iteration

			vnl_vector<double> neighborhood_intensity = feature_vector.extract(t_itr->second.size(),1);
		
			//add vnl_math::eps to avoid divide by zero.
			for(unsigned i = 1; i < feature_vector.size(); ++i)
				feature_vector[i] = feature_vector[0] / (feature_vector[i] + vnl_math::eps);
			

			feature_vector[0] /= (neighborhood_intensity.mean() + vnl_math::eps);			

			this->feature_map_[t_itr->first][t] = feature_vector;
		}//end target iteration
	}//end frame iteration
	return true;
}//end dsm_features::extract_target_intensity_ratio_features

bool dsm_features::extract_intensity_features()
{
	vidl_image_list_istream video_stream(this->video_glob_);

	unsigned nframes = video_stream.num_frames();

	vcl_cout << "Extracting Intensity Feature Vector..." << vcl_endl;
			
	for( unsigned t = 0; t < nframes; ++t )
	{

		vcl_cout << '\t' << "Frame Iteration " << (double(t)/nframes)*100 << "% complete." << vcl_endl;
		vil_image_view<vxl_byte> grey_img, curr_img;
		video_stream.seek_frame(t);
		vidl_convert_to_view(*video_stream.current_frame(),curr_img);
		vil_convert_planes_to_grey(curr_img, grey_img);

		vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator
			t_itr, t_end = this->neighborhood_.end();

		for( t_itr = this->neighborhood_.begin(); t_itr != t_end; ++t_itr )
		{
			vnl_vector<double> feature_vector(t_itr->second.size()+1,0);
			feature_vector[0] = grey_img(t_itr->first.x(), t_itr->first.y());
			
			vcl_vector<vgl_point_2d<unsigned> >::const_iterator n_itr, n_end = t_itr->second.end();

			unsigned feat_vect_idx;
			for( n_itr = t_itr->second.begin(), feat_vect_idx = 1; n_itr != n_end; ++n_itr, ++feat_vect_idx )
				feature_vector[feat_vect_idx] = double(grey_img(n_itr->x(),n_itr->y()));

			vcl_vector<double> fv(feature_vector.begin(), feature_vector.end());

			this->feature_map_[t_itr->first][t] = feature_vector;
		}//end target iteration
		
	}//end target iteration
	return true;
}//end dsm_features::extract_intensity_features()

bool dsm_features::reduce_dimensionality(unsigned const& ndims_to_keep)
{
	vcl_map<vgl_point_2d<unsigned>, vcl_map<unsigned, vnl_vector<double> >, dsm_vgl_point_2d_coord_compare<unsigned>  >::const_iterator
		t_itr, t_end = this->feature_map_.end();


	for( t_itr = this->feature_map_.begin(); t_itr != t_end; ++t_itr )
	{
		vnl_matrix<double> observations(t_itr->second.size(),t_itr->second.begin()->second.size());
		
		vcl_vector<unsigned> frame_idx_vector;
		vcl_map<unsigned, vnl_vector<double> >::const_iterator f_itr, f_end = t_itr->second.end();
		unsigned frame_idx;
		for( f_itr = t_itr->second.begin(), frame_idx = 0; f_itr != f_end; ++f_itr, ++frame_idx )
		{
			observations.set_row(frame_idx,f_itr->second);
			frame_idx_vector.push_back(f_itr->first);
		}

		//zero out the mean of each dimension of the observations
		observations.normalize_columns();

		//calculate covariance matrix
		vnl_matrix<double> covar(observations.cols(),observations.cols());
		covar = observations.transpose()*observations;

		//compute svd to get eigenvectors of covar
		vnl_svd<double> svd(covar);
		vnl_matrix<double> U_reduced(covar.rows(),ndims_to_keep);
		svd.U().extract(U_reduced);

		vnl_matrix<double> reduced_features = observations*U_reduced;

		vcl_vector<unsigned>::const_iterator fidx_itr, fidx_end = frame_idx_vector.end();

		//put the results of dimensionality reduction into reduced target/frame map.
		unsigned reduced_row_idx;
		for(fidx_itr = frame_idx_vector.begin(), reduced_row_idx = 0; fidx_itr != fidx_end; ++fidx_itr, ++reduced_row_idx)
			this->reduced_feature_map_[t_itr->first][*fidx_itr] = reduced_features.get_row(reduced_row_idx);

	}//end target iteration
	
	this->dim_reduced_ = true;
	return this->dim_reduced_;
}//end dsm_features::reduce_dimensionality

bool dsm_features::write_reduced_features_mfile(vcl_string const& filename) const
{
	//THE 2D TARGET LOCATION IS THE FIRST COLUMN OF THE CELL
	//THE NFRAMES X FEATURE DIM MATRIX IS THE SECOND COLUMN OF THE CELL
	//THE NUMBER OF ROWS OF THE CELL CORRESPOND TO THE NUMBER OF TARGETS
	if( this->dim_reduced_ == true )
	{
		vcl_ofstream os( filename.c_str(), vcl_ios::out );

		vidl_image_list_istream video_stream(this->video_glob_);

		unsigned width = video_stream.width();
		unsigned height = video_stream.height();

		os << "video_glob = '" << this->video_glob_ << "';\n\n";
		os << "video_path = video_glob(1:findstr(video_glob,'*')-1);\n\n";

		os << "ntargets = " << this->reduced_feature_map_.size() << ";\n\n";

		os << "features = cell(" << this->reduced_feature_map_.size() << "," << "2);\n";

		os << "filenames = dir(video_glob);\n";
		os << "img_seq = uint8(zeros(" << height << ',' << width << "));\n";
		os << "%for i = 1:length(filenames)\n";
		os << '\t' << "%img_seq(:,:,i) = rgb2gray(imread(strcat(video_path,filenames(i).name())));\n";
		os << "%end\n\n";

		vcl_map<vgl_point_2d<unsigned>, vcl_map<unsigned, vnl_vector<double> >,dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator
			t_itr, t_end = this->reduced_feature_map_.end();

		//TARGET INDEXING STARTS FROM 1!
		unsigned target_idx;
		for( t_itr = this->reduced_feature_map_.begin(), target_idx = 1; t_itr != t_end; ++t_itr, ++target_idx )
		{
			//1 is added to each coordinate because of indexing into the image in matlab coordinates.
			os << "features{" <<target_idx << ",1} = [" << t_itr->first.x()+1 << ", " << t_itr->first.y()+1 << "];\n\n";

			vcl_map<unsigned, vnl_vector<double> >::const_iterator f_itr, f_end = t_itr->second.end();
			unsigned nframes = t_itr->second.size();
			unsigned frame_idx;
			os << "features{" << target_idx << ",2} = [";

			for( f_itr = t_itr->second.begin(), frame_idx = 0; f_itr != f_end; ++f_itr , ++frame_idx)
			{
				unsigned ndims = f_itr->second.size();
				
				if( frame_idx < nframes - 1)
				{
					for( unsigned i = 0; i < ndims; ++ i)
					{
						if(i < ndims-1)
							os << f_itr->second[i] << ", ";
						else
							os << f_itr->second[i] << ";\n";
					}
				}
				else
				{
					for(unsigned i = 0; i < ndims; ++i )
					{
						if(i < ndims-1)
							os << f_itr->second[i] << ", ";
						else
							os << f_itr->second[i] << "];\n\n";
					}
				}

			}//end frame iteration
		}//end target iteration
		return true;
	}
	else
	{
		vcl_cerr << "ERROR: dsm_features::write_reduced_features_mfile -- dim_reduced_ = false." << vcl_flush;
		return false;
	}
}//end dsm_features::write_reduced_features_mfile

void dsm_features::b_read(vsl_b_istream& is)
{
	if(!is) return;

	short v;
	vsl_b_read(is,v);

	switch(v)
	{
	case 1:
		{
			vsl_b_read(is, this->neighborhood_xml_path_);
			vsl_b_read(is, this->video_glob_);
			vsl_b_read(is, this->num_targets_);
			vsl_b_read(is, this->num_neighbors_);
			vsl_b_read(is, this->neighborhood_valid_);
			vsl_b_read(is, this->features_valid_);
			vsl_b_read(is, this->dim_reduced_);
			vsl_b_read(is, this->octave_size_);
			vsl_b_read(is, this->num_octaves_);

			//read the neighborhood
			if( this->neighborhood_valid_ )
			{
				for( unsigned target_idx = 0; target_idx < this->num_targets_; ++target_idx )
				{
					vgl_point_2d<unsigned> target;
					vcl_vector<vgl_point_2d<unsigned> >neighborhood;
					vsl_b_read(is, target);
					for( unsigned neighbor_idx = 0; neighbor_idx < this->num_neighbors_; ++neighbor_idx )
					{
						vgl_point_2d<unsigned> neighbor;
						vsl_b_read(is,neighbor);
						neighborhood.push_back(neighbor);
					}//end neighbor iteration
					this->neighborhood_[target] = neighborhood;
				}//end target iteration
			}//end if(this->neighborhood_valid_)

			//read the feature map
			if( this->features_valid_ )
			{
				unsigned ntargets;
				vsl_b_read(is,ntargets);

				for( unsigned target_idx = 0; target_idx < ntargets; ++target_idx )
				{
					vgl_point_2d<unsigned> target;
					vsl_b_read(is,target);
					unsigned nframes;
					vsl_b_read(is,nframes);

					for( unsigned frame_idx = 0; frame_idx < nframes; ++frame_idx )
					{
						unsigned curr_frame;
						vsl_b_read(is,curr_frame);
						vnl_vector<double> feature_vector;
						vsl_b_read(is,feature_vector);
						this->feature_map_[target][curr_frame]=feature_vector;
					}//end frame iteration
					
				}//end target iteration
			}//end if(this->features_valid_)

			//read the reduced features
			if( this->dim_reduced_ )
			{
				unsigned ntargets;
				vsl_b_read(is, ntargets);
				
				for( unsigned target_idx = 0; target_idx < ntargets; ++target_idx )
				{
					vgl_point_2d<unsigned> target;
					vsl_b_read(is,target);

					unsigned nframes;
					vsl_b_read(is, nframes);

					for( unsigned frame_idx = 0; frame_idx < nframes; ++frame_idx )
					{
						unsigned curr_frame;
						vsl_b_read(is,curr_frame);
						vnl_vector<double> feature_vector;
						vsl_b_read(is, feature_vector);

						this->reduced_feature_map_[target][curr_frame] = feature_vector;
					}//end frame iteration
				}//end target iteration
			}//end if(this->dim_reduced_)
			return;
		}//end case 1
	default:
		{
			vcl_cerr << "ERROR: dsm_features::b_read() -- unknown version number." << vcl_flush;
			return;
		}//end default
	}//end switch(v)
}//end dsm_features::b_read()

void dsm_features::b_write(vsl_b_ostream& os) const
{
	const short version_no = 1;
	vsl_b_write(os, version_no);
	vsl_b_write(os, this->neighborhood_xml_path_);
	vsl_b_write(os, this->video_glob_);
	vsl_b_write(os, this->num_targets_);
	vsl_b_write(os, this->num_neighbors_);
	vsl_b_write(os, this->neighborhood_valid_);
	vsl_b_write(os, this->features_valid_);
	vsl_b_write(os, this->dim_reduced_);
	vsl_b_write(os, this->octave_size_);
	vsl_b_write(os, this->num_octaves_);

	//write the neighborhood
	if( this->neighborhood_valid_ )
	{
		//write the neighborhood
		vcl_map<vgl_point_2d<unsigned>, vcl_vector<vgl_point_2d<unsigned> >, dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator 
			tn_itr, tn_end = this->neighborhood_.end();
		for( tn_itr = this->neighborhood_.begin(); tn_itr != tn_end; ++tn_itr )
		{
			vsl_b_write(os, tn_itr->first);
			vcl_vector<vgl_point_2d<unsigned> >::const_iterator nv_itr, nv_end = tn_itr->second.end();
			for( nv_itr = tn_itr->second.begin(); nv_itr != nv_end; ++nv_itr )
				vsl_b_write(os,*nv_itr);
		}//end neighborhood iteration
	}//end if(neighborhood_valid_)

	//write the feature map
	if( this->features_valid_ )
	{
		unsigned ntargets = this->feature_map_.size();
		vsl_b_write(os, ntargets);

		vcl_map<vgl_point_2d<unsigned>, vcl_map<unsigned, vnl_vector<double> >,dsm_vgl_point_2d_coord_compare<unsigned>  >::const_iterator
			t_itr, t_end = this->feature_map_.end();

		for( t_itr = this->feature_map_.begin(); t_itr != t_end; ++t_itr )
		{
			vsl_b_write(os, t_itr->first);
			unsigned nframes = t_itr->second.size();
			vsl_b_write(os,nframes);
			vcl_map<unsigned, vnl_vector<double> >::const_iterator f_itr, f_end = t_itr->second.end();

			for( f_itr = t_itr->second.begin(); f_itr != f_end; ++f_itr )
			{
				vsl_b_write(os,f_itr->first);
				vsl_b_write(os, f_itr->second);
			}
		}//end target iteration
	}

	//write reduced feature map
	if( this->dim_reduced_ )
	{
		unsigned ntargets = this->reduced_feature_map_.size();
		vsl_b_write(os, ntargets);

		vcl_map<vgl_point_2d<unsigned>, vcl_map<unsigned, vnl_vector<double> > , dsm_vgl_point_2d_coord_compare<unsigned> >::const_iterator
			t_itr, t_end = this->reduced_feature_map_.end();

		for( t_itr = this->reduced_feature_map_.begin(); t_itr != t_end; ++t_itr )
		{
			vsl_b_write(os, t_itr->first);
			unsigned nframes = t_itr->second.size();
			vsl_b_write(os, nframes);
			vcl_map<unsigned, vnl_vector<double> >::const_iterator f_itr, f_end = t_itr->second.end();
		
			for( f_itr = t_itr->second.begin(); f_itr != f_end; ++f_itr )
			{
				vsl_b_write(os, f_itr->first);
				vsl_b_write(os, f_itr->second);
			}
		}//end target iteration
	}//end if(this->dim_reduced_)

}//end dsm_features::b_write()