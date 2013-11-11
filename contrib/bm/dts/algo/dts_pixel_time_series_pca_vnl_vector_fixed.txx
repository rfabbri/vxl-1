//this is /contrib/bm/dts/algo/dts_pixel_time_series_pca_vnl_vector_fixed.txx
#ifndef DTS_PIXEL_TIME_SERIES_PCA_VNL_VECTOR_FIXED_TXX_
#define DTS_PIXEL_TIME_SERIES_PCA_VNL_VECTOR_FIXED_TXX_
#include<dts/algo/dts_pixel_time_series_pca_vnl_vector_fixed.h>

#include<dts/algo/dts_time_series_pca_vnl_vector_fixed.h>

#include<vcl_iterator.h>//for vcl_distance

template< class pixelT, class timeT, class mathT, 
    unsigned dim, unsigned ndims2keep>
void dts_pixel_time_series_pca_vnl_vector_fixed<pixelT,timeT,mathT,dim,ndims2keep>::
	pca( dts_pixel_time_series_base_sptr& pts_sptr )
{
	src_pixel_time_series_type* pts_ptr =
		dynamic_cast<src_pixel_time_series_type*>(pts_sptr.as_pointer());

	if(!pts_ptr)
	{
		vcl_cerr << "----WARNING---- "
                 << "dts_pixel_time_series_pca_vnl_vector_fixed: "
				 << "Could not cast from base to child." << vcl_endl
                 << "\tFILE: " << __FILE__ << '\n'
                 << "\tLINE: " << __LINE__ << '\n'
                 << vcl_flush;
		return;
	}

	pts_sptr = pca_new_sptr( *pts_ptr );

	return;
}//end dts_pixel_time_series_pca_vnl_vector_fixed<pixelT,timeT,mathT,dim,ndims2keep>::pca

template< class pixelT, class timeT, class mathT, 
    unsigned dim, unsigned ndims2keep>
dts_pixel_time_series_base_sptr
	dts_pixel_time_series_pca_vnl_vector_fixed<pixelT,timeT,mathT,dim,ndims2keep>::
		pca_new_sptr( dts_pixel_time_series<pixelT,timeT,mathT,dim> const& pts )
{
	//the pointer to be returned
	dest_pixel_time_series_type* dest_pixel_time_series_ptr =
		new dest_pixel_time_series_type;

	src_pixel_time_series_map_type::const_iterator
		p_itr, p_end = pts.pixel_time_series_map.end();

	//loop over pixels
	for( p_itr = pts.pixel_time_series_map.begin();
			p_itr != p_end; ++p_itr )
	{
#ifdef _DEBUG
		vcl_cout << "Executing PCA on pixel: " << p_itr->first << vcl_endl
				 << "\t" << pts.pixel_time_series_map.size() - vcl_distance(p_itr,p_end) 
			     << " of " << pts.pixel_time_series_map.size() << vcl_endl;
#endif //_DEBUG
		dts_time_series<timeT,mathT,dim>* ts_ptr =
			dynamic_cast<dts_time_series<timeT,mathT,dim>*>(p_itr->second.as_pointer());

		if(!ts_ptr)
		{
			vcl_cerr << "----ERROR---- "
                     << "dts_pixel_time_series_pca_vnl_vector_fixed::pca_new_sptr"
                     << " failed dynamic_cast.\n"
                     << "\tFILE: " << __FILE__ << '\n'
                     << "\tLINE: " << __LINE__ << '\n'
                     << vcl_flush;
			exit(1);
		}

		//execute pca on each time series
		dts_time_series_base_sptr ts_base_sptr = 
			dts_time_series_pca_vnl_vector_fixed<timeT,mathT,dim,ndims2keep>
				::pca_new_sptr(*ts_ptr);

		//insert each time series sptr into destination pixel map
		dest_pixel_time_series_ptr->pixel_time_series_map[p_itr->first]=
				ts_base_sptr;
		
	}//end pixel iteration
	
	//make a smart pointer to new dts_pixel_time_series on the heap
	dts_pixel_time_series_base_sptr dts_sptr(dest_pixel_time_series_ptr);

	//return the smart pointer
	return dts_sptr;
}//end dts_pixel_time_series_pca_vnl_vector_fixed<pixelT,timeT,mathT,dim,ndims2keep>::pca_new_sptr

#define PIXEL_TIME_SERIES_PCA_FIXED_INSTANTIATE(pixelT,timeT,mathT,dim,ndims2keep)\
template class dts_pixel_time_series_pca_vnl_vector_fixed<pixelT,timeT,mathT,dim,ndims2keep>

#endif //DTS_PIXEL_TIME_SERIES_PCA_VNL_VECTOR_FIXED_TXX_