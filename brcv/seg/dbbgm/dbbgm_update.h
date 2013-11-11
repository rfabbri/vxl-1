// This is brcv/seg/dbbgm/dbbgm_update.h
#ifndef dbbgm_update_h_
#define dbbgm_update_h_

//:
// \file
// \brief Update wrappers for distribution images
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 10/3/05
//
// \verbatim
//  Modifications
// \endverbatim


#include "dbbgm_distribution_image.h"
#include <vil/vil_image_view.h>
#include <dbsta/dbsta_updater.h>

#include <vcl_cassert.h>
#include <dbsta/algo/dbsta_adaptive_updater.h>




//: Update with no data
template <class T>
void update(dbbgm_distribution_image<T>& dimg,
            const dbsta_updater<T>& updater)
{
  typedef typename dbbgm_distribution_image<T>::iterator d_itr;
  for(d_itr itr = dimg.begin(); itr != dimg.end(); ++itr)
    updater(*itr);
}


//: Update with no data only where mask(i,j) 
template <class T>
void update_masked(dbbgm_distribution_image<T>& dimg,
                   const dbsta_updater<T>& updater,
                   const vil_image_view<bool>& mask)
{
  assert(dimg.ni() == mask.ni());
  assert(dimg.nj() == mask.nj());

  const unsigned ni = mask.ni();
  const unsigned nj = mask.nj();
  const unsigned nplanes = mask.nplanes();

  const vcl_ptrdiff_t planestep = mask.planestep();
  const vcl_ptrdiff_t istep = mask.istep();
  const vcl_ptrdiff_t jstep = mask.jstep();

  typename dbbgm_distribution_image<T>::iterator itr = dimg.begin();
  const bool* col = mask.top_left_ptr();
  for (unsigned int i=0; i<ni; ++i, col+=istep){
    const bool* row = col;
    for (unsigned int j=0; j<nj; ++j, row+=jstep, ++itr){
      if(*row)
        updater(*itr);
    }
  }
}


//: Update with a new sample image
template <class T>
void update(dbbgm_distribution_image<T>& dimg,
            const vil_image_view<T>& image,
            const dbsta_data_updater<T>& updater)
{
  assert(dimg.ni() == image.ni());
  assert(dimg.nj() == image.nj());

  const unsigned ni = image.ni();
  const unsigned nj = image.nj();
  const unsigned nplanes = image.nplanes();

  const vcl_ptrdiff_t planestep = image.planestep();
  const vcl_ptrdiff_t istep = image.istep();
  const vcl_ptrdiff_t jstep = image.jstep();

  typename dbbgm_distribution_image<T>::iterator itr = dimg.begin();
  const T* col = image.top_left_ptr();
  for (unsigned int i=0; i<ni; ++i, col+=istep){
    const T* row = col;
    for (unsigned int j=0; j<nj; ++j, row+=jstep, ++itr){
      const T* data = row;
      vnl_vector<T> sample(nplanes);
      typename vnl_vector<T>::iterator s_itr = sample.begin();
      for (unsigned int p=0; p<nplanes; ++p, data += planestep, ++s_itr){
        *s_itr = *data;
      }
      updater(*itr,sample);
    }
  }
}


//: Update with a new sample image only where mask(i,j) 
template <class T>
void update_masked(dbbgm_distribution_image<T>& dimg,
                   const vil_image_view<T>& image,
                   const dbsta_data_updater<T>& updater,
                   const vil_image_view<bool>& mask)
{
  assert(dimg.ni() == image.ni());
  assert(dimg.nj() == image.nj());
  assert(dimg.ni() == mask.ni());
  assert(dimg.nj() == mask.nj());

  const unsigned ni = image.ni();
  const unsigned nj = image.nj();
  const unsigned nplanes = image.nplanes();

  const vcl_ptrdiff_t planestep = image.planestep();
  const vcl_ptrdiff_t istep = image.istep();
  const vcl_ptrdiff_t jstep = image.jstep();
  const vcl_ptrdiff_t m_istep = mask.istep();
  const vcl_ptrdiff_t m_jstep = mask.jstep();

  typename dbbgm_distribution_image<T>::iterator itr = dimg.begin();
  const bool* m_col = mask.top_left_ptr();
  const T* col = image.top_left_ptr();
  for (unsigned int i=0; i<ni; ++i, col+=istep, m_col+=m_istep){
    const T* row = col;
    const bool* m_row = m_col;
    for (unsigned int j=0; j<nj; ++j, row+=jstep, m_row+=m_jstep, ++itr){
      if(*m_row){
        const T* data = row;
        vnl_vector<T> sample(nplanes);
        typename vnl_vector<T>::iterator s_itr = sample.begin();
        for (unsigned int p=0; p<nplanes; ++p, data += planestep, ++s_itr){
          *s_itr = *data;
        }
        updater(*itr,sample);
      }
    }
  }
}


template <class T>
void update(dbbgm_distribution_image<T>& dimg,
            const vil_image_view<T>& image,
            const dbsta_data_updater<T>& updater,
            T intensitythresh)
{
  assert(dimg.ni() == image.ni());
  assert(dimg.nj() == image.nj());
  for(unsigned int i=0; i<dimg.ni(); ++i)
  {
    for(unsigned int j=0; j<dimg.nj(); ++j)
    {
      vnl_vector<T> sample(image.nplanes());
      bool flag=false;

      for(unsigned int k=0; k<sample.size(); ++k)
      {
        sample[k] = image(i,j,k);
        if(sample[k]>intensitythresh)
          flag=true;
      }
      if(flag || (i==0 && j==0))
        updater(dimg(i,j), sample);
    }
  }
}


template <class T>
void update(dbbgm_distribution_image<T>& dimg,
            const vil_image_view<T>& image,
            const dbsta_data_updater<T>& updater,
            T intensitythresh,
            const vil_image_view<T>& weights,
            const vbl_array_2d<double>& dx,
            const vbl_array_2d<double>& dy)
{
  assert(dimg.ni() == image.ni());
  assert(dimg.nj() == image.nj());

  for(unsigned int i=0; i<dimg.ni(); ++i)
  {
    for(unsigned int j=0; j<dimg.nj(); ++j)
    {
      vnl_vector<T> sample(image.nplanes());
      bool flag=false;

      for(unsigned int k=0; k<sample.size(); ++k)
      {
        sample[k] = image(i-dx(j,i),j-dy(j,i),k);
        if(sample[k]>intensitythresh)
          flag=true;
      }
      if(flag || (i==0 && j==0))
      {
        if( dbsta_mg_nn_statistical_updater<T> *nnupdater = dynamic_cast< dbsta_mg_nn_statistical_updater<T> *>(&updater))
        {
          nnupdater->setweight(weights(i,j));
          (*nnupdater)(dimg(i,j), sample);
        }
        else
        {
          updater(dimg(i,j), sample);
        }
      }
    }
  }
}



#endif // dbbgm_update_h_
