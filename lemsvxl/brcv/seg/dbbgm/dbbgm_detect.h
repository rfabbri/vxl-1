// This is brcv/seg/dbbgm/dbbgm_detect.h
#ifndef dbbgm_detect_h_
#define dbbgm_detect_h_

//:
// \file
// \brief Detection wrappers in distribution images
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/3/05
//
// \verbatim
//  Modifications
// \endverbatim

#include "dbbgm_distribution_image.h"
#include <vil/vil_image_view.h>
#include <dbsta/dbsta_detector.h>
#include <vnl/vnl_vector_fixed.h>


//: Apply the detector to every pixel
template <class T>
void detect(const dbbgm_distribution_image<T>& dimg,
            const vil_image_view<T>& image,
            const dbsta_detector<T>& detector,
            vil_image_view<bool>& result)
{
  assert(dimg.ni() == image.ni());
  assert(dimg.nj() == image.nj());
  result.set_size(image.ni(),image.nj(),1);

  for(unsigned int i=0; i<dimg.ni(); ++i)
  {
    for(unsigned int j=0; j<dimg.nj(); ++j)
    {
      vnl_vector<T> sample(image.nplanes());
      for(unsigned int k=0; k<sample.size(); ++k)
        sample[k] = image(i,j,k);
      result(i,j) = detector(dimg(i,j), sample);
    }
  }
}


//: Apply the detector to every pixel
template <class T>
void detect_masked(const dbbgm_distribution_image<T>& dimg,
                   const vil_image_view<T>& image,
                   const dbsta_detector<T>& detector,
                   const vil_image_view<bool>& mask,
                   vil_image_view<bool>& result)
{
  assert(dimg.ni() == image.ni());
  assert(dimg.nj() == image.nj());
  result.set_size(image.ni(),image.nj(),1);

  for(unsigned int i=0; i<dimg.ni(); ++i)
  {
    for(unsigned int j=0; j<dimg.nj(); ++j)
    {
      if(mask(i,j)){
        vnl_vector<T> sample(image.nplanes());
        for(unsigned int k=0; k<sample.size(); ++k)
          sample[k] = image(i,j,k);
        result(i,j) = detector(dimg(i,j), sample);
      }
    }
  }
}


template <class T>
void detect(const dbbgm_distribution_image<T>& dimg,
            const vil_image_view<T>& image,
            const dbsta_detector<T>& detector,
            const vnl_vector<T>& intensitythresh,
            vil_image_view<bool>& result)
{
  assert(dimg.ni() == image.ni());
  assert(dimg.nj() == image.nj());
  assert(image.nplanes()==intensitythresh.size());
  result.set_size(image.ni(),image.nj(),1);
  result.fill(false);

  for(unsigned int i=0; i<dimg.ni(); ++i)
  {
    for(unsigned int j=0; j<dimg.nj(); ++j)
    {
      vnl_vector<T> sample(image.nplanes());
      bool flag=true;

      for(unsigned int k=0; k<sample.size(); ++k)
      {
        sample[k] = image(i,j,k);
        if(sample[k]<intensitythresh[k])
          flag=false;
      }
      if(flag )
        result(i,j) = detector(dimg(i,j), sample);
      //else
      //  result(i,j) = detector(dimg(i,j), sample);

    }
  }
}



template <class T>
void detect(const dbbgm_distribution_image<T>& dimg,
            const vil_image_view<T>& image,
            const dbsta_detector<T>& detector,
            const vnl_vector<T>& intensitythresh,
            const vil_image_view<T>& weights,
            const vbl_array_2d<double>& dx,
            const vbl_array_2d<double>& dy,
            T weightthresh,
            vil_image_view<bool>& result)
{
  assert(dimg.ni() == image.ni());
  assert(dimg.nj() == image.nj());
  assert(image.nplanes()==intensitythresh.size());
  result.set_size(image.ni(),image.nj(),1);
  result.fill(false);

  for(unsigned int i=0; i<dimg.ni(); ++i)
  {
    for(unsigned int j=0; j<dimg.nj(); ++j)
    {
      vnl_vector<T> sample(image.nplanes());
      bool flag=true;

      for(unsigned int k=0; k<sample.size(); ++k)
      {
        sample[k] = image(i-dx(j,i),j-dy(j,i),k);
        if(sample[k]<intensitythresh[k])
          flag=false;
      }
      if(flag && weights(i,j)>weightthresh)
        result(i,j) = detector(dimg(i,j), sample);
      //else
      //  result(i,j) = detector(dimg(i,j), sample);

    }
  }
}
//: Apply the detector to every pixel greater than the threshold and allow for a radius of uncertainity
template <class T>
void detect(const dbbgm_distribution_image<T>& dimg,
            const vil_image_view<T>& image, 
            const dbsta_detector<T>& detector,
            vnl_vector<T> intensitythresh,int radius,
            vil_image_view<bool> &result) 
{
    assert(dimg.ni() == image.ni());
    assert(dimg.nj() == image.nj());
    assert(image.nplanes()==intensitythresh.size());
    result.set_size(image.ni(),image.nj(),1);
    result.fill(true);
    int dni = static_cast<int>(dimg.ni()), dnj = static_cast<int>(dimg.nj());
    for(int i=0; i<dni; ++i){
        for(int j=0; j<dnj; ++j){
            vnl_vector<T> sample(image.nplanes());
            bool flag=true;
            for(unsigned int k=0; k<sample.size(); ++k)
            {
                sample[k] = image(i,j,k);
                if(sample[k]<intensitythresh[k])
                    flag=false;
            }
            if(flag ) 
            {
                //result(i,j)=false;
                bool sampleresult=false;
                for(int l=i-radius;l<=i+radius;l++)
                {
                    if(l>=0 && l<dni)
                    {
                        for(int m=j-radius;m<=j+radius;m++)
                            if(m>=0 && m<dnj)
                                sampleresult = sampleresult||( detector(dimg(l,m), sample));
                    }
                }
                result(i,j)=!sampleresult;
            }  
            else
                result(i,j)=false;

        }
    }
}
//: Apply the detector to every pixel greater than the threshold and allow for a radius of uncertainity
template <class T>
void detect_masked(const dbbgm_distribution_image<T>& dimg,
                   const vil_image_view<T>& image, 
                   const dbsta_detector<T>& detector,
                   vnl_vector<T> intensitythresh,int radius,
                   const vil_image_view<bool>& mask,
                   vil_image_view<bool> &result) 
{
    assert(dimg.ni() == (int)image.ni());
    assert(dimg.nj() == (int)image.nj());
    assert(image.nplanes()==intensitythresh.size());
    result.set_size(image.ni(),image.nj(),1);
    result.fill(true);
    int dni = static_cast<int>(dimg.ni()), dnj = static_cast<int>(dimg.nj());
    for(int i=0; i<dni; ++i)
    {
        for(int j=0; j<dnj; ++j)
        {
            
            if(mask(i,j)){
                bool flag=true;
                vnl_vector<T> sample(image.nplanes());

                for(unsigned int k=0; k<sample.size(); ++k)
                {
                    sample[k] = image(i,j,k);
                    if(sample[k]<intensitythresh[k])
                        flag=false;
                }
                 if(flag ) 
                {
                    //result(i,j)=false;
                    bool sampleresult=false;
                    for(int l=i-radius;l<=i+radius;l++)
                    {
                        if(l>=0 && l<dni)
                        {
                            for(int m=j-radius;m<=j+radius;m++)
                                if(m>=0 && m<dnj)
                                    sampleresult = sampleresult||( detector(dimg(l,m), sample));
                        }
                    }
                    result(i,j)=!sampleresult;
                }  
                 else
                    result(i,j)=false;
            }
            else
                    result(i,j)=false;
        }
    }
}

//template <class T>
//void detect_masked(const dbbgm_distribution_image<T>& dimg,
//                   const vil_image_view<T>& image, 
//                   const dbsta_detector<T>& detector,
//                   vnl_vector<T> intensitythresh,int radius,
//                   const vil_image_view<bool>& mask,
//                   vil_image_view<bool> &result) 
//{
//    assert(dimg.ni() == (int)image.ni());
//    assert(dimg.nj() == (int)image.nj());
//    assert(image.nplanes()==intensitythresh.size());
//    result.set_size(image.ni(),image.nj(),1);
//    result.fill(true);
//    int dni = static_cast<int>(dimg.ni()), dnj = static_cast<int>(dimg.nj());
//    for(int i=0; i<dni; ++i)
//    {
//        for(int j=0; j<dnj; ++j)
//        {
//            
//            if(mask(i,j)){
//                bool flag=true;
//                vnl_vector<T> sample(image.nplanes());
//
//                for(unsigned int k=0; k<sample.size(); ++k)
//                {
//                    sample[k] = image(i,j,k);
//                    if(sample[k]<intensitythresh[k])
//                        flag=false;
//                }
//                 if(flag ) 
//                {
//                    //result(i,j)=false;
//                    bool sampleresult=false;
//                    for(int l=i-radius;l<=i+radius;l++)
//                    {
//                        if(l>=0 && l<dni)
//                        {
//                            for(int m=j-radius;m<=j+radius;m++)
//                                if(m>=0 && m<dnj)
//                                    sampleresult = sampleresult||( detector(dimg(l,m), sample));
//                        }
//                    }
//                    result(i,j)=!sampleresult;
//                }  
//                 else
//                    result(i,j)=false;
//            }
//            else
//                    result(i,j)=false;
//        }
//    }
//}

//template <class _dist, class _detector, class rT>
//void detect(dbbgm_image_of<_dist>& dimg,
//            const vil_image_view<typename _dist::math_type>& image,
//            vil_image_view<rT>& result,
//            const _detector& detector,
//            unsigned rad)
//{
//    typedef typename _dist::vector_type _vector;
//    typedef typename _dist::math_type T;
//
//    const unsigned ni = dimg.ni();
//    const unsigned nj = dimg.nj();
//    const unsigned d_np = _dist::dimension;
//    assert(image.ni() == ni);
//    assert(image.nj() == nj);
//    assert(image.nplanes() == d_np);
//
//    result.set_size(ni,nj,1);
//
//    const vcl_ptrdiff_t r_istep = result.istep();
//    const vcl_ptrdiff_t r_jstep = result.jstep();
//    const vcl_ptrdiff_t d_istep = image.istep();
//    const vcl_ptrdiff_t d_jstep = image.jstep();
//    const vcl_ptrdiff_t d_pstep = image.planestep();
//
//    rT temp_val;
//    _vector sample;
//
//    typename dbbgm_image_of<_dist>::iterator itr = dimg.begin();
//    rT* r_row = result.top_left_ptr();
//    const T* d_row = image.top_left_ptr();
//    for (unsigned int j=0; j<nj; ++j, d_row+=d_jstep, r_row+=r_jstep){
//        rT* r_col = r_row;
//        const T* d_col = d_row;
//        for (unsigned int i=0; i<ni; ++i, d_col+=d_istep, r_col+=r_istep, ++itr){
//            bool flag=false;
//            for(unsigned i=-rad;i<=rad;i++)
//                for(unsigned j=-rad;j<=rad;j++)
//                {
//                      const T* d_plane = d_col+j*d_jstep+i*d_istep;
//                      if(d_plane)
//                      {
//                         dbbgm_planes_to_sample<T,_vector,_dist::dimension>::apply(d_plane,sample,d_pstep);
//                         if(detector(*itr, sample, temp_val))
//                             flag=true;
//                             
//
//                      }
//                }
//            
//                const T* d_plane = d_col+j*d_jstep+i*d_istep;
//                dbbgm_planes_to_sample<T,_vector,_dist::dimension>::apply(d_plane,sample,d_pstep);
//                if(detector(*itr, sample, temp_val))
//                    *r_col =temp_val;
//            
//      
//    }
//  }
//}

#endif // dbbgm_detect_h_
