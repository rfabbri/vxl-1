#pragma once
#include <vil/vil_image_view.h>

template <class T>
class bbgm_vil_wavelet
{
enum subband {LL,HL,LH,HH};
public:

	bbgm_vil_wavelet(vil_image_view<T>& waveletIm,int level,int waveletNo);
	
	vil_image_view<T> LL_n(int ni,int nj,int depth);
	vil_image_view<T> getSubband(subband s,int depth);
	vil_image_view<T> spatialDomain();
	vil_image_view<T> waveletDomain();
	bool save(const char* name);
	~bbgm_vil_wavelet(){}
	
	const int waveletNo_ ;
	const int level_;
		

private:
	vil_image_view<T> _waveletIm;
	

};