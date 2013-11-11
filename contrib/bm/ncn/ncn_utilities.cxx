#include "ncn_utilities.h"

void ncn_utilities::vnl_matrix2dat(vcl_ofstream& ofs, vnl_matrix<float> const& mat)
{
    unsigned nrows = mat.rows();
    unsigned ncols = mat.cols();
    for(unsigned row = 0; row < nrows; ++row)
    {
        for(unsigned col = 0; col < ncols; ++col)
        {
            ofs << mat(row,col) << '\t';
        }
        ofs << '\n';
    }
}//end vnl_matrix2dat

void ncn_utilities::vnl_matrix2dat(vcl_ofstream& ofs,vnl_matrix<unsigned> const& mat)
{
    unsigned nrows = mat.rows();
    unsigned ncols = mat.cols();
    for(unsigned row = 0; row < nrows; ++row)
    {
        for(unsigned col = 0; col < ncols; ++col)
        {
            ofs << mat(row,col) << '\t';
        }
        ofs << '\n';
    }
}//end vnl_matrix2dat

void ncn_utilities::vnl_vector2dat(vcl_ofstream& ofs, vnl_vector<float> const& vec)
{
    vnl_vector<float>::const_iterator vec_itr = vec.begin();
    vnl_vector<float>::const_iterator vec_end = vec.end();

    for(;vec_itr!=vec_end;++vec_itr)
        ofs<< *vec_itr << '\t';

}//end vnl_vector2dat

//calculate_temporal_entropy can be parallelized with opencl
//return normalized entropy_matrix
vnl_matrix<float> ncn_utilities::calculate_temporal_entropy(vcl_map<unsigned, vil_image_view<float> >& img_sequence, unsigned const& nbins)
{
    unsigned nrows = img_sequence[1].nj();
    unsigned ncols = img_sequence[1].ni();
    unsigned nobs = img_sequence.size();
    bsta_histogram<float> histogram(float(0),float(255),nbins);
    vnl_matrix<float> output(nrows,ncols);

    for(unsigned row = 0; row < nrows; ++ row)
    {
        for(unsigned col = 0; col < ncols; ++ col)
        {
            histogram.clear();
            for(unsigned t = 0; t < nobs; ++t)
            {
                histogram.upcount(img_sequence[t](col,row,0),1);
            }
            output(row,col) = histogram.entropy();
        }
    }

    ncn_utilities::normalize_entropy_matrix(output);

    return output;
}//end calculate_temporal_entropy

vcl_map<unsigned, vil_image_view<float> > ncn_utilities::load_image_sequence(vcl_string const& img_directory)
{
    vcl_map<unsigned, vil_image_view<float> > output;
    vcl_vector<vcl_string> filenames;

    for(vul_file_iterator file_itr = img_directory + "\\*.jpg"; file_itr; ++file_itr)
        filenames.push_back(vul_file::strip_extension(file_itr.filename()));

    vcl_vector<vcl_string>::iterator filename_itr = filenames.begin();

    //sort filenames into correct order
    vcl_sort(filenames.begin(),filenames.end());

    for(unsigned i = 0; filename_itr != filenames.end(); ++filename_itr, ++i)
    {
        vcl_string curr_filename = img_directory + "\\" + *filename_itr + ".jpg";

        vcl_cout << "Loading Image: " << *filename_itr << vcl_endl;

        //convert to grey scale image
        vil_convert_planes_to_grey<vxl_byte,float>
            (vil_load(curr_filename.c_str()), output[i]);

    }

    return output;
}//end load_image_sequence

bool ncn_utilities::normalize_entropy_matrix(vnl_matrix<float>& entropy_matrix)
{
    // Check if this is a valid entropy matrix. Entropy must be positive.
    if(entropy_matrix.min_value() < 0)
    {
        vcl_cout << "Error ncn:utilities::normalize_entropy_matrix:\n"
                 << '\t' << "Matrix contains negative entropy value" << vcl_flush;
        return false;
    }
    
    vnl_matrix<float>::iterator entropy_matrix_itr = entropy_matrix.begin();

    float abs_sum = entropy_matrix.array_one_norm();

    for(; entropy_matrix_itr != entropy_matrix.end(); ++entropy_matrix_itr)
        *entropy_matrix_itr = *entropy_matrix_itr / abs_sum;

    return true;
}//end normalize_entropy_matrix

bool ncn_utilities::sample_pivot_pixels(vnl_matrix<float> const& entropy_matrix, unsigned const& num_piv_pix, vnl_matrix<unsigned>& output)
{
    unsigned nrows = entropy_matrix.rows();
    unsigned ncols = entropy_matrix.cols();

    output.set_size(num_piv_pix,2);

    vnl_matrix<float>::const_iterator ent_mat_itr = entropy_matrix.begin();
    unsigned npix = entropy_matrix.size();

    vcl_vector<unsigned> index(npix);
    vcl_vector<float> p(npix);
    vcl_vector<float>::iterator p_itr;
    vcl_vector<unsigned> loc;
    
    for(unsigned i = 0; ent_mat_itr < entropy_matrix.end(); ++ent_mat_itr, ++i)
    {
        index[i] = i;
        p[i] = *ent_mat_itr;
    }

    for(unsigned i = 0; i < num_piv_pix; ++i)
    {
        vcl_cout << vcl_setprecision(2) << vcl_fixed << (float(i)/float(num_piv_pix))*100.0f << "% pivot pixels sampled." << vcl_endl;
        loc.clear();
        if(bsta_sampler<unsigned>::sample(index,p,1,loc))
        {
            

            //convert vector index into matrix coordinates
            output(i,0) = loc[0] / nrows;
            output(i,1) = loc[0] % nrows;
                                              
            //sample without replacement
            p[loc[0]] = 0.0f;

            //renormalize
            float p_sum = 0.0f;
            for(p_itr = p.begin(); p_itr != p.end(); ++p_itr)
                p_sum += *p_itr;
            for(p_itr = p.begin(); p_itr != p.end(); ++p_itr)
                *p_itr = *p_itr/p_sum;  
        }
        else
        {
            vcl_cout << "Error ncn_utilities::sample_pivot_pixels, bsta_sampler returned false." << vcl_endl;
            return false;
        }
    }

        

    return true;
}//end sample_pivot_pixels

bool ncn_utilities::sample_pivot_pixels_fast(vnl_matrix<float> const& entropy_matrix, unsigned const& num_piv_pix,
        unsigned const& down_sample_factor, vnl_matrix<unsigned>& output)
{
    output.set_size(num_piv_pix,2);
    unsigned nrows = entropy_matrix.rows();

    vcl_vector<unsigned> index;
    vcl_vector<float> p;
    vnl_matrix<float>::const_iterator ent_itr = entropy_matrix.begin();
    vcl_vector<float>::iterator p_itr;
    vcl_vector<unsigned> linear_index;

    //the key will be the linear index, the value will be the index into the p vector.
    vcl_map<unsigned,unsigned> category_index_map;
    
    for(unsigned i=0;ent_itr < entropy_matrix.end(); ent_itr = ent_itr + down_sample_factor,i++)
    {
        index.push_back(ent_itr - entropy_matrix.begin());
        p.push_back(*ent_itr);
        category_index_map[(ent_itr - entropy_matrix.begin())] = i;
    }

    //normalize p
    float psum = 0.0f;
    for(p_itr = p.begin(); p_itr!=p.end(); ++p_itr)
        psum += *p_itr;
    for(p_itr = p.begin(); p_itr != p.end(); ++p_itr)
        *p_itr = *p_itr/psum;
    
    //check there are more pixels to sample from then desired pivot pixels.
    if(p.size() < num_piv_pix)
    {
        vcl_cout << "Error ncn_utilities::sample_pivot_pixels_fast \n"
                 << "\t The number of pixels to sample from is less than the required number of pivot pixels." << vcl_endl;
        return false;
    }
    //now sample without replacement
    for(unsigned i = 0; i < num_piv_pix; ++i)
    {
        vcl_cout << vcl_setprecision(2) << vcl_fixed << (float(i)/float(num_piv_pix))*100.0f << "% pivot pixels sampled" << vcl_endl;
        linear_index.clear();
        if(bsta_sampler<unsigned>::sample(index,p,1,linear_index))
        {
            //convert the linear index into martix coordinates
            output(i,0) = linear_index[0] / nrows;
            output(i,1) = linear_index[0] % nrows;

            unsigned p_index = category_index_map[linear_index[0]];

            //this is the new normalizing constant as the previous accumulant was unity.
            psum = 1 - p[p_index];

            //sample without replacement
            p[p_index] = 0.0f;

            //renormalize
            for(p_itr=p.begin();p_itr!=p.end();++p_itr)
                *p_itr = *p_itr/psum;
        }
        else
        {
            vcl_cout << "Error ncn_utilities::sample_pivot_pixels_fast, bsta_sampler returned false." << vcl_endl;
            return false;
        }

    }

    return true;
}//end sample_pivot_pixels_fast



bool ncn_utilities::sample_pivot_pixels_rejection(vnl_matrix<float> const& normalized_entropy_matrix, unsigned const& num_piv_pix,
                                                    vnl_matrix<unsigned>& output, unsigned const& down_sample_factor)
{
    //check entropy_matrix is a valid pdf
    if(normalized_entropy_matrix.min_value() < 0 || normalized_entropy_matrix.absolute_value_sum() > 1)
    {
        vcl_cerr << "Error ncn_utilities::sample_pivot_pixels_rejection,\n"
                 << "\t normalized_entropy_matrix is not a valid probability density" << vcl_flush;
        return false;
    }

    vnl_matrix<float>::const_iterator ent_mat_itr;
    vnl_matrix<float>::const_iterator ent_mat_end = normalized_entropy_matrix.end();
    unsigned nrows = normalized_entropy_matrix.rows();

    output.set_size(num_piv_pix,2);

    vnl_random rand;

    //create the CDF vector
    vnl_vector<float> cdf(normalized_entropy_matrix.size()+1);
    cdf[0] = 0;
    unsigned i;
    for(ent_mat_itr = normalized_entropy_matrix.begin(), i = 1; ent_mat_itr != ent_mat_end; ent_mat_itr+=down_sample_factor,++i)
        cdf[i]=cdf[i-1]+*ent_mat_itr;

    cdf[cdf.size()-1] = 1.0f;

    //vcl_ofstream of_cdf("C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\results\\westin1\\cdf.dat",vcl_ios::out);
    //ncn_utilities::vnl_vector2dat(of_cdf,cdf);
    //of_cdf.close();

    vcl_set<unsigned> sample_set;
    //sample until number of unique samples is sufficient
    unsigned first;
    unsigned last;
    unsigned mid;
    bool bin_found;
    vcl_pair<vcl_set<unsigned>::iterator, bool> ret;
    while(sample_set.size() < num_piv_pix)
    {
        
        double u = rand.drand64();

        //search the cdf
        first = 0;
        last = cdf.size()-1;
        bin_found = false;

        //DIVIDE AND CONQUER!!!
        while(!bin_found && first<=last)
        {
            mid = (first+last)/2;

            if(cdf[mid-1] <= u && u <= cdf[mid])
                bin_found = true;
            else
                if(cdf[mid] > u)
                    last = mid - 1;
                else
                    first = mid + 1;

        }//end search while loop
        
        if(bin_found)
        {
            ret = sample_set.insert(mid);
            if(ret.second == true)
                vcl_cout << vcl_setprecision(2) << vcl_fixed 
                         << (float(sample_set.size())/float(num_piv_pix)) * 100 
                         << "% pivot pixels sampled." << vcl_endl;

        }
        else
        {
            vcl_cerr << "Error ncn_utilities::sample_pivot_pixels_rejection a bin was not found in the cdf,"
                     << "this should not be the case." << vcl_flush;
            return false;
        }


    }//end while sample_set.size() < num_pix_pix

    //now the set is full must convert the samples from linear indicies to matrix coordinates and store in output
    vcl_set<unsigned>::iterator sample_set_itr;
    vcl_set<unsigned>::iterator sample_set_end = sample_set.end();


    for(sample_set_itr = sample_set.begin(),i=0;sample_set_itr!=sample_set_end;++sample_set_itr,i++)
    {
        output(i,0) = *sample_set_itr / nrows;
        output(i,1) = *sample_set_itr % nrows;
    }

    return true;
}//end sample_pivot_pixels_rejection


bool ncn_utilities::sample_pivot_pixels_importance(vnl_matrix<float> const& entropy_matrix, unsigned const& num_piv_pix,
                    vnl_matrix<unsigned>& output, unsigned const& nparticles)
{
    vnl_random rand;
    unsigned npix = entropy_matrix.size();
    unsigned nrows = entropy_matrix.rows();
    vcl_pair<vcl_set<unsigned>::iterator,bool> ret;
    vcl_map<unsigned,float> sample_particle_map;
    vnl_vector<float> p(entropy_matrix.data_block(),entropy_matrix.size());
    output.set_size(num_piv_pix,2);

    //1. Select a random subset of unique pixels and associate with the corresponding probability.
    // Also cache the sum of the particle weights...we'll need these later for normalization.
    vcl_ofstream of_samp_part("C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\results\\westin1\\sample_particles.dat");
    float wsum = 0;
    while(sample_particle_map.size() < nparticles)
    {
       unsigned rand_indx = vnl_math_rnd(rand.drand64(0,npix));
       sample_particle_map[rand_indx] = p[rand_indx];
       wsum += p[rand_indx];
    }

    
    //2. Reweight each sample and simultaneously construct the cdf
    vcl_map<unsigned,float>::iterator sample_particle_map_itr;
    vcl_map<unsigned,float>::iterator sample_particle_map_end = sample_particle_map.end();
    vcl_vector<float> cdf(nparticles);
    vcl_vector<unsigned> particle_vector;//for ease of indexing
    float cdf_tot = 0.0f;
    unsigned i;
    for(sample_particle_map_itr = sample_particle_map.begin(),i=0;sample_particle_map_itr!=sample_particle_map_end;++sample_particle_map_itr,++i)
    {
        sample_particle_map_itr->second = sample_particle_map_itr->second/wsum;
        of_samp_part << sample_particle_map_itr->first / nrows << '\t' << sample_particle_map_itr->first % nrows << '\n';
        particle_vector.push_back(sample_particle_map_itr->first); 
        cdf_tot += sample_particle_map_itr->second;
        cdf[i] = cdf_tot;
    }
    cdf[cdf.size()-1]=1.0f;

    //3. Sample a unique set of size num_piv_pix via inverse cdf method.
    vcl_set<unsigned> sample_set; 
    unsigned first = 0;
    unsigned last = cdf.size()-1;
    unsigned mid;
    bool bin_found = false;
    
    while(sample_set.size() < num_piv_pix)
    {
        double u = rand.drand64();

        //search the cdf
        first = 0;
        last = cdf.size()-1;
        bin_found = false;

        //DIVIDE AND CONQUER!!!
        if(u < cdf[0])
        {
            bin_found = true;
            mid = 0;
        }
        else if( u > cdf[cdf.size()-1] )
        {
            bin_found = true;
            mid = cdf.size()-1;
        }
        while(!bin_found && first<=last)
        {
            mid = (first+last)/2;

            if(cdf[mid-1] <= u && u <= cdf[mid])
                bin_found = true;
            else
                if(cdf[mid] > u)
                    last = mid - 1;
                else
                    first = mid + 1;

        }//end DIVIDE AND CONQUER!!! loop
        
        if(bin_found)
        {
            ret = sample_set.insert(mid);
            if(ret.second == true)
                vcl_cout << vcl_setprecision(2) << vcl_fixed 
                         << (float(sample_set.size())/float(num_piv_pix)) * 100 
                         << "% pivot pixels sampled." << vcl_endl;

        }
        else
        {
            vcl_cerr << "Error ncn_utilities::sample_pivot_pixels_rejection a bin was not found in the cdf,"
                     << "this should not be the case." << vcl_flush;
            return false;
        }
    }//end while(sample_set.size() < num_piv_pix)

    vcl_ofstream of_samp_set("C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\results\\westin1\\sample_set.dat");
    

    //sample_set is an offset from sample_particle_map.beginning()
    //use the sampled set to index into the particle set and convert the linear index to matrix coordinates.
    vcl_set<unsigned>::iterator sample_set_itr;
    vcl_set<unsigned>::iterator sample_set_end = sample_set.end();
    for( i=0,sample_set_itr = sample_set.begin(); sample_set_itr != sample_set_end; ++sample_set_itr,++i)
    {
        //if(*sample_set_itr > (particle_vector.size()-1))
        //{
        //    vcl_cerr << "Error: ncn_utilities::sample_pivot_pixels_importance, Indexing incorrectly" << vcl_endl;
        //    return false;
        //}
        of_samp_set << *sample_set_itr << '\n';
        output(i,0) = particle_vector[*sample_set_itr]/nrows;
        output(i,1) = particle_vector[*sample_set_itr]%nrows;
        //vcl_cout << "particle_vector[*sample_set_itr] = " << particle_vector[*sample_set_itr] << vcl_endl;
        /*vcl_cout << "*sample_set_itr = " <<  *sample_set_itr << vcl_endl;*/
    }

    

    return true;
}//end sample_pivot_pixels_importance

bool ncn_utilities::sample_pivot_pixels_importance(vnl_matrix<float> const& entropy_matrix, unsigned const& num_piv_pix,
                    vcl_set<ncn_image_point>& pivot_pixel_candidates, unsigned const& nparticles)
{
    vnl_random rand;
    unsigned npix = entropy_matrix.size();
    unsigned nrows = entropy_matrix.rows();
    vcl_pair<vcl_set<ncn_image_point>::iterator,bool> ret;
    vcl_map<unsigned,float> sample_particle_map;
    vnl_vector<float> p(entropy_matrix.data_block(),entropy_matrix.size());


    //1. Select a random subset of unique pixels and associate with the corresponding probability.
    // Also cache the sum of the particle weights...we'll need these later for normalization.
    vcl_ofstream of_samp_part("C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\results\\westin1\\sample_particles.dat");
    float wsum = 0;
    while(sample_particle_map.size() < nparticles)
    {
       unsigned rand_indx = vnl_math_rnd(rand.drand64(0,npix));
       sample_particle_map[rand_indx] = p[rand_indx];
       wsum += p[rand_indx];
    }

    
    //2. Reweight each sample and simultaneously construct the cdf
    vcl_map<unsigned,float>::iterator sample_particle_map_itr;
    vcl_map<unsigned,float>::iterator sample_particle_map_end = sample_particle_map.end();
    vcl_vector<float> cdf(nparticles);
    vcl_vector<unsigned> particle_vector;//for ease of indexing
    float cdf_tot = 0.0f;
    unsigned i;
    for(sample_particle_map_itr = sample_particle_map.begin(),i=0;sample_particle_map_itr!=sample_particle_map_end;++sample_particle_map_itr,++i)
    {
        sample_particle_map_itr->second = sample_particle_map_itr->second/wsum;
        of_samp_part << sample_particle_map_itr->first / nrows << '\t' << sample_particle_map_itr->first % nrows << '\n';
        particle_vector.push_back(sample_particle_map_itr->first); 
        cdf_tot += sample_particle_map_itr->second;
        cdf[i] = cdf_tot;
    }
    cdf[cdf.size()-1]=1.0f;

    //3. Sample a unique set of size num_piv_pix via inverse cdf method.
    vcl_set<unsigned> sample_set; 
    unsigned first = 0;
    unsigned last = cdf.size()-1;
    unsigned mid;
    bool bin_found = false;
    
    while(pivot_pixel_candidates.size() < num_piv_pix)
    {
        double u = rand.drand64();

        //search the cdf
        first = 0;
        last = cdf.size()-1;
        bin_found = false;

        //DIVIDE AND CONQUER!!!
        if(u < cdf[0])
        {
            bin_found = true;
            mid = 0;
        }
        else if( u > cdf[cdf.size()-1] )
        {
            bin_found = true;
            mid = cdf.size()-1;
        }
        while(!bin_found && first<=last)
        {
            mid = (first+last)/2;

            if(cdf[mid-1] <= u && u <= cdf[mid])
                bin_found = true;
            else
                if(cdf[mid] > u)
                    last = mid - 1;
                else
                    first = mid + 1;

        }//end DIVIDE AND CONQUER!!! loop
        
       

        if(bin_found)
        {
            ncn_image_point sample_pt(particle_vector[mid]/nrows,particle_vector[mid]%nrows);
            ret = pivot_pixel_candidates.insert(sample_pt);
            if(ret.second == true)
                vcl_cout << vcl_setprecision(2) << vcl_fixed 
                         << (float(pivot_pixel_candidates.size())/float(num_piv_pix)) * 100 
                         << "% pivot pixels sampled." << vcl_endl;

        }
        else
        {
            vcl_cerr << "Error ncn_utilities::sample_pivot_pixels_rejection a bin was not found in the cdf,"
                     << "this should not be the case." << vcl_flush;
            return false;
        }
    }//end while(sample_set.size() < num_piv_pix)

    
    // vcl_ofstream of_samp_set("C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\results\\westin1\\sample_set.dat");

    ////sample_set is an offset from sample_particle_map.beginning()
    ////use the sampled set to index into the particle set and convert the linear index to matrix coordinates.
    //vcl_set<ncn_image_point>::iterator pivot_pixel_candidates_itr;
    //vcl_set<ncn_image_point>::iterator pivot_pixel_candidates_end = pivot_pixel_candidates.end();
    //for( i=0,pivot_pixel_candidates_itr = pivot_pixel_candidates.begin(); pivot_pixel_candidates_itr != pivot_pixel_candidates_end; 
    //        ++pivot_pixel_candidates_itr,++i)
    //{
    //    //if(*sample_set_itr > (particle_vector.size()-1))
    //    //{
    //    //    vcl_cerr << "Error: ncn_utilities::sample_pivot_pixels_importance, Indexing incorrectly" << vcl_endl;
    //    //    return false;
    //    //}
    //    of_samp_set << *pivot_pixel_candidates_itr << '\n';
    //    //vcl_cout << "particle_vector[*sample_set_itr] = " << particle_vector[*sample_set_itr] << vcl_endl;
    //    /*vcl_cout << "*sample_set_itr = " <<  *sample_set_itr << vcl_endl;*/
    //}

    //

    return true;
}//end sample_pivot_pixels_importance SET

unsigned ncn_utilities::find_bin(vcl_vector<float> const& cdf, float const& target)
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


}//end find_bin



bool ncn_utilities::sample_pivot_pixels_dc(vnl_matrix<float> const& entropy_matrix, unsigned const& num_piv_pix,
                                             vnl_matrix<unsigned>& output)
{
    vnl_random rand;
    output.set_size(entropy_matrix.size(),2);
    unsigned nrows = entropy_matrix.rows();
    vcl_vector<unsigned> linear_indicies;
    vcl_vector<float> p;
    vcl_vector<float> cdf;
    float u;

    vnl_matrix<float>::const_iterator ent_mat_itr = entropy_matrix.begin();
    vnl_matrix<float>::const_iterator ent_mat_end = entropy_matrix.end();
    for(unsigned i = 0; i < entropy_matrix.size(); ++ent_mat_itr, ++i)
    {
        linear_indicies.push_back(i);
        p.push_back(*ent_mat_itr);
    }

    for(unsigned i = 0; i < num_piv_pix; ++i)
    {
        vcl_cout << vcl_setprecision(2) << vcl_fixed << float(i)/float(num_piv_pix)*100.0f << "% pivot pixels sampled." << vcl_endl;
        //1. create the cdf;
        cdf.clear();
        float cdf_tot = 0.0f;
        vcl_vector<float>::iterator p_itr = p.begin();
        vcl_vector<float>::iterator p_end = p.end();
        for(;p_itr!=p_end;++p_itr)
        {
            cdf_tot += *p_itr;
            cdf.push_back(cdf_tot);
        }

        //2. draw a sample given the cdf
        u = rand.drand64();
        unsigned lin_samp = find_bin(cdf,u);
        output(i,0) = linear_indicies[lin_samp]/nrows;
        output(i,1) = linear_indicies[lin_samp]%nrows;
  

        //3. remove the sample from the distribution
        linear_indicies.erase(linear_indicies.begin() + lin_samp);
        p.erase(p.begin()+lin_samp);

        //4. renormalize
        float psum = 0.0f;
        for(p_itr=p.begin();p_itr!=p.end();++p_itr)
            psum += *p_itr;
        for(p_itr=p.begin();p_itr!=p.end();++p_itr)
            *p_itr/=psum;
    }
    return true;
}//end sample_pivot_pixels_dc

//bool ncn_utilities::get_neighborhood(unsigned x_tl, unsigned y_tl, unsigned x_br, unsigned y_br,
//                                        vcl_map<unsigned, vil_image_view<float> >& img_seq, unsigned const& num_neighbors, 
//                                        vnl_matrix<unsigned> pivot_pixel_candidates,vnl_matrix<unsigned> non_compact_neighborhood)
//{
//    unsigned n_bins_joint_histogram = 16;
//
//    unsigned nrows = img_seq[0].nj();
//    unsigned ncols = img_seq[0].ni();
//    unsigned T = img_seq.size();
//
//    unsigned ncols_cropped = x_br - x_tl;
//    unsigned nrows_cropped = y_br - y_tl;
//    vcl_cout << "nrows_cropped: " << nrows_cropped << vcl_endl; 
//    vcl_cout << "ncols_cropped: " << ncols_cropped << vcl_endl;
//    vcl_vector<unsigned> linear_index;
//    ////1.find the linear index of every point in the rio
//    //for(unsigned i = x_tl; i < x_br; ++i) //columns
//    //    for(unsigned j = y_tl; j < y_br; ++j)//rows
//    //        linear_index.push_back(i*nrows + j);
//
//    ////Sanity check write out indecies and display in matlab.
//    //vcl_ofstream of_check("C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\results\\westin1\\roi.dat",vcl_ios::out);
//    //for(unsigned i = 0; i < linear_index.size(); ++i)
//    //     of_check << linear_index[i]/nrows << '\t' << linear_index[i]%nrows << '\n';
//
//
//    bool in_pivot_set = false;
//    unsigned pivot_pixel_row;
//    vnl_vector<float> mutual_information;
//    bsta_joint_histogram<double> joint_histogram(double(255),n_bins_joint_histogram);
//    unsigned num_pivot_pixel_candidates = pivot_pixel_candidates.rows();
//    vcl_set<ncn_mutual_information> smi;
//
//
//    for(unsigned target_r = y_tl; target_r < y_br; ++target_r)//rows
//        for(unsigned target_c = x_tl; target_c < x_br; ++target_c)
//        {
//            
//            in_pivot_set = false;
//            mutual_information.clear();
//            joint_histogram.clear();
//            //1. Calculate mutual information given a target pixel and all pivot pixel candidates
//            for(unsigned i = 0; i < num_pivot_pixel_candidates; ++i)
//            {
//                //check if the target is in the pivot pixel candidate set
//                if(!(target_c == pivot_pixel_candidates(i,1)) && !(target_r == pivot_pixel_candidates(i,2)))
//                {
//                    //iterate through time series and construct joint histogram of intensity values.
//                    for(unsigned t = 0; t < T; ++t)
//                        joint_histogram.upcount(img_seq[t](target_r,target_c,0),1,img_seq[t](pivot_pixel_candidates(i,1),pivot_pixel_candidates(i,2),0),1);
//                   
//                    //calculate the temporal mutual information insert into set and sort
//                    
//
//
//                }
//           
//            } 
//        }
//
    //return true;
        
//}//end get_neighborhood

bool ncn_utilities::pointSet2dat(vcl_ostream& os, vcl_set<ncn_image_point> const& point_set)
{
    vcl_set<ncn_image_point>::const_iterator sit;
    vcl_set<ncn_image_point>::const_iterator send = point_set.end();

    for(sit=point_set.begin(); sit != send; ++sit)
        os << *sit << vcl_endl;

    return true;
}

bool ncn_utilities::pointVect2dat(vcl_ostream& os, vcl_vector<ncn_image_point> const& point_vector)
{
    vcl_vector<ncn_image_point>::const_iterator vit;
    vcl_vector<ncn_image_point>::const_iterator vend = point_vector.end();

    for(vit=point_vector.begin();vit!=vend;++vit)
        os << *vit << vcl_endl;

    return true;
}

bool ncn_utilities::pointVect2dat(vcl_ostream& os, vcl_vector<vgl_point_2d<unsigned> > const& point_vector)
{
    vcl_vector<vgl_point_2d<unsigned> >::const_iterator vit;
    vcl_vector<vgl_point_2d<unsigned> >::const_iterator vend = point_vector.end();
    for(vit=point_vector.begin(); vit!=vend; ++vit)
        os << vit->x() << '\t' << vit->y() << '\n';

    return true;
}

bool ncn_utilities::get_region(unsigned const& x_tl, unsigned const& y_tl, unsigned const& x_lr, unsigned const& y_lr, vcl_vector<ncn_image_point>& roi)
{
    for(unsigned y = y_tl; y <= y_lr; ++y)
        for(unsigned x = x_tl; x <= x_lr; ++x)
        {
            ncn_image_point point(x,y);
            roi.push_back(point);
        }
     
    return true;
}//end get_region

bool ncn_utilities::get_region(unsigned const& x_tl, unsigned const& y_tl, unsigned const& x_lr, unsigned const& y_lr, vcl_vector<vgl_point_2d<unsigned> >&roi)
{
    for(unsigned y = y_tl; y <= y_lr; ++y)
        for(unsigned x=x_tl; x<= x_lr; ++x)
        {
            vgl_point_2d<unsigned> point(x,y);
            roi.push_back(point);
        }

     return true;
}