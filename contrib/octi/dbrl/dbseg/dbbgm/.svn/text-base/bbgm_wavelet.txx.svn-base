#include "dbbgm/bbgm_wavelet.h"
#include <dbbgm/bbgm_wavelet_engine_GE.txx>
#include <dbbgm/bbgm_wavelet_engine_kdu.h>
#include <dbbgm/bbgm_wavelet_image_traits.h>
#include <cmath>

template <class image_>
bbgm_wavelet<image_>::bbgm_wavelet(const image_* spatialIm,
	image_* waveletIm,int level,int waveletNo,engine prefEngine,int argc,char** argv):
waveletNo_(waveletNo),level_(level),LLBuf(0)
{
	if (argc==0)
		prefEngine=GE;
	switch (prefEngine)
	{
		case GE:
			this->engine_=new dbbgm_wavelet_engine_GE<image_>(); 
			eng_type=GE;
			break;
		case KDU:
			this->engine_=new dbbgm_wavelet_engine_kdu<image_>(argc,argv);
			eng_type=KDU;
			break;
		default:
			this->engine_=new dbbgm_wavelet_engine_kdu<image_>();
			eng_type=KDU;
			break;
	}
	bool success;
		if((prefEngine==KDU)&&(waveletNo==-1)){
			success =true;
		}else if ((prefEngine==KDU)&&(waveletNo==-2))
			success =this->engine_->backwardTransform(spatialIm,
			waveletIm,level,waveletNo);
		else
			success =this->engine_->forwardTransform(spatialIm,
			waveletIm,level,waveletNo);
	 if (success){
		this->waveletIm_=waveletIm;
		this->spatialIm_=spatialIm;
		this->isValid_=true;
	 }else
		 this->isValid_=false;
}
template <class image_>
bbgm_wavelet<image_>::bbgm_wavelet():waveletNo_(0),level_(0),LLBuf(0),
						engine_(new dbbgm_wavelet_engine_GE<image_>()),waveletIm_(0),spatialIm_(0){};

template <class image_>
bbgm_wavelet<image_>::~bbgm_wavelet()
{
	delete this->engine_;
	if (LLBuf)
		delete LLBuf;
}  

template <class image_>
image_* bbgm_wavelet<image_>::getSubband(subband s, int depth){
	
	int X1,Y1;
	int actual_depth=depth>=this->level_? this->level_ :depth;
	int ni=(int)floor(this->waveletIm_->ni()/pow(2.0,actual_depth));
	int nj=(int)floor(this->waveletIm_->nj()/pow(2.0,actual_depth));
	if (LLBuf)
		delete LLBuf;
	LLBuf=bbgm_wavelet_image_traits<image_>::init(ni,nj);
	
	dbbgm_wavelet_engine_kdu<image_>* engine=
		dynamic_cast<dbbgm_wavelet_engine_kdu<image_>*>(this->engine_);
	if (engine)	{
		engine->getLL(LLBuf,this->level_);
		return LLBuf;
	}
	

	switch(s)
	{
		case LL:
			this->LL_n(ni,nj,actual_depth,LLBuf);
			return LLBuf;
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
	
	for (int i=X1, ii=0; i<ni+X1,ii<ni; i++,ii++)
		for(int j=Y1,jj=0; j<nj+Y1,jj<nj; j++,jj++)
		{
		  (*LLBuf)(ii,jj)=(*this->waveletIm_)(i,j);
		}
	return LLBuf;

}

template <class image_>
bool bbgm_wavelet<image_>::LL_n(int ni,int nj,int depth,image_* tmpBuf)
{
	
	if (depth==0)				  //retrieve hi-res image.
	{
		bool success =this->engine_->backwardTransform(
			this->waveletIm_,tmpBuf,this->level_,this->waveletNo_);
		return true;
	}
	
	for (int i=0;i<ni; i++)
		for(int j=0; j<nj; j++)
			(*tmpBuf)(i,j)=(*this->waveletIm_)(i,j);

	if (depth>=this->level_)
		return false;
	
	image_* tmpWksp=new image_(); //make a copy object
	*tmpWksp=*tmpBuf;
	bool success =this->engine_->backwardTransform(tmpWksp,
		tmpBuf,this->level_-depth,this->waveletNo_);
	delete tmpWksp;
	return success;
}
template <class image_>
bool 
bbgm_wavelet<image_>::decompressLocation(unsigned int x,
unsigned int y,contained_type& result)
{
	return engine_->decompressLocation(x,y,result,
		this->waveletNo_,this->level_,*this->waveletIm_);
}
template<class image_>
vcl_string
bbgm_wavelet<image_>::is_a() const
{
  return "bbgm_wavelet<"+vcl_string(typeid(image_).name())+">";
}

template<class image_>
dbbgm_wavelet_base*
bbgm_wavelet<image_>::clone() const
{
  return new bbgm_wavelet<image_>(*this);
}


//: Return IO version number;
template<class image_>
short
bbgm_wavelet<image_>::version() const
{
  return 1;
}


//: Binary save self to stream.
template<class image_>
void
bbgm_wavelet<image_>::b_write(vsl_b_ostream &os) const
{
   vsl_b_write(os, version());
   vsl_b_write(os,static_cast<int>(this->waveletNo_));
   vsl_b_write(os,static_cast<int>(this->level_));
   image_ p=*this->waveletIm_;
   int array_rows = p.ni();
   int array_cols = p.nj();
   vsl_b_write(os, array_rows);
   vsl_b_write(os, array_cols);
   for (int i=0; i<array_rows; i++)
  {
    for (int j=0; j<array_cols; j++)
      vsl_b_write(os, p(i,j));
  }
   
}
 
//: Binary load self from stream.
template<class image_>
void
bbgm_wavelet<image_>::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
    case 1:
		vsl_b_read(is,static_cast<int>(this->waveletNo_));
        vsl_b_read(is,static_cast<int>(this->level_));
		int array_rows;
		int array_cols;
		vsl_b_read(is, array_rows);
		vsl_b_read(is, array_cols);
		this->engine_=new dbbgm_wavelet_engine_GE<image_>();
		{
		this->waveletIm_=bbgm_wavelet_image_traits<image_>
			::init(array_rows,array_cols); 
		for (int i=0; i<array_rows; i++)
			{
			for (int j=0; j<array_cols; j++)
				vsl_b_read(is, (*this->waveletIm_)(i,j));
			}
		}
		 break;
     default:
      vcl_cerr << "bbgm_image: unknown I/O version " << ver << '\n';
  }
}
template<class image_>
void
bbgm_wavelet<image_>::print_summary(vcl_ostream & os)
{
   image_ p=*this->waveletIm_;
   os << "Rows: " << p.ni() << vcl_endl
     << "Columns: " << p.nj() << vcl_endl;
  for (unsigned int i =0; i<p.ni() && i<5; i++)
  {
    for (unsigned int j=0; j<p.nj() && j<5; j++)
    {
      os << ' ';
      vsl_print_summary(os,p(i,j));
    }
    if (p.ni() > 5)
      os << "...";
    os << vcl_endl;
  }
  if (p.nj() > 5)
    os << " ...\n";
}

#define DBBGM_WAVELET_INSTANTIATE(image_)\
	DBBGM_WAVELET_ENGINE_INSTANTIATE(image_,GE);\
	template class bbgm_wavelet<image_>; 
	
	


