// This is brcv/seg/dbbgm/dbbgm_apply.h
#ifndef dbbgm_apply_h_
#define dbbgm_apply_h_

//:
// \file
// \brief Apply functors to distribution images
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/3/05
//
// \verbatim
//  Modifications
// \endverbatim


#include "dbbgm_distribution_image.h"
#include <vil/vil_image_view.h>
#include <dbsta/dbsta_functor.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_vector.h>



//: Apply the functor at every pixel
//  \returns an image of results, each vector component in a separate plane
//  \param fail_val sets the value of pixels where the functor fails
template <class T>
void apply(const dbbgm_distribution_image<T>& dimg,
           const dbsta_functor<T>& functor,
           vil_image_view<T>& result,
           vnl_vector<T> fail_val = vnl_vector<T>(1,T(0)) )
{
  unsigned int np = functor.return_size();
  if(result)
    np = result.nplanes();
  // if return size is unknown (i.e. zero) use the dimension
  if(np == 0){
    // find the first pixel with a valid dimension
    for(unsigned int i=0; np==0 && i<dimg.ni(); ++i)
      for(unsigned int j=0; np==0 && j<dimg.nj(); ++j)
        np = dimg(i,j).dim();
  }

  // make an image of failure values and return
  if(np == 0){
    result.set_size(dimg.ni(), dimg.nj(), fail_val.size());
    for(unsigned int i=0; np==0 && i<dimg.ni(); ++i)
      for(unsigned int j=0; np==0 && j<dimg.nj(); ++j)
        for(unsigned int k=0; k<fail_val.size(); ++k)
          result(i,j,k) = fail_val[k];
    return;
  }

  result.set_size(dimg.ni(), dimg.nj(), np);
  assert(fail_val.size() > 0);
  if(fail_val.size() != np)
    fail_val = vnl_vector<T>(np,fail_val[0]);

  for(unsigned int i=0; i<dimg.ni(); ++i)
  {
    for(unsigned int j=0; j<dimg.nj(); ++j)
    {
      vnl_vector<T> v = functor(dimg(i,j));
      // special case of functor failure
      if(v.size() == 0)
        v = fail_val;
      assert(v.size() == np);
      for(unsigned int k=0; k<np; ++k)
        result(i,j,k) = v[k];
    }
  }
}


//: Apply the data functor at every pixel with an image of data
//  \returns an image of results, each vector component in a separate plane
//  \param fail_val sets the value of pixels where the functor fails
//  The corresponding pixel in \p image is used for data
template <class T>
void apply(const dbbgm_distribution_image<T>& dimg,
           const dbsta_data_functor<T>& functor,
           const vil_image_view<T>& image,
           vil_image_view<T>& result,
           vnl_vector<T> fail_val = vnl_vector<T>(1,T(0)))
{
  assert(dimg.ni() == image.ni());
  assert(dimg.nj() == image.nj());
  unsigned int np = functor.return_size();
  // if return size is unknown (i.e. zero) use the input dimension
  if(np == 0) np = image.nplanes();
  result.set_size(dimg.ni(), dimg.nj(), np);
  assert(fail_val.size() > 0);
  if(fail_val.size() != np)
    fail_val = vnl_vector<T>(np,fail_val[0]);

  for(unsigned int i=0; i<dimg.ni(); ++i)
  {
    for(unsigned int j=0; j<dimg.nj(); ++j)
    {
      vnl_vector<T> sample(image.nplanes());
      for(unsigned int k=0; k<sample.size(); ++k)
        sample[k] = image(i,j,k);
      vnl_vector<T> v = functor(dimg(i,j),sample);
      // special case of functor failure
      if(v.size() == 0)
        v = fail_val;
      assert(v.size() == np);
      for(unsigned int k=0; k<np; ++k)
        result(i,j,k) = v[k];
    }
  }
}


//: Apply the data functor at every pixel with fixed data
//  \returns an image of results, each vector component in a separate plane
//  \param fail_val sets the value of pixels where the functor fails
//  The vector \p data is applied to all pixels
template <class T>
void apply(const dbbgm_distribution_image<T>& dimg,
           const dbsta_data_functor<T>& functor,
           const vnl_vector<T>& data,
           vil_image_view<T>& result,
           vnl_vector<T> fail_val = vnl_vector<T>(1,T(0)))
{
  unsigned int np = functor.return_size();
  // if return size is unknown (i.e. zero) use the input dimension
  if(np == 0) np = data.size();
  result.set_size(dimg.ni(), dimg.nj(), np);
  assert(fail_val.size() > 0);
  if(fail_val.size() != np)
    fail_val = vnl_vector<T>(np,fail_val[0]);

  for(unsigned int i=0; i<dimg.ni(); ++i)
  {
    for(unsigned int j=0; j<dimg.nj(); ++j)
    {
      vnl_vector<T> v = functor(dimg(i,j), data);
      // special case of functor failure
      if(v.size() == 0)
        v = fail_val;
      assert(v.size() == np);
      for(unsigned int k=0; k<np; ++k)
        result(i,j,k) = v[k];
    }
  }
}




#endif // dbbgm_apply_h_
