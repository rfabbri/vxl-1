// This is brl/bseg/bbgm/bbgm_measure.h
#ifndef bbgm_measure_h_
#define bbgm_measure_h_
//:
// \file
// \brief Measurement wrappers for distribution images and probablility images
// \author J.L. Mundy
// \date February 10, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_cassert.h>
#include <vil/vil_image_view.h>
#include <vbl/vbl_array_2d.h>
#include "bbgm_image_of.h"
#include "bbgm_planes_to_sample.h"
#include "bbgm_interpolate.h"
#include <dbbgm/bbgm_wavelet.h>
#include <vcl_fstream.h>
#include <vcl_sstream.h>
#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

//: measure a property (typically probability) requiring a tolerance, delta.
// For probability, delta defines a square n-dimensional box over which
// the probablity density is integrated. For example, the measure_functor_
// is the bsta_probability_functor.
template <class dist_, class measure_functor_>
void measure(bbgm_image_of<dist_>& dimg,
             const vil_image_view<typename dist_::math_type>& image,
             vil_image_view<typename dist_::math_type>& result,
             const measure_functor_& prop,
             typename dist_::math_type delta)
{
  typedef typename dist_::vector_type vector_;
  typedef typename dist_::math_type T;

  const unsigned ni = dimg.ni();
  const unsigned nj = dimg.nj();
  const unsigned d_np = dist_::dimension;

  assert(image.ni() == ni);
  assert(image.nj() == nj);
  assert(image.nplanes() == d_np);

  result.set_size(ni,nj,1);

  const vcl_ptrdiff_t pstep = image.planestep();

  vector_ del(delta);
  typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
  for ( unsigned j=0; j<nj; ++j)
    for ( unsigned i=0; i<ni; ++i, ++itr) {
      vector_ sample;
      const T* iptr = &image(i,j);
      bbgm_planes_to_sample<T, vector_, d_np>::apply(iptr, sample, pstep);
      T temp_val;
      vector_ r_min = sample-del, r_max = sample+del;
      prop(*itr, r_min, r_max, temp_val);
      result(i,j) = temp_val;
    }
}


template <class dist_, class measure_functor_,class interp_functor_>
void measure_sparse_image(bbgm_image_of<dist_>& dimg,
             const vil_image_view<typename dist_::math_type>& image,
             vil_image_view<typename dist_::math_type>& result,
             const measure_functor_& prop, const interp_functor_& interp_dist,
             typename dist_::math_type delta,int downFactor)
{
  typedef typename dist_::vector_type vector_;
  typedef typename dist_::math_type T;

  const unsigned ni = dimg.ni();
  const unsigned nj = dimg.nj();
  const unsigned d_np = dist_::dimension;

  assert(image.nplanes() == d_np);

  result.set_size(ni*downFactor,nj*downFactor,1);

  const vcl_ptrdiff_t pstep = image.planestep();

  vector_ del(delta);
  typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
  for ( unsigned j=0; j<result.nj(); ++j) {
    for ( unsigned i=0; i<result.ni(); ++i) {
      vector_ sample;
      const T* iptr = &image(i,j);
      bbgm_planes_to_sample<T, vector_, d_np>::apply(iptr, sample, pstep);
      T temp_val;
      vector_ r_min = sample-del, r_max = sample+del;
      dist_ out_dist;
	  dist_ tmp_dist;
	  if ((downFactor==1)||(!interp_dist(((float)i)/downFactor,((float)j)/downFactor,dimg,out_dist)))
		  out_dist=dimg(i/downFactor,j/downFactor);
	   prop(out_dist, r_min, r_max, temp_val);
      result(i,j) = temp_val;
	}
	
  }
}

template <class dist_, class measure_functor_,class interp_functor_>
void measure_wv_lookup(bbgm_wavelet<bbgm_image_of<dist_> >& wavelet,
             const vil_image_view<typename dist_::math_type>& image,
             vil_image_view<typename dist_::math_type>& result,
             const measure_functor_& prop, const interp_functor_& interp_dist,
             typename dist_::math_type delta,vcl_string data_path ,
			 vcl_string frame_suffix,float threshold)
			 
{
  typedef typename dist_::vector_type vector_;
  typedef typename dist_::math_type T;
  bbgm_image_of<dist_>* dimg= wavelet.getSubband(LL,wavelet.level_);
  int downFactor=int(pow(2.0,wavelet.level_));
  const unsigned ni = image.ni();
  const unsigned nj = image.nj();
  const unsigned d_np = dist_::dimension;
  unsigned int numLookup=0;
  assert(image.nplanes() == d_np);

  result.set_size(ni,nj,1);

  const vcl_ptrdiff_t pstep = image.planestep();

  vector_ del(delta);
  typename bbgm_image_of<dist_>::iterator itr = dimg->begin();
  vcl_fstream file_op,file_look;
  vcl_stringstream pointfilename,statsfilename;
  pointfilename<<data_path<<"points_"<<frame_suffix<<".txt";
  statsfilename<<data_path<<"stats_"<<frame_suffix<<".txt";
  file_op.open(pointfilename.str(),vcl_fstream::out);
  file_look.open(statsfilename.str(),vcl_fstream::app);
  unsigned b_w=64;
  unsigned b_h=64;
  unsigned n_w=(unsigned)floor(double(result.ni())/b_w)+1;
  unsigned n_h=(unsigned)floor(double(result.nj())/b_h)+1;
  unsigned start_x=0;
  unsigned start_y=0;
  for (unsigned r=0;r<n_h;r++){
	  for (unsigned k=0;k<n_w;k++ ){
		  unsigned end_y=start_y+b_h>result.nj()?result.nj():start_y+b_h;
		  unsigned end_x=start_x+b_w>result.ni()?result.ni():start_x+b_w;
		  for ( unsigned j=start_y; j<end_y; ++j) {
			  for ( unsigned i=start_x; i<end_x; ++i) {

				  vector_ sample;
				  const T* iptr = &image(i,j);
				  bbgm_planes_to_sample<T, vector_, d_np>::apply(iptr, sample, pstep);
				  T temp_val;
				  T temp_val2;
				  vector_ r_min = sample-del, r_max = sample+del;
				  dist_ out_dist;



				  //if ((downFactor==1)||(!interp_dist(((float)i)/downFactor,((float)j)/downFactor,dimg,out_dist)))
				  out_dist=(*dimg)(vcl_min(i/downFactor,dimg->ni()-1),vcl_min(j/downFactor,dimg->nj()-1));
				  prop(out_dist, r_min, r_max, temp_val);


				  // if (temp_val>=DBL_MAX)
				  //  vcl_cout<<" error encountered at "<<i<<" "<<j<<vcl_endl;

				  if ((temp_val<threshold)||(temp_val>=DBL_MAX)||(temp_val!=temp_val))  //prevent underflow
				  {   dist_ tmp_dist;
				  bool success =wavelet.decompressLocation(i,j,tmp_dist);
				  if (success)
				  {
					  numLookup++;
					  prop(tmp_dist, r_min, r_max, temp_val2);
					  temp_val=temp_val2;
					  file_op<<j<<" "<<i<<"\n";
				  }
				  }
				  result(i,j) = temp_val;
			  }
			  
		 
		  }
		  start_x+=b_w;
		  
	  }
	  start_x=0;
	  start_y+=b_h;
	//  vcl_cout<<"finished row of blocks "<<r<<vcl_endl;
  }

   file_op.close();
   vcl_cout<<"Number of Wavelet Lookups is : "<<float(numLookup)/(result.ni()*result.nj())<<vcl_endl;
   file_look<<wavelet.level_<<" "<<float(numLookup)/(result.ni()*result.nj())<<" "<<threshold<<vcl_endl;
   file_look.close();
#ifdef _CRTDBG_MAP_ALLOC
  _CrtDumpMemoryLeaks();
#endif
}


//: measure the probability of background given a fixed foreground probability (pf) at each pixel.
// Bayes rule gives
//
//               p(I|b)P(b)              p(I|b)(1-pf)          p(I|b)(1-pf)
//  P(b|I) =  ---------------------- = -------------------- =  -----------------
//          p(I|b)P(b) + p(I|f)P(f)  p(I|b)(1-pf)+ p(I|f)pf  p(I|b)(1-pf)+ pf
//
// the foreground probability is assumed to be uniform on the interval (0,1)
//
template <class dist_, class measure_functor_>
void measure_bkground(bbgm_image_of<dist_>& dimg,
                      const vil_image_view<typename dist_::math_type>& image,
                      vil_image_view<typename dist_::math_type>& result,
                      const measure_functor_& prop,
                      typename dist_::math_type pf)
{
  typedef typename dist_::vector_type vector_;
  typedef typename dist_::math_type T;

  const unsigned ni = dimg.ni();
  const unsigned nj = dimg.nj();
  const unsigned d_np = dist_::dimension;

  assert(image.ni() == ni);
  assert(image.nj() == nj);
  assert(image.nplanes() == d_np);

  result.set_size(ni,nj,1);

  const vcl_ptrdiff_t pstep = image.planestep();

  typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
  for ( unsigned j=0; j<nj; ++j)
    for ( unsigned i=0; i<ni; ++i, ++itr) {
      vector_ sample;
      const T* iptr = &image(i,j);
      bbgm_planes_to_sample<T, vector_, d_np>::apply(iptr, sample, pstep);
      T temp_val;
      prop(*itr, sample, temp_val);
      temp_val = temp_val*(T(1)-pf);
      temp_val = (temp_val)/(temp_val + pf);
      result(i,j) = temp_val;
    }
}

//: measure a property (typically probability) requiring a tolerance, delta.
// For probability, delta defines a square n-dimensional box over which
// the probablity density is integrated. For example, the measure_functor_
// is the bsta_probability_functor. In this method the tolerance is
// augmented by an additional value that varies with position in the image
template <class dist_, class measure_functor_>
void measure(bbgm_image_of<dist_>& dimg,
             const vil_image_view<typename dist_::math_type>& image,
             vil_image_view<typename dist_::math_type>& var,
             vil_image_view<typename dist_::math_type>& result,
             const measure_functor_& prop,
             typename dist_::math_type delta)
{
  typedef typename dist_::vector_type vector_;
  typedef typename dist_::math_type T;

  const unsigned ni = dimg.ni();
  const unsigned nj = dimg.nj();
  const unsigned d_np = dist_::dimension;

  assert(image.ni() == ni);
  assert(image.nj() == nj);
  assert(image.nplanes() == d_np);

  result.set_size(ni,nj,1);

  const vcl_ptrdiff_t pstep = image.planestep();

  vector_ sample;
  typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
  for ( unsigned j=0; j<nj; ++j)
    for ( unsigned i=0; i<ni; ++i, ++itr) {
      vector_ sample, var_val;
      const T* iptr = &image(i,j);
      bbgm_planes_to_sample<T, vector_, d_np>::apply(iptr, sample, pstep);
      const T* vptr = &var(i,j);
      bbgm_planes_to_sample<T, vector_, d_np>::apply(vptr, var_val, pstep);
      T temp_val;
      vector_ r_min = sample-delta-var_val, r_max = sample+delta+var_val;
      prop(*itr, r_min, r_max, temp_val);
      result(i,j) = temp_val;
    }
}

//: measure a property (typically probability) requiring a tolerance, delta.
// For probability, delta defines a square n-dimensional box over which
// the probablity density is integrated. As an example, the measure_functor_
// is the bsta_probability_functor. In this method, the covariance of the
// distribtution is augmented by and additional amount
template <class dist_, class measure_functor_>
void measure(bbgm_image_of<dist_>& dimg,
             const vil_image_view<typename dist_::math_type>& image,
             const vbl_array_2d<typename dist_::dist_type::covar_type> & covar,
             vil_image_view<typename dist_::math_type>& result,
             const measure_functor_& prop,
             const typename dist_::math_type delta)
{
  typedef typename dist_::vector_type vector_;
  typedef typename dist_::math_type T;
  typedef typename dist_::dist_type::covar_type covar_t;
  const unsigned ni = dimg.ni();
  const unsigned nj = dimg.nj();
  const unsigned d_np = dist_::dimension;

  assert(image.ni() == ni);
  assert(image.nj() == nj);
  assert(image.nplanes() == d_np);

  result.set_size(ni,nj,1);

  const vcl_ptrdiff_t pstep = image.planestep();

  vector_ sample;
  vector_ del(delta);
  typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
  for ( unsigned j=0; j<nj; ++j)
    for ( unsigned i=0; i<ni; ++i, ++itr) {
      vector_ sample;
      const T* iptr = &image(i,j);
      bbgm_planes_to_sample<T, vector_, d_np>::apply(iptr, sample, pstep);
      T temp_val;
      vector_ r_min = sample-del, r_max = sample+del;
      covar_t add_cov = covar[j][i];
      prop(*itr, r_min, r_max, add_cov, temp_val);
      result(i,j) = temp_val;
    }
}

//: measure a property (typically probability) requiring a tolerance.
// The tolerance is defined by a minumum and maximum values
// For probability, min and max defines a square n-dimensional box over which
// the probablity density is integrated. This functor assumes that the same
// box applies to the entire distribution image. An example is probability of
// area lying within an interval, where the distribution image is a
// 2-d array of area probability densities.
template <class dist_, class measure_functor_>
void measure(bbgm_image_of<dist_>& dimg,
             vil_image_view<typename dist_::math_type>& result,
             const measure_functor_& prop,
             typename dist_::vector_type min_value,
             typename dist_::vector_type max_value)
{
  typedef typename dist_::vector_type vector_;
  typedef typename dist_::math_type T;

  const unsigned ni = dimg.ni();
  const unsigned nj = dimg.nj();

  result.set_size(ni,nj,1);

  typename bbgm_image_of<dist_>::iterator itr = dimg.begin();
  for ( unsigned j=0; j<nj; ++j)
    for ( unsigned i=0; i<ni; ++i, ++itr) {
      T temp_val;
      prop(*itr, min_value, max_value, temp_val);
      result(i,j) = temp_val;
    }
}

#endif // bbgm_measure_h_
