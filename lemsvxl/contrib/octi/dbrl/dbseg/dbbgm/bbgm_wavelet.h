#pragma once
#include <dbbgm/bbgm_wavelet_engine_base.h>
#include <vbl/vbl_ref_count.h>
#include <vcl_typeinfo.h>
#include <vsl/vsl_binary_loader.h>
#include <vil/vil_image_view.h>

enum subband {LL,HL,LH,HH};
enum engine {GE,KDU};
class dbbgm_wavelet_base : public vbl_ref_count
{
  public:
    virtual ~dbbgm_wavelet_base(){}

    //: Binary save self to stream.
    virtual void b_write(vsl_b_ostream &os) const=0;

    //: Binary load self from stream.
    virtual void b_read(vsl_b_istream &is)=0;

    virtual vcl_string is_a() const=0;

    virtual dbbgm_wavelet_base* clone() const = 0;
};


template <class image_>
class bbgm_wavelet:public dbbgm_wavelet_base
{
public:
	typedef typename image_::iterator image_iterator;
	typedef typename image_::pixel_type contained_type;
	bool isValid_;
   
	
 bbgm_wavelet(const image_* spatialIm,image_* waveletIm,
	 int level,int wavelet,engine prefEngine=KDU,int argc=0,char** argv=0);
 bbgm_wavelet();
~bbgm_wavelet(); 
  
   image_* waveletDomain(){return waveletIm_;}
   const image_* spatialDomain(){return spatialIm_;}
   image_* getSubband(subband s,int depth);
   virtual bool decompressLocation(unsigned int x,unsigned int y,contained_type& result);
   
   const int waveletNo_;
   const int level_;

   virtual void b_write(vsl_b_ostream &os) const;
   //: Binary load self from stream.
   virtual void b_read(vsl_b_istream &is);
   virtual void print_summary(vcl_ostream & os);
   
   virtual vcl_string is_a() const;
   virtual dbbgm_wavelet_base* clone() const;
   short version() const;

   
private:
 	image_* waveletIm_, *LLBuf;
	const image_* spatialIm_;
	bool LL_n(int ni,int nj,int depth,image_* tmpBuf);
	dbbgm_wavelet_engine_base<image_> *engine_;
	engine eng_type;
	
};
void vsl_add_to_binary_loader(dbbgm_wavelet_base const& b);






