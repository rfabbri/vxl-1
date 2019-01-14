// this is contrib/bm/ncn1.cxx

//Brandon A. Mayer
//5/21/10

#include< bsta/bsta_histogram.h >
#include< bsta/bsta_sampler.h >

#include "ncn_builder.h"
#include "ncn_sampler.h"
#include "ncn_utilities.h"

#include< iostream >
#include< string >

#include< map >
#include< set >
#include< utility >

#include< vil/vil_convert.h >
#include< vil/vil_load.h >
#include< vil/vil_save.h >

#include< vnl/vnl_matlab_write.h >
#include< vnl/vnl_matrix.h >
#include< vnl/io/vnl_io_matrix.h >

#include< vsl/vsl_binary_io.h >

#include< vul/vul_file.h >
#include< vul/vul_file_iterator.h >

int main()
{
    std::cout << "---------------------------------------------------\n"
             << "                     ncn1\n"
             << "---------------------------------------------------\n";

    
    unsigned nbins = 50;
    unsigned n_piv_pix = 1000;
    unsigned down_sample_factor = 20;
    unsigned num_neighbors = 10;
    vnl_matrix<unsigned> pivot_pixel_locations;
    vnl_matrix<float> entropy_matrix;
     
    std::string img_dir = "C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\data\\westin1";
    std::string out_dir = "C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\results\\westin1\\";
    /*std::ofstream of_ent_mat((out_dir+"entropy_matrix.dat").c_str(),std::ios::out);*/
    std::ofstream of_piv_pix((out_dir+"pivot_pixel_matrix.dat").c_str(),std::ios::out);
    /*vsl_b_ofstream bofs((out_dir +"entropy_matrix.bin").c_str());*/
    vsl_b_ifstream bifs((out_dir +"entropy_matrix.bin").c_str());
    vsl_b_ofstream bofs_piv((out_dir + "pivot_pixels.bin").c_str());

    std::map<unsigned, vil_image_view<float> > img_seq = ncn_utilities::load_image_sequence(img_dir);

    std::cout << "Reading Binary Entropy Matrix File..." << std::endl;
    vsl_b_read(bifs,entropy_matrix);
    bifs.close();

    unsigned nrows = img_seq[0].ni();
    unsigned ncols = img_seq[0].nj();
    
    //std::cout << "Sampling Pivot Pixels..." << std::endl;
    //std::set<ncn_image_point> pivot_pixel_candidates;
    //if(!ncn_utilities::sample_pivot_pixels_importance(entropy_matrix,n_piv_pix,pivot_pixel_candidates))
    //    std::cout << "Error: sample_pivot_pixels_rejection " << std::endl;

    std::cout << "Sampling Pivot Pixels..." << std::endl;
    std::set<vgl_point_2d<unsigned>, vgl_point_2d_less_than> pivot_pixel_candidates;
    if(!ncn_sampler::sample_pivot_pixels(entropy_matrix,n_piv_pix,pivot_pixel_candidates,50000))
        std::cout << "Error: sample_pivot_pixels" << std::endl;

    //write pivot pixel dat file
    std::ofstream ops((out_dir+"pivot_pixel_candidates.dat").c_str());
    //ncn_utilities::pointSet2dat(ops, pivot_pixel_candidates);
    ncn_sampler::vgl_point_2d_set_to_dat(ops,pivot_pixel_candidates);
    ops.close();


    unsigned x_tl = 269, y_tl = 396, x_br = 286, y_br = 410;
    std::vector<vgl_point_2d<unsigned> > roi;
    ncn_utilities::get_region(x_tl,y_tl,x_br,y_br,roi);
    std::ofstream of_roi((out_dir+"roi.dat").c_str());
    ncn_utilities::pointVect2dat(of_roi,roi);
    of_roi.close();

   
    std::map<std::vector<vgl_point_2d<unsigned> >::const_iterator,std::vector<vgl_point_2d<unsigned> > > non_compact_neighborhood;

    
    std::cout << "Building Neighborhood..." << std::endl;
    ncn_builder::build_neighborhood(img_seq,roi,pivot_pixel_candidates,non_compact_neighborhood,8);
    
    std::vector<vgl_point_2d<unsigned> >::const_iterator pitr = roi.begin()+200;
    std::vector<vgl_point_2d<unsigned> > neighborhood = non_compact_neighborhood[pitr];

    //std::cout << "Writing Neighborhood .dat file..." << std::endl;
    //std::ofstream of_nbrhd((out_dir+"neighborhood.dat").c_str());
    //of_nbrhd << pitr->x() << '\t' << pitr->y() << '\n';
    //for(std::vector<vgl_point_2d<unsigned> >::const_iterator nitr = neighborhood.begin();nitr!=neighborhood.end();++nitr)
    //    of_nbrhd << nitr->x() << '\t' << nitr->y() << '\n';

    std::cout << "Writing Neighborhood .dat file..." << std::endl;
    std::ofstream of_nbrhd((out_dir+"neighborhood.dat").c_str());
    std::ofstream of_nbrhd_m( (out_dir + "neighborhood.m" ).c_str() );
    //ncn_builder::neighborhood2dat(of_nbrhd,non_compact_neighborhood);
    ncn_builder::neighborhood2dat(of_nbrhd,non_compact_neighborhood);
    ncn_builder::write_neighborhood_mfile(of_nbrhd_m,non_compact_neighborhood);
    of_nbrhd.close();
    of_nbrhd_m.close();
    return 0;
}

