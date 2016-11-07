#include "bbgm_wavelet_compressor.h"
#include <vil/vil_pixel_format.h>
#include <vnl/vnl_identity_3x3.h>  
#include <vnl/vnl_double_3x3.h>
#include <gevd/gevd_float_operators.h>
#include <gevd/gevd_bufferxy.h>
#include <vcl_sstream.h>
#include <dbbgm/bbgm_loader.h>
#include <vil/vil_copy.h>
#include <vul/vul_arg.h>
#include <cmath>
#include <dbbgm/bbgm_interpolate.h>
#include <vidl/vidl_convert.h>
#include <vidl/vidl_ffmpeg_ostream.h>
#include <vidl/vidl_ffmpeg_ostream_params.h>

using namespace std;


bbgm_wavelet_compressor::bbgm_wavelet_compressor(const vidl_istream_sptr& istr)
	{

	video_stream=istr;
	this->isValid_=true;
	if (video_stream->is_valid()){
		spatial_vector=new vcl_vector<vil_image_view<float> >();
		wavelet_vector=new vcl_vector<bbgm_vil_wavelet<float>* >();
		background_key_map= new vcl_map<int, vil_image_view<float> >();
		bool success=this->formFrameVector(istr,spatial_vector);
		if (!success)
			this->isValid_=false;
		else
			initParams();
			
		}
	}


bbgm_wavelet_compressor::bbgm_wavelet_compressor(vcl_string path)
{

	this->isValid_=true;
	this->video_stream=new vidl_image_list_istream(static_cast<vcl_string>(path));
	if(!this->video_stream->is_open())
	{
		vcl_cerr<<"could not open path :"<<path<<" !";
		this->isValid_=false;
		
	}
	else 
	{
		spatial_vector=new vcl_vector<vil_image_view<float> >();
		wavelet_vector=new vcl_vector<bbgm_vil_wavelet<float>* >();
		background_key_map= new vcl_map<int, vil_image_view<float> >();
		bool success=this->formFrameVector(this->video_stream,spatial_vector);
		if (!success)
			this->isValid_=false;
		else
			initParams();

	}
	
}

bbgm_wavelet_compressor::bbgm_wavelet_compressor(int argc, char* argv[])
{
	vul_arg<char*> inputPath("-p","Input Path","C:\\");
	vul_arg<int> waveletType("-wv","wavelet Method",0);
	vul_arg<int> maxComponents("-mxc","Max Components",3);
	vul_arg<int> windowSize("-wsz","Window size",300);
	vul_arg<int> startFrame("-start","start frame",0);
	vul_arg<int> endFrame("-end","end frame",-1);

	vul_arg<float> gtresh("-gtr","match threshold",2.5f);
	vul_arg<float> minStdev("-minstd","minimum standard deviation",0.02f);
	vul_arg<float> initstd("-ivar","initial variance",0.06f);
	vul_arg_parse(argc,argv);

	vcl_vector<int> intVector;
	vcl_vector<float> floatVector;
	intVector.push_back(waveletType.value_);
	intVector.push_back(maxComponents.value_);
	intVector.push_back(windowSize.value_);
	intVector.push_back(startFrame.value_);
	intVector.push_back(endFrame.value_);

	floatVector.push_back(gtresh.value_);
	floatVector.push_back(initstd.value_);
	floatVector.push_back(minStdev.value_);
	


	this->isValid_=true;
	this->video_stream=new vidl_image_list_istream(static_cast<vcl_string>(inputPath.value_));
	if(!this->video_stream->is_open())
	{
		vcl_cerr<<"could not open path :"<<inputPath.value_<<" !";
		this->isValid_=false;
				
	}
	else 
	{
		bbgm_wavelet_compressor_params * params=new bbgm_wavelet_compressor_params(intVector,floatVector);
		spatial_vector=new vcl_vector<vil_image_view<float> >();
		wavelet_vector=new vcl_vector<bbgm_vil_wavelet<float>* >();
		background_key_map= new vcl_map<int, vil_image_view<float> >();
		bool success=this->formFrameVector(this->video_stream,spatial_vector);
		if (!success)
			this->isValid_=false;
		else
			initParams(params);
	}

}

bool bbgm_wavelet_compressor::wavelet_decomposition_full(int waveletNo,int levels)
{

	if (!this->isValid_ || spatial_vector->empty())
	{
		vcl_cerr<<"null video stream! or empty image vector "<<vcl_endl;
		return false;
	}

	if (wavelet_vector->size()!=0)
		wavelet_vector->clear();
	for (unsigned int i=0;i< this->spatial_vector->size();i++){

		vil_image_view<float> im=this->spatial_vector->at(i);
		if (&im){
			bbgm_vil_wavelet<float>*  destImg=new bbgm_vil_wavelet<float>(im,levels,waveletNo);
			if (destImg){
			wavelet_vector->push_back(destImg);
			vcl_cerr<<"processed wavelet transform of frame "<< i<<"\n";
			}else
		    vcl_cerr<<"Error in the wavelet transform at "<< i<<"\n";
		}
	}
	return true;

}

bbgm_wavelet<vil_image_view <float> > bbgm_wavelet_compressor::wavelet_decompositionAt(int level,int waveletNo,int index)
{
	if (this->isValid_){
	vil_image_view<float> src=this->spatial_vector->at(index);
	vil_image_view<float> tmpIm= new vil_image_view<float>(src.ni(),src.nj(),1,1);
	return bbgm_wavelet<vil_image_view<float> >(&src,&tmpIm,level,waveletNo); 
	}
	else{
	vcl_cerr<<"Invalid compressor! possibly due to invalid video data "<<vcl_endl;
	return bbgm_wavelet<vil_image_view<float> >();
	}
}



bool bbgm_wavelet_compressor::wavelet_decomposition_model (int level,int waveletNo)

{
	if (!model_sptr)
	{
		vcl_cerr<<"model is null, train it on the video sequence first"<<vcl_endl;
		return false;
	}

	    this->wv_model_sptr = new bbgm_image_of<obs_mix_gauss_type3>(spatial_vector->at(0).ni(), spatial_vector->at(0).nj(),obs_mix_gauss_type3());
		
		 bbgm_image_of<obs_mix_gauss_type3> *out_wv_model =
		 static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(this->wv_model_sptr.ptr());

		 bbgm_image_of<obs_mix_gauss_type3> *in_wv_model =
		 static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(model_sptr.ptr());
		 this->model_wavelet_sptr= new bbgm_wavelet<imageofgaussmix3>(in_wv_model,out_wv_model,level,waveletNo);
		 return true;
}

vil_image_view<float> bbgm_wavelet_compressor::displayParam(bool waveletModel,vcl_string attr,int comp_index,bool scale,vcl_string path)
{
	// Sanity check
	bbgm_image_sptr bgm;
	// Retrieve background image
	if (!this->isValid_)
	{
		vcl_cout<<"invalid compressed background model !"<<vcl_endl;
		return 0;
	}
	if (waveletModel && this->wv_model_sptr)
		bgm = this->wv_model_sptr;
	else
		bgm = this->model_sptr;
	
	bbgm_viewer_sptr viewer;
	if (attr=="mean") {
		viewer = new bbgm_mean_viewer();
		register_mean_viewers();
	} else if (attr == "variance"||attr == "std_dev") {
		viewer = new bbgm_variance_viewer();
		register_variance_viewers();
	} else if (attr == "weight") {
		viewer = new bbgm_weight_viewer();
		register_weight_viewers();
	} else {
		vcl_cout  << " display attribute not available\n";
		return 0;
	}
	if (!viewer->probe(bgm)){
		vcl_cout << " displayer cannot process distribution image type\n"
			<< bgm->is_a() << '\n';
		return 0;
	}
	viewer->set_active_component(comp_index);
	vil_image_view<double> d_image;
	
	if (!viewer->apply(bgm, d_image)){
		vcl_cout <<" extract view (apply) failed\n";
		return 0;

	}
	vil_image_view<float> fimg;
	vil_convert_stretch_range(d_image, fimg, 0, 1);

	//convert to a byte image for display
	vil_image_view<vxl_byte> byte_image;
	if(attr == "variance"||attr == "std_dev"){
		if(attr == "std_dev") vil_math_sqrt(d_image);
		if(scale){
			double dmin, dmax;
			vil_math_value_range(d_image, dmin, dmax);
			vcl_cout<<"max and min are : "<<dmin<<" , "<<dmax<<vcl_endl;
			vil_convert_stretch_range_limited(d_image, byte_image, dmin, dmax);
			vil_save(byte_image,path.c_str());

		}else{
			vil_save(fimg,path.c_str()); //only tiff supports floats
		}
	}else if(scale){
		double dmin, dmax;
		vil_math_value_range(d_image, dmin, dmax);
		vil_convert_stretch_range_limited(d_image, byte_image, dmin, dmax);
		vil_save(byte_image,path.c_str());

	}else
		vil_save(fimg,path.c_str());
	return fimg;
}
 
 
bool bbgm_wavelet_compressor::dumpVectorToDisk(vcl_string path,vcl_string base,vcl_string ext)
{

	vcl_vector<vil_image_view<float> >* vec=this->spatial_vector;	
	if (!this->isValid_ || vec->empty())
	{
		vcl_cerr<<"empty image vector "<<vcl_endl;
		return false;
 	}

	vidl_ffmpeg_ostream_params videoParams;
	vidl_ffmpeg_ostream myVideo(path+"\\vid.avi",videoParams);
	for (unsigned int i=0;i< vec->size();i++)
	{
		
		vil_image_view<float> tmp=vec->at(i);
		
		//string final=path+ "\\" +this->determine_prefix(i,vec->size())+"."+ext; 
		//vil_save(tmp,final.c_str());
		myVideo.write_frame(vidl_convert_to_frame(tmp));
	}
	return true;
}


bool bbgm_wavelet_compressor::saveWaveletVector(vcl_string path,vcl_string base,vcl_string ext)

{
	vcl_vector<bbgm_vil_wavelet<float>* > *vec=this->wavelet_vector;
	
	if (!this->isValid_ || vec->empty())
	{
		vcl_cerr<<"empty wavelet vector "<<vcl_endl;
		return false;
	}
	
	for (unsigned int i=0;i< vec->size();i++)
	{
		bbgm_vil_wavelet<float>* tmp=vec->at(i);
		string final=path+ "\\" +base+this->determine_prefix(i,vec->size())+"."+ext; 
		tmp->save(final.c_str());
	}
	return true;
	
}

bool bbgm_wavelet_compressor::saveBackgroundVideoSequence(int startFrame,int endFrame,float tolerance,bool useSpatialModel,
														  vcl_string path,vcl_string base,vcl_string ext,int quality)

{
	if (!this->isValid_)
	{
		vcl_cerr<<" null background video sequence. train the model then measure the backgrund probability first "<<vcl_endl;
		return false;
	}
	if (!model_sptr && useSpatialModel)
	{
		vcl_cerr<<"background model is null !"<<vcl_endl;
		return false;
	}
	if (!wv_model_sptr && !useSpatialModel)
	{
		vcl_cerr<<"wavelet background model is null !"<<vcl_endl;
		return false;
	}
if(!startFrame)
	  startFrame=params_->startFrame();
  if(!endFrame)
	  endFrame=params_->endFrame();

	int finalFrame;
		 if ((endFrame<0)||(endFrame >spatial_vector->size()))
	 finalFrame=spatial_vector->size();
	else	
		finalFrame=endFrame;
	
		 bbgm_image_sptr mdl=useSpatialModel ? model_sptr:this->modelWavelet()->getSubband(LL,quality);
	for (int i=startFrame;i<finalFrame;i++)
	{
		vil_image_view<float> result= this->measureBackground(mdl,spatial_vector->at(i),tolerance,quality,i);
		if (!result)
			return false;
	  	float sum,sum_sq;
		vil_math_sum_squares(sum,sum_sq,result,0);
		fstream file_look;
		vcl_cout<<"average background probability is "<<sum/(result.ni()*result.nj())<<vcl_endl;
		file_look<<sum/(result.ni()*result.nj())<<vcl_endl;
		vil_image_view<vxl_byte> tmp;

		vil_convert_stretch_range_limited(result,tmp,0.0f,1.0f,0,255);
		vcl_string final=path+"\\"+base+this->determine_prefix(i,spatial_vector->size())+"."+ext;
		vil_save(tmp,final.c_str());
		vcl_cerr<<"measured background on frame "<<i<<vcl_endl;
		this->pMap_=result;
	}
	return true;
	
}

bool bbgm_wavelet_compressor::saveModel(bool useSpatialModel,vcl_string path)
{
  bbgm_image_sptr mdl=useSpatialModel ? this->model_sptr:this->wv_model_sptr;
  vsl_b_ofstream ostr(path);
  if (!ostr) {
    vcl_cerr << "Failed to save background image to "
             << path << vcl_endl;
    return false;
  }

  vsl_b_ostream& bos = static_cast<vsl_b_ostream&>(ostr);

   if (!mdl) {
    vcl_cerr << "Null background model\n";
    return false;
  }

  vsl_b_write(bos, mdl);

  return true;
 }

bool bbgm_wavelet_compressor::saveWaveletModel(vcl_string path)
{
  vsl_b_ofstream ostr(path);
  if (!ostr) {
    vcl_cerr << "Failed to save background image to "
             << path << vcl_endl;
    return false;
  }
  
  vsl_b_ostream& bos = static_cast<vsl_b_ostream&>(ostr);
  vsl_b_write(bos, this->model_wavelet_sptr);

  return true;
 }

bool bbgm_wavelet_compressor::loadModel(vcl_string path,bool isSpatialModel)
{

  vsl_b_ifstream istr(path);
  if (!istr) {
    vcl_cerr << "Failed to load background image from "
             << path << vcl_endl;
    return false;
  }
  bbgm_loader::register_loaders();
  //register different distributions for image content
  //the registration will only be done once since new instances of
  //the process are cloned  - maybe later make a separate registration step
  vsl_b_istream& bis = static_cast<vsl_b_istream&>(istr);
  if (isSpatialModel)
	vsl_b_read(bis, this->model_sptr);
  else
	vsl_b_read(bis, this->wv_model_sptr);
  return true;

}

bool bbgm_wavelet_compressor::loadWaveletModel(vcl_string path)
{

  vsl_b_ifstream istr(path);
  if (!istr) {
    vcl_cerr << "Failed to load background image from "
             << path << vcl_endl;
    return false;
  }
  bbgm_loader::register_loaders();
  vsl_b_istream& bis = static_cast<vsl_b_istream&>(istr);
  this->model_wavelet_sptr=new bbgm_wavelet<imageofgaussmix3>();
  vsl_b_read(bis, this->model_wavelet_sptr);
  this->wv_model_sptr=new bbgm_image_of<obs_mix_gauss_type3>(*this->modelWavelet()->waveletDomain());
  return true;

}


bool bbgm_wavelet_compressor::formFrameVector(const vidl_istream_sptr& istr,vcl_vector<vil_image_view<float> >* vec)
{
	int end_frame = istr->num_frames();
	int start_frame=0;
	while (istr->advance() && (end_frame<0||(int)(istr->frame_number()) <= end_frame)) {
		// get frame from stream
		if ((int)(istr->frame_number()) >= start_frame) {
			vidl_frame_sptr f = istr->current_frame();
			vil_image_view_base_sptr fb = vidl_convert_wrap_in_view(*f);
			if (!fb)
			{
				vcl_cerr<<"blank frame encountered at "<<(int)(istr->frame_number())<<"\n";
				return false;
			}
			vil_image_view<float> frame = *vil_convert_cast(float(), fb);
			vil_image_view<float> solid_frame=new vil_image_view<float>(frame.ni(),frame.nj(),1,1);
			vil_copy_deep(frame,solid_frame);
			if (fb->pixel_format() == VIL_PIXEL_FORMAT_BYTE)
				vil_math_scale_values(solid_frame,1.0/255.0);
			vcl_cerr<<"read frame "<<(int)(istr->frame_number())<<"\n";
			vec->push_back(solid_frame);
		}
	}
	return true;
}

bbgm_wavelet_compressor::~bbgm_wavelet_compressor(){

	this->spatial_vector->clear();
	this->wavelet_vector->clear();
	this->background_key_map->clear();
	delete this->spatial_vector;
	delete this->wavelet_vector;
	delete this->background_key_map;
}

vcl_string bbgm_wavelet_compressor::determine_prefix(unsigned int num,int maxn)
{
	string prefix;
	string out;
	int digits_num;
	if (num==0)
		digits_num=1;
	else
		digits_num=(int)(log10(double(num)))+1;
	int digits_maxn=(int)(log10(double(maxn)))+1;
	int diff=digits_maxn-digits_num;
	for (int i=0;i<diff;i++,prefix+="0");
	
	vcl_stringstream  prefixsstream;
	prefixsstream<<prefix<<num;
	prefixsstream>>out;		
 	return out;
}


void bbgm_wavelet_compressor::initParams(bbgm_wavelet_compressor_params* params)
{
 

	if (!params)
 {

	vcl_vector<int> intVector;
	vcl_vector<float> floatVector;
	
	intVector.push_back(0);
	intVector.push_back(3);
	intVector.push_back(10);
	intVector.push_back(0);
	intVector.push_back(-1);

	floatVector.push_back(3.0);
	floatVector.push_back(0.1f);
	floatVector.push_back(0.03f);
	bbgm_wavelet_compressor_params* params= new bbgm_wavelet_compressor_params(intVector,floatVector);
	
 } 	
	this->globalMax=0;
	this->globalMin=65535.0;
	this->setParams(params);

}

vil_image_view<float> bbgm_wavelet_compressor::spatialAt(int i)
{
	
	return this->spatial_vector->at(i);

}

bbgm_vil_wavelet<float>* bbgm_wavelet_compressor::waveletAt(int i)
{
	return this->wavelet_vector->at(i);
}


bool bbgm_wavelet_compressor::trainModel(int startFrame, int endFrame)
{
	if (!this->isValid_)
	{
		vcl_cerr<<"empty spatial vector!"<<vcl_endl;
		return false;
	}

  if(!startFrame)
	int  startFrame=params_->startFrame();
  if(!endFrame)
	 int endFrame=params_->endFrame();

  // get the templated mixture model
  if (!model_sptr) {
	  model_sptr = new bbgm_image_of<obs_mix_gauss_type3>(spatial_vector->at(0).ni(), spatial_vector->at(0).nj(), obs_mix_gauss_type3());
    vcl_cout << " Initialized the bbgm image\n";
    vcl_cout.flush();

  }
    bbgm_image_of<obs_mix_gauss_type3> *model =
    static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(model_sptr.ptr());
#if USE_FULL
	vnl_matrix_fixed<float,3,3> eye3;
	eye3.set_identity();
	const vnl_matrix_fixed<float,3,3> initMat=eye3*(params_->initVar());
	gauss_type3 init_gauss(vector3_(0.0f),initMat);
#else
	gauss_type3 init_gauss(vector3_(0.0f),vector3_(params_->initVar()));
#endif
    bsta_mg_grimson_window_updater<mix_gauss_type3> updater( init_gauss,
                                                         params_->maxComponents(),
                                                          params_->gThresh(),
                                                          params_->minStdev(),
                                                          params_->windowSize());
 int finalFrame;
 if ((endFrame<0)||(endFrame >spatial_vector->size()))
	 finalFrame=spatial_vector->size();
 else	
	 finalFrame=endFrame;

  vcl_cout << " will start at frame # " << startFrame << " will end at frame # " << finalFrame << vcl_endl;

 for (int i=startFrame;i<finalFrame;i++)
 {	 
	 update(*model,spatial_vector->at(i),updater);
	 vcl_cerr<<"updated frame "<<i<<endl;
  }	
 return true;

}
vil_image_view<float> bbgm_wavelet_compressor::measureBackground(bbgm_image_sptr bgm,vil_image_view<float> image,float tolerance,int quality,int frameNo)
{
  if (!bgm) {
    vcl_cerr << "null distribution image\n";
    return 0;
  }
  unsigned np = image.nplanes();
   vil_image_view<float> result;
  if (np ==1) {
    
      bbgm_image_of<obs_mix_gauss_type1> *model =
      static_cast<bbgm_image_of<obs_mix_gauss_type1>*>(bgm.ptr());
      bsta_probability_functor<mix_gauss_type1> functor_;
      measure(*model, image, result, functor_, tolerance);
   
  }
  if (np ==3)
  {
        bbgm_image_of<obs_mix_gauss_type3> *model =
        static_cast<bbgm_image_of<obs_mix_gauss_type3>*>(bgm.ptr());
        bsta_probability_functor<obs_mix_gauss_type3> functor_;
		vcl_string path=vcl_string("C:\\Users\\octi\\Documents\\Mundy Group\\datafiles\\");
		//bbgm_interp_bicubic_ftr<imageofgaussmix3,obs_mix_gauss_type3> interp_ftr_cub;
		bbgm_interp_bilinear_ftr<imageofgaussmix3,obs_mix_gauss_type3> interp_ftr_bil;
		//bbgm_interp_nearest_ftr<imageofgaussmix3,obs_mix_gauss_type3> interp_ftr_nn;
		//bbgm_interp_lanczos_ftr<imageofgaussmix3,obs_mix_gauss_type3> interp_ftr_lcz;
		//measure_sparse_image(*model, image, result, functor_,interp_ftr_bil, tolerance,int(pow(2.0,quality)));
		
		measure_wv_lookup(*this->modelWavelet(), image, result, functor_,interp_ftr_bil, tolerance,path,vcl_string("frm")+determine_prefix(frameNo,300),0.5);
		//measure(*model, image, result, functor_, tolerance);
    }
   return result;
} 

int bbgm_wavelet_compressor::waveletImgCount()
{
	if (this->isValid_)
		return this->wavelet_vector->size();
	return 0;
}

int bbgm_wavelet_compressor::spatialImgCount()
{
	if (this->isValid_)
		return this->spatial_vector->size();
	return 0;
}

//: if the application needs to display different types than the ones already registered, then add them to this function
void bbgm_wavelet_compressor::register_mean_viewers() {
  
  bbgm_mean_viewer::register_view_maker(new bbgm_view_maker<obs_mix_gauss_type3, bsta_mean_functor<obs_mix_gauss_type3> >);
  bbgm_mean_viewer::register_view_maker(new bbgm_view_maker<obs_mix_gauss_type1, bsta_mean_functor<obs_mix_gauss_type1> >);
}

void bbgm_wavelet_compressor::register_variance_viewers() {
  
  bbgm_variance_viewer::register_view_maker(new bbgm_view_maker<obs_mix_gauss_type3, bsta_diag_covar_functor<obs_mix_gauss_type3> >);
  bbgm_variance_viewer::register_view_maker(new bbgm_view_maker<obs_mix_gauss_type1, bsta_var_functor<obs_mix_gauss_type1> >);
}

void bbgm_wavelet_compressor::register_weight_viewers() {
  
  bbgm_weight_viewer::register_view_maker(new bbgm_view_maker<obs_mix_gauss_type3, bsta_weight_functor<obs_mix_gauss_type3> >);
  bbgm_weight_viewer::register_view_maker(new bbgm_view_maker<obs_mix_gauss_type1, bsta_weight_functor<obs_mix_gauss_type1> >);
}