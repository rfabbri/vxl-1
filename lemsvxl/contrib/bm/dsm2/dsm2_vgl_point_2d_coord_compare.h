//this is /contrib/bm/dsm2/dsm2_vgl_point_2d_coord_compare.h
#ifndef DSM2_VGL_POINT_2D_COORD_COMPARE_H_
#define DSM2_VGL_POINT_2D_COORD_COMPARE_H_

#include<vgl/vgl_point_2d.h>

template<class ptT>
class dsm_vgl_point_2d_coord_compare
{
public:
	dsm_vgl_point_2d_coord_compare(){}
	~dsm_vgl_point_2d_coord_compare(){}

	bool operator() (vgl_point_2d<ptT> const& pa, vgl_point_2d<ptT> const& pb) const
	{
		if( pa.x() != pb.x() )
			return pa.x() < pb.x();
		else
			return pa.y() < pb.y();
	}

};

#endif //DSM2_VGL_POINT_2D_COORD_COMPARE_H_