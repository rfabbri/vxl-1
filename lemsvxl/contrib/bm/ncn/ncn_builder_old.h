//this is contrib/bm/ncn/ncn_builder.h
#ifndef ncn_builder_h_
#define ncn_builder_h_
//
// \file
// \author Brandon A. Mayer
// \functions for building the non compact neighborhood
// \verbatim
// \Modifications
// June 8, 2010 Initial Version
// \endverbatim

#include<bsta/bsta_joint_histogram.h>

#include"ncn_sampler.h"

#include<vgl/vgl_point_2d.h>

#include<iostream>

#include<map>
#include<set>
#include<utility>
#include<string>

#include<vil/vil_image_view.h>

//double mutual_information(vgl_point_2d<unsigned> const& pa, vgl_point_2d<unsigned> const& pb,
//                            std::map<unsigned, vil_image_view<float> > const& image_sequence, unsigned const& nbins = 16)
//{
//    double mi;
//    //1. calculate the temporal mutual information between the two points.
//    //   Must create a histogram and bin the the join intensity values.
//    bsta_joint_histogram<double> joint_histogram(double(255),nbins);
//
//    std::map<unsigned, vil_image_view<float> >::const_iterator temp_itr;
//    std::map<unsigned, vil_image_view<float> >::const_iterator temp_end = image_sequence.begin();
//
//    for(temp_itr = image_sequence.begin(); temp_itr != temp_end; ++temp_itr)
//    {
//        
//    }
//
//    return mi;
//}

class ncn_builder
{
public:
    static bool build_neighborhood( std::map<unsigned,vil_image_view<float> > const& image_sequence, std::vector<vgl_point_2d<unsigned> > const& roi,
                                    std::set<vgl_point_2d<unsigned>,vgl_point_2d_less_than> const& pivot_pixel_candidates,
                                    std::map<std::vector<vgl_point_2d<unsigned> >::const_iterator,std::vector<vgl_point_2d<unsigned> > >& neighborhood,
                                    unsigned const& n_bins = 16, unsigned const& n_neighbors = 10);

    static bool neighborhood2dat(std::ostream& os,
                    std::map<std::vector<vgl_point_2d<unsigned> >::const_iterator,std::vector<vgl_point_2d<unsigned> > > const& non_compact_neighborhood);
   
    static bool write_neighborhood_mfile(std::ostream& os,
                    std::map<std::vector<vgl_point_2d<unsigned> >::const_iterator,std::vector<vgl_point_2d<unsigned> > > const& non_compact_neighborhood);
   
};

#endif //ncn_builder_h_