// This is brcv/seg/dbbgm/dbbgm_distribution_image.txx
#ifndef dbbgm_distribution_image_txx_
#define dbbgm_distribution_image_txx_
//:
// \file

#include "dbbgm_distribution_image.h"
#include <vcl_typeinfo.h>

//: Constructor
template <class T>
dbbgm_distribution_image<T>::dbbgm_distribution_image( unsigned int ni,
                                                       unsigned int nj,
                                                       const dbsta_distribution<T >& model)
 : data_(ni,nj,NULL)
{
  for(int i=0; i<data_.rows(); ++i){
    for(int j=0; j<data_.cols(); ++j){
      data_(i,j) = model.clone();
    }
  }  
}


//: Destructor
template <class T>
dbbgm_distribution_image<T>::~dbbgm_distribution_image()
{
  /*for(int i=0; i<data_.rows(); ++i){
    for(int j=0; j<data_.cols(); ++j){
      delete data_(i,j);
    }
  }  */
}
 

template <class T>
vcl_string dbbgm_distribution_image<T>::is_a() const 
{
  return "dbbgm_distribution_image_"+vcl_string(typeid(T).name());
}


template <class T>
dbbgm_dist_image_base* dbbgm_distribution_image<T>::clone() const
{
  return new dbbgm_distribution_image<T>(*this);
}

//----------------------------------------------------------------
// ================   Binary I/O Methods ========================
//----------------------------------------------------------------

//: Binary save self to stream.
template <class T>
void dbbgm_distribution_image<T>::b_write(vsl_b_ostream &os) const
{
  vsl_b_write(os, version());
  vsl_b_write(os, data_);
}


//: Binary load self from stream. (not typically used)
template <class T>
void dbbgm_distribution_image<T>::b_read(vsl_b_istream &is)
{
  if (!is)
    return;
  short ver;
  vsl_b_read(is, ver);
  switch (ver)
  {
   case 1:
   vsl_b_read(is, data_);

    break;
   default:
    vcl_cerr << "dbbgm_distribution_image: unknown I/O version " << ver << '\n';
  }
}


//: Return IO version number;
template <class T>
short dbbgm_distribution_image<T>::version() const
{
  return 1;
}


//: Binary save dbbgm_distribution_image to stream.
template <class T>
void
vsl_b_write(vsl_b_ostream &os, const dbbgm_distribution_image<T>* p)
{
  if (p==0) {
    vsl_b_write(os, false); // Indicate null pointer stored
  }
  else{
    vsl_b_write(os,true); // Indicate non-null pointer stored
    p->b_write(os);
  }
}


//: Binary load dbsta_gaussian_indep from stream.
template <class T>
void
vsl_b_read(vsl_b_istream &is, dbbgm_distribution_image<T>* &p)
{
  delete p;
  bool not_null_ptr;
  vsl_b_read(is, not_null_ptr);
  if (not_null_ptr) {
      
    p = new dbbgm_distribution_image<T>();
    p->b_read(is);
  }
  else
    p = 0;
}

#include <vbl/io/vbl_io_array_2d.txx>

#define DBBGM_DISTRIBUTION_IMAGE_INSTANTIATE(T) \
template class dbbgm_distribution_image<T >;\
template void vsl_b_write(vsl_b_ostream &os, const dbbgm_distribution_image<T>* p);\
template void vsl_b_read(vsl_b_istream &is, dbbgm_distribution_image<T>* &p);\
VBL_IO_ARRAY_2D_INSTANTIATE(dbsta_distribution<T> *);\


#endif // dbbgm_distribution_image_txx_
