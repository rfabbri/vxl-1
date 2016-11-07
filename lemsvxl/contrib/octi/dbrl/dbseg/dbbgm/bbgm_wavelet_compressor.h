#pragma once
#include <vcl_iostream.h>
#include <vcl_cstddef.h>
#include <vcl_string.h>
#include <vcl_vector.h>
#include <vcl_map.h>
#include <dbbgm/bbgm_image_of.h>
#include <dbbgm/bbgm_image_of.h>
#include <dbbgm/bbgm_image_sptr.h>
#include <dbbgm/bbgm_update.h>
#include <dbbgm/bbgm_measure.h>
#include <dbbgm/bbgm_viewer.h>
#include <dbbgm/bbgm_viewer_sptr.h>
#ifdef DEVELINCLUDEPATH
#include <dbsta/bsta_attributes.h>
#include <dbsta/bsta_gauss_if3.h>
#include <dbsta/bsta_gauss_f1.h>
#include <dbsta/bsta_gauss_sf3.h>
#include <dbsta/bsta_gauss_ff3.h>
#include <dbsta/bsta_gaussian_full.h>
#include <dbsta/bsta_mixture.h>
#include <dbsta/algo/bsta_adaptive_updater.h>
#include <dbsta/bsta_basic_functors.h>
#else
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_mixture.h>
#include <bsta/algo/bsta_adaptive_updater.h>
#include <bsta/bsta_basic_functors.h>
#endif
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vil/vil_math.h>
#include <vil/vil_save.h>
#include <vil/vil_convert.h>
#include <vil/vil_image_view.h>
#include <vidl/vidl_frame.h>
#include <vidl/vidl_convert.h>
#include <vidl/vidl_image_list_istream.h>
#include <vidl/vidl_istream_sptr.h>
#include <vidl/vidl_istream.h>
#include "bbgm_wavelet_sptr.h"
#include <dbbgm/bbgm_vil_wavelet.h>


#define USE_FULL 0

#if USE_FULL
  typedef bsta_gauss_ff3 gauss_type3;
#else
  typedef bsta_gauss_if3 gauss_type3;
#endif
	
	//color images background model 
	typedef bsta_num_obs<gauss_type3> gauss3;
	typedef bsta_mixture<gauss3> mix_gauss_type3;
	typedef bsta_num_obs<mix_gauss_type3> obs_mix_gauss_type3;
	typedef gauss_type3::vector_type vector3_;
	typedef bbgm_image_of<obs_mix_gauss_type3> imageofgaussmix3;
	
	//grayscale image background model
	typedef bsta_gauss_f1 bsta_gauss1_t;
    typedef bsta_num_obs<bsta_gauss1_t> gauss_type1;
    typedef bsta_mixture<gauss_type1> mix_gauss_type1;
    typedef bsta_num_obs<mix_gauss_type1> obs_mix_gauss_type1;

class bbgm_wavelet_compressor_params;
class bbgm_wavelet_compressor{

public:


	bbgm_wavelet_compressor(const vidl_istream_sptr& istr);
	bbgm_wavelet_compressor(vcl_vector<vil_image_view<float>  > * image_vector):spatial_vector(image_vector){};
	bbgm_wavelet_compressor(vcl_string path);
	bbgm_wavelet_compressor(int argc, char* argv[]);
// TODO
//	bbgm_wavelet_compressor(char * path);

	~bbgm_wavelet_compressor();
	
	// wavelet related methods
	bool wavelet_decomposition_full(int waveletNo,int levels);
	bbgm_wavelet<vil_image_view <float> > wavelet_decompositionAt(int level,int waveletNo,int index);
	bool wavelet_decomposition_model (int level,int waveletNo);
	bool testInterp()
	{
    bbgm_image_of<obs_mix_gauss_type3> *model =
		static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(this->modelWavelet()->getSubband(LL,3));

	 bbgm_image_of<mix_gauss_type3> *model_out =
		static_cast<bbgm_image_of<mix_gauss_type3>*>(model_sptr.ptr());
		bbgm_interp_bicubic_ftr<imageofgaussmix3,mix_gauss_type3> interp_ftr;
	int factor =(int)pow(2.0f,this->modelWavelet()->level_);		
		for (unsigned int j=0;j<model_out->nj();j++)
			for (unsigned int i=0;i<model_out->ni();i++)
				 if  (i/factor>0&& j/factor>0&& i/factor<model->ni()-2&&j/factor<model->nj()-2)
					interp_ftr(((float)i)/factor,((float)j)/factor,*model,(*model_out)(i,j));
				 else		
					(*model_out)(i,j)=(*model)(i/factor,j/factor);
				
	 return true;
	}

	vil_image_view<float> testInterpImage()
	{
		vil_image_view<float> model =this->wavelet_decompositionAt(3,2,0).getSubband(LL,3);
		vil_image_view<float> outIm= new vil_image_view<float>(720,480,1,1);
		 bbgm_interp_bicubic_ftr<vil_image_view<float>,float> interp_ftr;
		 int factor =(int)pow(2.0f,3);
		
		for (unsigned int j=0;j<outIm.nj();j++)
			for (unsigned int i=0;i<outIm.ni();i++)
				 if  (i/factor>0&& j/factor>0&& i/factor<model.ni()-2&&j/factor<model.nj()-2)
					 interp_ftr(((float)i)/factor,((float)j)/factor,model,outIm(i,j));
				 else		
					outIm(i,j)=model(i/factor,j/factor);
				
	 return outIm;
	}
	
	//utility methods
	bool dumpVectorToDisk(vcl_string path,vcl_string base,vcl_string ext);
	bool formFrameVector(const vidl_istream_sptr& istr,vcl_vector<vil_image_view<float> >* vec);
	bool saveWaveletVector(vcl_string path,vcl_string base,vcl_string ext);
	bool saveModel(bool waveletModel,vcl_string path);
	bool loadModel(vcl_string path,bool isSpatialModel);

	bool saveWaveletModel(vcl_string path);
	bool loadWaveletModel(vcl_string path);
	
	void register_mean_viewers();
	void register_variance_viewers();
	void register_weight_viewers();
	
	bool clear_wavelet_vector();
	vcl_string determine_prefix(unsigned int i,int maxn);
	void initParams(bbgm_wavelet_compressor_params* params =0);
	void setParams(bbgm_wavelet_compressor_params* params){params_=params;}
	
	//export methods

	int spatialImgCount();
	int waveletImgCount();

	vil_image_view<float> spatialAt(int i);
	bbgm_vil_wavelet<float>* waveletAt(int i);
	bbgm_wavelet_sptr modelWaveletSptr(){return this->model_wavelet_sptr;}
	bbgm_wavelet<imageofgaussmix3>* modelWavelet()
			{return static_cast<bbgm_wavelet<imageofgaussmix3> *>(this->model_wavelet_sptr.ptr());}const
	bbgm_image_sptr model(){return model_sptr ? model_sptr:0;}
	bbgm_image_sptr wvModel(){return wv_model_sptr ?wv_model_sptr:0;}
	
	//background modeling methods
	bool trainModel(int startFrame=0,int endFrame=0);
	vil_image_view<float> displayParam(bool waveletModel,vcl_string attr,int comp_index,bool scale,vcl_string path);
	vil_image_view<float> measureBackground(bbgm_image_sptr bgm,vil_image_view<float> image,float tolerance,int quality,int frameNo);
	bool saveBackgroundVideoSequence(int startFrame,int endFrame,float tolerance,bool useSpatialModel,
														  vcl_string path,vcl_string base,vcl_string ext,int quality);
	vil_image_view<float> pMap(){return pMap_;}
	bool isValid_;
	
private:
	vidl_istream_sptr video_stream;
	vcl_vector<vil_image_view<float> > * spatial_vector;
	vcl_vector<bbgm_vil_wavelet<float>* > * wavelet_vector;
	vcl_map< int, vil_image_view<float> > * background_key_map;
	bbgm_wavelet_compressor_params* params_;
	float globalMax, globalMin;
	vil_image_view<float> pMap_;
	bbgm_wavelet_sptr model_wavelet_sptr;
	bbgm_image_sptr model_sptr, wv_model_sptr;
};

class bbgm_wavelet_compressor_params
{
public:
	bbgm_wavelet_compressor_params(vcl_vector<int>& ints,vcl_vector<float>& floats ):intVector(ints),floatVector(floats){ }
	~bbgm_wavelet_compressor_params(){}
	int waveletType() {return intVector.at(0);}
	int maxComponents(){return intVector.at(1);}
	int windowSize() { return intVector.at(2);}
	int startFrame() {return intVector.at(3);}
	int endFrame() {return intVector.at(4);}
	float gThresh() {return floatVector.at(0);}
	float initVar() {return floatVector.at(1);}
	float minStdev() {return floatVector.at(2);}
	

	
private:
	enum {GE_WAVELET,NCSECW_WAVELET,OCTI_WAVELET};

protected:	
	vcl_vector<int> intVector;
	vcl_vector<float> floatVector;


};