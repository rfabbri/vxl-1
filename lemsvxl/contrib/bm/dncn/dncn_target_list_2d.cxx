//this is contrib/bm/dncn/dncn_target_list_2d.cxx
#include"dncn_target_list_2d.h"

void dncn_target_list_2d::add_target( dncn_target_2d_sptr target_sptr )
{
    //vcl_pair<target_list_type::iterator,bool> ret = this->target_list_.insert( vcl_pair<vgl_point_2d<unsigned>, dncn_target_2d_sptr>(target_sptr->target(),target_sptr) );

    //if( ret.second == false )
    //{
    //    this->target_list_.erase(ret.first);
    //    this->target_list_.insert( vcl_pair<vgl_point_2d<unsigned>, dncn_target_2d_sptr>(target_sptr->target(),target_sptr) );
    //}

	this->target_list_.push_back(target_sptr);
}//end dncn_target_list_2d::add_target


void dncn_target_list_2d::reduce_dimensionality( unsigned const& dimensions_to_retain )
{
    unsigned num_targets = this->num_targets();   
    unsigned num_dimensions = this->num_neighbors() + 1;
	unsigned num_frames = this->frames_;

    vnl_matrix<double> data(num_targets*num_frames,num_dimensions);

    target_list_type::iterator target_list_itr;
    target_list_type::iterator target_list_end = this->target_list_.end();

    //vcl_vector<vgl_point_2d<unsigned> > target_index_vector; //to maintain ordering

    //this will create the data matrix who's n'th row is the n'th data point.
    unsigned data_row_indx;
    for(target_list_itr = this->target_list_.begin(), data_row_indx = 0; target_list_itr != target_list_end; ++target_list_itr)
    {
        //target_index_vector.push_back(target_list_itr->first);
        
		dncn_target_2d_sptr curr_target_sptr = *target_list_itr;

        dncn_target_2d::feature_map_type::iterator feature_map_itr;
		dncn_target_2d::feature_map_type::iterator feature_map_end = curr_target_sptr->feature_map_end();
		
		for( feature_map_itr = curr_target_sptr->feature_map_begin(); feature_map_itr != feature_map_end; ++feature_map_itr )
        {
			data.set_row(data_row_indx,feature_map_itr->second->feature_vector());
            ++data_row_indx;
        }//end feature map (frame) iteration
        //++data_row_indx;
    }//end target iteration



    //DATA CONDITIONING
    vnl_matrix<double> data_adjusted_matrix(data.rows(), data.cols());

	this->dimension_means_.set_size(data.cols());

    for( unsigned col = 0; col < data.cols(); ++col )
    {
        double col_sum = 0;

        //calculate the sum down the row for the given column (feature dimension)
        for( unsigned row = 0; row < data.rows(); ++row )
            col_sum += data(row,col);

        //calculate the mean along the dimension
        double mean = col_sum/(data.rows());

		this->dimension_means_[col] = mean;

        //zero the data accross each dimension
        for( unsigned row = 0; row < data.rows(); ++row )
            data_adjusted_matrix(row,col) = data(row,col) - mean;
    }//end column iteration and zeroing

    vcl_cout << "Building Covariance Matrix..." << vcl_flush;
    vnl_matrix<double> covariance_matrix = data_adjusted_matrix.transpose() * data_adjusted_matrix;
	//vnl_matrix<double> covariance_matrix = data.transpose() * data;

    //can now do svd on the feature vocariance matrix and reduce the dimensionality of each feature vector
    vcl_cout << "Computing SVD of Covariance Matrix..." << vcl_endl;
    vnl_svd<double> svd(covariance_matrix);

    vcl_cout << "Computing PCA using the result of SVD..." << vcl_endl;
    vnl_matrix<double> U = svd.U();
    //vnl_matrix<double> reduced_U_(num_dimensions, dimensions_to_retain);
	this->reduced_U_.set_size(num_dimensions, dimensions_to_retain);
    vnl_matrix<double> reduced_data(data.rows(), dimensions_to_retain);
    U.extract(reduced_U_);
    reduced_data = data * reduced_U_;

    vcl_cout << "Re-organizing reduced points into reduced feature maps..." << vcl_endl;

    data_row_indx = 0;
    for( target_list_itr = this->target_list_.begin(); target_list_itr != target_list_end; ++target_list_itr )
    {
		dncn_target_2d_sptr curr_target_sptr = *target_list_itr;
        dncn_target_2d::feature_map_type::iterator feature_map_itr;
		dncn_target_2d::feature_map_type::iterator feature_map_end = curr_target_sptr->feature_map_end();

		for( feature_map_itr = curr_target_sptr->feature_map_begin(); feature_map_itr != feature_map_end; ++feature_map_itr )
        {
            dncn_feature_sptr curr_feature_sptr = feature_map_itr->second;

			vnl_vector<double> curr_reduced_feature_vector = reduced_data.get_row(data_row_indx);

            curr_feature_sptr->set_feature_vector_reduced(curr_reduced_feature_vector);
            ++data_row_indx;
        }//end feature map (frame) iteration over the current target
        //++data_row_indx;

		dncn_target_2d::feature_map_type::iterator target_map_itr;
		dncn_target_2d::feature_map_type::iterator target_map_end = curr_target_sptr->target_map_end();

		for( target_map_itr = curr_target_sptr->target_map_begin(); target_map_itr != target_map_end; ++target_map_itr )
		{
			dncn_feature_sptr curr_target_feature_sptr = target_map_itr->second;

			vnl_vector<double> curr_target_vector = curr_target_feature_sptr->feature_vector();

			//assert(this->dimension_means_.size() == curr_target_vector.size() );

			//vcl_cout << "Original curr_target_vector = " << curr_target_vector << vcl_endl << vcl_endl;

			//for( unsigned i = 0; i < curr_target_vector.size(); ++i )
			//	curr_target_vector[i] = curr_target_vector[i] - dimension_means_[i];

			vnl_vector<double> curr_target_reduced_vector = curr_target_vector * reduced_U_;

			//vcl_cout << "dimension_means_ = " << dimension_means_ << vcl_endl << vcl_endl;

			//vcl_cout << "mean subtracted curr_target_vector = " << curr_target_vector << vcl_endl << vcl_endl;

			//vcl_cout << "curr_target_reduced_vector = " << curr_target_reduced_vector << vcl_endl << vcl_endl << vcl_endl;

			curr_target_feature_sptr->set_feature_vector_reduced(curr_target_reduced_vector);

		}//end target map (frame) iteration over the current target location

    }//end target list iteration

	

    vcl_cout << "Feature Reduction Complete." << vcl_endl;

}//end dncn_target_list_2d::reduce_dimensionality

void dncn_target_list_2d::write_neighborhood_mfile( vcl_string const& filename )
{
    vcl_ofstream os(filename.c_str(), vcl_ios::out);

    os << "neighborhoods = cell(" << this->num_targets() << ", 2);\n";

    target_list_type::iterator target_list_itr;
    target_list_type::iterator target_list_end = this->target_list_.end();

    //REMEMBER MATLAB INDEXING STARTS FROM 1
    unsigned cell_indx;
    for( target_list_itr = this->target_list_.begin(), cell_indx = 1; target_list_itr != target_list_end; ++target_list_itr, ++cell_indx )
    {

		dncn_target_2d_sptr curr_target_sptr = *target_list_itr;

		os << "neighborhoods{" << cell_indx << ",1} = [" << curr_target_sptr->target().x() << '\t' << curr_target_sptr->target().y() << "];\n";

        dncn_neighborhood_2d_sptr curr_neighborhood_sptr = curr_target_sptr->neighborhood_sptr();
        dncn_neighborhood_2d::neighborhood_type::iterator neighborhood_itr;
        dncn_neighborhood_2d::neighborhood_type::iterator neighborhood_end = curr_neighborhood_sptr->neighborhood_end();

        //neighborhood iteration
        os << "neighborhoods{" << cell_indx << ",2} = [";
        for( neighborhood_itr = curr_neighborhood_sptr->neighborhood_begin(); neighborhood_itr != neighborhood_end; ++neighborhood_itr )
            os << neighborhood_itr->x() << '\t' << neighborhood_itr->y() << ";\n";
        os << "];\n";
    }//end target list iteration
}//end dncn_target_list_2d::save_neighborhood_mfile

void dncn_target_list_2d::write_feature_mfile( vcl_string const& filename, unsigned const& num_frames )
{
    vcl_ofstream os(filename.c_str(), vcl_ios::out);

    os << "features = cell(" << this->num_targets() << ',' << num_frames+ 1 << ");\n";

    target_list_type::iterator target_list_itr;
    target_list_type::iterator target_list_end = this->target_list_.end();

    //REMEMBER MATLAB INDEXING STARTS FROM 1
    unsigned cell_indx;
    for( target_list_itr = this->target_list_.begin(), cell_indx = 1; target_list_itr != target_list_end; ++target_list_itr, ++cell_indx )
    {
		dncn_target_2d_sptr curr_target_sptr = *target_list_itr;
        //1. write the target to the first entry of the cell
		os << "features{" << cell_indx <<",1} = [" << curr_target_sptr->target().x() << '\t' << curr_target_sptr->target().y() << "];\n";

        //2. create a matrix that is frame x num_dimensions long to store the feature vector
        dncn_target_2d::feature_map_type::iterator curr_feature_map_itr;
		dncn_target_2d::feature_map_type::iterator curr_feature_map_end = curr_target_sptr->feature_map_end();

        unsigned frame_indx;
		for( curr_feature_map_itr = curr_target_sptr->feature_map_begin(), frame_indx = 2; curr_feature_map_itr != curr_feature_map_end; ++curr_feature_map_itr, ++frame_indx )
        {
            dncn_feature_sptr curr_feature = curr_feature_map_itr->second;
            dncn_feature::feature_vector_type::const_iterator curr_feature_vector_itr;
            dncn_feature::feature_vector_type::const_iterator curr_feature_vector_end = curr_feature_map_itr->second->feature_vector_end();

            os << "features{" << cell_indx << ',' << frame_indx << "} = [";
            for( curr_feature_vector_itr = curr_feature_map_itr->second->feature_vector_begin(); curr_feature_vector_itr != curr_feature_vector_end; ++curr_feature_vector_itr )
            {
                os << *curr_feature_vector_itr << ' ';
            }//end feature vector iteration

            os << "];\n";

        }//end feature map iteration
        

    }//end target list iteration
}//end dncn_target_list_2d::write_features_mfile

void dncn_target_list_2d::write_reduced_feature_mfile( vcl_string const& filename, unsigned const& num_frames )
{
    vcl_ofstream os( filename.c_str(), vcl_ios::out );
    os << "reduced_features = cell(" << this->num_targets() << ',' << num_frames+ 1 << ");\n";

        target_list_type::iterator target_list_itr;
    target_list_type::iterator target_list_end = this->target_list_.end();

    //REMEMBER MATLAB INDEXING STARTS FROM 1
    unsigned cell_indx;
    for( target_list_itr = this->target_list_.begin(), cell_indx = 1; target_list_itr != target_list_end; ++target_list_itr, ++cell_indx )
    {
		dncn_target_2d_sptr curr_target_sptr = *target_list_itr;

        //1. write the target to the first entry of the cell
		os << "reduced_features{" << cell_indx <<",1} = [" << curr_target_sptr->target().x() << '\t' << curr_target_sptr->target().y() << "];\n";

        //2. create a matrix that is frame x num_dimensions long to store the feature vector
        dncn_target_2d::feature_map_type::iterator curr_feature_map_itr;
		dncn_target_2d::feature_map_type::iterator curr_feature_map_end = curr_target_sptr->feature_map_end();

        unsigned frame_indx;
		for( curr_feature_map_itr = curr_target_sptr->feature_map_begin(), frame_indx = 2; curr_feature_map_itr != curr_feature_map_end; ++curr_feature_map_itr, ++frame_indx )
        {
            dncn_feature_sptr curr_feature = curr_feature_map_itr->second;
            dncn_feature::feature_vector_type::const_iterator curr_reduced_feature_vector_itr;
            dncn_feature::feature_vector_type::const_iterator curr_reduced_feature_vector_end = curr_feature_map_itr->second->feature_vector_reduced_end();

            os << "reduced_features{" << cell_indx << ',' << frame_indx << "} = [";
            for( curr_reduced_feature_vector_itr = curr_feature_map_itr->second->feature_vector_reduced_begin(); 
                curr_reduced_feature_vector_itr != curr_reduced_feature_vector_end; ++curr_reduced_feature_vector_itr )
            {
                os << *curr_reduced_feature_vector_itr << ' ';
            }//end feature vector iteration

            os << "];\n";

        }//end feature map iteration
        

    }//end target list iteration

}//end dncn_target_list_2d::write_reduced_feature_mfile