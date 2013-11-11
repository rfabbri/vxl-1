#pragma once
#include <dbbgm/bbgm_wavelet_engine_base.h>

template <class image_>
class dbbgm_wavelet_engine_GE:public dbbgm_wavelet_engine_base<image_>
{
 typedef typename image_::iterator image_iterator;
 typedef typename image_::pixel_type contained_type;

public:	
	
	dbbgm_wavelet_engine_GE(){}
	~dbbgm_wavelet_engine_GE(){}
	virtual bool forwardTransform(const image_* from, image_* to,int nlevels,const int waveletno)
	{
		return this->bbgmWaveletTransform(from,to,true,nlevels,waveletno);
	}
	virtual bool backwardTransform(const image_* from, image_* to,int nlevels,const int waveletno)
	{
		return this->bbgmWaveletTransform(from,to,false,nlevels,waveletno);
	}
	bool decompressLocation(unsigned int x,unsigned int y,contained_type& result, const int waveletNo,
		int level,const image_& waveletDomain);
private:
	
	bool bbgmWaveletTransform(const image_* from, image_* to,const bool forwardp,int nlevels,const int waveletno);
    bool bbgmWaveletTransform(image_& array,const int* dims, const int ndim,const bool forwardp, int nlevels,const int waveletno);
    void bbgmCopyNdRecursive(const image_& from,const int from_size, const int* from_dims,image_& to,const int to_size, const int* to_dims,const int ndim,const bool fullp,int offsetTo,int offsetFrom);
    void bbgmWaveletTransformStep(image_& array, const int* dims, const int ndim,const bool forwardp,const float* lo_filter, const float* hi_filter,const int ncof,image_ buffer, image_ wksp);
    void bbgmWaveletTransformStep(image_& array, const int n,const bool forwardp,
                                           const float* lo_filter,
                                           const float* hi_filter,
                                           const int ncof,
                                           image_ wksp);
	void determine_coordinates(unsigned int x, unsigned int y,unsigned int** coords,unsigned int level);
	bool recursiveWaveletLookup(image_& copy_buffer,image_& out_buffer,image_& copy_buffer_1D,image_& out_buffer_1D,
		float* lo_filter,float* hi_filter,const image_& waveletDomain,const int level,unsigned int** coords,int ncof);
	void transformMinimal2DBuffer(const image_& in,image_& out,image_& in_1D,image_& out_1D,int dimension, float* lo_filter, float* hi_filter,int ncof);
	void transformMinimalLinearBuffer(const image_& buffer,image_& wksp,float* lo_filter,float* hi_filter,int ncof);

};
static bool bbgmFindWavelet(const int waveletno,float*& lo_filter, float*& hi_filter, int& ncof);