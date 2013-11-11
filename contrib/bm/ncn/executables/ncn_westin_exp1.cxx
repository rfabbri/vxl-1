// this is /contrib/bm/ncn/exe/ncn_westin_exp1.cxx
#include "../ncn_factory.h"
#include "../ncn_visualization.h"



int main()
{
	vcl_string img_dir = "C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\data\\westin1";
	vcl_string out_dir = "C:\\Users\\bm\\Documents\\vxl_src\\contrib\\brl\\lemsvxlsrc\\contrib\\bm\\results\\exp1\\";

	vcl_vector< vgl_point_2d<unsigned> > target_pixel_list;
	vgl_point_2d<unsigned> pt1(271,346), pt2(316,604), pt3(235,655), pt4(233,589), pt5(597,192);
	target_pixel_list.push_back(pt1);
	target_pixel_list.push_back(pt2);
	target_pixel_list.push_back(pt3);
	target_pixel_list.push_back(pt4);
	target_pixel_list.push_back(pt5);

	ncn_factory factory((img_dir+"\\*.jpg"),target_pixel_list,10);
	ncn_neighborhood_sptr neighborhood_sptr;
	neighborhood_sptr = factory.neighborhood_sptr();

	target_pixel_type  target_pixels;
	target_pixels = neighborhood_sptr->target_pixels();
	pivot_pixel_candidate_type pivot_pixel_candidates = neighborhood_sptr->pivot_pixel_candidates();
	neighborhood_type neighborhood = neighborhood_sptr->neighborhood();

	vcl_ofstream of((out_dir+"neighborhood.dat").c_str());
	ncn_visualization::neighborhood2dat(target_pixels,pivot_pixel_candidates,neighborhood,of);

	

	//write out the neighborhood
	
}