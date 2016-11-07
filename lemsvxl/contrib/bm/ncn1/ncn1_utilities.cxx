#include"ncn1_utilities.h"

void ncn1_utilities::save_pivot_pixel_candidate_matlab( vcl_string const& filename, pivot_pixel_list_type& pivot_pixel_candidates )
{

    vcl_ofstream of(filename.c_str());

    pivot_pixel_list_type::iterator pitr;
    pivot_pixel_list_type::iterator pend = pivot_pixel_candidates.end();

    for( pitr = pivot_pixel_candidates.begin(); pitr != pend; ++pitr )
        of << pitr->x() << '\t' << pitr->y() << vcl_endl;
    

    of.close();
}//end save_pivot_pixel_candidate_matlab

void ncn1_utilities::save_neighborhood_dat( vcl_string const& filename, ncn1_neighborhood_sptr neighborhood_sptr )
{
    vcl_ofstream of(filename.c_str());

    neighborhood_type nbrhd = neighborhood_sptr->neighborhood();

    neighborhood_type::const_iterator neighborhood_itr;
    neighborhood_type::const_iterator neighborhood_end = nbrhd.end();

    pivot_pixel_list_type::const_iterator piv_itr;
    pivot_pixel_list_type::const_iterator piv_end;

    unsigned count = 1;
    unsigned ntargets = neighborhood_sptr->ntargets();

    of << "targets = cell(" << ntargets << ",1);\n";
    of << "neighbors = cell(" << ntargets << ",1);\n";

    for( neighborhood_itr = nbrhd.begin(); neighborhood_itr != neighborhood_end; ++neighborhood_itr, ++count )
    {
        of << "targets{" << count << "} = [" << neighborhood_itr->first.x() << ' ' << neighborhood_itr->first.y() << "];\n";

        of << "neighbors{" << count << "} = [";

        piv_end = neighborhood_itr->second.end();
        for( piv_itr = neighborhood_itr->second.begin(); piv_itr != piv_end; ++piv_itr )
        {
            of << piv_itr->x() << ' ' << piv_itr->y() << '\n';
        }

        of << "];\n";  
    }
}

//void ncn1_utilities::save_neighborhood_dat( vcl_string const& filename, ncn1_neighborhood_sptr neighborhood_sptr)
//{
//    vcl_ofstream of(filename.c_str());
//
//    neighborhood_type nbrhd = neighborhood_sptr->neighborhood();
//
//    neighborhood_type::const_iterator neighborhood_itr;
//    neighborhood_type::const_iterator neighborhood_end = nbrhd.end();
//
//    pivot_pixel_list_type::const_iterator piv_itr;
//    pivot_pixel_list_type::const_iterator piv_end;
//
//    unsigned count = 0;
//    for( neighborhood_itr = nbrhd.begin(); neighborhood_itr != neighborhood_end; ++neighborhood_itr )
//    {
//        of << "target_" << count << " = [ " << neighborhood_itr->first.x() << ' ' << neighborhood_itr->first.y() << "];\n";
//        piv_end = neighborhood_itr->second.end();
//
//        of << "neighbors_" << count << " = [";
//        for( piv_itr = neighborhood_itr->second.begin(); piv_itr != piv_end; ++piv_itr )
//        {
//            of << piv_itr->x() << ' ' << piv_itr->y() << '\n';
//        }  
//        of << "];\n";
//        ++count;
//    }
//}

//void ncn1_utilities::save_feature_dat( vcl_string const& filename, ncn1_neighborhood_sptr neighborhood_sptr )
//{
//    vcl_ofstream of(filename.c_str());
//
//    feature_map_type feature_map = neighborhood_sptr->feature_map();
//
//    unsigned nframes = feature_map.size();
//    unsigned ntargets = feature_map[0].size();
//
//    of << "nframes = " << nframes << ";\n";
//    of << "ntargets = " << ntargets << ";\n";
//
//    of << "% targets are n x d -> the n'th row is the n'th target and the columns are x and y\n\n";
//    of << "targets = zeros(" << ntargets << ",2);\n";
//
//    feature_vector_type::const_iterator fv_itr;
//    feature_vector_type::const_iterator fv_end = feature_map[0].end();
//
//    unsigned indx = 1;
//    for( fv_itr = feature_map[0].begin(); fv_itr != fv_end; ++fv_itr )
//    {
//        of << "targets(" << indx << ",1) = " << fv_itr->first.x() << ";\n";
//        of << "targets(" << indx << ",2) = " << fv_itr->first.y() << ";\n";
//        ++indx;
//    }  
//
//    of << "\n";
//    of << "% feature cell is nframes x ntargets\n\n";
//    of << "features = cell(" << nframes << "," << ntargets << ");\n";
//
//    for( unsigned frame_cnt = 0; frame_cnt < nframes; ++frame_cnt )
//    {
//        
//        fv_itr = feature_map[frame_cnt].begin();
//        fv_end = feature_map[frame_cnt].end();
//        unsigned target_cnt = 1;
//        for( ; fv_itr != fv_end; ++fv_itr )
//        {
//            //vcl_cout << "target = " << fv_itr->first << vcl_endl;
//            vcl_vector<vxl_byte> intensity_vector = fv_itr->second;
//            vcl_vector<vxl_byte>::const_iterator intensity_itr;
//            vcl_vector<vxl_byte>::const_iterator intensity_end = intensity_vector.end();
//            
//            of << "features{" << frame_cnt+1 << "," << target_cnt << "} = [";
//            for( intensity_itr = intensity_vector.begin(); intensity_itr != intensity_end; ++intensity_itr )
//            {
//                of << unsigned(*intensity_itr);
//                //vcl_cout << '\t' << unsigned(*intensity_itr) << vcl_endl;
//                if( intensity_itr != intensity_end - 1)
//                    of << ", ";
//            }//end intensity vector iteration
//            of << "];\n";
//            ++target_cnt;
//        }//end target_iteration
//
//    }//end of frame iteration
//
//}//end save_feature_dat