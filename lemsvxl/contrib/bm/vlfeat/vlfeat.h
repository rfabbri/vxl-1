//this is /contrib/bm/vlfeat/vlfeat.h
#ifndef VLFEAT_H_
#define VLFEAT_H_
extern "C"
{
#include"vl/dsift.h"
}

#include<vcl_cassert.h>
#include<vcl_map.h>
#include<vcl_vector.h>
#include<vcl_utility.h>


#include<vil/vil_image_view.h>

#include<vgl/vgl_point_2d.h>
#include<vgl/vgl_distance.h>

template<class ptT>
class vlfeat_vgl_point_2d_coord_compare
{
public:
	vlfeat_vgl_point_2d_coord_compare(){}
	~vlfeat_vgl_point_2d_coord_compare(){}
	

	bool operator()( vgl_point_2d<ptT> const& pa, vgl_point_2d<ptT> const& pb )
	{
		if( pa.x() != pb.x())
			return pa.x() < pb.x();
		else
			return pa.y() < pb.y();
	}

};

class vlfeat
{
public:

	static vcl_map<vgl_point_2d<float>, 
		   vcl_vector<float>,vlfeat_vgl_point_2d_coord_compare<float> > dsift( vil_image_view<float> &img,
																		unsigned const& step = 1,
																		unsigned const& size = 4,
																		bool const& flat_window = false );
	
	static vcl_vector<float> dsift(vil_image_view<float> &img, vgl_point_2d<unsigned> const& pt, 
									unsigned const& size = 4, bool const& flat_window = false );

	static vcl_vector<float> dsift(vil_image_view<float> &img, unsigned const& i, unsigned const& j, 
								   unsigned const& size = 4, bool const& flat_window = false );
private:
	vlfeat(){}
	~vlfeat(){}
	
};



#endif //VL_DSIFT_H_