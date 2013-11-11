#pragma once
#include <dbsta/bsta_attributes.h>
#include <stdio.h>
#include <dbsta/bsta_gauss_if3.h>
#include <dbsta/bsta_gauss_f1.h>
#include <dbsta/bsta_gauss_ff3.h>
#include <dbsta/bsta_mixture.h>
#include <dbbgm/bbgm_image_of.h>
#include <dkdu/apps/kdu_compress/kdu_compress_bg.h>
#include <dkdu/coresys/common/kdu_compressed.h>
#include <dkdu/apps/kdu_expand/kdu_expand_bg.h>
#include <vil/vil_image_view.h>
#include <vcl_sstream.h>
typedef bsta_gauss_if3 bsta_gauss_i;
typedef bsta_num_obs<bsta_gauss_i> gauss_indep;
typedef bsta_mixture<gauss_indep> mix_gauss_indep3;
typedef bsta_num_obs<mix_gauss_indep3> bgm_mix;
typedef bbgm_image_of<bgm_mix> gauss_model;


typedef bsta_gauss_f1 bsta_gauss1_t;
typedef bsta_num_obs<bsta_gauss1_t> gauss_type1;


typedef bsta_gauss_ff3 bsta_gauss3_f;
typedef bsta_num_obs<bsta_gauss3_f> gauss_full;

template<class image_>
class bbgm_wavelet_engine_kdu_traits
{
public:
	static bool bbgmWaveletTransform(const image_* from,image_* to,
		bool forward,int nLevels,int argc,char** argv,kdu_dims* region_dims=0);
   
};

template<>
class bbgm_wavelet_engine_kdu_traits<gauss_model>
{

public:
	static bool bbgmWaveletTransform(const gauss_model* from,gauss_model* to,
		bool forward,int nLevels,int kd_argc,char** kd_argv,kdu_dims* region_dims=0)
	{
		
		int num_modes=bgm_mix::max_components;
		int parameter_sz=6*num_modes+3;
	    bool err;
		if (forward)
			err=kdu_compress_bg(from,to,parameter_sz,nLevels,kd_argc,kd_argv);
		else
			err=kde_expand_bg(from,to,kd_argc,kd_argv,region_dims);
		return err;
	}

};

template<class T>
class bbgm_wavelet_engine_kdu_traits<vil_image_view<T> >
{
public: 
	static bool bbgmWaveletTransform(const vil_image_view<T>* from,vil_image_view<T>* to,
		bool forward,int nLevels,int kd_argc,char** kd_argv,kdu_dims* region_dims=0)
	{
		
		int num_modes=bgm_mix::max_components;
		int parameter_sz=6*num_modes+3;
	    bool err;
		int argc=6;
		if (forward)
			err=0;
		else
			err=0;
		return err;
	}



};
