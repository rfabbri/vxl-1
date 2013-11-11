//this is contrib/bm/dblc/dbcl_vgl_point_2d_dist_compare.h
#ifndef DBCL_VGL_POINT_2D_DIST_COMPARE_H_
#define DBCL_VGL_POINT_2D_DIST_COMPARE_H_
//:
// \file
// \date January 1, 2011
// \author Brandon A. Mayer
//
// Simple class to compare 2d points based on euclidean distance. Used to order points in map and set data structures.
//
// \verbatim
//  Modifications
// \endverbatim

#include<vgl/vgl_distance.h>
#include<vgl/vgl_point_2d.h>

class dbcl_vgl_point_2d_dist_compare
{
public:
	dbcl_vgl_point_2d_dist_compare(  vgl_point_2d<unsigned> const& p ):focus_(p){}

	dbcl_vgl_point_2d_dist_compare(){ vgl_point_2d<unsigned> temp(0,0); focus_ = temp; }

	bool operator()(vgl_point_2d<unsigned> const& pa, vgl_point_2d<unsigned> const& pb) const
	{
		return vgl_distance<unsigned>(pa, focus_) < vgl_distance<unsigned>(pb,focus_);
	}

private:
	vgl_point_2d<unsigned> focus_;
};

#endif //DBCL_VGL_POINT_2D_DIST_COMPARE_H_