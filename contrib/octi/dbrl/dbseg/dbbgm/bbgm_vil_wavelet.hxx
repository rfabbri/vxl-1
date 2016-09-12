#include <dbbgm/bbgm_vil_wavelet.h>
#include <gevd/gevd_float_operators.h>
#include <vcl_algorithm.h>
#include <gevd/gevd_bufferxy.h>
#include <vil/vil_copy.h>
#include <vil/vil_crop.h>
#include <vil/vil_save.h>
template <class T>
bbgm_vil_wavelet<T>::bbgm_vil_wavelet(vil_image_view<T> &spatialIm,int level,int waveletNo)
	:waveletNo_(waveletNo),level_(level)
{
	_waveletIm= new vil_image_view<T>(spatialIm.ni(),spatialIm.nj(),1,1);
	bool success=waveletTransform(spatialIm,_waveletIm,level,waveletNo,true);
	if(!success)
		vcl_cerr<<"error in wavelet transform "<<vcl_endl;
	

}
template <class T>

vil_image_view<T> bbgm_vil_wavelet<T>::getSubband(subband s, int depth){
	int X1,Y1;
	int ni=(int)floor(this->_waveletIm.ni()/pow(2.0,depth)+0.5);
	int nj=(int)floor(this->_waveletIm.nj()/pow(2.0,depth)+0.5);

	switch(s)
	{
		case LL:
			return this->LL_n(ni,nj,depth);
			break;		
		case LH:
			Y1=nj;
			X1=0; 
			break;
		case HL:
			Y1=0;
			X1=ni; 	
			break;
		case HH:
			Y1=nj;
			X1=ni; 	
			break;
		default:
			X1=0;
			Y1=0;
			break;
	}
	return vil_crop(this->_waveletIm,X1,ni,Y1,nj);

}

template <class T>
vil_image_view<T> bbgm_vil_wavelet<T>::LL_n(int ni,int nj,int depth)
{

	if (depth>this->level_)
		return 0;
			
		
	vcl_string path=vcl_string("C:\\Users\\Deus\\Documents\\Mundy Group\\imagedata\\small tree sequence\\out");
	vil_image_view<T> tmp=vil_crop(this->_waveletIm,0,ni,0,nj);
	vil_save(tmp,(path+"\\Input.tiff").c_str());
	vil_image_view<T> subband=vil_image_view<T>(ni,nj,1,1);
	vil_copy_deep(tmp,subband);
	vil_save(subband,(path+"\\Input2.tiff").c_str());
	
	if(depth==this->level_)
		return subband;
		
	
	vil_image_view<T> spatial_output=new vil_image_view<T>(subband.ni(),subband.nj(),1,1);
	bool success=waveletTransform(subband,spatial_output,depth,this->waveletNo_,false);
	if (success)
	return spatial_output;
		else
	return 0;
}
template <class T>

vil_image_view<T> bbgm_vil_wavelet<T>::spatialDomain()
{
	
    vil_image_view<T> spatial_output=new vil_image_view<T>(this->_waveletIm.ni(),this->_waveletIm.nj(),1,1);
	bool success=waveletTransform(this->_waveletIm,spatial_output,this->level_,this->waveletNo_,false);
	
	if (success)
		return spatial_output;
	else
		return 0;
}

template <class T>

vil_image_view<T> bbgm_vil_wavelet<T>::waveletDomain()
{
	return this->_waveletIm;
}

template <class T> 
bool bbgm_vil_wavelet<T>::save(const char* name)
{
	return vil_save(_waveletIm,name);
}


template <class T>
static bool waveletTransform(vil_image_view<T> & in,vil_image_view<T> & out,int nLevels,int waveletNo,bool forward) 
{
 	gevd_bufferxy buf(in.ni(),in.nj(),vil_pixel_format_sizeof_components(in.pixel_format())*8,in.top_left_ptr());
	gevd_bufferxy* outBuf=new gevd_bufferxy(in.ni(),in.nj(),vil_pixel_format_sizeof_components(in.pixel_format())*8);
	bool success=gevd_float_operators::WaveletTransformByBlock(buf,outBuf,forward,nLevels,waveletNo);		
	
	if (success){
			vil_image_view<T>  srcImg((T *)outBuf->GetBuffer(),outBuf->GetSizeX(),outBuf->GetSizeY(),1,1,outBuf->GetSizeX(),outBuf->GetSizeX()*outBuf->GetSizeY());
			vil_copy_deep(srcImg,out);
			delete outBuf;
			return true;
	}else{
		delete outBuf;	
		return false;
	}
}

#define BBGM_WAVELET_INSTANTIATE(T)\
template class bbgm_vil_wavelet<T>;\
template bool waveletTransform(vil_image_view<T> & in,vil_image_view<T> & out,int nLevels,int waveletNo,bool forward);