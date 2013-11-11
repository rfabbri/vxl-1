//this is /contrib/bm/ncn1/ncn1/feature.cxx

#include"ncn1_feature.h"

void ncn1_feature::save_feature_dat( vcl_string const& filename )
{
    vcl_ofstream of(filename.c_str());

    unsigned nframes = this->feature_map_.size();
    unsigned ntargets = this->feature_map_[0].size();

    of << "nframes = " << nframes << ";\n";
    of << "ntargets = " << ntargets << ";\n";

    of << "% targets are n x d -> the n'th row is the n'th target and the columns are x and y\n\n";
    of << "targets = zeros(" << ntargets << ",2);\n";

    feature_vector_type::const_iterator fv_itr;
    feature_vector_type::const_iterator fv_end = feature_map_[0].end();

    unsigned indx = 1;
    for( fv_itr = feature_map_[0].begin(); fv_itr != fv_end; ++fv_itr )
    {
        of << "targets(" << indx << ",1) = " << fv_itr->first.x() << ";\n";
        of << "targets(" << indx << ",2) = " << fv_itr->first.y() << ";\n";
        ++indx;
    }  

    of << "\n";
    of << "% feature cell is nframes x ntargets\n\n";
    of << "features = cell(" << nframes << "," << ntargets << ");\n";

    for( unsigned frame_cnt = 0; frame_cnt < nframes; ++frame_cnt )
    {
        
        fv_itr = feature_map_[frame_cnt].begin();
        fv_end = feature_map_[frame_cnt].end();
        unsigned target_cnt = 1;
        for( ; fv_itr != fv_end; ++fv_itr )
        {
            //vcl_cout << "target = " << fv_itr->first << vcl_endl;
            intensity_vector_type intensity_vector = fv_itr->second;
            intensity_vector_type::const_iterator intensity_itr;
            intensity_vector_type::const_iterator intensity_end = intensity_vector.end();
            
            of << "features{" << frame_cnt+1 << "," << target_cnt << "} = [";
            for( intensity_itr = intensity_vector.begin(); intensity_itr != intensity_end; ++intensity_itr )
            {
                of << unsigned(*intensity_itr);
                //vcl_cout << '\t' << unsigned(*intensity_itr) << vcl_endl;
                if( intensity_itr != intensity_end - 1)
                    of << ", ";
            }//end intensity vector iteration
            of << "];\n";
            ++target_cnt;
        }//end target_iteration

    }//end of frame iteration

}//end ncn1_feature::save_feature_dat



void ncn1_feature::reduce_features_2d()
{
   
    unsigned nframes = feature_map_.size();
    unsigned ntargets = feature_map_[0].size();
    unsigned ndimensions = this->ndimensions();
    unsigned dimensions_to_retain = 2;

    vcl_cout << "Reducing " << ndimensions << " dimensional feature vector to " << dimensions_to_retain << " dimensions..." << vcl_endl;

    vnl_matrix<double> target_frame_matrix(nframes*ntargets,ndimensions);

    //target vector to ease indexing

    vcl_vector<vgl_point_2d<unsigned> > target_pixel_vector;
    for( feature_vector_type::iterator target_itr = feature_map_[0].begin(); target_itr != feature_map_[0].end(); ++target_itr ) 
        target_pixel_vector.push_back(target_itr->first);

    feature_map_type::const_iterator frame_itr;
    feature_map_type::const_iterator frame_end = this->feature_map_.end();

    vcl_cout << "Forming Target/Frame Matrix..." << vcl_endl;
    unsigned row_indx;
    for( frame_itr = this->feature_map_.begin(), row_indx = 0; frame_itr != frame_end; ++frame_itr )
    {
        feature_vector_type::const_iterator target_itr;
        feature_vector_type::const_iterator target_end = frame_itr->second.end();

        for( target_itr = frame_itr->second.begin(); target_itr != target_end; ++target_itr,++row_indx )
        {
            intensity_vector_type::const_iterator feat_vect_itr;
            intensity_vector_type::const_iterator feat_vect_end = target_itr->second.end();
            unsigned col_indx;
            for( feat_vect_itr = target_itr->second.begin(), col_indx = 0; feat_vect_itr != feat_vect_end; ++feat_vect_itr,++col_indx )
            {
                target_frame_matrix(row_indx,col_indx) = double(*feat_vect_itr);
            }// end feature vector iteration
        }//end target iteration
    }//end frame iteration

    //need to precondition the target_frame_matrix so that it will have a good condition number. 
    //We want a square matrix with zero mean and values ranging between -1 and 1 => construct covariance matrix.

    vcl_cout << "Zeroing Target/Frame Matrix..." << vcl_endl;

    vnl_matrix<double> zeroed_matrix(target_frame_matrix.rows(),target_frame_matrix.cols());
    
    for( unsigned col = 0; col < target_frame_matrix.cols(); ++col )
    {
        double col_sum = 0;
       
        //calculate row major sum
        for( unsigned row = 0; row < target_frame_matrix.rows(); ++row )
            col_sum+=target_frame_matrix(row,col);

        //calculate row major mean
        double mean = col_sum/(target_frame_matrix.rows());

        //zero the data accross each dimension
        for( unsigned row = 0; row < target_frame_matrix.rows(); ++row )
            zeroed_matrix(row,col) = target_frame_matrix(row,col) - mean;
    }//end column iteration
   
    vcl_cout << "Building Covariance Matrix... " << vcl_endl;
    vnl_matrix<double> covariance_matrix(ndimensions,ndimensions);
    covariance_matrix = zeroed_matrix.transpose() * zeroed_matrix;
    
    //can now do svd on the feature covariance matrix and reduce the dimensionality of each feature vector
    vcl_cout << "Computing SVD of Covariance Matrix..." << vcl_endl;
    vnl_svd<double> svd(covariance_matrix);

    vcl_cout << "Compute PCA using the result of SVD..." << vcl_endl;
    vnl_matrix<double> U = svd.U();
    vnl_matrix<double> reduced_U(ndimensions,dimensions_to_retain);
    vnl_matrix<double> reduced_dimensionality_point_matrix(target_frame_matrix.rows(),dimensions_to_retain);
    U.extract(reduced_U);
    reduced_dimensionality_point_matrix = target_frame_matrix * reduced_U;

    vcl_cout << vcl_endl;
    //vcl_cout << reduced_dimensionality_point_matrix << vcl_endl;
    vcl_cout << "1 % 4  = " << 1%4 << vcl_endl;
    //now re-organize and place into the frame_reduced_feature_map
    frame_reduced_feature_map_2d_type temp_reduced_frame_map;

    vcl_cout << "Re-organizing reduced points into reduced frame/target map..." << vcl_endl;
    
    //for( unsigned row_indx = 0; row_indx < reduced_dimensionality_point_matrix.rows(); ++row_indx )
    //{
    //    vgl_point_2d<double> temp_reduced_pt;
    //    vgl_point_2d<unsigned> temp_target_coord;
    //    unsigned curr_frame_indx = row_indx / ntargets;
    //    unsigned curr_target_indx = row_indx % ntargets;
    //    temp_reduced_pt.set(reduced_dimensionality_point_matrix(row_indx,1),reduced_dimensionality_point_matrix(row_indx,2));
    //    temp_target_coord.set(target_pixel_vector[curr_target_indx].x(),target_pixel_vector[curr_target_indx].y());


    //}


    frame_reduced_feature_map_2d_type temp_frame_map;
    for( unsigned frame_indx = 0; frame_indx < nframes; ++frame_indx )
    {
        target_reduced_feature_map_2d_type temp_target_map;
        for( unsigned target_indx = 0; target_indx < ntargets; ++target_indx )
        {
            unsigned linear_indx = frame_indx*ntargets + target_indx;
            vgl_point_2d<double> temp_reduced_pt;
            vgl_point_2d<unsigned> temp_target_coord;
            vcl_cout << reduced_dimensionality_point_matrix(linear_indx,0) << '\t' << reduced_dimensionality_point_matrix(linear_indx,1) << vcl_endl;
            temp_reduced_pt.set(reduced_dimensionality_point_matrix(linear_indx,0),reduced_dimensionality_point_matrix(linear_indx,1));
            temp_target_coord.set(target_pixel_vector[target_indx].x(),target_pixel_vector[target_indx].y());
            temp_target_map[temp_target_coord] = temp_reduced_pt;
        }
        temp_frame_map[frame_indx] = temp_target_map;
    }
        
    this->frame_feature_map_2d_= temp_frame_map;
        
    vcl_cout << "Dimensionality Reduction Complete..." << vcl_endl;

    
     
}//end ncn1_feature::reduce_features_2d()


void ncn1_feature::save_2d_pts_dat( vcl_string const& filename )
{
    vcl_ofstream os(filename.c_str(),vcl_ios::out);

    frame_reduced_feature_map_2d_type::const_iterator frame_itr;
    frame_reduced_feature_map_2d_type::const_iterator frame_end = this->frame_feature_map_2d_.end();

    os << "feature_cell = cell(" << this->frame_feature_map_2d_.size() << ",1);\n";

    unsigned frame_cnt;
    double xmin, xmax, ymin, ymax;
    //remember matlab indexing starts from 1
    for( frame_itr = this->frame_feature_map_2d_.begin(), frame_cnt = 1; frame_itr != frame_end; ++frame_itr, ++frame_cnt )
    {
        target_reduced_feature_map_2d_type::const_iterator target_itr;
        target_reduced_feature_map_2d_type::const_iterator target_end = frame_itr->second.end();

        //only output a target list once
        if( frame_cnt == 1 )
        {
            os << "target_list = [";

            unsigned target_cnt;
            for( target_itr = frame_itr->second.begin(), target_cnt = 0; target_itr != target_end; ++target_itr, ++target_cnt )
            {
                os << target_itr->first.x() << '\t' << target_itr->first.y();
                if( target_cnt == frame_itr->second.size() - 1)
                    os << "];\n";
                else
                    os << ";\n";
            }
        }

        os << "feature_cell{" << frame_cnt << "} = [";

        unsigned target_cnt;
        
        for( target_itr = frame_itr->second.begin(),target_cnt = 0; target_itr != target_end; ++target_itr, ++target_cnt )
        {
            if( target_cnt == 0 && frame_cnt == 1 )
            {
                xmin = target_itr->second.x();
                xmax = target_itr->second.x();
                ymin = target_itr->second.y();
                ymax = target_itr->second.y();
            }

            if( target_itr->second.x() > xmax )
                xmax = target_itr->second.x();
            if( target_itr->second.x() < xmin )
                xmin = target_itr->second.x();
            if( target_itr->second.y() > ymax )
                ymax = target_itr->second.y();
            if( target_itr->second.y() < ymin )
                ymin = target_itr->second.y();

            os << target_itr->second.x() << '\t' << target_itr->second.y();
            if( target_cnt < frame_itr->second.size() - 1 )
                os << ";\n";
            else
                os << "];\n";
        }

    }

    os << "\n\n";
    unsigned nframes = this->frame_feature_map_2d_.size();
    unsigned ntargets = this->frame_feature_map_2d_[0].size();
    os << "color = ['r' 'g' 'b' 'k' 'c'];\n";

    
    os << "for i = 1:" << nframes <<",\n";
    os << '\t' <<"figure\n";
    os << '\t' << "for j = 1:" << ntargets << ",\n";
    os << '\t' << '\t' << "hold on, plot(feature_cell{i}(j,1), feature_cell{i}(j,2), [color( mod((j-1),5)+1) '.'], \'MarkerSize\', 10 );\n";
    os << '\t' << '\t' << "axis([" << xmin << ',' << xmax << ',' << ymin << ',' << ymax << "]);\n";
    os << '\t' << '\t' << "xlabel(\'PCA 1\');\n";
    os << '\t' << '\t' << "ylabel(\'PCA 2\');\n";
    os << '\t' << "end\n";
    os << "end\n";

}//end ncn1_feature::save_2d_pts_dat
