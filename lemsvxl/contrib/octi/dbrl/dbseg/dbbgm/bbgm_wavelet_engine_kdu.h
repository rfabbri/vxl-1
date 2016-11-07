#pragma once
#include <dbbgm/bbgm_wavelet_engine_kdu_traits.h>

template <class image_>
class dbbgm_wavelet_engine_kdu:public dbbgm_wavelet_engine_base<image_>
{
typedef typename image_::iterator image_iterator;
typedef typename image_::pixel_type contained_type;
public:
	dbbgm_wavelet_engine_kdu(int argc=0,char** argv=0):kd_argc(argc),kd_argv(argv),proc(0.03f),
	o_x(0),o_y(0),initialized(false){

	for (int i=0;i<kd_argc;i++)
		args.push_back(kd_argv[i]);
	cache= new image_();
	}
	~dbbgm_wavelet_engine_kdu(){
		delete cache;
	}
	virtual bool forwardTransform(const image_* from, 
		image_* to,int nlevels,const int waveletno)
	{
		return bbgm_wavelet_engine_kdu_traits<image_>::
			bbgmWaveletTransform(from,to,
			true,nlevels,kd_argc,kd_argv);
	}
	virtual bool backwardTransform(const image_ *from, image_* to,
		int nlevels,const int waveletno)
	{
		return bbgm_wavelet_engine_kdu_traits<image_>
			::bbgmWaveletTransform(from,to,false,
		nlevels,kd_argc,kd_argv);
	}
	bool decompressLocation(unsigned int x,unsigned int y,
		contained_type& result, const int waveletNo,
		int level,const image_& waveletDomain){
		
		if (!initialized)
		{
			proc_x=64.0f/waveletDomain.ni();
			proc_y=64.0f/waveletDomain.nj();
			s_x=(int)ceil(proc_x*waveletDomain.ni());
			s_y=(int)ceil(proc_y*waveletDomain.nj());
			
		}
		
		if ( (int(x)<o_x+s_x)&&
			(int(y)<o_y+s_y)&&
            (int(y)>=o_y)&&   			
			(int(x)>=o_x)&&
			initialized)	{
			result=(*cache)(x-o_x,y-o_y);
			return true;
		}
				
		float kdu_o_x=float(x)/waveletDomain.ni();
		float kdu_o_y=float(y)/waveletDomain.nj();
				
		args.pop_back();
		if(strcmp(args.back(),"-region")!=0)
		{
			args.pop_back();
			args.push_back("-region");
		}
			
		sprintf(buf2,"{%f,%f},{%f,%f}",kdu_o_y,kdu_o_x,proc_y,proc_x);
		args.push_back(buf2);
		kdu_dims region_dims;
		bool success =bbgm_wavelet_engine_kdu_traits<image_>
			::bbgmWaveletTransform(&waveletDomain,cache,false,
		0,args.size(),&args[0],&region_dims);
		if (success)
			initialized=true;
		
		o_x=region_dims.pos.x;
		o_y=region_dims.pos.y;
		
		result=(*cache)(x-o_x,y-o_y);
		return success;
	
	}
	bool getLL(image_* LL,int res_lvl) {
		while (args.size()>kd_argc)
			args.pop_back();
		args.push_back("-reduce");
		sprintf(buf2,"%d",res_lvl);
		args.push_back(buf2);
		return bbgm_wavelet_engine_kdu_traits<image_>
			::bbgmWaveletTransform(0,LL,false,
		0,args.size(),&args[0]);
		

	}

private:
	int	kd_argc,o_x,o_y,window,s_x,s_y;
	char **kd_argv;
	bool initialized;
	char buf[8],buf2[400];
	vcl_vector<char*> args;
	image_* cache;
	float proc_x,proc_y,proc;


};