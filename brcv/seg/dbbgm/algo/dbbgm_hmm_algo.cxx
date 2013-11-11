// This is brcv/seg/dbbgm/algo/dbbgm_hmm_algo.cxx
//:
// \file


#include "dbbgm_hmm_algo.h"
#include <vcl_vector.h>
#include <vcl_limits.h>
#include <vil/vil_plane.h>
#include <vil/vil_crop.h>
#include <vil/algo/vil_correlate_2d.h>

//: Apply transistion probabilities to the image of probablities
void dbbgm_update_probs(const vil_image_view<float>& probs,
                        const vil_image_view<float>& trans,
                        vil_image_view<float>& next_probs)
{
  const unsigned int ni = probs.ni();
  const unsigned int nj = probs.nj();
  const unsigned int np = probs.nplanes();

  assert(ni == trans.ni());
  assert(nj == trans.nj());
  assert(np*np == trans.nplanes());

  next_probs.set_size(ni,nj,np);

  for(unsigned int j=0; j<nj; ++j){
    for(unsigned int i=0; i<ni; ++i){
      for(unsigned int m1=0; m1<np; ++m1){
        float t = 0.0f;
        for(unsigned int m2=0; m2<np; ++m2)
          t += trans(i,j,m2*3+m1)*probs(i,j,m2);
        next_probs(i,j,m1) = t;
      }
    }
  }
}


//: Normalize a probability field image
void dbbgm_normalize_probs(vil_image_view<float>& probs)
{
  const unsigned int ni = probs.ni();
  const unsigned int nj = probs.nj();
  const unsigned int np = probs.nplanes();

  for(unsigned int j=0; j<nj; ++j){
    for(unsigned int i=0; i<ni; ++i){
      double sum = 0.0;
      for(unsigned int p=0; p<np; ++p){
        sum += probs(i,j,p);
      }
      if(sum > vcl_numeric_limits<double>::epsilon()){
        for(unsigned int p=0; p<np; ++p){
          probs(i,j,p) /= sum;
        }
      }
      else{
        for(unsigned int p=0; p<np; ++p){
          probs(i,j,p) = 1.0/np;
        }
      }
    }
  }
}


//: Apply a mean field approximation to adding a MRF to the probabilities
void dbbgm_mean_field(vil_image_view<float>& probs,
                      unsigned int k_rad, float alpha)
{
  const unsigned int ni = probs.ni();
  const unsigned int nj = probs.nj();
  const unsigned int nc = probs.nplanes();

  vil_image_view<float> qb(ni+2*k_rad,nj+2*k_rad,nc), V(ni,nj,nc);
  qb.fill(0.0f);
  vil_image_view<float> q = vil_crop(qb,k_rad,ni,k_rad,nj);
  q.deep_copy(probs);

  // build kernel
  const unsigned int size_k = (2*k_rad+1)*(2*k_rad+1)-1;
  vil_image_view<float> k(k_rad*2+1,k_rad*2+1);
  for(int j=-k_rad; j<=(int)k_rad; ++j)
    for(int i=-k_rad; i<=(int)k_rad; ++i)
      k(i+k_rad,j+k_rad) = alpha/(i*i+j*j)/size_k;
  k(k_rad,k_rad) = 0.0f;


  float sqr_diff=0.0f;
  for(unsigned int itr=0; itr<10; ++itr){
    for(unsigned int c=0; c<nc; ++c){
      vil_image_view<float> qc = vil_plane(qb,c);
      vil_image_view<float> Vc = vil_plane(V,c);
      vil_correlate_2d(qc,Vc,k,double());
    }

    float sum = 0.0f, Vsum = 0.0f;
    vcl_vector<float> diff(nc);
    sqr_diff=0.0f;
    for(unsigned int j=0; j<nj; ++j){
      for(unsigned int i=0; i<ni; ++i){
        Vsum = 0.0f;
        for(unsigned int c=0; c<nc; ++c)
          Vsum += V(i,j,c);
        sum = 0.0f;
        for(unsigned int c=0; c<nc; ++c){
          diff[c] = q(i,j,c);
          q(i,j,c) = probs(i,j,c)*vcl_exp(-Vsum + V(i,j,c));
          sum += q(i,j,c);
        }
        for(unsigned int c=0; c<nc; ++c){
          q(i,j,c) /= sum;
          diff[c] -= q(i,j,c);
          sqr_diff += diff[c]*diff[c];
        }
      }
    }
    vcl_cout << itr << ": sqr_diff = "<< sqr_diff <<vcl_endl;
    if(sqr_diff<1e-6)
      break;
  }
  probs.deep_copy(q);
}
