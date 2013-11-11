// This is brcv/seg/dbsta/algo/dbsta_image_stats.h
#ifndef dbsta_image_stats_h_
#define dbsta_image_stats_h_

//:
// \file
// \brief Statistics over images
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 08/18/05
//
// \verbatim
//  Modifications
// \endverbatim

#include <dbsta/dbsta_distribution.h>
#include <dbsta/dbsta_mixture.h>
#include <dbsta/dbsta_updater.h>
#include <vil/vil_image_view.h>

//: A function to update a distribution using all pixels in an image
// The planes of the image are used as the components of the sample vectors
template <class T>
void
dbsta_image_statistics(const vil_image_view<T>& image, 
                       dbsta_distribution<T>& d, 
                       const dbsta_data_updater<T>& updater)
{
  unsigned ni = image.ni(), nj = image.nj(), np = image.nplanes();
  vcl_ptrdiff_t istep=image.istep(), jstep=image.jstep(), pstep = image.planestep();
  vnl_vector<T> sample(np);
  const T* row = image.top_left_ptr();
  for (unsigned j=0;j<nj;++j,row += jstep)
  {
    const T* col = row;
    for (unsigned i=0;i<ni;++i,col+=istep)
    { 
      const T* pixel = col;
      for (unsigned p=0;p<np;++p,pixel += pstep)
        sample[p] = *pixel;
      updater(d, sample);
    }
  }
}


//: A function to update a distribution using all pixels in a masked image
// The planes of the image are used as the components of the sample vectors
// The mask is a single plane boolean image with the same size as the sample image
// The distribution is updated only when the mask pixel is true
template <class T>
void
dbsta_image_statistics(const vil_image_view<T>& image, 
                       const vil_image_view<bool>& mask,
                       dbsta_distribution<T>& d, 
                       const dbsta_data_updater<T>& updater)
{
  unsigned ni = image.ni(), nj = image.nj(), np = image.nplanes();
  assert(mask.ni() == ni);
  assert(mask.nj() == nj);
  assert(mask.nplanes() == 1);
  vcl_ptrdiff_t istep=image.istep(), jstep=image.jstep(), pstep = image.planestep();
  vcl_ptrdiff_t istepM=mask.istep(), jstepM=mask.jstep();
  vnl_vector<T> sample(np);
  const T* row = image.top_left_ptr();
  bool* rowM = mask.top_left_ptr();
  for (unsigned j=0; j<nj; ++j, row+=jstep, rowM+=jstepM)
  {
    const T* col = row;
    const bool* bit = rowM;
    for (unsigned i=0; i<ni; ++i, col+=istep, bit+=istepM)
    { 
      if(*bit){
        const T* pixel = col;
        for (unsigned p=0;p<np;++p,pixel += pstep)
          sample[p] = *pixel;
        updater(d, sample);
      }
    }
  }
}


//: A function to update a mixture distribution using all pixels in a labeled image
// The planes of the image are used as the components of the sample vectors
// The labels are in a single plane byte image with the same size as the sample image
// The indexed distribution is updated only when the label matches its index
template <class T>
void
dbsta_image_statistics(const vil_image_view<T>& image, 
                       const vil_image_view<vxl_byte>& labels,
                       dbsta_mixture<T>& mix, 
                       const dbsta_data_updater<T>& updater)
{
  unsigned ni = image.ni(), nj = image.nj(), np = image.nplanes();
  assert(labels.ni() == ni);
  assert(labels.nj() == nj);
  assert(labels.nplanes() == 1);
  vcl_ptrdiff_t istep=image.istep(), jstep=image.jstep(), pstep = image.planestep();
  vcl_ptrdiff_t istepL=labels.istep(), jstepL=labels.jstep();
  vnl_vector<T> sample(np);
  const T* row = image.top_left_ptr();
  const vxl_byte* rowL = labels.top_left_ptr();
  for (unsigned j=0; j<nj; ++j, row+=jstep, rowL+=jstepL)
  {
    const T* col = row;
    const vxl_byte* lbl = rowL;
    for (unsigned i=0; i<ni; ++i, col+=istep, lbl+=istepL)
    { 
      if(*lbl > 0){
        const T* pixel = col;
        for (unsigned p=0;p<np;++p,pixel += pstep)
          sample[p] = *pixel;
        updater(mix.distribution((*lbl)-1), sample);
        mix.set_weight((*lbl)-1,mix.weight((*lbl)-1)+T(1));
      }
    }
  }
  mix.normalize_weights();
}



#endif // dbsta_image_stats_h_
