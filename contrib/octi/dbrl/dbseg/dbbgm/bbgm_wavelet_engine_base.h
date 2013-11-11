#pragma once

template <class image_>
class dbbgm_wavelet_engine_base
{
typedef typename image_::pixel_type contained_type;
public:	
	virtual bool forwardTransform(const image_* from, image_* to,int nlevels,const int waveletno)=0;
	virtual bool backwardTransform(const image_* from, image_* to,int nlevels,const int waveletno)=0;
	virtual bool decompressLocation(unsigned int x,unsigned int y,contained_type& result, const int waveletNo,
		int level,const image_& waveletDomain)=0;
	 ~dbbgm_wavelet_engine_base(){}

};

#define DBBGM_WAVELET_ENGINE_INSTANTIATE(image_,engine_type)\
	template class dbbgm_wavelet_engine_ ## engine_type<image_>;
