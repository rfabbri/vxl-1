#pragma once
#include <vil/vil_image_view.h>
#include <dbsta/bsta_attributes.h>
#include <dbsta/bsta_mixture.h>
#include <dbbgm/bbgm_image_of.h>

template <class image_>
class bbgm_wavelet_image_traits
{
public:
	static image_ init()
	{
		return image_();
	}
	static image_* init(int ni,int nj);
	static image_& zeroVal(image_& im);

};

template<class T>
class bbgm_wavelet_image_traits<vil_image_view<T> >
{
public:
	static vil_image_view<T>* init(int ni, int nj) 
	{
		vil_image_view<T>* im=new vil_image_view<T>(ni,nj,1,1);
		return im;
	}
	static vil_image_view<T> zeroVal(vil_image_view<T>& im )
	{
	 	im.fill(0.0);
		return im;
	}
};


template <class dist_>
class bbgm_wavelet_image_traits<bbgm_image_of<dist_> >
{

public:
	static bbgm_image_of<dist_>* init(int ni,int nj)
	{
	 return new bbgm_image_of<dist_>(ni,nj,dist_());
	}

	static bbgm_image_of<dist_>& zeroVal(bbgm_image_of<dist_> & im)
	{
		bbgm_image_of<dist_> tmpIm(im.ni(),im.nj(),dist_()); 
		im=tmpIm;
		return im;
	}
		
};
