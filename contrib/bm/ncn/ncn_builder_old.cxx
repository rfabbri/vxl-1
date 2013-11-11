//this is contrib/bm/ncn/ncn_builder.cxx

#include"ncn_builder.h"

bool ncn_builder::build_neighborhood( vcl_map<unsigned,vil_image_view<float> > const& image_sequence, vcl_vector<vgl_point_2d<unsigned> > const& roi,
                                    vcl_set<vgl_point_2d<unsigned>,vgl_point_2d_less_than> const& pivot_pixel_candidates,
                                    vcl_map<vcl_vector<vgl_point_2d<unsigned> >::const_iterator,vcl_vector<vgl_point_2d<unsigned> > >& neighborhood,
                                    unsigned const& n_bins, unsigned const& n_neighbors)
{
    unsigned nbins = 16;
    vcl_map<double,vgl_point_2d<unsigned> > mi_point_map;
    bsta_joint_histogram<double> joint_histogram(255,nbins);

    
    vcl_vector<vgl_point_2d<unsigned>>::const_iterator roi_itr;
    vcl_vector<vgl_point_2d<unsigned>>::const_iterator roi_end = roi.end();

    vcl_set<vgl_point_2d<unsigned>, vgl_point_2d_less_than>::const_iterator pivot_pixel_candidates_itr;
    vcl_set<vgl_point_2d<unsigned>, vgl_point_2d_less_than>::const_iterator pivot_pixel_candidates_end = pivot_pixel_candidates.end();

    vcl_map<unsigned, vil_image_view<float> >::const_iterator image_sequence_itr;
    vcl_map<unsigned, vil_image_view<float> >::const_iterator image_sequence_end = image_sequence.end();
    //form neighborhoods for every point in the roi
    unsigned counts = 0;
    unsigned n_targets = roi.size();
    for(roi_itr=roi.begin();roi_itr!=roi_end;++roi_itr)
    {

        //calculate the mutual information between the target pixel location and all pivot pixel candidates 
        //if the target pixel and the pivot pixel are different

        mi_point_map.clear();
        unsigned check_count = 0;
        unsigned check_count2 = 0;
        for(pivot_pixel_candidates_itr=pivot_pixel_candidates.begin();pivot_pixel_candidates_itr!=pivot_pixel_candidates_end;++pivot_pixel_candidates_itr)
        {
            joint_histogram.clear();
            
            if(*roi_itr != *pivot_pixel_candidates_itr)
            {
                //parse the temporal information at the target pixel and pivot pixel to calculate the mutual information.
                for(image_sequence_itr = image_sequence.begin(); image_sequence_itr!=image_sequence_end;++image_sequence_itr)
                {
                    double target_intensity = image_sequence_itr->second(roi_itr->x(),roi_itr->y());
                    double pivot_intensity = image_sequence_itr->second(pivot_pixel_candidates_itr->x(),pivot_pixel_candidates_itr->y());
                    joint_histogram.upcount(target_intensity,1,pivot_intensity,1);
                }
                //joint_histogram.print(vcl_cout);
                double mi = joint_histogram.mutual_information();
                //vcl_cout << "mi = " << mi << vcl_endl;
                mi_point_map[joint_histogram.mutual_information()] = *pivot_pixel_candidates_itr;
                ++check_count2;
            }
            ++check_count;
        }
        //create the neighborhood vector
		unsigned i = 0;
		vcl_map<double,vgl_point_2d<unsigned> >::const_iterator mi_point_map_itr = mi_point_map.end();
		--mi_point_map_itr;
		vcl_vector<vgl_point_2d<unsigned> > nbrhd;
		for(;i<n_neighbors;--mi_point_map_itr,++i)
			nbrhd.push_back(mi_point_map_itr->second);

		//associate the point and the neighborhood
		vcl_pair<vcl_vector<vgl_point_2d<unsigned> >::const_iterator,vcl_vector<vgl_point_2d<unsigned> > > pr(roi_itr,nbrhd);
		neighborhood.insert(pr);
		vcl_cout << counts << " out of " << n_targets << " neighborhoods built." << vcl_endl;
		++counts;
	}
    
    return true;
}//end build_neighborhood

bool ncn_builder::neighborhood2dat(vcl_ostream& os,
                    vcl_map<vcl_vector<vgl_point_2d<unsigned> >::const_iterator,vcl_vector<vgl_point_2d<unsigned> > > const& non_compact_neighborhood)
{
    vcl_map<vcl_vector<vgl_point_2d<unsigned> >::const_iterator,vcl_vector<vgl_point_2d<unsigned> > >::const_iterator map_itr;
    vcl_map<vcl_vector<vgl_point_2d<unsigned> >::const_iterator,vcl_vector<vgl_point_2d<unsigned> > >::const_iterator map_end = non_compact_neighborhood.end();

    vcl_vector<vgl_point_2d<unsigned> >::const_iterator nbrhd_itr;
    vcl_vector<vgl_point_2d<unsigned> >::const_iterator nbrhd_end;

    for(map_itr = non_compact_neighborhood.begin(); map_itr != map_end; ++map_itr)
    {
        os << "target pixel\n"
            << map_itr->first->x() << '\t' << map_itr->first->y() << '\n';
        os << "neighborhood\n";
        nbrhd_end = map_itr->second.end();
        for(nbrhd_itr = map_itr->second.begin(); nbrhd_itr != nbrhd_end; ++nbrhd_itr)
        {
            os << nbrhd_itr->x() << '\t' << nbrhd_itr->y() << '\n';
        }

        os << '\n';
    }

    return true;

}//end neighborhood2dat

bool ncn_builder::write_neighborhood_mfile(vcl_ostream& os,
                    vcl_map<vcl_vector<vgl_point_2d<unsigned> >::const_iterator,vcl_vector<vgl_point_2d<unsigned> > > const& non_compact_neighborhood)
{

    vcl_map<vcl_vector<vgl_point_2d<unsigned> >::const_iterator,vcl_vector<vgl_point_2d<unsigned> > >::const_iterator map_itr;
    vcl_map<vcl_vector<vgl_point_2d<unsigned> >::const_iterator,vcl_vector<vgl_point_2d<unsigned> > >::const_iterator map_end = non_compact_neighborhood.end();

    vcl_vector<vgl_point_2d<unsigned> >::const_iterator nbrhd_itr;
    vcl_vector<vgl_point_2d<unsigned> >::const_iterator nbrhd_end;

    unsigned n_neighborhoods = non_compact_neighborhood.size();
    
    os << "target_pixels = zeros(" << n_neighborhoods << ",2);\n";
    os << "neighborhoods = cell("  << n_neighborhoods << ",1);\n";
    
    unsigned matlab_indx = 1;
    for(map_itr = non_compact_neighborhood.begin(); map_itr != map_end; ++map_itr)
    {
        unsigned tx = map_itr->first->x();
        unsigned ty = map_itr->first->y();
        os << "target_pixel(i) = [" << map_itr->first->x() << '\t' << map_itr->first->y() << '];' << '\n';
        os << "neighborhoods{" << matlab_indx << "} = [";
        nbrhd_end = map_itr->second.end();
        for(nbrhd_itr = map_itr->second.begin(); nbrhd_itr != nbrhd_end; ++nbrhd_itr)
        {
            unsigned x = nbrhd_itr->x();
            unsigned y = nbrhd_itr->y();
            os << nbrhd_itr->x() << '\t' << nbrhd_itr->y() << ';\n';
        }
        os << "];\n";
        os << '\n';
        ++matlab_indx;
    }

    return true;

}//end neighborhood_mfile