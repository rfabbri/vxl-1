//this is /contrib/bm/dsm/dsm_utilities.h
#ifndef DSM_UTILITIES_H_
#define DSM_UTILITIES_H_
//:
// \file
// \date January 12, 2011
// \author Brandon A. Mayer
//
// Utility functions for dsm.
//
// \verbatim
//  Modifications
// \endverbatim
#include<bapl/bapl_dense_sift_sptr.h>

#include<vgl/vgl_distance.h>
#include<vgl/vgl_point_2d.h>

#include<vcl_cstdlib.h>
#include<vcl_map.h>
#include<vcl_string.h>
#include<vcl_utility.h>

#include<vil/vil_image_view.h>
#include<vil/vil_load.h>
#include<vil/vil_save.h>

#include<vnl/vnl_matrix.h>

#include<vsl/vsl_binary_io.h>

class dsm_vgl_point_2d_dist_compare
{
public:
	dsm_vgl_point_2d_dist_compare( vgl_point_2d<unsigned> const& p )
            :focus_(p){}

	dsm_vgl_point_2d_dist_compare()
        { vgl_point_2d<unsigned> temp(0,0); focus_ = temp; }

	bool operator()( vgl_point_2d<unsigned> const& pa, 
                         vgl_point_2d<unsigned> const& pb) const
	{
		return vgl_distance<unsigned>(pa, focus_) < 
                       vgl_distance<unsigned>(pb, focus_);
	}

private:
	vgl_point_2d<unsigned> focus_;
};

template<class ptT>
class dsm_vgl_point_2d_coord_compare
{
public:
	dsm_vgl_point_2d_coord_compare(){}
	~dsm_vgl_point_2d_coord_compare(){}

    bool operator() (vgl_point_2d<ptT> const& pa, 
                         vgl_point_2d<ptT> const& pb) const
	{
		if( pa.x() != pb.x() )
			return pa.x() < pb.x();
		else
			return pa.y() < pb.y();
	}

};

//namespace for utility functions
namespace dsm_utilities
{
	//: function to evaluate confusion matrix of binary 
        //:   change detection classification
	//: output[0][0] = true positive
	//: output[0][1] = false negative
	//: output[1][0] = false positive
	//: output[1][1] = true negative
	//: input[0] = filename of the tiff image which serves as ground 
        //:   truth with values 0 for no change 255 for change
	//: input[1] = filename of the tiff image which serves as 
        //:   prediction result with values 0 for no change 255 for change
	vnl_matrix<double> compute_confusion_matrix( 
            vcl_string const& img_gt_filename, 
            vcl_string const& img_prediction_filename);

	//: function to evaluate confusion matrix of binary change 
        //: detection classification
	//: output[0][0] = true positive
	//: output[0][1] = false negative
	//: output[1][0] = false positive
	//: output[1][1] = true negative
	//: input[0] = vil_image_view which serves as ground truth with 
        //:   values 0 for no change 255 for change
	//:   input[1] = vil_image_view which serves as prediction 
        //:   result with values 0 for no change 255 for change
	vnl_matrix<double> compute_confusion_matrix( 
            vil_image_view<vxl_byte> const& gt_view, 
       	    vil_image_view<vxl_byte> const& prediction_view);
}//end namespace dsm_utilities

#endif //DSM_UTILITIES_H_
