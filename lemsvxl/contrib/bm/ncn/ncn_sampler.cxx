//this is /contrib/bm/ncn/ncn_sampler.cxx

#include"ncn_sampler.h"


bool ncn_sampler::sample_pivot_pixels(vnl_matrix<float> const& entropy_matrix, unsigned const& num_pivot_pixels,
                                vcl_set<vgl_point_2d<unsigned>,vgl_point_2d_less_than >& pivot_pixel_candidates, unsigned const& nparticles)
{
    vnl_random rand;
    unsigned npix = entropy_matrix.size();
    unsigned nrows = entropy_matrix.rows();
    vcl_map<vgl_point_2d<unsigned>, double,vgl_point_2d_less_than > particle_map;
    vgl_point_2d<unsigned> point;

    //1. Select a random subset of unique pixels and associate with the corresponding probability.
    // Also cache the sum of the particle weights...we'll need these later for normalization.
    float wsum = 0;
    //vgl_point_2d_less_than pl;//order points with relationship to the origin
    while(particle_map.size() < nparticles)
    {
       unsigned rand_indx = vnl_math_rnd(rand.drand64(0,npix));
       point.set(rand_indx/nrows,rand_indx%nrows);
       particle_map[point] = entropy_matrix(point.y(), point.x());
       wsum += entropy_matrix(point.y(),point.x());
    }

    //2. Reweight each sample and simultaneously construct the cdf
    vcl_map<vgl_point_2d<unsigned>, double, vgl_point_2d_less_than >::iterator pmit;
    vcl_map<vgl_point_2d<unsigned>, double, vgl_point_2d_less_than >::iterator pmend = particle_map.end();
    vbl_array_1d<double> cdf;

    vcl_vector<vgl_point_2d<unsigned> > particle_vector;//to ease indexing
    double cdf_tot = 0.0;
    for(pmit = particle_map.begin(); pmit != pmend; ++pmit)
    {
        pmit->second = pmit->second/wsum;
        particle_vector.push_back(pmit->first);
        cdf_tot += pmit->second;
        cdf.push_back(cdf_tot);
    }
    //cdf.push_back(1.0);

    //3. Sample a unique set of size num_piv_pix via inverse cdf method.
    vcl_pair<vcl_set<vgl_point_2d<unsigned>,vgl_point_2d_less_than>::iterator,bool> ret;

    pmit = particle_map.begin();
    while(pivot_pixel_candidates.size() < num_pivot_pixels)
    {
        double u = rand.drand64();
        unsigned bin = ncn_sampler::find_bin(cdf,u);
        point.set(particle_vector[bin].x(),particle_vector[bin].y());
        ret = pivot_pixel_candidates.insert(point);
        if(ret.second == true)
            vcl_cout << vcl_setprecision(2) << vcl_fixed 
                     << (float(pivot_pixel_candidates.size())/float(num_pivot_pixels)) * 100 
                     << "% pivot pixels sampled." << vcl_endl;
        
    }//end sample loop

    return true;
}

unsigned ncn_sampler::find_bin(vbl_array_1d<double> const& cdf, double const& target)
{
    unsigned first = 0;
    unsigned last = cdf.size();
    bool bin_found = false;
    unsigned mid;

    if( target <= cdf[0] )
    {
        bin_found = true;
        mid = 0;
    }
    else if( target >= cdf[cdf.size()-1] )
    {
        bin_found = true;
        mid = cdf.size()-1;
    }
    else
    {
        //divide and conquer
        while( !bin_found && first<=last )
        {
            mid = (first+last)/2;

            if( cdf[mid-1] <= target && target <=cdf[mid] )
                bin_found = true;
            else
                if( cdf[mid] > target )
                    last = mid - 1;
                else
                    first = mid + 1;
        }//end divide and conquer while loop
    }

    if( !bin_found )
        vcl_cerr << "Error: ncn_sampler::find_bin, bin was not found." << vcl_flush;

    return mid;

}//end find_bin

bool ncn_sampler::vgl_point_2d_set_to_dat(vcl_ofstream& os, vcl_set<vgl_point_2d<unsigned>,vgl_point_2d_less_than> const& point_set)
{
    vcl_set<vgl_point_2d<unsigned>,vgl_point_2d_less_than>::const_iterator psit;
    vcl_set<vgl_point_2d<unsigned>,vgl_point_2d_less_than>::const_iterator psend = point_set.end();

    for(psit = point_set.begin(); psit != psend; ++ psit)
        os << psit->x() << '\t' << psit->y() << '\n';
   
    return true;

}//end vgl_point_2d_set_to_dat